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

#define HAS_ARGS(...) BOOL(FIRST(__VA_ARGS__))
#define BOOL(x) BOOL_ ## x
#define BOOL_0 0
#define BOOL_1 1
#define FIRST(a, ...) a
#define IF_ELSE(condition) _IF_ELSE(BOOL(condition))
#define _IF_ELSE(condition) _IF_ ## condition
#define _IF_1(...) __VA_ARGS__ _IF_1_ELSE
#define _IF_0(...)             _IF_0_ELSE
#define _IF_1_ELSE(...)
#define _IF_0_ELSE(...) __VA_ARGS__

#if WINDOWS
    #if defined(__x86_64__) || defined(_M_X64)
    #define fsize _ftelli64
    #else
    #define fsize std::ftell
    #endif
#else
    #if defined(__x86_64__) || defined(_M_X64)
    #define fsize ftello64
    #else
    #define fsize std::ftell
    #endif
#endif

#define fill_rijndael_block(raw, iv)                                            \
    auto padding = raw.size() - ((raw.size() + iv.size() - 1) % iv.size() + 1); \
    raw.writeNull(padding);

namespace Sen::Kernel {
	
}