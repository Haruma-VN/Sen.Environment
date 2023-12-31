#pragma once

#include "kernel/support/popcap/render_effects/common.hpp"
#include "kernel/support/popcap/render_effects/definition.hpp"

namespace Sen::Kernel::Support::PopCap::RenderEffects {

	#pragma region using

	// using DataStreamView

	using Sen::Kernel::Definition::DataStreamView;


	#pragma endregion
	

	#pragma region decode

	/**
	 * Decode Struct
	*/

	struct Decode : public Common {
		
		private:

			#pragma region using

			using RenderEffects = PopCapRenderEffects;

			using BasicDefinition = Common;

			#pragma endregion

		protected:

			/**
			 * Stream Handler
			*/

			DataStreamView decoder;

		public:

			/**
			 * Constructor
			*/

			explicit Decode(

			) = default;

			/**
			 * Constructor
			*/

			explicit Decode(
				const std::string & source
			) : decoder(source)
			{

			}

			/**
			 * Constructor
			*/

			explicit Decode(
				DataStreamView & it
			) : decoder(it)
			{

			}

			/**
			 * Destructor
			*/

			~Decode(

			) = default;

			/**
			 * Decode method
			*/
			
			inline auto decode(

			) -> RenderEffects
			{
				auto info = std::unique_ptr<BasicDefinition>(new BasicDefinition());
				try_assert(decoder.readString(4) == BasicDefinition::magic, "mismatch magic");
				try_assert(decoder.readUint32LE() == BasicDefinition::version, "mismatch version");
				{
					info->block1_size = decoder.readUint32LE();
					info->block1_section_offset = decoder.readUint32LE();
					try_assert(decoder.readUint32LE() == info->Block1SectionSize, "invalid block1");
					info->block2_size = decoder.readUint32LE();
					info->block2_section_offset = decoder.readUint32LE();
					try_assert(decoder.readUint32LE() == info->Block2SectionSize, "invalid block2");
					info->block3_size = decoder.readUint32LE();
					info->block3_section_offset = decoder.readUint32LE();
					try_assert(decoder.readUint32LE() == info->Block3SectionSize, "invalid block3");
					info->block4_size = decoder.readUint32LE();
					info->block4_section_offset = decoder.readUint32LE();
					try_assert(decoder.readUint32LE() == info->Block4SectionSize, "invalid block4");
					info->block5_size = decoder.readUint32LE();
					info->block5_section_offset = decoder.readUint32LE();
					try_assert(decoder.readUint32LE() == info->Block5SectionSize, "invalid block5");
					info->block6_size = decoder.readUint32LE();
					info->block6_section_offset = decoder.readUint32LE();
					try_assert(decoder.readUint32LE() == info->Block6SectionSize, "invalid block6");
					info->block7_size = decoder.readUint32LE();
					info->block7_section_offset = decoder.readUint32LE();
					try_assert(decoder.readUint32LE() == info->Block7SectionSize, "invalid block7");
					info->block8_size = decoder.readUint32LE();
					info->block8_section_offset = decoder.readUint32LE();
					try_assert(decoder.readUint32LE() == info->Block8SectionSize, "invalid block8");
					info->string_section_offset = decoder.readUint32LE();
				}
				auto block1 = std::vector<Block1>{};
				decoder.change_position(info->block1_section_offset);
				for	(auto i : Range<uint32_t>(info->block1_size)) {
					block1.emplace_back(Block1{decoder.readUint32LE(), decoder.readUint32LE(), decoder.readUint32LE(), decoder.readUint32LE(), decoder.readUint32LE(), decoder.readUint32LE()});
				}
				auto block2 = std::vector<Block2>{};
				decoder.change_position(info->block2_section_offset);
				for	(auto i : Range<uint32_t>(info->block2_size)) {
					block2.emplace_back(Block2{decoder.readUint32LE(), decoder.readUint32LE()});
				}
				auto string_section = DataStreamView{decoder.get(info->string_section_offset, decoder.size())};
				auto block3 = std::vector<Block3>{};
				decoder.change_position(info->block3_section_offset);
				for	(auto i : Range<uint32_t>(info->block3_size)){
					decoder.readUint32LE();
					auto block_3 = Block3{};
					block_3.unknown_2 = decoder.readUint32LE();
					string_section.change_position(decoder.readUint32LE());
					block_3.string = string_section.readStringByEmpty();
					block3.emplace_back(block_3);
				}
				auto block4 = std::vector<Block4>{};
				decoder.change_position(info->block4_section_offset);
				for (auto i : Range<uint32_t>(info->block4_size)) {
					block4.emplace_back(Block4{decoder.readUint32LE(), decoder.readUint32LE(), decoder.readUint32LE(),decoder.readUint32LE(), decoder.readUint32LE()});
				}
				auto block5 = std::vector<Block5>{};
				decoder.change_position(info->block5_section_offset);
				for	(auto i : Range<uint32_t>(info->block5_size)) {
					block5.emplace_back(Block5{decoder.readUint32LE(), decoder.readUint32LE(), decoder.readUint32LE(), decoder.readUint32LE(), decoder.readUint32LE(), decoder.readUint32LE(), decoder.readUint32LE()});
				}
				auto block6 = std::vector<Block6>{};
				decoder.change_position(info->block6_section_offset);
				for	(auto i : Range<uint32_t>(info->block6_size)) {
					block6.emplace_back(Block6{decoder.readUint32LE(), decoder.readUint32LE(), decoder.readUint32LE(), decoder.readUint32LE(), decoder.readUint32LE()});
				}
				auto block7 = std::vector<Block7>{};
				decoder.change_position(info->block7_section_offset);
				for	(auto i : Range<uint32_t>(info->block7_size)) {
					block7.emplace_back(Block7{decoder.readUint32LE(), decoder.readUint32LE()});
				}
				auto block8 = std::vector<Block8>{};
				decoder.change_position(info->block8_section_offset);
				for (auto i : Range<uint32_t>(info->block8_size)) {
					block8.emplace_back(Block8{decoder.readUint32LE(), decoder.readUint32LE(), decoder.readUint32LE(),decoder.readUint32LE(), decoder.readUint32LE()});
				}
				return RenderEffects{
					block1,
					block2,
					block3,
					block4,
					block5,
					block6,
					block7,
					block8
				};
			}

			/**
			 * --------------------------------------
			 * Process method
			 * @param source: source file
			 * @param destination: destination file
			 * @returns: output file after decode
			 * --------------------------------------
			*/

			inline static auto process_fs(
				const std::string & source,
				const std::string & destination
			) -> void
			{
				auto decoder = Decode{source};
				FileSystem::write_json(destination, decoder.decode());
				return;
			}

	};

	#pragma endregion
}