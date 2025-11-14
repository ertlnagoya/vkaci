import func

# フレーム切り替えを基準としてFPS制御
def _on_Swap_called(f: func.Func) -> func.Hook:
    return func.Hook(
        is_target=f.name == "vkQueuePresentKHR",
        before_run="""onSwapCompleted();
  """,
    )

def entry(f: func.Func) -> str:
    return (
        # f'DTRACE_PROBE(vkapi, {f.name}_entry);'
        f'//printf("{f.name}_entry\\n");'
    )

def exit(f: func.Func) -> str:
    return (
        f'//printf("{f.name}_exit\\n");'
        # f'DTRACE_PROBE(vkapi, {f.name}_exit);'
    )


vkaci_hooks = func.Hooks(
    header='#include "throttling_frame_joined.cpp"',
    hook_funcs=[
        _on_Swap_called,
        lambda f: func.Hook(
            is_target=True,
            before_run=entry(f),
            after_run=exit(f),
        ),
    ],
)
