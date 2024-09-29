#pragma once

#include "kernel/definition/utility.hpp"
#include "kernel/support/popcap/re_animation/definition.hpp"
#include "kernel/support/popcap/re_animation/common.hpp"

namespace Sen::Kernel::Support::PopCap::ReAnimation
{
    using namespace Definition;

    class Encode : Common
    {

    protected:
        template <auto UseVariant>
		inline auto encode_zlib(
			const std::vector<unsigned char> &data
		) -> std::vector<unsigned char>
		{
			static_assert(sizeof(UseVariant) == sizeof(bool));
			static_assert(UseVariant == true or UseVariant == false);
			auto compress = Sen::Kernel::Support::PopCap::Zlib::Compress<UseVariant>{};
            return compress.compress(data);
		}

        inline auto encode_pc(
            const ReanimInfo &reanim
        ) -> void
        {
            stream.writeInt32(reanim_pc_head);
            stream.writeInt32(0);
            const auto &tracks_size = reanim.tracks.size();
            stream.writeInt32(tracks_size);
            stream.writeFloat(reanim.fps);
            stream.writeInt32(0);
            stream.writeInt32(0x0C);
            for (const auto &track : reanim.tracks)
            {
                stream.writeNull(8);
                stream.writeInt32(track.transforms.size());
            }
            for (const auto &track : reanim.tracks)
            {
                stream.writeStringByUint32(track.name);
                stream.writeInt32(0x2C);
                for (const auto &transform : track.transforms)
                {
                    stream.writeFloat(transform.x);
                    stream.writeFloat(transform.y);
                    stream.writeFloat(transform.kx);
                    stream.writeFloat(transform.ky);
                    stream.writeFloat(transform.sx);
                    stream.writeFloat(transform.sy);
                    stream.writeFloat(transform.f);
                    stream.writeFloat(transform.a);
                    stream.writeNull(12);
                }
                for (const auto &transform : track.transforms)
                {
                    stream.writeStringByInt32(transform.i);
                    stream.writeStringByInt32(transform.font);
                    stream.writeStringByInt32(transform.text);
                }
            }
            auto data = encode_zlib<false>(stream.toBytes());
			stream.close();
			stream.writeBytes(data);
            return;
        }

        inline auto encode_tv(
            const ReanimInfo &reanim
        ) -> void
        {
            stream.writeNull(8);
            const auto &tracks_size = reanim.tracks.size();
            stream.writeInt32(tracks_size);
            stream.writeFloat(reanim.fps);
            stream.writeInt32(0);
            stream.writeInt32(0x14);
            for (const auto &track : reanim.tracks)
            {
                stream.writeNull(12);
                stream.writeInt32(track.transforms.size());
                stream.writeNull(4);
            }
            for (const auto &track : reanim.tracks)
            {
                stream.writeStringByUint32(track.name);
                stream.writeInt32(0x30);
                for (const auto &transform : track.transforms)
                {
                    stream.writeFloat(transform.x);
                    stream.writeFloat(transform.y);
                    stream.writeFloat(transform.kx);
                    stream.writeFloat(transform.ky);
                    stream.writeFloat(transform.sx);
                    stream.writeFloat(transform.sy);
                    stream.writeFloat(transform.f);
                    stream.writeFloat(transform.a);
                    stream.writeNull(16);
                }
                for (const auto &transform : track.transforms)
                {
                    stream.writeStringByInt32(transform.i);
                    stream.writeStringByInt32(transform.resource);
                    stream.writeStringByInt32(transform.i2);
                    stream.writeStringByInt32(transform.resource2);
                    stream.writeStringByInt32(transform.font);
                    stream.writeStringByInt32(transform.text);
                }
            }
            auto data = encode_zlib<false>(stream.toBytes());
			stream.close();
			stream.writeBytes(data);;
            return;
        }

        inline auto encode_phone_32(
            const ReanimInfo &reanim
        ) -> void
        {
            stream.writeInt32(-14326347);
            stream.writeInt32(0);
            const auto &tracks_size = reanim.tracks.size();
            stream.writeInt32(tracks_size);
            stream.writeFloat(reanim.fps);
            stream.writeInt32(0);
            stream.writeInt32(0x10);
            for (const auto &track : reanim.tracks)
            {
                stream.writeNull(12);
                stream.writeInt32(track.transforms.size());
            }
            for (const auto &track : reanim.tracks)
            {
                stream.writeStringByUint32(track.name);
                stream.writeInt32(0x2C);
                for (const auto &transform : track.transforms)
                {
                    stream.writeFloat(transform.x);
                    stream.writeFloat(transform.y);
                    stream.writeFloat(transform.kx);
                    stream.writeFloat(transform.ky);
                    stream.writeFloat(transform.sx);
                    stream.writeFloat(transform.sy);
                    stream.writeFloat(transform.f);
                    stream.writeFloat(transform.a);
                    stream.writeNull(12);
                }
                for (const auto &transform : track.transforms)
                {
                    stream.writeStringByInt32(transform.i);
                    stream.writeStringByInt32(transform.font);
                    stream.writeStringByInt32(transform.text);
                }
            }
            auto data = encode_zlib<false>(stream.toBytes());
			stream.close();
			stream.writeBytes(data);
            return;
        }

        inline auto encode_phone_64(
            const ReanimInfo &reanim
        ) -> void
        {
            stream.writeInt32(-1069095568);
            stream.writeNull(8);
            const auto &tracks_size = reanim.tracks.size();
            stream.writeInt32(tracks_size);
            stream.writeFloat(reanim.fps);
            stream.writeNull(8);
            stream.writeInt32(0x20);
            for (const auto &track : reanim.tracks)
            {
                stream.writeNull(24);
                stream.writeInt32(track.transforms.size());
                stream.writeNull(0);
            }
            for (const auto &track : reanim.tracks)
            {
                stream.writeStringByUint32(track.name);
                stream.writeInt32(0x38);
                for (const auto &transform : track.transforms)
                {
                    stream.writeFloat(transform.x);
                    stream.writeFloat(transform.y);
                    stream.writeFloat(transform.kx);
                    stream.writeFloat(transform.ky);
                    stream.writeFloat(transform.sx);
                    stream.writeFloat(transform.sy);
                    stream.writeFloat(transform.f);
                    stream.writeFloat(transform.a);
                    stream.writeNull(24);
                }
                for (const auto &transform : track.transforms)
                {
                    stream.writeStringByInt32(transform.i);
                    stream.writeStringByInt32(transform.font);
                    stream.writeStringByInt32(transform.text);
                }
            }
            auto data = encode_zlib<true>(stream.toBytes());
			stream.close();
			stream.writeBytes(data);
            return;
        }

        inline auto encode_game_console(
            const ReanimInfo &reanim
        ) -> void
        {
            auto stream_big = DataStreamViewBigEndian{};
            stream_big.writeNull(8);
            const auto &tracks_size = reanim.tracks.size();
            stream_big.writeInt32(tracks_size);
            stream_big.writeFloat(reanim.fps);
            stream_big.writeNull(4);
            stream_big.writeInt32(0x0C);
            for (const auto &track : reanim.tracks)
            {
                stream_big.writeNull(8);
                stream_big.writeInt32(track.transforms.size());
            }
            for (const auto &track : reanim.tracks)
            {
                stream_big.writeStringByUint32(track.name);
                stream_big.writeInt32(0x2C);
                for (const auto &transform : track.transforms)
                {
                    stream_big.writeFloat(transform.x);
                    stream_big.writeFloat(transform.y);
                    stream_big.writeFloat(transform.kx);
                    stream_big.writeFloat(transform.ky);
                    stream_big.writeFloat(transform.sx);
                    stream_big.writeFloat(transform.sy);
                    stream_big.writeFloat(transform.f);
                    stream_big.writeFloat(transform.a);
                    stream_big.writeNull(12);
                }
                for (const auto &transform : track.transforms)
                {
                    stream_big.writeStringByInt32(transform.i);
                    stream_big.writeStringByInt32(transform.font);
                    stream_big.writeStringByInt32(transform.text);
                }
            }
            stream.writeBytes(stream_big.toBytes());
            return;
        }

    public:
        DataStreamView stream;

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
            encode.stream.out_file(destination);
            return;
        }
    };

}