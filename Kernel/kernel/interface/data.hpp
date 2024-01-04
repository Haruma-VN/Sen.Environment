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

	// argument

	struct Argument {
		BasicStringView* data;
		size_t size;
	};

	// parameter

	struct Parameter {
		BasicStringView* data;
		size_t size;
	};

	// vector init

	using std::vector;

	// string init

    using std::string;

	// argument -> vector<string>

    inline static auto convertArgumentToVectorString(
		Argument* argument
	) -> std::vector<std::string>
	{
        auto result = std::vector<std::string> {};
        for (auto i = 0; i < argument->size; i++){
            result.push_back(std::string {argument->data[i].data, argument->data[i].size});
        }
        return result;
    }

	// param -> vector<string>

	inline static auto convertParameterToVectorString(
		Parameter* param
	) -> std::vector<std::string>
	{
        auto result = std::vector<std::string> {};
        for (auto i = 0; i < param->size; i++){
            result.push_back(std::string {param->data[i].data, param->data[i].size});
        }
        return result;
    }

	// C Struct -> C++ String

	inline static auto convertBasicStringViewToString(
		BasicStringView* argument
	) -> std::string const
	{
		return std::string{argument->data, argument->size};
	}

}