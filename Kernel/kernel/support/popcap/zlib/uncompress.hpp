#pragma once

#include "kernel/definition/utility.hpp"
#include "kernel/support/popcap/zlib/common.hpp"

namespace Sen::Kernel::Support::PopCap::Zlib {

	// use namespace

	using namespace Definition;

	// virtual to inherit

	struct Virtual : PopCap::Zlib::Common {

		public:

			// must override this method

			virtual auto uncompress(
				const std::vector<unsigned char> & source 
			) -> std::vector<unsigned char> = 0;

	};

	/**
	 * Uncompress struct
	*/

	struct Uncompress : Virtual {

		private:

			// the use of the 64 bit variant

			bool use_64_bit_variant;

		public:

			// default constructor

			Uncompress(

			) = default;

			// should use this

			Uncompress(
				bool use_64_bit_variant
			) : use_64_bit_variant(use_64_bit_variant)
			{

			}

			// destructor

			~Uncompress(

			) = default;

			// uncompress method

			/**
			 * source: full buffer source
			 * return: uncompressed buffer source
			*/

			auto uncompress(
				const std::vector<unsigned char> & source 
			) -> std::vector<unsigned char> override final
			{
				auto* sen = new Buffer::Vector{source};
				auto magic = sen->readUint32LE();
				try_assert(magic == static_cast<uint32_t>(Uncompress::magic), fmt::format("Mismatch zlib magic, should begin with: 0x{:X}", Uncompress::magic));
				auto cut_offset = static_cast<size_t>(8);
				if(thiz.use_64_bit_variant){
					cut_offset += 8;
					sen->readUint32LE();
					sen->readUint32LE();
				}
				auto result = Compression::Zlib::uncompress(sen->get_raw(cut_offset, sen->size()));
				delete sen;
				return result;
			}

			/**
			 * @param source: source file
			 * @param destination: destination file
			 * @param use_64_bit_variant: use the 64-bit variant (most popcap game doesn't use this)
			 * @return: the uncompressed data
			 */

			static auto uncompress_fs(
				const std::string & source,
				const std::string & destination,
				bool use_64_bit_variant
			) -> void
			{
				auto *uncompress_zlib = new PopCap::Zlib::Uncompress{use_64_bit_variant};
				auto uncompressed_data = uncompress_zlib->uncompress(FileSystem::readBinary<unsigned char>(source));
				FileSystem::writeBinary<unsigned char>(destination, uncompressed_data);
				delete uncompress_zlib;
				uncompress_zlib = nullptr;
				return;
			}
	};
}