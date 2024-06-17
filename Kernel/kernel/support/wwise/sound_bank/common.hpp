#pragma once

#include "kernel/support/wwise/sound_bank/definition.hpp"

namespace Sen::Kernel::Support::WWise::SoundBank
{
    using namespace Definition;

    struct Common
    {
        inline static auto k_version = uint32_t{0};

        inline static constexpr auto k_hierarchy_event_type_string = "event"_sv;

        inline static constexpr auto k_data_block_padding_size = 0x10_size;

        struct ChunkSignFlag {

			inline static constexpr auto bkhd = "BKHD"_sv;

			inline static constexpr auto didx = "DIDX"_sv;

			inline static constexpr auto data = "DATA"_sv;

			inline static constexpr auto init = "INIT"_sv;

			inline static constexpr auto stmg = "STMG"_sv;

			inline static constexpr auto hirc = "HIRC"_sv;

			inline static constexpr auto stid = "STID"_sv;

			inline static constexpr auto envs = "ENVS"_sv;

			inline static constexpr auto plat = "PLAT"_sv;

		};

        struct ChunkSign {
            std::string_view id;
            uint32_t pos;
            uint32_t size;
        };

        inline static auto exchange_chuck_sign(
            DataStreamView &stream,
            ChunkSign& value
        ) -> void
        {
            value.id = stream.readString(4);
            value.size = stream.readUint32();
            return;
        }

        inline static auto exchange_chuck_sign(
            ChunkSign const &value,
            DataStreamView &stream
        ) -> void
        {   
            stream.writeString(value.id);
            stream.writeUint32(value.size);
            return;
        }
        
        inline static auto exchange_to_graph_point(
            DataStreamView &stream,
            GraphPoint & value
        ) -> void
        {
            value.from = stream.readFloat();
            value.to = stream.readFloat();
            value.interp = stream.readUint32();
            return;
        }

        inline static auto exchange_from_graph_point(
            DataStreamView &stream,
            GraphPoint const & value
        ) -> void
        {
            stream.writeFloat(value.from);
            stream.writeFloat(value.to);
            stream.writeUint32(value.interp);
            return;
        }

        inline static auto fnv_hash(
            const std::string &name) -> uint32_t
        {
            auto hash = uint32_t{2166136261};
            for (const auto &c : name)
            {
                hash = (hash * 16777619) ^ static_cast<uint8_t>(tolower(c));
            }
            return hash;
        }

        inline static auto hirc_type_index_map_bank_version_126 = std::map<uint32_t, std::string_view>{
            std::pair(0x01, "stateful_property_setting"_sv),
            std::pair(0x02, "sound"_sv),
            std::pair(0x03, "event_action"_sv),
            std::pair(0x04, "event"_sv),
            std::pair(0x05, "sound_playlist_container"_sv),
            std::pair(0x06, "sound_switch_container"_sv),
            std::pair(0x07, "actor_mixer"_sv),
            std::pair(0x08, "audio_bus"_sv),
            std::pair(0x09, "sound_blend_container"_sv),
            std::pair(0x0a, "music_segment"_sv),
            std::pair(0x0b, "music_track"_sv),
            std::pair(0x0c, "music_switch_container"_sv),
            std::pair(0x0d, "music_playlist_container"_sv),
            std::pair(0x0e, "attenuation"_sv),
            std::pair(0x0f, "dialogue_event"_sv),
            std::pair(0x10, "feedback_bus"_sv),
            std::pair(0x11, "feedback_node"_sv),
            std::pair(0x12, "effect"_sv),
            std::pair(0x13, "source"_sv),
            std::pair(0x14, "auxiliary_audio_bus"_sv),
            std::pair(0x15, "low_frequency_oscillator_modulator"_sv),
            std::pair(0x16, "envelope_modulator"_sv),
            std::pair(0x17, "audio_device")
        };

        inline static auto hirc_type_index_map_bank_version_128 = std::map<uint32_t, std::string_view>{
            std::pair(0x01, "stateful_property_setting"_sv),
            std::pair(0x02, "sound"_sv),
            std::pair(0x03, "event_action"_sv),
            std::pair(0x04, "event"_sv),
            std::pair(0x05, "sound_playlist_container"_sv),
            std::pair(0x06, "sound_switch_container"_sv),
            std::pair(0x07, "actor_mixer"_sv),
            std::pair(0x08, "audio_bus"_sv),
            std::pair(0x09, "sound_blend_container"_sv),
            std::pair(0x0a, "music_segment"_sv),
            std::pair(0x0b, "music_track"_sv),
            std::pair(0x0c, "music_switch_container"_sv),
            std::pair(0x0d, "music_playlist_container"_sv),
            std::pair(0x0e, "attenuation"_sv),
            std::pair(0x0f, "dialogue_event"_sv),
            std::pair(0x10, "effect"_sv),
            std::pair(0x11, "source"_sv),
            std::pair(0x12, "auxiliary_audio_bus"_sv),
            std::pair(0x13, "low_frequency_oscillator_modulator"_sv),
            std::pair(0x14, "envelope_modulator"_sv),
            std::pair(0x15, "audio_device"_sv),
            std::pair(0x16, "time_modulator")
        };

        inline static auto exchange_hierarchy_type(
            uint32_t const &data
        ) -> std::string_view
        {
            if (k_version <= uint32_t{126})
            {
                assert_conditional(hirc_type_index_map_bank_version_126.contains(data), String::format(fmt::format("{}", Language::get("wwise.soundbank.decode.invalid_type_id")), std::to_string(data)), "exchange_hierarchy_type");
                return hirc_type_index_map_bank_version_126[data];
            }
            else
            {
                assert_conditional(hirc_type_index_map_bank_version_128.contains(data), String::format(fmt::format("{}", Language::get("wwise.soundbank.decode.invalid_type_id")), std::to_string(data)), "exchange_hierarchy_type");
                return hirc_type_index_map_bank_version_128[data];
            }
        }

        inline static auto hirc_type_string_map_bank_version_126 = std::map<std::string_view, uint32_t>{
            std::pair("stateful_property_setting"_sv, 0x01),
            std::pair("sound"_sv, 0x02),
            std::pair("event_action"_sv, 0x03),
            std::pair("event"_sv, 0x04),
            std::pair("sound_playlist_container"_sv, 0x05),
            std::pair("sound_switch_container"_sv, 0x06),
            std::pair("actor_mixer"_sv, 0x07),
            std::pair("audio_bus"_sv, 0x08),
            std::pair("sound_blend_container"_sv, 0x09),
            std::pair("music_segment"_sv, 0x0a),
            std::pair("music_track"_sv, 0x0b),
            std::pair("music_switch_container"_sv, 0x0c),
            std::pair("music_playlist_container"_sv, 0x0d),
            std::pair("attenuation"_sv, 0x0e),
            std::pair("dialogue_event"_sv, 0x0f),
            std::pair("feedback_bus"_sv, 0x10),
            std::pair("feedback_node"_sv, 0x11),
            std::pair("effect"_sv, 0x12),
            std::pair("source"_sv, 0x13),
            std::pair("auxiliary_audio_bus"_sv, 0x14),
            std::pair("low_frequency_oscillator_modulator"_sv, 0x15),
            std::pair("envelope_modulator"_sv, 0x16),
            std::pair("audio_device"_sv, 0x17)};

        inline static auto hirc_type_string_map_bank_version_128 = std::map<std::string_view, uint32_t>{
            std::pair("stateful_property_setting"_sv, 0x01),
            std::pair("sound"_sv, 0x02),
            std::pair("event_action"_sv, 0x03),
            std::pair("event"_sv, 0x04),
            std::pair("sound_playlist_container"_sv, 0x05),
            std::pair("sound_switch_container"_sv, 0x06),
            std::pair("actor_mixer"_sv, 0x07),
            std::pair("audio_bus"_sv, 0x08),
            std::pair("sound_blend_container"_sv, 0x09),
            std::pair("music_segment"_sv, 0x0a),
            std::pair("music_track"_sv, 0x0b),
            std::pair("music_switch_container"_sv, 0x0c),
            std::pair("music_playlist_container"_sv, 0x0d),
            std::pair("attenuation"_sv, 0x0e),
            std::pair("dialogue_event"_sv, 0x0f),
            std::pair("effect"_sv, 0x10),
            std::pair("source"_sv, 0x11),
            std::pair("auxiliary_audio_bus"_sv, 0x12),
            std::pair("low_frequency_oscillator_modulator"_sv, 0x13),
            std::pair("envelope_modulator"_sv, 0x14),
            std::pair("audio_device"_sv, 0x15),
            std::pair("time_modulator"_sv, 0x16)};

		inline static auto exchange_hierarchy_type(
            std::string_view data
        ) -> uint32_t
        {
            if (k_version <= uint32_t{126})
            {
				assert_conditional(hirc_type_string_map_bank_version_126.contains(data), String::format(fmt::format("{}", Language::get("wwise.soundbank.encode.invalid_type"), data)), "exchange_hierarchy_type");
				return hirc_type_string_map_bank_version_126[data];
            }
            else
            {
				assert_conditional(hirc_type_string_map_bank_version_128.contains(data), String::format(fmt::format("{}", Language::get("wwise.soundbank.encode.invalid_type"), data)), "exchange_hierarchy_type");
                return hirc_type_string_map_bank_version_128[data];
            }
        }
    };

}