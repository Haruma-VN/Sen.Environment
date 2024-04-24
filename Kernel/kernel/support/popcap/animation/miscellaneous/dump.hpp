#pragma once

#include "kernel/support/popcap/animation/miscellaneous/common.hpp"

namespace Sen::Kernel::Support::PopCap::Animation::Miscellaneous {

	class Dump {

		protected:

			inline auto read_symbols_include(
				XMLElement* document,
				BasicDocument& doc
			) -> void
			{
				assert_conditional(document != nullptr, fmt::format("{}", Language::get("popcap.animation.miscellaneous.symbols_is_null")), "read_symbols_include");
				for (auto child = document->FirstChildElement("Include"); child != nullptr; child = child->NextSiblingElement("Include"))
				{
					assert_conditional(child != nullptr, fmt::format("{}", Language::get("popcap.animation.from_flash.invalid_symbols_include")), "read_symbols_include");
					auto item = std::string{child->FirstAttribute()->Value()};
					if (item.starts_with("image"))
					{
						doc.image.emplace_back(item.substr(6, item.size() - 10));
					}
					if (item.starts_with("sprite"))
					{
						doc.sprite.emplace_back(item.substr(7, item.size() - 11));
					}
					if (item.starts_with("action"))
					{
						doc.action.emplace_back(item.substr(7, item.size() - 11));
					}
				}
				return;
			}

			inline auto read_media(
				XMLElement* document,
				BasicDocument& doc
			) -> void
			{
				assert_conditional(document != nullptr, fmt::format("{}", Language::get("popcap.animation.miscellaneous.media_is_null")), "read_media");
				for (auto child = document->FirstChildElement("DOMBitmapItem"); child != nullptr; child = child->NextSiblingElement("DOMBitmapItem"))
				{
					assert_conditional(child != nullptr, fmt::format("{}", Language::get("popcap.animation.miscellaneous.invalid_media")), "read_media");
					doc.media.emplace_back(child->FindAttribute("name")->Value());
				}
				return;
			}

			inline auto process(
				std::string_view source,
				BasicDocument& doc
			) -> void
			{
				auto xml = XMLDocument{};
				FileSystem::read_xml(source, &xml);
				auto document = xml.FirstChildElement("DOMDocument");
				assert_conditional(document != nullptr, fmt::format("{}", Language::get("popcap.animation.miscellaneous.document_is_null")), "process");
				read_symbols_include(document->FirstChildElement("symbols"), doc);
				read_media(document->FirstChildElement("media"), doc);
				return;
			}

		public:
			explicit Dump(

			) = default;

			~Dump(

			) = default;

			auto operator=(
				Dump &&that
			) -> Dump & = delete;

			Dump(
				Dump &&that
			) = delete;

			inline static auto process_fs(
				std::string_view source,
				BasicDocument& document
			) -> void
			{
				auto dump = Dump{};
				dump.process(fmt::format("{}/DomDocument.xml", source), document);
				return;
			}


	};

	class Generator {
		protected:
			using Image = Miscellaneous::Image;

			using Sprite = Miscellaneous::Sprite;
		public:
			explicit Generator(

			) = default;

			~Generator(

			) = default;

			auto operator=(
				Generator &&that
			) -> Generator & = delete;

			Generator(
				Generator &&that
			) = delete;
		protected:

			inline auto make_image(
				XMLDocument* document,
				Image* image
			) -> void
			{
				auto DOMSymbolItem = document->NewElement("DOMSymbolItem");
				DOMSymbolItem->SetAttribute("xmlns:xsi", "http://www.w3.org/2001/XMLSchema-instance");
				DOMSymbolItem->SetAttribute("xmlns", "http://ns.adobe.com/xfl/2008/");
				DOMSymbolItem->SetAttribute("name", fmt::format("image/{}", image->id).data());
				DOMSymbolItem->SetAttribute("symbolType", "graphic");
				auto timeline = document->NewElement("timeline");
				auto DOMTimeline = document->NewElement("DOMTimeline");
				DOMTimeline->SetAttribute("name", image->id.data());
				auto layers = document->NewElement("layers");
				auto DOMLayer = document->NewElement("DOMLayer");
				auto frames = document->NewElement("frames");
				auto DOMFrame = document->NewElement("DOMFrame");
				DOMFrame->SetAttribute("index", 0);
				auto elements = document->NewElement("elements");
				auto DOMBitmapInstance = document->NewElement("DOMBitmapInstance");
				DOMBitmapInstance->SetAttribute("libraryItemName", fmt::format("media/{}", image->name).data());
				auto matrix = document->NewElement("matrix");
				auto Matrix = document->NewElement("Matrix");
				Matrix->SetAttribute("a", to_fixed<6, double>(image->transform[0]).data());
				Matrix->SetAttribute("b", to_fixed<6, double>(image->transform[1]).data());
				Matrix->SetAttribute("c", to_fixed<6, double>(image->transform[2]).data());
				Matrix->SetAttribute("d", to_fixed<6, double>(image->transform[3]).data());
				Matrix->SetAttribute("tx", to_fixed<6, double>(image->transform[4]).data());
				Matrix->SetAttribute("ty", to_fixed<6, double>(image->transform[5]).data());
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

			inline auto make_sprite(
				XMLDocument* document,
				Sprite* sprite
			) -> void
			{
				auto DOMSymbolItem = document->NewElement("DOMSymbolItem");
				DOMSymbolItem->SetAttribute("xmlns:xsi", "http://www.w3.org/2001/XMLSchema-instance");
				DOMSymbolItem->SetAttribute("xmlns", "http://ns.adobe.com/xfl/2008/");
				DOMSymbolItem->SetAttribute("name", fmt::format("image/{}", sprite->name).data());
				DOMSymbolItem->SetAttribute("symbolType", "graphic");
				auto timeline = document->NewElement("timeline");
				auto DOMTimeline = document->NewElement("DOMTimeline");
				DOMTimeline->SetAttribute("name", sprite->name.data());
				auto layers = document->NewElement("layers");
				auto DOMLayer = document->NewElement("DOMLayer");
				DOMLayer->SetAttribute("name", 1);
				auto frames = document->NewElement("frames");
				auto DOMFrame = document->NewElement("DOMFrame");
				DOMFrame->SetAttribute("index", 0);
				DOMFrame->SetAttribute("duration", 1);
				auto elements = document->NewElement("elements");
				auto DOMSymbolInstance = document->NewElement("DOMSymbolInstance");
				DOMSymbolInstance->SetAttribute("libraryItemName", sprite->link.data());
				DOMSymbolInstance->SetAttribute("symbolType", "graphic");
				DOMSymbolInstance->SetAttribute("loop", "loop");
				auto matrix = document->NewElement("matrix");
				auto Matrix = document->NewElement("Matrix");
				Matrix->SetAttribute("a", to_fixed<6, double>(sprite->transform[0]).data());
				Matrix->SetAttribute("b", to_fixed<6, double>(sprite->transform[1]).data());
				Matrix->SetAttribute("c", to_fixed<6, double>(sprite->transform[2]).data());
				Matrix->SetAttribute("d", to_fixed<6, double>(sprite->transform[3]).data());
				Matrix->SetAttribute("tx", to_fixed<6, double>(sprite->transform[4]).data());
				Matrix->SetAttribute("ty", to_fixed<6, double>(sprite->transform[5]).data());
				matrix->InsertEndChild(Matrix);
				DOMSymbolInstance->InsertEndChild(matrix);
				auto color = document->NewElement("color");
				auto Color = document->NewElement("Color");
				Color->SetAttribute("redMultiplier", to_fixed<6, double>(sprite->color[0]).data());
				Color->SetAttribute("greenMultiplier", to_fixed<6, double>(sprite->color[1]).data());
				Color->SetAttribute("blueMultiplier", to_fixed<6, double>(sprite->color[2]).data());
				Color->SetAttribute("alphaMultiplier", to_fixed<6, double>(sprite->color[3]).data());
				color->InsertEndChild(Color);
				DOMSymbolInstance->InsertEndChild(color);
				elements->InsertEndChild(DOMSymbolInstance);
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

		public:

			inline static auto generate_image(
				std::string_view destination,
				Image* image
			) -> void
			{
				auto generator = Generator{};
				auto xml = XMLDocument{};
				generator.make_image(&xml, image);
				FileSystem::write_xml(destination, &xml);
				return;
			}

			inline static auto generate_sprite(
				std::string_view destination,
				Sprite* sprite
			) -> void
			{
				auto generator = Generator{};
				auto xml = XMLDocument{};
				generator.make_sprite(&xml, sprite);
				FileSystem::write_xml(destination, &xml);
				return;
			}
	};
}