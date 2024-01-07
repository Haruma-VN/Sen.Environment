#pragma once

#include "kernel/definition/library.hpp"
#include "kernel/definition/macro.hpp"

namespace Sen::Kernel::Interface {

	// basic string view
	
	struct BasicStringView {
		const char* data;
		size_t size;
	};

	// call back

	typedef void (*callback)(const char*, const Sen::Kernel::Interface::Color);

	// input

	typedef struct BasicStringView (*input)();

	// parameter

	struct Parameter {
		BasicStringView* data;
		size_t size;
	};

	// param -> vector<string>

	inline static auto convert_parameter_to_vector_string(
		Parameter* param
	) -> std::vector<std::string>
	{
        auto result = std::vector<std::string> {};
        for (auto i : Range<size_t>(param->size)){
            result.emplace_back(std::string {param->data[i].data, param->data[i].size});
        }
        return result;
    }

	// C Struct -> C++ String

	inline static auto convert_basic_string_view_to_string(
		BasicStringView* argument
	) -> std::string const
	{
		return std::string{argument->data, argument->size};
	}

}