#pragma once

#include <iostream>
#include <string.h>
#include <string>
#include <stdio.h>
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

	~Parameter(

	)
	{
		delete this->data;
	}


};

typedef void (*callback)(const char*, const char*, const Sen::Shell::Interactive::Color);

typedef BasicStringView (*input)();

typedef int (*KernelExecute)
(BasicStringView* argument, Parameter* params, callback sendMessage, input inputW, Sen::Shell::CliCallBack);