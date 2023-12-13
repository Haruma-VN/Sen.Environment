#pragma once

#include "kernel/definition/library.hpp"
#include "kernel/definition/macro.hpp"

namespace Sen::Kernel::Definition::Encryption::XOR 
{

	/**
	 * key: the key to encrypt
	 * plain: the plain text
	 * return: cipher text
	*/

	inline static auto encrypt(
		const char* &key,
		const string &plain
	) -> string
	{
		auto result = plain;
		for (auto i = 0; i < plain.size(); i++)
		{
			result[i] = plain[i] ^ key[i % (sizeof(key) / sizeof(char))];
		}
		return result;
	}
	

}