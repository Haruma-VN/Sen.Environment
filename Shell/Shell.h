#pragma once

#include <iostream>
#include <string.h>
#include <string>
#include <stdio.h>
#include <vector>
#include <filesystem>
#include "interactive/color.hpp"

#define WINDOWS _WIN32

#define LINUX __linux__

#define ANDROID __ANDROID__

#define APPLE __APPLE__

#define IPHONE TARGET_OS_IPHONE

#define MACINTOSH __MACH__

#if WINDOWS
#include <windows.h>
#else
#include <dlfcn.h>
#endif

#define MAIN_FUNCTION int main(int size, char** argc)
struct StringView {
	size_t size;
	const char* value;
};

struct StringList {
	StringView* value;
	size_t size;

	static auto to_vector(
		const StringList& that
	) -> std::vector<std::string>
	{
		auto destination = std::vector<std::string>{};
		for (auto i = 0; i < that.size; ++i)
		{
			destination.emplace_back(std::string{ that.value[i].value, that.value[i].size });
		}
		return destination;
	}
};

using CStringView = StringView;

using CStringList = StringList;

typedef CStringView(*ShellCallback)(CStringList list);

namespace Sen::Shell {

	inline auto constexpr version = 1;

}

#if WINDOWS
#define KERNEL_DEFAULT "kernel.dll"
#elif MACINTOSH || IPHONE
#define KERNEL_DEFAULT "kernel.dylib"
#elif ANDROID || LINUX
#define KERNEL_DEFAULT "kernel.so"
#endif

char* copy = new char[1];

typedef int (*execute)
(CStringView* script, CStringList* argument, ShellCallback m_callback);

#define assert_conditional(condition, message) \
	if(!(condition)) {\
		throw std::runtime_error(message); \
	}