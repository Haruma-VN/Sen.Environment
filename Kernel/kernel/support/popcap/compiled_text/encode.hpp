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
			 * Key for compiled text
			*/

			std::string_view key;

			/**
			 * Iv for compiled text
			*/

			std::string_view iv;

			/**
			 * The use of 64bit variant
			*/

			bool use_64_bit_variant;

		public:

			/**
			 * Buffer handling
			*/

			std::unique_ptr<DataStreamView> sen;

			/**
			 * Buffer handling
			*/

			std::unique_ptr<DataStreamView> result;

			/**
			 * Default constructor
			*/

			explicit Encode(

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
				std::string_view source,
				std::string_view key,
				std::string_view iv,
				bool use_64_bit_variant
			) : sen(std::make_unique<DataStreamView>(source)), key(key), iv(iv), use_64_bit_variant(use_64_bit_variant), result(std::make_unique<DataStreamView>())
			{

			}

			explicit Encode(
				DataStreamView & it,
				std::string_view key,
				std::string_view iv,
				bool use_64_bit_variant
			) : sen(&it), key(key), iv(iv), use_64_bit_variant(use_64_bit_variant), result(std::make_unique<DataStreamView>())
			{

			}

			/**
			 * --------------
			 * Process method
			 * --------------
			*/

			inline auto process(

			) -> void
			{
				auto buffer = DataStreamView{};
				buffer.writeBytes(PopCap::Zlib::Compress<false>{}.compress(thiz.sen->getBytes(0, thiz.sen->size())));
				fill_rijndael_block(buffer, thiz.iv);
				auto decoded_base64 = DataStreamView{};
				decoded_base64.writeUint8(0x10);
				decoded_base64.writeUint8(0x00);
				decoded_base64.writeBytes(Rijndael::encrypt<std::uint64_t, Sen::Kernel::Definition::Encryption::Rijndael::Mode::CBC>(reinterpret_cast<char *>(buffer.getBytes(0, buffer.size()).data()), key, iv, buffer.size()));
				auto encoded_base64 = DataStreamView{};
				encoded_base64.fromString(Base64::encode(decoded_base64.getBytes(0, decoded_base64.size()).data(), decoded_base64.size()));
				result->writeBytes(encoded_base64.getBytes(0, encoded_base64.size()));
				return;
			}

			/**
			 * @param source: source file
			 * @param destination: destination file
			 * @returns: Encoded file
			*/

			inline static auto process_fs(
				std::string_view source,
				std::string_view destination,
				std::string_view key,
				std::string_view iv,
				bool use_64_bit_variant
			) -> void
			{
				auto compiled_text = Encode{source, key, iv, use_64_bit_variant};
				compiled_text.process();
				compiled_text.result->out_file(destination);
				return;
			}

	};
}