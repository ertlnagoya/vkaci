#!/usr/bin/bash
DISPLAY=:0 LD_PRELOAD=$PWD/../build/intercept.so vkmark -b vertex:duration=20 -s 5760x3240 &
APP_PID1=$!
echo vkmark1 is started with pid [$APP_PID1].

sleep 1

sudo ./fps_monitor.py $APP_PID1 &

P_PID1=$!

sleep 9

kill -9 $P_PID1

DISPLAY=:0 LD_PRELOAD=$PWD/../build/intercept.so vkmark -b vertex:duration=10 -s 11520x6480 &
APP_PID2=$!
echo vkmark2 is started with pid [$APP_PID2].

sudo ./fps_limiter.py $APP_PID1 30 &

P_PID1=$!

sleep 1

sudo ./fps_monitor.py $APP_PID2 &

P_PID2=$!

sleep 9

kill -9 $P_PID1
kill -9 $P_PID2