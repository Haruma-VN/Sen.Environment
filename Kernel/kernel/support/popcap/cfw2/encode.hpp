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

			/**
			 * Instance
			*/

			M_INSTANCE_OF_STRUCT(
				Encode
			);

			/**
			 * Process method
			*/

			inline auto process(
				const CharacterFontWidget2 & cfw2_json
			) -> DataStreamView
			{
				auto result = DataStreamView{};
				result.append(cfw2_json.unknown);
				result.writeInt32(cfw2_json.ascent);
				result.writeInt32(cfw2_json.ascent_padding);
				result.writeInt32(cfw2_json.height);
				result.writeInt32(cfw2_json.line_sepacing_offset);
				result.writeBoolean(cfw2_json.initialized);
				result.writeInt32(cfw2_json.default_point_size);
				auto characterCount = cfw2_json.character.size();
            	result.writeUint32(static_cast<uint32_t>(characterCount));
				for	(auto i : Range<uint32_t>(static_cast<uint32_t>(characterCount))){
					result.writeCharByInt16(cfw2_json.character[i].index);
                	result.writeCharByInt16(cfw2_json.character[i].value);
				}
            	auto layerCount = cfw2_json.layer.size();
				result.writeUint32(static_cast<uint32_t>(layerCount));
				for (auto i : Range<uint32_t>(static_cast<uint32_t>(layerCount))) {
					auto & layer = cfw2_json.layer[i];
					result.writeStringByInt32(layer.name);
					auto tag_require_count = layer.tag_require.size();
					result.writeUint32(static_cast<uint32_t>(tag_require_count));
					for (auto k : Range<uint32_t>(static_cast<uint32_t>(tag_require_count))) {
						result.writeStringByInt32(layer.tag_require[k]);
					}
					auto tag_exclude_count = layer.tag_exclude.size();
					result.writeUint32(static_cast<uint32_t>(tag_exclude_count));
					for (auto k : Range<uint32_t>(static_cast<uint32_t>(tag_exclude_count))) {
						result.writeStringByInt32(layer.tag_exclude[k]);
					}
					auto kerning_count = layer.kerning.size();
					result.writeUint32(static_cast<uint32_t>(kerning_count));
					for (auto k : Range<uint32_t>(static_cast<uint32_t>(kerning_count))) {
						result.writeUint16(layer.kerning[k].offset);
						result.writeCharByInt16(layer.kerning[k].index);
					}
					auto character_count = layer.character.size();
					result.writeUint32(static_cast<uint32_t>(character_count));
					for (auto k : Range<uint32_t>(static_cast<uint32_t>(character_count))) {
						auto character = layer.character[k];
						result.writeCharByInt16(character.index);
						result.writeInt32(character.image_rect_x);
						result.writeInt32(character.image_rect_y);
						result.writeInt32(character.image_rect_width);
						result.writeInt32(character.image_rect_height);
						result.writeInt32(character.image_offset_x);
						result.writeInt32(character.image_offset_y);
						result.writeUint16(character.kerning_count);
						result.writeUint16(character.kerning_first);
						result.writeInt32(character.width);
						result.writeInt32(character.order);
					}
					result.writeInt32(layer.multiply_red);
					result.writeInt32(layer.multiply_green);
					result.writeInt32(layer.multiply_blue);
					result.writeInt32(layer.multiply_alpha);
					result.writeInt32(layer.add_red);
					result.writeInt32(layer.add_green);
					result.writeInt32(layer.add_blue);
					result.writeInt32(layer.add_alpha);
					result.writeStringByInt32(layer.image_file);
					result.writeInt32(layer.draw_mode);
					result.writeInt32(layer.offset_x);
					result.writeInt32(layer.offset_y);
					result.writeInt32(layer.spacing);
					result.writeInt32(layer.minimum_point_size);
					result.writeInt32(layer.maximum_point_size);
					result.writeInt32(layer.point_size);
					result.writeInt32(layer.ascent);
					result.writeInt32(layer.ascent_padding);
					result.writeInt32(layer.height);
					result.writeInt32(layer.default_height);
					result.writeInt32(layer.line_spacing_offset);
					result.writeInt32(layer.base_order);
				}
				result.writeStringByInt32(cfw2_json.source_file);
				result.writeStringByInt32(cfw2_json.error_header);
				result.writeInt32(cfw2_json.point_size);
				auto tagCount = cfw2_json.tag.size();
				result.writeUint32(static_cast<uint32_t>(tagCount));
				for (auto i : Range<uint32_t>(static_cast<uint32_t>(tagCount))) {
					result.writeStringByInt32(cfw2_json.tag[i]);
				}
				result.writeDouble(cfw2_json.scale);
				result.writeBoolean(cfw2_json.force_scaled_image_white);
				result.writeBoolean(cfw2_json.activate_all_layer);
				return result;
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
				auto c = Encode::instance().process(FileSystem::read_json(source));
				FileSystem::write_binary(destination, c.get());
				return;
			}

	};

	#pragma endregion
}