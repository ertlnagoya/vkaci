#!/usr/bin/python3
from bcc import BPF, USDT
import sys

binary_to_usdt_dict = {}
binary_to_probes_dict = {}
probe_to_binary_dict = {}
fps_limit = 30
pid_app_high = None
pid_app_low = None

def add_monitored_process(pid: int):
    path = str(pid)
    assert path not in binary_to_usdt_dict
    usdt = USDT(pid=pid)
    binary_to_usdt_dict[path] = usdt

def create_bpf_for_monitoring():
    usdt_contexts = []
    bpf_text = f"""
        #define TARGET_FRAME_TIME_US ({1000*1000//fps_limit}U)

        struct log_entry {{
            u32  pid;
            u32  fps;
            char message[128];
        }};
        BPF_PERF_OUTPUT(log_entries);

        int test_on_fps_update_limiter(struct pt_regs *ctx) {{
            uint32_t fps;
            void *p_frame_tims_us;
            uint32_t frame_tims_us = TARGET_FRAME_TIME_US;
            bpf_usdt_readarg(1, ctx, &fps);
            bpf_usdt_readarg(2, ctx, &p_frame_tims_us);
            bpf_probe_write_user(p_frame_tims_us, &frame_tims_us, sizeof(frame_tims_us));
            struct log_entry entry = {{ .message = "test_on_fps_update_limiter", .pid = bpf_get_current_pid_tgid() }};
            entry.fps = fps;
            log_entries.perf_submit(ctx, &entry, sizeof(entry));
            return 0;
        }}

    """

    usdt = binary_to_usdt_dict[str(pid_app_high)]
    usdt.enable_probe(probe='vkaci:on_fps_update', fn_name='test_on_fps_update_limiter')
    usdt_contexts.append(usdt)

    #usdt = binary_to_usdt_dict[str(pid_app_low)]
    #usdt.enable_probe(probe='glaci:on_fps_update', fn_name='test_on_fps_update')
    #usdt_contexts.append(usdt)
    #print(usdt_contexts)

    #print(bpf_text)
    return BPF(text=bpf_text, usdt_contexts=usdt_contexts)

def main(args) -> None:
    global fps_limit
    global pid_app_high

    process_pid = args[0]
    fps_limit = int(args[1])

    add_monitored_process(int(process_pid))
    pid_app_high = process_pid

    bpf = create_bpf_for_monitoring()

    print("BCC Log:", flush=True)

    # process event
    def print_event(cpu, data, size):
        event = bpf['log_entries'].event(data)
        print(event.message, event.pid, event.fps)
        with open("limiter.txt", "a", encoding="utf-8") as f:
            f.write(f"{event.message} {event.pid} {event.fps}\n")

    # loop with callback to print_event
    bpf["log_entries"].open_perf_buffer(print_event)
    while 1:
        try:
            bpf.perf_buffer_poll()
        except KeyboardInterrupt:
            exit()

if __name__ == "__main__":
    main(sys.argv[1:])