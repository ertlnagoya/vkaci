#!/usr/bin/env python3

# 外部で起動された、暗黙レイヤー適用済みVulkanプロセスのIDを受け取る想定のデモ

import subprocess
import os
import time
import signal
import sys

from multiprocessing import shared_memory
import struct
import matplotlib.pyplot as plt
import threading

fps_get_interval = 10   # 0.1秒単位でFPS取得間隔を設定
benchmark_count = 2     # ベンチマークの個数
pid1 = sys.argv[1]
pid2 = sys.argv[2]
target_fpses = [(60, 120), (30, 60)]

for i in range(benchmark_count):
    with open("benchmark_" + str(i) + ".txt", "w", encoding="utf-8") as f:
        f.write("")

# 共有メモリのリスト
shms_get:list[shared_memory.SharedMemory] = []
shms_send:list[shared_memory.SharedMemory] = []
for i in range(benchmark_count):
    try:
        shm_g = shared_memory.SharedMemory(create=True, size=4, name="monitored_fps_" + str(i))
        shms_get.append(shm_g)
    except FileExistsError:
        shm_g = shared_memory.SharedMemory(name="monitored_fps_" + str(i))
        shms_get.append(shm_g)
    shms_get[-1].buf[:4] = struct.pack("i", -1)
    try:
        shm_s = shared_memory.SharedMemory(create=True, size=4, name="limit_fps_" + str(i))
        shms_send.append(shm_s)
    except FileExistsError:
        shm_s = shared_memory.SharedMemory(name="limit_fps_" + str(i))
        shms_send.append(shm_s)
    shms_send[-1].buf[:4] = struct.pack("i", -1)

def additional_mark():
    time.sleep(10)
    subprocess.Popen(
        ["vkmark", "-b", "clear:duration=10", "-s", "5760x3240"]
    )

# thread = threading.Thread(target=additional_mark, daemon=True)
# thread.start()

app_pids = [pid1, pid2]
python_pids = []

for i in range(benchmark_count):
    p = subprocess.Popen(
        ["sudo", "./fps_limiter.py", app_pids[i], str(target_fpses[i][1]), str(fps_get_interval), str(i)]
    )
    python_pids.append(p.pid)

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
# 最優先プロセスのFPSは、Switch2の最大リフレッシュレートである120を初期設定とする
current_target_fpses = [120, target_fpses[1][1]]
monitored_fpses = [[] for i in range(benchmark_count)]
target_fps_logs = [[] for i in range(benchmark_count)]
controller = PController()

while t <= 29:
    time.sleep(0.1)
    t += 0.1
    
    all_updated = True
    fpses = []
    for i in range(benchmark_count):
        value = struct.unpack("i", shms_get[i].buf[:4])[0]
        if value == -1:
            all_updated = False
            break
        fpses.append(value)

    if not all_updated:
        continue

    for i in range(benchmark_count):
        monitored_fpses[i].append(fpses[i])
        shms_get[i].buf[:4] = struct.pack("i", -1)

    # 高優先度側が目標に達していない時
    if fpses[0] < target_fpses[0][0]:
        current_target_fpses[1] += controller.update(fpses[0])
        shms_send[1].buf[:4] = struct.pack("i", current_target_fpses[1])
    # 低優先度側が目標に達しておらず、高優先度側に余裕がある時
    elif fpses[1] < target_fpses[1][0] - 1 and fpses[0] > target_fpses[0][0] + 10:
        current_target_fpses[0] = min(fpses[0], current_target_fpses[0] - 5)
        shms_send[0].buf[:4] = struct.pack("i", current_target_fpses[0])
    # 低優先度側に余裕があり、高優先度側の制限値が120未満かつ実FPSが制限値に達している時
    elif fpses[1] > (target_fpses[1][0] + target_fpses[1][1]) / 2 and current_target_fpses[0] < 120 and fpses[0] >= current_target_fpses[0] - 1:
        current_target_fpses[0] = min(120, current_target_fpses[0] + 5)
        shms_send[0].buf[:4] = struct.pack("i", current_target_fpses[0])
    # 高優先度側が目標上限に達しており、低優先度側の制限値が目標上限未満かつ実FPSが制限値に達している時
    elif fpses[0] >= 119 and current_target_fpses[1] < target_fpses[1][1] and fpses[1] >= current_target_fpses[1] - 1:
        current_target_fpses[1] = min(target_fpses[1][1], current_target_fpses[1] + controller.update(fpses[0]))
        shms_send[1].buf[:4] = struct.pack("i", current_target_fpses[1])

    target_fps_logs[0].append(current_target_fpses[0])
    target_fps_logs[1].append(current_target_fpses[1])


# プロセス停止
for i in range(benchmark_count):
    os.kill(python_pids[i], signal.SIGTERM)
    shms_get[i].unlink()
    shms_send[i].unlink()

# グラフ化
def read_integers_from_file(filename):
    """テキストファイルから整数を1行ずつ読み込む"""
    with open(filename, "r") as f:
        return [int(line.strip()) for line in f if line.strip()]

def plot_graph():
    col = len(monitored_fpses[0])

    # 横軸を作成
    x = [1 + (fps_get_interval/10) * i for i in range(col)]

    colors = ["orange", "blue"]
    shapes = ["o", "x"]
    # グラフ描画
    plt.figure(figsize=(10, 6))
    for i in range(benchmark_count):
        plt.plot(x, target_fps_logs[i], label="benchmark_" + str(i) + "_target", marker=shapes[i], color="gray")
        plt.plot(x, monitored_fpses[i], label="benchmark_" + str(i), marker=shapes[i], color=colors[i])
        plt.axhspan(target_fpses[i][0], target_fpses[i][1], facecolor=colors[i], alpha=0.1)

    # グラフの装飾
    plt.xlabel("Time(s)")
    plt.ylabel("FPS")
    plt.title("Benchmark FPS")
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