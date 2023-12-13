#pragma once

#include "kernel/definition/library.hpp"
#include "kernel/definition/macro.hpp"

namespace Sen::Kernel::Definition::Encryption::Sha224 
{

	// using C++ String

	using std::string;

	/**
	 * message: the message need to hash
	 * return: hashed message
	*/

	inline static auto hash(
		const string &message
	) -> string const
	{
		auto *sha224 = new Dependencies::SHA224CryptoServiceProvider::sha224();
		auto result = sha224->Hashing(message);
		delete sha224;
		return result;
	}

}