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

			/**
			 * Destructor
			*/

			~Encrypt(

			) = default;

			/**
			 * Process method
			*/

			inline static auto process(
				const DataStreamView & view,
				const std::string & key
			) -> DataStreamView
			{
            	auto size = view.size();
				auto result = DataStreamView{};
				result.append<uint8_t, BasicDefinition::magic.size()>(BasicDefinition::magic);
				result.writeUint64LE(size);
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
				FileSystem::write_binary(destination, Encrypt::process(DataStreamView{source}, key).get());
				return;
			}

	};


	#pragma endregion
}
