#pragma once

#include "kernel/definition/library.hpp"
#include "kernel/definition/macro.hpp"
#include "kernel/definition/assert.hpp"
#include "kernel/dependencies/Rijndael.hpp"

namespace Sen::Kernel::Encryption 
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

			static auto encrypt(
				char const* plain,
				const string key,
				const string iv,
				const size_t plain_size,
				const RijndaelMode mode
			) -> std::vector<unsigned char>
			{			
				auto* rijndael = new Dependencies::Rijndael::CRijndael{};
				rijndael->MakeKey(key.c_str(), iv.c_str(), static_cast<int>(key.size()), static_cast<int>(iv.size()));
				auto result = new char[plain_size];
				rijndael->Encrypt(plain, result, plain_size, mode);
				delete rijndael;
				auto m_result = std::vector<unsigned char>{reinterpret_cast<unsigned char*>(result), reinterpret_cast<unsigned char*>(result + plain_size)};
				delete[] result;
				result = nullptr;
				return m_result;
			}

			static auto decrypt(
				char const* cipher,
				const string key,
				const string iv,
				const size_t cipher_len,
				const RijndaelMode mode
			) -> std::vector<unsigned char>
			{			
				auto* rijndael = new Dependencies::Rijndael::CRijndael{};
				rijndael->MakeKey(key.c_str(), iv.c_str(), static_cast<int>(key.size()), static_cast<int>(iv.size()));
				auto result = new char[cipher_len];
				rijndael->Decrypt(cipher, result, cipher_len, mode);
				delete rijndael;
				auto m_result = std::vector<unsigned char>{reinterpret_cast<unsigned char*>(result), reinterpret_cast<unsigned char*>(result + cipher_len)};
				delete[] result;
				result = nullptr;
				return m_result;
			}

	};
}