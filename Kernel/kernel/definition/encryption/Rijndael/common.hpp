#pragma once

#include "kernel/definition/library.hpp"
#include "kernel/definition/macro.hpp"
#include "kernel/definition/assert.hpp"
#include "kernel/dependencies/Rijndael.hpp"

namespace Sen::Kernel::Encryption {

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

	struct RijndaelDataView {
		private:
			char *value;
			size_t length;

		public:

			RijndaelDataView(char* value, size_t length)
			{
				thiz.length = length;
				thiz.value = value;
			}

			inline auto getValue(

			) -> char*
			{
				return thiz.value;
			}

			inline auto size(

			) -> size_t
			{
				return thiz.length;
			}
	};

	// Rijndael Struct

	struct Rijndael {

		public:

			inline auto static encrypt(
				char const* plain,
				const string key,
				const string iv,
				const size_t plain_size,
				const RijndaelMode mode
			) -> RijndaelDataView
			{			
				auto* rijndael = new Dependencies::Rijndael::CRijndael{};
				rijndael->MakeKey(key.c_str(), iv.c_str(), static_cast<int>(key.size()), static_cast<int>(iv.size()));
				auto result = new char[plain_size];
				rijndael->Encrypt(plain, result, plain_size, mode);
				delete rijndael;
				return RijndaelDataView{result, plain_size};
			}

			inline auto static decrypt(
				char const* cipher,
				const string key,
				const string iv,
				const size_t cipher_len,
				const RijndaelMode mode
			) -> RijndaelDataView
			{			
				auto* rijndael = new Dependencies::Rijndael::CRijndael{};
				rijndael->MakeKey(key.c_str(), iv.c_str(), static_cast<int>(key.size()), static_cast<int>(iv.size()));
				auto result = new char[cipher_len];
				rijndael->Decrypt(cipher, result, cipher_len, mode);
				delete rijndael;
				return RijndaelDataView{result, cipher_len};
			}

	};
}