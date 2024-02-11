#pragma once

#include "kernel/definition/library.hpp"
#include "kernel/definition/macro.hpp"

namespace Sen::Kernel {

	// free c memory allocation

	inline static auto constexpr free_c_memory = [](auto p)
	{
		free(p);
		return;
	};

	// free c++ memory allocation

	inline static auto constexpr free_memory = [](auto p)
	{
		delete p;
		return;
	};

	// free c++ memory allocation

	inline static auto constexpr free_array = [](auto p)
	{
		delete[] p;
		return;
	};
}