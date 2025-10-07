#!/usr/bin/env python3

# 低優先度プロセスのFPSを下げることで高優先度プロセスのFPSを上げるデモ

import subprocess
import os
import time
import signal

from multiprocessing import shared_memory
import struct
import matplotlib.pyplot as plt
import threading

fps_get_interval = 10   # 0.1秒単位でFPS取得間隔を設定

with open("monitor.txt", "w", encoding="utf-8") as f:
    f.write("")
with open("limiter.txt", "w", encoding="utf-8") as f:
    f.write("")
target_values = []

# 優先度の高いプロセスから受け取るFPS
shm = shared_memory.SharedMemory(create=True, size=4, name="fps_shm")
shm.buf[:4] = struct.pack("i", -1)
# 優先度の低いプロセスに与えるFPS
shm2 = shared_memory.SharedMemory(create=True, size=4, name="limited_fps_shm")
shm2.buf[:4] = struct.pack("i", -1)

# 共通の環境変数設定
env = os.environ.copy()
# env["DISPLAY"] = ":0"
# env["LD_PRELOAD"] = os.path.join(os.getcwd(), "../build/intercept.so")
env["VK_INSTANCE_LAYERS"] = "VK_LAYER_VKACI_FPS"

def additional_mark():
    time.sleep(10)
    vkmark3 = subprocess.Popen(
        ["vkmark", "-b", "clear:duration=10", "-s", "7680x4320"],
        env=env
    )
    APP_PID3 = vkmark3.pid
    print(f"vkmark3 is started with pid [{APP_PID3}].")

# thread = threading.Thread(target=additional_mark, daemon=True)
# thread.start()

# --- vkmark1(重・低優先度) 開始 ---
vkmark1 = subprocess.Popen(
    ["vkmark", "-b", "effect2d:duration=30", "-s", "7680x4320"],
    # ["vkcube"],
    # ["./GravityMark.x64", "-vk", "-a", "1000", "-width", "720", "-height", "480", "-benchmark", "1"],
    # cwd="../../GravityMark/bin",
    env=env
)
APP_PID1 = vkmark1.pid

# --- vkmark2(軽・高優先度) 開始 ---
vkmark2 = subprocess.Popen(
    # ["vkmark", "-b", "effect2d:duration=30", "-s", "7680x4320"],
    # ["vkmark", "-b", "desktop:duration=30", "-s", "7680x4320"],
    ["./GravityMark.x64", "-vk", "-a", "1000", "-width", "720", "-height", "480", "-benchmark", "1"],
    cwd="../../GravityMark/bin",
    env=env
)
APP_PID2 = vkmark2.pid

time.sleep(0.4)

# fps-limiter 開始
fps_limiter = subprocess.Popen(
    ["sudo", "./fps_limiter.py", str(APP_PID1), "60", str(fps_get_interval)]
)
P_PID1 = fps_limiter.pid

# fps-monitor 開始
fps_monitor = subprocess.Popen(
    ["sudo", "./fps_monitor.py", str(APP_PID2), str(fps_get_interval)]
)
P_PID2 = fps_monitor.pid

# 目標FPSとの差が大きい時のみP制御、それ以外は定数で変化(低優先度プロセスのFPSを上げすぎない方向性)
class PController:
    def __init__(self, target=60, kp_up=0.2, kp_down=0.5, up_limit=10, down_limit=-20):
        self.target = target
        self.kp_up = kp_up
        self.kp_down = kp_down
        self.up_limit = up_limit
        self.down_limit = down_limit

    def update(self, monitored_value) -> int:
        error = monitored_value - self.target
        if error >= 0:
            return (int)(min(self.kp_up * error, self.up_limit))
        else:
            return (int)(min(-5, max(self.kp_down * error, self.down_limit)))

t = 0
target_fps = 60
limited_fps = 60
controller = PController()

while t <= 29:
    time.sleep(0.1)
    t += 0.1
    
    value = struct.unpack("i", shm.buf[:4])[0]
    if value != -1:
        if value < target_fps or target_fps + 20 < value:
            limited_fps += controller.update(value)
            shm2.buf[:4] = struct.pack("i", limited_fps)
            shm.buf[:4] = struct.pack("i", -1)
        else:
            shm.buf[:4] = struct.pack("i", -1)
        target_values.append(limited_fps)

# fps-limiter, fps-monitor 停止
os.kill(P_PID1, signal.SIGKILL)
os.kill(P_PID2, signal.SIGKILL)
os.kill(APP_PID1, signal.SIGKILL)
os.kill(APP_PID2, signal.SIGKILL)
shm.unlink()
shm2.unlink()

# グラフ化
def read_integers_from_file(filename):
    """テキストファイルから整数を1行ずつ読み込む"""
    with open(filename, "r") as f:
        return [int(line.strip()) for line in f if line.strip()]

def plot_graph():
    # ファイルからデータを読み込む
    monitor_values = read_integers_from_file("monitor.txt")
    limiter_values = read_integers_from_file("limiter.txt")

    # 要素数を揃える
    col = min(len(monitor_values), len(limiter_values))

    # 横軸を作成
    x = [1 + (fps_get_interval/10) * i for i in range(0, col)]

    # グラフ描画
    plt.figure(figsize=(10, 6))
    plt.plot(x, target_values[:col], label="Target(Limiter follow this)", marker="x", color="gray")
    plt.plot(x, monitor_values[:col], label="Monitor(high priority)", marker="o", color="orange")
    plt.plot(x, limiter_values[:col], label="Limiter(low priority)", marker="x", color="blue")

    plt.axhspan(target_fps, target_fps+20, facecolor="yellow", alpha=0.2)

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
    plt.savefig("fps_graph.png", dpi=300, bbox_inches="tight")

    # グラフ表示
    plt.show()

plot_graph()