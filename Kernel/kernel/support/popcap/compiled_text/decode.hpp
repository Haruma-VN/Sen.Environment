#pragma once

#include "kernel/definition/utility.hpp"
#include "kernel/support/popcap/zlib/uncompress.hpp"

namespace Sen::Kernel::Support::PopCap::CompiledText {

	/**
	 * Use Sen Buffer
	*/

	using namespace Sen::Kernel::Definition;

	/**
	 * Use base64
	*/

	using namespace Sen::Kernel::Definition::Encryption;

	/**
	 * Compiled Text decode struct
	*/

	using Zlib = Sen::Kernel::Support::PopCap::Zlib::Uncompress;

	/**
	 * Decode Struct
	*/

	struct Decode {

		private:

			/**
			 * Buffer handling
			*/

			SenBuffer sen;

			/**
			 * Key for compiled text
			*/

			std::string key;

			/**
			 * Iv for compiled text
			*/

			std::string iv;

			/**
			 * The use of 64bit variant
			*/

			bool use_64_bit_variant;

		public:

			/**
			 * Default constructor
			*/

			Decode(

			) = default;

			/**
			 * Default destructor
			*/

			~Decode(

			) = default;

			/**
			 * @param source: source file
			 * Constructor
			*/

			explicit Decode(
				const std::string & source,
				const std::string & key,
				const std::string & iv,
				bool use_64_bit_variant
			) : sen(source), key(key), iv(iv), use_64_bit_variant(use_64_bit_variant)
			{

			}

			explicit Decode(
				SenBuffer & it,
				const std::string & key,
				const std::string & iv,
				bool use_64_bit_variant
			) : sen(it), key(key), iv(iv), use_64_bit_variant(use_64_bit_variant)
			{

			}

			/**
			 * --------------
			 * Process method
			 * --------------
			*/

			auto process(

			) -> SenBuffer
			{
				auto buffer = SenBuffer{};
				auto decoded_base64 = SenBuffer::fromString(Base64::decode(thiz.sen.toString()));
				buffer.append<unsigned char>(Zlib{thiz.use_64_bit_variant}.uncompress(Rijndael::decrypt(reinterpret_cast<char *>(decoded_base64.getBytes(0x02, decoded_base64.size())), thiz.key, thiz.iv, decoded_base64.size() - 0x02, RijndaelMode::CBC)));
				return buffer;
			}

			/**
			 * @param source: source file
			 * @param destination: destination file
			 * @returns: decoded file
			*/

			static auto process_fs(
				const std::string & source,
				const std::string & destination,
				const std::string & key,
				const std::string & iv,
				bool use_64_bit_variant
			) -> void
			{
				auto compiled_text = Decode{source, key, iv, use_64_bit_variant};
				auto sen = compiled_text.process();
				FileSystem::write_binary<unsigned char>(destination, sen.get());
				return;
			}

	};
}