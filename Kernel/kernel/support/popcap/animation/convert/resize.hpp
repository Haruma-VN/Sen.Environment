#pragma once

#include "kernel/definition/utility.hpp"
#include "kernel/support/popcap/animation/definition.hpp"

namespace Sen::Kernel::Support::PopCap::Animation {

	class Resize {

		protected:

			using XMLDocument = tinyxml2::XMLDocument;

		protected:
			inline static auto constexpr resolution_ratio_list = std::array<double, 6>{ 0.78125, 1.5625, 1.875, 3.125, 1 };

			template <auto point, typename T>
				requires std::is_integral<T>::value or std::is_floating_point<T>::value
			inline static auto to_fixed(
				T number
			) -> std::string
			{
				static_assert(sizeof(point) == sizeof(int));
				auto stream = std::ostringstream{};
				stream << std::fixed << std::setprecision(static_cast<std::streamsize>(point)) << number;
				return stream.str();
			}

		public:
			Resize(
				Resize &&
			) = delete;

			explicit Resize(

			) = default;

			~Resize(

			) = default;

			auto operator =(
				Resize&& that
			)->Resize & = delete;

		protected:

			inline auto resize_image(
				const std::string& image_id,
				XMLDocument* document,
				float ratio
			) -> void
			{
				auto DOMSymbolItem = document->FirstChildElement("DOMSymbolItem");
				if (DOMSymbolItem == nullptr)
				{
					throw Exception(String::format(fmt::format("{}", Language::get("popcap.animation.from_flash.image_has_no_DOMSymbolItem")), image_id), std::source_location::current(), "resize_image");
				}
				auto image_child = DOMSymbolItem->FindAttribute("name");
				if (image_child == nullptr or fmt::format("image/{}", image_id) != std::string{image_child->Value()})
				{
					throw Exception(String::format(fmt::format("{}", Language::get("popcap.animation.from_flash.invalid_image_name")), image_id), std::source_location::current(), "resize_image");
				}
				auto timeline = DOMSymbolItem->FirstChildElement("timeline");
				if (timeline == nullptr)
				{
					throw Exception(String::format(fmt::format("{}", Language::get("popcap.animation.from_flash.image_has_no_timeline")), image_id), std::source_location::current(), "resize_image");
				}
				auto DOMTimeline = timeline->FirstChildElement("DOMTimeline");
				if (DOMTimeline == nullptr)
				{
					throw Exception(String::format(fmt::format("{}", Language::get("popcap.animation.from_flash.image_has_no_DOMtimeline")), image_id), std::source_location::current(), "resize_image");
				}
				auto id_name = DOMTimeline->FindAttribute("name");
				if (id_name == nullptr or image_id != std::string{id_name->Value()})
				{
					throw Exception(String::format(fmt::format("{}", Language::get("popcap.animation.from_flash.invalid_image_name")), image_id), std::source_location::current(), "resize_image");
				}
				auto layers = DOMTimeline->FirstChildElement("layers");
				if (layers == nullptr)
				{
					throw Exception(String::format(fmt::format("{}", Language::get("popcap.animation.from_flash.image_has_no_layers")), image_id), std::source_location::current(), "resize_image");
				}
				auto DOMLayer = layers->FirstChildElement("DOMLayer");
				if (DOMLayer == nullptr)
				{
					throw Exception(String::format(fmt::format("{}", Language::get("popcap.animation.from_flash.image_has_no_DOMLayer")), image_id), std::source_location::current(), "resize_image");
				}
				auto frames = DOMLayer->FirstChildElement("frames");
				if (frames == nullptr)
				{
					throw Exception(String::format(fmt::format("{}", Language::get("popcap.animation.from_flash.image_has_no_frames")), image_id), std::source_location::current(), "resize_image");
				}
				auto DOMFrame = frames->FirstChildElement("DOMFrame");
				if (DOMFrame == nullptr)
				{
					throw Exception(String::format(fmt::format("{}", Language::get("popcap.animation.from_flash.image_has_no_DOMframe")), image_id), std::source_location::current(), "resize_image");
				}
				auto elements = DOMFrame->FirstChildElement("elements");
				if (elements == nullptr)
				{
					throw Exception(String::format(fmt::format("{}", Language::get("popcap.animation.from_flash.image_has_no_elements")), image_id), std::source_location::current(), "resize_image");
				}
				auto DOMBitmapInstance = elements->FirstChildElement("DOMBitmapInstance");
				if (DOMBitmapInstance == nullptr)
				{
					throw Exception(String::format(fmt::format("{}", Language::get("popcap.animation.from_flash.image_has_no_DOMBitmapInstance")), image_id), std::source_location::current(), "resize_image");
				}
				auto image_name = std::string{DOMBitmapInstance->FindAttribute("libraryItemName")->Value()}.substr(6);
				auto matrix = DOMBitmapInstance->FirstChildElement("matrix");
				if (matrix == nullptr)
				{
					throw Exception(String::format(fmt::format("{}", Language::get("popcap.animation.from_flash.image_has_no_matrix")), image_id), std::source_location::current(), "resize_image");
				}
				auto Matrix = matrix->FirstChildElement("Matrix");
				if (Matrix == nullptr)
				{
					throw Exception(String::format(fmt::format("{}", Language::get("popcap.animation.from_flash.invalid_image_matrix")), image_id), std::source_location::current(), "resize_image");
				}
				auto a_matrix = Matrix->FirstChildElement("a");
				auto a = double{};
				if (a_matrix != nullptr) {
					a = std::stof(a_matrix->GetText());
				}
				else {
					a = 1.0;
				}
				if (std::find(resolution_ratio_list.begin(), resolution_ratio_list.end(), a) != resolution_ratio_list.end()) {
					a = 1.0;
				}
				auto d_matrix = Matrix->FirstChildElement("d");
				auto d = double{};
				if (d_matrix != nullptr) {
					d = std::stof(d_matrix->GetText());
				}
				else {
					d = 1.0;
				}
				if (std::find(resolution_ratio_list.begin(), resolution_ratio_list.end(), d) != resolution_ratio_list.end()) {
					d = 1.0;
				}
				a_matrix->SetText(to_fixed<6>((a * ratio)).data());
				d_matrix->SetText(to_fixed<6>((d * ratio)).data());
				return;
			}

			template <typename T> requires std::is_floating_point<T>::value
			auto constexpr calculate_ratio(
				auto image_resolution
			) -> T
			{
				return static_cast<T>(static_cast<T>(1200) / static_cast<T>(image_resolution));
			}

		protected:

			inline auto process(
				std::string_view source,
				float resolution
			) -> void
			{
				auto image_list = FileSystem::read_directory_only_file(fmt::format("{}/library/image", source));
				auto ratio = calculate_ratio<double>(resolution);
				for (auto& image : image_list) {
					auto document = XMLDocument{};
					FileSystem::read_xml(image, &document);
					resize_image(Path::getFileName(image), &document, ratio);
					FileSystem::write_xml(image, &document);
				}
				return;
			}
			
		public:

			inline static auto process_fs(
				std::string_view source,
				float resolution
			) -> void
			{
				auto resize = Resize{};
				resize.process(source, resolution);
				return;
			}


	};
}