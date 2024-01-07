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
			 * Process method
			*/

			inline static auto process(
				const DataStreamView & view,
				const std::string & key
			) -> DataStreamView
			{
				auto result = DataStreamView{};
				auto code = std::vector<uint8_t>{key.begin(), key.end()};
				try_assert((view.readString(BasicDefinition::magic.size()) == std::string{BasicDefinition::magic.begin(), BasicDefinition::magic.end()}), "Mismatch Crypt-Data magic");
            	auto size = view.readUint64LE();
				if (view.size() > 0x112){
					auto index = 0;
					auto arysize = key.size();
					for (auto i : Range<int>(0x100))
					{
						result.writeUint8((view.readUint8() ^ code[index++]));
						index %= arysize;
					}
				}
				result.append(view.get(view.get_position(), view.size()));
				return result;
			}

			/**
			 * Process file sync
			*/

			inline static auto process_fs(
				const std::string & source,
				const std::string & destination,
				const std::string & key
			) -> void
			{
				FileSystem::write_binary(destination, Decrypt::process(DataStreamView{source}, key).get());
				return;
			}

	};

	#pragma endregion
}