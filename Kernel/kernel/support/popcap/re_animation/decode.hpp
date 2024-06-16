#pragma once

#include "kernel/definition/utility.hpp"
#include "kernel/support/popcap/re_animation/definition.hpp"
#include "kernel/support/popcap/zlib/uncompress.hpp"
#include "kernel/definition/basic/string.hpp"

namespace Sen::Kernel::Support::PopCap::ReAnimation
{
    using namespace Definition;

    class Decode
    {
    private:
        using XMLDocument = tinyxml2::XMLDocument;

        using XMLElement = tinyxml2::XMLElement;

    protected:
        std::unique_ptr<DataStreamView> stream;

        // PC
        inline auto decode_pc(
            ReanimInfo &reanim) -> void
        {
            const auto &magic = stream->readInt32();
            if (magic == reanim_compress_head)
            {
                const auto &data = Definition::Compression::Zlib::uncompress(stream->readBytes(stream->size() - 8));
                stream->close();
                stream->writeBytes(data);
            }
            stream->read_pos = 8;
            const auto &track_size = stream->readUint32();
            reanim.fps = stream->readFloat();
            stream->read_pos += 4;
            const auto &c_magic = stream->readUint32();
            assert_conditional(c_magic == 0xC, String::format(fmt::format("{}", Language::get("popcap.reanim.decode.invalid_pc_reanim_magic")), String::decimal_to_hexadecimal(c_magic)), "decode_pc");
            for (const auto &i : Range<int>(track_size))
            {
                stream->read_pos += 8;
                auto track = ReanimTrack{};
                track.transforms.resize(stream->readUint32());
                reanim.tracks.emplace_back(track);
            }
            for (auto &track : reanim.tracks)
            {
                track.name = stream->readStringByUint32();
                const auto &c_track = stream->readUint32();
                assert_conditional(c_track == 0x2C, String::format(fmt::format("{}", Language::get("popcap.reanim.decode.invalid_pc_reanim_track")), String::decimal_to_hexadecimal(c_track)), "decode_pc");
                for (auto &transform : track.transforms)
                {
                    auto tfloat = stream->readFloat();
                    if (tfloat != transform_tfloat_end)
                    {
                        transform.x = tfloat;
                    }
                    tfloat = stream->readFloat();
                    if (tfloat != transform_tfloat_end)
                    {
                        transform.y = tfloat;
                    }
                    tfloat = stream->readFloat();
                    if (tfloat != transform_tfloat_end)
                    {
                        transform.kx = tfloat;
                    }
                    tfloat = stream->readFloat();
                    if (tfloat != transform_tfloat_end)
                    {
                        transform.ky = tfloat;
                    }
                    tfloat = stream->readFloat();
                    if (tfloat != transform_tfloat_end)
                    {
                        transform.sx = tfloat;
                    }
                    tfloat = stream->readFloat();
                    if (tfloat != transform_tfloat_end)
                    {
                        transform.sy = tfloat;
                    }
                    tfloat = stream->readFloat();
                    if (tfloat != transform_tfloat_end)
                    {
                        transform.f = tfloat;
                    }
                    tfloat = stream->readFloat();
                    if (tfloat != transform_tfloat_end)
                    {
                        transform.a = tfloat;
                    }
                    stream->read_pos += 12;
                }
                for (auto &transform : track.transforms)
                {
                    auto tstring = stream->readStringByUint32();
                    if (tstring.empty())
                    {
                        transform.i = tstring;
                    }
                    tstring = stream->readStringByUint32();
                    if (tstring.empty())
                    {
                        transform.font = tstring;
                    }
                    tstring = stream->readStringByUint32();
                    if (tstring.empty())
                    {
                        transform.text = tstring;
                    }
                }
            }
            return;
        }

        // TV
        inline auto decode_tv(
            ReanimInfo &reanim) -> void
        {
            const auto &magic = stream->readInt32();
            if (magic == reanim_compress_head)
            {
                const auto &data = Definition::Compression::Zlib::uncompress(stream->readBytes(stream->size() - 8));
                stream->close();
                stream->writeBytes(data);
            }
            stream->read_pos = 8;
            const auto &track_size = stream->readUint32();
            reanim.fps = stream->readFloat();
            stream->read_pos += 4;
            const auto &c_magic = stream->readUint32();
            assert_conditional(c_magic == 0x14, String::format(fmt::format("{}", Language::get("popcap.reanim.decode.invalid_tv_reanim_magic")), String::decimal_to_hexadecimal(c_magic)), "decode_tv");
            for (const auto &i : Range<int>(track_size))
            {
                stream->read_pos += 12;
                auto track = ReanimTrack{};
                track.transforms.resize(stream->readUint32());
                reanim.tracks.emplace_back(track);
                stream->read_pos += 4;
            }
            for (auto &track : reanim.tracks)
            {
                track.name = stream->readStringByUint32();
                const auto &c_track = stream->readUint32();
                assert_conditional(c_track == 0x30, String::format(fmt::format("{}", Language::get("popcap.reanim.decode.invalid_tv_reanim_track")), String::decimal_to_hexadecimal(c_track)), "decode_tv");
                for (auto &transform : track.transforms)
                {
                    auto tfloat = stream->readFloat();
                    if (tfloat != transform_tfloat_end)
                    {
                        transform.x = tfloat;
                    }
                    tfloat = stream->readFloat();
                    if (tfloat != transform_tfloat_end)
                    {
                        transform.y = tfloat;
                    }
                    tfloat = stream->readFloat();
                    if (tfloat != transform_tfloat_end)
                    {
                        transform.kx = tfloat;
                    }
                    tfloat = stream->readFloat();
                    if (tfloat != transform_tfloat_end)
                    {
                        transform.ky = tfloat;
                    }
                    tfloat = stream->readFloat();
                    if (tfloat != transform_tfloat_end)
                    {
                        transform.sx = tfloat;
                    }
                    tfloat = stream->readFloat();
                    if (tfloat != transform_tfloat_end)
                    {
                        transform.sy = tfloat;
                    }
                    tfloat = stream->readFloat();
                    if (tfloat != transform_tfloat_end)
                    {
                        transform.f = tfloat;
                    }
                    tfloat = stream->readFloat();
                    if (tfloat != transform_tfloat_end)
                    {
                        transform.a = tfloat;
                    }
                    stream->read_pos += 16;
                }
                for (auto &transform : track.transforms)
                {
                    auto tstring = stream->readStringByUint32();
                    if (tstring.empty())
                    {
                        transform.i = tstring;
                    }
                    tstring = stream->readStringByUint32();
                    if (tstring.empty())
                    {
                        transform.resource = tstring;
                    }
                    tstring = stream->readStringByUint32();
                    if (tstring.empty())
                    {
                        transform.i2 = tstring;
                    }
                    tstring = stream->readStringByUint32();
                    if (tstring.empty())
                    {
                        transform.resource2 = tstring;
                    }
                    tstring = stream->readStringByUint32();
                    if (tstring.empty())
                    {
                        transform.font = tstring;
                    }
                    tstring = stream->readStringByUint32();
                    if (tstring.empty())
                    {
                        transform.text = tstring;
                    }
                }
            }
            return;
        }
        /*
        // WP
        inline auto decode_wp(
            ReanimInfo &reanim
        ) -> void
        {
            const auto &magic = stream->readInt32();
            const auto &version = stream->readUint16();
            assert_conditional(magic == reanim_xmb_head && version == reanim_xmb_version, String::format(fmt::format("{}", Language::get("popcap.reanim.decode.invalid_wp_reanim_magic")), String::decimal_to_hexadecimal(magic), String::decimal_to_hexadecimal(version)), "decode_wp");
            stream->read_pos += 4;
            const auto &info = stream->readBytes(0x26);
            for (const auto &i : Range<int>(0x26))
            {
                assert_conditional(info.at(i) == reanim_xmb_info.at(i), fmt::format("{}", Language::get("popcap.reanim.decode.invalid_reanim_info")), "decode_wp");
            }
            reanim.do_scale = stream->readInt8();
            reanim.fps = stream->readFloat();
            const auto &track_size = stream->readUint32();
            reanim.tracks.resize(track_size);
            for (auto &track : reanim.tracks)
            {
                track.name = stream->readString(stream->readInt32() << 1);
                const auto &time = stream->readUint32();
                track.transforms.resize(time);
                for (auto &transform : track.transforms)
                {
                    if (stream->readUint8() != 0_byte)
                        continue;
                    auto tint = stream->readInt32();
                    if (tint != 0)
                    {
                        transform.font = stream->readString(tint << 1);
                    }
                    tint = stream->readInt32();
                    if (tint != 0)
                    {
                        transform.i = stream->readString(tint << 1);
                    }
                    tint = stream->readInt32();
                    if (tint != 0)
                    {
                        transform.text = stream->readString(tint << 1);
                    }
                    auto tfloat = stream->readFloat();
                    if (tfloat != transform_wp_tfloat_end)
                    {
                        transform.a = tfloat;
                    }
                    tfloat = stream->readFloat();
                    if (tfloat != transform_wp_tfloat_end)
                    {
                        transform.f = tfloat;
                    }
                    tfloat = stream->readFloat();
                    if (tfloat != transform_wp_tfloat_end)
                    {
                        transform.sx = tfloat;
                    }
                    tfloat = stream->readFloat();
                    if (tfloat != transform_wp_tfloat_end)
                    {
                        transform.sy = tfloat;
                    }
                    tfloat = stream->readFloat();
                    if (tfloat != transform_wp_tfloat_end)
                    {
                        transform.kx = tfloat;
                    }
                    tfloat = stream->readFloat();
                    if (tfloat != transform_wp_tfloat_end)
                    {
                        transform.ky = tfloat;
                    }
                    tfloat = stream->readFloat();
                    if (tfloat != transform_wp_tfloat_end)
                    {
                        transform.x = tfloat;
                    }
                    tfloat = stream->readFloat();
                    if (tfloat != transform_wp_tfloat_end)
                    {
                        transform.y = tfloat;
                    }
                }
            }
            return;
        }
        */
        // Phone 32
        inline auto decode_phone_32(
            ReanimInfo &reanim) -> void
        {
            const auto &magic = stream->readInt32();
            if (magic == reanim_compress_head)
            {
                stream->read_pos += 4;
                const auto &data = Definition::Compression::Zlib::uncompress(stream->readBytes(stream->size() - 8));
                stream->close();
                stream->writeBytes(data);
            }
            stream->read_pos = 8;
            const auto &track_size = stream->readUint32();
            reanim.fps = stream->readFloat();
            stream->read_pos += 4;
            const auto &c_magic = stream->readUint32();
            assert_conditional(c_magic == 0x10, String::format(fmt::format("{}", Language::get("popcap.reanim.decode.invalid_phone_32_reanim_magic")), String::decimal_to_hexadecimal(c_magic)), "decode_phone_32");
            for (const auto &i : Range<int>(track_size))
            {
                stream->read_pos += 12;
                auto track = ReanimTrack{};
                track.transforms.resize(stream->readUint32());
                reanim.tracks.emplace_back(track);
            }
            for (auto &track : reanim.tracks)
            {
                track.name = stream->readStringByUint32();
                const auto &c_track = stream->readUint32();
                assert_conditional(c_track == 0x2C, String::format(fmt::format("{}", Language::get("popcap.reanim.decode.invalid_phone_32_reanim_track")), String::decimal_to_hexadecimal(c_track)), "decode_phone_32");
                for (auto &transform : track.transforms)
                {
                    auto tfloat = stream->readFloat();
                    if (tfloat != transform_tfloat_end)
                    {
                        transform.x = tfloat;
                    }
                    tfloat = stream->readFloat();
                    if (tfloat != transform_tfloat_end)
                    {
                        transform.y = tfloat;
                    }
                    tfloat = stream->readFloat();
                    if (tfloat != transform_tfloat_end)
                    {
                        transform.kx = tfloat;
                    }
                    tfloat = stream->readFloat();
                    if (tfloat != transform_tfloat_end)
                    {
                        transform.ky = tfloat;
                    }
                    tfloat = stream->readFloat();
                    if (tfloat != transform_tfloat_end)
                    {
                        transform.sx = tfloat;
                    }
                    tfloat = stream->readFloat();
                    if (tfloat != transform_tfloat_end)
                    {
                        transform.sy = tfloat;
                    }
                    tfloat = stream->readFloat();
                    if (tfloat != transform_tfloat_end)
                    {
                        transform.f = tfloat;
                    }
                    tfloat = stream->readFloat();
                    if (tfloat != transform_tfloat_end)
                    {
                        transform.a = tfloat;
                    }
                    stream->read_pos += 12;
                }
                for (auto &transform : track.transforms)
                {
                    auto tint = stream->readInt32();
                    if (tint != -1)
                    {
                        transform.i = fmt::format("{}", tint); // string instead of int
                    }
                    auto tstring = stream->readStringByUint32();
                    if (tstring.empty())
                    {
                        transform.font = tstring;
                    }
                    tstring = stream->readStringByUint32();
                    if (tstring.empty())
                    {
                        transform.text = tstring;
                    }
                }
            }
            return;
        }

        // Phone 64
        inline auto decode_phone_64(
            ReanimInfo &reanim) -> void
        {
            const auto &magic = stream->readInt32();
            if (magic == reanim_compress_head)
            {
                stream->read_pos += 16;
                const auto &data = Definition::Compression::Zlib::uncompress(stream->readBytes(stream->size() - 8));
                stream->close();
                stream->writeBytes(data);
            }
            stream->read_pos = 12;
            const auto &track_size = stream->readUint32();
            reanim.fps = stream->readFloat();
            stream->read_pos += 8;
            const auto &c_magic = stream->readUint32();
            assert_conditional(c_magic == 0x20, String::format(fmt::format("{}", Language::get("popcap.reanim.decode.invalid_phone_64_reanim_magic")), String::decimal_to_hexadecimal(c_magic)), "decode_phone_64");
            for (const auto &i : Range<int>(track_size))
            {
                stream->read_pos += 24;
                auto track = ReanimTrack{};
                track.transforms.resize(stream->readUint32());
                reanim.tracks.emplace_back(track);
                stream->read_pos += 4;
            }
            for (auto &track : reanim.tracks)
            {
                track.name = stream->readStringByUint32();
                const auto &c_track = stream->readUint32();
                assert_conditional(c_track == 0x38, String::format(fmt::format("{}", Language::get("popcap.reanim.decode.invalid_phone_64_reanim_track")), String::decimal_to_hexadecimal(c_track)), "decode_phone_64");
                for (auto &transform : track.transforms)
                {
                    auto tfloat = stream->readFloat();
                    if (tfloat != transform_tfloat_end)
                    {
                        transform.x = tfloat;
                    }
                    tfloat = stream->readFloat();
                    if (tfloat != transform_tfloat_end)
                    {
                        transform.y = tfloat;
                    }
                    tfloat = stream->readFloat();
                    if (tfloat != transform_tfloat_end)
                    {
                        transform.kx = tfloat;
                    }
                    tfloat = stream->readFloat();
                    if (tfloat != transform_tfloat_end)
                    {
                        transform.ky = tfloat;
                    }
                    tfloat = stream->readFloat();
                    if (tfloat != transform_tfloat_end)
                    {
                        transform.sx = tfloat;
                    }
                    tfloat = stream->readFloat();
                    if (tfloat != transform_tfloat_end)
                    {
                        transform.sy = tfloat;
                    }
                    tfloat = stream->readFloat();
                    if (tfloat != transform_tfloat_end)
                    {
                        transform.f = tfloat;
                    }
                    tfloat = stream->readFloat();
                    if (tfloat != transform_tfloat_end)
                    {
                        transform.a = tfloat;
                    }
                    stream->read_pos += 24;
                }
                for (auto &transform : track.transforms)
                {
                    auto tint = stream->readInt32();
                    if (tint != -1)
                    {
                        transform.i = fmt::format("{}", tint); // string instead of int
                    }
                    auto tstring = stream->readStringByUint32();
                    if (tstring.empty())
                    {
                        transform.font = tstring;
                    }
                    tstring = stream->readStringByUint32();
                    if (tstring.empty())
                    {
                        transform.text = tstring;
                    }
                }
            }
            return;
        }

        // GameConsole
        inline auto decode_game_console(
            ReanimInfo &reanim) -> void
        {
            auto stream_big = std::unique_ptr<DataStreamViewBigEndian>{};
            std::copy(stream_big->begin(), stream_big->end(), stream->begin());
            const auto &magic = stream_big->readInt32();
            if (magic == reanim_compress_head)
            {
                stream_big->read_pos += 4;
                const auto &data = Definition::Compression::Zlib::uncompress(stream_big->readBytes(stream_big->size() - 8));
                stream_big->close();
                stream_big->writeBytes(data);
            }
            stream_big->read_pos = 8;
            const auto &track_size = stream_big->readUint32();
            reanim.fps = stream_big->readFloat();
            stream_big->read_pos += 4;
            const auto &c_magic = stream_big->readUint32();
            assert_conditional(c_magic == 0xC, String::format(fmt::format("{}", Language::get("popcap.reanim.decode.invalid_game_console_reanim_magic")), String::decimal_to_hexadecimal(c_magic)), "decode_game_console");
            for (const auto &i : Range<int>(track_size))
            {
                stream_big->read_pos += 8;
                auto track = ReanimTrack{};
                track.transforms.resize(stream_big->readUint32());
                reanim.tracks.emplace_back(track);
            }
            for (auto &track : reanim.tracks)
            {
                track.name = stream_big->readStringByUint32();
                const auto &c_track = stream_big->readUint32();
                assert_conditional(c_track == 0x2C, String::format(fmt::format("{}", Language::get("popcap.reanim.decode.invalid_game_console_reanim_track")), String::decimal_to_hexadecimal(c_track)), "decode_game_console");
                for (auto &transform : track.transforms)
                {
                    auto tfloat = stream_big->readFloat();
                    if (tfloat != transform_tfloat_end)
                    {
                        transform.x = tfloat;
                    }
                    tfloat = stream_big->readFloat();
                    if (tfloat != transform_tfloat_end)
                    {
                        transform.y = tfloat;
                    }
                    tfloat = stream_big->readFloat();
                    if (tfloat != transform_tfloat_end)
                    {
                        transform.kx = tfloat;
                    }
                    tfloat = stream_big->readFloat();
                    if (tfloat != transform_tfloat_end)
                    {
                        transform.ky = tfloat;
                    }
                    tfloat = stream_big->readFloat();
                    if (tfloat != transform_tfloat_end)
                    {
                        transform.sx = tfloat;
                    }
                    tfloat = stream_big->readFloat();
                    if (tfloat != transform_tfloat_end)
                    {
                        transform.sy = tfloat;
                    }
                    tfloat = stream_big->readFloat();
                    if (tfloat != transform_tfloat_end)
                    {
                        transform.f = tfloat;
                    }
                    tfloat = stream_big->readFloat();
                    if (tfloat != transform_tfloat_end)
                    {
                        transform.a = tfloat;
                    }
                    stream_big->read_pos += 12;
                }
                for (auto &transform : track.transforms)
                {
                    auto tstring = stream_big->readStringByUint32();
                    if (tstring.empty())
                    {
                        transform.i = tstring;
                    }
                    tstring = stream_big->readStringByUint32();
                    if (tstring.empty())
                    {
                        transform.font = tstring;
                    }
                    tstring = stream_big->readStringByUint32();
                    if (tstring.empty())
                    {
                        transform.text = tstring;
                    }
                }
            }
            return;
        }

        inline auto decode_raw_xml(
            ReanimInfo &reanim) -> void
        {
            auto document = XMLDocument{};
            const auto &data = stream->toString();
            auto status_code = document.Parse(data.data(), data.size());
            assert_conditional(status_code == tinyxml2::XML_SUCCESS, "xml.read_error", "decode_raw_xml");
            auto fps = document.FirstChildElement("fps");
            assert_conditional(fps == nullptr, fmt::format("{}", Language::get("popcap.reanim.decode.invalid_fps_chuck")), "decode_raw_xml");
            reanim.fps = std::stof(fps->FirstChild()->Value());
            auto do_scale = document.NextSiblingElement("doScale");
            if (do_scale != nullptr)
            {
                reanim.do_scale = static_cast<int8_t>(std::stoi(do_scale->FirstChild()->Value()));
            }
            auto tracks = document.NextSiblingElement("track");
            for (auto track = tracks->FirstChildElement(); track != nullptr; track = track->NextSiblingElement())
            {
                auto track_info = ReanimTrack{};
                auto name = track->FirstChildElement("name");
                if (name != nullptr)
                {
                    track_info.name = std::string{name->FirstChild()->Value()};
                }

                auto transforms = track->NextSiblingElement("t");
                for (auto transform = transforms->FirstChildElement(); transform != nullptr; transform = transform->NextSiblingElement())
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
        explicit Decode(
            std::string_view source) : stream(std::make_unique<DataStreamView>(source))
        {
        }

        explicit Decode(
            DataStreamView &it) : stream(&it)
        {
        }

        ~Decode(

            ) = default;

        inline auto process(
            ReanimInfo &reanim,
            ReanimPlatform platform) -> void
        {
            switch (platform)
            {
            case ReanimPlatform::PC_Compile:
            {
                decode_pc(reanim);
                return;
            }
            case ReanimPlatform::TV_Compile:
            {
                decode_tv(reanim);
                return;
            }
            case ReanimPlatform::Phone32_Compile:
            {
                decode_phone_32(reanim);
                return;
            }
            case ReanimPlatform::Phone64_Compile:
            {
                decode_phone_64(reanim);
                return;
            }
            case ReanimPlatform::GameConsole_Compile:
            {
                decode_game_console(reanim);
                return;
            }
            case ReanimPlatform::RawXML:
            {
                decode_raw_xml(reanim);
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
            auto decode = Decode{source};
            auto reanim_info = ReanimInfo{};
            decode.process(reanim_info, platform);
            FileSystem::write_json(destination, reanim_info);
            return;
        }
    };

}