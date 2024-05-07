#pragma once

#include "kernel/support/wwise/sound_bank/definition.hpp"

namespace Sen::Kernel::Support::WWise::SoundBank
{
    using namespace Definition;

    struct Decode
    {

    private:

    public:

        inline static auto fnv_hash(
            const std::string &name
        ) -> uint32_t
        {
            auto hash = uint32_t{2166136261};
            for (const auto &c : name)
            {
                hash = (hash * 16777619) ^ static_cast<uint8_t>(tolower(c));
            }
            return hash;
        }

    private:

        inline auto create_hex_string(
            const std::vector<uint8_t> &buffer) -> std::string
        {
            auto oss = std::ostringstream{};
            for (const auto &byte : buffer)
            {
                oss << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(byte) << ' ';
            }
            auto hexString = oss.str();
            std::transform(hexString.cbegin(), hexString.cend(), hexString.begin(), [&](auto c)
                           { return std::toupper(c); });
            return hexString.substr(0, hexString.length() - 1);
        }

    protected:
        DataStreamView stream;

        uint32_t version;

        inline auto decode_bank_header(
            SoundBankInformation *info) -> void
        {
            const auto chuck_size = stream.readUint32();
            const auto bank_version = stream.readUint32();
            if (bank_version <= 26)
            {
                throw Exception("unsupported_bank_version"); // TODO add localization.
            }
            version = bank_version;
            info->bank_header.version = bank_version;
            info->bank_header.soundbank_id = stream.readUint32();
            info->bank_header.language_id = stream.readUint32();
            info->bank_header.head_info.values = stream.readUint32();
            if (version > 76)
            {
                info->bank_header.head_info.project_id = stream.readUint32();
            }
            if (version > 141)
            {
                info->bank_header.head_info.soundbank_type = stream.readUint32();
            }
            auto gap_size = 0;
            if (version <= 76)
            {
                gap_size = chuck_size - 0x10;
            }
            else if (version < 141)
            {
                gap_size = chuck_size - 0x14;
            }
            else
            {
                gap_size = chuck_size;
            }
            info->bank_header.head_info.padding = gap_size;
            stream.read_pos += gap_size;
            return;
        }

        inline auto decode_embedded_media(
            std::string_view destination,
            SoundBankInformation *info) -> void
        {
            const auto chuck_size_end = stream.readUint32() + stream.read_pos;
            auto didx_list = std::vector<uint32_t>{};
            auto data_list = std::vector<DATA>{};
            while (stream.read_pos < chuck_size_end)
            {
                const auto id = stream.readUint32();
                data_list.emplace_back(DATA{
                    .pos = stream.readUint32(),
                    .size = stream.readUint32(),
                });
                didx_list.emplace_back(id);
            }
            if (stream.readString(4) != "DATA"_sv)
            {
                throw Exception(fmt::format("{}", Kernel::Language::get("wwise.soundbank.decode.invalid_wem_data_bank")), std::source_location::current(), "decode_embedded_media");
            }
            const auto data_chuck_size = stream.readUint32();
            const auto data_start_pos = stream.read_pos;
            FileSystem::create_directory(fmt::format("{}/{}", destination, "embedded_audio"));
            for (const auto &i : Range(data_list.size()))
            {
                const auto pos = static_cast<size_t>(data_list[i].pos) + data_start_pos;
                auto wem = stream.getBytes(pos, pos + data_list[i].size);
                FileSystem::write_binary(fmt::format("{}/{}/{}.wem", destination, "embedded_audio", didx_list.at(i)), wem);
            }
            stream.read_pos += data_chuck_size;
            info->embedded_media = std::move(didx_list);
            info->has_embedded_media = true;
            return;
        }

        inline auto decode_plugin_chuck(SoundBankInformation *info) -> void
        {
            const auto chuck_size = stream.readUint32();
            const auto count = stream.readUint32();
            auto init_list = std::vector<INIT>{};
            for (const auto &i : Range(count))
            {
                init_list.emplace_back(INIT{
                    .id = stream.readUint32(),
                    .name = stream.readStringByEmpty()});
            }
            info->initialization = std::move(init_list);
            info->has_initialization = true;
            return;
        }

        inline auto decode_global_settings(SoundBankInformation *info) -> void
        {
            const auto chuck_size = stream.readUint32();
            if (version > 140)
            {
                info->game_synchronization.filter_behavior = stream.readUint16();
            }
            info->game_synchronization.volume_threshold = stream.readFloat();
            if (version > 53)
            {
                info->game_synchronization.max_num_voices_limit_internal = stream.readUint16();
            }
            if (version > 126)
            {
                info->game_synchronization.max_num_dangerous_virt_voices_limit_internal = stream.readUint16();
            }
            auto state_group = std::vector<STMGStageGroup>{};
            const auto state_count = stream.readUint32();
            for (const auto &i : Range(state_count))
            {
                const auto id = stream.readUint32();
                const auto default_transition_time = stream.readUint32();
                auto state = STMGStageGroup{
                    .id = id,
                    .name = "",
                };
                if (version < 52)
                {
                    throw Exception("");
                }
                const auto transitions_count = stream.readUint32();
                state.data.default_transition_time = default_transition_time;
                for (const auto &k : Range(transitions_count))
                {
                    state.data.state_transition.emplace_back(StateTransition{
                        .state_from = stream.readUint32(),
                        .state_to = stream.readUint32(),
                        .transition_time = stream.readUint32()
                    });
                }
                state_group.emplace_back(state);
            }
            auto switch_group = std::vector<STMGSwitchGroup>{};
            const auto switch_count = stream.readUint32();
            for (const auto &i : Range(switch_count))
            {
                const auto id = stream.readUint32();
                auto stmg_switch = STMGSwitchGroup{
                    .id = id,
                    .name = "",
                };
                stmg_switch.data.rtpc_id = stream.readUint32();
                if (version > 89)
                {
                    stmg_switch.data.rtpc_type = stream.readUint8();
                }
                const auto count = stream.readUint32();
                for (const auto &k : Range(count))
                {
                    stmg_switch.data.graph_point.emplace_back(GraphPoint{
                        .from = stream.readFloat(),
                        .to = stream.readFloat(),
                        .interp = stream.readUint32()});
                }
                switch_group.emplace_back(stmg_switch);
            }
            if (version <= 38)
                return;
            auto game_parameter = std::vector<STMGGameParameter>{};
            const auto game_parameter_count = stream.readUint32();
            for (const auto &i : Range(game_parameter_count))
            {
                const auto id = stream.readUint32();
                auto gp = STMGGameParameter{
                    .id = id,
                    .name = "",
                };
                gp.data.value = stream.readFloat();
                if (version > 89)
                {
                    gp.data.ramp_type = stream.readUint32();
                    gp.data.ramp_up = stream.readFloat();
                    gp.data.ramp_down = stream.readFloat();
                    gp.data.built_in_param = stream.readUint8();
                }
                game_parameter.emplace_back(gp);
            }
            auto acoustic_texture = std::vector<STMGAcousticTexture>{};
            if (version <= 118)
            {
                // Nothing to do
            }
            else if (version <= 122)
            {
                const auto texture_count = stream.readUint32();
                for (const auto &i : Range(texture_count))
                {
                    const auto id = stream.readUint32();
                    auto texture = STMGAcousticTexture{
                        .id = id,
                        .name = "",
                    };
                    texture.data.on_off_band[0] = stream.readUint16();
                    texture.data.on_off_band[1] = stream.readUint16();
                    texture.data.on_off_band[2] = stream.readUint16();
                    texture.data.filter_type_band[0] = stream.readUint16();
                    texture.data.filter_type_band[1] = stream.readUint16();
                    texture.data.filter_type_band[2] = stream.readUint16();
                    texture.data.frequecy_band[0] = stream.readUint16();
                    texture.data.frequecy_band[1] = stream.readUint16();
                    texture.data.frequecy_band[2] = stream.readUint16();
                    texture.data.factor_band[0] = stream.readFloat();
                    texture.data.factor_band[1] = stream.readFloat();
                    texture.data.factor_band[2] = stream.readFloat();
                    texture.data.gain_band[0] = stream.readFloat();
                    texture.data.gain_band[1] = stream.readFloat();
                    texture.data.gain_band[2] = stream.readFloat();
                    texture.data.output_gain = stream.readFloat();
                    acoustic_texture.emplace_back(texture);
                }
            }
            else
            {
                const auto texture_count = stream.readUint32();
                for (const auto &i : Range(texture_count))
                {
                    const auto id = stream.readUint32();
                    auto texture = STMGAcousticTexture{
                        .id = id,
                        .name = "",
                    };
                    texture.data.absorption_offset = stream.readFloat();
                    texture.data.absorption_low = stream.readFloat();
                    texture.data.absorption_mid_low = stream.readFloat();
                    texture.data.absorption_mid_high = stream.readFloat();
                    texture.data.absorption_high = stream.readFloat();
                    texture.data.scattering = stream.readFloat();
                    acoustic_texture.emplace_back(texture);
                }
            }
            auto diffuse_reverberator = std::vector<STMGDiffuseReverberator>{};
            if (version <= 118)
            {
                // Nothing to do
            }
            else if (version <= 122)
            {
                const auto reverberator_count = stream.readUint32();
                for (const auto &i : Range(reverberator_count))
                {
                    const auto id = stream.readUint32();
                    auto diffuse = STMGDiffuseReverberator{
                        .id = id,
                        .name = "",
                    };
                    diffuse.data.time = stream.readFloat();
                    diffuse.data.hf_ratio = stream.readFloat();
                    diffuse.data.dry_level = stream.readFloat();
                    diffuse.data.wet_level = stream.readFloat();
                    diffuse.data.spread = stream.readFloat();
                    diffuse.data.density = stream.readFloat();
                    diffuse.data.quality = stream.readFloat();
                    diffuse.data.diffusion = stream.readFloat();
                    diffuse.data.scale = stream.readFloat();
                    diffuse_reverberator.emplace_back(diffuse);
                }
            }
            info->game_synchronization.state_group = std::move(state_group);
            info->game_synchronization.switch_group = std::move(switch_group);
            info->game_synchronization.game_parameter = std::move(game_parameter);
            info->game_synchronization.acoustic_texture = std::move(acoustic_texture);
            info->game_synchronization.diffuse_reverberator = std::move(diffuse_reverberator);
            info->has_game_synchronization = true;
            return;
        }

        inline auto decode_envs_item(ENVSItem &info) -> void
        {
            info.volume.curve_enabled = stream.readBoolean();
            info.volume.curve_scaling = stream.readUint8();
            const auto volume_count = stream.readUint16();
            auto volume_graph_point = std::vector<GraphPoint>{};
            for (const auto &i : Range(volume_count))
            {
                volume_graph_point.emplace_back(GraphPoint{
                    .from = stream.readFloat(),
                    .to = stream.readFloat(),
                    .interp = stream.readUint32()});
            }
            info.volume.graph_point = std::move(volume_graph_point);
            info.low_pass_filter.curve_enabled = stream.readBoolean();
            info.low_pass_filter.curve_scaling = stream.readUint8();
            const auto low_pass_filter_count = stream.readUint16();
            auto low_pass_filter_graph_point = std::vector<GraphPoint>{};
            for (const auto &i : Range(low_pass_filter_count))
            {
                low_pass_filter_graph_point.emplace_back(GraphPoint{
                    .from = stream.readFloat(),
                    .to = stream.readFloat(),
                    .interp = stream.readUint32()});
            }
            info.low_pass_filter.graph_point = std::move(low_pass_filter_graph_point);
            if (version > 89)
            {
                info.high_pass_filter.curve_enabled = stream.readBoolean();
                info.high_pass_filter.curve_scaling = stream.readUint8();
                const auto high_pass_filter_count = stream.readUint16();
                auto high_pass_filter_graph_point = std::vector<GraphPoint>{};
                for (const auto &i : Range(high_pass_filter_count))
                {
                    high_pass_filter_graph_point.emplace_back(GraphPoint{
                        .from = stream.readFloat(),
                        .to = stream.readFloat(),
                        .interp = stream.readUint32()});
                }
                info.high_pass_filter.graph_point = std::move(high_pass_filter_graph_point);
            }
            return;
        }

        inline auto decode_env_settings(SoundBankInformation *info) -> void
        {
            const auto chuck_size = stream.readUint32();
            decode_envs_item(info->environments.obstruction);
            decode_envs_item(info->environments.occlusion);
            info->has_environments = true;
            return;
        }

        inline static auto hirc_type_126 = std::map<uint32_t, std::string>{
            std::pair(0x01, "stateful_property_setting"),
            std::pair(0x02, "sound"),
            std::pair(0x03, "event_action"),
            std::pair(0x04, "event"),
            std::pair(0x05, "sound_playlist_container"),
            std::pair(0x06, "sound_switch_container"),
            std::pair(0x07, "actor_mixer"),
            std::pair(0x08, "audio_bus"),
            std::pair(0x09, "sound_blend_container"),
            std::pair(0x0a, "music_segment"),
            std::pair(0x0b, "music_track"),
            std::pair(0x0c, "music_switch_container"),
            std::pair(0x0d, "music_playlist_container"),
            std::pair(0x0e, "attenuation"),
            std::pair(0x0f, "dialogue_event"),
            std::pair(0x10, "feedback_bus"),
            std::pair(0x11, "feedback_node"),
            std::pair(0x12, "effect"),
            std::pair(0x13, "source"),
            std::pair(0x14, "auxiliary_audio_bus"),
            std::pair(0x15, "low_frequency_oscillator_modulator"),
            std::pair(0x16, "envelope_modulator"),
            std::pair(0x17, "audio_device")};
        inline static auto hirc_type_128 = std::map<uint32_t, std::string>{
            std::pair(0x01, "stateful_property_setting"),
            std::pair(0x02, "sound"),
            std::pair(0x03, "event_action"),
            std::pair(0x04, "event"),
            std::pair(0x05, "sound_playlist_container"),
            std::pair(0x06, "sound_switch_container"),
            std::pair(0x07, "actor_mixer"),
            std::pair(0x08, "audio_bus"),
            std::pair(0x09, "sound_blend_container"),
            std::pair(0x0a, "music_segment"),
            std::pair(0x0b, "music_track"),
            std::pair(0x0c, "music_switch_container"),
            std::pair(0x0d, "music_playlist_container"),
            std::pair(0x0e, "attenuation"),
            std::pair(0x0f, "dialogue_event"),
            std::pair(0x10, "effect"),
            std::pair(0x11, "source"),
            std::pair(0x12, "auxiliary_audio_bus"),
            std::pair(0x13, "low_frequency_oscillator_modulator"),
            std::pair(0x14, "envelope_modulator"),
            std::pair(0x15, "audio_device"),
            std::pair(0x16, "time_modulator")};

        inline auto get_hierarchy_type(uint32_t type_id) -> std::string
        {
            if (version <= 126)
            {
                if (!hirc_type_126.contains(type_id))
                {
                    throw Exception("invaild_type_id"); // TODO add localization;
                }
                return hirc_type_126[type_id];
            }
            else
            {
                if (!hirc_type_128.contains(type_id))
                {
                    throw Exception("invaild_type_id"); // TODO add localization;
                }
                return hirc_type_128[type_id];
            }
        }

        inline auto decode_hierarchy(SoundBankInformation *info) -> void
        {
            const auto chuck_size = stream.readUint32();
            const auto hirc_count = stream.readUint32();
            auto hirc_list = std::vector<HIRC>{};
            for (const auto &i : Range(hirc_count))
            {
                auto hirc_type = uint32_t{};
                if (version <= 48)
                {
                    hirc_type = stream.readUint32();
                }
                else
                {
                    hirc_type = static_cast<uint32_t>(stream.readUint8());
                }
                const auto length = stream.readUint32();
                const auto id = stream.readUint32();
                const auto str_type = get_hierarchy_type(hirc_type);
                auto hirc = HIRC{
                    .id = id,
                    .name = "",
                    .type = str_type};
                if (str_type == "event"_sv)
                {
                    auto child = std::vector<uint32_t>{};
                    const auto child_count = version <= 122 ? stream.readUint32() : stream.readVarUint32();
                    for (const auto &k : Range(child_count))
                    {
                        child.emplace_back(stream.readUint32());
                    }
                    hirc.child = std::move(child);
                }
                else
                {
                    hirc.data = create_hex_string(stream.readBytes(static_cast<size_t>(length - 4)));
                }
                hirc_list.emplace_back(hirc);
            }
            info->hierarchy = std::move(hirc_list);
            info->has_hierarchy = true;
            return;
        }

        inline auto decode_string_mapping(SoundBankInformation *info) -> void
        {
            const auto chuck_size = stream.readUint32();
            info->reference.type = stream.readUint32();
            const auto stid_count = stream.readUint32();
            auto stid_data_list = std::vector<STIDDATA>{};
            for (const auto &i : Range(stid_count))
            {
                stid_data_list.emplace_back(STIDDATA{
                    .id = stream.readUint32(),
                    .name = stream.readStringByUint8()});
            }
            info->reference.data = std::move(stid_data_list);
            info->has_reference = true;
            return;
        }

        inline auto decode_platform_settings(SoundBankInformation *info) -> void
        {
            const auto chuck_size = stream.readUint32();
            info->platform_setting.platform = stream.readStringByEmpty();
            info->has_platform_setting = true;
            return;
        }

        inline auto parse_chuck(
            std::string_view destination,
            SoundBankInformation *info) -> void
        {
            const auto tag = stream.readString(4);
            switch (hash_sv(tag))
            {
            case hash_sv("BKHD"_sv):
            {
                decode_bank_header(info);
                break;
            }
            case hash_sv("DIDX"_sv):
            {
                decode_embedded_media(destination, info);
                break;
            }
            case hash_sv("INIT"_sv):
            {
                decode_plugin_chuck(info);
                break;
            }
            case hash_sv("STMG"_sv):
            {
                decode_global_settings(info);
                break;
            }
            case hash_sv("HIRC"_sv):
            {
                decode_hierarchy(info);
                break;
            }
            case hash_sv("ENVS"_sv):
            {
                decode_env_settings(info);
                break;
            }
            case hash_sv("STID"_sv):
            {
                decode_string_mapping(info);
                break;
            }
            case hash_sv("PLAT"_sv):
            {
                decode_platform_settings(info);
                break;
            }
            case hash_sv("FXPR"_sv):
            {
                throw Exception(fmt::format("{}", Kernel::Language::get("wwise.soundbank.decode.unsupported_fxpr")), std::source_location::current(), "parse_chuck");
            }
            default:
                throw Exception(fmt::format("{} | {}: {:02x}", Kernel::Language::get("wwise.decode.invalid_bnk"), Kernel::Language::get("offset"), stream.read_pos), std::source_location::current(), "parse_chuck");
            }
            return;
        }

    public:
        explicit Decode(
            std::string_view source) : stream(source)
        {
        }

        explicit Decode(
            const std::vector<uint8_t> data) : stream(data)
        {
        }

        ~Decode(

            ) = default;

        inline auto process(
            std::string_view destination,
            SoundBankInformation *info) -> void
        {
            const auto magic = stream.readString(4);
            if (magic != "BKHD"_sv)
            {
                throw Exception(fmt::format("{}", Kernel::Language::get("wwise.soundbank.decode.invalid_bnk_magic")), std::source_location::current(), "process");
            }
            stream.read_pos = 0;
            FileSystem::create_directory(destination);
            while (stream.read_pos < stream.size())
            {
                parse_chuck(destination, info);
            }
            return;
        }

        inline static auto process_fs(
            std::string_view source,
            std::string_view destination) -> void
        {
            auto info = SoundBankInformation{};
            Decode(source).process(destination, &info);
            FileSystem::write_json(fmt::format("{}/{}", destination, "definition.json"), info);
            return;
        }
    };

}