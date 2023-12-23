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
	char data[700];
	size_t size;
};

struct Argument {
	BasicStringView* data;
	size_t size;

	Argument(
	) {
		this->size = 1;
		this->data = new BasicStringView[this->size];
		this->data[0] = BasicStringView{};
	}

	~Argument(

	)
	{
		delete this->data;
	}
};

struct Parameter {
	BasicStringView* data;
	size_t size;

	Parameter(

	)
	{
		this->size = 1;
		this->data = new BasicStringView[this->size];
		this->data[0] =  BasicStringView{};
	}

	Parameter(
		const std::vector<std::string>& data
	)
	{
		this->size = data.size();
		this->data = new BasicStringView[data.size()];
		for (auto i = 0; i < data.size(); ++i) {
			this->data[i] = BasicStringView{};
			strcpy_s(this->data[i].data, data.at(i).c_str());
			this->data[i].size = data.at(i).size();
		}
	}

	~Parameter(

	)
	{
		delete this->data;
	}


};

typedef void (*callback)(const char*, const Sen::Shell::Interactive::Color);

typedef BasicStringView (*input)();

typedef int (*KernelExecute)
(BasicStringView* argument, Parameter* params, callback sendMessage, input inputW, Sen::Shell::CliCallBack);

#define try_assert(condition, message) \
	if(!(condition)) {\
		throw std::runtime_error(message); \
	}