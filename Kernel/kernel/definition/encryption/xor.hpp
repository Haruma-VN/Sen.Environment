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
		const std::string & plain,
		const char* & key
	) -> std::string
	{
		auto result = plain;
		for (auto i = 0; i < plain.size(); i++)
		{
			result[i] = plain[i] ^ key[i % (sizeof(key) / sizeof(char))];
		}
		return result;
	}

	/**
	 * @param source: source file
	 * @param destination: destination file
	 * @param key: key
	 * @returns: encrypted file
	*/

	inline static auto encrypt_fs(
		const std::string & source,
		const std::string & destination,
		const char* & key
	) -> void
	{
		FileSystem::writeFile(destination, XOR::encrypt(FileSystem::readFile(source), key));
		return;
	}
	

}