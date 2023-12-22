#pragma once

#include "kernel/definition/library.hpp"
#include "kernel/definition/macro.hpp"

namespace Sen::Kernel::Definition::Encryption::Sha224 {

	// using C++ String

	using std::string;

	/**
	 * message: the message need to hash
	 * return: hashed message
	*/

	inline static auto hash(
		const std::string & message
	) -> std::string
	{
		auto sha224 = std::make_shared<Dependencies::SHA224CryptoServiceProvider::sha224>();
		auto result = sha224->Hashing(message);
		return result;
	}

	/**
	 * message: the source file need to hash
	 * return: hashed message
	*/

	inline static auto hash_fs(
		const std::string & source
	) -> std::string
	{
		return Sen::Kernel::Definition::Encryption::Sha224::hash(FileSystem::readFile(source));
	}


}