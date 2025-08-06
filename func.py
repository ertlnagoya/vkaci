from reg import reg
from dataclasses import dataclass
from typing import Callable, Optional


@dataclass
class Func:
    @dataclass
    class Param:
        ptype: str | None
        name: str
        content: str

    name: str
    proto: str
    ret_type: str | None
    params: list[Param]

    @staticmethod
    def from_command(cmd: reg.Command) -> "Func":
        return Func(
            cmd.proto.name,
            cmd.proto.content,
            cmd.proto.ptype,
            [
                Func.Param(
                    p.ptype,
                    p.name,
                    p.content,
                )
                for p in cmd.params
            ],
        )

    def to_c_call(self) -> str:
        "Return a string that represents a C function call. e.g. `foo(bar, baz)`"
        args_str = ", ".join([p.name for p in self.params])
        return f"{self.name}({args_str})"

    def to_original_c_call(self) -> str:
        "Return a string that represents a C function call to the original function. e.g. `original_foo(bar, baz)`"
        args_str = ", ".join([p.name for p in self.params])
        return f"(*original_{self.name})({args_str})"

    def defined_type(self) -> str:
        return f"PFN{self.name.upper()}PROC"

    def define(
        self,
        beforeRun: str | None = None,
        afterRun: str | None = None,
    ) -> str:
        res = []
        res.append('extern "C" PUBLIC')
        res.append(
            f"{self.proto}({', '.join([p.content for p in self.params])})" + " {"
        )
        if beforeRun:
            res.append("    " + beforeRun)
        if self.ret_type:
            res.append(f"    auto _result = {self.to_original_c_call()};")
        else:
            res.append(f"    {self.to_original_c_call()};")
        if afterRun:
            res.append("    " + afterRun)
        if self.ret_type:
            res.append("    return _result;")
        res.append("}")
        return "\n".join(res)


@dataclass
class Hook:
    is_target: bool = False
    before_run: str = ""
    after_run: str = ""


HookFunc = Callable[[Func], Hook]


@dataclass
class Hooks:
    header: str
    hook_funcs: list[HookFunc]

    def apply(self, func: Func) -> Optional[str]:
        """
        Apply hooks to the function if it is a target.
        If the function is not a target, return None.
        Before is FIFO, after is LIFO.
        Ex. before[0] -> before[1] -> before[2] -> original -> after[2] -> after[1] -> after[0].
        """

        befores, afters = [], []
        is_target = False

        for hook_func in self.hook_funcs:
            hook = hook_func(func)
            if not hook.is_target:
                continue
            is_target = True
            befores.append(hook.before_run)
            afters.append(hook.after_run)

        if not is_target:
            return None

        return func.define("\n\n".join(befores), "\n\n".join(reversed(afters)))


def header() -> str:
    return '#include "header.hpp"'


def filter_funcs(
    registry: reg.Registry,
    filter: Callable[[str, float], bool] = lambda api, version: True,
) -> dict[str, Func]:
    required_cmds: set[str] = set()
    removed_cmds: set[str] = set()
    for feature in registry.features:
        if filter(feature.api, float(feature.number)):
            for req in feature.require:
                required_cmds.update(
                    [elm.name for elm in req.elements if elm.type_ == "command"]
                )
            for rem in feature.remove:
                removed_cmds.update(
                    [elm.name for elm in rem.elements if elm.type_ == "command"]
                )
    target_cmds = required_cmds - removed_cmds

    funcs: dict[str, Func] = {}
    for cmds in registry.commands:
        for cmd in cmds.commands:
            if cmd.proto.name in target_cmds:
                funcs[cmd.proto.name] = Func.from_command(cmd)

    return funcs


# バージョンなどを気にせず全ての関数を返す
def no_filter_funcs(
    registry: reg.Registry,
) -> dict[str, Func]:
    required_cmds: set[str] = set()
    removed_cmds: set[str] = set()
    for feature in registry.features:
        if feature.api != "vulkansc":   #VulkanSCにしか含まれないものは一旦除外
            for req in feature.require:
                required_cmds.update(
                    [elm.name for elm in req.elements if elm.type_ == "command" and elm.name != "vkGetInstanceProcAddr" and elm.name != "vkGetDeviceProcAddr"]
                )
            for rem in feature.remove:
                removed_cmds.update(
                    [elm.name for elm in rem.elements if elm.type_ == "command"]
                )
    # for extention in registry.extensions:
    #     if extention.promotedto != "VK_VERSION_1_4":
    #         for req in extention.require:
    #             required_cmds.update(
    #                 [elm.name for elm in req.elements if elm.type_ == "command"]
    #             )
    #         for rem in extention.remove:
    #                 removed_cmds.update(
    #                     [elm.name for elm in rem.elements if elm.type_ == "command"]
    #                     )
    for extention in registry.extensions:
        for req in extention.require:
            required_cmds.update(
                [elm.name for elm in req.elements if elm.name == "vkQueuePresentKHR"]
            )
    target_cmds = required_cmds - removed_cmds

    funcs: dict[str, Func] = {}
    for cmds in registry.commands:
        for cmd in cmds.commands:
            if cmd.proto.name in target_cmds:
                funcs[cmd.proto.name] = Func.from_command(cmd)

    return funcs


def define_get_proc_addr_func(get_proc_func: Func, funcs: list[Func]):
    res: list[str] = []
    res.extend(
        [
            'extern "C" PUBLIC',
            f"{get_proc_func.proto}({', '.join([p.content for p in get_proc_func.params])})",
            "{",
            f'    printf("{get_proc_func.name}_entry\\n");',
            f"    auto procPtr = {get_proc_func.to_original_c_call()};",
        ]
    )

    for fn in funcs:
        res.extend(
            [
                f'    if (strcmp("{fn.name}", (const char *){get_proc_func.params[0].name}) == 0)',
                "    {",
                f"        original_{fn.name} = reinterpret_cast<decltype(original_{fn.name})>(procPtr);",
                f"        return ({get_proc_func.ret_type})&{fn.name};",
                "    }",
            ]
        )

    res.extend(
        [
            f'    printf("{get_proc_func.name}_exit\\n");',
            "    return procPtr;",
            "}",
            "",
        ]
    )
    return "\n".join(res)


def setup_original_funcs(funcs: list[Func]) -> str:
    res: list[str] = [
        f"static typeof({func.name}) *original_{func.name};" for func in funcs
    ]
    res.append("")
    res.append("PRIVATE INITIALIZER")
    res.append("void load_original_functions()")
    res.append("{")
    res.append("    auto flag = RTLD_NEXT;")
    for func in funcs:
        res.append(
            f'    original_{func.name} = reinterpret_cast<decltype(original_{func.name})>(original_dlsym(flag, "{func.name}"));',
        )
    res.append("}")
    return "\n".join(res)


def define_dlsym(funcs: list[Func]) -> str:
    res = []
    res.append('extern "C" PUBLIC')
    res.append("void *dlsym(void *handle, const char *symbol)")
    res.append("{")
    res.append("    auto ptr = original_dlsym(handle, symbol);")
    for func in funcs:
        res.extend(
            [
                f'    if (strcmp("{func.name}", symbol) == 0)',
                "    {",
                f"        original_{func.name} = reinterpret_cast<decltype(original_{func.name})>(ptr);",
                f"        return reinterpret_cast<void *>({func.name});",
                "    }",
            ]
        )
    res.append("    return ptr;")
    res.append("}")
    return "\n".join(res)
