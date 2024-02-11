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
			) : sen(std::make_unique<DataStreamView>(source)), key(key), iv(iv), use_64_bit_variant(use_64_bit_variant)
			{

			}

			explicit Encode(
				DataStreamView & it,
				std::string_view key,
				std::string_view iv,
				bool use_64_bit_variant
			) : sen(&it), key(key), iv(iv), use_64_bit_variant(use_64_bit_variant)
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
				buffer.append(PopCap::Zlib::Compress{use_64_bit_variant}.compress(thiz.sen->get()));
    			buffer.writeNull(buffer.size() - ((buffer.size() + iv.size() - 1) % iv.size() + 1));
				auto result = DataStreamView{};
				result.writeUint8(0x10);
				result.writeUint8(0x00);
				auto decoded_base64 = DataStreamView{};
				decoded_base64.fromString(Base64::encode(reinterpret_cast<char*>(Rijndael::encrypt<std::uint64_t, Sen::Kernel::Definition::Encryption::Rijndael::Mode::CBC>(reinterpret_cast<const char *>(buffer.get().data()), key, iv, buffer.size()).data())));
				result.writeBytes(decoded_base64.get());
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
				FileSystem::write_binary<unsigned char>(destination, compiled_text.sen->get());
				return;
			}

	};
}