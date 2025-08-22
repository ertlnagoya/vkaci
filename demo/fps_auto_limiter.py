#!/usr/bin/env python3
import subprocess
import os
import time
import signal

from multiprocessing import shared_memory
import struct
import matplotlib.pyplot as plt
import threading

with open("monitor.txt", "w", encoding="utf-8") as f:
    f.write("")
with open("limiter.txt", "w", encoding="utf-8") as f:
    f.write("")

# 優先度の高いプロセスから受け取るFPS
shm = shared_memory.SharedMemory(create=True, size=4, name="fps_shm")
shm.buf[:4] = struct.pack("i", -1)
# 優先度の低いプロセスに与えるFPS
shm2 = shared_memory.SharedMemory(create=True, size=4, name="limited_fps_shm")
shm2.buf[:4] = struct.pack("i", -1)

# 共通の環境変数設定
env = os.environ.copy()
env["DISPLAY"] = ":0"
env["LD_PRELOAD"] = os.path.join(os.getcwd(), "../build/intercept.so")

# --- vkmark1(重・低優先度) 開始 ---
vkmark1 = subprocess.Popen(
    ["vkmark", "-b", "effect2d:duration=30", "-s", "7680x4320"],
    env=env
)
APP_PID1 = vkmark1.pid
print(f"vkmark1 is started with pid [{APP_PID1}].")

# --- vkmark2(軽・高優先度) 開始 ---
vkmark2 = subprocess.Popen(
    ["vkmark", "-b", "desktop:duration=30", "-s", "7680x4320"],
    env=env
)
APP_PID2 = vkmark2.pid
print(f"vkmark2 is started with pid [{APP_PID2}].")

time.sleep(0.5)

# fps-limiter 開始
fps_limiter = subprocess.Popen(
    ["sudo", "./fps_limiter.py", str(APP_PID1), "60"]
)
P_PID1 = fps_limiter.pid

# fps-monitor 開始
fps_monitor = subprocess.Popen(
    ["sudo", "./fps_monitor.py", str(APP_PID2)]
)
P_PID2 = fps_monitor.pid

def additional_mark():
    time.sleep(10)
    vkmark3 = subprocess.Popen(
        ["vkmark", "-b", "clear:duration=30", "-s", "7680x4320"],
        env=env
    )
    APP_PID3 = vkmark3.pid
    print(f"vkmark3 is started with pid [{APP_PID3}].")
    time.sleep(10)
    os.kill(APP_PID3, signal.SIGKILL)

# thread = threading.Thread(target=additional_mark, daemon=True)
# thread.start()

t = 0
target_fps = 60
limited_fps = 60

while t < 30:
    t += 0.2
    
    value = struct.unpack("i", shm.buf[:4])[0]
    if value != -1:
        if value < target_fps:
            # os.kill(P_PID1, signal.SIGKILL)
            limited_fps -= 5
            # fps_limiter = subprocess.Popen(
            #     ["sudo", "./fps_limiter.py", str(APP_PID1), (str)(limited_fps)]
            # )
            # P_PID1 = fps_limiter.pid
            shm2.buf[:4] = struct.pack("i", limited_fps)
            shm.buf[:4] = struct.pack("i", -1)
        elif value > target_fps + 30:
            # os.kill(P_PID1, signal.SIGKILL)
            limited_fps += 5
            # fps_limiter = subprocess.Popen(
            #     ["sudo", "./fps_limiter.py", str(APP_PID1), (str)(limited_fps)]
            # )
            # P_PID1 = fps_limiter.pid
            shm2.buf[:4] = struct.pack("i", limited_fps)
            shm.buf[:4] = struct.pack("i", -1)

    time.sleep(0.2)

# fps-limiter, fps-monitor 停止
os.kill(P_PID1, signal.SIGKILL)
os.kill(P_PID2, signal.SIGKILL)
shm.unlink()
shm2.unlink()

# グラフ化
def read_integers_from_file(filename):
    """テキストファイルから整数を1行ずつ読み込む"""
    with open(filename, "r") as f:
        return [int(line.strip()) for line in f if line.strip()]

def plot_graph(monitor_file, limiter_file, output_file="fps_graph.png"):
    # ファイルからデータを読み込む
    monitor_values = read_integers_from_file(monitor_file)
    limiter_values = read_integers_from_file(limiter_file)

    # 行数(横軸)を作成
    x = list(range(1, len(monitor_values) + 1))

    # グラフ描画
    plt.figure(figsize=(10, 6))
    plt.plot(x, monitor_values, label="Monitor(high priority)", marker="o", color="orange")
    plt.plot(x, limiter_values, label="Limiter(low priority)", marker="x", color="blue")

    plt.axhspan(target_fps, target_fps+30, facecolor="yellow", alpha=0.2)

    # グラフの装飾
    plt.xlabel("Time(s)")
    plt.ylabel("FPS")
    plt.title("Monitor & Limiter FPS")
    plt.legend()
    plt.grid(True)

    plt.xlim(left=0)
    plt.xlim(right=30)
    plt.ylim(bottom=0)
    plt.ylim(top=120)

    # PNGファイルとして保存
    plt.savefig(output_file, dpi=300, bbox_inches="tight")

    # グラフ表示
    plt.show()

plot_graph("monitor.txt", "limiter.txt")