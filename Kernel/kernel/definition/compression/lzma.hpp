#pragma once

#include "kernel/definition/library.hpp"
#include "kernel/definition/macro.hpp"
#include "kernel/definition/filesystem/common.hpp"

namespace Sen::Kernel::Definition::Compression {


	class Lzma {

		public:

			/**
			 * Lzma compression level
			*/

			enum Level
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
				LEVEL_9
			};

			/**
			 * data: data stream
			 * level: compression level
			*/

			static auto compress(
				const std::vector<unsigned char> &data,
				const Level &level
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
				try_assert(ret != SZ_OK, "lzma compression failed");
				result.resize(destination_length);
				return result;
			}

			/**
			 * data: data stream
			 * size: actual size
			 * return: uncompressed data
			*/

			static auto uncompress(
				const std::vector<unsigned char> &data,
				const size_t &actual_size
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
				try_assert(ret != SZ_OK, "lzma uncomress failed");
				result.resize(destination_length);
				return result;
			}

			/**
			 * file in: input file
			 * file out: output file
			 * return: compressed file
			*/

			static auto compress_fs(
				const string &fileIn,
				const string &fileOut
			) -> void
			{
				auto dataStream = FileSystem::readBinary<unsigned char>(fileIn);
				auto compressedData = Lzma::compress(dataStream, Lzma::Level::LEVEL_9);
				FileSystem::writeBinary<unsigned char>(fileOut, compressedData);
				return;
			}

			/**
			 * file in: input file
			 * file out: output file
			 * size: actual size
			 * return: uncompressed file
			*/

			static auto uncompress_fs(
				const string &fileIn,
				const string &fileOut,
				const size_t &actual_size
			) -> void
			{
				auto dataStream = FileSystem::readBinary<unsigned char>(fileIn);
				auto uncompressedData = Lzma::uncompress(dataStream, actual_size);
				FileSystem::writeBinary<unsigned char>(fileOut, uncompressedData);
				return;
			}

	};

}