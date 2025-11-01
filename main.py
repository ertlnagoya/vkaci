import hooks.example_trace
from reg import reg
import func
import hooks

VK_REGISTRY = reg.Registry.from_file("./reg/vk.xml")

# Change the hooks to the ones you want to apply.
HOOKS = hooks.example_trace.vkaci_hooks

GET_PROC_FUNCS = [
    "vkGetDeviceProcAddr",
    "vkGetInstanceProcAddr",
]

REQUIRED_FUNCS = [
    "vkCreateInstance",
    "vkDestroyInstance",
    "vkCreateDevice",
    "vkDestroyDevice",
]

def main() -> None:
    # def filter_by_api(api: str, version: float) -> bool:
    #     b = (api == "gl" and version <= 3.2) or (api == "glx") or (api == "egl")
    #     return b

    # Get all functions
    funcs = (
        func.no_filter_funcs(VK_REGISTRY)
    )

    # Apply hooks to the functions and save hooked functions and their definitions.
    get_proc_funcs = [funcs[name] for name in GET_PROC_FUNCS if name in funcs]
    funcs = {name: f for name, f in funcs.items() if name not in GET_PROC_FUNCS and name not in REQUIRED_FUNCS}
    hooked_funcs: list[func.Func] = []
    hooked_func_defs: list[str] = []
    for fn in funcs.values():
        applied = HOOKS.apply(fn)
        if applied is None:
            continue
        hooked_funcs.append(fn)
        hooked_func_defs.append(applied)

    # Print headers
    print(func.header())
    print(HOOKS.header)
    print()

    # Print global something
    print(func.global_things())

    # Print required functions.
    print(func.define_required_func(hooked_funcs))

    # Print the definitions of the hooked functions.
    for defn in hooked_func_defs:
        print(defn)
        print()

    # Print the definitions of the get_proc_addr functions.
    print(func.define_get_proc_addr_func_device(get_proc_funcs[0], hooked_funcs))
    print()
    print(func.define_get_proc_addr_func_instance(get_proc_funcs[1], hooked_funcs))
    print()

if __name__ == "__main__":
    main()
