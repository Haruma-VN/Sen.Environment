#pragma once

#include "kernel/definition/library.hpp"
#include "kernel/definition/macro.hpp"
#include "kernel/definition/basic/filesystem.hpp"

namespace Sen::Kernel::Definition::Compression {

	/**
	 * Lzma compression and uncompression
	*/

	class Lzma {

		public:

			/**
			 * Constructor
			*/

			explicit Lzma(

			) = default;

			/**
			 * Destructor
			*/

			~Lzma(

			) = default;

			/**
			 * Lzma compression level
			*/

			enum class Level : std::uint8_t
			{
				LEVEL_0,
				LEVEL_1,
				LEVEL_2,
				LEVEL_3,
				LEVEL_4,
				LEVEL_5,
				LEVEL_6,
				LEVEL_7,
				LEVEL_8,
				LEVEL_9,
			};

			/**
			 * data: data stream
			 * level: compression level
			*/
			template <auto level>
			inline static auto compress(
				const std::vector<unsigned char> &data
			) -> std::vector<unsigned char>
			{
				auto destination_length = static_cast<size_t>(data.size() * 1.1 + 1024);
				auto propsSize = static_cast<size_t>(LZMA_PROPS_SIZE);
				auto result = std::vector<unsigned char>(destination_length);
				auto props = std::vector<unsigned char>(propsSize);
				auto ret = LzmaCompress(
					&result[0],
					&destination_length,
					&data[0],
					data.size(),
					&props[0],
					&propsSize,
					static_cast<int>(level),
					0,
					-1,
					-1,
					-1,
					-1,
					-1
				);
				assert_conditional(ret != SZ_OK, fmt::format("{}", Kernel::Language::get("lzma.compress.failed")), "compress");
				result.resize(destination_length);
				return result;
			}

			/**
			 * data: data stream
			 * size: actual size
			 * return: uncompressed data
			*/
			template <typename T> requires std::is_integral<T>::value
			inline static auto uncompress(
				const std::vector<unsigned char> &data,
				T actual_size
			) -> std::vector<unsigned char>
			{
				auto result = std::vector<unsigned char>(actual_size);
				auto destination_length = actual_size;
				auto source_length = data.size() - LZMA_PROPS_SIZE;
				auto ret = LzmaUncompress(
					&result[0],
					&destination_length,
					&data[LZMA_PROPS_SIZE],
					&source_length,
					&data[0],
					LZMA_PROPS_SIZE
				);
				assert_conditional(ret != SZ_OK, fmt::format("{}", Kernel::Language::get("lzma.uncompress.failed")), "compress");
				result.resize(destination_length);
				return result;
			}

			/**
			 * source: input file
			 * destination: output file
			 * return: compressed file
			*/
			inline static auto compress_fs(
				std::string_view source,
				std::string_view destination,
				Level level
			) -> void
			{
				auto data = FileSystem::read_binary<unsigned char>(source);
				switch(level){
					case Level::LEVEL_0:{
						FileSystem::write_binary<unsigned char>(destination, Lzma::compress<Level::LEVEL_0>(data));
						break;
					}
					case Level::LEVEL_1:{
						FileSystem::write_binary<unsigned char>(destination, Lzma::compress<Level::LEVEL_1>(data));
						break;
					}
					case Level::LEVEL_2:{
						FileSystem::write_binary<unsigned char>(destination, Lzma::compress<Level::LEVEL_2>(data));
						break;
					}
					case Level::LEVEL_3:{
						FileSystem::write_binary<unsigned char>(destination, Lzma::compress<Level::LEVEL_3>(data));
						break;
					}
					case Level::LEVEL_4:{
						FileSystem::write_binary<unsigned char>(destination, Lzma::compress<Level::LEVEL_4>(data));
						break;
					}
					case Level::LEVEL_5:{
						FileSystem::write_binary<unsigned char>(destination, Lzma::compress<Level::LEVEL_5>(data));
						break;
					}
					case Level::LEVEL_6:{
						FileSystem::write_binary<unsigned char>(destination, Lzma::compress<Level::LEVEL_6>(data));
						break;
					}
					case Level::LEVEL_7:{
						FileSystem::write_binary<unsigned char>(destination, Lzma::compress<Level::LEVEL_7>(data));
						break;
					}
					case Level::LEVEL_8:{
						FileSystem::write_binary<unsigned char>(destination, Lzma::compress<Level::LEVEL_8>(data));
						break;
					}
					case Level::LEVEL_9:{
						FileSystem::write_binary<unsigned char>(destination, Lzma::compress<Level::LEVEL_9>(data));
						break;
					}
					default:{
						throw Exception(fmt::format("{}", Kernel::Language::get("lzma.compress.no_level_found")), std::source_location::current(), "compress_fs");
					}
				}
				return;
			}

			/**
			 * source: input file
			 * destination: output file
			 * size: actual size
			 * return: uncompressed file
			*/
			template <typename T> requires std::is_integral<T>::value
			inline static auto uncompress_fs(
				std::string_view source,
				std::string_view destination,
				T actual_size
			) -> void
			{
				auto data = FileSystem::read_binary<unsigned char>(source);
				auto uncompressed_data = Lzma::uncompress<T>(data, actual_size);
				FileSystem::write_binary<unsigned char>(destination, uncompressed_data);
				return;
			}

	};

}