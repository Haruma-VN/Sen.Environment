#pragma once

#include "kernel/definition/library.hpp"
#include "kernel/definition/macro.hpp"
#include "kernel/definition/basic/filesystem.hpp"

namespace Sen::Kernel::Definition::Compression {

	/**
	 * Bzip2 kernel
	*/

	struct Bzip2 {

		private:

			// CHUNK

			inline static auto constexpr CHUNK = 4096;

			// BLOCK SIZE

			inline static auto constexpr BLOCK_SIZE = 9;

			// WORK FACTOR

			inline static auto constexpr WORK_FACTOR = 30;

		public:

			/**
			 * input: the unsigned char stream
			 * block_size: block size
			 * work_factor: work factor
			 * return: compressed bzip2 stream
			*/

			static auto compress(
				const std::vector<unsigned char>& input,
				int block_size,
				int work_factor
			) -> std::vector<unsigned char> 
			{
				auto bzerror = int{};
				auto strm = bz_stream{};
				strm.bzalloc = null;
				strm.bzfree = null;
				strm.opaque = null;
				BZ2_bzCompressInit(&strm, block_size, 0, work_factor);
				strm.next_in = (char*)(input.data());
				strm.avail_in = input.size();
				auto result = std::vector<unsigned char>{};
				unsigned char outbuffer[CHUNK];
				do {
					strm.next_out = (char*)outbuffer;
					strm.avail_out = sizeof(outbuffer);
					bzerror = BZ2_bzCompress(&strm, BZ_FINISH);
					if (bzerror < 0) {
						BZ2_bzCompressEnd(&strm);
						throw std::runtime_error("bzip2 compression failed");
					}
					result.insert(result.end(), outbuffer, outbuffer + sizeof(outbuffer) - strm.avail_out);
				} while (bzerror != BZ_STREAM_END);
				BZ2_bzCompressEnd(&strm);
				return result;
		}

		/**
		 * input: stream
		 * return: result after uncompress
		*/

		static auto uncompress(
			const std::vector<unsigned char> & input
		) -> std::vector<unsigned char> 
		{
			auto bzerror = int{};
			auto strm = bz_stream{};
			strm.bzalloc = NULL;
			strm.bzfree = NULL;
			strm.opaque = NULL;
			BZ2_bzDecompressInit(&strm, 0, 0);
			strm.next_in = (char*)input.data();
			strm.avail_in = input.size();
			auto result = std::vector<unsigned char>{};
			unsigned char outbuffer[4096];
			do {
				strm.next_out = (char*)outbuffer;
				strm.avail_out = sizeof(outbuffer);
				bzerror = BZ2_bzDecompress(&strm);
				if (bzerror < 0) {
					BZ2_bzDecompressEnd(&strm);
					throw std::runtime_error("bzip2 decompression failed");
				}
				result.insert(result.end(), outbuffer, outbuffer + sizeof(outbuffer) - strm.avail_out);
			} while (bzerror != BZ_STREAM_END);
			BZ2_bzDecompressEnd(&strm);
			return result;
		}

		/**
		 * @param source: source file
		 * @param destination: output file
		 * @return: compressed file
		*/

		static auto compress_fs(
			const std::string & source,
			const std::string & destination
		) -> void
		{
			FileSystem::writeBinary<unsigned char>(destination, Bzip2::compress(FileSystem::readBinary<unsigned char>(source), Bzip2::BLOCK_SIZE, Bzip2::WORK_FACTOR));
			return;
		}

		/**
		 * @param source: source file
		 * @param destination: destination file
		 * @return: uncompressed file
		*/

		static auto uncompress_fs(
			const std::string & source,
			const std::string & destination
		) -> void
		{
			FileSystem::writeBinary<unsigned char>(destination, Bzip2::uncompress(FileSystem::readBinary<unsigned char>(source)));
			return;
		}
	};
}