#pragma once

#include "kernel/definition/library.hpp"
#include "kernel/definition/macro.hpp"

namespace Sen::Kernel::Definition::Encryption::SHA256
{

	// Using C++ string

	using std::string;

	// Using C++ Array

	using std::array;

	// Hash method

	/**
	 * Pass: the string to hash
	 * Return: hashed string
	*/

	inline static auto hash(
		const std::string & message
	) -> std::string
	{

		auto sha = std::make_shared<Sen::Kernel::Dependencies::SHA256::SHA256>();
		sha->update(message);
		auto digest = static_cast<array<uint8_t, 32>>(sha->digest());
		return Sen::Kernel::Dependencies::SHA256::SHA256::toString(digest);
	}

	/**
	 * @param source: source file to read
	 * @returns: hashed string
	*/

	inline static auto hash_fs(
		const std::string & source
	) -> std::string
	{
		return SHA256::hash(FileSystem::readFile(source));
	}



}