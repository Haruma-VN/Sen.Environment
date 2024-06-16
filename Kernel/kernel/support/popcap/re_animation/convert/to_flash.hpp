#pragma once

#include "kernel/definition/utility.hpp"
#include "kernel/support/popcap/re_animation/definition.hpp"

namespace Sen::Kernel::Support::PopCap::ReAnimation::Convert
{
#define M_PIl 3.141592653589793238462643383279502884L

	class ToFlash
	{
	private:
		using XMLDocument = tinyxml2::XMLDocument;

		using XMLElement = tinyxml2::XMLElement;

	protected:
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
		long use_label_name = 0;

		inline auto get_name_by_id(
			const std::string& id,
			const std::string& label,
			int index
		) -> std::string
		{
			if (use_label_name > 0)
			{
				if (index != 0)
				{
					return fmt::format("{}_{}", label, index);
				}
			}
			else if (use_label_name < 0)
			{
				return id;
			}
			else
			{
				if (id.starts_with("IMAGE_REANIM_"))
				{
					return id.substr(13);
				}
				return id;
			}
		}

		inline auto write_sprite(
			const std::string &sprite_name,
			XMLDocument *document
		) -> void
		{
			auto DOMSymbolItem = document->NewElement("DOMSymbolItem");
			DOMSymbolItem->SetAttribute("xmlns:xsi", "http://www.w3.org/2001/XMLSchema-instance");
			DOMSymbolItem->SetAttribute("xmlns", "http://ns.adobe.com/xfl/2008/");
			DOMSymbolItem->SetAttribute("name", sprite_name.data());
			DOMSymbolItem->SetAttribute("symbolType", "graphic");
			auto timeline = document->NewElement("timeline");
			auto DOMTimeline = document->NewElement("DOMTimeline");
			DOMTimeline->SetAttribute("name", sprite_name.data());
			auto layers = document->NewElement("layers");
			auto DOMLayer = document->NewElement("DOMLayer");
			DOMLayer->SetAttribute("name", "1");
			DOMLayer->SetAttribute("color", "#4FFF4F");
			DOMLayer->SetAttribute("current", "true");
			DOMLayer->SetAttribute("isSelected", "true");
			auto frames = document->NewElement("frames");
			auto DOMFrame = document->NewElement("DOMFrame");
			DOMFrame->SetAttribute("index", "0");
			auto elements = document->NewElement("elements");
			auto DOMBitmapInstance = document->NewElement("DOMBitmapInstance");
			DOMBitmapInstance->SetAttribute("libraryItemName", sprite_name.data());
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

	public:
		inline auto process(
			const ReanimInfo &reanim,
			std::string_view destination
		) -> void
		{
			auto document = XMLDocument{};
			auto media = document.NewElement("media");
			auto symbols = document.NewElement("symbols");
			auto layers = document.NewElement("layers");
			for (auto i = reanim.tracks.size() - 1; i >= 0; i--)
			{
				const auto &track = reanim.tracks[i];
				auto index = 0;
				auto initial_transform = ReanimTransform{0, 0, 1, 1, 0, 0, 0, 1};
				auto frame = document.NewElement("frame");
				for (const auto &transform : track.transforms)
				{
					if (transform.x != transform_tfloat_end)
					{
						initial_transform.x = transform.x;
					}
					if (transform.y != transform_tfloat_end)
					{
						initial_transform.y = transform.y;
					}
					if (transform.kx != transform_tfloat_end)
					{
						initial_transform.kx = transform.kx;
					}
					if (transform.ky != transform_tfloat_end)
					{
						initial_transform.ky = transform.ky;
					}
					if (transform.sx != transform_tfloat_end)
					{
						initial_transform.sx = transform.sx;
					}
					if (transform.sy != transform_tfloat_end)
					{
						initial_transform.sy = transform.sy;
					}
					if (transform.f != transform_tfloat_end)
					{
						initial_transform.f = transform.f;
					}
					if (transform.a != transform_tfloat_end)
					{
						initial_transform.a = transform.a;
					}
					if (transform.i.empty())
					{
						auto sprite_name = get_name_by_id(transform.i, track.name, index);
						initial_transform.i = sprite_name;
						auto DOMBitmapItem = document.NewElement("DOMBitmapItem");
						DOMBitmapItem->SetAttribute("name", fmt::format("{}.png", sprite_name).data());
						DOMBitmapItem->SetAttribute("href", fmt::format("{}.png", sprite_name).data());
						media->InsertEndChild(DOMBitmapItem);

						auto Include = document.NewElement("Include");
						Include->SetAttribute("name", fmt::format("{}.png", sprite_name).data());
						symbols->InsertEndChild(Include);
						auto sprtie_document = XMLDocument{};
						write_sprite(sprite_name, &sprtie_document);
						FileSystem::write_xml(fmt::format("{}/library/{}.xml", destination, sprite_name), &sprtie_document);
					}
					auto dx = static_cast<double>(180.0 / M_PIl);
					auto skew_x = initial_transform.kx / dx;
					auto skew_y = initial_transform.ky / dx;
					auto DOMFrame = document.NewElement("DOMFrame");
					DOMFrame->SetAttribute("index", "3");
					auto elements = document.NewElement("elements");
					if (initial_transform.f == -1) {
						 continue;
					}
					auto DOMSymbolInstance = document.NewElement("DOMSymbolInstance");
					auto matrix = document.NewElement("matrix");
					auto Matrix = document.NewElement("Matrix");
					Matrix->SetAttribute("a", to_fixed<6, double>(std::cos(skew_x) * initial_transform.sx).data());
					Matrix->SetAttribute("b", to_fixed<6, double>(std::sin(skew_x) * initial_transform.sx).data());
					Matrix->SetAttribute("c", to_fixed<6, double>(std::sin(skew_y) * initial_transform.sy).data());
					Matrix->SetAttribute("d", to_fixed<6, double>(std::cos(skew_y) * initial_transform.sy).data());
					Matrix->SetAttribute("tx", to_fixed<6, double>(initial_transform.x).data());
					Matrix->SetAttribute("ty", to_fixed<6, double>(initial_transform.y).data());
					matrix->InsertEndChild(Matrix);
					DOMSymbolInstance->InsertEndChild(matrix);
					if (initial_transform.a != 1.0) {
						auto color = document.NewElement("color");
						auto Color = document.NewElement("Color");
						Color->SetAttribute("alphaMultiplier", fmt::format("{}", initial_transform.a).data());
						color->InsertEndChild(Color);
						DOMSymbolInstance->InsertEndChild(color);
					}
					elements->InsertEndChild(DOMSymbolInstance);
					DOMFrame->InsertEndChild(elements);
					frame->InsertEndChild(DOMFrame);
				}
				auto DOMLayer = document.NewElement("DOMLayer");
				DOMLayer->SetAttribute("name", track.name.data());
				layers->InsertEndChild(DOMLayer);
			}
			auto DOMDocument = document.NewElement("DOMDocument");
			DOMDocument->SetAttribute("xmlns:xsi", "http://www.w3.org/2001/XMLSchema-instance");
			DOMDocument->SetAttribute("xmlns", "http://ns.adobe.com/xfl/2008/");
			DOMDocument->SetAttribute("frameRate", fmt::format("{}", reanim.fps).data());
			DOMDocument->SetAttribute("width", "80");
			DOMDocument->SetAttribute("height", "80");
			DOMDocument->SetAttribute("xflVersion", "2.971");
			auto timelines = document.NewElement("timelines");
			auto DOMTimeline = document.NewElement("DOMTimeline");
			DOMTimeline->SetAttribute("name", "animation");
			DOMTimeline->InsertEndChild(layers);
			timelines->InsertEndChild(DOMTimeline);
			DOMDocument->InsertEndChild(media);
			DOMDocument->InsertEndChild(symbols);
			DOMDocument->InsertEndChild(timelines);
			document.InsertEndChild(DOMDocument);
			FileSystem::write_xml(fmt::format("{}/DOMDocument.xml", destination), &document);
			FileSystem::write_file(fmt::format("{}/main.xfl", destination), "PROXY-CS5");
			return;
		}

		inline static auto process_fs(
			std::string_view source,
			std::string_view destination
		) -> void
		{
			auto to_flash = ToFlash{};
			auto reanim = *FileSystem::read_json(source);
			to_flash.process(reanim, destination);
			return;
		}
	};
}