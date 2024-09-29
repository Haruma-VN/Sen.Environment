#pragma once

#include "kernel/definition/utility.hpp"
#include "kernel/support/popcap/re_animation/definition.hpp"
#include "kernel/support/popcap/re_animation/common.hpp"

namespace Sen::Kernel::Support::PopCap::ReAnimation
{
    using namespace Definition;

    class ToXML : Common
    {
    private:
        using XMLDocument = tinyxml2::XMLDocument;

        using XMLElement = tinyxml2::XMLElement;

    protected:
        inline auto convert(
            ReanimInfo const &reanim,
            XMLDocument &document
        ) -> void
        {
            auto fps = document.NewElement("fps");
            fps->InsertEndChild(document.NewText(fmt::format("{}", reanim.fps).data()));
            auto doScale = document.NewElement("doScale");
            doScale->InsertEndChild(document.NewText(fmt::format("{}", reanim.do_scale).data()));
            document.InsertEndChild(fps);
            document.InsertEndChild(doScale);
            for (const auto & track : reanim.tracks) {
                auto track_element = document.NewElement("track");
                auto name = document.NewElement("name");
                name->InsertEndChild(document.NewText(fmt::format("{}", track.name).data()));
                track_element->InsertEndChild(name);
                for (const auto & transform : track.transforms) {
                    auto transform_element = document.NewElement("t");
                    if (transform.x != transform_tfloat_end) {
                        auto x = document.NewElement("x");
                        x->InsertEndChild(document.NewText(fmt::format("{}", transform.x).data()));
                        transform_element->InsertEndChild(x);
                    }
                    if (transform.y != transform_tfloat_end) {
                        auto y = document.NewElement("y");
                        y->InsertEndChild(document.NewText(fmt::format("{}", transform.y).data()));
                        transform_element->InsertEndChild(y);
                    }
                    if (transform.kx != transform_tfloat_end) {
                        auto kx = document.NewElement("kx");
                        kx->InsertEndChild(document.NewText(fmt::format("{}", transform.kx).data()));
                        transform_element->InsertEndChild(kx);
                    }
                    if (transform.ky != transform_tfloat_end) {
                        auto ky = document.NewElement("ky");
                        ky->InsertEndChild(document.NewText(fmt::format("{}", transform.ky).data()));
                        transform_element->InsertEndChild(ky);
                    }
                    if (transform.sx != transform_tfloat_end) {
                        auto sx = document.NewElement("sx");
                        sx->InsertEndChild(document.NewText(fmt::format("{}", transform.sx).data()));
                        transform_element->InsertEndChild(sx);
                    }
                    if (transform.sy != transform_tfloat_end) {
                        auto sy = document.NewElement("sy");
                        sy->InsertEndChild(document.NewText(fmt::format("{}", transform.sy).data()));
                        transform_element->InsertEndChild(sy);
                    }
                    if (transform.f != transform_tfloat_end) {
                        auto f = document.NewElement("f");
                        f->InsertEndChild(document.NewText(fmt::format("{}", transform.f).data()));
                        transform_element->InsertEndChild(f);
                    }
                    if (transform.a != transform_tfloat_end) {
                        auto a = document.NewElement("a");
                        a->InsertEndChild(document.NewText(fmt::format("{}", transform.a).data()));
                        transform_element->InsertEndChild(a);
                    }
                    if (!transform.i.empty()) {
                        auto i = document.NewElement("i");
                        i->InsertEndChild(document.NewText(fmt::format("{}", transform.i).data()));
                        transform_element->InsertEndChild(i);
                    }   
                    if (!transform.resource.empty()) {
                        auto resource = document.NewElement("resource");
                        resource->InsertEndChild(document.NewText(fmt::format("{}", transform.resource).data()));
                        transform_element->InsertEndChild(resource);
                    }
                    if (!transform.i2.empty()) {
                        auto i2 = document.NewElement("i2");
                        i2->InsertEndChild(document.NewText(fmt::format("{}", transform.i2).data()));
                        transform_element->InsertEndChild(i2);
                    }
                    if (!transform.resource2.empty()) {
                        auto resource2 = document.NewElement("resource2");
                        resource2->InsertEndChild(document.NewText(fmt::format("{}", transform.resource2).data()));
                        transform_element->InsertEndChild(resource2);
                    }
                    if (!transform.font.empty()) {
                        auto font = document.NewElement("font");
                        font->InsertEndChild(document.NewText(fmt::format("{}", transform.font).data()));
                        transform_element->InsertEndChild(font);
                    }
                    if (!transform.text.empty()) {
                        auto text = document.NewElement("text");
                        text->InsertEndChild(document.NewText(fmt::format("{}", transform.text).data()));
                        transform_element->InsertEndChild(text);
                    }
                    track_element->InsertEndChild(transform_element);
                }
                document.InsertEndChild(track_element);
            }
            return;
        }

    public:
        explicit ToXML(

        ) = default;

        ~ToXML(

            ) = default;

        inline auto process(
            const ReanimInfo &reanim,
            XMLDocument &document) -> void
        {
            convert(reanim, document);
            return;
        }

        inline static auto process_fs(
            std::string_view source,
            std::string_view destination
        ) -> void
        {
            auto to_xml = ToXML{};
            auto definition = *FileSystem::read_json(source);
            auto document = XMLDocument{};
            to_xml.process(definition, document);
            FileSystem::write_xml(destination, &document);
            return;
        }
    };

}