#pragma once

#include "kernel/support/popcap/render_effects/common.hpp"
#include "kernel/support/popcap/render_effects/definition.hpp"

namespace Sen::Kernel::Support::PopCap::RenderEffects {

	#pragma region Block Offset

	/**
	 * Block Offset
	*/

	struct BlockOffset {
		public:
			uint32_t block1_section_offset;
			uint32_t block2_section_offset;
			uint32_t block3_section_offset;
			uint32_t block4_section_offset;
			uint32_t block5_section_offset;
			uint32_t block6_section_offset;
			uint32_t block7_section_offset;
			uint32_t block8_section_offset;

			explicit BlockOffset(

			) = default;

			~BlockOffset(

			) = default;
	};

	#pragma endregion

	#pragma region using

	// using DataStreamView

	using Sen::Kernel::Definition::DataStreamView;

	#pragma endregion

	#pragma region encode

	/**
	 * Encoder
	*/

	struct Encode : public Common {

		private:

			#pragma region using

			using RenderEffects = PopCapRenderEffects;

			using BasicDefinition = Common;

			#pragma endregion

		protected:

			// Render Effects

			RenderEffects data;

		public:
			
			/**
			 * Constructor
			*/

			explicit Encode(

			) = default;

			// DataStreamView

			std::unique_ptr<DataStreamView> sen;

			/**
			 * Constructor
			*/

			explicit Encode(
				std::string_view source
			) : data(FileSystem::read_json(source))
			{

			}

			/**
			 * Destructor
			*/

			~Encode(

			) = default;

			/**
			 * Encode method
			*/

			inline auto encode(

			) -> void
			{
				auto data = DataStreamView{};
            	auto blockSectionOffset = BasicDefinition::BlockSectionOffset;
				auto block_offset = BlockOffset{};
				data.writeNull(BasicDefinition::BlockSectionOffset);
				sen->writeString(std::string{BasicDefinition::magic.data(), BasicDefinition::magic.size()});
				sen->writeUint32(BasicDefinition::version);
				block_offset.block1_section_offset = static_cast<uint32_t>(data.get_write_pos());
				for (auto i : Range<uint32_t>(thiz.data.block_1.size())) {
					data.writeUint32(thiz.data.block_1[i].unknown_1);
					data.writeUint32(thiz.data.block_1[i].unknown_2);
					data.writeUint32(thiz.data.block_1[i].unknown_3);
					data.writeUint32(thiz.data.block_1[i].unknown_4);
					data.writeUint32(thiz.data.block_1[i].unknown_5);
					data.writeUint32(thiz.data.block_1[i].unknown_6);
				}
				block_offset.block5_section_offset = static_cast<uint32_t>(data.get_write_pos());
				for (auto i : Range<uint32_t>(static_cast<uint32_t>(thiz.data.block_5.size())))
				{
					data.writeUint32(thiz.data.block_5[i].unknown_1); 
					data.writeUint32(thiz.data.block_5[i].unknown_2);
					data.writeUint32(thiz.data.block_5[i].unknown_3);
					data.writeUint32(thiz.data.block_5[i].unknown_4);
					data.writeUint32(thiz.data.block_5[i].unknown_5);
					data.writeUint32(thiz.data.block_5[i].unknown_6);
					data.writeUint32(thiz.data.block_5[i].unknown_7);
				}
				block_offset.block6_section_offset = static_cast<uint32_t>(data.get_write_pos());
				for (auto i : Range<uint32_t>(static_cast<uint32_t>(thiz.data.block_6.size())))
				{
					data.writeUint32(thiz.data.block_6[i].unknown_1);
					data.writeUint32(thiz.data.block_6[i].unknown_2);
					data.writeUint32(thiz.data.block_6[i].unknown_3);
					data.writeUint32(thiz.data.block_6[i].unknown_4);
					data.writeUint32(thiz.data.block_6[i].unknown_5);
				}
				block_offset.block2_section_offset = static_cast<uint32_t>(data.get_write_pos());
				for (auto i : Range<uint32_t>(static_cast<uint32_t>(thiz.data.block_2.size())))
				{
					data.writeUint32(thiz.data.block_2[i].unknown_1);
					data.writeUint32(thiz.data.block_2[i].unknown_2);
				}
				block_offset.block3_section_offset = static_cast<uint32_t>(data.get_write_pos());
				auto string_section = DataStreamView{};
				for (auto i : Range<uint32_t>(static_cast<uint32_t>(thiz.data.block_3.size())))
				{
					data.writeUint32(static_cast<uint32_t>(thiz.data.block_3[i].string.size()));
					data.writeUint32(thiz.data.block_3[i].unknown_2);
					data.writeUint32(string_section.get_write_pos());
					string_section.writeStringByEmpty(thiz.data.block_3[i].string);
				}
				block_offset.block4_section_offset = static_cast<uint32_t>(data.get_write_pos());
				for (auto i : Range<uint32_t>(thiz.data.block_4.size())) {
					data.writeUint32(thiz.data.block_4[i].unknown_1);
					data.writeUint32(thiz.data.block_4[i].unknown_2);
					data.writeUint32(thiz.data.block_4[i].unknown_3);
					data.writeUint32(thiz.data.block_4[i].unknown_4);
					data.writeUint32(thiz.data.block_4[i].unknown_5);
				}
				block_offset.block7_section_offset = static_cast<uint32_t>(data.get_write_pos());
				for (auto i : Range<uint32_t>(static_cast<uint32_t>(thiz.data.block_7.size())))
				{
					data.writeUint32(thiz.data.block_7[i].unknown_1);
					data.writeUint32(thiz.data.block_7[i].unknown_2);
				}
				block_offset.block8_section_offset = static_cast<uint32_t>(data.get_write_pos());
				for (auto i : Range<uint32_t>(thiz.data.block_8.size())) {
					data.writeUint32(thiz.data.block_8[i].unknown_1);
					data.writeUint32(thiz.data.block_8[i].unknown_2);
					data.writeUint32(thiz.data.block_8[i].unknown_4);
					data.writeUint32(thiz.data.block_8[i].unknown_5);
					data.writeUint32(thiz.data.block_8[i].unknown_3);
				}
				sen->writeUint32(static_cast<uint32_t>(thiz.data.block_1.size()));
				sen->writeUint32(block_offset.block1_section_offset);
				sen->writeUint32(BasicDefinition::Block1SectionSize);
				sen->writeUint32(static_cast<uint32_t>(thiz.data.block_2.size()));
				sen->writeUint32(block_offset.block2_section_offset);
				sen->writeUint32(BasicDefinition::Block2SectionSize);
				sen->writeUint32(static_cast<uint32_t>(thiz.data.block_3.size()));
				sen->writeUint32(block_offset.block3_section_offset);
				sen->writeUint32(BasicDefinition::Block3SectionSize);
				sen->writeUint32(static_cast<uint32_t>(thiz.data.block_4.size()));
				sen->writeUint32(block_offset.block4_section_offset);
				sen->writeUint32(BasicDefinition::Block4SectionSize);
				sen->writeUint32(static_cast<uint32_t>(thiz.data.block_5.size()));
				sen->writeUint32(block_offset.block5_section_offset);
				sen->writeUint32(BasicDefinition::Block5SectionSize);
				sen->writeUint32(static_cast<uint32_t>(thiz.data.block_6.size()));
				sen->writeUint32(block_offset.block6_section_offset);
				sen->writeUint32(BasicDefinition::Block6SectionSize);
				sen->writeUint32(static_cast<uint32_t>(thiz.data.block_7.size()));
				sen->writeUint32(block_offset.block7_section_offset);
				sen->writeUint32(BasicDefinition::Block7SectionSize);
				sen->writeUint32(static_cast<uint32_t>(thiz.data.block_8.size()));
				sen->writeUint32(block_offset.block8_section_offset);
				sen->writeUint32(BasicDefinition::Block8SectionSize);
				sen->writeUint32(static_cast<uint32_t>(blockSectionOffset));
				sen->writeBytes(data.get(static_cast<uint64_t>(BasicDefinition::BlockSectionOffset), data.size()));
				sen->writeBytes(string_section.get(0, string_section.size()));
				return;
			}

			/**
			 * --------------------------------------
			 * Process method
			 * @param source: source file
			 * @param destination: destination file
			 * @returns: output file after encode
			 * --------------------------------------
			*/

			inline static auto process_fs(
				std::string_view source,
				std::string_view destination
			) -> void
			{
				auto view = Encode{ source };
				view.encode();
				view.sen->out_file(destination);
				return;
			}
	};

	#pragma endregion
}
