#pragma once

#include "kernel/support/popcap/character_font_widget_2/definition.hpp"

namespace Sen::Kernel::Support::PopCap::CharacterFontWidget2
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


			DataStreamView stream;

			/**
			 * Process method
			*/

			inline auto process(
				const CharacterFontWidget2 & cfw2_json
			) -> void
			{
				for (auto i : Range<int>(cfw2_json.unknown.size())) {
					stream.writeUint8(cfw2_json.unknown[i]);
				}
				stream.writeInt32(cfw2_json.ascent);
				stream.writeInt32(cfw2_json.ascent_padding);
				stream.writeInt32(cfw2_json.height);
				stream.writeInt32(cfw2_json.line_sepacing_offset);
				stream.writeBoolean(cfw2_json.initialized);
				stream.writeInt32(cfw2_json.default_point_size);
				auto characterCount = cfw2_json.character.size();
            	stream.writeUint32(static_cast<uint32_t>(characterCount));
				for	(auto i : Range<uint32_t>(static_cast<uint32_t>(characterCount))){
					stream.writeCharByInt16(cfw2_json.character[i].index);
                	stream.writeCharByInt16(cfw2_json.character[i].value);
				}
            	auto layerCount = cfw2_json.layer.size();
				stream.writeUint32(static_cast<uint32_t>(layerCount));
				for (auto i : Range<uint32_t>(static_cast<uint32_t>(layerCount))) {
					auto & layer = cfw2_json.layer[i];
					stream.writeStringByInt32(layer.name);
					auto tag_require_count = layer.tag_require.size();
					stream.writeUint32(static_cast<uint32_t>(tag_require_count));
					for (auto k : Range<uint32_t>(static_cast<uint32_t>(tag_require_count))) {
						stream.writeStringByInt32(layer.tag_require[k]);
					}
					auto tag_exclude_count = layer.tag_exclude.size();
					stream.writeUint32(static_cast<uint32_t>(tag_exclude_count));
					for (auto k : Range<uint32_t>(static_cast<uint32_t>(tag_exclude_count))) {
						stream.writeStringByInt32(layer.tag_exclude[k]);
					}
					auto kerning_count = layer.kerning.size();
					stream.writeUint32(static_cast<uint32_t>(kerning_count));
					for (auto k : Range<uint32_t>(static_cast<uint32_t>(kerning_count))) {
						stream.writeUint16(layer.kerning[k].offset);
						stream.writeCharByInt16(layer.kerning[k].index);
					}
					auto character_count = layer.character.size();
					stream.writeUint32(static_cast<uint32_t>(character_count));
					for (auto k : Range<uint32_t>(static_cast<uint32_t>(character_count))) {
						auto &character = layer.character[k];
						stream.writeCharByInt16(character.index);
						stream.writeInt32(character.image_rect_x);
						stream.writeInt32(character.image_rect_y);
						stream.writeInt32(character.image_rect_width);
						stream.writeInt32(character.image_rect_height);
						stream.writeInt32(character.image_offset_x);
						stream.writeInt32(character.image_offset_y);
						stream.writeUint16(character.kerning_count);
						stream.writeUint16(character.kerning_first);
						stream.writeInt32(character.width);
						stream.writeInt32(character.order);
					}
					stream.writeInt32(layer.multiply_red);
					stream.writeInt32(layer.multiply_green);
					stream.writeInt32(layer.multiply_blue);
					stream.writeInt32(layer.multiply_alpha);
					stream.writeInt32(layer.add_red);
					stream.writeInt32(layer.add_green);
					stream.writeInt32(layer.add_blue);
					stream.writeInt32(layer.add_alpha);
					stream.writeStringByInt32(layer.image_file);
					stream.writeInt32(layer.draw_mode);
					stream.writeInt32(layer.offset_x);
					stream.writeInt32(layer.offset_y);
					stream.writeInt32(layer.spacing);
					stream.writeInt32(layer.minimum_point_size);
					stream.writeInt32(layer.maximum_point_size);
					stream.writeInt32(layer.point_size);
					stream.writeInt32(layer.ascent);
					stream.writeInt32(layer.ascent_padding);
					stream.writeInt32(layer.height);
					stream.writeInt32(layer.default_height);
					stream.writeInt32(layer.line_spacing_offset);
					stream.writeInt32(layer.base_order);
				}
				stream.writeStringByInt32(cfw2_json.source_file);
				stream.writeStringByInt32(cfw2_json.error_header);
				stream.writeInt32(cfw2_json.point_size);
				auto tagCount = cfw2_json.tag.size();
				stream.writeUint32(static_cast<uint32_t>(tagCount));
				for (auto i : Range<uint32_t>(static_cast<uint32_t>(tagCount))) {
					stream.writeStringByInt32(cfw2_json.tag[i]);
				}
				stream.writeDouble(cfw2_json.scale);
				stream.writeBoolean(cfw2_json.force_scaled_image_white);
				stream.writeBoolean(cfw2_json.activate_all_layer);
				return;
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
				auto view = Encode{};
				view.process(*FileSystem::read_json(source));
				view.stream.out_file(destination);
				return;
			}

	};

	#pragma endregion
}