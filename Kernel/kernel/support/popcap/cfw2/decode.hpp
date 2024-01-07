#pragma once

#include "kernel/support/popcap/cfw2/definition.hpp"

namespace Sen::Kernel::Support::PopCap::CFW2
{

	struct Decode {

		public:

			/**
			 * Constructor
			*/

			explicit Decode(

			) = default;

			/**
			 * Destructor
			*/

			~Decode(

			) = default;

			/**
			 * Process method
			*/

			inline static auto process(
				const DataStreamView & view
			) -> CharacterFontWidget2
			{
				auto unknown = std::vector<uint8_t>{};
				for (auto i : Range<int>(16)) {
					unknown.emplace_back(view.readUint8());
				}
				auto cfw2_json = CharacterFontWidget2{unknown};
				cfw2_json.ascent = view.readInt32LE();
				cfw2_json.ascent_padding = view.readInt32LE();
				cfw2_json.height = view.readInt32LE();
				cfw2_json.line_sepacing_offset = view.readInt32LE();
				cfw2_json.initialized = view.readBoolean();
				cfw2_json.default_point_size = view.readInt32LE();
				auto characterCount = view.readUint32LE();
				cfw2_json.character = std::vector<CharacterItem>();
				for (auto i : Range<uint32_t>(characterCount)) {
					cfw2_json.character.emplace_back(CharacterItem{
						view.readCharByInt16LE(),
						view.readCharByInt16LE()
					});
				}
				auto layer_count = view.readUint32LE();
            	cfw2_json.layer = std::vector<FontLayer>{};
				for (auto i : Range<uint32_t>(layer_count)) {
					auto name = view.readStringByInt32LE();
					auto tag_require_count = view.readUint32LE();
					auto tag_require =  std::vector<std::string>{};
					for (auto k : Range<uint32_t>(tag_require_count)) {
						tag_require.emplace_back(view.readStringByInt32LE());
					}
					auto tag_exclude_count = view.readUint32LE();
					auto tag_exclude = std::vector<std::string>{};
					for (auto k : Range<uint32_t>(tag_exclude_count)) {
						tag_exclude.emplace_back(view.readStringByInt32LE());
					}
					auto kerning_count = view.readUint32LE();
					auto kerning = std::vector<FontKerning>{};
					for (auto k : Range<uint32_t>(kerning_count)) {
						kerning.emplace_back(FontKerning {
							view.readCharByInt16LE(),
							static_cast<unsigned short>(view.readUint16LE())
						});
					}
					auto character_count = view.readUint32LE();
					auto character = std::vector<FontCharacter>{};
					for (auto k : Range<uint32_t>(character_count)) {
						character.emplace_back(FontCharacter {
							view.readCharByInt16LE(),
							view.readInt32LE(),
							view.readInt32LE(),
							view.readInt32LE(),
							view.readInt32LE(),
							view.readInt32LE(),
							view.readInt32LE(),
							view.readUint16LE(),
							view.readUint16LE(),
							view.readInt32LE(),
							view.readInt32LE()
						});
					}
					cfw2_json.layer.emplace_back(FontLayer{
						name,
						std::move(tag_require),
						std::move(tag_exclude),
						std::move(kerning),
						std::move(character),
						view.readInt32LE(),
						view.readInt32LE(),
						view.readInt32LE(),
						view.readInt32LE(),
						view.readInt32LE(),
						view.readInt32LE(),
						view.readInt32LE(),
						view.readInt32LE(),
						view.readStringByInt32LE(),
						view.readInt32LE(),
						view.readInt32LE(),
						view.readInt32LE(),
						view.readInt32LE(),
						view.readInt32LE(),
						view.readInt32LE(),
						view.readInt32LE(),
						view.readInt32LE(),
						view.readInt32LE(),
						view.readInt32LE(),
						view.readInt32LE(),
						view.readInt32LE(),
						view.readInt32LE()
					});
				}
				cfw2_json.source_file = view.readStringByInt32LE();
				cfw2_json.error_header = view.readStringByInt32LE();
				cfw2_json.point_size = view.readInt32LE();
				auto tagCount = view.readUint32LE();
				for (auto i : Range<uint32_t>(static_cast<uint32_t>(tagCount))) {
					cfw2_json.tag.emplace_back(view.readStringByInt32LE());
				}
				cfw2_json.scale = view.readDoubleLE();
				cfw2_json.force_scaled_image_white = view.readBoolean();
				cfw2_json.activate_all_layer = view.readBoolean();
				return cfw2_json;
			}

			/**
			 * Process file sync
			 * @param source - source file
			 * @param destination - destination file
			 * @returns - Decoded file
			*/


			inline static auto process_fs(
				const std::string & source,
				const std::string & destination
			) -> void
			{
				FileSystem::write_json(destination, Decode::process(DataStreamView{source}));
				return;
			}

	};
}