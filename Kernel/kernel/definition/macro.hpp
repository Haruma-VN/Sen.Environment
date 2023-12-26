#pragma once 

#include "kernel/definition/library.hpp"

#define thiz (*this)

#define debug(arg) std::cout << arg << std::endl;

#define WINDOWS _WIN32

#define LINUX __linux__

#define ANDROID __ANDROID__

#define APPLE __APPLE__

#define IPHONE TARGET_OS_IPHONE

#define MACINTOSH __MACH__

#if WINDOWS
#define M_EXPORT_API extern "C" __declspec(dllexport)
#else
#define M_EXPORT_API extern "C" __attribute__((visibility("default")))
#endif

#define calculate_area(width, height) width * height