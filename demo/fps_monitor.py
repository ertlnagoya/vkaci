#!/usr/bin/python3
from bcc import BPF, USDT
import sys

from multiprocessing import shared_memory
import struct

binary_to_usdt_dict = {}
binary_to_probes_dict = {}
probe_to_binary_dict = {}
fps_limit = 30
pid_app_high = None

fps_get_interval = 10

def add_monitored_process(pid: int):
    path = str(pid)
    assert path not in binary_to_usdt_dict
    usdt = USDT(pid=pid)
    binary_to_usdt_dict[path] = usdt

def create_bpf_for_monitoring():
    usdt_contexts = []
    bpf_text = f"""
        #define FPS_GET_INTERVAL ({fps_get_interval}U)

        struct log_entry {{
            u32  pid;
            u32  fps;
            char message[128];
        }};
        BPF_PERF_OUTPUT(log_entries);

        int test_on_fps_get(struct pt_regs *ctx) {{
            uint32_t fps = 0;
            void *p_fps_get_interval;
            uint32_t fps_get_interval = FPS_GET_INTERVAL;

            bpf_usdt_readarg(1, ctx, &fps);
            bpf_usdt_readarg(2, ctx, &p_fps_get_interval);

            bpf_probe_write_user(p_fps_get_interval, &fps_get_interval, sizeof(fps_get_interval));
            
            struct log_entry entry = {{ .message = "on_fps_get", .pid = bpf_get_current_pid_tgid() }};
            entry.fps = fps;
            log_entries.perf_submit(ctx, &entry, sizeof(entry));
            return 0;
        }}
    """

    usdt = binary_to_usdt_dict[str(pid_app_high)]
    usdt.enable_probe(probe='vkaci:on_fps_get', fn_name='test_on_fps_get')
    usdt_contexts.append(usdt)

    return BPF(text=bpf_text, usdt_contexts=usdt_contexts)

def main(args) -> None:
    global fps_limit, pid_app_high, fps_get_interval

    process_pid = args[0]
    fps_get_interval = int(args[1])

    add_monitored_process(int(process_pid))
    pid_app_high = process_pid

    bpf = create_bpf_for_monitoring()

    shm = shared_memory.SharedMemory(name="fps_shm")

    # process event
    def print_event(cpu, data, size):
        event = bpf['log_entries'].event(data)
        # print(event.message, event.pid, event.fps)
        with open("monitor.txt", "a", encoding="utf-8") as f:
            f.write(f"{event.fps}\n")
        shm.buf[:4] = struct.pack("i", event.fps)

    # loop with callback to print_event
    bpf["log_entries"].open_perf_buffer(print_event)
    while 1:
        try:
            bpf.perf_buffer_poll()
        except KeyboardInterrupt:
            exit()

if __name__ == "__main__":
    main(sys.argv[1:])