#pragma once

#include "kernel/support/popcap/animation/definition.hpp"
#include "kernel/support/popcap/animation/convert/definition.hpp"

namespace Sen::Kernel::Support::PopCap::Animation::Convert {

	class ToFlash : public Common {

		private:

			using XMLDocument = tinyxml2::XMLDocument;

			using XMLElement = tinyxml2::XMLElement;

			using ExtraInfo = ExtraInfo;

			using Common = Common;

			using Animation = SexyAnimation;


		protected:
			
			template <auto point, typename T> requires std::is_integral<T>::value or std::is_floating_point<T>::value
			inline static auto to_fixed(
				T number
			) -> std::string
			{
				auto stream = std::ostringstream{};
				stream << std::fixed << std::setprecision(point) << number;
				return stream.str();
			}

			template <typename T> requires std::is_integral<T>::value or std::is_floating_point<T>::value
			inline static auto constexpr to_fixed_6 = to_fixed<6, T>;

		protected:

			Animation animation;

		protected:

			template <typename T> requires std::is_integral<T>::value or std::is_floating_point<T>::value
			auto write_image(
				XMLDocument* document,
				std::string_view name,
				nlohmann::ordered_json& image,
				T scale
			) -> void
			{
				assert_conditional(image["transform"].get<std::vector<float>>().size() == 6, "transform size must be 6 to write image", "write_image");
				auto DOMSymbolItem = document->NewElement("DOMSymbolItem");
				DOMSymbolItem->SetAttribute("xmlns:xsi", "http://www.w3.org/2001/XMLSchema-instance");
				DOMSymbolItem->SetAttribute("xmlns", "http://ns.adobe.com/xfl/2008/");
				DOMSymbolItem->SetAttribute("name", fmt::format("image/{}", name).data());
				DOMSymbolItem->SetAttribute("symbolType", "graphic");
				auto timeline = document->NewElement("timeline");
				auto DOMTimeline = document->NewElement("DOMTimeline");
				DOMTimeline->SetAttribute("name", name.data());
				auto layers = document->NewElement("layers");
				auto DOMLayer = document->NewElement("DOMLayer");
				auto frames = document->NewElement("frames");
				auto DOMFrame = document->NewElement("DOMFrame");
				DOMFrame->SetAttribute("index", 0);
				auto elements = document->NewElement("elements");
				auto DOMBitmapInstance = document->NewElement("DOMBitmapInstance");
				DOMBitmapInstance->SetAttribute("libraryItemName", fmt::format("media/{}", image["name"].get<std::string>()).data());
				auto matrix = document->NewElement("matrix");
				auto Matrix = document->NewElement("Matrix");
				Matrix->SetAttribute("a", to_fixed_6<double>(image["transform"][0].get<double>() * scale).data());
				Matrix->SetAttribute("b", to_fixed_6<double>(image["transform"][1].get<double>()).data());
				Matrix->SetAttribute("c", to_fixed_6<double>(image["transform"][2].get<double>()).data());
				Matrix->SetAttribute("d", to_fixed_6<double>(image["transform"][3].get<double>() * scale).data());
				Matrix->SetAttribute("tx", to_fixed_6<double>(image["transform"][4].get<double>()).data());
				Matrix->SetAttribute("ty", to_fixed_6<double>(image["transform"][5].get<double>()).data());
				matrix->InsertEndChild(Matrix);
				DOMBitmapInstance->InsertEndChild(matrix);
				elements->InsertEndChild(DOMBitmapInstance);
				DOMFrame->InsertEndChild(elements);
				frames->InsertEndChild(DOMFrame);
				DOMLayer->InsertEndChild(frames);
				layers->InsertEndChild(DOMLayer);
				DOMTimeline->InsertEndChild(layers);
				timeline->InsertEndChild(DOMTimeline);
				DOMSymbolItem->InsertEndChild(timeline);
				document->InsertEndChild(DOMSymbolItem);
				return;
			}

			auto decode_frame_list(
				nlohmann::ordered_json &animation_sprite,
				nlohmann::ordered_json& animation_sprite_map,
				std::vector<std::string>& animation_sprite_name_list
			) -> void
			{
				//auto frame_model = std::map<>();
				return;
			}

		public:

			explicit ToFlash(
				const SexyAnimation& that
			) : animation(that)
			{

			}

			~ToFlash(

			) = default;

			ToFlash(
				ToFlash&& that
			) = delete;

			auto operator =(
				ToFlash&& that
			)->ToFlash & = delete;


			auto process(
				std::string_view destination,
				int resolution
			) -> void
			{
				auto scale_ratio = 1200.0f / static_cast<float>(resolution);
				auto extra_info  = ExtraInfo {
					.version = animation.version,
				};
				for (auto & [key, value] : animation.image.items()) {
					extra_info.group[key] = nlohmann::ordered_json {
						{ "name", value["name"],},
						{ "size", value["size"], },
					};
					auto image_document = XMLDocument{};
					write_image(&image_document, key, value, scale_ratio);
					FileSystem::write_xml(fmt::format("{}/library/image/{}.xml", destination, key), &image_document);
				}
				for (auto& [sprite_name, sprite_value] : animation.sprite.items()) {

				}
				return;
			}

	};
}