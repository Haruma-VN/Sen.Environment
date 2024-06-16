#pragma once

#include "kernel/definition/utility.hpp"
#include "kernel/support/popcap/re_animation/definition.hpp"

namespace Sen::Kernel::Support::PopCap::ReAnimation
{
    using namespace Definition;

    class Encode
    {
    private:
        using XMLDocument = tinyxml2::XMLDocument;

        using XMLElement = tinyxml2::XMLElement;

    public:

        std::unique_ptr<DataStreamView> stream = std::make_unique<DataStreamView>();

    protected:

        inline auto encode_pc(
            const ReanimInfo &reanim
        ) -> void
        {
            stream->writeInt32(reanim_pc_head);
            stream->writeInt32(0);
            const auto &tracks_size = reanim.tracks.size();
            stream->writeInt32(tracks_size);
            stream->writeFloat(reanim.fps);
            stream->writeInt32(0);
            stream->writeInt32(0x0C);
            for (const auto &track : reanim.tracks)
            {
                stream->writeNull(8);
                stream->writeInt32(track.transforms.size());
            }
            for (const auto &track : reanim.tracks)
            {
                stream->writeStringByUint32(track.name);
                stream->writeInt32(0x2C);
                for (const auto &transform : track.transforms)
                {
                    stream->writeFloat(transform.x);
                    stream->writeFloat(transform.y);
                    stream->writeFloat(transform.kx);
                    stream->writeFloat(transform.ky);
                    stream->writeFloat(transform.sx);
                    stream->writeFloat(transform.sy);
                    stream->writeFloat(transform.f);
                    stream->writeFloat(transform.a);
                    stream->writeNull(12);
                }
                for (const auto &transform : track.transforms)
                {
                    stream->writeStringByInt32(transform.i);
                    stream->writeStringByInt32(transform.font);
                    stream->writeStringByInt32(transform.text);
                }
            }
            const auto &length = stream->size();
            const auto &zlib_data = Compression::Zlib::compress<Compression::Zlib::Level::LEVEL_6>(stream->toBytes());
            stream->close();
            stream->writeInt32(reanim_compress_head);
            stream->writeBytes(zlib_data);
            return;
        }

        inline auto encode_tv(
            const ReanimInfo &reanim
        ) -> void
        {
            stream->writeNull(8);
            const auto &tracks_size = reanim.tracks.size();
            stream->writeInt32(tracks_size);
            stream->writeFloat(reanim.fps);
            stream->writeInt32(0);
            stream->writeInt32(0x14);
            for (const auto &track : reanim.tracks)
            {
                stream->writeNull(12);
                stream->writeInt32(track.transforms.size());
                stream->writeNull(4);
            }
            for (const auto &track : reanim.tracks)
            {
                stream->writeStringByUint32(track.name);
                stream->writeInt32(0x30);
                for (const auto &transform : track.transforms)
                {
                    stream->writeFloat(transform.x);
                    stream->writeFloat(transform.y);
                    stream->writeFloat(transform.kx);
                    stream->writeFloat(transform.ky);
                    stream->writeFloat(transform.sx);
                    stream->writeFloat(transform.sy);
                    stream->writeFloat(transform.f);
                    stream->writeFloat(transform.a);
                    stream->writeNull(16);
                }
                for (const auto &transform : track.transforms)
                {
                    stream->writeStringByInt32(transform.i);
                    stream->writeStringByInt32(transform.resource);
                    stream->writeStringByInt32(transform.i2);
                    stream->writeStringByInt32(transform.resource2);
                    stream->writeStringByInt32(transform.font);
                    stream->writeStringByInt32(transform.text);
                }
            }
            const auto &length = stream->size();
            const auto &zlib_data = Compression::Zlib::compress<Compression::Zlib::Level::LEVEL_6>(stream->toBytes());
            stream->close();
            stream->writeInt32(reanim_compress_head);
            stream->writeBytes(zlib_data);
            return;
        }

        inline auto encode_phone_32(
            const ReanimInfo &reanim
        ) -> void
        {
            stream->writeInt32(-14326347);
            stream->writeInt32(0);
            const auto &tracks_size = reanim.tracks.size();
            stream->writeInt32(tracks_size);
            stream->writeFloat(reanim.fps);
            stream->writeInt32(0);
            stream->writeInt32(0x10);
            for (const auto &track : reanim.tracks)
            {
                stream->writeNull(12);
                stream->writeInt32(track.transforms.size());
            }
            for (const auto &track : reanim.tracks)
            {
                stream->writeStringByUint32(track.name);
                stream->writeInt32(0x2C);
                for (const auto &transform : track.transforms)
                {
                    stream->writeFloat(transform.x);
                    stream->writeFloat(transform.y);
                    stream->writeFloat(transform.kx);
                    stream->writeFloat(transform.ky);
                    stream->writeFloat(transform.sx);
                    stream->writeFloat(transform.sy);
                    stream->writeFloat(transform.f);
                    stream->writeFloat(transform.a);
                    stream->writeNull(12);
                }
                for (const auto &transform : track.transforms)
                {
                    stream->writeStringByInt32(transform.i);
                    stream->writeStringByInt32(transform.font);
                    stream->writeStringByInt32(transform.text);
                }
            }
            const auto &length = stream->size();
            const auto &zlib_data = Compression::Zlib::compress<Compression::Zlib::Level::LEVEL_6>(stream->toBytes());
            stream->close();
            stream->writeInt32(reanim_compress_head);
            stream->writeBytes(zlib_data);
            return;
        }

        inline auto encode_phone_64(
            const ReanimInfo &reanim
        ) -> void
        {
            stream->writeInt32(-1069095568);
            stream->writeNull(8);
            const auto &tracks_size = reanim.tracks.size();
            stream->writeInt32(tracks_size);
            stream->writeFloat(reanim.fps);
            stream->writeNull(8);
            stream->writeInt32(0x20);
            for (const auto &track : reanim.tracks)
            {
                stream->writeNull(24);
                stream->writeInt32(track.transforms.size());
                stream->writeNull(0);
            }
            for (const auto &track : reanim.tracks)
            {
                stream->writeStringByUint32(track.name);
                stream->writeInt32(0x38);
                for (const auto &transform : track.transforms)
                {
                    stream->writeFloat(transform.x);
                    stream->writeFloat(transform.y);
                    stream->writeFloat(transform.kx);
                    stream->writeFloat(transform.ky);
                    stream->writeFloat(transform.sx);
                    stream->writeFloat(transform.sy);
                    stream->writeFloat(transform.f);
                    stream->writeFloat(transform.a);
                    stream->writeNull(24);
                }
                for (const auto &transform : track.transforms)
                {
                    stream->writeStringByInt32(transform.i);
                    stream->writeStringByInt32(transform.font);
                    stream->writeStringByInt32(transform.text);
                }
            }
            const auto &length = stream->size();
            const auto &zlib_data = Compression::Zlib::compress<Compression::Zlib::Level::LEVEL_6>(stream->toBytes());
            stream->close();
            stream->writeInt32(reanim_compress_head);
            stream->writeBytes(zlib_data);
            return;
        }

        inline auto encode_game_console(
            const ReanimInfo &reanim
        ) -> void
        {
            auto stream_big = std::unique_ptr<DataStreamViewBigEndian>{};
            stream_big->writeNull(8);
            const auto &tracks_size = reanim.tracks.size();
            stream_big->writeInt32(tracks_size);
            stream_big->writeFloat(reanim.fps);
            stream_big->writeNull(4);
            stream_big->writeInt32(0x0C);
            for (const auto &track : reanim.tracks)
            {
                stream_big->writeNull(8);
                stream_big->writeInt32(track.transforms.size());
            }
            for (const auto &track : reanim.tracks)
            {
                stream_big->writeStringByUint32(track.name);
                stream_big->writeInt32(0x2C);
                for (const auto &transform : track.transforms)
                {
                    stream_big->writeFloat(transform.x);
                    stream_big->writeFloat(transform.y);
                    stream_big->writeFloat(transform.kx);
                    stream_big->writeFloat(transform.ky);
                    stream_big->writeFloat(transform.sx);
                    stream_big->writeFloat(transform.sy);
                    stream_big->writeFloat(transform.f);
                    stream_big->writeFloat(transform.a);
                    stream_big->writeNull(12);
                }
                for (const auto &transform : track.transforms)
                {
                    stream_big->writeStringByInt32(transform.i);
                    stream_big->writeStringByInt32(transform.font);
                    stream_big->writeStringByInt32(transform.text);
                }
            }
            const auto &length = stream_big->size();
            const auto &zlib_data = Compression::Zlib::compress<Compression::Zlib::Level::LEVEL_6>(stream_big->toBytes());
            stream->writeInt32(reanim_compress_head);
            stream->writeBytes(zlib_data);
            return;
        }

        inline auto encode_raw_xml(
            const ReanimInfo &reanim
        ) -> void
        {
            auto document = XMLDocument{};
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
        explicit Encode(

        ) = default;

        ~Encode(

            ) = default;

        inline auto process(
            const ReanimInfo &reanim,
            ReanimPlatform platform) -> void
        {
            switch (platform)
            {
            case ReanimPlatform::PC_Compile:
            {
                encode_pc(reanim);
                return;
            }
            case ReanimPlatform::TV_Compile:
            {
                encode_tv(reanim);
                return;
            }
            case ReanimPlatform::Phone32_Compile:
            {
                encode_phone_32(reanim);
                return;
            }
            case ReanimPlatform::Phone64_Compile:
            {
                encode_phone_64(reanim);
                return;
            }
            case ReanimPlatform::GameConsole_Compile:
            {
                encode_game_console(reanim);
                return;
            }
            case ReanimPlatform::RawXML:
            {
                encode_raw_xml(reanim);
                return;
            }
            default:
                throw Exception(fmt::format("{}", Language::get("popcap.reanim.decode.invalid_reanim_platform")), std::source_location::current(), "process");
            }

            return;
        }

        inline static auto process_fs(
            std::string_view source,
            std::string_view destination,
            ReanimPlatform platform
        ) -> void
        {
            auto encode = Encode{};
            auto reanim_info = *FileSystem::read_json(source);
            encode.process(reanim_info, platform);
            encode.stream->out_file(destination);
            return;
        }
    };

}