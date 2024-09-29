#pragma once

#include "kernel/definition/utility.hpp"
#include "kernel/support/popcap/re_animation/definition.hpp"
#include "kernel/support/popcap/re_animation/common.hpp"

namespace Sen::Kernel::Support::PopCap::ReAnimation
{
    using namespace Definition;

    class FromXML : Common
    {
    private:
        using XMLDocument = tinyxml2::XMLDocument;

        using XMLElement = tinyxml2::XMLElement;

    protected:
        inline auto convert(
            ReanimInfo &reanim,
            XMLDocument *document) -> void
        {
            auto fps = document->FirstChildElement("fps");
            assert_conditional(fps != nullptr, fmt::format("{}", Language::get("popcap.reanim.decode.invalid_fps_chuck")), "convert");
            reanim.fps = std::stof(fps->FirstChild()->Value());
            auto do_scale = document->FirstChildElement("doScale");
            if (do_scale != nullptr)
            {
                reanim.do_scale = static_cast<int8_t>(std::stoi(do_scale->FirstChild()->Value()));
            }
            for (auto track = document->FirstChildElement("track"); track != nullptr; track = track->NextSiblingElement("track"))
            {
                auto track_info = ReanimTrack{};
                auto name = track->FirstChildElement("name");
                if (name != nullptr)
                {
                    track_info.name = std::string{name->FirstChild()->Value()};
                }
                for (auto transform = track->FirstChildElement("t"); transform != nullptr; transform = transform->NextSiblingElement("t"))
                {
                    auto transform_info = ReanimTransform{};
                    auto x = transform->FirstChildElement("x");
                    if (x != nullptr)
                    {
                        transform_info.x = std::stof(x->FirstChild()->Value());
                    }
                    auto y = transform->FirstChildElement("y");
                    if (y != nullptr)
                    {
                        transform_info.y = std::stof(y->FirstChild()->Value());
                    }
                    auto kx = transform->FirstChildElement("kx");
                    if (kx != nullptr)
                    {
                        transform_info.kx = std::stof(kx->FirstChild()->Value());
                    }
                    auto ky = transform->FirstChildElement("ky");
                    if (ky != nullptr)
                    {
                        transform_info.ky = std::stof(ky->FirstChild()->Value());
                    }
                    auto sx = transform->FirstChildElement("sx");
                    if (sx != nullptr)
                    {
                        transform_info.sx = std::stof(sx->FirstChild()->Value());
                    }
                    auto sy = transform->FirstChildElement("sy");
                    if (sy != nullptr)
                    {
                        transform_info.sy = std::stof(sy->FirstChild()->Value());
                    }
                    auto a = transform->FirstChildElement("a");
                    if (a != nullptr)
                    {
                        transform_info.x = std::stof(a->FirstChild()->Value());
                    }
                    auto i = transform->FirstChildElement("i");
                    if (i != nullptr)
                    {
                        transform_info.i = std::string{i->FirstChild()->Value()};
                    }
                    auto resource = transform->FirstChildElement("resource");
                    if (resource != nullptr)
                    {
                        transform_info.resource = std::string{resource->FirstChild()->Value()};
                    }
                    auto i2 = transform->FirstChildElement("i2");
                    if (i2 != nullptr)
                    {
                        transform_info.i2 = std::string{i2->FirstChild()->Value()};
                    }
                    auto resource2 = transform->FirstChildElement("resource2");
                    if (resource2 != nullptr)
                    {
                        transform_info.resource2 = std::string{resource2->FirstChild()->Value()};
                    }
                    auto font = transform->FirstChildElement("font");
                    if (font != nullptr)
                    {
                        transform_info.font = std::string{font->FirstChild()->Value()};
                    }
                    auto text = transform->FirstChildElement("text");
                    if (text != nullptr)
                    {
                        transform_info.text = std::string{text->FirstChild()->Value()};
                    }
                    track_info.transforms.emplace_back(transform_info);
                }
                reanim.tracks.emplace_back(track_info);
            }
            return;
        }

    public:
        explicit FromXML(

        ) = default;

        ~FromXML(

            ) = default;

        inline auto process(
            ReanimInfo &reanim,
            XMLDocument &document) -> void
        {
            convert(reanim, &document);
            return;
        }

        inline static auto process_fs(
            std::string_view source,
            std::string_view destination
        ) -> void
        {
            auto from_xml = FromXML{};
            auto definition = ReanimInfo{};
            auto document = XMLDocument{};
            FileSystem::read_xml(source, &document);
            from_xml.process(definition, document);
            FileSystem::write_json(destination, definition);
            return;
        }
    };

}