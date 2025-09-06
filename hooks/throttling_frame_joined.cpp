#include <iostream>
#include <chrono>
#include <cstdlib>
#include <stdexcept>
#include <thread>

#include "header.hpp"

#if 0
int getEnvInt(const char *name, int default_)
{
    // 環境変数の値を取得
    const auto envvar = std::getenv(name);
    if (envvar == nullptr)
        return default_;

    try
    {
        return std::stoi(envvar);
    }
    catch (const std::invalid_argument &e)
    {
        std::cerr << "Invalid integer value: " << name << "=" << envvar << std::endl;
    }
    catch (const std::out_of_range &e)
    {
        std::cerr << "Integer value out of range: " << name << "=" << envvar << std::endl;
    }
    std::cerr << "Defaulting to " << default_ << std::endl;
    return default_;
}
#endif

#if 0
static const auto TARGET_FPS = getEnvInt("TARGET_FPS", 30);
static const auto frame_time_target = std::chrono::microseconds(1000 * 1000 / TARGET_FPS);
#endif
static uint32_t frame_time_target_us = 0; // 1000U * 1000U / 60U;
static uint32_t fps_get_interval = 10;
static auto pred_frame_start = std::chrono::high_resolution_clock::now();

static auto fps_counter_begin_time = std::chrono::high_resolution_clock::now();
static auto fps_counter_begin_frame = 0U;

void onSwapCompleted()
{
    static auto frame_no = 0U;
    static uint32_t fps_get_count = 0;
    static uint32_t fps_counter_duration_sum = 0;
    static uint32_t fps_counter_duration_lastsum = 0;

    if (frame_no == 0) {
        fps_counter_begin_time = std::chrono::high_resolution_clock::now();
        fps_counter_begin_frame = frame_no;
    }

    std::this_thread::sleep_until(pred_frame_start + std::chrono::microseconds(frame_time_target_us));

    frame_no++;

    pred_frame_start = std::chrono::high_resolution_clock::now();

#if 0
    auto sleep_for = std::chrono::duration_cast<std::chrono::microseconds>(pred_frame_start - now);

    std::cerr << "FRAME_NUMBER:     " << frame_no++ << std::endl
              << "CPU_ELAPSED_TIME_US " << cpu_elp_time.count() << std::endl
              << "GPU_ELAPSED_TIME: " << gpu_elp_time.count() << "(us)" << std::endl
              << "GPU_USAGE       : " << (double)gpu_elp_time.count() / cpu_elp_time.count() << std::endl
              << "FRAME_ERROR     : " << frame_time_err.count() << "(us)" << std::endl
              << "SLEEP_FOR       : " << sleep_for.count() << "(us)" << std::endl;
#endif

    auto fps_counter_duration = std::chrono::duration_cast<std::chrono::microseconds>(pred_frame_start - fps_counter_begin_time);
    // 0.1秒ごとに目標FPS更新を受付、一定回数ごとに今のFPSを送信
    if (fps_counter_duration.count() + fps_counter_duration_sum >= 100000U * (fps_get_count + 1)) {
        DTRACE_PROBE1(vkaci, on_fps_update, &frame_time_target_us);
        fps_get_count++;
        fps_counter_begin_time = pred_frame_start;
        fps_counter_duration_sum += fps_counter_duration.count();
        if (fps_get_count % fps_get_interval == 0){
            // 前回計測時のフレームからの経過数 / 経過時間
            uint32_t fps = (frame_no - fps_counter_begin_frame) * 1000000U / (fps_counter_duration_sum - fps_counter_duration_lastsum);
            // 100秒ごとにリセット(誤差の蓄積を抑えるため)
            if (fps_get_count >= 1000){
                frame_no = 0;
                fps_get_count = 0;
                fps_counter_duration_sum = 0;
            }
            fps_counter_begin_frame = frame_no;
            fps_counter_duration_lastsum = fps_counter_duration_sum;
            DTRACE_PROBE2(vkaci, on_fps_get, fps, &fps_get_interval);
        }
    }
}