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

	class Decode : public Common {
		
		private:

			#pragma region using

			using RenderEffects = PopCapRenderEffects;

			using BasicDefinition = Common;

			#pragma endregion

		protected:

			/**
			 * Stream Handler
			*/

			std::unique_ptr<DataStreamView> decoder;

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
				std::string_view source
			) : decoder(std::make_unique<DataStreamView>(source))
			{

			}

			/**
			 * Constructor
			*/

			explicit Decode(
				DataStreamView & it
			) : decoder(&it)
			{

			}

			/**
			 * Destructor
			*/

			~Decode(

			) = default;

			/**
			 * Create instance
			*/

			M_INSTANCE_OF_STRUCT(
				Decode
			);

			/**
			 * Decode method
			*/
			
			inline auto process(
				RenderEffects* render_effects
			) -> void
			{
				auto info = std::make_unique<BasicDefinition>();
				assert_conditional(decoder->readString(4) == BasicDefinition::magic, fmt::format("{}", Kernel::Language::get("popcap.render_effects.decode.mismatch_magic")), "process");
				assert_conditional(decoder->readUint32() == BasicDefinition::version, fmt::format("{}", Kernel::Language::get("popcap.render_effects.decode.mismatch_version")), "process");
				{
					info->block1_size = decoder->readUint32();
					info->block1_section_offset = decoder->readUint32();
					assert_conditional(decoder->readUint32() == info->Block1SectionSize, fmt::format("{}", Kernel::Language::get("popcap.render_effects.decode.invalid_block_1_section_size")), "process");
					info->block2_size = decoder->readUint32();
					info->block2_section_offset = decoder->readUint32();
					assert_conditional(decoder->readUint32() == info->Block2SectionSize, fmt::format("{}", Kernel::Language::get("popcap.render_effects.decode.invalid_block_2_section_size")), "process");
					info->block3_size = decoder->readUint32();
					info->block3_section_offset = decoder->readUint32();
					assert_conditional(decoder->readUint32() == info->Block3SectionSize, fmt::format("{}", Kernel::Language::get("popcap.render_effects.decode.invalid_block_3_section_size")), "process");
					info->block4_size = decoder->readUint32();
					info->block4_section_offset = decoder->readUint32();
					assert_conditional(decoder->readUint32() == info->Block4SectionSize, fmt::format("{}", Kernel::Language::get("popcap.render_effects.decode.invalid_block_4_section_size")), "process");
					info->block5_size = decoder->readUint32();
					info->block5_section_offset = decoder->readUint32();
					assert_conditional(decoder->readUint32() == info->Block5SectionSize, fmt::format("{}", Kernel::Language::get("popcap.render_effects.decode.invalid_block_5_section_size")), "process");
					info->block6_size = decoder->readUint32();
					info->block6_section_offset = decoder->readUint32();
					assert_conditional(decoder->readUint32() == info->Block6SectionSize, fmt::format("{}", Kernel::Language::get("popcap.render_effects.decode.invalid_block_6_section_size")), "process");
					info->block7_size = decoder->readUint32();
					info->block7_section_offset = decoder->readUint32();
					assert_conditional(decoder->readUint32() == info->Block7SectionSize, fmt::format("{}", Kernel::Language::get("popcap.render_effects.decode.invalid_block_7_section_size")), "process");
					info->block8_size = decoder->readUint32();
					info->block8_section_offset = decoder->readUint32();
					assert_conditional(decoder->readUint32() == info->Block8SectionSize, fmt::format("{}", Kernel::Language::get("popcap.render_effects.decode.invalid_block_8_section_size")), "process");
					info->string_section_offset = decoder->readUint32();
				}
				decoder->change_read_pos(info->block1_section_offset);
				for	(auto i : Range<uint32_t>(info->block1_size)) {
					render_effects->block_1.emplace_back(Block1{decoder->readUint32(), decoder->readUint32(), decoder->readUint32(), decoder->readUint32(), decoder->readUint32(), decoder->readUint32()});
				}
				decoder->change_read_pos(info->block2_section_offset);
				for	(auto i : Range<uint32_t>(info->block2_size)) {
					render_effects->block_2.emplace_back(Block2{decoder->readUint32(), decoder->readUint32()});
				}
				auto string_section = DataStreamView{decoder->get(info->string_section_offset, decoder->size())};
				decoder->change_read_pos(info->block3_section_offset);
				for	(auto i : Range<uint32_t>(info->block3_size)){
					decoder->readUint32();
					auto block_3 = Block3{};
					block_3.unknown_2 = decoder->readUint32();
					string_section.change_read_pos(decoder->readUint32());
					block_3.string = string_section.readStringByEmpty();
					render_effects->block_3.emplace_back(block_3);
				}
				decoder->change_read_pos(info->block4_section_offset);
				for (auto i : Range<uint32_t>(info->block4_size)) {
					render_effects->block_4.emplace_back(Block4{decoder->readUint32(), decoder->readUint32(), decoder->readUint32(),decoder->readUint32(), decoder->readUint32()});
				}
				decoder->change_read_pos(info->block5_section_offset);
				for	(auto i : Range<uint32_t>(info->block5_size)) {
					render_effects->block_5.emplace_back(Block5{decoder->readUint32(), decoder->readUint32(), decoder->readUint32(), decoder->readUint32(), decoder->readUint32(), decoder->readUint32(), decoder->readUint32()});
				}
				decoder->change_read_pos(info->block6_section_offset);
				for	(auto i : Range<uint32_t>(info->block6_size)) {
					render_effects->block_6.emplace_back(Block6{decoder->readUint32(), decoder->readUint32(), decoder->readUint32(), decoder->readUint32(), decoder->readUint32()});
				}
				decoder->change_read_pos(info->block7_section_offset);
				for	(auto i : Range<uint32_t>(info->block7_size)) {
					render_effects->block_7.emplace_back(Block7{decoder->readUint32(), decoder->readUint32()});
				}
				decoder->change_read_pos(info->block8_section_offset);
				for (auto i : Range<uint32_t>(info->block8_size)) {
					render_effects->block_8.emplace_back(Block8{decoder->readUint32(), decoder->readUint32(), decoder->readUint32(),decoder->readUint32(), decoder->readUint32()});
				}
				return;
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
				std::string_view source,
				std::string_view destination
			) -> void
			{
				auto view = std::make_unique<Decode>(source);
				auto result = RenderEffects{};
				view->process(&result);
				FileSystem::write_json(destination, result);
				return;
			}

	};

	#pragma endregion
}