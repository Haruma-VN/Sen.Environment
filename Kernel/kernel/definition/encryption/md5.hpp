#pragma once 

#include "kernel/definition/library.hpp"
#include "kernel/definition/assert.hpp"
#include "kernel/definition/macro.hpp"

namespace Sen::Kernel::Definition::Encryption::MD5 
{

	// Byte definition

	typedef unsigned char byte;


	// provide message to hash
	// return: the hashed message
	
	inline static auto hash(
		const std::span<const byte> &message
	) -> std::string
	{
		auto md5 = std::make_unique<Dependencies::md5::MD5>(message);
		auto result = md5->toStr();
		return result;
	}

	/**
	 * @param source: source file
	 * @returns: hashed string
	*/

	inline static auto hash_fs(
		std::string_view source
	) -> std::string
	{
		auto str = FileSystem::read_binary<unsigned char>(source);
		auto result = Kernel::Definition::Encryption::MD5::hash(str);
		return result;
	}

}