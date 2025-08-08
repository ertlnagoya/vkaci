#!/usr/bin/env python3
import subprocess
import os
import time
import signal

from multiprocessing import shared_memory
import struct

with open("monitor.txt", "w", encoding="utf-8") as f:
    f.write("Monitoring start.\n")
with open("limiter.txt", "w", encoding="utf-8") as f:
    f.write("Limiting start.\n")

shm = shared_memory.SharedMemory(create=True, size=4, name="fps_shm")
shm.buf[:4] = struct.pack("i", -1)

# 共通の環境変数設定
env = os.environ.copy()
env["DISPLAY"] = ":0"
env["LD_PRELOAD"] = os.path.join(os.getcwd(), "../build/intercept.so")

# --- vkmark1 開始 ---
vkmark1 = subprocess.Popen(
    ["vkmark", "-b", "vertex:duration=30", "-s", "5760x3240"],
    env=env
)
APP_PID1 = vkmark1.pid
print(f"vkmark1 is started with pid [{APP_PID1}].")

# --- vkmark2 開始 ---
vkmark2 = subprocess.Popen(
    ["vkmark", "-b", "vertex:duration=30", "-s", "11520x6480"],
    env=env
)
APP_PID2 = vkmark2.pid
print(f"vkmark2 is started with pid [{APP_PID2}].")

time.sleep(1)

# fps-limiter 開始
fps_limiter = subprocess.Popen(
    ["sudo", "./fps_limiter.py", str(APP_PID1), "60"]
)
P_PID1 = fps_limiter.pid

time.sleep(1)

# fps-monitor 開始
fps_monitor = subprocess.Popen(
    ["sudo", "./fps_monitor.py", str(APP_PID2)]
)
P_PID2 = fps_monitor.pid

t = 0
target_fps = 80
limited_fps = 60

while t < 30:
    t += 0.2
    
    value = struct.unpack("i", shm.buf[:4])[0]
    if value != -1:
        if value < target_fps:
            os.kill(P_PID1, signal.SIGKILL)
            limited_fps -= 5
            fps_limiter = subprocess.Popen(
                ["sudo", "./fps_limiter.py", str(APP_PID1), (str)(limited_fps)]
            )
            P_PID1 = fps_limiter.pid
            shm.buf[:4] = struct.pack("i", -1)
        elif value > target_fps + 10:
            os.kill(P_PID1, signal.SIGKILL)
            limited_fps += 5
            fps_limiter = subprocess.Popen(
                ["sudo", "./fps_limiter.py", str(APP_PID1), (str)(limited_fps)]
            )
            P_PID1 = fps_limiter.pid
            shm.buf[:4] = struct.pack("i", -1)

    time.sleep(0.2)

# fps-limiter, fps-monitor2 停止
os.kill(P_PID1, signal.SIGKILL)
os.kill(P_PID2, signal.SIGKILL)
shm.unlink()