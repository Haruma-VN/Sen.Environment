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

	// Vector

	using std::vector;

	// String

	using std::string;

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

			static auto encrypt(
				char const* plain,
				const std::string & key,
				const std::string & iv,
				size_t plain_size,
				RijndaelMode mode
			) -> std::vector<unsigned char>
			{			
				auto rijndael = std::make_shared<Sen::Kernel::Dependencies::Rijndael::CRijndael>();
				rijndael->MakeKey(key.c_str(), iv.c_str(), static_cast<int>(key.size()), static_cast<int>(iv.size()));
				auto result = new char[plain_size];
				rijndael->Encrypt(plain, result, plain_size, mode);
				auto m_result = std::vector<unsigned char>{reinterpret_cast<unsigned char*>(result), reinterpret_cast<unsigned char*>(result + plain_size)};
				delete[] result;
				result = nullptr;
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

			static auto decrypt(
				char const* cipher,
				const std::string & key,
				const std::string & iv,
				size_t cipher_len,
				RijndaelMode mode
			) -> std::vector<unsigned char>
			{			
				auto rijndael = std::make_shared<Sen::Kernel::Dependencies::Rijndael::CRijndael>();
				rijndael->MakeKey(key.c_str(), iv.c_str(), static_cast<int>(key.size()), static_cast<int>(iv.size()));
				auto result = new char[cipher_len];
				rijndael->Decrypt(cipher, result, cipher_len, mode);
				auto m_result = std::vector<unsigned char>{reinterpret_cast<unsigned char*>(result), reinterpret_cast<unsigned char*>(result + cipher_len)};
				delete[] result;
				result = nullptr;
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

			static auto encrypt_fs(
				const std::string & source,
				const std::string & destination,
				const std::string key,
				const std::string iv,
				RijndaelMode mode
			) -> void
			{
				auto source_data = FileSystem::readBinary<char>(source);
				FileSystem::writeBinary<unsigned char>(destination, Rijndael::encrypt(source_data.data(), key, iv, source_data.size(), mode));
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

			static auto decrypt_fs(
				const std::string & source,
				const std::string & destination,
				const std::string key,
				const std::string iv,
				RijndaelMode mode
			) -> void
			{
				auto source_data = FileSystem::readBinary<char>(source);
				FileSystem::writeBinary<unsigned char>(destination, Rijndael::decrypt(source_data.data(), key, iv, source_data.size(), mode));
				return;
			}

	};
}