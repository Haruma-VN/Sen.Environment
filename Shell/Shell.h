#pragma once

#include <iostream>
#include <string.h>
#include <string>
#include <stdio.h>
#include <vector>
#include "callback/method.hpp"
#include "interactive/color.hpp"

#if WIN32
#include <windows.h>
#else
#include <dlfcn.h>
#endif


#define MAIN_FUNCTION int main(int size, char** argc)

struct BasicStringView {
	const char* data;
	size_t size;

	BasicStringView(const char* str) : size(strlen(str)) {
		data = new char[size + 1];
		strcpy(const_cast<char*>(data), str);
	}

	BasicStringView() : size(0), data(new char[1]) {
	}

	~BasicStringView() {
		delete[] data;
	}
};

struct Parameter {
	BasicStringView* data;
	size_t size;

	explicit Parameter(

	) : size(0)
	{
		this->data = new BasicStringView[this->size];
	}

	inline static auto constexpr init(
		const std::vector<std::string>& data
	) -> BasicStringView*
	{
		BasicStringView* array = new BasicStringView[data.size()];
		for (size_t i = 0; i < data.size(); ++i) {
			array[i] = BasicStringView{ data[i].c_str() };
		}
		return array;
	}


	explicit constexpr Parameter(
		const std::vector<std::string> & data
	) : size(data.size()), data(init(data))
	{
	}


	~Parameter(

	)
	{
		delete[] this->data;
	}


};

struct MShellAPI {
	public:
		unsigned int version;
		bool is_gui;

		~MShellAPI(

		) = default;

		explicit MShellAPI(
			unsigned int version,
			bool is_gui
		) : version(version), is_gui(is_gui)
		{

		}
};

namespace Sen::Shell {

	inline auto constexpr version = 1Ui32;

}

typedef void (*callback)(const char*, const Sen::Shell::Interactive::Color);

typedef BasicStringView (*input)();

typedef int (*execute)
(BasicStringView* argument, Parameter* params, callback sendMessage, input inputW, MShellAPI _version);

#define try_assert(condition, message) \
	if(!(condition)) {\
		throw std::runtime_error(message); \
	}