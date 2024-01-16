#pragma once

#include "kernel/definition/library.hpp"
#include "kernel/definition/macro.hpp"
#include "kernel/definition/basic/filesystem.hpp"

namespace Sen::Kernel::Definition::Compression {

	class Zlib {

		private:

			// zlib compression check

			inline static constexpr auto Z_COMPRESS_END = 0;

			// zlib uncompression check

			inline static constexpr auto Z_UNCOMPRESS_END = 0;

			// chunk

			inline static constexpr auto CHUNK = 32768;

		public:

			/**
			 * Constructor
			*/

			explicit Zlib(

			) = default;

			/**
			 * Destructor
			*/

			~Zlib(

			) = default;
			

			/**
			 * Zlib compression level supported by zlib.h
			*/

			enum Level
			{
				DEFAULT = -1,
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
			 * data: the binary data to compress
			 * level: zlib compression level
			 * return: the compressed data
			 */

			inline static auto compress(
				const std::vector<unsigned char> &data,
				const Level &level
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
			
			inline static auto uncompress(
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
				} while (zlib_init.avail_out == Zlib::Z_UNCOMPRESS_END);
				inflateEnd(&zlib_init);
				return result;
			}

			/**
			 * data: the binary data to compress
			 * level: zlib level to compress, should use ZlibLevel::LEVEL_9 for BEST COMPRESSION
			 * return: the compressed data
			 */


			inline static auto compress_deflate(
				const std::vector<unsigned char> &data,
				const Level &level
			) ->  std::vector<unsigned char>
			{
				auto zlib_outdata = std::vector<unsigned char>(static_cast<size_t>(compressBound(static_cast<uLong>(data.size()))));
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
			 * data: data stream
			 * level: zlib level
			 * return: compressed data
			*/

			inline static auto compress_gzip(
				const std::vector<unsigned char> &data,
				const Level &level
			) -> std::vector<unsigned char>
			{
				auto zlib_init = z_stream{
					.zalloc = Z_NULL,
					.zfree = Z_NULL,
					.opaque = Z_NULL
				};
				zlib_init.avail_in = static_cast<uInt>(data.size());
				zlib_init.next_in = const_cast<Bytef *>(data.data());
				deflateInit2(&zlib_init, level, Z_DEFLATED, 15 | 16, 8, Z_DEFAULT_STRATEGY);
				auto result = std::vector<unsigned char>();
				unsigned char out_chunk[Zlib::CHUNK];
				do {
					zlib_init.avail_out = sizeof(out_chunk);
					zlib_init.next_out = out_chunk;
					auto ret = deflate(&zlib_init, Z_FINISH);
					assert(ret != Z_STREAM_ERROR);
					result.insert(result.end(), out_chunk, out_chunk + sizeof(out_chunk) - zlib_init.avail_out);
				} while (zlib_init.avail_out == Zlib::Z_COMPRESS_END);
				deflateEnd(&zlib_init);
				return result;
			}

			/**
			 * data: data stream
			 * result: output stream
			*/

			inline static auto uncompress_gzip(
				const std::vector<unsigned char> &data
			) -> std::vector<unsigned char>
			{
				auto zlib_init = z_stream{
					.zalloc = Z_NULL,
					.zfree = Z_NULL,
					.opaque = Z_NULL
				};
				zlib_init.avail_in = 0;
				zlib_init.next_in = Z_NULL;
				if (inflateInit2(&zlib_init, 16 + MAX_WBITS) != Z_OK){
					throw Exception("Uncompress initialize failed");
				}
				zlib_init.avail_in = static_cast<uInt>(data.size());
				zlib_init.next_in = const_cast<Bytef *>(reinterpret_cast<const Bytef *>(data.data()));
				auto result = std::vector<unsigned char>();
				do {
					unsigned char out_chunk[32768];
					zlib_init.avail_out = sizeof(out_chunk);
					zlib_init.next_out = reinterpret_cast<Bytef *>(out_chunk);
					auto ret = inflate(&zlib_init, 0);
					if(result.size() < zlib_init.total_out){
						result.insert(result.end(), out_chunk, out_chunk + sizeof(out_chunk) - zlib_init.avail_out);
					}
					if(ret == Z_STREAM_END){
						break;
					}
					if(ret != Z_OK){
						inflateEnd(&zlib_init);
						throw Exception("Uncompress failed"); 
					}
				} while (zlib_init.avail_out == Zlib::Z_UNCOMPRESS_END);
				inflateEnd(&zlib_init);
				return result;
			}

			/**
			 * filePath: input file
			 * fileOut: output file
			 * level: zlib level
			 * return: compress file
			*/

			inline static auto compress_fs(
				const string &filePath,
				const string &fileOut,
				const Level &level
			) -> void
			{
				auto data = FileSystem::read_binary<unsigned char>(filePath);
				auto compressedData = Zlib::compress_deflate(data, level);
				FileSystem::write_binary<unsigned char>(fileOut, compressedData);
				return;
			}

			/**
			 * filePath: input file
			 * fileOut: output file
			 * return: uncompress file
			*/

			inline static auto uncompress_fs(
				const string &fileIn,
				const string &fileOut
			) -> void
			{
				auto data = FileSystem::read_binary<unsigned char>(fileIn);
				auto uncompressedData = Zlib::uncompress(data);
				if(uncompressedData.empty()){
					throw Exception(fmt::format("The specific file cannot be uncompressed: {}", fileIn));
				}
				FileSystem::write_binary<unsigned char>(fileOut, uncompressedData);
				return;
			}

			/**
			 * file in: file path
			 * file out: output path
			 * return: compressed gzip file
			*/

			inline static auto compress_gzip_fs(
				const string &fileIn,
				const string &fileOut
			) -> void
			{
				auto data = FileSystem::read_binary<unsigned char>(fileIn);
				auto compressed_data = Zlib::compress_gzip(data, Zlib::Level::DEFAULT);
				FileSystem::write_binary<unsigned char>(fileOut, compressed_data);
				return;
			}

			/**
			 * filePath: input file
			 * fileOut: output file
			 * level: zlib level
			 * return: compress file
			*/

			inline static auto compress_deflate_fs(
				const string &filePath,
				const string &fileOut
			) -> void
			{
				auto data = FileSystem::read_binary<unsigned char>(filePath);
				auto compressedData = Zlib::compress_deflate(data, Zlib::Level::DEFAULT);
				FileSystem::write_binary<unsigned char>(fileOut, compressedData);
				return;
			}

			/**
			 * fileIn: file path
			 * file out: output path
			 * return: uncompressed file
			*/

			inline static auto uncompress_gzip_fs(
				const string & fileIn,
				const string & fileOut
			) -> void
			{
				auto data = FileSystem::read_binary<unsigned char>(fileIn);
				auto compressed_data = Zlib::uncompress_gzip(data);
				FileSystem::write_binary<unsigned char>(fileOut, compressed_data);
				return;
			}

	};
}