#pragma once

#include "kernel/definition/library.hpp"
#include "kernel/definition/macro.hpp"
#include "kernel/definition/assert.hpp"
#include "kernel/dependencies/Rijndael.hpp"

namespace Sen::Kernel::Definition::Encryption 
{

	// Rijndael Mode

	enum RijndaelMode {
		ECB,
		CBC,
		CFB,
	};

	// Rijndael Struct

	struct Rijndael {

		public:

			/**
			 * @param plain: plain buffer
			 * @param key: key
			 * @param iv: iv
			 * @param plain_size: buffer size
			 * @param mode: rijndael mode
			 * @returns: encrypted buffer
			*/

			inline static auto encrypt(
				char const* plain,
				const std::string & key,
				const std::string & iv,
				size_t plain_size,
				RijndaelMode mode
			) -> std::vector<unsigned char>
			{			
				auto rijndael = std::make_shared<Sen::Kernel::Dependencies::Rijndael::CRijndael>();
				rijndael->MakeKey(key.c_str(), iv.c_str(), static_cast<int>(key.size()), static_cast<int>(iv.size()));
				auto result = std::unique_ptr<char[]>(new char[plain_size]);
				rijndael->Encrypt(plain, result.get(), plain_size, mode);
				auto m_result = std::vector<unsigned char>{reinterpret_cast<unsigned char*>(result.get()), reinterpret_cast<unsigned char*>(result.get() + plain_size)};
				return m_result;
			}

			/**
			 * @param plain: cipher buffer
			 * @param key: key
			 * @param iv: iv
			 * @param plain_size: buffer size
			 * @param mode: rijndael mode
			 * @returns: decrypted buffer
			*/

			inline static auto decrypt(
				char const* cipher,
				const std::string & key,
				const std::string & iv,
				size_t cipher_len,
				RijndaelMode mode
			) -> std::vector<unsigned char>
			{			
				auto rijndael = std::make_shared<Sen::Kernel::Dependencies::Rijndael::CRijndael>();
				rijndael->MakeKey(key.c_str(), iv.c_str(), static_cast<int>(key.size()), static_cast<int>(iv.size()));
				auto result = std::unique_ptr<char[]>(new char[cipher_len]);
				rijndael->Decrypt(cipher, result.get(), cipher_len, mode);
				auto m_result = std::vector<unsigned char>{reinterpret_cast<unsigned char*>(result.get()), reinterpret_cast<unsigned char*>(result.get() + cipher_len)};
				return m_result;
			}

			/**
			 * @param source: source file
			 * @param destination: destination file
			 * @param key: key
			 * @param iv: iv
			 * @param mode: rijndael mode
			 * @returns: decrypted buffer
			*/

			inline static auto encrypt_fs(
				const std::string & source,
				const std::string & destination,
				const std::string & key,
				const std::string & iv,
				RijndaelMode mode
			) -> void
			{
				auto source_data = FileSystem::read_binary<char>(source);
				FileSystem::write_binary<unsigned char>(destination, Rijndael::encrypt(source_data.data(), key, iv, source_data.size(), mode));
				return;
			}

			/**
			 * @param source: source file
			 * @param destination: destination file
			 * @param key: key
			 * @param iv: iv
			 * @param mode: rijndael mode
			 * @returns: decrypted buffer
			*/

			inline static auto decrypt_fs(
				const std::string & source,
				const std::string & destination,
				const std::string & key,
				const std::string & iv,
				RijndaelMode mode
			) -> void
			{
				auto source_data = FileSystem::read_binary<char>(source);
				FileSystem::write_binary<unsigned char>(destination, Rijndael::decrypt(source_data.data(), key, iv, source_data.size(), mode));
				return;
			}

	};
}