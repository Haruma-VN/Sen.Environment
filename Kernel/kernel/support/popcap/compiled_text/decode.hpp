#pragma once

#include "kernel/definition/utility.hpp"
#include "kernel/support/popcap/zlib/uncompress.hpp"

namespace Sen::Kernel::Support::PopCap::CompiledText
{

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
	template <auto UseVariant>
	using Zlib = Sen::Kernel::Support::PopCap::Zlib::Uncompress<UseVariant>;

	/**
	 * Decode Struct
	 */

	struct Decode
	{

	private:
		/**
		 * Buffer handling
		 */

		std::unique_ptr<DataStreamView> sen;

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
		 * Default constructor
		 */

		explicit Decode(

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
			std::string_view source,
			std::string_view key,
			std::string_view iv,
			bool use_64_bit_variant
		) : sen(std::make_unique<DataStreamView>(source)), key(key), iv(iv), use_64_bit_variant(use_64_bit_variant), destination(std::make_unique<DataStreamView>())
		{
		}

		explicit Decode(
			DataStreamView & it,
			std::string_view key,
			std::string_view iv,
			bool use_64_bit_variant
		) : sen(&it), key(key), iv(iv), use_64_bit_variant(use_64_bit_variant), destination(std::make_unique<DataStreamView>())
		{
		}

		// Store the output data

		std::unique_ptr<DataStreamView> destination;

		/**
		 * --------------
		 * Process method
		 * --------------
		 */

		inline auto process(

		) -> void
		{
			auto decoded_base64 = DataStreamView{};
			decoded_base64.fromString(Base64::decode(thiz.sen->toString()));
			if (thiz.use_64_bit_variant) {
				destination->append<unsigned char>(Zlib<true>{}.uncompress(Rijndael::decrypt<std::uint64_t, Rijndael::Mode::CBC>(reinterpret_cast<char *>(decoded_base64.getBytes(0x02, decoded_base64.size()).data()), thiz.key, thiz.iv, decoded_base64.size() - 0x02)));
			}
			else {
				destination->append<unsigned char>(Zlib<false>{}.uncompress(Rijndael::decrypt<std::uint64_t, Rijndael::Mode::CBC>(reinterpret_cast<char *>(decoded_base64.getBytes(0x02, decoded_base64.size()).data()), thiz.key, thiz.iv, decoded_base64.size() - 0x02)));
			}
			return;
		}

		/**
		 * @param source: source file
		 * @param destination: destination file
		 * @returns: decoded file
		 */

		inline static auto process_fs(
			std::string_view source,
			std::string_view destination,
			std::string_view key,
			std::string_view iv,
			bool use_64_bit_variant
		) -> void
		{
			auto compiled_text = Decode{source, key, iv, use_64_bit_variant};
			compiled_text.process();
			compiled_text.destination->out_file(destination);
			return;
		}
	};
}