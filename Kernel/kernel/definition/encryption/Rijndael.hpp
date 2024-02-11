#pragma once

#include "kernel/definition/library.hpp"
#include "kernel/definition/macro.hpp"
#include "kernel/definition/assert.hpp"
#include "kernel/dependencies/Rijndael.hpp"

namespace Sen::Kernel::Definition::Encryption 
{

	// Rijndael Struct

	struct Rijndael {

		public:

			Rijndael(

			) = default;

			~Rijndael(

			) = default;

			// Rijndael Mode

			enum class Mode {
				ECB,
				CBC,
				CFB,
			};

			/**
			 * @param plain: plain buffer
			 * @param key: key
			 * @param iv: iv
			 * @param plain_size: buffer size
			 * @param mode: rijndael mode
			 * @returns: encrypted buffer
			*/
			template <typename T, auto mode> requires std::is_integral<T>::value
			inline static auto encrypt(
				char const* plain,
				std::string_view key,
				std::string_view iv,
				T plain_size
			) -> std::vector<unsigned char>
			{			
				auto rijndael = std::make_shared<Sen::Kernel::Dependencies::Rijndael::CRijndael>();
				rijndael->MakeKey(key.data(), iv.data(), static_cast<int>(key.size()), static_cast<int>(iv.size()));
				auto result = std::make_unique<char[]>(plain_size);
				rijndael->Encrypt(plain, result.get(), plain_size, static_cast<int>(mode));
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
			template <typename T, auto mode> requires std::is_integral<T>::value
			inline static auto decrypt(
				char const* cipher,
				std::string_view key,
				std::string_view iv,
				T cipher_len
			) -> std::vector<unsigned char>
			{			
				auto rijndael = std::make_shared<Sen::Kernel::Dependencies::Rijndael::CRijndael>();
				rijndael->MakeKey(key.data(), iv.data(), static_cast<int>(key.size()), static_cast<int>(iv.size()));
				auto result = std::make_unique<char[]>(cipher_len);
				rijndael->Decrypt(cipher, result.get(), cipher_len, static_cast<int>(mode));
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
				std::string_view source,
				std::string_view destination,
				std::string_view key,
				std::string_view iv,
				Mode mode
			) -> void
			{
				auto source_data = FileSystem::read_binary<char>(source);
				switch (mode) {
					case Mode::CBC:{
						FileSystem::write_binary<unsigned char>(destination, Rijndael::encrypt<std::uint64_t, Mode::CBC>(source_data.data(), key, iv, source_data.size()));
						break;
					}
					case Mode::CFB:{
						FileSystem::write_binary<unsigned char>(destination, Rijndael::encrypt<std::uint64_t, Mode::CFB>(source_data.data(), key, iv, source_data.size()));
						break;
					}
					case Mode::ECB:{
						FileSystem::write_binary<unsigned char>(destination, Rijndael::encrypt<std::uint64_t, Mode::ECB>(source_data.data(), key, iv, source_data.size()));
						break;
					}
				}
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
				std::string_view source,
				std::string_view destination,
				std::string_view key,
				std::string_view iv,
				Mode mode
			) -> void
			{
				auto source_data = FileSystem::read_binary<char>(source);
				switch (mode) {
					case Mode::CBC:{
						FileSystem::write_binary<unsigned char>(destination, Rijndael::decrypt<std::uint64_t, Mode::CBC>(source_data.data(), key, iv, source_data.size()));
						break;
					}
					case Mode::CFB:{
						FileSystem::write_binary<unsigned char>(destination, Rijndael::decrypt<std::uint64_t, Mode::CFB>(source_data.data(), key, iv, source_data.size()));
						break;
					}
					case Mode::ECB:{
						FileSystem::write_binary<unsigned char>(destination, Rijndael::decrypt<std::uint64_t, Mode::ECB>(source_data.data(), key, iv, source_data.size()));
						break;
					}
				}
				return;
			}

	};
}