#pragma once

#include "kernel/definition/utility.hpp"
#include "kernel/support/popcap/zlib/common.hpp"

namespace Sen::Kernel::Support::PopCap::Zlib {

	// use namespace

	using namespace Definition;

	/**
	 * Virtual class for inherit
	*/

	struct VirtualC : PopCap::Zlib::Common {
		public:

			/**
			 * Must override this method
			 * 
			*/

			virtual auto compress(
				const std::vector<unsigned char> & source
			) -> std::vector<unsigned char> = 0;

			// constructor

			VirtualC(

			) = default;

			// destructor

			~VirtualC(

			) = default;
	};

	struct Compress : VirtualC {

		private:

			// the use of the 64 bit variant

			bool use_64_bit_variant;

		public:

			// constructor

			Compress(

			) = default;

			// destructor

			~Compress(

			) = default;

			// constructor

			Compress(
				bool use_64_bit_variant
			) : use_64_bit_variant(use_64_bit_variant)
			{

			}

			/**
			 * source: the binary source
			 * return: the compressed source
			*/

			auto compress(
				const std::vector<unsigned char> & source
			) -> std::vector<unsigned char> override final
			{
				auto sen = Buffer::Vector{};
				// magic
				sen.writeUint32LE(static_cast<uint32_t>(thiz.magic));
				if(thiz.use_64_bit_variant){
					// 4 blank byte
					sen.writeUint32LE(static_cast<uint32_t>(0x00));
				}
				// size
				sen.writeUint32LE(static_cast<uint32_t>(source.size()));
				if(thiz.use_64_bit_variant){
					// 4 blank byte
					sen.writeUint32LE(static_cast<uint32_t>(0x00));
				}
				// compressed zlib part
				sen.append<unsigned char>(Compression::Zlib::compress(source, level));
				return sen.get();
			}

			/**
			 * Compress a file into popcap zlib
			 * @param source: source file
			 * @param destination: destination file
			 * @param use_64_bit_variant: use the 64-bit variant
			 * @return: the compressed file
			*/

			static auto compress_fs(
				const std::string & source,
				const std::string & destination,
				bool use_64_bit_variant
			) -> void
			{
				auto* sen = new PopCap::Zlib::Compress{use_64_bit_variant};
				FileSystem::writeBinary<unsigned char>(destination, sen->compress(FileSystem::readBinary<unsigned char>(source)));
				delete sen;
				sen = nullptr;
				return;
			}

	};

}