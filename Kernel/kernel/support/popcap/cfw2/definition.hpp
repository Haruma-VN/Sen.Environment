#pragma once

#include "kernel/definition/utility.hpp"

namespace Sen::Kernel::Support::PopCap::CFW2
{

	#pragma region using

	using Sen::Kernel::Definition::DataStreamView;

	#pragma endregion

	#pragma region FontCharacter

	struct FontCharacter 
    {
        public:
			char index;
			int image_rect_x;
			int image_rect_y;
			int image_rect_width;
			int image_rect_height;
			int image_offset_x;
			int image_offset_y;
			unsigned short kerning_first;
			unsigned short kerning_count;
			int width;
			int order;

			explicit FontCharacter(

			) = default;

			explicit constexpr FontCharacter(
				char index,
				int image_rect_x,
				int image_rect_y,
				int image_rect_width,
				int image_rect_height,
				int image_offset_x,
				int image_offset_y,
				unsigned short kerning_first,
				unsigned short kerning_count,
				int width,
				int order
			) : index(index), image_rect_x(image_rect_x), image_rect_y(image_rect_y), image_rect_width(image_rect_width), image_rect_height(image_rect_height),image_offset_x(image_offset_x), image_offset_y(image_offset_y), kerning_first(kerning_first), kerning_count(kerning_count), width(width),order(order) 
			{

			}

			~FontCharacter(

			) = default;
	};

	/**
	 * JSON Support 
	*/

	inline static auto from_json(
		const nlohmann::ordered_json & json,
		FontCharacter & font
	) -> void
	{
		json.at("index").get_to(font.index);
		json.at("image_rect_x").get_to(font.image_rect_x);
		json.at("image_rect_y").get_to(font.image_rect_y);
		json.at("image_rect_width").get_to(font.image_rect_width);
		json.at("image_rect_height").get_to(font.image_rect_height);
		json.at("image_offset_x").get_to(font.image_offset_x);
		json.at("image_offset_y").get_to(font.image_offset_y);
		json.at("kerning_first").get_to(font.kerning_first);
		json.at("kerning_count").get_to(font.kerning_count);
		json.at("width").get_to(font.width);
		json.at("order").get_to(font.order);
		return;
	}

	/**
	 * JSON Support 
	*/

	inline static auto to_json(
		nlohmann::ordered_json & json,
		const FontCharacter & font
	) -> void
	{
		json = nlohmann::ordered_json{
			{"index", font.index},
			{"image_rect_x", font.image_rect_x},
			{"image_rect_y", font.image_rect_y},
			{"image_rect_width", font.image_rect_width},
			{"image_rect_height", font.image_rect_height},
			{"image_offset_x", font.image_offset_x},
			{"image_offset_y", font.image_offset_y},
			{"kerning_first", font.kerning_first},
			{"kerning_count", font.kerning_count},
			{"width", font.width},
			{"order", font.order}
		};
		return;
	}

	#pragma endregion

	#pragma region CharacterItem

	struct CharacterItem {

		public:
			char index;
        	char value;

			explicit CharacterItem(

			) = default;

			explicit CharacterItem(
				char index,
				char value
			) : index(index), value(value)
			{

			}

			~CharacterItem(

			) = default;
	};

	/**
	 * JSON Support 
	*/

	inline static auto from_json(
		const nlohmann::ordered_json & json,
		CharacterItem & font
	) -> void
	{
		json.at("index").get_to(font.index);
		json.at("value").get_to(font.value);
		return;
	}

	/**
	 * JSON Support 
	*/

	inline static auto to_json(
		nlohmann::ordered_json & json,
		const CharacterItem & font
	) -> void
	{
		json = nlohmann::ordered_json{
			{"index", font.index},
			{"value", font.value}
		};
		return;
	}

	#pragma endregion

	#pragma region FontKerning
	
	struct FontKerning {
		public:
			char index;
			unsigned short offset;

			explicit FontKerning(

			) = default;

			explicit constexpr FontKerning(
				char index,
				unsigned short offset
			) : index(index), offset(offset)
			{

			}

			~FontKerning(

			) = default;
	};

	/**
	 * JSON Support 
	*/

	inline static auto from_json(
		const nlohmann::ordered_json & json,
		FontKerning & font
	) -> void
	{
		json.at("index").get_to(font.index);
		json.at("offset").get_to(font.offset);
		return;
	}

	/**
	 * JSON Support 
	*/

	inline static auto to_json(
		nlohmann::ordered_json & json,
		const FontKerning & font
	) -> void
	{
		json = nlohmann::ordered_json{
			{"index", font.index},
			{"offset", font.offset}
		};
		return;
	}

	#pragma endregion

	#pragma region FontLayer

	struct FontLayer 
    {
        public:
			std::string name;
			std::vector<std::string> tag_require;
			std::vector<std::string> tag_exclude;
			std::vector<FontKerning> kerning;
			std::vector<FontCharacter> character;
			int multiply_red;
			int multiply_green;
			int multiply_blue;
			int multiply_alpha;
			int add_red;
			int add_green;
			int add_blue;
			int add_alpha;
			std::string image_file;
			int draw_mode;
			int offset_x;
			int offset_y;
			int spacing;
			int minimum_point_size;
			int maximum_point_size;
			int point_size;
			int ascent;
			int ascent_padding;
			int height;
			int default_height;
			int line_spacing_offset;
			int base_order;

			FontLayer(
				const std::string & name,
				const std::vector<std::string> & tag_require,
				const std::vector<std::string> & tag_exclude,
				const std::vector<FontKerning> & kerning,
				const std::vector<FontCharacter> & character,
				int multiply_red,
				int multiply_green,
				int multiply_blue,
				int multiply_alpha,
				int add_red,
				int add_green,
				int add_blue,
				int add_alpha,
				const std::string & image_file,
				int draw_mode,
				int offset_x,
				int offset_y,
				int spacing,
				int minimum_point_size,
				int maximum_point_size,
				int point_size,
				int ascent,
				int ascent_padding,
				int height,
				int default_height,
				int line_spacing_offset,
				int base_order
			) : name(name), tag_require(std::move(tag_require)), tag_exclude(std::move(tag_exclude)), kerning(std::move(kerning)), character(std::move(character)), multiply_red(multiply_red), multiply_green(multiply_green), multiply_blue(multiply_blue), multiply_alpha(multiply_alpha), add_red(add_red), add_green(add_green), add_blue(add_blue), add_alpha(add_alpha), image_file(image_file), draw_mode(draw_mode), offset_x(offset_x), offset_y(offset_y), spacing(spacing), minimum_point_size(minimum_point_size), maximum_point_size(maximum_point_size), point_size(point_size), ascent(ascent),ascent_padding(ascent_padding), height(height), default_height(default_height), line_spacing_offset(line_spacing_offset), base_order(base_order)
			{

			}
			

			FontLayer(

			) = default;

			~FontLayer(

			) = default;
	};

	/**
	 * JSON Support 
	*/

	inline static auto from_json(
		const nlohmann::ordered_json & json,
		FontLayer & font
	) -> void
	{
		json.at("name").get_to(font.name);
		json.at("tag_require").get_to(font.tag_require);
		json.at("tag_exclude").get_to(font.tag_exclude);
		json.at("kerning").get_to(font.kerning);
		json.at("character").get_to(font.character);
		json.at("multiply_red").get_to(font.multiply_red);
		json.at("multiply_green").get_to(font.multiply_green);
		json.at("multiply_blue").get_to(font.multiply_blue);
		json.at("multiply_alpha").get_to(font.multiply_alpha);
		json.at("add_red").get_to(font.add_red);
		json.at("add_green").get_to(font.add_green);
		json.at("add_blue").get_to(font.add_blue);
		json.at("add_alpha").get_to(font.add_alpha);
		json.at("image_file").get_to(font.image_file);
		json.at("draw_mode").get_to(font.draw_mode);
		json.at("offset_x").get_to(font.offset_x);
		json.at("offset_y").get_to(font.offset_y);
		json.at("spacing").get_to(font.spacing);
		json.at("minimum_point_size").get_to(font.minimum_point_size);
		json.at("maximum_point_size").get_to(font.maximum_point_size);
		json.at("point_size").get_to(font.point_size);
		json.at("ascent").get_to(font.ascent);
		json.at("ascent_padding").get_to(font.ascent_padding);
		json.at("height").get_to(font.height);
		json.at("default_height").get_to(font.default_height);
		json.at("line_spacing_offset").get_to(font.line_spacing_offset);
		json.at("base_order").get_to(font.base_order);

		return;
	}

	/**
	 * JSON Support 
	*/

	inline static auto to_json(
		nlohmann::ordered_json & json,
		const FontLayer & font
	) -> void
	{
		json = nlohmann::ordered_json{};
		json["name"] = font.name;
		json["tag_require"] = font.tag_require;
		json["tag_exclude"] = font.tag_exclude;
		json["kerning"] = font.kerning;
		json["character"] = font.character;
		json["multiply_red"] = font.multiply_red;
		json["multiply_green"] = font.multiply_green;
		json["multiply_blue"] = font.multiply_blue;
		json["multiply_alpha"] = font.multiply_alpha;
		json["add_red"] = font.add_red;
		json["add_green"] = font.add_green;
		json["add_blue"] = font.add_blue;
		json["add_alpha"] = font.add_alpha;
		json["image_file"] = font.image_file;
		json["draw_mode"] = font.draw_mode;
		json["offset_x"] = font.offset_x;
		json["offset_y"] = font.offset_y;
		json["spacing"] = font.spacing;
		json["minimum_point_size"] = font.minimum_point_size;
		json["maximum_point_size"] = font.maximum_point_size;
		json["point_size"] = font.point_size;
		json["ascent"] = font.ascent;
		json["ascent_padding"] = font.ascent_padding;
		json["height"] = font.height;
		json["default_height"] = font.default_height;
		json["line_spacing_offset"] = font.line_spacing_offset;
		json["base_order"] = font.base_order;
		return;
	}

	#pragma endregion

	#pragma region CharacterFontWidget2

	struct CharacterFontWidget2 {

		public:

			std::array<uint8_t, 16> unknown;
			int ascent;
			int ascent_padding;
			int height;
			int line_sepacing_offset;
			bool initialized;
			int default_point_size;
			std::vector<CharacterItem> character;
			std::vector<FontLayer> layer;
			std::string source_file;
			std::string error_header;
			int point_size;
			std::vector<std::string> tag;
			double scale;
			bool force_scaled_image_white;
			bool activate_all_layer;

			CharacterFontWidget2(

			) = default;

			~CharacterFontWidget2(

			) = default;
	};

	/**
	 * JSON Support 
	*/

	inline static auto from_json(
		const nlohmann::ordered_json & json,
		CharacterFontWidget2 & font
	) -> void
	{
		json.at("unknown").get_to(font.unknown);
		json.at("ascent").get_to(font.ascent);
		json.at("ascent_padding").get_to(font.ascent_padding);
		json.at("height").get_to(font.height);
		json.at("line_sepacing_offset").get_to(font.line_sepacing_offset);
		json.at("initialized").get_to(font.initialized);
		json.at("default_point_size").get_to(font.default_point_size);
		json.at("character").get_to(font.character);
		font.layer = json.at("layer");
		font.source_file = json.at("source_file");
		json.at("error_header").get_to(font.error_header);
		json.at("point_size").get_to(font.point_size);
		json.at("tag").get_to(font.tag);
		json.at("scale").get_to(font.scale);
		json.at("force_scaled_image_white").get_to(font.force_scaled_image_white);
		json.at("activate_all_layer").get_to(font.activate_all_layer);
		return;
	}

	/**
	 * JSON Support 
	*/

	inline static auto to_json(
		nlohmann::ordered_json & json,
		const CharacterFontWidget2 & font
	) -> void
	{
		json = nlohmann::ordered_json{
			{"unknown", font.unknown},
			{"ascent", font.ascent},
			{"ascent_padding", font.ascent_padding},
			{"height", font.height},
			{"line_sepacing_offset", font.line_sepacing_offset},
			{"initialized", font.initialized},
			{"default_point_size", font.default_point_size}
		};
		json["character"] = font.character;
		json["layer"] = font.layer;
		json["source_file"] = font.source_file;
		json["error_header"] = font.error_header;
		json["point_size"] = font.point_size;
		json["tag"] = font.tag;
		json["scale"] = font.scale;
		json["force_scaled_image_white"] = font.force_scaled_image_white;
		json["activate_all_layer"] = font.activate_all_layer;
		return;
	}

	#pragma endregion
}
