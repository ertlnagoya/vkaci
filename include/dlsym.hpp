#pragma once

#include <dlfcn.h>
#include <stdio.h>

#ifndef GLIBC_VERSION
#error "GLIBC_VERSION is not defined"
#endif
#define _TO_STR(x) #x
#define TO_STR(x) _TO_STR(x)
#define GLIBC_VERSION_STR TO_STR(GLIBC_VERSION)

typedef void *(*dlsym_func_t)(void *, const char *);

void *original_dlsym(void *handle, const char *symbol)
{
    static dlsym_func_t original_dlsym_ptr = nullptr;
    if (original_dlsym_ptr == nullptr)
    {
        auto lib_handle = dlopen("libc.so.6", RTLD_LAZY);
        if (!lib_handle)
        {
            fprintf(stderr, "Error opening libc: %s\n", dlerror());
            return nullptr;
        }

        original_dlsym_ptr = reinterpret_cast<dlsym_func_t>(dlvsym(lib_handle, "dlsym", GLIBC_VERSION_STR));
        if (original_dlsym_ptr == nullptr)
        {
            fprintf(stderr, "Error finding dlsym: %s\n", dlerror());
            return nullptr;
        }
    }

    return original_dlsym_ptr(handle, symbol);
}
