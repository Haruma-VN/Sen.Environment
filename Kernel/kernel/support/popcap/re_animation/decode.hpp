#pragma once

#include "kernel/definition/utility.hpp"
#include "kernel/support/popcap/re_animation/definition.hpp"
#include "kernel/support/popcap/re_animation/common.hpp"
#include "kernel/support/popcap/zlib/uncompress.hpp"
#include "kernel/definition/basic/string.hpp"

namespace Sen::Kernel::Support::PopCap::ReAnimation
{
    using namespace Definition;

    class Decode : Common
    {
    protected:
        template <auto UseVariant>
        inline auto decode_zlib() -> void
        {
            static_assert(sizeof(UseVariant) == sizeof(bool));
			static_assert(UseVariant == true or UseVariant == false);
            auto magic = stream.readInt32();
            stream.read_pos = 0_size;
            if (magic == zlib_magic || magic == zlib_magic_big)
            {
                auto uncompress = Sen::Kernel::Support::PopCap::Zlib::Uncompress<UseVariant>{};
                auto data = stream.toBytes();
                stream.close();
                stream.writeBytes(uncompress.uncompress(data));
            }
            return;
        }

        // PC
        inline auto decode_pc(
            ReanimInfo &reanim) -> void
        {
            decode_zlib<false>(); 
            stream.read_pos = 8_size;
            auto track_size = stream.readUint32();
            reanim.fps = stream.readFloat();
            stream.read_pos += 4_size;
            auto c_magic = stream.readUint32();
            assert_conditional(c_magic == 0xC, String::format(fmt::format("{}", Language::get("popcap.reanim.decode.invalid_pc_reanim_magic")), String::decimal_to_hexadecimal(c_magic)), "decode_pc");
            for (auto i : Range<int>(track_size))
            {
                stream.read_pos += 8;
                auto track = ReanimTrack{};
                track.transforms.resize(stream.readUint32());
                reanim.tracks.emplace_back(track);
            }
            for (auto &track : reanim.tracks)
            {
                track.name = stream.readStringByUint32();
                auto c_track = stream.readUint32();
                assert_conditional(c_track == 0x2C, String::format(fmt::format("{}", Language::get("popcap.reanim.decode.invalid_pc_reanim_track")), String::decimal_to_hexadecimal(c_track)), "decode_pc");
                for (auto &transform : track.transforms)
                {
                    auto tfloat = stream.readFloat();
                    if (tfloat != transform_tfloat_end)
                    {
                        transform.x = tfloat;
                    }
                    tfloat = stream.readFloat();
                    if (tfloat != transform_tfloat_end)
                    {
                        transform.y = tfloat;
                    }
                    tfloat = stream.readFloat();
                    if (tfloat != transform_tfloat_end)
                    {
                        transform.kx = tfloat;
                    }
                    tfloat = stream.readFloat();
                    if (tfloat != transform_tfloat_end)
                    {
                        transform.ky = tfloat;
                    }
                    tfloat = stream.readFloat();
                    if (tfloat != transform_tfloat_end)
                    {
                        transform.sx = tfloat;
                    }
                    tfloat = stream.readFloat();
                    if (tfloat != transform_tfloat_end)
                    {
                        transform.sy = tfloat;
                    }
                    tfloat = stream.readFloat();
                    if (tfloat != transform_tfloat_end)
                    {
                        transform.f = tfloat;
                    }
                    tfloat = stream.readFloat();
                    if (tfloat != transform_tfloat_end)
                    {
                        transform.a = tfloat;
                    }
                    stream.read_pos += 12;
                }
                for (auto &transform : track.transforms)
                {
                    auto tstring = stream.readStringByUint32();
                    if (tstring.empty())
                    {
                        transform.i = tstring;
                    }
                    tstring = stream.readStringByUint32();
                    if (tstring.empty())
                    {
                        transform.font = tstring;
                    }
                    tstring = stream.readStringByUint32();
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
            decode_zlib<false>();
            stream.read_pos = 8;
            auto track_size = stream.readUint32();
            reanim.fps = stream.readFloat();
            stream.read_pos += 4;
            auto c_magic = stream.readUint32();
            assert_conditional(c_magic == 0x14, String::format(fmt::format("{}", Language::get("popcap.reanim.decode.invalid_tv_reanim_magic")), String::decimal_to_hexadecimal(c_magic)), "decode_tv");
            for (auto i : Range<int>(track_size))
            {
                stream.read_pos += 12;
                auto track = ReanimTrack{};
                track.transforms.resize(stream.readUint32());
                reanim.tracks.emplace_back(track);
                stream.read_pos += 4;
            }
            for (auto &track : reanim.tracks)
            {
                track.name = stream.readStringByUint32();
                auto c_track = stream.readUint32();
                assert_conditional(c_track == 0x30, String::format(fmt::format("{}", Language::get("popcap.reanim.decode.invalid_tv_reanim_track")), String::decimal_to_hexadecimal(c_track)), "decode_tv");
                for (auto &transform : track.transforms)
                {
                    auto tfloat = stream.readFloat();
                    if (tfloat != transform_tfloat_end)
                    {
                        transform.x = tfloat;
                    }
                    tfloat = stream.readFloat();
                    if (tfloat != transform_tfloat_end)
                    {
                        transform.y = tfloat;
                    }
                    tfloat = stream.readFloat();
                    if (tfloat != transform_tfloat_end)
                    {
                        transform.kx = tfloat;
                    }
                    tfloat = stream.readFloat();
                    if (tfloat != transform_tfloat_end)
                    {
                        transform.ky = tfloat;
                    }
                    tfloat = stream.readFloat();
                    if (tfloat != transform_tfloat_end)
                    {
                        transform.sx = tfloat;
                    }
                    tfloat = stream.readFloat();
                    if (tfloat != transform_tfloat_end)
                    {
                        transform.sy = tfloat;
                    }
                    tfloat = stream.readFloat();
                    if (tfloat != transform_tfloat_end)
                    {
                        transform.f = tfloat;
                    }
                    tfloat = stream.readFloat();
                    if (tfloat != transform_tfloat_end)
                    {
                        transform.a = tfloat;
                    }
                    stream.read_pos += 16;
                }
                for (auto &transform : track.transforms)
                {
                    auto tstring = stream.readStringByUint32();
                    if (tstring.empty())
                    {
                        transform.i = tstring;
                    }
                    tstring = stream.readStringByUint32();
                    if (tstring.empty())
                    {
                        transform.resource = tstring;
                    }
                    tstring = stream.readStringByUint32();
                    if (tstring.empty())
                    {
                        transform.i2 = tstring;
                    }
                    tstring = stream.readStringByUint32();
                    if (tstring.empty())
                    {
                        transform.resource2 = tstring;
                    }
                    tstring = stream.readStringByUint32();
                    if (tstring.empty())
                    {
                        transform.font = tstring;
                    }
                    tstring = stream.readStringByUint32();
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
            auto magic = stream.readInt32();
            auto version = stream.readUint16();
            assert_conditional(magic == reanim_xmb_head && version == reanim_xmb_version, String::format(fmt::format("{}", Language::get("popcap.reanim.decode.invalid_wp_reanim_magic")), String::decimal_to_hexadecimal(magic), String::decimal_to_hexadecimal(version)), "decode_wp");
            stream.read_pos += 4;
            auto info = stream.readBytes(0x26);
            for (auto i : Range<int>(0x26))
            {
                assert_conditional(info.at(i) == reanim_xmb_info.at(i), fmt::format("{}", Language::get("popcap.reanim.decode.invalid_reanim_info")), "decode_wp");
            }
            reanim.do_scale = stream.readInt8();
            reanim.fps = stream.readFloat();
            auto track_size = stream.readUint32();
            reanim.tracks.resize(track_size);
            for (auto &track : reanim.tracks)
            {
                track.name = stream.readString(stream.readInt32() << 1);
                auto time = stream.readUint32();
                track.transforms.resize(time);
                for (auto &transform : track.transforms)
                {
                    if (stream.readUint8() != 0_byte)
                        continue;
                    auto tint = stream.readInt32();
                    if (tint != 0)
                    {
                        transform.font = stream.readString(tint << 1);
                    }
                    tint = stream.readInt32();
                    if (tint != 0)
                    {
                        transform.i = stream.readString(tint << 1);
                    }
                    tint = stream.readInt32();
                    if (tint != 0)
                    {
                        transform.text = stream.readString(tint << 1);
                    }
                    auto tfloat = stream.readFloat();
                    if (tfloat != transform_wp_tfloat_end)
                    {
                        transform.a = tfloat;
                    }
                    tfloat = stream.readFloat();
                    if (tfloat != transform_wp_tfloat_end)
                    {
                        transform.f = tfloat;
                    }
                    tfloat = stream.readFloat();
                    if (tfloat != transform_wp_tfloat_end)
                    {
                        transform.sx = tfloat;
                    }
                    tfloat = stream.readFloat();
                    if (tfloat != transform_wp_tfloat_end)
                    {
                        transform.sy = tfloat;
                    }
                    tfloat = stream.readFloat();
                    if (tfloat != transform_wp_tfloat_end)
                    {
                        transform.kx = tfloat;
                    }
                    tfloat = stream.readFloat();
                    if (tfloat != transform_wp_tfloat_end)
                    {
                        transform.ky = tfloat;
                    }
                    tfloat = stream.readFloat();
                    if (tfloat != transform_wp_tfloat_end)
                    {
                        transform.x = tfloat;
                    }
                    tfloat = stream.readFloat();
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
            decode_zlib<false>();
            stream.read_pos = 8;
            auto track_size = stream.readUint32();
            reanim.fps = stream.readFloat();
            stream.read_pos += 4;
            auto c_magic = stream.readUint32();
            assert_conditional(c_magic == 0x10, String::format(fmt::format("{}", Language::get("popcap.reanim.decode.invalid_phone_32_reanim_magic")), String::decimal_to_hexadecimal(c_magic)), "decode_phone_32");
            for (auto i : Range<int>(track_size))
            {
                stream.read_pos += 12;
                auto track = ReanimTrack{};
                track.transforms.resize(stream.readUint32());
                reanim.tracks.emplace_back(track);
            }
            for (auto &track : reanim.tracks)
            {
                track.name = stream.readStringByUint32();
                auto c_track = stream.readUint32();
                assert_conditional(c_track == 0x2C, String::format(fmt::format("{}", Language::get("popcap.reanim.decode.invalid_phone_32_reanim_track")), String::decimal_to_hexadecimal(c_track)), "decode_phone_32");
                for (auto &transform : track.transforms)
                {
                    auto tfloat = stream.readFloat();
                    if (tfloat != transform_tfloat_end)
                    {
                        transform.x = tfloat;
                    }
                    tfloat = stream.readFloat();
                    if (tfloat != transform_tfloat_end)
                    {
                        transform.y = tfloat;
                    }
                    tfloat = stream.readFloat();
                    if (tfloat != transform_tfloat_end)
                    {
                        transform.kx = tfloat;
                    }
                    tfloat = stream.readFloat();
                    if (tfloat != transform_tfloat_end)
                    {
                        transform.ky = tfloat;
                    }
                    tfloat = stream.readFloat();
                    if (tfloat != transform_tfloat_end)
                    {
                        transform.sx = tfloat;
                    }
                    tfloat = stream.readFloat();
                    if (tfloat != transform_tfloat_end)
                    {
                        transform.sy = tfloat;
                    }
                    tfloat = stream.readFloat();
                    if (tfloat != transform_tfloat_end)
                    {
                        transform.f = tfloat;
                    }
                    tfloat = stream.readFloat();
                    if (tfloat != transform_tfloat_end)
                    {
                        transform.a = tfloat;
                    }
                    stream.read_pos += 12;
                }
                for (auto &transform : track.transforms)
                {
                    auto tint = stream.readInt32();
                    if (tint != -1)
                    {
                        transform.i = fmt::format("{}", tint); // string instead of int
                    }
                    auto tstring = stream.readStringByUint32();
                    if (tstring.empty())
                    {
                        transform.font = tstring;
                    }
                    tstring = stream.readStringByUint32();
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
            decode_zlib<true>();
            stream.read_pos = 12;
            auto track_size = stream.readUint32();
            reanim.fps = stream.readFloat();
            stream.read_pos += 8;
            auto c_magic = stream.readUint32();
            assert_conditional(c_magic == 0x20, String::format(fmt::format("{}", Language::get("popcap.reanim.decode.invalid_phone_64_reanim_magic")), String::decimal_to_hexadecimal(c_magic)), "decode_phone_64");
            for (auto i : Range<int>(track_size))
            {
                stream.read_pos += 24;
                auto track = ReanimTrack{};
                track.transforms.resize(stream.readUint32());
                reanim.tracks.emplace_back(track);
                stream.read_pos += 4;
            }
            for (auto &track : reanim.tracks)
            {
                track.name = stream.readStringByUint32();
                auto c_track = stream.readUint32();
                assert_conditional(c_track == 0x38, String::format(fmt::format("{}", Language::get("popcap.reanim.decode.invalid_phone_64_reanim_track")), String::decimal_to_hexadecimal(c_track)), "decode_phone_64");
                for (auto &transform : track.transforms)
                {
                    auto tfloat = stream.readFloat();
                    if (tfloat != transform_tfloat_end)
                    {
                        transform.x = tfloat;
                    }
                    tfloat = stream.readFloat();
                    if (tfloat != transform_tfloat_end)
                    {
                        transform.y = tfloat;
                    }
                    tfloat = stream.readFloat();
                    if (tfloat != transform_tfloat_end)
                    {
                        transform.kx = tfloat;
                    }
                    tfloat = stream.readFloat();
                    if (tfloat != transform_tfloat_end)
                    {
                        transform.ky = tfloat;
                    }
                    tfloat = stream.readFloat();
                    if (tfloat != transform_tfloat_end)
                    {
                        transform.sx = tfloat;
                    }
                    tfloat = stream.readFloat();
                    if (tfloat != transform_tfloat_end)
                    {
                        transform.sy = tfloat;
                    }
                    tfloat = stream.readFloat();
                    if (tfloat != transform_tfloat_end)
                    {
                        transform.f = tfloat;
                    }
                    tfloat = stream.readFloat();
                    if (tfloat != transform_tfloat_end)
                    {
                        transform.a = tfloat;
                    }
                    stream.read_pos += 24;
                }
                for (auto &transform : track.transforms)
                {
                    auto tint = stream.readInt32();
                    if (tint != -1)
                    {
                        transform.i = fmt::format("{}", tint); // string instead of int
                    }
                    auto tstring = stream.readStringByUint32();
                    if (tstring.empty())
                    {
                        transform.font = tstring;
                    }
                    tstring = stream.readStringByUint32();
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
            decode_zlib<false>();
            auto stream_big = DataStreamViewBigEndian{};
            stream_big.writeBytes(stream.toBytes());
            stream_big.read_pos = 8;
            auto track_size = stream_big.readUint32();
            reanim.fps = stream_big.readFloat();
            stream_big.read_pos += 4;
            auto c_magic = stream_big.readUint32();
            assert_conditional(c_magic == 0xC, String::format(fmt::format("{}", Language::get("popcap.reanim.decode.invalid_game_console_reanim_magic")), String::decimal_to_hexadecimal(c_magic)), "decode_game_console");
            for (auto i : Range<int>(track_size))
            {
                stream_big.read_pos += 8;
                auto track = ReanimTrack{};
                track.transforms.resize(stream_big.readUint32());
                reanim.tracks.emplace_back(track);
            }
            for (auto &track : reanim.tracks)
            {
                track.name = stream_big.readStringByUint32();
                auto c_track = stream_big.readUint32();
                assert_conditional(c_track == 0x2C, String::format(fmt::format("{}", Language::get("popcap.reanim.decode.invalid_game_console_reanim_track")), String::decimal_to_hexadecimal(c_track)), "decode_game_console");
                for (auto &transform : track.transforms)
                {
                    auto tfloat = stream_big.readFloat();
                    if (tfloat != transform_tfloat_end)
                    {
                        transform.x = tfloat;
                    }
                    tfloat = stream_big.readFloat();
                    if (tfloat != transform_tfloat_end)
                    {
                        transform.y = tfloat;
                    }
                    tfloat = stream_big.readFloat();
                    if (tfloat != transform_tfloat_end)
                    {
                        transform.kx = tfloat;
                    }
                    tfloat = stream_big.readFloat();
                    if (tfloat != transform_tfloat_end)
                    {
                        transform.ky = tfloat;
                    }
                    tfloat = stream_big.readFloat();
                    if (tfloat != transform_tfloat_end)
                    {
                        transform.sx = tfloat;
                    }
                    tfloat = stream_big.readFloat();
                    if (tfloat != transform_tfloat_end)
                    {
                        transform.sy = tfloat;
                    }
                    tfloat = stream_big.readFloat();
                    if (tfloat != transform_tfloat_end)
                    {
                        transform.f = tfloat;
                    }
                    tfloat = stream_big.readFloat();
                    if (tfloat != transform_tfloat_end)
                    {
                        transform.a = tfloat;
                    }
                    stream_big.read_pos += 12;
                }
                for (auto &transform : track.transforms)
                {
                    auto tstring = stream_big.readStringByUint32();
                    if (tstring.empty())
                    {
                        transform.i = tstring;
                    }
                    tstring = stream_big.readStringByUint32();
                    if (tstring.empty())
                    {
                        transform.font = tstring;
                    }
                    tstring = stream_big.readStringByUint32();
                    if (tstring.empty())
                    {
                        transform.text = tstring;
                    }
                }
            }
            return;
        }

    public:
        DataStreamView stream;

        explicit Decode(
            std::string_view source) : stream(source)
        {
        }

        explicit Decode(
            std::vector<uint8_t> data) : stream(data)
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