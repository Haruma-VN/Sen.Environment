#pragma once

#include "kernel/definition/library.hpp"
#include "kernel/definition/macro.hpp"
#include "kernel/definition/assert.hpp"
#include "kernel/dependencies/Rijndael.hpp"

namespace Sen::Kernel::Definition::Encryption 
{

	// Rijndael Struct

	struct Rijndael {

		private:
			using CRijndael = Sen::Kernel::Dependencies::Rijndael::CRijndael;

		protected:

			template <typename T> requires std::is_integral<T>::value
			inline static auto constexpr is_padded_size(
				T size, 
				T block_size
			) noexcept -> bool
			{
    			return size % block_size == 0;
			}

			template <typename T> requires std::is_integral<T>::value
			inline static auto constexpr is_valid_block_size(
				T block_size
			) noexcept -> bool {
    			return block_size == 128 || block_size == 160 || block_size == 192 || block_size == 224 || block_size == 256;
			}

		public:

			Rijndael(

			) = default;

			~Rijndael(

			) = default;

			// Rijndael Mode

			enum class Mode : uint8_t {
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
				static_assert(mode == Mode::CBC || mode == Mode::CFB || mode == Mode::ECB, "mode is invalid, expected cbc, cfb or ecb");
				assert_conditional(is_valid_block_size<std::size_t>(key.size() * 8), fmt::format("{}", Language::get("rijndael.key_size_is_not_valid")), "encrypt");
				assert_conditional(is_valid_block_size<std::size_t>(iv.size() * 8), fmt::format("{}", Language::get("rijndael.iv_size_is_not_valid")), "encrypt");
				assert_conditional(is_padded_size<std::size_t>(key.size(), 32), fmt::format("{}", Language::get("rijndael.key_is_not_padded")), "encrypt");
				assert_conditional(plain_size != 0, fmt::format("{}", Language::get("rijndael.encrypt.plain_is_empty")), "encrypt");
				auto rijndael = std::make_shared<CRijndael>();
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
				static_assert(mode == Mode::CBC || mode == Mode::CFB || mode == Mode::ECB, "mode is invalid, expected cbc, cfb or ecb");
				assert_conditional(is_valid_block_size<std::size_t>(key.size() * 8), fmt::format("{}", Language::get("rijndael.key_size_is_not_valid")), "decrypt");
				assert_conditional(is_valid_block_size<std::size_t>(iv.size() * 8), fmt::format("{}", Language::get("rijndael.iv_size_is_not_valid")), "decrypt");
				assert_conditional(is_padded_size<std::size_t>(key.size(), 32), fmt::format("{}", Language::get("rijndael.key_is_not_padded")), "decrypt");
				assert_conditional(cipher_len != 0, fmt::format("{}", Language::get("rijndael.decrypt.cipher_is_empty")), "decrypt");
				assert_conditional((iv.size() - ((cipher_len + iv.size() - 1) % iv.size() + 1)) == 0, fmt::format("{}", Language::get("rijndael.decrypt.cipher_size_is_invalid_to_decrypt")), "decrypt");
				auto rijndael = std::make_shared<CRijndael>();
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