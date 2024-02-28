#pragma once

#include "kernel/support/popcap/cfw2/definition.hpp"

namespace Sen::Kernel::Support::PopCap::CFW2
{

	#pragma region encode

	/**
	 * Encode Struct
	*/

	struct Encode {

		public:

			/**
			 * Constructor
			*/

			explicit Encode(

			) = default;

			/**
			 * Destructor
			*/

			~Encode(

			) = default;


			std::unique_ptr<DataStreamView> sen;

			/**
			 * Process method
			*/

			inline auto process(
				const CharacterFontWidget2 & cfw2_json
			) -> void
			{
				sen->close();
				sen->append(cfw2_json.unknown);
				sen->writeInt32(cfw2_json.ascent);
				sen->writeInt32(cfw2_json.ascent_padding);
				sen->writeInt32(cfw2_json.height);
				sen->writeInt32(cfw2_json.line_sepacing_offset);
				sen->writeBoolean(cfw2_json.initialized);
				sen->writeInt32(cfw2_json.default_point_size);
				auto characterCount = cfw2_json.character.size();
            	sen->writeUint32(static_cast<uint32_t>(characterCount));
				for	(auto i : Range<uint32_t>(static_cast<uint32_t>(characterCount))){
					sen->writeCharByInt16(cfw2_json.character[i].index);
                	sen->writeCharByInt16(cfw2_json.character[i].value);
				}
            	auto layerCount = cfw2_json.layer.size();
				sen->writeUint32(static_cast<uint32_t>(layerCount));
				for (auto i : Range<uint32_t>(static_cast<uint32_t>(layerCount))) {
					auto & layer = cfw2_json.layer[i];
					sen->writeStringByInt32(layer.name);
					auto tag_require_count = layer.tag_require.size();
					sen->writeUint32(static_cast<uint32_t>(tag_require_count));
					for (auto k : Range<uint32_t>(static_cast<uint32_t>(tag_require_count))) {
						sen->writeStringByInt32(layer.tag_require[k]);
					}
					auto tag_exclude_count = layer.tag_exclude.size();
					sen->writeUint32(static_cast<uint32_t>(tag_exclude_count));
					for (auto k : Range<uint32_t>(static_cast<uint32_t>(tag_exclude_count))) {
						sen->writeStringByInt32(layer.tag_exclude[k]);
					}
					auto kerning_count = layer.kerning.size();
					sen->writeUint32(static_cast<uint32_t>(kerning_count));
					for (auto k : Range<uint32_t>(static_cast<uint32_t>(kerning_count))) {
						sen->writeUint16(layer.kerning[k].offset);
						sen->writeCharByInt16(layer.kerning[k].index);
					}
					auto character_count = layer.character.size();
					sen->writeUint32(static_cast<uint32_t>(character_count));
					for (auto k : Range<uint32_t>(static_cast<uint32_t>(character_count))) {
						auto &character = layer.character[k];
						sen->writeCharByInt16(character.index);
						sen->writeInt32(character.image_rect_x);
						sen->writeInt32(character.image_rect_y);
						sen->writeInt32(character.image_rect_width);
						sen->writeInt32(character.image_rect_height);
						sen->writeInt32(character.image_offset_x);
						sen->writeInt32(character.image_offset_y);
						sen->writeUint16(character.kerning_count);
						sen->writeUint16(character.kerning_first);
						sen->writeInt32(character.width);
						sen->writeInt32(character.order);
					}
					sen->writeInt32(layer.multiply_red);
					sen->writeInt32(layer.multiply_green);
					sen->writeInt32(layer.multiply_blue);
					sen->writeInt32(layer.multiply_alpha);
					sen->writeInt32(layer.add_red);
					sen->writeInt32(layer.add_green);
					sen->writeInt32(layer.add_blue);
					sen->writeInt32(layer.add_alpha);
					sen->writeStringByInt32(layer.image_file);
					sen->writeInt32(layer.draw_mode);
					sen->writeInt32(layer.offset_x);
					sen->writeInt32(layer.offset_y);
					sen->writeInt32(layer.spacing);
					sen->writeInt32(layer.minimum_point_size);
					sen->writeInt32(layer.maximum_point_size);
					sen->writeInt32(layer.point_size);
					sen->writeInt32(layer.ascent);
					sen->writeInt32(layer.ascent_padding);
					sen->writeInt32(layer.height);
					sen->writeInt32(layer.default_height);
					sen->writeInt32(layer.line_spacing_offset);
					sen->writeInt32(layer.base_order);
				}
				sen->writeStringByInt32(cfw2_json.source_file);
				sen->writeStringByInt32(cfw2_json.error_header);
				sen->writeInt32(cfw2_json.point_size);
				auto tagCount = cfw2_json.tag.size();
				sen->writeUint32(static_cast<uint32_t>(tagCount));
				for (auto i : Range<uint32_t>(static_cast<uint32_t>(tagCount))) {
					sen->writeStringByInt32(cfw2_json.tag[i]);
				}
				sen->writeDouble(cfw2_json.scale);
				sen->writeBoolean(cfw2_json.force_scaled_image_white);
				sen->writeBoolean(cfw2_json.activate_all_layer);
				return;
			}

			inline static auto instance(

			) -> Encode& 
			{
				static auto INSTANCE = Encode{}; 
				return INSTANCE;
			}

			/**
			 * Process file sync
			 * @param source - source file
			 * @param destination - destination file
			 * @return - encoded file
			*/

			inline static auto process_fs(
				std::string_view source,
				std::string_view destination
			) -> void
			{
				auto& view = Encode::instance();
				view.process(*FileSystem::read_json(source));
				view.sen->out_file(destination);
				return;
			}

	};

	#pragma endregion
}