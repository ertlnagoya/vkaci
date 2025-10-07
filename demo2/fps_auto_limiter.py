#!/usr/bin/env python3

# 開発中断中
# 高優先度プロセスにもlimiterを適用し、余裕があれば低優先度プロセスのFPSも一定以上にするデモ(急峻な負荷変動はいったん考慮しない)

import subprocess
import os
import time
import signal

from multiprocessing import shared_memory
import struct
import matplotlib.pyplot as plt
import threading

fps_get_interval = 10   # 0.1秒単位でFPS取得間隔を設定
benchmark_count = 3     # ベンチマークの個数
target_fpses = [(60, 100), (30, 60), (10, 30)]

for i in range(benchmark_count):
    with open("benchmark_" + str(i) + ".txt", "w", encoding="utf-8") as f:
        f.write("")

# 共有メモリのリスト
shms_get:list[shared_memory.SharedMemory] = []
shms_send:list[shared_memory.SharedMemory] = []
for i in range(benchmark_count):
    shms_get.append(shared_memory.SharedMemory(create=True, size=4, name="monitored_fps_" + str(i)))
    shms_get[-1].buf[:4] = struct.pack("i", -1)
    shms_send.append(shared_memory.SharedMemory(create=True, size=4, name="limit_fps_" + str(i)))
    shms_send[-1].buf[:4] = struct.pack("i", -1)

# 共通の環境変数設定
env = os.environ.copy()
env["VK_INSTANCE_LAYERS"] = "VK_LAYER_VKACI_FPS"

def additional_mark():
    time.sleep(10)
    subprocess.Popen(
        ["vkmark", "-b", "clear:duration=10", "-s", "5760x3240"],
        env=env
    )

thread = threading.Thread(target=additional_mark, daemon=True)
thread.start()

app_pids = []
python_pids = []

# --- benchmark0(目標60〜100) 開始 ---
benchmark0 = subprocess.Popen(
    ["vkmark", "-b", "effect2d:duration=30", "-s", "7680x4320"],
    # ["vkmark", "-b", "desktop:duration=30", "-s", "7680x4320"],
    # ["./GravityMark.x64", "-vk", "-a", "1000", "-width", "720", "-height", "480", "-benchmark", "1"],
    # cwd="../../GravityMark/bin",
    env=env
)
app_pids.append(benchmark0.pid)

# --- benchmark1(目標30〜60) 開始 ---
benchmark1 = subprocess.Popen(
    # ["vkmark", "-b", "effect2d:duration=30", "-s", "7680x4320"],
    ["vkmark", "-b", "desktop:duration=30", "-s", "5760x3240"],
    # ["./GravityMark.x64", "-vk", "-a", "1000", "-width", "720", "-height", "480", "-benchmark", "1"],
    # cwd="../../GravityMark/bin",
    env=env
)
app_pids.append(benchmark1.pid)

# --- benchmark2(目標30以下) 開始 ---
benchmark2 = subprocess.Popen(
    # ["vkmark", "-b", "effect2d:duration=30", "-s", "7680x4320"],
    # ["vkmark", "-b", "desktop:duration=30", "-s", "7680x4320"],
    ["./GravityMark.x64", "-vk", "-a", "1000", "-width", "480", "-height", "270", "-benchmark", "1"],
    cwd="../../GravityMark/bin",
    env=env
)
app_pids.append(benchmark2.pid)

time.sleep(0.4)

fps_limiters = []

for i in range(benchmark_count):
    p = subprocess.Popen(
        ["sudo", "./fps_limiter.py", str(app_pids[i]), str(target_fpses[i][1]), str(fps_get_interval), str(i)]
    )
    python_pids.append(p.pid)

t = 0
current_target_fpses = [target_fpses[0][1], target_fpses[1][1], target_fpses[2][1]]
monitored_fpses = [[] for i in range(benchmark_count)]
target_fps_logs = [[] for i in range(benchmark_count)]

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

    # 最優先プロセスが目標に達していないとき
    if fpses[0] < target_fpses[0][0]:
        # 低優先プロセスに余裕があればそこから下げる
        if fpses[2] >= target_fpses[2][0] + 5:
            current_target_fpses[2] -= 5
            shms_send[2].buf[:4] = struct.pack("i", current_target_fpses[2])
        # 中優先プロセスに余裕があればそこから下げる
        elif fpses[1] >= target_fpses[1][0] + 5:
            current_target_fpses[1] -= 5
            shms_send[1].buf[:4] = struct.pack("i", current_target_fpses[1])
        # どちらにも余裕がなければ両方から下げる(最低5まで)
        else:
            current_target_fpses[1] = max(5, current_target_fpses[1] - 3)
            current_target_fpses[2] = max(5, current_target_fpses[2] - 1)
            shms_send[1].buf[:4] = struct.pack("i", current_target_fpses[1])
            shms_send[2].buf[:4] = struct.pack("i", current_target_fpses[2])

    # 中優先プロセスが目標に達していないとき
    if fpses[1] < target_fpses[1][0]:
        # 低優先プロセスに余裕があればそこから下げる
        if fpses[2] >= target_fpses[2][0] + 5:
            current_target_fpses[2] -= 5
            shms_send[2].buf[:4] = struct.pack("i", current_target_fpses[2])
        # 最優先プロセスに余裕があればそこから下げる
        if fpses[0] >= target_fpses[0][0] + 5:
            current_target_fpses[0] -= 5
            shms_send[0].buf[:4] = struct.pack("i", current_target_fpses[0])
    
    # 下げすぎたFPSを上げる


    target_fps_logs[0].append(current_target_fpses[0])
    target_fps_logs[1].append(current_target_fpses[1])
    target_fps_logs[2].append(current_target_fpses[2])


# プロセス停止
for i in range(benchmark_count):
    os.kill(python_pids[i], signal.SIGKILL)
    os.kill(app_pids[i], signal.SIGKILL)
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

    colors = ["orange", "green", "blue"]
    shapes = ["o", "^", "x"]
    # グラフ描画
    plt.figure(figsize=(10, 6))
    for i in range(benchmark_count):
        plt.plot(x, monitored_fpses[i], label="benchmark_" + str(i), marker=shapes[i], color=colors[i])
        plt.plot(x, target_fps_logs[i], label="benchmark_" + str(i) + "_target", marker=shapes[i], color="gray")
        plt.axhspan(target_fpses[i][0], target_fpses[i][1], facecolor=colors[i], alpha=0.2)

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