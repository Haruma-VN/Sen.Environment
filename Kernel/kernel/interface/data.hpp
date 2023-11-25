#pragma once

#include "kernel/definition/library.hpp"
#include "kernel/definition/macro.hpp"

namespace Sen::Kernel::Interface 
{

	// basic string view
	
	struct BasicStringView {
		char data[700];
		size_t size;
	};

	// call back

	typedef void (*callback)(const char*);

	// input

	typedef BasicStringView (*input)();

	// argument

	struct Argument {
		BasicStringView* data;
		size_t size;
	};

	// parameter

	struct Parameter {
		Argument* data;
		size_t size;
	};

	// vector init

	using std::vector;

	// string init

    using std::string;

	// argument -> vector<string>

    inline static auto convertArgumentToVectorString(
		Argument* argument
	) -> vector<string>
	{
        auto result = vector<string> {};
        for (auto i = 0; i < argument->size; i++){
            result.push_back(string {argument->data[i].data, argument->data[i].size});
        }
        return result;
    }

	// param -> vector<string>

	inline static auto convertParameterToVectorString(
		Parameter* param
	) -> vector<vector<string>>
	{
        auto result = vector<vector<string>> {};
        for (auto i = 0; i < param->size; i++)
		{
			result.at(i) = vector<string>{};
			for (auto j = 0; j < param->data[i].size; j++)
			{
				result.at(i).push_back(string {param->data[i].data->data, param->data[i].data->size});
			}
		}
        return result;
    }

}