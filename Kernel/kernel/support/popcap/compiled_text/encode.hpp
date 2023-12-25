#pragma once

#include "kernel/definition/utility.hpp"
#include "kernel/support/popcap/zlib/compress.hpp"

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
	 * Encode Struct
	*/

	struct Encode {

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

			Encode(

			) = default;

			/**
			 * Default destructor
			*/

			~Encode(

			) = default;

			/**
			 * @param source: source file
			 * Constructor
			*/

			explicit Encode(
				const std::string & source,
				const std::string & key,
				const std::string & iv,
				bool use_64_bit_variant
			) : sen(source), key(key), iv(iv), use_64_bit_variant(use_64_bit_variant)
			{

			}

			explicit Encode(
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
				buffer.append(PopCap::Zlib::Compress{use_64_bit_variant}.compress(thiz.sen.get()));
    			buffer.writeNull(buffer.size() - ((buffer.size() + iv.size() - 1) % iv.size() + 1));
				auto result = SenBuffer{};
				result.writeUint8(0x10);
				result.writeUint8(0x00);
				result.append(SenBuffer::fromString(Base64::encode(reinterpret_cast<char*>(Rijndael::encrypt(reinterpret_cast<char *>(buffer.get().data()), key, iv, buffer.size(), Sen::Kernel::Definition::Encryption::RijndaelMode::CBC).data()))).get());
				return result;
			}

			/**
			 * @param source: source file
			 * @param destination: destination file
			 * @returns: Encoded file
			*/

			static auto process_fs(
				const std::string & source,
				const std::string & destination,
				const std::string & key,
				const std::string & iv,
				bool use_64_bit_variant
			) -> void
			{
				auto compiled_text = Encode{source, key, iv, use_64_bit_variant};
				auto sen = compiled_text.process();
				FileSystem::writeBinary<unsigned char>(destination, sen.get());
				return;
			}

	};
}