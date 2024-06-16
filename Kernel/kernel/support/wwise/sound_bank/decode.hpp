#pragma once

#include "kernel/definition/utility.hpp"
#include "kernel/support/miscellaneous/shared.hpp"
#include "kernel/support/wwise/sound_bank/definition.hpp"

namespace Sen::Kernel::Support::WWise::SoundBank
{
    using namespace Definition;

    using namespace Sen::Kernel::Support::Miscellaneous::Shared;

    struct Decode : Common
    {
    protected:
        inline static auto exchange_hex_string(
            std::vector<uint8_t> const &buffer) -> std::string
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

        inline static auto exchange_game_synchronization(
            DataStreamView &stream,
            typename STMG & value
        ) -> void
        {
            if (k_version > 140_ui) {
                value.filter_behavior = stream.readUint16();
            }
            value.volume_threshold = stream.readFloat();
            if (k_version > 53_ui) {
                value.max_num_voices_limit_internal = stream.readUint16();
            }
            if (k_version > 126_ui) {
                value.max_num_dangerous_virt_voices_limit_internal = stream.readUint16();
            }
            auto state_count = stream.readUint32();
            value.state_group.resize(static_cast<size_t>(state_count));
            for (auto & state : value.state_group) {
                state.id = stream.readUint32();
                state.data.default_transition_time = stream.readUint32();
                auto transitions_count = stream.readUint32();
                state.data.state_transition.resize(static_cast<size_t>(transitions_count));
                for (auto &state_transition : state.data.state_transition) {
                    state_transition.state_from = stream.readUint32();
                    state_transition.state_to = stream.readUint32();
                    state_transition.transition_time = stream.readUint32();
                }
            }
            auto switch_count = stream.readUint32();
            value.switch_group.resize(static_cast<size_t>(switch_count));
            for (auto & switch_layer : value.switch_group) {
                switch_layer.id = stream.readUint32();
                switch_layer.data.rtpc_id = stream.readUint32();
                if (k_version > 89_ui) {
                    switch_layer.data.rtpc_type = stream.readUint8();
                }
                auto graph_point_count = stream.readUint32();
                exchange_list(stream, switch_layer.data.graph_point, &exchange_to_graph_point, static_cast<size_t>(graph_point_count));
            }
            if (k_version <= 38) return;
            auto game_parameter_count = stream.readUint32();
            value.game_parameter.resize(static_cast<size_t>(game_parameter_count));
            for (auto & game_parameter: value.game_parameter) {
                game_parameter.id = stream.readUint32();
                game_parameter.data.value = stream.readFloat();
                if (k_version > 89_ui) {
                    game_parameter.data.ramp_type = stream.readUint32();
                    game_parameter.data.ramp_up = stream.readFloat();
                    game_parameter.data.ramp_down = stream.readFloat();
                    game_parameter.data.built_in_param = stream.readUint8();
                }
            }
            if (k_version <= 118_ui) return;
            if (k_version <= 122_ui) {
                auto texture_count = stream.readUint32();
                value.acoustic_texture.resize(static_cast<size_t>(texture_count));
                for (auto & texture : value.acoustic_texture) {
                    texture.id = stream.readUint32();
                    texture.data.on_off_band[0] = stream.readUint16();
                    texture.data.on_off_band[1] = stream.readUint16();
                    texture.data.on_off_band[2] = stream.readUint16();
                    texture.data.filter_type_band[0] = stream.readUint16();
                    texture.data.filter_type_band[1] = stream.readUint16();
                    texture.data.filter_type_band[2] = stream.readUint16();
                    texture.data.frequecy_band[0] = stream.readUint16();
                    texture.data.frequecy_band[1] = stream.readUint16();
                    texture.data.frequecy_band[2] = stream.readUint16();
                    texture.data.factor_band[0] = stream.readUint16();
                    texture.data.factor_band[1] = stream.readFloat();
                    texture.data.factor_band[2] = stream.readFloat();
                    texture.data.gain_band[0] = stream.readFloat();
                    texture.data.gain_band[1] = stream.readFloat();
                    texture.data.gain_band[2] = stream.readFloat();
                    texture.data.output_gain = stream.readFloat();
                }
                auto reverberator_count = stream.readUint32();
                value.diffuse_reverberator.resize(static_cast<size_t>(reverberator_count));
                for (auto diffuse : value.diffuse_reverberator) {
                    diffuse.id = stream.readUint32();
                    diffuse.data.time = stream.readFloat();
                    diffuse.data.hf_ratio = stream.readFloat();
                    diffuse.data.dry_level = stream.readFloat();
                    diffuse.data.wet_level = stream.readFloat();
                    diffuse.data.spread = stream.readFloat();
                    diffuse.data.density = stream.readFloat();
                    diffuse.data.diffusion = stream.readFloat();
                    diffuse.data.scale = stream.readFloat();
                }
            }
            else {
                auto texture_count = stream.readUint32();
                value.acoustic_texture.resize(static_cast<size_t>(texture_count));
                for (auto & texture : value.acoustic_texture) {
                    texture.id = stream.readUint32();
                    texture.data.absorption_offset = stream.readFloat();
                    texture.data.absorption_low = stream.readFloat();
                    texture.data.absorption_mid_low = stream.readFloat();
                    texture.data.absorption_mid_high = stream.readFloat();
                    texture.data.absorption_high = stream.readFloat();
                }   
            }
            return;
        }
        

        inline static auto exchange_plugin_chunk(
            DataStreamView &stream,
            std::vector<INIT> & value
        ) -> void
        {
            auto initialization_count = static_cast<size_t>(stream.readUint32());
            exchange_list(stream, value, [](DataStreamView &stream, INIT &element) {  
                element.id = stream.readUint32();
                element.name = stream.readStringByEmpty();
            }, initialization_count);
            return;
        }

        inline static auto exchange_embedded_media(
            DataStreamView &stream,
            std::vector<uint32_t> &value,
            uint32_t const &chuck_size,
            std::string_view destination
        ) -> void
        {
            auto data_list = std::vector<DATA>(chuck_size / (3_size * k_block_size));
            for (auto & element : data_list) {
                value.emplace_back(stream.readUint32());
                element.pos = stream.readUint32();
                element.size = stream.readUint32();
            }
            auto data_sign = ChunkSign{};
            exchange_chuck_sign(stream, data_sign);
            assert_conditional(data_sign.id == ChunkSignFlag::data, fmt::format("{}", Kernel::Language::get("wwise.soundbank.decode.invalid_wem_data_bank")), "exchange_embedded_media");
            auto data_stream = DataStreamView{stream.readBytes(static_cast<size_t>(data_sign.size))};
            for (auto data_index : Range(data_list.size())) {
                auto data = data_stream.getBytes(data_list[data_index].pos, data_list[data_index].pos + data_list[data_index].size);
                write_bytes(fmt::format("{}/embedded_audio/{}.wem", destination, value.at(data_index)), data);
            }
            return;
        }

        inline static auto exchange_bank_header(
            DataStreamView &stream,
            typename BKHD &value,
            uint32_t const &chuck_size
        ) -> void
        {
            auto version = stream.readUint32();
            assert_conditional(version > 52_ui, String::format(fmt::format("{}", Language::get("wwise.soundbank.decode.unsupported_bank_version")), std::to_string(version)), "exchange_bank_header");
            k_version = version;
            value.version = version;
            value.soundbank_id = stream.readUint32();
            value.language_id = stream.readUint32();
            value.head_info.values = stream.readUint32();
            if (k_version > 76_ui) {
                value.head_info.project_id = stream.readUint32();
            }
            if (k_version > 141_ui) {
                value.head_info.soundbank_type = stream.readUint32();
                value.head_info.bank_hash = exchange_hex_string(stream.readBytes(0x10_size));
            }
            auto padding = version < 141_ui ? (version <= 76_ui ? chuck_size - 0x10_ui : chuck_size - 0x14_ui) : chuck_size - 0x28_ui;
            value.head_info.padding = padding;
            stream.read_pos += static_cast<size_t>(padding);
            return;
        }

        inline static auto exchange_hierarchy(
            DataStreamView &stream,
            std::vector<HIRC> &value
        ) -> void
        {
            auto hierarchy_count = stream.readUint32();
            value.resize(static_cast<size_t>(hierarchy_count));
            for (auto &hierarchy : value) {
                auto type = static_cast<uint32_t>(stream.readUint8());
                auto length = stream.readUint32();
                hierarchy.id = stream.readUint32();
                hierarchy.type = exchange_hierarchy_type(type);
                if (hierarchy.type == k_hierarchy_event_type_string) {
                    auto child_count = k_version <= 122_ui ? stream.readUint32() : stream.readVarInt32();
                    hierarchy.child.resize(static_cast<size_t>(child_count));
                    for (auto &child : hierarchy.child) {
                        child = stream.readUint32();
                    }
                }
                else {
                    hierarchy.data = exchange_hex_string(stream.readBytes(static_cast<size_t>(length - 4_size)));
                    if (hierarchy.type == "sound_playlist_container"_sv) {
                        debug(hierarchy.data);
                    }
                }
            }
            return;
        }

        inline static auto exchange_environment_item(
            DataStreamView &stream,
            typename ENVSItem &value
        ) -> void
        {
            value.volume.curve_enabled = stream.readBoolean();
            value.volume.curve_scaling = stream.readUint8();
            auto volume_count = stream.readUint16();
            exchange_list(stream, value.volume.graph_point, &exchange_to_graph_point, static_cast<size_t>(volume_count));
            value.low_pass_filter.curve_enabled = stream.readBoolean();
            value.low_pass_filter.curve_scaling = stream.readUint8();
            auto low_pass_filter_count = stream.readUint16();
            exchange_list(stream, value.low_pass_filter.graph_point, &exchange_to_graph_point, static_cast<size_t>(low_pass_filter_count));
            if (k_version > 89_ui) {
                value.high_pass_filter.curve_enabled = stream.readBoolean();
                value.high_pass_filter.curve_scaling = stream.readUint8();
                auto high_pass_filter_count = stream.readUint16();
                exchange_list(stream, value.high_pass_filter.graph_point, &exchange_to_graph_point, static_cast<size_t>(high_pass_filter_count));
            }
            return;
        }

        inline static auto exchange_environments_settings(
            DataStreamView &stream,
            typename ENVS &value
        ) -> void
        {
            exchange_environment_item(stream, value.obstruction);
            exchange_environment_item(stream, value.occlusion);
            return;
        }

        inline static auto exchange_string_mapping(
            DataStreamView &stream,
            typename STID &value
        ) -> void
        {
            value.type = stream.readUint32();
            auto reference_count = stream.readUint32();
            exchange_list(stream, value.data, [](DataStreamView &stream, STIDDATA &element) {
                element.id = stream.readUint32();
                element.name = stream.readStringByUint8();
            }, static_cast<size_t>(reference_count)
            );
            return;
        }

        inline static auto exchange_platform_settings(
            DataStreamView &stream,
            typename PLAT &value
        ) -> void
        {
            value.platform = stream.readStringByEmpty();
            return;
        }

        inline static auto exchange_sound_bank(
            DataStreamView &stream,
            typename SoundBankInformation &definition,
            std::string_view destination
        ) -> void
        {
            auto bank_header_chunk_sign_flag = stream.readString(4_size);
            assert_conditional(bank_header_chunk_sign_flag == ChunkSignFlag::bkhd, fmt::format("{}", Kernel::Language::get("wwise.soundbank.decode.invalid_bnk_magic")), "process_whole");
            stream.read_pos = k_begin_index;
            while (stream.read_pos < stream.size())
            {
                auto sign = ChunkSign{};
                exchange_chuck_sign(stream, sign);
                if (sign.id == ChunkSignFlag::bkhd) {
                    exchange_bank_header(stream, definition.bank_header, sign.size);
                }
                else if (sign.id == ChunkSignFlag::didx) {
                    exchange_embedded_media(stream, definition.embedded_media, sign.size, destination);
                    definition.has_embedded_media = true;
                }
                else if (sign.id == ChunkSignFlag::init) {
                    exchange_plugin_chunk(stream, definition.initialization);
                    definition.has_initialization = true;
                }
                else if (sign.id == ChunkSignFlag::stmg) {
                    exchange_game_synchronization(stream, definition.game_synchronization);
                    definition.has_game_synchronization = true;
                }
                else if (sign.id == ChunkSignFlag::hirc) {
                    exchange_hierarchy(stream, definition.hierarchy);
                    definition.has_hierarchy = true;
                }
                else if (sign.id == ChunkSignFlag::stid) {
                    exchange_string_mapping(stream, definition.reference);
                    definition.has_reference = true;
                }
                else if (sign.id == ChunkSignFlag::envs) {
                    exchange_environments_settings(stream, definition.environments);
                    definition.has_environments = true;
                }
                else if (sign.id == ChunkSignFlag::plat) {
                    exchange_platform_settings(stream, definition.platform_setting);
                    definition.has_platform_setting = true;
                }
                else {
                    assert_conditional(false, fmt::format("{} | {}: {:02x}", Kernel::Language::get("wwise.decode.invalid_bnk"), Kernel::Language::get("offset"), stream.read_pos), "exchange_sound_bank");
                }
            }
            try_assert(stream.read_pos == stream.size(), "");
            return;
        }

    public:

        inline static auto process_whole(
            DataStreamView &stream,
            typename SoundBankInformation &definition,
            std::string_view destination
        ) -> void
        {
            exchange_sound_bank(stream, definition, destination);
            return;
        }

        inline static auto process_fs(
            std::string_view source,
            std::string_view destination) -> void
        {
            auto stream = DataStreamView{source};
            auto definition = SoundBankInformation{};
            process_whole(stream, definition, destination);
            write_json(fmt::format("{}/{}", destination, "definition.json"), definition);
            return;
        }
    };

}