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

	template <auto UseVariant>
	struct Compress : VirtualC {

		static_assert(sizeof(UseVariant) == sizeof(bool));

		static_assert(UseVariant == true or UseVariant == false);

	public:
		// constructor

		Compress(

			) = default;

		// destructor

		~Compress(

			) = default;

		/**
		 * source: the binary source
		 * return: the compressed source
		 */

		inline auto compress(
			const std::vector<unsigned char> &source) -> std::vector<unsigned char> override final
		{
			auto sen = DataStreamView{};
			// magic
			sen.writeUint32(static_cast<uint32_t>(thiz.magic));
			if constexpr (UseVariant)
			{
				// 4 blank byte
				sen.writeUint32(static_cast<uint32_t>(0x00));
			}
			// size
			sen.writeUint32(static_cast<uint32_t>(source.size()));
			if constexpr (UseVariant)
			{
				// 4 blank byte
				sen.writeUint32(static_cast<uint32_t>(0x00));
			}
			// compressed zlib part
			sen.writeBytes(Compression::Zlib::compress<Compression::Zlib::Level::LEVEL_9>(source));
			return sen.getBytes(0, sen.size());
			}

			/**
			 * Compress a file into popcap zlib
			 * @param source: source file
			 * @param destination: destination file
			 * @param use_64_bit_variant: use the 64-bit variant
			 * @return: the compressed file
			*/

			inline static auto compress_fs(
				std::string_view source,
				std::string_view destination
			) -> void
			{
				auto sen = std::make_unique<PopCap::Zlib::Compress<UseVariant>>();
				FileSystem::write_binary<unsigned char>(destination, sen->compress(FileSystem::read_binary<unsigned char>(source)));
				return;
			}

	};

}