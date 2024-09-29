#pragma once

#include "kernel/definition/utility.hpp"

namespace Sen::Kernel::Support::PopCap::Animation::Convert
{

	struct Resize : Common
	{

	protected:
		using XMLDocument = tinyxml2::XMLDocument;

	private:
		// inline static auto constexpr resolution_ratio_list = std::array<double, 6>{ 0.78125, 1.5625, 1.875, 3.125, 1 };

		template <auto point, typename T>
			requires std::is_integral<T>::value or std::is_floating_point<T>::value
		inline static auto to_fixed(
			T number) -> std::string
		{
			static_assert(sizeof(point) == sizeof(int));
			auto stream = std::ostringstream{};
			stream << std::fixed << std::setprecision(static_cast<std::streamsize>(point)) << number;
			return stream.str();
		}

		inline static auto resize_image(
			XMLDocument &value,
			std::string const &image_name,
			float const &ratio,
			float const &old_ratio) -> void
		{
			auto dom_symbol_item = value.FirstChildElement("DOMSymbolItem");
			assert_conditional(dom_symbol_item != nullptr, String::format(fmt::format("{}", Language::get("popcap.animation.from_flash.image_has_no_DOMSymbolItem")), image_name), "resize_image");
			auto image_child = dom_symbol_item->FindAttribute("name");
			assert_conditional((image_child != nullptr && fmt::format("image/{}", image_name) == std::string{image_child->Value()}), String::format(fmt::format("{}", Language::get("popcap.animation.from_flash.invalid_image_name")), image_name), "resize_image");
			auto timeline = dom_symbol_item->FirstChildElement("timeline");
			assert_conditional(timeline != nullptr, String::format(fmt::format("{}", Language::get("popcap.animation.from_flash.image_has_no_timeline")), image_name), "resize_image");
			auto dom_timeline = timeline->FirstChildElement("DOMTimeline");
			assert_conditional(dom_timeline != nullptr, String::format(fmt::format("{}", Language::get("popcap.animation.from_flash.image_has_no_DOMtimeline")), image_name), "resize_image");
			auto document_name = dom_timeline->FindAttribute("name");
			assert_conditional((document_name != nullptr && image_name == std::string{document_name->Value()}), String::format(fmt::format("{}", Language::get("popcap.animation.from_flash.invalid_image_name")), image_name), "resize_image");
			auto layers = dom_timeline->FirstChildElement("layers");
			assert_conditional(layers != nullptr, String::format(fmt::format("{}", Language::get("popcap.animation.from_flash.image_has_no_layers")), image_name), "resize_image");
			auto dom_layer = layers->FirstChildElement("DOMLayer");
			assert_conditional(dom_layer != nullptr, String::format(fmt::format("{}", Language::get("popcap.animation.from_flash.image_has_no_DOMLayer")), image_name), "resize_image");
			auto frames = dom_layer->FirstChildElement("frames");
			assert_conditional(frames != nullptr, String::format(fmt::format("{}", Language::get("popcap.animation.from_flash.image_has_no_frames")), image_name), "resize_image");
			auto dom_frame = frames->FirstChildElement("DOMFrame");
			assert_conditional(dom_frame != nullptr, String::format(fmt::format("{}", Language::get("popcap.animation.from_flash.image_has_no_DOMframe")), image_name), "resize_image");
			auto elements = dom_frame->FirstChildElement("elements");
			assert_conditional(elements != nullptr, String::format(fmt::format("{}", Language::get("popcap.animation.from_flash.image_has_no_elements")), image_name), "resize_image");
			auto dom_bitmap_instance = elements->FirstChildElement("DOMBitmapInstance");
			assert_conditional(dom_bitmap_instance != nullptr, String::format(fmt::format("{}", Language::get("popcap.animation.from_flash.image_has_no_DOMBitmapInstance")), image_name), "resize_image");
			auto matrix = dom_bitmap_instance->FirstChildElement("matrix");
			if (matrix == nullptr)
			{
				return;
			}
			else
			{
				assert_conditional(matrix != nullptr, String::format(fmt::format("{}", Language::get("popcap.animation.from_flash.image_has_no_matrix")), image_name), "exchange_image_document");
				auto transform_matrix = matrix->FirstChildElement("Matrix");
				assert_conditional(transform_matrix != nullptr, String::format(fmt::format("{}", Language::get("popcap.animation.from_flash.invalid_image_matrix")), image_name), "exchange_image_document");
				auto a_matrix = transform_matrix->FindAttribute("a");
				auto d_matrix = transform_matrix->FindAttribute("d");
				transform_matrix->SetAttribute("a", to_fixed<6>((((a_matrix != nullptr ? std::stod(a_matrix->Value()) : 1.0) / old_ratio) * ratio)).data());
				transform_matrix->SetAttribute("d", to_fixed<6>((((d_matrix != nullptr ? std::stod(d_matrix->Value()) : 1.0) / old_ratio) * ratio)).data());
			}
			return;
		}

		template <typename T>
			requires std::is_floating_point<T>::value
		inline static auto constexpr calculate_ratio(
			int const &image_resolution) -> T
		{
			return static_cast<T>(static_cast<T>(1200) / static_cast<T>(image_resolution));
		}

	public:
		inline static auto process_whole(
			std::string_view source,
			int const &resolution) -> void
		{
			ExtraInfo definition = *FileSystem::read_json(fmt::format("{}/data.json", source));
			auto image_list = FileSystem::read_directory_only_file(fmt::format("{}/library/image", source));
			auto ratio = calculate_ratio<double>(resolution);
			auto old_ratio = calculate_ratio<double>(definition.resolution);
			for (auto &[image_name, image_value] : definition.image)
			{
				auto document = XMLDocument{};
				auto path = fmt::format("{}/library/image/{}.xml", source, image_name);
				FileSystem::read_xml(path, &document);
				resize_image(document, image_name, ratio, old_ratio);
				FileSystem::write_xml(path, &document);
			}
			definition.resolution = resolution;
			FileSystem::write_json(fmt::format("{}/data.json", source), definition);
			return;
		}

		inline static auto process_fs(
			std::string_view source,
			int const &resolution) -> void
		{
			process_whole(source, resolution);
			return;
		}
	};
}