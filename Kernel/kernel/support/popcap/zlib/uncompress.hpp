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

			explicit Uncompress(

			) = default;

			// should use this

			explicit constexpr Uncompress(
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

			inline auto uncompress(
				const std::vector<unsigned char> & source 
			) -> std::vector<unsigned char> override final
			{
				auto sen = DataStreamView{source};
				auto magic = sen.readUint32();
				assert_conditional(magic == static_cast<uint32_t>(Uncompress::magic), fmt::format("{}: 0x{:X}", Language::get("popcap.zlib.uncompress.mismatch_zlib_magic"), Uncompress::magic), "uncompress");
				auto cut_offset = static_cast<size_t>(8);
				if(thiz.use_64_bit_variant){
					cut_offset += 8;
					sen.readUint32();
					sen.readUint32();
				}
				auto result = Compression::Zlib::uncompress(sen.getBytes(cut_offset, sen.size()));
				return result;
			}

			/**
			 * @param source: source file
			 * @param destination: destination file
			 * @param use_64_bit_variant: use the 64-bit variant (most popcap game doesn't use this)
			 * @return: the uncompressed data
			 */

			inline static auto uncompress_fs(
				std::string_view source,
				std::string_view destination,
				bool use_64_bit_variant
			) -> void
			{
				auto uncompress_zlib = std::make_unique<PopCap::Zlib::Uncompress>(use_64_bit_variant);
				auto uncompressed_data = uncompress_zlib->uncompress(FileSystem::read_binary<unsigned char>(source));
				FileSystem::write_binary<unsigned char>(destination, uncompressed_data);
				return;
			}
	};
}