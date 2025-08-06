#!/bin/bash

mkdir -p build

python3 main.py > intercept.cpp
g++ -DGLIBC_VERSION=${GLIBC_VERSION:=GLIBC_2.34} -shared -fPIC -Iinclude -Ihooks -ldl intercept.cpp -o ./build/intercept.so -lvulkan
