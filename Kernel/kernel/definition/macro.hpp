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

#define countof(x) (sizeof(x) / sizeof((x)[0]))

#if WINDOWS
#include <windows.h>
#endif

#if APPLE
#include <Carbon/Carbon.h>
#endif

#define M_INSTANCE_OF_STRUCT(struct_name)\
inline static auto instance(\
) -> struct_name&\
{\
static auto INSTANCE = struct_name{};\
return INSTANCE;\
}

#define M_INSTANCE_OF_CLASS(class_name) M_INSTANCE_OF_STRUCT(class_name)

namespace Sen::Kernel {
	
}