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

	inline static auto encrypt (
		const std::vector<std::uint8_t> & plain,
		const std::vector<std::uint8_t> & key
	) -> std::vector<std::uint8_t>
	{
		auto result = std::vector<std::uint8_t>(plain.size(), 0x00_byte);
		for (auto i : Range(plain.size())) {
			result[i] = plain[i] ^ key[i % key.size()];
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
		std::string_view source,
		std::string_view destination,
		const std::vector<std::uint8_t> & key
	) -> void
	{
		FileSystem::write_binary(destination, XOR::encrypt(FileSystem::read_binary<std::uint8_t>(source), std::vector<std::uint8_t>{key.begin(), key.end()}));
		return;
	}
	

}