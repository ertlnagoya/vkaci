import func


def _trace(f: func.Func) -> str:
    return f'std::cerr << "{f.name} called" << std::endl;'

# フレーム切り替えのタイミング表示
def _on_Swap_called(f: func.Func) -> func.Hook:
    return func.Hook(
        is_target=f.name == "vkQueuePresentKHR",
        after_run="""
    onSwapCompleted();""",
    )

trace_hooks = func.Hooks(
    header="#include <iostream>",
    hook_funcs=[
        lambda f: func.Hook(
            is_target=True,
            before_run=_trace(f),
        ),
    ],
)


def _dump_time(f: func.Func) -> str:
    return (
        "auto now = std::chrono::high_resolution_clock::now().time_since_epoch();"
        f'std::cerr << std::chrono::duration_cast<std::chrono::nanoseconds>(now).count() << " ({f.name})" << std::endl;'
    )

def _begin_measure(f: func.Func) -> str:
    return (
        f'DTRACE_PROBE(vkapi, {f.name}_entry);'
        # f'printf("{f.name}_entry\\n");'
    )

def _end_measure(f: func.Func) -> str:
    return (
        # f'printf("{f.name}_exit\\n");'
        f'DTRACE_PROBE(vkapi, {f.name}_exit);'
    )


dump_time_hooks = func.Hooks(
    header='#include <iostream>\n#include <chrono>\n#include <sys/sdt.h>\n#include "throttling_frame_joined.cpp"',
    hook_funcs=[
        _on_Swap_called,
        lambda f: func.Hook(
            is_target=True,
            before_run=_begin_measure(f),
            after_run=_end_measure(f),
            #before_run=_dump_time(f),
        ),
    ],
)
