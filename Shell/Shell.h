#pragma once

#include <iostream>
#include <string.h>
#include <string>
#include <stdio.h>
#include <vector>
#include "interactive/color.hpp"

#if WIN32
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

typedef int (*execute)
(CStringView* argument, ShellCallback m_callback);

#define try_assert(condition, message) \
	if(!(condition)) {\
		throw std::runtime_error(message); \
	}