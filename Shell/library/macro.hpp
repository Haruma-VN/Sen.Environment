#pragma once

#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <cmath>
#include <iomanip>
#include <math.h>
#include <stdexcept>
#include <filesystem>
#include <fstream>
#include <cstdint>
#include <regex>
#include <version>
#include <stdio.h>
#include <future>
#include <map>
#include <stack>
#include <list>
#include <stdlib.h>
#include <string.h>
#include <array>
#include <cstdio>
#include <cstring>
#include <exception>
#include <span>
#include "library/color.hpp"
#include "String.hpp"
#include "deps/cpp-httplib/httplib.h"

#define WINDOWS _WIN32

#define LINUX __linux__

#define ANDROID __ANDROID__

#define APPLE __APPLE__

#define IPHONE TARGET_OS_IPHONE

#define MACINTOSH __MACH__

using namespace httplib;

#if WINDOWS
#include <windows.h>
#else
#include <dlfcn.h>
#endif

#if WINDOWS
#define MAIN_FUNCTION int wmain(int size, wchar_t** argc)
#else
#define MAIN_FUNCTION int main(int size, char** argc)
#endif
struct StringView {
	int size;
	const char* value;
};

struct StringList {
	StringView* value;
	int size;

	static auto to_vector(
		const StringList& that
	) -> std::vector<std::string>
	{
		auto destination = std::vector<std::string>{};
		for (auto i = 0; i < that.size; ++i)
		{
			destination.emplace_back(std::string{ that.value[i].value, static_cast<std::size_t>(that.value[i].size)});
		}
		return destination;
	}
};

using CStringView = StringView;

using CStringList = StringList;

typedef void(*ShellCallback)(CStringList* list, CStringView* destination, void* proxy);

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