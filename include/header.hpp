#pragma once

#include <dlfcn.h>
#include <cstring>
#include <iostream>
#include <stdio.h>

#include <vulkan/vulkan.h>

#include "dlsym.hpp"

#define PUBLIC __attribute__((visibility("default")))
#define PRIVATE __attribute__((visibility("hidden")))
#define INITIALIZER __attribute__((constructor))
