import hooks.example_trace
from reg import reg
import func
import hooks

VK_REGGITRY = reg.Registry.from_file("./reg/vk.xml")

# Change the hooks to the ones you want to apply.
HOOKS = hooks.example_trace.dump_time_hooks

# GET_PROC_FUNCS = [
#     "vkGetInstanceProcAddr",
#     "vkGetDeviceProcAddr",
# ]


def main() -> None:
    # def filter_by_api(api: str, version: float) -> bool:
    #     b = (api == "gl" and version <= 3.2) or (api == "glx") or (api == "egl")
    #     return b

    # Get all functions
    funcs = (
        func.no_filter_funcs(VK_REGGITRY)
    )

    # Apply hooks to the functions and save hooked functions and their definitions.
    # get_proc_funcs = [funcs[name] for name in GET_PROC_FUNCS if name in funcs]
    # funcs = {name: f for name, f in funcs.items() if name not in GET_PROC_FUNCS}
    funcs = {name: f for name, f in funcs.items()}
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
    print()
    print(HOOKS.header)
    print()

    # Print declarations of the original functions and initializer of the function pointers.
    # print(func.setup_original_funcs(hooked_funcs + get_proc_funcs))
    print(func.setup_original_funcs(hooked_funcs))
    print()

    # Print the definitions of the hooked functions.
    for defn in hooked_func_defs:
        print(defn)
        print()

    # Print the definitions of the get_proc_addr functions.
    # for f in get_proc_funcs:
    #     print(func.define_get_proc_addr_func(f, hooked_funcs))
    #     print()

    # Print the definitions of the dlsym function.
    # print(func.define_dlsym(hooked_funcs + get_proc_funcs))
    print(func.define_dlsym(hooked_funcs))


if __name__ == "__main__":
    main()
