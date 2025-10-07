#!/usr/bin/python3
from bcc import BPF, USDT
import sys

from multiprocessing import shared_memory
import struct
from typing import cast
from bcc.table import HashTable
import ctypes as ct
import threading
import time

binary_to_usdt_dict = {}
fps_limit = 30
pid_app_low = None

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
        BPF_HASH(frame_time_us, u32, u32);

        struct log_entry {{
            u32  pid;
            u32  fps;
            char message[128];
        }};
        BPF_PERF_OUTPUT(log_entries);

        int test_on_fps_update(struct pt_regs *ctx) {{
            void *p_frame_tims_us;

            bpf_usdt_readarg(1, ctx, &p_frame_tims_us);

            u32 key = 0;
            u32 *frame_time_ptr = frame_time_us.lookup(&key);
            if (frame_time_ptr) {{
                bpf_probe_write_user(p_frame_tims_us, frame_time_ptr, sizeof(*frame_time_ptr));
            }}

            return 0;
        }}

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

    usdt = binary_to_usdt_dict[str(pid_app_low)]
    usdt.enable_probe(probe='vkaci:on_fps_update', fn_name='test_on_fps_update')
    usdt.enable_probe(probe='vkaci:on_fps_get', fn_name='test_on_fps_get')
    usdt_contexts.append(usdt)

    return BPF(text=bpf_text, usdt_contexts=usdt_contexts)

def main(args) -> None:
    global fps_limit, pid_app_low, fps_get_interval

    process_pid = args[0]
    fps_limit = int(args[1])
    fps_get_interval = int(args[2])
    benchmark_number = int(args[3])

    add_monitored_process(int(process_pid))
    pid_app_low = process_pid

    bpf = create_bpf_for_monitoring()
    frame_time_us = cast(HashTable, bpf.get_table("frame_time_us"))
    frame_time_us[ct.c_uint(0)] = ct.c_uint(1000*1000//fps_limit)

    shm_get = shared_memory.SharedMemory(name="limit_fps_" + str(benchmark_number))
    shm_send = shared_memory.SharedMemory(name="monitored_fps_" + str(benchmark_number))

    # process event
    def print_event(cpu, data, size):
        global fps_limit
        event = bpf['log_entries'].event(data)
        # print(event.message, event.pid, event.fps)
        with open("benchmark_" + str(benchmark_number) + ".txt", "a", encoding="utf-8") as f:
            f.write(f"{event.fps}\n")
        shm_send.buf[:4] = struct.pack("i", event.fps)
    def fps_limit_update():
        while (True):
            fps_limit = struct.unpack("i", shm_get.buf[:4])[0]
            if fps_limit != -1:
                shm_get.buf[:4] = struct.pack("i", -1)
                frame_time_us[ct.c_uint(0)] = ct.c_uint(1000*1000//fps_limit)
            time.sleep(0.1)

    t = threading.Thread(target=fps_limit_update, daemon=True)
    t.start()

    # loop with callback to print_event
    bpf["log_entries"].open_perf_buffer(print_event)
    while 1:
        try:
            bpf.perf_buffer_poll()
        except KeyboardInterrupt:
            exit()

if __name__ == "__main__":
    main(sys.argv[1:])