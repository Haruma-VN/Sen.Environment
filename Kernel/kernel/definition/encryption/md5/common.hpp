#pragma once 

#include "kernel/definition/library.hpp"
#include "kernel/definition/assert.hpp"
#include "kernel/definition/macro.hpp"

namespace Sen::Kernel::Definition::Encryption::MD5 
{

	// Byte definition

	typedef unsigned char byte;

	// Using C++ String

	using std::string;

	// Using C++ Span

	using std::span;


	// provide message to hash
	// return: the hashed message
	
	inline static auto hash(
		const span<const byte> &message
	) -> string
	{
		auto* md5 = new Dependencies::md5::MD5(message);
		auto result = md5->toStr();
		delete md5;
		return result;
	}

}