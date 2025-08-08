#!/usr/bin/bash
DISPLAY=:0 LD_PRELOAD=$PWD/../build/intercept.so vkmark -b vertex:duration=10 -s 5760x3240 &
APP_PID=$!
echo vkmark1 is started with pid [$APP_PID].

sleep 1

sudo ./fps_monitor.py $APP_PID &

P_PID=$!

sleep 9

kill $P_PID