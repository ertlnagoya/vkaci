#!/bin/bash

mkdir -p build

python3 main.py > vkaci_layer.cpp
g++ -shared -fPIC -Ihooks vkaci_layer.cpp -o ./build/libvkaci_layer_fps.so
cp ./build/libvkaci_layer_fps.so $VULKAN_SDK/lib