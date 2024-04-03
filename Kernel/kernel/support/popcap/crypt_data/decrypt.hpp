#pragma once

#include "kernel/support/popcap/crypt_data/definition.hpp"

namespace Sen::Kernel::Support::PopCap::CryptData
{

	#pragma region decrypt

	struct Decrypt : public Definition {
		
		protected:

			using BasicDefinition = Definition;

		public:

			/**
			 * Constructor
			*/

			explicit Decrypt(

			) = default;

			/**
			 * Destructor
			*/

			~Decrypt(

			) = default;

			/**
			 * Get instance
			*/

			M_INSTANCE_OF_STRUCT(
				Decrypt
			);

			/**
			 * Process method
			*/

			inline auto process(
				const DataStreamView & view,
				std::string_view key
			) -> std::vector<std::uint8_t>
			{
				auto result = DataStreamView{};
				auto code = std::vector<uint8_t>{key.begin(), key.end()};
				assert_conditional((view.readString(BasicDefinition::magic.size()) == std::string{BasicDefinition::magic.begin(), BasicDefinition::magic.end()}), fmt::format("{}", Kernel::Language::get("popcap.crypt_data.decrypt.mismatch_magic")), "process");
            	auto size = view.readUint64();
				if (view.size() > 0x112){
					auto index = 0;
					auto arysize = key.size();
					for (auto i : Range<int>(0x100))
					{
						result.writeUint8((view.readUint8() ^ code[index++]));
						index %= arysize;
					}
				}
				result.append(view.get(view.get_read_pos(), view.size()));
				return result.toBytes();
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
				auto result = Decrypt::instance().process(DataStreamView{source}, key);
				FileSystem::write_binary(destination, result);
				return;
			}

	};

	#pragma endregion
}