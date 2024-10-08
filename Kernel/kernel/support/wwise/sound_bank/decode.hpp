#pragma once

#include "kernel/definition/utility.hpp"
#include "kernel/support/miscellaneous/shared.hpp"
#include "kernel/support/wwise/sound_bank/definition.hpp"

namespace Sen::Kernel::Support::WWise::SoundBank
{
    using namespace Definition;

    using namespace Sen::Kernel::Support::Miscellaneous::Shared;

    template <typename Value>
    concept IsValidArgument = (std::is_same<Value, bool>::value || std::is_enum<Value>::value) && true;

    struct Decode : Common
    {
    protected:
        inline static auto exchange_data(
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

        inline static auto exchange_section(
            DataStreamView &stream,
            StateGroup &value) -> void
        {
            value.id = stream.readUint32();
            value.default_transition = stream.readUint32();
            exchange_list(stream, value.custom_transition, [](auto &data, auto &value)
                          {
                value.from = data.readUint32();
                value.to = data.readUint32();
                value.time = data.readUint32(); }, stream.readUint32());
            return;
        }

        //--------------------------

        inline static auto exchange_section(
            DataStreamView &stream,
            SwitchGroup &value) -> void
        {
            value.id = stream.readUint32();
            value.parameter.id = stream.readUint32();
            if (k_version >= 112_ui)
            {
                exchange_bit_multi<uint8_t>(stream, value.parameter.category);
            }
            exchange_list(stream, value.point, [](auto &data, auto &value)
                          {
                value.position.x = data.readFloat();
                value.position.y = data.readUint32();
                exchange_bit_multi<uint32_t>(data, value.curve); }, stream.readUint32());
            return;
        }

        //---------------------------

        inline static auto exchange_section(
            DataStreamView &stream,
            GameParameter &value) -> void
        {
            value.id = stream.readUint32();
            value.range_default = stream.readFloat();
            if (k_version >= 112_ui)
            {
                exchange_bit_multi<uint32_t>(stream, value.interpolation_mode);
                value.interpolation_attack = stream.readFloat();
                value.interpolation_release = stream.readFloat();
                exchange_bit_multi<uint8_t>(stream, value.bind_to_built_in_parameter);
            }
            return;
        }

        //--------------------------

        inline static auto exchange_section(
            DataStreamView &stream,
            GameSynchronizationU1 &value) -> void
        {
            value.id = stream.readUint32();
            if (k_version >= 140_ui)
            {
                value.u1 = stream.readFloat();
                value.u2 = stream.readFloat();
                value.u3 = stream.readFloat();
                value.u4 = stream.readFloat();
                value.u5 = stream.readFloat();
                value.u6 = stream.readFloat();
            }
            return;
        }

        //--------------------------

        inline static auto exchange_game_synchronization(
            DataStreamView &stream,
            Setting &value,
            GameSynchronization &game_synchronization_value) -> void
        {
            if (k_version >= 145_ui)
            {
                exchange_bit_multi<uint16_t>(stream, value.voice_filter_behavior);
            }
            value.volume_threshold = stream.readFloat();
            value.maximum_voice_instance = stream.readUint16();
            if (k_version >= 128_ui)
            {
                exchange_raw_constant(stream, 50_us);
            }
            exchange_list(stream, game_synchronization_value.state_group, [](auto &data, auto &value)
                          { exchange_section(data, value); }, stream.readUint32());
            exchange_list(stream, game_synchronization_value.switch_group, [](auto &data, auto &value)
                          { exchange_section(data, value); }, stream.readUint32());
            exchange_list(stream, game_synchronization_value.game_parameter, [](auto &data, auto &value)
                          { exchange_section(data, value); }, stream.readUint32());
            if (k_version >= 120_ui)
            {
                if (k_version < 125_ui)
                {
                    exchange_raw_constant(stream, 0_ui);
                    exchange_raw_constant(stream, 0_ui);
                }
                else if (k_version < 140_ui)
                {
                    exchange_raw_constant(stream, 0_ui);
                }
                else
                {
                    exchange_list(stream, game_synchronization_value.u1, [](auto &data, auto &value)
                                  { exchange_section(data, value); }, stream.readUint32());
                }
            }
            return;
        }

        inline static auto exchange_plugin_chunk(
            DataStreamView &stream,
            Setting &value) -> void
        {
            if (k_version >= 118_ui)
            {
                exchange_list(stream, value.plug_in, [](auto &data, auto &value)
                              {
                            value.id = data.readUint32();
                if (k_version < 140_ui) {
                    auto length = data.readUint32();
                    assert_conditional(length - 1_ui > k_none_size, fmt::format("{}", Language::get("wwise.sound_bank.invalid_size")), "exchange_plugin_chunk");
                    value.library = data.readString(length - 1_ui);
                    exchange_raw_constant(data, 0_byte); // skip last empty byte.
                }
                else {
                    value.library = data.readStringByEmpty();
                } }, stream.readUint32());
            }
            return;
        }

        inline static auto exchange_media(
            DataStreamView &stream,
            std::vector<uint32_t> &value,
            uint32_t const &chuck_size,
            std::string_view destination) -> void
        {
            auto data_list = std::vector<DATA>(chuck_size / (3_size * k_block_size));
            for (auto &element : data_list)
            {
                value.emplace_back(stream.readUint32());
                element.pos = stream.readUint32();
                element.size = stream.readUint32();
            }
            auto data_sign = ChunkSign{};
            exchange_chuck_sign(stream, data_sign);
            assert_conditional(data_sign.id == ChunkSignFlag::data, fmt::format("{}", Kernel::Language::get("wwise.soundbank.decode.invalid_wem_data_bank")), "exchange_media");
            auto data_stream = DataStreamView{stream.readBytes(static_cast<size_t>(data_sign.size))};
            for (auto data_index : Range(data_list.size()))
            {
                auto data = data_stream.getBytes(data_list[data_index].pos, data_list[data_index].pos + data_list[data_index].size);
                write_bytes(fmt::format("{}/media/{}.wem", destination, value.at(data_index)), data);
            }
            return;
        }

        inline static auto exchange_bank_header(
            DataStreamView &stream,
            BankHeader &value,
            uint32_t const &chuck_size) -> void
        {
            auto version = stream.readUint32();
            assert_conditional(version > 72_ui, String::format(fmt::format("{}", Language::get("wwise.soundbank.decode.unsupported_bank_version")), std::to_string(version)), "exchange_bank_header");
            k_version = version;
            value.version = version;
            value.soundbank_id = stream.readUint32();
            value.language_id = stream.readUint32();
            value.head_info.values = stream.readUint32();
            if (k_version > 76_ui)
            {
                value.head_info.project_id = stream.readUint32();
            }
            if (k_version > 141_ui)
            {
                value.head_info.soundbank_type = stream.readUint32();
                value.head_info.bank_hash = exchange_data(stream.readBytes(0x10_size));
            }
            auto padding = version < 141_ui ? (version <= 76_ui ? chuck_size - 0x10_ui : chuck_size - 0x14_ui) : chuck_size - 0x28_ui;
            value.head_info.padding = padding;
            stream.read_pos += static_cast<size_t>(padding);
            return;
        }

        //------------------------------------

        template <auto flag_count, typename ArgsValue, typename... Args>
            requires((IsValidArgument<Args> && ...) && IsValidArgument<ArgsValue>)
        inline static auto exchange_bit(
            size_t current_index,
            std::bitset<flag_count> const &flag,
            ArgsValue &value,
            Args &...args) -> void
        {
            if constexpr (std::is_same<ArgsValue, bool>::value)
            {
                value = flag.test(current_index);
                ++current_index;
            }
            else if constexpr (std::is_enum<ArgsValue>::value)
            {
                auto index_value = uint8_t{};
                for (auto bit_index : Range(exchange_enumeration<ArgsValue, size_t>(value)))
                {
                    index_value |= static_cast<uint8_t>(flag.test(current_index)) << bit_index;
                    ++current_index;
                }
                value = exchange_enumeration<uint8_t, ArgsValue>(index_value);
            }
            if constexpr (sizeof...(args) > 0_size)
            {
                exchange_bit(current_index, flag, args...);
            }
            return;
        }

        template <typename RawValue>
            requires std::is_arithmetic<RawValue>::value
        inline static auto exchange_raw_constant(
            DataStreamView &stream,
            RawValue const &value) -> void
        {
            assert_conditional(value == stream.read_of<RawValue>(), fmt::format("invalid_raw_constant_value_at_pos: {}. size: {}", stream.read_pos, sizeof(RawValue)), "exchange_raw_constant"); // TODO.
        }

        template <typename RawValue, typename... Args>
            requires(IsValidArgument<Args> && ...)
        inline static auto exchange_bit_multi(
            DataStreamView &stream,
            Args &...args) -> void
        {
            exchange_bit(k_begin_index, std::bitset<static_cast<size_t>(sizeof(RawValue) * 8_size)>(static_cast<u_long>(stream.read_of<RawValue>())), args...);
            return;
        }

        //------------------------------------

        template <auto type, typename Value>
            requires(std::is_same<Value, bool>::value || std::is_enum<Value>::value || std::is_arithmetic<Value>::value) && std::is_enum<decltype(type)>::value
        inline static auto convert_common_property(
            CommonPropertyMap<decltype(type)> const &map,
            Value &value) -> void
        {
            if (auto it = std::find_if(map.regular.begin(), map.regular.end(), [&](auto &element)
                                       { return element.first == type; });
                it != map.regular.end())
            {
                if constexpr (std::is_enum<Value>::value)
                {
                    value = exchange_enumeration<uint8_t, Value>(static_cast<uint8_t>(std::any_cast<uint32_t>(it->second)));
                }
                else
                {
                    value = std::any_cast<Value>(it->second);
                }
            }
            else
            {
                auto any_type = declare_common_property_type<decltype(type)>(type);
                if constexpr (std::is_enum<Value>::value)
                {
                    value = static_cast<Value>(std::any_cast<uint32_t>(any_type));
                }
                else
                {
                    value = std::any_cast<Value>(any_type);
                }
            }
            return;
        }

        //------------------------------------

        template <auto type, typename Value>
            requires(std::is_same<Value, bool>::value || std::is_enum<Value>::value || std::is_arithmetic<Value>::value) && std::is_enum<decltype(type)>::value
        inline static auto convert_common_property_as_regular(
            CommonPropertyMap<decltype(type)> const &map,
            Value &value) -> void
        {
            if (auto it = std::find_if(map.regular.begin(), map.regular.end(), [&](auto &element)
                                       { return element.first == type; });
                it != map.regular.end())
            {
                value = std::any_cast<Value>(it->second);
            }
            else
            {
                value = std::any_cast<Value>(declare_common_property_type<decltype(type)>(type));
            }
            return;
        }

        //------------------------------------

        template <auto type, typename Value>
            requires(std::is_same<Value, bool>::value || std::is_enum<Value>::value || std::is_arithmetic<Value>::value) && std::is_enum<decltype(type)>::value
        inline static auto convert_common_property_as_randomizable(
            CommonPropertyMap<decltype(type)> const &map,
            RandomizableValue<Value> &value) -> void
        {
            if (auto it = std::find_if(map.regular.begin(), map.regular.end(), [&](auto &element)
                                       { return element.first == type; });
                it != map.regular.end())
            {

                value.value = std::any_cast<Value>(it->second);
            }
            else
            {
                value.value = std::any_cast<Value>(declare_common_property_type<decltype(type)>(type));
            }
            if (auto it = std::find_if(map.randomizer.begin(), map.randomizer.end(), [&](auto &element)
                                       { return element.first == type; });
                it != map.randomizer.end())
            {
                value.minimum_value = std::any_cast<Value>(it->second[0]);
                value.maximum_value = std::any_cast<Value>(it->second[1]);
            }
            else
            {
                value.minimum_value = static_cast<Value>(0);
                value.maximum_value = static_cast<Value>(0);
            }
            return;
        }

        //------------------------------------
        inline static auto exchange_common_property_value(
            DataStreamView &stream,
            std::any &value) -> void
        {
            if (value.type() == typeid(bool))
            {
                auto bool_value = false;
                exchange_bit_multi<uint32_t>(stream, bool_value);
                value = bool_value;
            }
            if (value.type() == typeid(uint8_t))
            {
                value = static_cast<uint8_t>(stream.readUint32());
            }
            if (value.type() == typeid(float))
            {
                value = stream.readFloat();
            }
            if (value.type() == typeid(double))
            {
                value = stream.readDouble();
            }
            if (value.type() == typeid(uint32_t))
            {
                value = stream.readUint32();
            }
            return;
        }

        //------------------------------------

        inline static auto load_common_property(
            CommonPropertyMap<AudioCommonPropertyType> const &map,
            AudioOutputBusSetting &value) -> void
        {
            convert_common_property_as_regular<AudioCommonPropertyType::output_bus_volume>(map, value.volume);
            convert_common_property_as_regular<AudioCommonPropertyType::output_bus_low_pass_filter>(map, value.low_pass_filter);
            if (k_version >= 112_ui)
            {
                convert_common_property_as_regular<AudioCommonPropertyType::output_bus_high_pass_filter>(map, value.high_pass_filter);
            }
            return;
        }

        //------------------------------------

        inline static auto load_common_property(
            CommonPropertyMap<AudioCommonPropertyType> const &map,
            AudioVoice &value) -> void
        {
            convert_common_property_as_randomizable<AudioCommonPropertyType::voice_volume>(map, value.volume);
            convert_common_property_as_randomizable<AudioCommonPropertyType::voice_pitch>(map, value.pitch);
            convert_common_property_as_randomizable<AudioCommonPropertyType::voice_low_pass_filter>(map, value.low_pass_filter);
            if (k_version >= 112_ui)
            {
                convert_common_property_as_randomizable<AudioCommonPropertyType::voice_high_pass_filter>(map, value.high_pass_filter);
            }
            return;
        }

        //------------------------------------

        inline static auto load_common_property(
            CommonPropertyMap<AudioCommonPropertyType> const &map,
            AudioVoiceVolumeGainSetting &value) -> void
        {
            if (k_version >= 88_ui && k_version < 112_ui)
            {
                convert_common_property_as_regular<AudioCommonPropertyType::voice_volume_make_up_gain>(map, value.make_up_value);
            }
            else
            {
                convert_common_property_as_randomizable<AudioCommonPropertyType::voice_volume_make_up_gain>(map, value.make_up);
            }
            return;
        }

        //------------------------------------

        inline static auto load_common_property(
            CommonPropertyMap<AudioCommonPropertyType> const &map,
            AudioPositioningSetting &value) -> void
        {
            convert_common_property_as_regular<AudioCommonPropertyType::positioning_center_percent>(map, value.center_percent);
            convert_common_property<AudioCommonPropertyType::positioning_speaker_panning_x>(map, value.speaker_panning.position.x);
            convert_common_property<AudioCommonPropertyType::positioning_speaker_panning_y>(map, value.speaker_panning.position.y);
            if (k_version >= 140_ui)
            {
                convert_common_property<AudioCommonPropertyType::positioning_speaker_panning_z>(map, value.speaker_panning.position.z);
            }
            if (k_version >= 132_ui)
            {
                convert_common_property_as_regular<AudioCommonPropertyType::positioning_listener_routing_speaker_panning_division_spatialization_mix>(map, value.listener_routing.speaker_panning_divsion_spatialization_mix);
                convert_common_property_as_regular<AudioCommonPropertyType::positioning_listener_routing_attenuation_id>(map, value.listener_routing.attenuation.id);
            }
            return;
        }

        //------------------------------------

        inline static auto load_common_property(
            CommonPropertyMap<AudioCommonPropertyType> const &map,
            AudioHDRSetting &value) -> void
        {
            if (k_version >= 88_ui)
            {
                convert_common_property<AudioCommonPropertyType::hdr_envelope_tracking_active_range>(map, value.envelope_tracking.active_range);
            }
            return;
        }

        //------------------------------------

        inline static auto load_common_property(
            CommonPropertyMap<AudioCommonPropertyType> const &map,
            BusVoiceVolumeGainSetting &value) -> void
        {
            if (k_version >= 125_ui)
            {
                convert_common_property<AudioCommonPropertyType::voice_volume_make_up_gain>(map, value.make_up);
            }
            return;
        }

        //------------------------------------

        inline static auto load_common_property(
            CommonPropertyMap<AudioCommonPropertyType> const &map,
            MusicMIDISetting &value) -> void
        {
            if (k_version >= 112_ui)
            {
                convert_common_property<AudioCommonPropertyType::midi_target_id>(map, value.target.id);
                convert_common_property<AudioCommonPropertyType::midi_clip_tempo_source>(map, value.clip_tempo.source);
            }
            return;
        }

        //------------------------------------

        inline static auto load_common_property(
            CommonPropertyMap<AudioCommonPropertyType> const &map,
            AudioPlaybackPrioritySetting &value) -> void
        {
            convert_common_property_as_regular<AudioCommonPropertyType::playback_priority_value>(map, value.value);
            convert_common_property_as_regular<AudioCommonPropertyType::playback_priority_offset_at_maximum_distance>(map, value.offset_at_maximum_distance);
            return;
        }

        //------------------------------------

        inline static auto load_common_property(
            CommonPropertyMap<AudioCommonPropertyType> const &map,
            AudioMotionSetting &value) -> void
        {
            if (k_version < 128_ui)
            {
                convert_common_property_as_randomizable<AudioCommonPropertyType::motion_low_pass_filter>(map, value.low_pass_filter);
                convert_common_property_as_randomizable<AudioCommonPropertyType::motion_volume_offset>(map, value.volume_offset);
            }
            return;
        }

        //------------------------------------

        inline static auto load_common_property(
            CommonPropertyMap<AudioCommonPropertyType> const &map,
            AudioMixerSetting &value) -> void
        {
            if (k_version >= 112 && k_version < 150_ui)
            {
                convert_common_property<AudioCommonPropertyType::mixer_id>(map, value.id);
            }
            return;
        }

        //------------------------------------

        inline static auto load_common_property(
            CommonPropertyMap<AudioCommonPropertyType> const &map,
            AudioPlaybackLimitSetting &value) -> void
        {
            return;
        }

        //------------------------------------

        inline static auto load_common_property(
            CommonPropertyMap<AudioCommonPropertyType> const &map,
            AudioVirtualVoiceSetting &value) -> void
        {
            return;
        }

        //------------------------------------

        inline static auto load_common_property(
            CommonPropertyMap<AudioCommonPropertyType> const &map,
            AudioAuxiliarySendSetting &value) -> void
        {
            convert_common_property_as_regular<AudioCommonPropertyType::game_defined_auxiliary_send_volume>(map, value.game_defined.volume);
            if (k_version >= 128_ui)
            {
                convert_common_property_as_regular<AudioCommonPropertyType::game_defined_auxiliary_send_low_pass_filter>(map, value.game_defined.low_pass_filter);
                convert_common_property_as_regular<AudioCommonPropertyType::game_defined_auxiliary_send_high_pass_filter>(map, value.game_defined.high_pass_filter);
            }
            convert_common_property_as_regular<AudioCommonPropertyType::user_defined_auxiliary_send_volume_0>(map, value.user_defined.item_1.volume);
            if (k_version >= 128_ui)
            {
                convert_common_property_as_regular<AudioCommonPropertyType::user_defined_auxiliary_send_low_pass_filter_0>(map, value.user_defined.item_1.low_pass_filter);
                convert_common_property_as_regular<AudioCommonPropertyType::user_defined_auxiliary_send_high_pass_filter_0>(map, value.user_defined.item_1.high_pass_filter);
            }
            convert_common_property_as_regular<AudioCommonPropertyType::user_defined_auxiliary_send_volume_1>(map, value.user_defined.item_2.volume);
            if (k_version >= 128_ui)
            {
                convert_common_property_as_regular<AudioCommonPropertyType::user_defined_auxiliary_send_low_pass_filter_1>(map, value.user_defined.item_2.low_pass_filter);
                convert_common_property_as_regular<AudioCommonPropertyType::user_defined_auxiliary_send_high_pass_filter_1>(map, value.user_defined.item_2.high_pass_filter);
            }
            convert_common_property_as_regular<AudioCommonPropertyType::user_defined_auxiliary_send_volume_2>(map, value.user_defined.item_3.volume);
            if (k_version >= 128_ui)
            {
                convert_common_property_as_regular<AudioCommonPropertyType::user_defined_auxiliary_send_low_pass_filter_2>(map, value.user_defined.item_3.low_pass_filter);
                convert_common_property_as_regular<AudioCommonPropertyType::user_defined_auxiliary_send_high_pass_filter_2>(map, value.user_defined.item_3.high_pass_filter);
            }
            convert_common_property_as_regular<AudioCommonPropertyType::user_defined_auxiliary_send_volume_3>(map, value.user_defined.item_4.volume);
            if (k_version >= 128_ui)
            {
                convert_common_property_as_regular<AudioCommonPropertyType::user_defined_auxiliary_send_low_pass_filter_3>(map, value.user_defined.item_4.low_pass_filter);
                convert_common_property_as_regular<AudioCommonPropertyType::user_defined_auxiliary_send_high_pass_filter_3>(map, value.user_defined.item_4.high_pass_filter);
            }
            if (k_version >= 135_ui)
            {
                convert_common_property_as_regular<AudioCommonPropertyType::early_reflection_auxiliary_send_volume>(map, value.early_reflection.volume);
            }
            return;
        }

        //------------------------------------

        inline static auto load_common_property(
            CommonPropertyMap<AudioCommonPropertyType> const &map,
            BusHDRSetting &value) -> void
        {
            if (k_version >= 88_ui)
            {
                convert_common_property<AudioCommonPropertyType::hdr_threshold>(map, value.dynamic.threshold);
                convert_common_property<AudioCommonPropertyType::hdr_ratio>(map, value.dynamic.ratio);
                convert_common_property<AudioCommonPropertyType::hdr_release_time>(map, value.dynamic.release_time);
                convert_common_property<AudioCommonPropertyType::hdr_window_tap_output_game_parameter_id>(map, value.window_top_output_game_parameter.id);
                convert_common_property<AudioCommonPropertyType::hdr_window_tap_output_game_parameter_minimum>(map, value.window_top_output_game_parameter.minimum);
                convert_common_property<AudioCommonPropertyType::hdr_window_tap_output_game_parameter_maximum>(map, value.window_top_output_game_parameter.maximum);
            }
            return;
        }

        //------------------------------------

        inline static auto load_common_property(
            CommonPropertyMap<AudioCommonPropertyType> const &map,
            SoundMIDISetting &value) -> void
        {
            if (k_version >= 112_ui)
            {
                convert_common_property<AudioCommonPropertyType::midi_event_play_on>(map, value.event.play_on);
                convert_common_property<AudioCommonPropertyType::midi_note_tracking_root_note>(map, value.note_tracking.root_note);
                convert_common_property_as_regular<AudioCommonPropertyType::midi_transformation_transposition>(map, value.transformation.transposition);
                convert_common_property_as_regular<AudioCommonPropertyType::midi_transformation_velocity_offset>(map, value.transformation.velocity_offset);
                convert_common_property<AudioCommonPropertyType::midi_filter_key_range_minimum>(map, value.filter.key_range_minimum);
                convert_common_property<AudioCommonPropertyType::midi_filter_key_range_maximum>(map, value.filter.key_range_maximum);
                convert_common_property<AudioCommonPropertyType::midi_filter_velocity_minimum>(map, value.filter.velocity_minimum);
                convert_common_property<AudioCommonPropertyType::midi_filter_velocity_maximum>(map, value.filter.velocity_maximum);
                convert_common_property<AudioCommonPropertyType::midi_filter_channel>(map, value.filter.channel);
            }
            return;
        }

        //------------------------------------

        inline static auto load_common_property(
            CommonPropertyMap<AudioCommonPropertyType> const &map,
            BusVoiceSetting &value) -> void
        {
            convert_common_property_as_regular<AudioCommonPropertyType::voice_volume>(map, value.volume);
            convert_common_property_as_regular<AudioCommonPropertyType::voice_pitch>(map, value.pitch);
            convert_common_property_as_regular<AudioCommonPropertyType::voice_low_pass_filter>(map, value.low_pass_filter);
            if (k_version >= 112_ui)
            {
                convert_common_property_as_regular<AudioCommonPropertyType::voice_high_pass_filter>(map, value.high_pass_filter);
            }
            return;
        }

        //------------------------------------

        inline static auto load_common_property(
            CommonPropertyMap<AudioCommonPropertyType> const &map,
            BusBusSetting &value) -> void
        {
            convert_common_property_as_regular<AudioCommonPropertyType::bus_volume>(map, value.volume);
            return;
        }

        //------------------------------------

        inline static auto load_common_property(
            CommonPropertyMap<AudioCommonPropertyType> const &map,
            BusOutputBusSetting &value) -> void
        {
            if (k_version >= 128_ui)
            {
                convert_common_property_as_regular<AudioCommonPropertyType::output_bus_volume>(map, value.volume);
                convert_common_property_as_regular<AudioCommonPropertyType::output_bus_low_pass_filter>(map, value.low_pass_filter);
                convert_common_property_as_regular<AudioCommonPropertyType::output_bus_high_pass_filter>(map, value.high_pass_filter);
            }
            return;
        }

        //------------------------------------

        template <typename Type, typename Parser>
            requires std::is_enum<Type>::value && (!std::is_void<Parser>::value && (!std::is_reference<Parser>::value) && std::is_same_v<Parser, std::remove_cvref_t<Parser>>)
        inline static auto exchange_section_sub(
            DataStreamView &stream,
            bool const &randomizable,
            Parser const &parser) -> void
        {
            auto map = CommonPropertyMap<Type>{};
            exchange_list(
                stream,
                map.regular,
                [](auto &data, auto &element)
                {
                    element.first = exchange_enumeration<uint8_t, Type>(data.readUint8());
                },
                stream.readUint8());
            exchange_list(
                stream,
                map.regular,
                [](auto &data, auto &element)
                {
                    element.second = declare_common_property_type<Type>(element.first);
                    exchange_common_property_value(data, element.second);
                });
            if (randomizable)
            {
                exchange_list(
                    stream,
                    map.randomizer,
                    [](auto &data, auto &element)
                    {
                        element.first = exchange_enumeration<uint8_t, Type>(data.readUint8());
                    },
                    stream.readUint8());
                exchange_list(
                    stream,
                    map.randomizer,
                    [](auto &data, auto &element)
                    {
                        element.second[0] = declare_common_property_type<Type>(element.first);
                        element.second[1] = declare_common_property_type<Type>(element.first);
                        exchange_common_property_value(data, element.second[0]);
                        exchange_common_property_value(data, element.second[1]);
                    });
            }
            parser(map);
            return;
        }

        //------------------------------------

        template <typename RawValue>
            requires std::is_integral<RawValue>::value
        inline static auto exchange_section_sub(
            DataStreamView &stream,
            std::vector<uint32_t> &value_list) -> void
        {
            exchange_list(
                stream, value_list, [](auto &data, auto &value)
                { value = data.readUint32(); }, stream.read_of<RawValue>());
            return;
        }

        //------------------------------------

        inline static auto exchange_section_sub(
            DataStreamView &stream,
            MusicMIDISetting &midi_value,
            bool &midi_target_override,
            bool &midi_clip_tempo_override) -> void
        {
            exchange_bit_multi<uint8_t>(stream, k_bitset_boolean_fill, midi_target_override, midi_clip_tempo_override);
            return;
        }

        //------------------------------------

        inline static auto exchange_section_sub(
            DataStreamView &stream,
            AudioSourceSetting &value) -> void
        {
            value.plug_in = stream.readUint32();
            if (k_version < 112_ui)
            {
                exchange_bit_multi<uint32_t>(stream, value.type);
            }
            else
            {
                exchange_bit_multi<uint8_t>(stream, value.type);
            }
            value.resource = stream.readUint32();
            if (k_version < 113_ui)
            {
                value.source = stream.readUint32();
                if (value.type != AudioSourceType::streamed)
                {
                    value.resource_offset = stream.readUint32();
                }
            }
            if (k_version < 112_ui)
            {
                if (value.type != AudioSourceType::streamed)
                {
                    value.resource_size = stream.readUint32();
                }
            }
            else
            {
                value.resource_size = stream.readUint32();
            }
            if (k_version < 112_ui)
            {
                exchange_bit_multi<uint8_t>(stream, value.is_voice);
            }
            else
            {
                exchange_bit_multi<uint8_t>(stream, value.is_voice, k_bitset_boolean_fill, k_bitset_boolean_fill, value.non_cachable_stream);
            }
            if ((value.plug_in & 0x0000FFFF) >= 0x0002)
            {
                exchange_raw_constant(stream, 0_ui);
            }
            return;
        }

        //------------------------------------

        inline static auto exchange_section_sub(
            DataStreamView &stream,
            std::vector<AudioSourceSetting> &value) -> void
        {
            exchange_list(stream, value, [](auto &data, auto &value)
                          { exchange_section_sub(data, value); }, stream.readUint32());
            return;
        }

        //------------------------------------

        inline static auto exchange_section_sub(
            DataStreamView &stream,
            MusicTrackClip &clip_value) -> void
        {
            exchange_list(stream, clip_value.item, [](auto &data, auto &value)
                          {
                value.u1 = data.readUint32();
                value.source = data.readUint32();
                if (k_version >= 140) {
                    value.event = data.readUint32();
                } 
                value.offset = data.readDouble();
                value.begin = data.readDouble();
                value.end = data.readDouble();
                value.duration = data.readDouble(); }, stream.readUint32());
            if (!clip_value.item.empty())
            {
                clip_value.u1 = stream.readUint32();
            }
            exchange_list(stream, clip_value.curve, [](auto &data, auto &value)
                          {
                value.index = data.readUint32();
                exchange_bit_multi<uint32_t>(data, value.type);
                exchange_list(data, value.point, [] (auto &data, auto &value) {
                    value.position.x = data.readFloat();
                    value.position.y = data.readFloat();
                    exchange_bit_multi<uint32_t>(data, value.curve);
                }, data.readUint32()); }, stream.readUint32());
            return;
        }

        //------------------------------------

        inline static auto exchange_section_sub(
            DataStreamView &stream,
            AudioEffectSetting &effect_value) -> void
        {
            auto item_size = stream.readUint8();
            if (static_cast<size_t>(item_size) > k_none_size)
            {
                if (k_version < 150_ui)
                {
                    exchange_bit_multi<uint8_t>(stream,
                                                effect_value.bypass[0],
                                                effect_value.bypass[1],
                                                effect_value.bypass[2],
                                                effect_value.bypass[3],
                                                effect_value.bypass[4]);
                }
                else
                {
                    exchange_bit_multi<uint8_t>(stream, effect_value.bypass[0]);
                }
            }
            exchange_list(stream, effect_value.item, [](auto &data, auto &value)
                          {
                value.index = data.readUint8();
                value.id = data.readUint32();
                if (k_version < 150_ui) {
                    exchange_bit_multi<uint8_t>(data, value.use_share_set);
                    exchange_bit_multi<uint8_t>(data, value.u1);
                }
                else {
                    exchange_bit_multi<uint8_t>(data, value.bypass, value.use_share_set);
                } }, item_size);
            return;
        }

        //------------------------------------

        inline static auto exchange_section_sub(
            DataStreamView &stream,
            AudioEffectSetting &effect_value,
            bool &effect_override) -> void
        {
            exchange_bit_multi<uint8_t>(stream, effect_override);
            exchange_section_sub(stream, effect_value);
            return;
        }

        //------------------------------------

        inline static auto exchange_section_sub(
            DataStreamView &stream,
            AudioMetadataSetting &metadata_value) -> void
        {
            if (k_version >= 140_ui)
            {
                exchange_list(stream, metadata_value.item, [](auto &data, auto &value)
                              {
                    value.index = data.readUint8();
                    value.id = data.readUint32();
                    exchange_bit_multi<uint8_t>(data, value.use_share_set); }, stream.readUint8());
            }
            return;
        }

        //------------------------------------

        inline static auto exchange_section_sub(
            DataStreamView &stream,
            AudioMetadataSetting &metadata_value,
            bool &metadata_override) -> void
        {
            if (k_version >= 140_ui)
            {
                exchange_bit_multi<uint8_t>(stream, metadata_override);
            }
            exchange_section_sub(stream, metadata_value);
            return;
        }

        //------------------------------------

        inline static auto exchange_section_sub(
            DataStreamView &stream,
            AudioMixerSetting &mixer_value,
            bool &mixer_override) -> void
        {
            if (k_version >= 112_ui && k_version < 150_ui)
            {
                exchange_bit_multi<uint8_t>(stream, mixer_override);
            }
            return;
        }

        //------------------------------------

        inline static auto exchange_section_sub(
            DataStreamView &stream,
            AudioOutputBusSetting &output_bus_value) -> void
        {
            output_bus_value.bus = stream.readUint32();
            return;
        }

        //------------------------------------

        inline static auto exchange_section_sub(
            DataStreamView &stream,
            SoundMIDISetting &midi_value,
            AudioPlaybackPrioritySetting &playback_priority_value,
            bool &midi_event_override,
            bool &midi_note_tracking_override,
            bool &playback_priority_override) -> void
        {
            if (k_version >= 112_ui)
            {
                exchange_bit_multi<uint8_t>(stream, playback_priority_override, playback_priority_value.use_distance_factor, midi_event_override, midi_event_override, midi_value.note_tracking.enable, midi_value.event.break_on_note_off);
            }
            return;
        }

        //------------------------------------

        inline static auto exchange_section_sub(
            DataStreamView &stream,
            AudioPlaybackPrioritySetting &playback_priority_value,
            bool &playback_priority_override) -> void
        {
            if (k_version < 112_ui)
            {
                exchange_bit_multi<uint8_t>(stream, playback_priority_override);
                exchange_bit_multi<uint8_t>(stream, playback_priority_value.use_distance_factor);
            }
            else
            {
                exchange_bit_multi<uint8_t>(stream, playback_priority_override, playback_priority_value.use_distance_factor);
            }
            return;
        }

        //------------------------------------

        inline static auto exchange_section_sub(
            DataStreamView &stream,
            AudioPositioningSetting &positioning_value,
            bool &positioning_override) -> void
        {
            if (k_version < 112_ui)
            {
                exchange_bit_multi<uint8_t>(stream, positioning_override);
                if (positioning_override)
                {
                    auto b1 = false;
                    auto b2 = false;
                    auto b3 = false;
                    if (k_version >= 88_ui && k_version < 112_ui)
                    {
                        exchange_bit_multi<uint8_t>(stream, b3);
                    }
                    if (k_version < 112_ui)
                    {
                        exchange_bit_multi<uint8_t>(stream, positioning_value.type);
                    }
                    if (k_version < 88_ui)
                    {
                        exchange_bit_multi<uint8_t>(stream, b1, b2);
                    }
                    if (k_version >= 88_ui && k_version < 112_ui)
                    {
                        exchange_bit_multi<uint8_t>(stream, b1);
                    }
                    if (positioning_value.type == AudioPositioningSettingType::two_dimension)
                    {
                        if (k_version < 88_ui)
                        {
                            assert_conditional(!b2, fmt::format("{}", Language::get("wwise.sound_bank.invalid_bitset")), "exchange_section_sub");
                        }
                        if (k_version >= 88_ui && k_version < 112_ui)
                        {
                            assert_conditional(b3, fmt::format("{}", Language::get("wwise.sound_bank.invalid_bitset")), "exchange_section_sub");
                        }
                        positioning_value.speaker_panning.enable = b1;
                    }
                    if (positioning_value.type == AudioPositioningSettingType::three_dimension)
                    {
                        if (k_version < 88_ui)
                        {
                            assert_conditional(b2, fmt::format("{}", Language::get("wwise.sound_bank.invalid_bitset")), "exchange_section_sub");
                        }
                        if (k_version >= 88_ui && k_version < 112_ui)
                        {
                            assert_conditional(!b3, fmt::format("{}", Language::get("wwise.sound_bank.invalid_bitset")), "exchange_section_sub");
                        }
                        positioning_value.listener_routing.position_source.mode = !b1 ? AudioPositioningSettingListenerRoutingPositionSourceMode::user_defined : AudioPositioningSettingListenerRoutingPositionSourceMode::game_defined;
                        exchange_raw_constant(stream, 0_byte);
                        exchange_raw_constant(stream, 0_byte);
                        exchange_raw_constant(stream, 0_byte);
                        positioning_value.listener_routing.attenuation.id = stream.readUint32();
                        if (positioning_value.listener_routing.position_source.mode == AudioPositioningSettingListenerRoutingPositionSourceMode::game_defined)
                        {
                            exchange_bit_multi<uint8_t>(stream, positioning_value.listener_routing.position_source.update_at_each_frame);
                        }
                        if (positioning_value.listener_routing.position_source.mode == AudioPositioningSettingListenerRoutingPositionSourceMode::user_defined)
                        {
                            exchange_bit_multi<uint8_t>(stream,
                                                        positioning_value.listener_routing.position_source.automation.play_type,
                                                        positioning_value.listener_routing.position_source.automation.play_mode,
                                                        positioning_value.listener_routing.position_source.automation.pick_new_path_when_sound_start);
                            exchange_raw_constant(stream, 0_byte);
                            exchange_raw_constant(stream, 0_byte);
                            exchange_raw_constant(stream, 0_byte);
                            exchange_bit_multi<uint8_t>(stream, positioning_value.listener_routing.position_source.automation.loop);
                            positioning_value.listener_routing.position_source.automation.transition_time = stream.readUint32();
                            exchange_bit_multi<uint8_t>(stream, positioning_value.listener_routing.position_source.hold_listener_orientation);
                            exchange_list(
                                stream,
                                positioning_value.listener_routing.position_source.automation.point,
                                [](auto &data, auto &value)
                                {
                                    value.position.x = data.readFloat();
                                    exchange_raw_constant(data, 0_ui);
                                    value.position.y = data.readFloat();
                                    value.duration = data.readUint32();
                                },
                                stream.readUint32());
                            auto size = stream.readUint32();
                            exchange_list(
                                stream,
                                positioning_value.listener_routing.position_source.automation.path,
                                [](auto &data, auto &value)
                                {
                                    value.point.begin = data.readUint32();
                                    value.point.count = data.readUint32();
                                },
                                size);
                            exchange_list(
                                stream,
                                positioning_value.listener_routing.position_source.automation.path,
                                [](auto &data, auto &value)
                                {
                                    value.random_range.left_right = data.readFloat();
                                    value.random_range.front_back = data.readFloat();
                                });
                        }
                    }
                }
            }
            else if (k_version < 132_ui)
            {
                auto b2 = false;
                if (k_version < 125_ui)
                {
                    exchange_bit_multi<uint8_t>(stream, positioning_override, b2, positioning_value.speaker_panning.enable, positioning_value.type,
                                                positioning_value.listener_routing.spatialization_bool,
                                                positioning_value.listener_routing.position_source.automation.loop,
                                                positioning_value.listener_routing.position_source.update_at_each_frame,
                                                positioning_value.listener_routing.position_source.hold_listener_orientation);
                }
                if (k_version >= 125_ui)
                {
                    exchange_bit_multi<uint8_t>(stream, positioning_override, positioning_value.enable, b2, positioning_value.speaker_panning.enable, positioning_value.type);
                }
                if (positioning_value.type == AudioPositioningSettingType::three_dimension)
                {
                    if (k_version < 125_ui)
                    {
                        exchange_bit_multi<uint8_t>(stream, positioning_value.listener_routing.position_source.mode);
                    }
                    else
                    {
                        if (k_version < 128_ui)
                        {
                            exchange_bit_multi<uint8_t>(stream, positioning_value.listener_routing.spatialization_bool,
                                                        positioning_value.listener_routing.position_source.automation.loop,
                                                        positioning_value.listener_routing.position_source.update_at_each_frame,
                                                        positioning_value.listener_routing.position_source.hold_listener_orientation,
                                                        positioning_value.listener_routing.position_source.mode);
                        }
                        else
                        {
                            exchange_bit_multi<uint8_t>(stream, positioning_value.listener_routing.spatialization,
                                                        positioning_value.listener_routing.position_source.automation.loop,
                                                        positioning_value.listener_routing.position_source.update_at_each_frame,
                                                        positioning_value.listener_routing.position_source.hold_listener_orientation,
                                                        positioning_value.listener_routing.position_source.mode);
                        }
                    }
                    positioning_value.listener_routing.attenuation.id = stream.readUint32();
                    if (positioning_value.listener_routing.position_source.mode == AudioPositioningSettingListenerRoutingPositionSourceMode::user_defined)
                    {
                        exchange_bit_multi<uint8_t>(stream, positioning_value.listener_routing.position_source.automation.play_type,
                                                    positioning_value.listener_routing.position_source.automation.play_mode,
                                                    positioning_value.listener_routing.position_source.automation.pick_new_path_when_sound_start);
                        positioning_value.listener_routing.position_source.automation.transition_time = stream.readUint32();
                        exchange_list(stream, positioning_value.listener_routing.position_source.automation.point, [](auto &data, auto &value)
                                      {
                        value.position.x = data.readFloat();
                        value.position.z = data.readFloat();
                        value.position.y = data.readFloat();
                        value.duration = data.readUint32(); }, stream.readUint32());
                        auto size = stream.readUint32();
                        exchange_list(stream, positioning_value.listener_routing.position_source.automation.path, [](auto &data, auto &value)
                                      {
                        value.point.begin = data.readUint32();
                        value.point.count = data.readUint32(); }, size);
                        exchange_list(stream, positioning_value.listener_routing.position_source.automation.path, [](auto &data, auto &value)
                                      {
                        value.random_range.left_right = data.readFloat();
                        value.random_range.front_back = data.readFloat();
                        value.random_range.up_down = data.readFloat(); });
                    }
                }
            }
            else
            {
                exchange_bit_multi<uint8_t>(
                    stream,
                    positioning_override,
                    positioning_value.listener_routing.enable,
                    positioning_value.speaker_panning.mode,
                    k_bitset_boolean_fill,
                    positioning_value.listener_routing.position_source.mode,
                    k_bitset_boolean_fill);
                if (positioning_value.listener_routing.enable)
                {
                    if (k_version < 134_ui)
                    {
                        exchange_bit_multi<uint8_t>(
                            stream,
                            positioning_value.listener_routing.spatialization,
                            positioning_value.listener_routing.position_source.hold_emitter_position_and_orientation,
                            positioning_value.listener_routing.position_source.hold_listener_orientation,
                            positioning_value.listener_routing.position_source.automation.loop);
                    }
                    else if (k_version < 140_ui)
                    {
                        exchange_bit_multi<uint8_t>(
                            stream,
                            positioning_value.listener_routing.spatialization,
                            positioning_value.listener_routing.attenuation.enable,
                            positioning_value.listener_routing.position_source.hold_emitter_position_and_orientation,
                            positioning_value.listener_routing.position_source.hold_listener_orientation,
                            positioning_value.listener_routing.position_source.automation.loop);
                    }
                    else
                    {
                        exchange_bit_multi<uint8_t>(
                            stream,
                            positioning_value.listener_routing.spatialization,
                            positioning_value.listener_routing.attenuation.enable,
                            positioning_value.listener_routing.position_source.hold_emitter_position_and_orientation,
                            positioning_value.listener_routing.position_source.hold_listener_orientation,
                            positioning_value.listener_routing.position_source.automation.loop,
                            positioning_value.listener_routing.position_source.diffraction_and_transmission);
                    }
                    if (positioning_value.listener_routing.position_source.mode != AudioPositioningSettingListenerRoutingPositionSourceMode::emitter)
                    {
                        exchange_bit_multi<uint8_t>(
                            stream,
                            positioning_value.listener_routing.position_source.automation.play_type,
                            positioning_value.listener_routing.position_source.automation.play_mode,
                            positioning_value.listener_routing.position_source.automation.pick_new_path_when_sound_start);
                        positioning_value.listener_routing.position_source.automation.transition_time = stream.readUint32();
                        exchange_list(stream, positioning_value.listener_routing.position_source.automation.point, [](auto &data, auto &value)
                                      {
                        value.position.x = data.readFloat();
                        value.position.z = data.readFloat();
                        value.position.y = data.readFloat();
                        value.duration = data.readUint32(); }, stream.readUint32());
                        auto size = stream.readUint32();
                        exchange_list(stream, positioning_value.listener_routing.position_source.automation.path, [](auto &data, auto &value)
                                      {
                        value.point.begin = data.readUint32();
                        value.point.count = data.readUint32(); }, size);
                        exchange_list(stream, positioning_value.listener_routing.position_source.automation.path, [](auto &data, auto &value)
                                      {
                        value.random_range.left_right = data.readFloat();
                        value.random_range.front_back = data.readFloat();
                        value.random_range.up_down = data.readFloat(); });
                    }
                }
            }
            return;
        }

        //------------------------------------

        inline static auto exchange_section_sub(
            DataStreamView &stream,
            AudioAuxiliarySendSetting &auxiliary_send_value,
            bool &game_defined_auxiliary_send_override,
            bool &user_defined_auxiliary_send_override) -> void
        {
            if (k_version < 112_ui)
            {
                exchange_bit_multi<uint8_t>(stream, game_defined_auxiliary_send_override);
                exchange_bit_multi<uint8_t>(stream, auxiliary_send_value.game_defined.enable);
                exchange_bit_multi<uint8_t>(stream, user_defined_auxiliary_send_override);
                exchange_bit_multi<uint8_t>(stream, auxiliary_send_value.user_defined.enable);
            }
            else if (k_version < 135_ui)
            {
                exchange_bit_multi<uint8_t>(stream, game_defined_auxiliary_send_override, auxiliary_send_value.game_defined.enable, user_defined_auxiliary_send_override, auxiliary_send_value.user_defined.enable);
            }
            if (k_version < 135_ui)
            {
                if (auxiliary_send_value.user_defined.enable)
                {
                    auxiliary_send_value.user_defined.item_1.bus = stream.readUint32();
                    auxiliary_send_value.user_defined.item_2.bus = stream.readUint32();
                    auxiliary_send_value.user_defined.item_3.bus = stream.readUint32();
                    auxiliary_send_value.user_defined.item_4.bus = stream.readUint32();
                }
            }
            return;
        }

        //------------------------------------

        inline static auto exchange_section_sub(
            DataStreamView &stream,
            AudioAuxiliarySendSetting &auxiliary_send_value,
            bool &game_defined_auxiliary_send_override,
            bool &user_defined_auxiliary_send_override,
            bool &early_reflection_auxiliary_send_override) -> void
        {
            if (k_version >= 135_ui)
            {
                exchange_bit_multi<uint8_t>(
                    stream,
                    game_defined_auxiliary_send_override,
                    auxiliary_send_value.game_defined.enable,
                    user_defined_auxiliary_send_override,
                    auxiliary_send_value.user_defined.enable,
                    early_reflection_auxiliary_send_override);
                if (auxiliary_send_value.user_defined.enable)
                {
                    auxiliary_send_value.user_defined.item_1.bus = stream.readUint32();
                    auxiliary_send_value.user_defined.item_2.bus = stream.readUint32();
                    auxiliary_send_value.user_defined.item_3.bus = stream.readUint32();
                    auxiliary_send_value.user_defined.item_4.bus = stream.readUint32();
                }
                auxiliary_send_value.early_reflection.bus = stream.readUint32();
            }
            return;
        }

        //------------------------------------

        inline static auto exchange_section_sub(
            DataStreamView &stream,
            AudioPlaybackLimitSetting &playback_limit_value,
            bool &playback_limit_override) -> void
        {
            if (k_version < 112_ui)
            {
                exchange_bit_multi<uint8_t>(stream, playback_limit_value.when_priority_is_equal);
                exchange_bit_multi<uint8_t>(stream, playback_limit_value.when_limit_is_reached);
                playback_limit_value.value = stream.readUint16();
                exchange_bit_multi<uint8_t>(stream, playback_limit_override);
            }
            return;
        }

        //------------------------------------

        inline static auto exchange_section_sub(
            DataStreamView &stream,
            AudioPlaybackLimitSetting &playback_limit_value,
            AudioBusMuteForBackgroundMusic &mute_for_background_music_value,
            bool &playback_limit_override) -> void
        {
            if (k_version >= 112_ui)
            {
                exchange_bit_multi<uint8_t>(
                    stream,
                    playback_limit_value.when_priority_is_equal,
                    playback_limit_value.when_limit_is_reached,
                    playback_limit_override,
                    mute_for_background_music_value.enable);
                playback_limit_value.value = stream.readUint16();
            }
            return;
        }

        //------------------------------------

        inline static auto exchange_section_sub(
            DataStreamView &stream,
            AudioPlaybackLimitSetting &playback_limit_value,
            AudioVirtualVoiceSetting &virtual_voice_value,
            bool &playback_limit_override,
            bool &virtual_voice_override) -> void
        {
            if (k_version < 112_ui)
            {
                exchange_bit_multi<uint8_t>(stream, virtual_voice_value.on_return_to_physical);
                exchange_bit_multi<uint8_t>(stream, playback_limit_value.when_priority_is_equal);
                exchange_bit_multi<uint8_t>(stream, playback_limit_value.when_limit_is_reached);
                playback_limit_value.value = stream.readUint16();
                exchange_bit_multi<uint8_t>(stream, playback_limit_value.scope);
                exchange_bit_multi<uint8_t>(stream, virtual_voice_value.behavior);
                exchange_bit_multi<uint8_t>(stream, playback_limit_override);
                exchange_bit_multi<uint8_t>(stream, virtual_voice_override);
            }
            else
            {
                exchange_bit_multi<uint8_t>(
                    stream,
                    playback_limit_value.when_priority_is_equal,
                    playback_limit_value.when_limit_is_reached,
                    playback_limit_value.scope,
                    playback_limit_override,
                    virtual_voice_override);
                exchange_bit_multi<uint8_t>(stream, virtual_voice_value.on_return_to_physical);
                playback_limit_value.value = stream.readUint16();
                exchange_bit_multi<uint8_t>(stream, virtual_voice_value.behavior);
            }
            return;
        }

        //------------------------------------

        inline static auto exchange_section_sub(
            DataStreamView &stream,
            AudioVoiceVolumeGainSetting &voice_volume_gain_value,
            AudioHDRSetting &hdr_value,
            bool &voice_volume_loudness_normalization_override,
            bool &hdr_envelope_tracking_override) -> void
        {
            if (k_version >= 88_ui && k_version < 112_ui)
            {
                exchange_bit_multi<uint8_t>(stream, hdr_envelope_tracking_override);
                exchange_bit_multi<uint8_t>(stream, voice_volume_loudness_normalization_override);
                exchange_bit_multi<uint8_t>(stream, voice_volume_gain_value.normalization);
                exchange_bit_multi<uint8_t>(stream, hdr_value.envelope_tracking.enable);
            }
            else if (k_version >= 112_ui)
            {
                exchange_bit_multi<uint8_t>(
                    stream,
                    hdr_envelope_tracking_override,
                    voice_volume_loudness_normalization_override,
                    voice_volume_gain_value.normalization,
                    hdr_value.envelope_tracking.enable);
            }
            return;
        }

        //------------------------------------

        inline static auto exchange_section_sub(
            DataStreamView &stream,
            StateSetting &state_value) -> void
        {
            if (k_version < 125_ui)
            {
                exchange_list(stream, state_value.item, [](auto &data, auto &value)
                              {
                    value.group = data.readUint32();
                    exchange_bit_multi<uint8_t>(data, value.change_occur_at);
                    exchange_list(data, value.apply, [] (auto &data, auto &value) {
                        value.target = data.readUint32();
                        value.setting = data.readUint32();
                    }, data.readUint16()); }, stream.readUint32());
            }
            else
            {
                exchange_list(
                    stream, state_value.attribute, [](auto &data, auto &value)
                    {
                        value.type = data.readUint8();
                        exchange_bit_multi<uint8_t>(data, value.category);
                        if (k_version >= 128_ui) {
                            value.u1 = data.readUint8();
                        }}, stream.readUint8());
                exchange_list(
                    stream,
                    state_value.item, [](auto &data, auto &value)
                    {
                        value.group = data.readUint32();
                        exchange_bit_multi<uint8_t>(data, value.change_occur_at);
                        exchange_list(data, value.apply, [] (auto &data, auto &value) {
                            value.target = data.readUint32();
                            value.setting = data.readUint32();
                        }, data.readUint8()); }, stream.readUint8());
            }
            return;
        }

        //------------------------------------

        inline static auto exchange_section_sub(
            DataStreamView &stream,
            RealTimeParameterControlSetting &real_time_parameter_control_value) -> void
        {
            exchange_list(
                stream, real_time_parameter_control_value.item, [](auto &data, auto &value)
                {
                    value.parameter.id = data.readUint32();
                    if (k_version >= 112_ui) {
                        exchange_bit_multi<uint8_t>(data, value.parameter.category);
                        exchange_bit_multi<uint8_t>(data, value.u1);
                    }
                    if (k_version < 112_ui) {
                        value.type = data.readUint32();
                    }
                    else {
                        value.type = data.readUint8();
                    }
                    value.u2 = data.readUint32();
                    exchange_bit_multi<uint8_t>(data, value.mode);
                    exchange_list(data, value.point, [] (auto &data, auto &value) {
                        value.position.x = data.readFloat();
                        value.position.y = data.readFloat();
                        exchange_bit_multi<uint32_t>(data, value.curve);
                    }, data.readUint16()); }, stream.readUint16());
            return;
        }

        //------------------------------------

        inline static auto exchange_section_sub(
            DataStreamView &stream,
            MusicTransitionSettingItemFade &fade_value) -> void
        {
            fade_value.time = stream.readUint32();
            fade_value.curve = stream.readUint32();
            fade_value.offset = stream.readUint32();
            return;
        }

        //------------------------------------

        inline static auto exchange_section_sub(
            DataStreamView &stream,
            AudioSwitcherSetting &switcher_value) -> void
        {
            if (k_version < 112_ui)
            {
                exchange_bit_multi<uint32_t>(stream, switcher_value.is_state);
            }
            else
            {
                exchange_bit_multi<uint8_t>(stream, switcher_value.is_state);
            }
            switcher_value.group = stream.readUint32();
            switcher_value.default_item = stream.readUint32();
            return;
        }

        //------------------------------------

        inline static auto exchange_section_sub(
            DataStreamView &stream,
            MusicTrackTransitionSetting &transition_value) -> void
        {
            if (k_version >= 112_ui)
            {
             //   transition_value.switcher = stream.readUint32();
                exchange_section_sub(stream, transition_value.source.fade_out);
                exchange_bit_multi<uint32_t>(stream, transition_value.source.exit_source_at);
                transition_value.source.exit_source_at_custom_cue_match = stream.readUint32();
                exchange_section_sub(stream, transition_value.destination.fade_in);
            }
            return;
        }

        //------------------------------------

        inline static auto exchange_section_sub(
            DataStreamView &stream,
            MusicTrackTrackType &track_type_value) -> void
        {
            if (k_version < 112_ui)
            {
                exchange_bit_multi<uint32_t>(stream, track_type_value);
            }
            return;
        }

        //------------------------------------

        inline static auto exchange_section_sub(
            DataStreamView &stream,
            MusicTrackTrackType &track_type_value,
            AudioSwitcherSetting &switcher_value,
            MusicTrackTransitionSetting &transition_value) -> void
        {
            if (k_version >= 112_ui)
            {
                exchange_bit_multi<uint8_t>(stream, track_type_value);
                if (track_type_value == MusicTrackTrackType::switcher)
                {
                    exchange_section_sub(stream, switcher_value);
                    exchange_section_sub<uint32_t>(stream, switcher_value.association);
                    exchange_section_sub(stream, transition_value);
                }
            }
            return;
        }

        //------------------------------------

        inline static auto exchange_section_sub(
            DataStreamView &stream,
            MusicTrackStream &stream_value) -> void
        {
            stream_value.look_ahead_time = stream.readUint16();
            return;
        }

        //------------------------------------

        inline static auto exchange_section_sub(
            DataStreamView &stream,
            std::vector<EffectU1> &u1_value) -> void
        {
            if (k_version >= 112_ui)
            {
                exchange_list(stream, u1_value, [](auto &data, auto &value)
                              {
                    value.type = data.readUint8();
                    if (k_version >= 128_ui) {
                        exchange_bit_multi<uint8_t>(data, value.mode);
                    }
                    value.value = data.readFloat(); }, stream.readUint16());
            }
            return;
        }

        //------------------------------------

        inline static auto exchange_section_sub(
            DataStreamView &stream,
            BusHDRSetting &hdr_value) -> void
        {
            if (k_version >= 88_ui)
            {
                if (k_version < 112_ui)
                {
                    exchange_bit_multi<uint8_t>(stream, hdr_value.enable);
                    exchange_bit_multi<uint8_t>(stream, hdr_value.dynamic.release_mode);
                }
                else
                {
                    exchange_bit_multi<uint8_t>(stream, hdr_value.enable, hdr_value.dynamic.release_mode);
                }
            }
            return;
        }

        //------------------------------------

        inline static auto exchange_section_sub(
            DataStreamView &stream,
            AudioTimeSetting &time_setting_value,
            bool &time_setting_override) -> void
        {
            time_setting_value.time = stream.readDouble();
            time_setting_value.offset = stream.readDouble();
            time_setting_value.tempo = stream.readFloat();
            time_setting_value.signature.first = stream.readUint8();
            time_setting_value.signature.second = stream.readUint8();
            if (k_version < 140_ui)
            {
                auto b2 = false;
                auto b3 = false;
                auto b4 = false;
                auto b5 = false;
                auto b6 = false;
                auto b7 = false;
                auto b8 = false;
                exchange_bit_multi<uint8_t>(stream, time_setting_override, b2, b3, b4, b5, b6, b7, b8);
                assert_conditional(b2 == b3 && b3 == b4 && b4 == b5 && b5 == b6 && b6 == b7 && b7 == b8, fmt::format("{}", Language::get("wwise.sound_bank.invalid_time_setting_override")), "exchange_section_sub");
            }
            else
            {
                exchange_bit_multi<uint8_t>(stream, time_setting_override);
            }
            return;
        }

        //------------------------------------

        inline static auto exchange_section_sub(
            DataStreamView &stream,
            MusicStingerSetting &stinger_value) -> void
        {
            exchange_list(stream, stinger_value.item, [](auto &data, auto &value)
                          {
							value.trigger = data.readUint32();
							value.segment_to_play = data.readUint32();
							exchange_bit_multi<uint32_t>(data, value.play_at);
							value.cue_name = data.readUint32();
							value.do_not_play_this_stinger_again_for = data.readUint32();
							exchange_bit_multi<uint32_t>(data, value.allow_playing_stinger_in_next_segment); }, stream.readUint32());
            return;
        }

        //------------------------------------

        inline static auto exchange_section_sub(
            DataStreamView &stream,
            AudioBusConfiguration &bus_configuration_value) -> void
        {
            if (k_version >= 88_ui)
            {
                bus_configuration_value.u1 = stream.readUint32();
            }
            return;
        }

        //------------------------------------

        inline static auto exchange_section_sub(
            DataStreamView &stream,
            MusicTransitionSetting &transition_value) -> void
        {
            exchange_list(stream, transition_value.item, [](auto &data, auto &value)
                          {
                            exchange_section_sub<uint32_t>(data, value.source.id);
                            exchange_section_sub<uint32_t>(data, value.destination.id);
                exchange_section_sub(data, value.source.fade_out);
                exchange_bit_multi<uint32_t>(data, value.source.exit_source_at);
                value.source.exit_source_at_custom_cue_match = data.readUint32();
                if (k_version < 140_ui) {
                    exchange_bit_multi<uint8_t>(data, value.source.play_post_exit); //TODO.
                }
                else {
                    exchange_bit_multi<uint8_t>(data, value.source.play_post_exit);
                }
                exchange_section_sub(data, value.destination.fade_in);
                value.destination.custom_cue_filter_match_target = data.readUint32();
                value.u1 = data.readUint32();
                if (k_version >= 134_ui) {
                    exchange_bit_multi<uint16_t>(data, value.destination.jump_to);
                }
                exchange_bit_multi<uint16_t>(data, value.destination.synchronize_to);
                if (k_version < 140_ui) {
                    exchange_bit_multi<uint8_t>(data, value.destination.play_pre_entry); //TODO.
                }
                else {
                    exchange_bit_multi<uint8_t>(data, value.destination.play_pre_entry);
                }
                exchange_bit_multi<uint8_t>(data, value.destination.custom_cue_filter_match_source_cue_name);
                exchange_bit_multi<uint8_t>(data, value.segment.enable);
                auto has_segment_data = false;
                if (k_version < 88_ui) {
                    has_segment_data = true;
                }
                else {
                    has_segment_data = value.segment.enable;
                }
                if (has_segment_data) {
                    value.segment.id = data.readUint32();
                    exchange_section_sub(data, value.segment.fade_in);
                    exchange_section_sub(data, value.segment.fade_out);
                    if (k_version < 140_ui) {
                        exchange_bit_multi<uint8_t>(data, value.segment.play_pre_entry);
                    }
                    else {
                        exchange_bit_multi<uint8_t>(data, value.segment.play_pre_entry); //TODO.
                    }
                    if (k_version < 140_ui) {
                        exchange_bit_multi<uint8_t>(data, value.segment.play_post_exit);
                    }
                    else {
                        exchange_bit_multi<uint8_t>(data, value.segment.play_pre_entry);
                    }
                } }, stream.readUint32());
            return;
        }

        //-----------------------------------

        inline static auto exchange_section_sub(
            DataStreamView &stream,
            AudioAssociationSetting &association_value) -> void
        {
            auto argument_size = stream.readUint32();
            exchange_list(stream, association_value.argument, [](auto &data, auto &value)
                          { value.id = data.readUint32(); }, argument_size);
            if (k_version >= 88_ui)
            {
                exchange_list(stream, association_value.argument, [](auto &data, auto &value)
                              { exchange_bit_multi<uint8_t>(data, value.is_state); });
            }
            auto path_size = stream.readUint32();
            assert_conditional(path_size % 12_size == 0_size, fmt::format("{}", Language::get("wwise.sound_bank.invalid_path_size")), "exchange_section_sub");
            path_size /= 12_size;
            if (k_version < 88_ui)
            {
                association_value.probability = stream.readUint8();
            }
            exchange_bit_multi<uint8_t>(stream, association_value.mode);
            exchange_list(stream, association_value.path, [](auto &data, auto &value)
                          {
                value.key = data.readUint32();
                value.object = data.readUint32();
                value.weight = data.readUint16();
                value.probability = data.readUint16(); }, path_size);
            return;
        }

        //-----------------------------------

        inline static auto exchange_section_sub(
            DataStreamView &stream,
            SoundPlaylistContainerScope &scope_value,
            AudioPlayType &play_type_value,
            AudioPlayTypeSetting &play_type_setting_value,
            AudioPlayMode &play_mode_value,
            AudioPlayModeSetting &play_mode_setting_value) -> void
        {
            play_mode_setting_value.continuous.loop.value = stream.readUint16();
            if (k_version >= 88_ui)
            {
                play_mode_setting_value.continuous.loop.minimum_value = stream.readUint16();
                play_mode_setting_value.continuous.loop.maximum_value = stream.readUint16();
            }
            play_mode_setting_value.continuous.transition_duration.value = stream.readFloat();
            play_mode_setting_value.continuous.transition_duration.minimum_value = stream.readFloat();
            play_mode_setting_value.continuous.transition_duration.maximum_value = stream.readFloat();
            play_type_setting_value.random.avoid_repeat = stream.readUint16();
            exchange_bit_multi<uint8_t>(stream, play_mode_setting_value.continuous.transition_type);
            exchange_bit_multi<uint8_t>(stream, play_type_setting_value.random.type);
            exchange_bit_multi<uint8_t>(stream, play_type_value);
            if (k_version < 112_ui)
            {
                exchange_bit_multi<uint8_t>(stream, k_bitset_boolean_fill);
                exchange_bit_multi<uint8_t>(stream, play_mode_setting_value.continuous.always_reset_playlist);
                exchange_bit_multi<uint8_t>(stream, play_type_setting_value.sequence.at_end_of_playlist);
                exchange_bit_multi<uint8_t>(stream, play_mode_value);
                exchange_bit_multi<uint8_t>(stream, scope_value);
            }
            else
            {
                exchange_bit_multi<uint8_t>(stream,
                                            k_bitset_boolean_fill,
                                            play_mode_setting_value.continuous.always_reset_playlist,
                                            play_type_setting_value.sequence.at_end_of_playlist,
                                            play_mode_value,
                                            scope_value);
            }
            return;
        }

        //-----------------------------------

        inline static auto exchange_section_sub(
            DataStreamView &stream,
            std::vector<SoundPlaylistContainerPlaylistItem> &playlist_value) -> void
        {
            exchange_list(stream, playlist_value, [](auto &data, auto &value)
                          {
                value.item = data.readUint32();
                value.weight = data.readUint32(); }, stream.readUint16());
            return;
        }

        //-----------------------------------

        inline static auto exchange_section_sub(
            DataStreamView &stream,
            std::vector<SoundSwitchContainerObjectAttributeItem> &object_attribute_value) -> void
        {
            exchange_list(stream, object_attribute_value, [](auto &data, auto &value)
                          {
                value.id = data.readUint32();
                if (k_version < 112_ui) {
                    exchange_bit_multi<uint8_t>(data, value.play_first_only);
                    exchange_bit_multi<uint8_t>(data, value.continue_to_play_across_switch);
                }
                else {
                    exchange_bit_multi<uint8_t>(data, value.play_first_only, value.continue_to_play_across_switch);
                }
                if (k_version < 112_ui) {
                    value.u1 = data.readUint32();
                }
                else {
                    value.u1 = data.readUint8();
                }
                value.fade_out_time = data.readUint32();
                value.fade_in_time = data.readUint32(); }, stream.readUint32());
            return;
        }

        //-----------------------------------

        inline static auto exchange_section_sub(
            DataStreamView &stream,
            std::vector<SoundSwitchContainerObjectAssignItem> &assigned_object_value) -> void
        {
            exchange_list(stream, assigned_object_value, [](auto &data, auto &value)
                          {
                value.item = data.readUint32();
                exchange_list(data, value.object, [] (auto &data, auto &value) {
                    value = data.readUint32();
                }, data.readUint32()); }, stream.readUint32());
            return;
        }

        //-----------------------------------

        inline static auto exchange_section_sub(
            DataStreamView &stream,
            std::vector<SoundBlendContainerTrackItem> &track_value) -> void
        {
            exchange_list(stream, track_value, [](auto &data, auto &value)
                          {
                value.id = data.readUint32();
                exchange_section_sub(data, value.real_time_parameter_control);
                value.cross_fade.id = data.readUint32();
                exchange_bit_multi<uint8_t>(data, value.cross_fade.category);
                exchange_list(data, value.child, [] (auto &data, auto &value) {
                    value.id = data.readUint32();
                    exchange_list(data, value.point, [] (auto &data, auto &value) {
                         value.position.x = data.readFloat();
                         value.position.y = data.readFloat();
                         exchange_bit_multi<uint32_t>(data, value.curve);
                    }, data.readUint32());
                }, data.readUint32()); }, stream.readUint32());
            return;
        }

        //-----------------------------------

        inline static auto exchange_section_sub(
            DataStreamView &stream,
            MusicSegmentCue &cue_value) -> void
        {
            exchange_list(stream, cue_value.item, [](auto &data, auto &value)
                          {
                value.id = data.readUint32();
                value.time = data.readDouble();
                if (k_version < 140_ui) {
                    value.name = data.readStringByUint32();
                }
                else {
                    value.name = data.readStringByUint8();
                } }, stream.readUint32());
            return;
        }

        //-----------------------------------

        inline static auto exchange_section_sub(
            DataStreamView &stream,
            std::vector<MusicPlaylistContainerPlaylistItem> &playlist_value) -> void
        {
            exchange_list(stream, playlist_value, [](auto &data, auto &value)
                          {
                value.item = data.readUint32();
                value.u1 = data.readUint32();
                value.child_count = data.readUint32();
                exchange_bit_multi<uint32_t>(data, value.play_mode, value.play_type); //TODO.
                value.loop = data.readUint16();
                if (k_version >= 112_ui) {
                    exchange_raw_constant(data, 0_ui);
                }
                value.weight = data.readUint32();
                value.random_setting.avoid_repeat = data.readUint16();
                exchange_bit_multi<uint8_t>(data, value.group);
                exchange_bit_multi<uint8_t>(data, value.random_setting.type); }, stream.readUint32());
            return;
        }

        //-----------------------------------

        inline static auto exchange_section_sub(
            DataStreamView &stream,
            std::vector<MusicSwitchContainerAssociationItem> &association_value) -> void
        {
            if (k_version < 88_ui)
            {
                exchange_list(stream, association_value, [](auto &data, auto &value)
                              {
                value.item = data.readUint32();
                value.child = data.readUint32(); }, stream.readUint32());
            }
            return;
        }

        //-----------------------------------

        inline static auto exchange_section_sub(
            DataStreamView &stream,
            AudioPlayMode &play_mode_value) -> void
        {
            exchange_bit_multi<uint8_t>(stream, play_mode_value);
            return;
        }

        //-----------------------------------

        inline static auto exchange_section_sub(
            DataStreamView &stream,
            BusAutomaticDuckingSetting &automatic_ducking_value) -> void
        {
            automatic_ducking_value.recovery_time = stream.readUint32();
            automatic_ducking_value.maximum_ducking_volume = stream.readFloat();
            exchange_list(stream, automatic_ducking_value.bus, [](auto &data, auto &value)
                          {
                value.id = data.readUint32();
                value.volume = data.readFloat();
                value.fade_out = data.readUint32();
                value.fade_in = data.readUint32();
                exchange_bit_multi<uint8_t>(data, value.curve);
                exchange_bit_multi<uint8_t>(data, value.target); }, stream.readUint32());
            return;
        }

        //------------------------------------

        inline static auto exchange_section(
            DataStreamView &stream,
            StatefulPropertySetting &value) -> void
        {
            if (k_version < 128_ui)
            {
                auto size = stream.readUint8();
                exchange_list(stream, value.value, [](auto &data, auto &value)
                              { value.type = data.readUint8(); }, size);
                exchange_list(stream, value.value, [](auto &data, auto &value)
                              { value.value = data.readFloat(); });
            }
            else
            {
                auto size = stream.readUint16();
                exchange_list(stream, value.value, [](auto &data, auto &value)
                              { value.type = data.readUint16(); }, size);
                exchange_list(stream, value.value, [](auto &data, auto &value)
                              { value.value = data.readFloat(); });
            }
            return;
        }

        //------------------------------------

        inline static auto exchange_section(
            DataStreamView &stream,
            EventAction &value) -> void
        {
            exchange_bit_multi<uint8_t>(stream, value.scope, value.mode);
            value.type = stream.readUint8();
            value.target = stream.readUint32();
            value.u1 = stream.readUint8();
            auto exchange_section_sub_of_exception_list = [&]() -> void
            {
                if (k_version < 125)
                {
                    exchange_list(stream, value.exception, [](auto &data, auto &value)
                                  {
                        value.id = data.readUint32();
                        exchange_bit_multi<uint8_t>(data, value.u1); }, stream.readUint32());
                }
                else
                {
                    exchange_list(stream, value.exception, [](auto &data, auto &value)
                                  {
                        value.id = data.readUint32();
                        exchange_bit_multi<uint8_t>(data, value.u1); }, stream.readUint8());
                }
                return;
            };
            auto has_case = false;
            switch (value.type)
            {
            case 4_byte:
            {
                value.action_type = EventActionProperty::Type::play_audio;
                auto &property_value = value.play_audio;
                exchange_section_sub<EventActionCommonPropertyType>(
                    stream, true, [&](auto &common_property)
                    {
                            convert_common_property_as_randomizable<EventActionCommonPropertyType::delay>(common_property, property_value.delay);
                            convert_common_property_as_randomizable<EventActionCommonPropertyType::fade_time>(common_property, property_value.fade_time);
                            convert_common_property<EventActionCommonPropertyType::probability>(common_property, property_value.probability); });
                exchange_bit_multi<uint8_t>(stream, property_value.fade_curve);
                property_value.sound_bank = stream.readUint32();
                if (k_version >= 145_ui)
                {
                    exchange_raw_constant(stream, 0_ui);
                }
                has_case = true;
                break;
            }
            case 1_byte:
            {
                value.action_type = EventActionProperty::Type::stop_audio;
                auto &property_value = value.stop_audio;
                exchange_section_sub<EventActionCommonPropertyType>(
                    stream, true, [&](auto &common_property)
                    {
                            convert_common_property_as_randomizable<EventActionCommonPropertyType::delay>(common_property, property_value.delay);
                            convert_common_property_as_randomizable<EventActionCommonPropertyType::fade_time>(common_property, property_value.fade_time); });
                exchange_bit_multi<uint8_t>(stream, property_value.fade_curve);
                if (k_version >= 125_ui)
                {
                    exchange_bit_multi<uint8_t>(stream, k_bitset_boolean_fill, property_value.resume_state_transition, property_value.apply_to_dynamic_sequence);
                }
                exchange_section_sub_of_exception_list();
                has_case = true;
                break;
            }
            case 2_byte:
            {
                value.action_type = EventActionProperty::Type::pause_audio;
                auto &property_value = value.pause_audio;
                exchange_section_sub<EventActionCommonPropertyType>(
                    stream, true, [&](auto &common_property)
                    {
                            convert_common_property_as_randomizable<EventActionCommonPropertyType::delay>(common_property, property_value.delay);
                            convert_common_property_as_randomizable<EventActionCommonPropertyType::fade_time>(common_property, property_value.fade_time); });
                exchange_bit_multi<uint8_t>(stream, property_value.fade_curve);
                if (k_version < 125_ui)
                {
                    exchange_bit_multi<uint8_t>(stream, property_value.include_delayed_resume_action);
                }
                else
                {
                    exchange_bit_multi<uint8_t>(stream, property_value.include_delayed_resume_action, property_value.resume_state_transition, property_value.apply_to_dynamic_sequence);
                }
                exchange_section_sub_of_exception_list();
                has_case = true;
                break;
            }
            case 3_byte:
            {
                value.action_type = EventActionProperty::Type::resume_audio;
                auto &property_value = value.resume_audio;
                exchange_section_sub<EventActionCommonPropertyType>(
                    stream, true, [&](auto &common_property)
                    {
                            convert_common_property_as_randomizable<EventActionCommonPropertyType::delay>(common_property, property_value.delay);
                            convert_common_property_as_randomizable<EventActionCommonPropertyType::fade_time>(common_property, property_value.fade_time); });
                exchange_bit_multi<uint8_t>(stream, property_value.fade_curve);
                if (k_version < 125_ui)
                {
                    exchange_bit_multi<uint8_t>(stream, property_value.master_resume);
                }
                else
                {
                    exchange_bit_multi<uint8_t>(stream, property_value.master_resume, property_value.resume_state_transition, property_value.apply_to_dynamic_sequence);
                }
                exchange_section_sub_of_exception_list();
                has_case = true;
                break;
            }
            case 28_byte:
            {
                value.action_type = EventActionProperty::Type::break_audio;
                auto &property_value = value.break_audio;
                exchange_section_sub<EventActionCommonPropertyType>(
                    stream, true, [&](auto &common_property)
                    { convert_common_property_as_randomizable<EventActionCommonPropertyType::delay>(common_property, property_value.delay); });
                has_case = true;
                break;
            }
            case 30_byte:
            {
                value.action_type = EventActionProperty::Type::seek_audio;
                auto &property_value = value.seek_audio;
                exchange_section_sub<EventActionCommonPropertyType>(
                    stream, true, [&](auto &common_property)
                    { convert_common_property_as_randomizable<EventActionCommonPropertyType::delay>(common_property, property_value.delay); });
                exchange_bit_multi<uint8_t>(stream, property_value.seek_type);
                property_value.seek_value.value = stream.readFloat();
                property_value.seek_value.minimum_value = stream.readFloat();
                property_value.seek_value.maximum_value = stream.readFloat();
                exchange_bit_multi<uint8_t>(stream, property_value.seek_to_nearest_marker);
                exchange_section_sub_of_exception_list();
                has_case = true;
                break;
            }
            case 33_byte:
            {
                value.action_type = EventActionProperty::Type::post_event;
                auto &property_value = value.post_event;
                exchange_section_sub<EventActionCommonPropertyType>(
                    stream, true, [&](auto &common_property)
                    { convert_common_property_as_randomizable<EventActionCommonPropertyType::delay>(common_property, property_value.delay); });
                has_case = true;
                break;
            }
            case 8_byte:
            case 9_byte:
            {
                value.action_type = EventActionProperty::Type::set_voice_pitch;
                auto &property_value = value.set_voice_pitch;
                property_value.reset = value.type == 9_byte;
                exchange_section_sub<EventActionCommonPropertyType>(
                    stream, true, [&](auto &common_property)
                    {
                            convert_common_property_as_randomizable<EventActionCommonPropertyType::delay>(common_property, property_value.delay);
                            convert_common_property_as_randomizable<EventActionCommonPropertyType::fade_time>(common_property, property_value.fade_time); });
                exchange_bit_multi<uint8_t>(stream, property_value.fade_curve);
                exchange_bit_multi<uint8_t>(stream, property_value.apply_mode);
                property_value.value.value = stream.readFloat();
                property_value.value.minimum_value = stream.readFloat();
                property_value.value.maximum_value = stream.readFloat();
                exchange_section_sub_of_exception_list();
                has_case = true;
                break;
            }
            case 10_byte:
            case 11_byte:
            {
                value.action_type = EventActionProperty::Type::set_voice_volume;
                auto &property_value = value.set_voice_volume;
                property_value.reset = value.type == 11_byte;
                exchange_section_sub<EventActionCommonPropertyType>(
                    stream, true, [&](auto &common_property)
                    {
                            convert_common_property_as_randomizable<EventActionCommonPropertyType::delay>(common_property, property_value.delay);
                            convert_common_property_as_randomizable<EventActionCommonPropertyType::fade_time>(common_property, property_value.fade_time); });
                exchange_bit_multi<uint8_t>(stream, property_value.fade_curve);
                exchange_bit_multi<uint8_t>(stream, property_value.apply_mode);
                property_value.value.value = stream.readFloat();
                property_value.value.minimum_value = stream.readFloat();
                property_value.value.maximum_value = stream.readFloat();
                exchange_section_sub_of_exception_list();
                has_case = true;
                break;
            }
            case 12_byte:
            case 13_byte:
            {
                value.action_type = EventActionProperty::Type::set_bus_volume;
                auto &property_value = value.set_bus_volume;
                property_value.reset = value.type == 13_byte;
                exchange_section_sub<EventActionCommonPropertyType>(
                    stream, true, [&](auto &common_property)
                    {
                            convert_common_property_as_randomizable<EventActionCommonPropertyType::delay>(common_property, property_value.delay);
                            convert_common_property_as_randomizable<EventActionCommonPropertyType::fade_time>(common_property, property_value.fade_time); });
                exchange_bit_multi<uint8_t>(stream, property_value.fade_curve);
                exchange_bit_multi<uint8_t>(stream, property_value.apply_mode);
                property_value.value.value = stream.readFloat();
                property_value.value.minimum_value = stream.readFloat();
                property_value.value.maximum_value = stream.readFloat();
                exchange_section_sub_of_exception_list();
                has_case = true;
                break;
            }
            case 14_byte:
            case 15_byte:
            {
                value.action_type = EventActionProperty::Type::set_voice_low_pass_filter;
                auto &property_value = value.set_voice_low_pass_filter;
                property_value.reset = value.type == 15_byte;
                exchange_section_sub<EventActionCommonPropertyType>(
                    stream, true, [&](auto &common_property)
                    {
                            convert_common_property_as_randomizable<EventActionCommonPropertyType::delay>(common_property, property_value.delay);
                            convert_common_property_as_randomizable<EventActionCommonPropertyType::fade_time>(common_property, property_value.fade_time); });
                exchange_bit_multi<uint8_t>(stream, property_value.fade_curve);
                exchange_bit_multi<uint8_t>(stream, property_value.apply_mode);
                property_value.value.value = stream.readFloat();
                property_value.value.minimum_value = stream.readFloat();
                property_value.value.maximum_value = stream.readFloat();
                exchange_section_sub_of_exception_list();
                has_case = true;
                break;
            }
            case 32_byte:
            case 48_byte:
            {
                if (k_version >= 112_ui)
                {
                    value.action_type = EventActionProperty::Type::set_voice_high_pass_filter;
                    auto &property_value = value.set_voice_high_pass_filter;
                    property_value.reset = value.type == 48_byte;
                    exchange_section_sub<EventActionCommonPropertyType>(
                        stream, true, [&](auto &common_property)
                        {
                            convert_common_property_as_randomizable<EventActionCommonPropertyType::delay>(common_property, property_value.delay);
                            convert_common_property_as_randomizable<EventActionCommonPropertyType::fade_time>(common_property, property_value.fade_time); });
                    exchange_bit_multi<uint8_t>(stream, property_value.fade_curve);
                    exchange_bit_multi<uint8_t>(stream, property_value.apply_mode);
                    property_value.value.value = stream.readFloat();
                    property_value.value.minimum_value = stream.readFloat();
                    property_value.value.maximum_value = stream.readFloat();
                    exchange_section_sub_of_exception_list();
                    has_case = true;
                }
                break;
            }
            case 6_byte:
            case 7_byte:
            {
                value.action_type = EventActionProperty::Type::set_mute;
                auto &property_value = value.set_mute;
                property_value.reset = value.type == 7_byte;
                exchange_section_sub<EventActionCommonPropertyType>(
                    stream, true, [&](auto &common_property)
                    {
                            convert_common_property_as_randomizable<EventActionCommonPropertyType::delay>(common_property, property_value.delay);
                            convert_common_property_as_randomizable<EventActionCommonPropertyType::fade_time>(common_property, property_value.fade_time); });
                exchange_bit_multi<uint8_t>(stream, property_value.fade_curve);
                exchange_section_sub_of_exception_list();
                has_case = true;
                break;
            }
            case 19_byte:
            case 20_byte:
            {
                value.action_type = EventActionProperty::Type::set_game_parameter;
                auto &property_value = value.set_game_parameter;
                property_value.reset = value.type == 20_byte;
                exchange_section_sub<EventActionCommonPropertyType>(
                    stream, true, [&](auto &common_property)
                    {
                            convert_common_property_as_randomizable<EventActionCommonPropertyType::delay>(common_property, property_value.delay);
                            convert_common_property_as_randomizable<EventActionCommonPropertyType::fade_time>(common_property, property_value.fade_time); });
                exchange_bit_multi<uint8_t>(stream, property_value.fade_curve);
                if (k_version >= 112_ui)
                {
                    exchange_bit_multi<uint8_t>(stream, property_value.bypass_game_parameter_interpolation);
                }
                exchange_bit_multi<uint8_t>(stream, property_value.apply_mode);
                property_value.value.value = stream.readFloat();
                property_value.value.minimum_value = stream.readFloat();
                property_value.value.maximum_value = stream.readFloat();
                exchange_section_sub_of_exception_list();
                has_case = true;
                break;
            }
            case 16_byte:
            case 17_byte:
            {
                value.action_type = EventActionProperty::Type::set_state_availability;
                auto &property_value = value.set_state_availability;
                property_value.enable = value.type == 16_byte;
                exchange_section_sub<EventActionCommonPropertyType>(
                    stream, true, [&](auto &common_property)
                    { convert_common_property_as_randomizable<EventActionCommonPropertyType::delay>(common_property, property_value.delay); });
                has_case = true;
                break;
            }
            case 18_byte:
            {
                value.action_type = EventActionProperty::Type::activate_state;
                auto &property_value = value.activate_state;
                exchange_section_sub<EventActionCommonPropertyType>(
                    stream, true, [&](auto &common_property)
                    { convert_common_property_as_randomizable<EventActionCommonPropertyType::delay>(common_property, property_value.delay); });
                property_value.group = stream.readUint32();
                property_value.item = stream.readUint32();
                has_case = true;
                break;
            }
            case 25_byte:
            {
                value.action_type = EventActionProperty::Type::activate_switch;
                auto &property_value = value.activate_switch;
                exchange_section_sub<EventActionCommonPropertyType>(
                    stream, true, [&](auto &common_property)
                    { convert_common_property_as_randomizable<EventActionCommonPropertyType::delay>(common_property, property_value.delay); });
                property_value.group = stream.readUint32();
                property_value.item = stream.readUint32();
                has_case = true;
                break;
            }
            case 29_byte:
            {
                value.action_type = EventActionProperty::Type::activate_trigger;
                auto &property_value = value.activate_trigger;
                exchange_section_sub<EventActionCommonPropertyType>(
                    stream, true, [&](auto &common_property)
                    { convert_common_property_as_randomizable<EventActionCommonPropertyType::delay>(common_property, property_value.delay); });
                has_case = true;
                break;
            }
            case 26_byte:
            case 27_byte:
            {
                value.action_type = EventActionProperty::Type::set_bypass_effect;
                auto &property_value = value.set_bypass_effect;
                property_value.reset = value.type == 27_byte;
                exchange_section_sub<EventActionCommonPropertyType>(
                    stream, true, [&](auto &common_property)
                    { convert_common_property_as_randomizable<EventActionCommonPropertyType::delay>(common_property, property_value.delay); });
                exchange_bit_multi<uint8_t>(stream, property_value.enable);
                auto reset = property_value.reset;
                exchange_bit_multi<uint8_t>(stream, property_value.value[0],
                                            property_value.value[1],
                                            property_value.value[2],
                                            property_value.value[3],
                                            property_value.value[4],
                                            reset,
                                            reset,
                                            reset);
                exchange_section_sub_of_exception_list();
                has_case = true;
                break;
            }
            case 31_byte:
            {
                if (k_version >= 112_ui)
                {
                    value.action_type = EventActionProperty::Type::release_envelope;
                    auto &property_value = value.release_envelope;
                    exchange_section_sub<EventActionCommonPropertyType>(
                        stream, true, [&](auto &common_property)
                        { convert_common_property_as_randomizable<EventActionCommonPropertyType::delay>(common_property, property_value.delay); });
                    has_case = true;
                }
                break;
            }
            case 34_byte:
            {
                if (k_version >= 113_ui)
                {
                    value.action_type = EventActionProperty::Type::reset_playlist;
                    auto &property_value = value.reset_playlist;
                    exchange_section_sub<EventActionCommonPropertyType>(
                        stream, true, [&](auto &common_property)
                        { convert_common_property_as_randomizable<EventActionCommonPropertyType::delay>(common_property, property_value.delay); });
                    exchange_raw_constant(stream, 4_byte); // TODO.
                    if (k_version < 115_ui)
                    {
                        exchange_raw_constant(stream, 0_ui);
                    }
                    else
                    {
                        exchange_raw_constant(stream, 0_byte);
                    }
                    has_case = true;
                }
                break;
            }
            default:
            {
                assert_conditional(false, fmt::format("{}", Language::get("wwise.sound_bank.invalid_action_type")), "exchange_section");
            }
            };
            assert_conditional(has_case, fmt::format("{}", Language::get("wwise.sound_bank.must_has_an_action_type")), "exchange_section");
            return;
        }

        //-----------------------------------

        inline static auto exchange_section(
            DataStreamView &stream,
            Event &value) -> void
        {
            if (k_version < 125_ui)
            {
                exchange_section_sub<uint32_t>(stream, value.child);
            }
            else
            {
                exchange_section_sub<uint8_t>(stream, value.child);
            }
            return;
        }

        //-----------------------------------

        inline static auto exchange_section(
            DataStreamView &stream,
            DialogueEvent &value) -> void
        {
            if (k_version >= 88_ui)
            {
                value.probability = stream.readUint8();
            }
            exchange_section_sub(stream, value.association);
            if (k_version >= 120_ui)
            {
                exchange_raw_constant(stream, 0_us);
            }
            return;
        }

        //-----------------------------------

        inline static auto exchange_section(
            DataStreamView &stream,
            Attenuation &value) -> void
        {
            if (k_version >= 140_ui)
            {
                exchange_bit_multi<uint8_t>(stream, value.height_spread);
            }
            exchange_bit_multi<uint8_t>(stream, value.cone.enable);
            if (value.cone.enable)
            {
                value.cone.inner_angle = stream.readFloat();
                value.cone.outer_angle = stream.readFloat();
                value.cone.maximum_value = stream.readFloat();
                value.cone.low_pass_filter = stream.readFloat();
                if (k_version >= 112_ui)
                {
                    value.cone.high_pass_filter = stream.readFloat();
                }
            }
            if (k_version < 88_ui)
            {
                value.apply.output_bus_volume = stream.readUint8();
                value.apply.auxiliary_send_volume = stream.readUint8();
                value.apply.low_pass_filter = stream.readUint8();
                value.apply.spread = stream.readUint8();
            }
            else if (k_version < 112_ui)
            {
                value.apply.output_bus_volume = stream.readUint8();
                value.apply.game_defined_auxiliary_send_volume = stream.readUint8();
                value.apply.user_defined_auxiliary_send_volume = stream.readUint8();
                value.apply.low_pass_filter = stream.readUint8();
                value.apply.spread = stream.readUint8();
            }
            else if (k_version < 145_ui)
            {
                value.apply.output_bus_volume = stream.readUint8();
                value.apply.game_defined_auxiliary_send_volume = stream.readUint8();
                value.apply.user_defined_auxiliary_send_volume = stream.readUint8();
                value.apply.low_pass_filter = stream.readUint8();
                value.apply.high_pass_filter = stream.readUint8();
                value.apply.spread = stream.readUint8();
                value.apply.focus = stream.readUint8();
            }
            else
            {
                value.apply.distance_output_bus_volume = stream.readUint8();
                value.apply.distance_game_defined_auxiliary_send_volume = stream.readUint8();
                value.apply.distance_user_defined_auxiliary_send_volume = stream.readUint8();
                value.apply.distance_low_pass_filter = stream.readUint8();
                value.apply.distance_high_pass_filter = stream.readUint8();
                value.apply.distance_spread = stream.readUint8();
                value.apply.distance_focus = stream.readUint8();
                value.apply.obstruction_volume = stream.readUint8();
                value.apply.obstruction_low_pass_filter = stream.readUint8();
                value.apply.obstruction_high_pass_filter = stream.readUint8();
                value.apply.occlusion_volume = stream.readUint8();
                value.apply.occlusion_low_pass_filter = stream.readUint8();
                value.apply.occlusion_high_pass_filter = stream.readUint8();
                value.apply.diffraction_volume = stream.readUint8();
                value.apply.output_bus_volume = stream.readUint8();
                value.apply.diffraction_low_pass_filter = stream.readUint8();
                value.apply.diffraction_high_pass_filter = stream.readUint8();
                value.apply.transmission_volume = stream.readUint8();
                value.apply.transmission_low_pass_filter = stream.readUint8();
                value.apply.transmission_high_pass_filter = stream.readUint8();
            }
            exchange_list(stream, value.curve, [](auto &data, auto &value)
                          {
                exchange_bit_multi<uint8_t>(data, value.mode);
                exchange_list(data, value.point, [] (auto &data, auto &value) {
                    value.position.x = data.readFloat();
                    value.position.y = data.readFloat();
                    exchange_bit_multi<uint32_t>(data, value.curve);
                }, data.readUint16()); }, stream.readUint8());
            exchange_section_sub(stream, value.real_time_parameter_control);
            return;
        }

        //-----------------------------------

        inline static auto exchange_section(
            DataStreamView &stream,
            LowFrequencyOscillatorModulator &value) -> void
        {
            if (k_version >= 112_ui)
            {
                exchange_section_sub<ModulatorCommonPropertyType>(
                    stream, true, [&](auto &common_property)
                    {
                        convert_common_property_as_randomizable<ModulatorCommonPropertyType::depth>(common_property, value.depth);
                        convert_common_property_as_randomizable<ModulatorCommonPropertyType::frequency>(common_property, value.frequency);
                        convert_common_property<ModulatorCommonPropertyType::waveform>(common_property, value.waveform);
                        convert_common_property_as_randomizable<ModulatorCommonPropertyType::smoothing>(common_property, value.smoothing);
                        convert_common_property_as_randomizable<ModulatorCommonPropertyType::pulse_width_modulation>(common_property, value.pulse_width_modulation);
                        convert_common_property_as_randomizable<ModulatorCommonPropertyType::attack>(common_property, value.attack);
                        convert_common_property_as_randomizable<ModulatorCommonPropertyType::initial_phase_offset>(common_property, value.initial_phase_offset);
                        convert_common_property<ModulatorCommonPropertyType::scope>(common_property, value.scope); });
                exchange_section_sub(stream, value.real_time_parameter_control);
            }
            return;
        }

        //-----------------------------------

        inline static auto exchange_section(
            DataStreamView &stream,
            EnvelopeModulator &value) -> void
        {
            if (k_version >= 112_ui)
            {
                exchange_section_sub<ModulatorCommonPropertyType>(
                    stream, true, [&](auto &common_property)
                    {
							convert_common_property_as_randomizable<ModulatorCommonPropertyType::attack_time>(common_property, value.attack_time);
							convert_common_property_as_randomizable<ModulatorCommonPropertyType::attack_curve>(common_property, value.attack_curve);
							convert_common_property_as_randomizable<ModulatorCommonPropertyType::decay_time>(common_property, value.decay_time);
							convert_common_property_as_randomizable<ModulatorCommonPropertyType::sustain_level>(common_property, value.sustain_level);
							convert_common_property_as_randomizable<ModulatorCommonPropertyType::release_time>(common_property, value.release_time);
							convert_common_property<ModulatorCommonPropertyType::scope>(common_property, value.scope);
							convert_common_property<ModulatorCommonPropertyType::trigger_on>(common_property, value.trigger_on);
							convert_common_property_as_randomizable<ModulatorCommonPropertyType::sustain_time>(common_property, value.sustain_time);
							convert_common_property<ModulatorCommonPropertyType::stop_playback>(common_property, value.stop_playback_after_release); });
                exchange_section_sub(stream, value.real_time_parameter_control);
            }
            return;
        }

        //-----------------------------------

        inline static auto exchange_section(
            DataStreamView &stream,
            TimeModulator &value) -> void
        {
            if (k_version >= 132_ui)
            {
                exchange_section_sub<ModulatorCommonPropertyType>(
                    stream, true, [&](auto &common_property)
                    {
							convert_common_property_as_randomizable<ModulatorCommonPropertyType::initial_delay>(common_property, value.initial_delay);
							convert_common_property_as_regular<ModulatorCommonPropertyType::duration>(common_property, value.duration);
							convert_common_property_as_randomizable<ModulatorCommonPropertyType::loop>(common_property, value.loop);
							convert_common_property_as_randomizable<ModulatorCommonPropertyType::playback_rate>(common_property, value.playback_rate);
							convert_common_property<ModulatorCommonPropertyType::scope>(common_property, value.scope);
							convert_common_property<ModulatorCommonPropertyType::trigger_on>(common_property, value.trigger_on);					
							convert_common_property<ModulatorCommonPropertyType::stop_playback>(common_property, value.stop_playback_at_end); });
                exchange_section_sub(stream, value.real_time_parameter_control);
            }
            return;
        }

        //-----------------------------------

        inline static auto exchange_section(
            DataStreamView &stream,
            Effect &value) -> void
        {
            value.plug_in = stream.readUint32();
            value.expand = exchange_data(stream.readBytes(stream.readUint32()));
            exchange_raw_constant(stream, 0_byte);
            exchange_section_sub(stream, value.real_time_parameter_control);
            if (k_version >= 125_ui)
            {
                if (k_version < 128_ui)
                {
                    exchange_raw_constant(stream, 0_us);
                }
                else
                {
                    exchange_section_sub(stream, value.state);
                }
            }
            if (k_version >= 112_ui)
            {
                exchange_section_sub(stream, value.u1);
            }
            return;
        }

        //-----------------------------------

        inline static auto exchange_section(
            DataStreamView &stream,
            Source &value) -> void
        {
            value.plug_in = stream.readUint32();
            value.expand = exchange_data(stream.readBytes(stream.readUint32()));
            exchange_raw_constant(stream, 0_byte);
            exchange_section_sub(stream, value.real_time_parameter_control);
            if (k_version >= 125_ui)
            {
                if (k_version < 128_ui)
                {
                    exchange_raw_constant(stream, 0_us);
                }
                else
                {
                    exchange_section_sub(stream, value.state);
                }
            }
            if (k_version >= 112_ui)
            {
                exchange_section_sub(stream, value.u1);
            }
            return;
        }

        //-----------------------------------

        inline static auto exchange_section(
            DataStreamView &stream,
            AudioDevice &value) -> void
        {
            if (k_version >= 128_ui)
            {
                value.plug_in = stream.readUint32();
                value.expand = exchange_data(stream.readBytes(stream.readUint32()));
                exchange_raw_constant(stream, 0_byte);
                exchange_section_sub(stream, value.real_time_parameter_control);
                exchange_section_sub(stream, value.state);
                exchange_section_sub(stream, value.u1);
                if (k_version >= 140_ui)
                {
                    exchange_section_sub(stream, value.effect);
                }
            }
            return;
        }

        //-----------------------------------

        inline static auto exchange_section(
            DataStreamView &stream,
            AudioBus &value) -> void
        {
            value.parent = stream.readUint32();
            if (k_version >= 128_ui)
            {
                if (value.parent == 0_ui)
                {
                    value.audio_device = stream.readUint32();
                }
            }
            exchange_section_sub<AudioCommonPropertyType>(
                stream, false, [&](auto &common_property)
                {
                    load_common_property(common_property, value.voice);
                    if (k_version >= 125_ui) {
                        load_common_property(common_property, value.voice_volume_gain);
                    }
                    load_common_property(common_property, value.bus);
                    if (k_version >= 128_ui) {
                        load_common_property(common_property, value.output_bus);
                    }
                    if (k_version >= 125_ui) {
                        load_common_property(common_property, value.auxiliary_send);
                    }
                    if (k_version >= 88_ui) {
                        load_common_property(common_property, value.positioning);
                        load_common_property(common_property, value.hdr);
                    }
                    load_common_property(common_property, value.playback_limit); });
            if (k_version >= 88_ui)
            {
                if (k_version < 112_ui)
                {
                    exchange_bit_multi<uint8_t>(stream, value.override_positioning);
                    exchange_bit_multi<uint8_t>(stream, value.positioning.speaker_panning.enable);
                }
                else if (k_version < 125_ui)
                {
                    exchange_bit_multi<uint8_t>(stream, value.override_positioning, value.positioning.speaker_panning.enable);
                }
                if (k_version >= 125_ui)
                {
                    auto override_positioning = false;
                    exchange_section_sub(stream, value.positioning, override_positioning);
                    assert_conditional(override_positioning, fmt::format("{}", Language::get("wwise.sound_bank.invalid_override_positioning")), "exchange_section");
                    if (k_version < 135_ui)
                    {
                        auto override_game_defined_auxiliary_send = false;
                        auto override_user_defined_auxiliary_send = false;
                        exchange_section_sub(stream, value.auxiliary_send, override_game_defined_auxiliary_send, override_user_defined_auxiliary_send);
                        assert_conditional(override_game_defined_auxiliary_send, fmt::format("{}", Language::get("wwise.sound_bank.override_user_defined_auxiliary_send")), "exchange_section");
                        assert_conditional(override_user_defined_auxiliary_send, fmt::format("{}", Language::get("wwise.sound_bank.invalid_override_user_defined_auxiliary_send")), "exchange_section");
                    }
                    else
                    {
                        auto override_game_defined_auxiliary_send = false;
                        auto override_user_defined_auxiliary_send = false;
                        auto override_early_reflection_auxiliary_send = false;
                        exchange_section_sub(stream, value.auxiliary_send, override_game_defined_auxiliary_send, override_user_defined_auxiliary_send, override_early_reflection_auxiliary_send);
                        assert_conditional(override_game_defined_auxiliary_send, fmt::format("{}", Language::get("wwise.sound_bank.override_user_defined_auxiliary_send")), "exchange_section");
                        assert_conditional(override_user_defined_auxiliary_send, fmt::format("{}", Language::get("wwise.sound_bank.invalid_override_user_defined_auxiliary_send")), "exchange_section");
                        assert_conditional(override_early_reflection_auxiliary_send, fmt::format("{}", Language::get("wwise.sound_bank.invalid_override_early_reflection_auxiliary_send")), "exchange_section");
                    }
                }
            }
            if (k_version < 112_ui)
            {
                exchange_section_sub(stream, value.playback_limit, value.override_playback_limit);
            }
            else
            {
                exchange_section_sub(stream, value.playback_limit, value.mute_for_background_music, value.override_playback_limit);
            }
            if (k_version >= 88_ui)
            {
                exchange_section_sub(stream, value.bus_configuration);
                exchange_section_sub(stream, value.hdr);
            }
            if (k_version < 88_ui)
            {
                exchange_raw_constant(stream, 63_ui);
            }
            exchange_section_sub(stream, value.automatic_ducking);
            exchange_section_sub(stream, value.effect);
            if (k_version >= 112_ui)
            {
                if (k_version < 150_size)
                {
                    value.mixer = stream.readUint32();
                    exchange_raw_constant(stream, 0_us);
                }
            }
            if (k_version >= 140_ui)
            {
                exchange_section_sub(stream, value.metadata);
            }
            exchange_section_sub(stream, value.real_time_parameter_control);
            exchange_section_sub(stream, value.state);
            return;
        }

        //-----------------------------------

        inline static auto exchange_section(
            DataStreamView &stream,
            AuxiliaryAudioBus &value) -> void
        {
            value.parent = stream.readUint32();
            if (k_version >= 128_ui)
            {
                if (value.parent == 0_ui)
                {
                    value.audio_device = stream.readUint32();
                }
            }
            exchange_section_sub<AudioCommonPropertyType>(
                stream, false, [&](auto &common_property)
                {
                    load_common_property(common_property, value.voice);
                    if (k_version >= 125_ui) {
                        load_common_property(common_property, value.voice_volume_gain);
                    }
                    load_common_property(common_property, value.bus);
                    if (k_version >= 128_ui) {
                        load_common_property(common_property, value.output_bus);
                    }
                    if (k_version >= 125_ui) {
                        load_common_property(common_property, value.auxiliary_send);
                    }
                    if (k_version >= 88_ui) {
                        load_common_property(common_property, value.positioning);
                        load_common_property(common_property, value.hdr);
                    }
                    load_common_property(common_property, value.playback_limit); });
            if (k_version >= 88_ui)
            {
                if (k_version < 112_ui)
                {
                    exchange_bit_multi<uint8_t>(stream, value.override_positioning);
                    exchange_bit_multi<uint8_t>(stream, value.positioning.speaker_panning.enable);
                }
                else if (k_version < 125_ui)
                {
                    exchange_bit_multi<uint8_t>(stream, value.override_positioning, value.positioning.speaker_panning.enable);
                }
                if (k_version >= 125_ui)
                {
                    auto override_positioning = false;
                    exchange_section_sub(stream, value.positioning, override_positioning);
                    assert_conditional(override_positioning, fmt::format("{}", Language::get("wwise.sound_bank.invalid_override_positioning")), "exchange_section");
                    if (k_version < 135_ui)
                    {
                        auto override_game_defined_auxiliary_send = false;
                        auto override_user_defined_auxiliary_send = false;
                        exchange_section_sub(stream, value.auxiliary_send, override_game_defined_auxiliary_send, override_user_defined_auxiliary_send);
                        assert_conditional(override_game_defined_auxiliary_send, fmt::format("{}", Language::get("wwise.sound_bank.override_user_defined_auxiliary_send")), "exchange_section");
                        assert_conditional(override_user_defined_auxiliary_send, fmt::format("{}", Language::get("wwise.sound_bank.invalid_override_user_defined_auxiliary_send")), "exchange_section");
                    }
                    else
                    {
                        auto override_game_defined_auxiliary_send = false;
                        auto override_user_defined_auxiliary_send = false;
                        auto override_early_reflection_auxiliary_send = false;
                        exchange_section_sub(stream, value.auxiliary_send, override_game_defined_auxiliary_send, override_user_defined_auxiliary_send, override_early_reflection_auxiliary_send);
                        assert_conditional(override_game_defined_auxiliary_send, fmt::format("{}", Language::get("wwise.sound_bank.override_user_defined_auxiliary_send")), "exchange_section");
                        assert_conditional(override_user_defined_auxiliary_send, fmt::format("{}", Language::get("wwise.sound_bank.invalid_override_user_defined_auxiliary_send")), "exchange_section");
                        assert_conditional(override_early_reflection_auxiliary_send, fmt::format("{}", Language::get("wwise.sound_bank.invalid_override_early_reflection_auxiliary_send")), "exchange_section");
                    }
                }
            }
            if (k_version < 112_ui)
            {
                exchange_section_sub(stream, value.playback_limit, value.override_playback_limit);
            }
            else
            {
                exchange_section_sub(stream, value.playback_limit, value.mute_for_background_music, value.override_playback_limit);
            }
            if (k_version >= 88_ui)
            {
                exchange_section_sub(stream, value.bus_configuration);
                exchange_section_sub(stream, value.hdr);
            }
            if (k_version < 88_ui)
            {
                exchange_raw_constant(stream, 63_ui);
            }
            exchange_section_sub(stream, value.automatic_ducking);
            exchange_section_sub(stream, value.effect);
            if (k_version >= 112_ui)
            {
                if (k_version < 150_size)
                {
                    value.mixer = stream.readUint32();
                    exchange_raw_constant(stream, 0_us);
                }
            }
            if (k_version >= 140_ui)
            {
                exchange_section_sub(stream, value.metadata);
            }
            exchange_section_sub(stream, value.real_time_parameter_control);
            exchange_section_sub(stream, value.state);
            return;
        }

        //-----------------------------------

        inline static auto exchange_section(
            DataStreamView &stream,
            Sound &value) -> void
        {
            exchange_section_sub(stream, value.source);
            exchange_section_sub(stream, value.effect, value.override_effect);
            if (k_version >= 140_ui)
            {
                exchange_section_sub(stream, value.metadata, value.override_metadata);
            }
            if (k_version >= 112_ui && k_version < 150_ui)
            {
                exchange_section_sub(stream, value.mixer, value.override_mixer);
            }
            exchange_section_sub(stream, value.output_bus);
            value.parent = stream.readUint32();
            if (k_version < 112_ui)
            {
                exchange_section_sub(stream, value.playback_priority, value.override_playback_priority);
            }
            else
            {
                exchange_section_sub(stream, value.midi, value.playback_priority, value.override_midi_event, value.override_midi_note_tracking, value.override_playback_priority);
            }
            exchange_section_sub<AudioCommonPropertyType>(stream, true, [&](auto &common_property)
                                                          {
                if (k_version >= 88_ui) {
                    convert_common_property_as_randomizable<AudioCommonPropertyType::playback_initial_delay>(common_property, value.playback_setting.initial_delay);
                }
                convert_common_property_as_randomizable<AudioCommonPropertyType::playback_loop>(common_property, value.playback_setting.loop);
                load_common_property(common_property, value.voice);
                if (k_version >= 88_ui) {
                    load_common_property(common_property, value.voice_volume_gain);
                }
                load_common_property(common_property, value.output_bus);
                load_common_property(common_property, value.auxiliary_send);
                load_common_property(common_property, value.positioning);
                if (k_version >= 88_ui) {
                    load_common_property(common_property, value.hdr);
                }
                if (k_version >= 112_ui) {
                    load_common_property(common_property, value.midi);
                }
                load_common_property(common_property, value.playback_limit);
                load_common_property(common_property, value.virtual_voice);
                load_common_property(common_property, value.playback_priority);
                if (k_version < 128_ui) {
                    load_common_property(common_property, value.motion);
                }
                if(k_version >= 112_ui && k_version < 150_ui) {
                    load_common_property(common_property, value.mixer);
                } });
            exchange_section_sub(stream, value.positioning, value.override_positioning);
            if (k_version < 135_ui)
            {
                exchange_section_sub(stream, value.auxiliary_send, value.override_game_defined_auxiliary_send, value.override_user_defined_auxiliary_send);
            }
            else
            {
                exchange_section_sub(stream, value.auxiliary_send, value.override_game_defined_auxiliary_send, value.override_user_defined_auxiliary_send, value.override_early_reflection_auxiliary_send);
            }
            exchange_section_sub(stream, value.playback_limit, value.virtual_voice, value.override_playback_limit, value.override_virtual_voice);
            if (k_version >= 88_ui)
            {
                exchange_section_sub(stream, value.voice_volume_gain, value.hdr, value.override_voice_volume_loudness_normalization, value.override_hdr_envelope_tracking);
            }
            exchange_section_sub(stream, value.state);
            exchange_section_sub(stream, value.real_time_parameter_control);
            return;
        }

        //-----------------------------------

        inline static auto exchange_section(
            DataStreamView &stream,
            SoundPlaylistContainer &value) -> void
        {
            exchange_section_sub(stream, value.effect, value.override_effect);
            if (k_version >= 140_ui)
            {
                exchange_section_sub(stream, value.metadata, value.override_metadata);
            }
            if (k_version >= 112_ui && k_version < 150_ui)
            {
                exchange_section_sub(stream, value.mixer, value.override_mixer);
            }
            exchange_section_sub(stream, value.output_bus);
            value.parent = stream.readUint32();
            if (k_version < 112_ui)
            {
                exchange_section_sub(stream, value.playback_priority, value.override_playback_priority);
            }
            else
            {
                exchange_section_sub(stream, value.midi, value.playback_priority, value.override_midi_event, value.override_midi_note_tracking, value.override_playback_priority);
            }
            exchange_section_sub<AudioCommonPropertyType>(stream, true, [&](auto &common_property)
                                                          {
                if (k_version >= 88_ui) {
                    convert_common_property_as_randomizable<AudioCommonPropertyType::playback_initial_delay>(common_property, value.playback_setting.initial_delay);
                }
                load_common_property(common_property, value.voice);
                if (k_version >= 88_ui) {
                    load_common_property(common_property, value.voice_volume_gain);
                }
                load_common_property(common_property, value.output_bus);
                load_common_property(common_property, value.auxiliary_send);
                load_common_property(common_property, value.positioning);
                if (k_version >= 88_ui) {
                    load_common_property(common_property, value.hdr);
                }
                if (k_version >= 112_ui) {
                    load_common_property(common_property, value.midi);
                }
                load_common_property(common_property, value.playback_limit);
                load_common_property(common_property, value.virtual_voice);
                load_common_property(common_property, value.playback_priority);
                if (k_version < 128_ui) {
                    load_common_property(common_property, value.motion);
                }
                if(k_version >= 112_ui && k_version < 150_ui) {
                    load_common_property(common_property, value.mixer);
                } });
            exchange_section_sub(stream, value.positioning, value.override_positioning);
            if (k_version < 135_ui)
            {
                exchange_section_sub(stream, value.auxiliary_send, value.override_game_defined_auxiliary_send, value.override_user_defined_auxiliary_send);
            }
            else
            {
                exchange_section_sub(stream, value.auxiliary_send, value.override_game_defined_auxiliary_send, value.override_user_defined_auxiliary_send, value.override_early_reflection_auxiliary_send);
            }
            exchange_section_sub(stream, value.playback_limit, value.virtual_voice, value.override_playback_limit, value.override_virtual_voice);
            if (k_version >= 88_ui)
            {
                exchange_section_sub(stream, value.voice_volume_gain, value.hdr, value.override_voice_volume_loudness_normalization, value.override_hdr_envelope_tracking);
            }
            exchange_section_sub(stream, value.state);
            exchange_section_sub(stream, value.real_time_parameter_control);
            exchange_section_sub(stream, value.playback_setting.scope, value.playback_setting.type, value.playback_setting.type_setting, value.playback_setting.mode, value.playback_setting.mode_setting);
            exchange_section_sub<uint32_t>(stream, value.child);
            exchange_section_sub(stream, value.playback_setting.playlist);
            return;
        }

        //-----------------------------------

        inline static auto exchange_section(
            DataStreamView &stream,
            SoundSwitchContainer &value) -> void
        {
            exchange_section_sub(stream, value.effect, value.override_effect);
            if (k_version >= 140_ui)
            {
                exchange_section_sub(stream, value.metadata, value.override_metadata);
            }
            if (k_version >= 112_ui && k_version < 150_ui)
            {
                exchange_section_sub(stream, value.mixer, value.override_mixer);
            }
            exchange_section_sub(stream, value.output_bus);
            value.parent = stream.readUint32();
            if (k_version < 112_ui)
            {
                exchange_section_sub(stream, value.playback_priority, value.override_playback_priority);
            }
            else
            {
                exchange_section_sub(stream, value.midi, value.playback_priority, value.override_midi_event, value.override_midi_note_tracking, value.override_playback_priority);
            }
            exchange_section_sub<AudioCommonPropertyType>(stream, true, [&](auto &common_property)
                                                          {
                if (k_version >= 88_ui) {
                    convert_common_property_as_randomizable<AudioCommonPropertyType::playback_initial_delay>(common_property, value.playback_setting.initial_delay);
                }
                load_common_property(common_property, value.voice);
                if (k_version >= 88_ui) {
                    load_common_property(common_property, value.voice_volume_gain);
                }
                load_common_property(common_property, value.output_bus);
                load_common_property(common_property, value.auxiliary_send);
                load_common_property(common_property, value.positioning);
                if (k_version >= 88_ui) {
                    load_common_property(common_property, value.hdr);
                }
                if (k_version >= 112_ui) {
                    load_common_property(common_property, value.midi);
                }
                load_common_property(common_property, value.playback_limit);
                load_common_property(common_property, value.virtual_voice);
                load_common_property(common_property, value.playback_priority);
                if (k_version < 128_ui) {
                    load_common_property(common_property, value.motion);
                }
                if(k_version >= 112_ui && k_version < 150_ui) {
                    load_common_property(common_property, value.mixer);
                } });
            exchange_section_sub(stream, value.positioning, value.override_positioning);
            if (k_version < 135_ui)
            {
                exchange_section_sub(stream, value.auxiliary_send, value.override_game_defined_auxiliary_send, value.override_user_defined_auxiliary_send);
            }
            else
            {
                exchange_section_sub(stream, value.auxiliary_send, value.override_game_defined_auxiliary_send, value.override_user_defined_auxiliary_send, value.override_early_reflection_auxiliary_send);
            }
            exchange_section_sub(stream, value.playback_limit, value.virtual_voice, value.override_playback_limit, value.override_virtual_voice);
            if (k_version >= 88_ui)
            {
                exchange_section_sub(stream, value.voice_volume_gain, value.hdr, value.override_voice_volume_loudness_normalization, value.override_hdr_envelope_tracking);
            }
            exchange_section_sub(stream, value.state);
            exchange_section_sub(stream, value.real_time_parameter_control);
            exchange_section_sub(stream, value.playback_setting.switcher);
            exchange_section_sub(stream, value.playback_setting.mode);
            exchange_section_sub<uint32_t>(stream, value.child);
            exchange_section_sub(stream, value.playback_setting.object_assign);
            exchange_section_sub(stream, value.playback_setting.object_attribute);
            return;
        }

        //-----------------------------------

        inline static auto exchange_section(
            DataStreamView &stream,
            SoundBlendContainer &value) -> void
        {
            exchange_section_sub(stream, value.effect, value.override_effect);
            if (k_version >= 140_ui)
            {
                exchange_section_sub(stream, value.metadata, value.override_metadata);
            }
            if (k_version >= 112_ui && k_version < 150_ui)
            {
                exchange_section_sub(stream, value.mixer, value.override_mixer);
            }
            exchange_section_sub(stream, value.output_bus);
            value.parent = stream.readUint32();
            if (k_version < 112_ui)
            {
                exchange_section_sub(stream, value.playback_priority, value.override_playback_priority);
            }
            else
            {
                exchange_section_sub(stream, value.midi, value.playback_priority, value.override_midi_event, value.override_midi_note_tracking, value.override_playback_priority);
            }
            exchange_section_sub<AudioCommonPropertyType>(stream, true, [&](auto &common_property)
                                                          {
                if (k_version >= 88_ui) {
                    convert_common_property_as_randomizable<AudioCommonPropertyType::playback_initial_delay>(common_property, value.playback_setting.initial_delay);
                }
                load_common_property(common_property, value.voice);
                if (k_version >= 88_ui) {
                    load_common_property(common_property, value.voice_volume_gain);
                }
                load_common_property(common_property, value.output_bus);
                load_common_property(common_property, value.auxiliary_send);
                load_common_property(common_property, value.positioning);
                if (k_version >= 88_ui) {
                    load_common_property(common_property, value.hdr);
                }
                if (k_version >= 112_ui) {
                    load_common_property(common_property, value.midi);
                }
                load_common_property(common_property, value.playback_limit);
                load_common_property(common_property, value.virtual_voice);
                load_common_property(common_property, value.playback_priority);
                if (k_version < 128_ui) {
                    load_common_property(common_property, value.motion);
                }
                if(k_version >= 112_ui && k_version < 150_ui) {
                    load_common_property(common_property, value.mixer);
                } });
            exchange_section_sub(stream, value.positioning, value.override_positioning);
            if (k_version < 135_ui)
            {
                exchange_section_sub(stream, value.auxiliary_send, value.override_game_defined_auxiliary_send, value.override_user_defined_auxiliary_send);
            }
            else
            {
                exchange_section_sub(stream, value.auxiliary_send, value.override_game_defined_auxiliary_send, value.override_user_defined_auxiliary_send, value.override_early_reflection_auxiliary_send);
            }
            exchange_section_sub(stream, value.playback_limit, value.virtual_voice, value.override_playback_limit, value.override_virtual_voice);
            if (k_version >= 88_ui)
            {
                exchange_section_sub(stream, value.voice_volume_gain, value.hdr, value.override_voice_volume_loudness_normalization, value.override_hdr_envelope_tracking);
            }
            exchange_section_sub(stream, value.state);
            exchange_section_sub(stream, value.real_time_parameter_control);
            exchange_section_sub<uint32_t>(stream, value.child);
            exchange_section_sub(stream, value.playback_setting.track);
            if (k_version >= 120_ui)
            {
                exchange_section_sub(stream, value.playback_setting.mode);
            }
            return;
        }

        //-----------------------------------

        inline static auto exchange_section(
            DataStreamView &stream,
            ActorMixer &value) -> void
        {
            exchange_section_sub(stream, value.effect, value.override_effect);
            if (k_version >= 140_ui)
            {
                exchange_section_sub(stream, value.metadata, value.override_metadata);
            }
            if (k_version >= 112_ui && k_version < 150_ui)
            {
                exchange_section_sub(stream, value.mixer, value.override_mixer);
            }
            exchange_section_sub(stream, value.output_bus);
            value.parent = stream.readUint32();
            if (k_version < 112_ui)
            {
                exchange_section_sub(stream, value.playback_priority, value.override_playback_priority);
            }
            else
            {
                exchange_section_sub(stream, value.midi, value.playback_priority, value.override_midi_event, value.override_midi_note_tracking, value.override_playback_priority);
            }
            exchange_section_sub<AudioCommonPropertyType>(stream, true, [&](auto &common_property)
                                                          {
                load_common_property(common_property, value.voice);
                if (k_version >= 88_ui) {
                    load_common_property(common_property, value.voice_volume_gain);
                }
                load_common_property(common_property, value.output_bus);
                load_common_property(common_property, value.auxiliary_send);
                load_common_property(common_property, value.positioning);
                if (k_version >= 88_ui) {
                    load_common_property(common_property, value.hdr);
                }
                if (k_version >= 112_ui) {
                    load_common_property(common_property, value.midi);
                }
                load_common_property(common_property, value.playback_limit);
                load_common_property(common_property, value.virtual_voice);
                load_common_property(common_property, value.playback_priority);
                if (k_version < 128_ui) {
                    load_common_property(common_property, value.motion);
                }
                if(k_version >= 112_ui && k_version < 150_ui) {
                    load_common_property(common_property, value.mixer);
                } });
            exchange_section_sub(stream, value.positioning, value.override_positioning);
            if (k_version < 135_ui)
            {
                exchange_section_sub(stream, value.auxiliary_send, value.override_game_defined_auxiliary_send, value.override_user_defined_auxiliary_send);
            }
            else
            {
                exchange_section_sub(stream, value.auxiliary_send, value.override_game_defined_auxiliary_send, value.override_user_defined_auxiliary_send, value.override_early_reflection_auxiliary_send);
            }
            exchange_section_sub(stream, value.playback_limit, value.virtual_voice, value.override_playback_limit, value.override_virtual_voice);
            if (k_version >= 88_ui)
            {
                exchange_section_sub(stream, value.voice_volume_gain, value.hdr, value.override_voice_volume_loudness_normalization, value.override_hdr_envelope_tracking);
            }
            exchange_section_sub(stream, value.state);
            exchange_section_sub(stream, value.real_time_parameter_control);
            exchange_section_sub<uint32_t>(stream, value.child);
            return;
        }

        //------------------------------------

        inline static auto exchange_section(
            DataStreamView &stream,
            MusicTrack &value) -> void
        {
            if (k_version >= 112_ui)
            {
                exchange_section_sub(stream, value.midi, value.override_midi_target, value.override_midi_clip_tempo);
            }
            exchange_section_sub(stream, value.source);
            exchange_section_sub(stream, value.playback_setting.clip);
            exchange_section_sub(stream, value.effect, value.override_effect);
            if (k_version >= 140)
            {
                exchange_section_sub(stream, value.metadata, value.override_metadata);
            }
            if (k_version >= 112 && k_version < 150_ui)
            {
                exchange_section_sub(stream, value.mixer, value.override_mixer);
            }
            exchange_section_sub(stream, value.output_bus);
            value.parent = stream.readUint32();
            exchange_section_sub(stream, value.playback_priority, value.override_playback_priority);
            exchange_section_sub<AudioCommonPropertyType>(stream, true, [&](auto &common_property)
                                                          {
                load_common_property(common_property, value.voice);
                if (k_version >= 88_ui) {
                    load_common_property(common_property, value.voice_volume_gain);
                }
                load_common_property(common_property, value.output_bus);
                load_common_property(common_property, value.auxiliary_send);
                load_common_property(common_property, value.positioning);
                if (k_version >= 88_ui) {
                    load_common_property(common_property, value.hdr);
                }
                if (k_version >= 112_ui) {
                    load_common_property(common_property, value.midi);
                }
                load_common_property(common_property, value.playback_limit);
                load_common_property(common_property, value.virtual_voice);
                load_common_property(common_property, value.playback_priority);
                if (k_version < 128_ui) {
                    load_common_property(common_property, value.motion);
                }
                if (k_version >= 112_ui && k_version < 150_ui) {
                    load_common_property(common_property, value.mixer);
                } });
            exchange_section_sub(stream, value.positioning, value.override_positioning);
            if (k_version < 135_ui)
            {
                exchange_section_sub(stream, value.auxiliary_send, value.override_game_defined_auxiliary_send, value.override_user_defined_auxiliary_send);
            }
            else
            {
                exchange_section_sub(stream, value.auxiliary_send, value.override_game_defined_auxiliary_send, value.override_user_defined_auxiliary_send, value.override_early_reflection_auxiliary_send);
            }
            exchange_section_sub(stream, value.playback_limit, value.virtual_voice, value.override_playback_limit, value.override_virtual_voice);
            if (k_version >= 88_ui)
            {
                exchange_section_sub(stream, value.voice_volume_gain, value.hdr, value.override_voice_volume_loudness_normalization, value.override_hdr_envelope_tracking);
            }
            exchange_section_sub(stream, value.state);
            exchange_section_sub(stream, value.real_time_parameter_control);
            if (k_version < 112_ui)
            {
                exchange_section_sub(stream, value.playback_setting.type);
            }
            else
            {
                exchange_section_sub(stream, value.playback_setting.type, value.playback_setting.switcher, value.playback_setting.transition);
            }
            exchange_section_sub(stream, value.stream);
            exchange_raw_constant(stream, 0_us);
            return;
        }

        //------------------------------------

        inline static auto exchange_section(
            DataStreamView &stream,
            MusicSegment &value) -> void
        {
            if (k_version >= 112_ui)
            {
                exchange_section_sub(stream, value.midi, value.override_midi_target, value.override_midi_clip_tempo);
            }
            exchange_section_sub(stream, value.effect, value.override_effect);
            if (k_version >= 140)
            {
                exchange_section_sub(stream, value.metadata, value.override_metadata);
            }
            if (k_version >= 112 && k_version < 150_ui)
            {
                exchange_section_sub(stream, value.mixer, value.override_mixer);
            }
            exchange_section_sub(stream, value.output_bus);
            value.parent = stream.readUint32();
            exchange_section_sub(stream, value.playback_priority, value.override_playback_priority);
            exchange_section_sub<AudioCommonPropertyType>(stream, true, [&](auto &common_property)
                                                          {
                if (k_version >= 112_ui) {
                    convert_common_property_as_regular<AudioCommonPropertyType::playback_speed>(common_property, value.playback_setting.speed);
                }
                load_common_property(common_property, value.voice);
                if (k_version >= 88_ui) {
                    load_common_property(common_property, value.voice_volume_gain);
                }
                load_common_property(common_property, value.output_bus);
                load_common_property(common_property, value.auxiliary_send);
                load_common_property(common_property, value.positioning);
                if (k_version >= 88_ui) {
                    load_common_property(common_property, value.hdr);
                }
                if (k_version >= 112_ui) {
                    load_common_property(common_property, value.midi);
                }
                load_common_property(common_property, value.playback_limit);
                load_common_property(common_property, value.virtual_voice);
                load_common_property(common_property, value.playback_priority);
                if (k_version < 128_ui) {
                    load_common_property(common_property, value.motion);
                }
                if (k_version >= 112_ui && k_version < 150_ui) {
                    load_common_property(common_property, value.mixer);
                } });
            exchange_section_sub(stream, value.positioning, value.override_positioning);
            if (k_version < 135_ui)
            {
                exchange_section_sub(stream, value.auxiliary_send, value.override_game_defined_auxiliary_send, value.override_user_defined_auxiliary_send);
            }
            else
            {
                exchange_section_sub(stream, value.auxiliary_send, value.override_game_defined_auxiliary_send, value.override_user_defined_auxiliary_send, value.override_early_reflection_auxiliary_send);
            }
            exchange_section_sub(stream, value.playback_limit, value.virtual_voice, value.override_playback_limit, value.override_virtual_voice);
            if (k_version >= 88_ui)
            {
                exchange_section_sub(stream, value.voice_volume_gain, value.hdr, value.override_voice_volume_loudness_normalization, value.override_hdr_envelope_tracking);
            }
            exchange_section_sub(stream, value.state);
            exchange_section_sub(stream, value.real_time_parameter_control);
            exchange_section_sub<uint32_t>(stream, value.child);
            exchange_section_sub(stream, value.time_setting, value.override_time_setting);
            exchange_section_sub(stream, value.stinger);
            value.playback_setting.duration = stream.readDouble();
            exchange_section_sub(stream, value.playback_setting.cue);
            return;
        }

        //------------------------------------

        inline static auto exchange_section(
            DataStreamView &stream,
            MusicPlaylistContainer &value) -> void
        {
            if (k_version >= 112_ui)
            {
                exchange_section_sub(stream, value.midi, value.override_midi_target, value.override_midi_clip_tempo);
            }
            exchange_section_sub(stream, value.effect, value.override_effect);
            if (k_version >= 140)
            {
                exchange_section_sub(stream, value.metadata, value.override_metadata);
            }
            if (k_version >= 112 && k_version < 150_ui)
            {
                exchange_section_sub(stream, value.mixer, value.override_mixer);
            }
            exchange_section_sub(stream, value.output_bus);
            value.parent = stream.readUint32();
            exchange_section_sub(stream, value.playback_priority, value.override_playback_priority);
            exchange_section_sub<AudioCommonPropertyType>(stream, true, [&](auto &common_property)
                                                          {
                if (k_version >= 112_ui) {
                    convert_common_property_as_regular<AudioCommonPropertyType::playback_speed>(common_property, value.playback_setting.speed);
                }
                load_common_property(common_property, value.voice);
                if (k_version >= 88_ui) {
                    load_common_property(common_property, value.voice_volume_gain);
                }
                load_common_property(common_property, value.output_bus);
                load_common_property(common_property, value.auxiliary_send);
                load_common_property(common_property, value.positioning);
                if (k_version >= 88_ui) {
                    load_common_property(common_property, value.hdr);
                }
                if (k_version >= 112_ui) {
                    load_common_property(common_property, value.midi);
                }
                load_common_property(common_property, value.playback_limit);
                load_common_property(common_property, value.virtual_voice);
                load_common_property(common_property, value.playback_priority);
                if (k_version < 128_ui) {
                    load_common_property(common_property, value.motion);
                }
                if (k_version >= 112_ui && k_version < 150_ui) {
                    load_common_property(common_property, value.mixer);
                } });
            exchange_section_sub(stream, value.positioning, value.override_positioning);
            if (k_version < 135_ui)
            {
                exchange_section_sub(stream, value.auxiliary_send, value.override_game_defined_auxiliary_send, value.override_user_defined_auxiliary_send);
            }
            else
            {
                exchange_section_sub(stream, value.auxiliary_send, value.override_game_defined_auxiliary_send, value.override_user_defined_auxiliary_send, value.override_early_reflection_auxiliary_send);
            }
            exchange_section_sub(stream, value.playback_limit, value.virtual_voice, value.override_playback_limit, value.override_virtual_voice);
            if (k_version >= 88_ui)
            {
                exchange_section_sub(stream, value.voice_volume_gain, value.hdr, value.override_voice_volume_loudness_normalization, value.override_hdr_envelope_tracking);
            }
            exchange_section_sub(stream, value.state);
            exchange_section_sub(stream, value.real_time_parameter_control);
            exchange_section_sub<uint32_t>(stream, value.child);
            exchange_section_sub(stream, value.time_setting, value.override_time_setting);
            exchange_section_sub(stream, value.stinger);
            exchange_section_sub(stream, value.transition);
            exchange_section_sub(stream, value.playback_setting.playlist);
            return;
        }

        //------------------------------------

        inline static auto exchange_section(
            DataStreamView &stream,
            MusicSwitchContainer &value) -> void
        {
            if (k_version >= 112_ui)
            {
                exchange_section_sub(stream, value.midi, value.override_midi_target, value.override_midi_clip_tempo);
            }
            exchange_section_sub(stream, value.effect, value.override_effect);
            if (k_version >= 140)
            {
                exchange_section_sub(stream, value.metadata, value.override_metadata);
            }
            if (k_version >= 112 && k_version < 150_ui)
            {
                exchange_section_sub(stream, value.mixer, value.override_mixer);
            }
            exchange_section_sub(stream, value.output_bus);
            value.parent = stream.readUint32();
            exchange_section_sub(stream, value.playback_priority, value.override_playback_priority);
            exchange_section_sub<AudioCommonPropertyType>(stream, true, [&](auto &common_property)
                                                          {
                if (k_version >= 112_ui) {
                    convert_common_property_as_regular<AudioCommonPropertyType::playback_speed>(common_property, value.playback_setting.speed);
                }
                load_common_property(common_property, value.voice);
                if (k_version >= 88_ui) {
                    load_common_property(common_property, value.voice_volume_gain);
                }
                load_common_property(common_property, value.output_bus);
                load_common_property(common_property, value.auxiliary_send);
                load_common_property(common_property, value.positioning);
                if (k_version >= 88_ui) {
                    load_common_property(common_property, value.hdr);
                }
                if (k_version >= 112_ui) {
                    load_common_property(common_property, value.midi);
                }
                load_common_property(common_property, value.playback_limit);
                load_common_property(common_property, value.virtual_voice);
                load_common_property(common_property, value.playback_priority);
                if (k_version < 128_ui) {
                    load_common_property(common_property, value.motion);
                }
                if (k_version >= 112_ui && k_version < 150_ui) {
                    load_common_property(common_property, value.mixer);
                } });
            exchange_section_sub(stream, value.positioning, value.override_positioning);
            if (k_version < 135_ui)
            {
                exchange_section_sub(stream, value.auxiliary_send, value.override_game_defined_auxiliary_send, value.override_user_defined_auxiliary_send);
            }
            else
            {
                exchange_section_sub(stream, value.auxiliary_send, value.override_game_defined_auxiliary_send, value.override_user_defined_auxiliary_send, value.override_early_reflection_auxiliary_send);
            }
            exchange_section_sub(stream, value.playback_limit, value.virtual_voice, value.override_playback_limit, value.override_virtual_voice);
            if (k_version >= 88_ui)
            {
                exchange_section_sub(stream, value.voice_volume_gain, value.hdr, value.override_voice_volume_loudness_normalization, value.override_hdr_envelope_tracking);
            }
            exchange_section_sub(stream, value.state);
            exchange_section_sub(stream, value.real_time_parameter_control);
            exchange_section_sub<uint32_t>(stream, value.child);
            exchange_section_sub(stream, value.time_setting, value.override_time_setting);
            exchange_section_sub(stream, value.stinger);
            exchange_section_sub(stream, value.transition);
            if (k_version < 88_ui)
            {
                exchange_section_sub(stream, value.playback_setting.switcher);
            }
            exchange_bit_multi<uint8_t>(stream, value.playback_setting.continue_playing_on_switch_change);
            exchange_section_sub(stream, value.playback_setting.association);
            return;
        }

        //-----------------------------------

        inline static auto exchange_hierarchy(
            DataStreamView &stream,
            std::vector<Hierarchy> &value) -> void
        {
            exchange_list(
                stream, value, [](auto &data, auto &value)
                {
                    exchange_bit_multi<uint8_t>(data, value.type);
                    auto next_item_pos = data.read_pos + static_cast<size_t>(data.readUint32());
                    value.id = data.readUint32();
                    switch (value.type) {
                        case HierarchyType::stateful_property_setting: {
                            exchange_section(data, value.stateful_property_setting);
                            break;
                        }
                        case HierarchyType::event_action: {
                            exchange_section(data, value.event_action);
                            break;
                        }
                        case HierarchyType::event: {
                            exchange_section(data, value.event);
                            break;
                        }
                        case HierarchyType::dialogue_event: {
                            exchange_section(data, value.dialogue_event);
                            break;
                        }
                        case HierarchyType::attenuation: {
                            exchange_section(data, value.attenuation);
                            break;
                        }
                        case HierarchyType::low_frequency_oscillator_modulator: {
                            exchange_section(data, value.low_frequency_oscillator_modulator);
                            break;
                        }
                        case HierarchyType::envelope_modulator: {
                            exchange_section(data, value.envelope_modulator);
                            break;
                        }
                        case HierarchyType::time_modulator: {
                            exchange_section(data, value.time_modulator);
                            break;
                        }
                        case HierarchyType::effect: {
                            exchange_section(data, value.effect);
                            break;
                        }
                        case HierarchyType::source: {
                            exchange_section(data, value.source);
                            break;
                        }
                        case HierarchyType::audio_device: {
                            exchange_section(data, value.audio_device);
                            break;
                        }
                        case HierarchyType::audio_bus: {
                            exchange_section(data, value.audio_bus);
                            break;
                        }
                        case HierarchyType::auxiliary_audio_bus: {
                            exchange_section(data, value.auxiliary_audio_bus);
                            break;
                        }
                        case HierarchyType::sound: {
                            exchange_section(data, value.sound);
                            break;
                        }
                        case HierarchyType::sound_playlist_container: {
                            exchange_section(data, value.sound_playlist_container);
                            break;
                        }
                        case HierarchyType::sound_switch_container: {
                            exchange_section(data, value.sound_switch_container);
                            break;
                        }
                        case HierarchyType::sound_blend_container: {
                            exchange_section(data, value.sound_blend_container);
                            break;
                        }
                        case HierarchyType::actor_mixer: {
                            exchange_section(data, value.actor_mixer);
                            break;
                        }
                        case HierarchyType::music_track: {
                            exchange_section(data, value.music_track);
                            break;
                        }
                        case HierarchyType::music_segment: {
                            exchange_section(data, value.music_segment);
                            break;
                        }
                        case HierarchyType::music_playlist_container: {
                            exchange_section(data, value.music_playlist_container);
                            break;
                        }
                        case HierarchyType::music_switch_container: {
                            exchange_section(data, value.music_switch_container);
                            break;
                        }
                        default: {
                            auto pos = std::string_view{std::to_string(data.read_pos)};
                            assert_conditional(false, String::format(fmt::format("{}", Language::get("wwise.soundbank.decode.invalid_hierarchy_type")), magic_enum::enum_name(value.type), pos), "exchange_hierarchy");
                        }
                    }
                    assert_conditional(data.read_pos == next_item_pos, fmt::format("invalid_pos. current: {} | next : {}", data.read_pos, next_item_pos), "exchange_hierarchy"); }, stream.readUint32());
            return;
        }

        inline static auto exchange_section_sub(
            DataStreamView &stream,
            ObstructionSetting &value) -> void
        {
            exchange_bit_multi<uint8_t>(stream, value.enable);
            exchange_bit_multi<uint8_t>(stream, value.mode);
            exchange_list(stream, value.point, [](auto &data, auto &value)
                          {
                value.position.x = data.readFloat();
                value.position.y = data.readFloat();
                exchange_bit_multi<uint32_t>(data, value.curve); }, stream.readUint16());
            return;
        }

        inline static auto exchange_environments_settings(
            DataStreamView &stream,
            Setting &value) -> void
        {
            exchange_section_sub(stream, value.obstruction.volume);
            exchange_section_sub(stream, value.obstruction.low_pass_filter);
            if (k_version >= 112_ui)
            {
                exchange_section_sub(stream, value.obstruction.high_pass_filter);
            }
            exchange_section_sub(stream, value.occlusion.volume);
            exchange_section_sub(stream, value.occlusion.low_pass_filter);
            if (k_version >= 112_ui)
            {
                exchange_section_sub(stream, value.occlusion.high_pass_filter);
            }
            return;
        }

        inline static auto exchange_string_mapping(
            DataStreamView &stream,
            std::vector<SoundBankReference> &value) -> void
        {
            exchange_raw_constant(stream, 1_ui);
            exchange_list(stream, value, [](auto &data, auto &value)
                          {
                value.id = data.readUint32();
                value.name = data.readStringByUint8(); }, stream.readUint32());
            return;
        }

        inline static auto exchange_platform_settings(
            DataStreamView &stream,
            Setting &value) -> void
        {
            if (k_version >= 113_ui)
            {
                if (k_version < 118_ui)
                {
                    value.platform = stream.readStringByUint32();
                }
                else if (k_version < 140_ui)
                {
                    auto length = stream.readUint32();
                    assert_conditional(length - 1_ui > k_none_size, fmt::format("{}", Language::get("wwise.sound_bank.invalid_size")), "exchange_platform_settings");
                    value.platform = stream.readString(length - 1_ui);
                    exchange_raw_constant(stream, 0_byte); // skip last empty byte.
                }
                else
                {
                    value.platform = stream.readStringByEmpty();
                }
            }
            return;
        }

        inline static auto exchange_sound_bank(
            DataStreamView &stream,
            SoundBankInformation &definition,
            std::string_view destination) -> void
        {
            auto bank_header_chunk_sign_flag = stream.readString(4_size);
            assert_conditional(bank_header_chunk_sign_flag == ChunkSignFlag::bkhd, fmt::format("{}", Kernel::Language::get("wwise.soundbank.decode.invalid_bnk_magic")), "process_whole");
            stream.read_pos = k_begin_index;
            while (stream.read_pos < stream.size())
            {
                auto sign = ChunkSign{};
                exchange_chuck_sign(stream, sign);
                if (sign.id == ChunkSignFlag::bkhd)
                {
                    exchange_bank_header(stream, definition.bank_header, sign.size);
                }
                else if (sign.id == ChunkSignFlag::didx)
                {
                    exchange_media(stream, definition.media, sign.size, destination);
                    definition.has_media = true;
                }
                else if (sign.id == ChunkSignFlag::init && k_version >= 118_ui)
                {
                    exchange_plugin_chunk(stream, definition.setting);
                    definition.has_setting = true;
                }
                else if (sign.id == ChunkSignFlag::stmg)
                {
                    exchange_game_synchronization(stream, definition.setting, definition.game_synchronization);
                    definition.has_game_synchronization = true;
                }
                else if (sign.id == ChunkSignFlag::hirc)
                {
                    exchange_hierarchy(stream, definition.hierarchy);
                    definition.has_hierarchy = true;
                }
                else if (sign.id == ChunkSignFlag::stid)
                {
                    exchange_string_mapping(stream, definition.reference);
                    definition.has_reference = true;
                }
                else if (sign.id == ChunkSignFlag::envs)
                {
                    exchange_environments_settings(stream, definition.setting);
                    definition.has_setting = true;
                }
                else if (sign.id == ChunkSignFlag::plat && k_version >= 113_ui)
                {
                    exchange_platform_settings(stream, definition.setting);
                    definition.has_setting = true;
                }
                else
                {
                    assert_conditional(false, fmt::format("{} | {}: {:02x}", Kernel::Language::get("wwise.decode.invalid_bnk"), Kernel::Language::get("offset"), stream.read_pos), "exchange_sound_bank");
                }
            }
            assert_conditional(stream.read_pos == stream.size(), fmt::format("{}", Language::get("wwise.sound_bank.stream_is_not_full")), "exchange_sound_bank");
            return;
        }

    public:
        inline static auto process_whole(
            DataStreamView &stream,
            SoundBankInformation &definition,
            std::string_view destination) -> void
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
            write_json(fmt::format("{}/{}", destination, "data.json"), definition);
            return;
        }
    };

}