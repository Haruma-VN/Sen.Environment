#pragma once

#include "kernel/support/popcap/crypt_data/definition.hpp"

namespace Sen::Kernel::Support::PopCap::CryptData
{

	#pragma region encrypt

	struct Encrypt : public Definition {

		protected:

			using BasicDefinition = Definition;

		public:

			/**
			 * Constructor
			*/

			explicit Encrypt(

			) = default;

			M_INSTANCE_OF_STRUCT(
				Encrypt
			);

			/**
			 * Destructor
			*/

			~Encrypt(

			) = default;

			/**
			 * Process method
			*/

			inline auto process(
				const DataStreamView & view,
				std::string_view key
			) -> DataStreamView
			{
            	auto size = view.size();
				auto result = DataStreamView{};
				result.append<uint8_t, BasicDefinition::magic.size()>(BasicDefinition::magic);
				result.writeUint64(size);
				auto code = std::vector<uint8_t>{key.begin(), key.end()};
				if (size >= 0x100)
				{
					auto index = 0;
					auto arysize = key.size();
					for (auto i : Range<int>(0x100))
					{
						result.writeUint8((view.readUint8() ^ code[index++]));
						index %= arysize;
					}
				}
				result.append(view.get(view.get_write_pos(), view.size()));
				return result;
			}

			/**
			 * Process file sync
			*/

			inline static auto process_fs(
				std::string_view source,
				std::string_view destination,
				std::string_view key
			) -> void
			{
				auto result = Encrypt::instance().process(DataStreamView{source}, key).toBytes();
				FileSystem::write_binary(destination, result);
				return;
			}

	};


	#pragma endregion
}
