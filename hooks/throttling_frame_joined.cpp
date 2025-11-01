#include <iostream>
#include <thread>
#include <sys/sdt.h>

uint32_t frame_time_target_us = 0;
uint32_t fps_get_interval = 10;
auto pred_frame_start = std::chrono::high_resolution_clock::now();
auto fps_counter_begin_time = std::chrono::high_resolution_clock::now();
auto fps_counter_begin_frame = 0U;
auto frame_no = 0U;
uint32_t fps_get_count = 0;
uint32_t fps_counter_duration_sum = 0;
uint32_t fps_counter_duration_lastsum = 0;
uint32_t target_fps_miss_count = 0;

void onSwapCompleted()
{
    // 前フレームを基準とするのではなく、目標間隔を足し合わせていくことで誤差の蓄積を抑える
    pred_frame_start += std::chrono::microseconds(frame_time_target_us);
    std::this_thread::sleep_until(pred_frame_start);
    
    frame_no++;
    auto real_frame_start = std::chrono::high_resolution_clock::now();  // FPSの計算には実際にフレームが切り替わるタイミングを用いる
    // 目標フレーム間隔を明らかに満たしていない状態が続いた場合(制限がないとき、リソースが足りない時)は目標時刻の基準をリセット
    if (pred_frame_start + std::chrono::microseconds(2000) < real_frame_start){
        target_fps_miss_count++;
        if (target_fps_miss_count ==30){
            pred_frame_start = real_frame_start;
            target_fps_miss_count = 0;
        }
    }
    else{
        target_fps_miss_count = 0;
    }

    auto fps_counter_duration = std::chrono::duration_cast<std::chrono::microseconds>(real_frame_start - fps_counter_begin_time);
    // 0.1秒ごとに目標FPS更新を受付、一定回数ごとに今のFPSを送信
    if (fps_counter_duration.count() + fps_counter_duration_sum >= 100000U * (fps_get_count + 1)) {
        DTRACE_PROBE1(vkaci, on_fps_update, &frame_time_target_us);
        fps_get_count++;
        fps_counter_begin_time = real_frame_start;
        fps_counter_duration_sum += fps_counter_duration.count();
        if (fps_get_count % fps_get_interval == 0){
            printf("frame_count:%d, FPS:%f\n", frame_no - fps_counter_begin_frame , (frame_no - fps_counter_begin_frame) * (float)1000000 / (fps_counter_duration_sum - fps_counter_duration_lastsum));
            // 前回計測時のフレームからの経過数 / 経過時間
            uint32_t fps = (frame_no - fps_counter_begin_frame) * 1000000U / (fps_counter_duration_sum - fps_counter_duration_lastsum);
            // 100秒ごとにリセット(1秒ごとだとすぐに誤差が蓄積する)
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