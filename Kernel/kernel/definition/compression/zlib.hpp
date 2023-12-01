#pragma once

#include "kernel/definition/library.hpp"
#include "kernel/definition/macro.hpp"
#include "kernel/definition/filesystem/common.hpp"

namespace Sen::Kernel::Definition::Compression {

	/**
	 * Zlib compression level supported by zlib.h
	*/

	enum ZlibLevel
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

	class Zlib {

		private:

			// zlib compression check

			static constexpr auto Z_COMPRESS_END = 0;

			// chunk

			static constexpr auto CHUNK = 32768;

		public:
			/**
			 * data: the binary data to compress
			 * level: zlib compression level
			 * return: the compressed data
			 */

			static auto compress(
				const std::vector<unsigned char> &data,
				const ZlibLevel &level
			) -> std::vector<unsigned char> 
			{
				auto destLen = compressBound(data.size());
				auto zlib_outdata = std::vector<unsigned char>(static_cast<size_t>(destLen));
				auto ret = compress2(zlib_outdata.data(), &destLen, data.data(), data.size(), static_cast<int>(level));
				assert(ret == Z_OK);
				zlib_outdata.resize(destLen);
				return zlib_outdata;
			}

			/**
			 * data: the binary data to uncompress
			 * return: the uncompressed data
			 */
			
			static auto uncompress(
				const std::vector<unsigned char> &data
			) -> std::vector<unsigned char>
			{
				auto zlib_init = z_stream{
					.zalloc = Z_NULL,
					.zfree = Z_NULL,
					.opaque = Z_NULL
				};
				zlib_init.avail_in = static_cast<uInt>(data.size());
				zlib_init.next_in = const_cast<Bytef*>(data.data());
				inflateInit(&zlib_init);
				auto result = std::vector<unsigned char>{};
				unsigned char out_chunk[Zlib::CHUNK];
				do {
					zlib_init.avail_out = sizeof(out_chunk);
					zlib_init.next_out = out_chunk;
					auto ret = inflate(&zlib_init, Z_NO_FLUSH);
					assert(ret != Z_STREAM_ERROR);
					switch(ret){
						case Z_NEED_DICT:
							ret = Z_DATA_ERROR;
						case Z_DATA_ERROR:
						case Z_MEM_ERROR:
							inflateEnd(&zlib_init);
							return std::vector<unsigned char>();
					}
					result.insert(result.end(), out_chunk, out_chunk + sizeof(out_chunk) - zlib_init.avail_out);
				} while (zlib_init.avail_out == Zlib::Z_COMPRESS_END);
				inflateEnd(&zlib_init);
				return result;
			}

			/**
			 * data: the binary data to compress
			 * level: zlib level to compress, should use ZlibLevel::LEVEL_9 for BEST COMPRESSION
			 * return: the compressed data
			 */


			static auto compress_deflate(
				const std::vector<unsigned char> &data,
				const ZlibLevel &level
			) ->  std::vector<unsigned char>
			{
				auto zlib_outdata = std::vector<unsigned char>(compressBound(static_cast<uLong>(data.size())));
				auto zlib_init = z_stream{
					.zalloc = Z_NULL,
					.zfree = Z_NULL,
					.opaque = Z_NULL
				};
				zlib_init.avail_in = static_cast<uInt>(data.size());
				zlib_init.next_in = const_cast<unsigned char*>(data.data());
				auto ret = deflateInit(&zlib_init, static_cast<int>(level));
				assert(ret == Z_OK);
				zlib_init.avail_out = static_cast<uInt>(zlib_outdata.size());
				zlib_init.next_out = const_cast<Bytef*>(zlib_outdata.data());
				while (zlib_init.avail_in != Zlib::Z_COMPRESS_END){
					deflate(&zlib_init, Z_NO_FLUSH);
				}
				auto deflate_res = Z_OK;
				while (deflate_res == Z_OK){
					if(zlib_init.avail_out == Zlib::Z_COMPRESS_END){
						zlib_outdata.insert(zlib_outdata.end(), zlib_outdata.size());
						zlib_init.next_out = const_cast<Bytef*>(&zlib_outdata[zlib_outdata.size() / 2]);
						zlib_init.avail_out = static_cast<uInt>(zlib_outdata.size() / 2);
					}
					deflate_res = deflate(&zlib_init, Z_FINISH);
				}
				assert(deflate_res == Z_STREAM_END);
				zlib_outdata.resize(zlib_outdata.size() - zlib_init.avail_out);
				deflateEnd(&zlib_init);
				return zlib_outdata;
			}

			/**
			 * filePath: input file
			 * fileOut: output file
			 * level: zlib level
			 * return: compress file
			*/

			static auto compress_fs(
				const string &filePath,
				const string &fileOut,
				const ZlibLevel &level
			) -> void
			{
				auto data = FileSystem::readBinary<unsigned char>(filePath);
				auto compressedData = Zlib::compress_deflate(data, level);
				FileSystem::writeBinary<unsigned char>(fileOut, compressedData);
				return;
			}

			/**
			 * filePath: input file
			 * fileOut: output file
			 * return: uncompress file
			*/

			static auto uncompress_fs(
				const string &fileIn,
				const string &fileOut
			) -> void
			{
				auto data = FileSystem::readBinary<unsigned char>(fileIn);
				auto uncompressedData = Zlib::uncompress(data);
				FileSystem::writeBinary<unsigned char>(fileOut, uncompressedData);
				return;
			}

	};
}