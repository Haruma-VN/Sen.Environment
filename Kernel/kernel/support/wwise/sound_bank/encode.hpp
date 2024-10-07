#pragma once

#include "kernel/definition/utility.hpp"
#include "kernel/support/miscellaneous/shared.hpp"
#include "kernel/support/wwise/sound_bank/definition.hpp"
#include "kernel/support/wwise/sound_bank/common.hpp"

namespace Sen::Kernel::Support::WWise::SoundBank
{
    using namespace Definition;

    using namespace Sen::Kernel::Support::Miscellaneous::Shared;

    template <typename Value>
    concept IsValidConstArgument = (std::is_same<Value, bool>::value || std::is_enum<Value>::value);

    struct Encode : Common
    {
    protected:
        inline static auto exchange_data(
            DataStreamView &stream,
            std::string const &value) -> void
        {
            auto hexString = value;
            hexString.erase(std::remove(hexString.begin(), hexString.end(), ' '), hexString.end());
            auto bytes = std::vector<uint8_t>{};
            for (auto i = static_cast<unsigned int>(0); i < hexString.length(); i += 2)
            {
                auto byteString = hexString.substr(i, 2);
                auto byte = static_cast<std::uint8_t>(std::strtol(byteString.data(), NULL, 16));
                bytes.push_back(byte);
            }
            stream.writeUint32(static_cast<uint32_t>(bytes.size()));
            stream.writeBytes(bytes);
            return;
        }

        inline static auto chuck_sign_set(
            DataStreamView &stream,
            ChunkSign &value) -> void
        {
            stream.writeString(value.id);
            value.pos = stream.write_pos;
            stream.writeNull(4_size);
            return;
        }

        inline static auto chuck_sign_write(
            DataStreamView &stream,
            ChunkSign const &value) -> void
        {
            auto current_pos = stream.write_pos;
            auto chuck_size = current_pos - static_cast<size_t>(value.pos) - k_block_size;
            stream.writeUint32(static_cast<uint32_t>(chuck_size), static_cast<size_t>(value.pos));
            stream.write_pos = current_pos;
            return;
        }

        //------------------------------------

        template <typename RawValue>
            requires std::is_arithmetic<RawValue>::value
        inline static auto exchange_raw_constant(
            DataStreamView &stream,
            RawValue const &value) -> void
        {
            stream.write_of<RawValue>(value);
            return;
        }

        //------------------------------------

        template <auto flag_count, typename ArgsValue, typename... Args>
            requires((IsValidConstArgument<Args> && ...) && IsValidConstArgument<ArgsValue>)
        inline static auto exchange_bit(
            size_t current_index,
            std::bitset<flag_count> &flag,
            ArgsValue const &value,
            Args const &...args) -> void
        {
            if constexpr (std::is_same<ArgsValue, bool>::value)
            {
                flag.set(current_index, value);
                ++current_index;
            }
            else if constexpr (std::is_enum<ArgsValue>::value)
            {
                auto index_value = exchange_enumeration<ArgsValue, uint8_t>(value);
                for (auto bit_index : Range(exchange_enumeration<ArgsValue, size_t>(value)))
                {
                    flag.set(current_index, static_cast<bool>(clip_bit(index_value, bit_index, 1_size)));
                    ++current_index;
                }
            }
            if constexpr (sizeof...(args) > 0_size)
            {
                exchange_bit(current_index, flag, args...);
            }
            return;
        }

        template <typename RawValue, typename... Args>
            requires(IsValidConstArgument<Args> && ...)
        inline static auto exchange_bit_multi(
            DataStreamView &stream,
            Args const &...args) -> void
        {
            auto bitset = std::bitset<static_cast<size_t>(sizeof(RawValue) * 8_size)>{};
            exchange_bit(k_begin_index, bitset, args...);
            stream.write_of<RawValue>(static_cast<RawValue>(bitset.to_ullong()));
            return;
        }

        //--------------------------------

        template <auto type, typename Value>
            requires(std::is_same<Value, bool>::value || std::is_enum<Value>::value || std::is_arithmetic<Value>::value) && std::is_enum<decltype(type)>::value
        inline static auto convert_common_property(
            CommonPropertyMap<decltype(type)> &map,
            Value const &value) -> void
        {
            auto default_value = declare_common_property_type<decltype(type)>(type);
            if constexpr (std::is_enum<Value>::value)
            {
                auto index = exchange_enumeration<Value, uint8_t>(value);
                if (static_cast<uint32_t>(index) != std::any_cast<uint32_t>(default_value))
                {
                    map.regular.emplace_back(std::pair<decltype(type), std::any>(type, std::make_any<uint32_t>(index)));
                }
            }
            else
            {
                if (value != std::any_cast<Value>(default_value))
                {
                    map.regular.emplace_back(std::pair<decltype(type), std::any>(type, std::make_any<Value>(value)));
                }
            }
            return;
        }

        //------------------------------------

        template <auto type, typename Value>
            requires(std::is_same<Value, bool>::value || std::is_enum<Value>::value || std::is_arithmetic<Value>::value) && std::is_enum<decltype(type)>::value
        inline static auto convert_common_property_as_regular(
            CommonPropertyMap<decltype(type)> &map,
            Value const &value) -> void
        {
            if (value != std::any_cast<Value>(declare_common_property_type<decltype(type)>(type)))
            {
                map.regular.emplace_back(std::pair<decltype(type), std::any>(type, std::make_any<Value>(value)));
            }
            return;
        }

        //------------------------------------

        template <auto type, typename Value>
            requires(std::is_same<Value, bool>::value || std::is_enum<Value>::value || std::is_arithmetic<Value>::value) && std::is_enum<decltype(type)>::value
        inline static auto convert_common_property_as_randomizable(
            CommonPropertyMap<decltype(type)> &map,
            RandomizableValue<Value> const &value) -> void
        {
            if (value.value != std::any_cast<Value>(declare_common_property_type<decltype(type)>(type)))
            {
                map.regular.emplace_back(std::pair<decltype(type), std::any>(type, std::make_any<Value>(value.value)));
            }
            if (value.minimum_value != static_cast<Value>(0) || value.maximum_value != static_cast<Value>(0))
            {
                map.randomizer.emplace_back(std::pair<decltype(type), std::array<std::any, 2>>(type, std::array<std::any, 2>{std::make_any<Value>(value.minimum_value), std::make_any<Value>(value.maximum_value)}));
            }
            return;
        }

        //------------------------------------
        inline static auto exchange_common_property_value(
            DataStreamView &stream,
            std::any const &value) -> void
        {
            if (value.type() == typeid(bool))
            {
                exchange_bit_multi<uint32_t>(stream, std::any_cast<bool>(value));
            }
            if (value.type() == typeid(uint8_t))
            {
                stream.writeUint32(std::any_cast<uint32_t>(value));
            }
            if (value.type() == typeid(float))
            {
                stream.writeFloat(std::any_cast<float>(value));
            }
            if (value.type() == typeid(double))
            {
                stream.writeDouble(std::any_cast<double>(value));
            }
            if (value.type() == typeid(uint32_t))
            {
                stream.writeUint32(std::any_cast<uint32_t>(value));
            }
            return;
        }

        //------------------------------------

        inline static auto load_common_property(
            CommonPropertyMap<AudioCommonPropertyType> &map,
            AudioOutputBusSetting const &value) -> void
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
            CommonPropertyMap<AudioCommonPropertyType> &map,
            AudioVoice const &value) -> void
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
            CommonPropertyMap<AudioCommonPropertyType> &map,
            AudioVoiceVolumeGainSetting const &value) -> void
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
            CommonPropertyMap<AudioCommonPropertyType> &map,
            AudioPositioningSetting const &value) -> void
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
            CommonPropertyMap<AudioCommonPropertyType> &map,
            AudioHDRSetting const &value) -> void
        {
            if (k_version >= 88_ui)
            {
                convert_common_property<AudioCommonPropertyType::hdr_envelope_tracking_active_range>(map, value.envelope_tracking.active_range);
            }
            return;
        }

        //------------------------------------

        inline static auto load_common_property(
            CommonPropertyMap<AudioCommonPropertyType> &map,
            BusVoiceVolumeGainSetting const &value) -> void
        {
            if (k_version >= 125_ui)
            {
                convert_common_property<AudioCommonPropertyType::voice_volume_make_up_gain>(map, value.make_up);
            }
            return;
        }

        //------------------------------------

        inline static auto load_common_property(
            CommonPropertyMap<AudioCommonPropertyType> &map,
            MusicMIDISetting const &value) -> void
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
            CommonPropertyMap<AudioCommonPropertyType> &map,
            AudioPlaybackPrioritySetting const &value) -> void
        {
            convert_common_property_as_regular<AudioCommonPropertyType::playback_priority_value>(map, value.value);
            convert_common_property_as_regular<AudioCommonPropertyType::playback_priority_offset_at_maximum_distance>(map, value.offset_at_maximum_distance);
            return;
        }

        //------------------------------------

        inline static auto load_common_property(
            CommonPropertyMap<AudioCommonPropertyType> &map,
            AudioMotionSetting const &value) -> void
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
            CommonPropertyMap<AudioCommonPropertyType> &map,
            AudioMixerSetting const &value) -> void
        {
            if (k_version >= 112 && k_version < 150_ui)
            {
                convert_common_property<AudioCommonPropertyType::mixer_id>(map, value.id);
            }
            return;
        }

        //------------------------------------

        inline static auto load_common_property(
            CommonPropertyMap<AudioCommonPropertyType> &map,
            AudioPlaybackLimitSetting const &value) -> void
        {
            return;
        }

        //------------------------------------

        inline static auto load_common_property(
            CommonPropertyMap<AudioCommonPropertyType> &map,
            AudioVirtualVoiceSetting const &value) -> void
        {
            return;
        }

        //------------------------------------

        inline static auto load_common_property(
            CommonPropertyMap<AudioCommonPropertyType> &map,
            AudioAuxiliarySendSetting const &value) -> void
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
            CommonPropertyMap<AudioCommonPropertyType> &map,
            BusHDRSetting const &value) -> void
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
            CommonPropertyMap<AudioCommonPropertyType> &map,
            SoundMIDISetting const &value) -> void
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
            CommonPropertyMap<AudioCommonPropertyType> &map,
            BusVoiceSetting const &value) -> void
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
            CommonPropertyMap<AudioCommonPropertyType> &map,
            BusBusSetting const &value) -> void
        {
            convert_common_property_as_regular<AudioCommonPropertyType::bus_volume>(map, value.volume);
            return;
        }

        //------------------------------------

        inline static auto load_common_property(
            CommonPropertyMap<AudioCommonPropertyType> &map,
            BusOutputBusSetting const &value) -> void
        {
            if (k_version >= 128_ui)
            {
                convert_common_property_as_regular<AudioCommonPropertyType::output_bus_volume>(map, value.volume);
                convert_common_property_as_regular<AudioCommonPropertyType::output_bus_low_pass_filter>(map, value.low_pass_filter);
                convert_common_property_as_regular<AudioCommonPropertyType::output_bus_high_pass_filter>(map, value.high_pass_filter);
            }
            return;
        }

        //-------------------------------

        //------------------------------------

        template <typename Type, typename Parser>
            requires std::is_enum<Type>::value && (!std::is_void<Parser>::value && (!std::is_reference<Parser>::value) && std::is_same_v<Parser, std::remove_cvref_t<Parser>>)
        inline static auto exchange_section_sub(
            DataStreamView &stream,
            bool const &randomizable,
            Parser const &parser) -> void
        {
            auto map = CommonPropertyMap<Type>{};
            parser(map);
            exchange_list<true, uint8_t>(stream, map.regular, [](auto &data, auto &element)
                                         { data.writeUint8(exchange_enumeration<Type, uint8_t>(element.first)); });
            exchange_list<false, uint8_t>(stream, map.regular, [](auto &data, auto &element)
                                          { exchange_common_property_value(data, element.second); });
            if (randomizable) {
                exchange_list<true, uint8_t>(stream, map.randomizer, [](auto &data, auto &element)
                                         { data.writeUint8(exchange_enumeration<Type, uint8_t>(element.first)); });
                exchange_list<false, uint8_t>(stream, map.randomizer, [](auto &data, auto &element)
                                          { exchange_common_property_value(data, element.second[0]);
                                            exchange_common_property_value(data, element.second[1]); });
            }
            return;
        }

        //------------------------------------

        template <typename RawValue>
            requires std::is_integral<RawValue>::value
        inline static auto exchange_section_sub(
            DataStreamView &stream,
            std::vector<uint32_t> const &value_list) -> void
        {
            exchange_list<true, RawValue>(
                stream, value_list, [](auto &data, auto &value)
                { data.writeUint32(value); });
            return;
        }

        //------------------------------------

        inline static auto exchange_section_sub(
            DataStreamView &stream,
            MusicMIDISetting const &midi_value,
            bool const &midi_target_override,
            bool const &midi_clip_tempo_override) -> void
        {
            if (k_version >= 112_ui)
            {
                exchange_bit_multi<uint8_t>(stream, k_bitset_boolean_fill, midi_target_override, midi_clip_tempo_override);
            }
            return;
        }

        //------------------------------------

        inline static auto exchange_section_sub(
            DataStreamView &stream,
            AudioSourceSetting const &value) -> void
        {
            stream.writeUint32(value.plug_in);
            if (k_version < 112_ui)
            {
                exchange_bit_multi<uint32_t>(stream, value.type);
            }
            else
            {
                exchange_bit_multi<uint8_t>(stream, value.type);
            }
            stream.writeUint32(value.resource);
            if (k_version < 113_ui)
            {
                stream.writeUint32(value.source);
                if (value.type != AudioSourceType::streamed)
                {
                    stream.writeUint32(value.resource_offset);
                }
            }
            if (k_version < 112_ui)
            {
                if (value.type != AudioSourceType::streamed)
                {
                    stream.writeUint32(value.resource_size);
                }
            }
            else
            {
                stream.writeUint32(value.resource_size);
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
            std::vector<AudioSourceSetting> const &value) -> void
        {
            exchange_list<true, uint32_t>(stream, value, [](auto &data, auto &value)
                                          { exchange_section_sub(data, value); });
            return;
        }

        //------------------------------------

        inline static auto exchange_section_sub(
            DataStreamView &stream,
            MusicTrackClip const &clip_value) -> void
        {
            exchange_list<true, uint32_t>(stream, clip_value.item, [](auto &data, auto &value)
                                          {
				data.writeUint32(value.u1);
				data.writeUint32(value.source);
                if (k_version >= 140) {
					data.writeUint32(value.event);
                }
				data.writeDouble(value.offset);
				data.writeDouble(value.begin);
				data.writeDouble(value.end);
				data.writeDouble(value.duration); });
            if (!clip_value.item.empty())
            {
                stream.writeUint32(clip_value.u1);
            }
            exchange_list<true, uint32_t>(stream, clip_value.curve, [](auto &data, auto &value)
                                          {
				data.writeUint32(value.index);
                exchange_bit_multi<uint32_t>(data, value.type);
                exchange_list<true, uint32_t>(data, value.point, [] (auto &data, auto &value) {
                    data.writeFloat(value.position.x);
                    data.writeFloat(value.position.y);
                    exchange_bit_multi<uint32_t>(data, value.curve);
                }); });
            return;
        }

        //------------------------------------

        inline static auto exchange_section_sub(
            DataStreamView &stream,
            AudioEffectSetting const &effect_value) -> void
        {
            auto item_size = effect_value.item.size();
            stream.writeUint8(static_cast<uint8_t>(item_size));
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
            exchange_list<false, uint8_t>(stream, effect_value.item, [](auto &data, auto &value)
                                          {
                            data.writeUint8(value.index);
                            data.writeUint32(value.id);
                if (k_version < 150_ui) {
                    exchange_bit_multi<uint8_t>(data, value.use_share_set);
                    exchange_bit_multi<uint8_t>(data, value.u1);
                }
                else {
                    exchange_bit_multi<uint8_t>(data, value.bypass, value.use_share_set);
                } });
            return;
        }

        //------------------------------------

        inline static auto exchange_section_sub(
            DataStreamView &stream,
            AudioEffectSetting const &effect_value,
            bool const &effect_override) -> void
        {
            exchange_bit_multi<uint8_t>(stream, effect_override);
            exchange_section_sub(stream, effect_value);
            return;
        }

        //------------------------------------

        inline static auto exchange_section_sub(
            DataStreamView &stream,
            AudioMetadataSetting const &metadata_value) -> void
        {
            if (k_version >= 140_ui)
            {
                exchange_list<true, uint8_t>(stream, metadata_value.item, [](auto &data, auto &value)
                                             {
                                data.writeUint8(value.index);
                                data.writeUint8(value.id);
                    exchange_bit_multi<uint8_t>(data, value.use_share_set); });
            }
            return;
        }

        //------------------------------------

        inline static auto exchange_section_sub(
            DataStreamView &stream,
            AudioMetadataSetting const &metadata_value,
            bool const &metadata_override) -> void
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
            AudioMixerSetting const &mixer_value,
            bool const &mixer_override) -> void
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
            AudioOutputBusSetting const &output_bus_value) -> void
        {
            stream.writeUint32(output_bus_value.bus);
            return;
        }

        //------------------------------------

        inline static auto exchange_section_sub(
            DataStreamView &stream,
            SoundMIDISetting const &midi_value,
            AudioPlaybackPrioritySetting const &playback_priority_value,
            bool const &midi_event_override,
            bool const &midi_note_tracking_override,
            bool const &playback_priority_override) -> void
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
            AudioPlaybackPrioritySetting const &playback_priority_value,
            bool const &playback_priority_override) -> void
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
            AudioPositioningSetting const &positioning_value,
            bool const &positioning_override) -> void
        {
            if (k_version < 112_ui)
            {
                exchange_bit_multi<uint8_t>(stream, positioning_override);
                if (positioning_override)
                {
                    auto b1 = false;
                    auto b2 = false;
                    auto b3 = false;
                    if (positioning_value.type == AudioPositioningSettingType::two_dimension)
                    {
                        if (k_version < 88_ui)
                        {
                            b2 = false;
                        }
                        else if (k_version < 112_ui)
                        {
                            b3 = true;
                        }
                        b1 = positioning_value.speaker_panning.enable;
                    }
                    if (positioning_value.type == AudioPositioningSettingType::three_dimension)
                    {
                        if (k_version < 88_ui)
                        {
                            b2 = true;
                        }
                        else if (k_version < 112_ui)
                        {
                            b3 = false;
                        }
                        b1 = positioning_value.listener_routing.position_source.mode == AudioPositioningSettingListenerRoutingPositionSourceMode::game_defined;
                    }

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
                    }
                    if (positioning_value.type == AudioPositioningSettingType::three_dimension)
                    {
                        exchange_raw_constant(stream, 0_byte);
                        exchange_raw_constant(stream, 0_byte);
                        exchange_raw_constant(stream, 0_byte);
                        stream.writeUint32(positioning_value.listener_routing.attenuation.id);
                        exchange_bit_multi<uint8_t>(stream, positioning_value.listener_routing.spatialization);
                        if (positioning_value.listener_routing.position_source.mode == AudioPositioningSettingListenerRoutingPositionSourceMode::game_defined)
                        {
                            exchange_bit_multi<uint8_t>(stream, positioning_value.listener_routing.position_source.update_at_each_frame);
                        }
                        if (positioning_value.listener_routing.position_source.mode == AudioPositioningSettingListenerRoutingPositionSourceMode::user_defined)
                        {
                            exchange_bit_multi<uint8_t>(
                                stream,
                                positioning_value.listener_routing.position_source.automation.play_type,
                                positioning_value.listener_routing.position_source.automation.play_mode,
                                positioning_value.listener_routing.position_source.automation.pick_new_path_when_sound_start);
                            exchange_raw_constant(stream, 0_byte);
                            exchange_raw_constant(stream, 0_byte);
                            exchange_raw_constant(stream, 0_byte);
                            exchange_bit_multi<uint8_t>(stream, positioning_value.listener_routing.position_source.automation.loop);
                            stream.writeUint32(positioning_value.listener_routing.position_source.automation.transition_time);
                            exchange_bit_multi<uint8_t>(stream, positioning_value.listener_routing.position_source.hold_listener_orientation);
                            exchange_list<true, uint32_t>(
                                stream,
                                positioning_value.listener_routing.position_source.automation.point,
                                [](auto &data, auto &value)
                                {
                                    data.writeFloat(value.position.x);
                                    exchange_raw_constant(data, 0_ui);
                                    data.writeFloat(value.position.y);
                                    data.writeUint32(value.duration);
                                });
                            exchange_list<true, uint32_t>(
                                stream,
                                positioning_value.listener_routing.position_source.automation.path,
                                [](auto &data, auto &value)
                                {
                                    data.writeUint32(value.point.begin);
                                    data.writeUint32(value.point.count);
                                });
                            exchange_list<false, uint32_t>(
                                stream,
                                positioning_value.listener_routing.position_source.automation.path,
                                [](auto &data, auto &value)
                                {
                                    data.writeFloat(value.random_range.left_right);
                                    data.writeFloat(value.random_range.front_back);
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
                else
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
                    stream.writeUint32(positioning_value.listener_routing.attenuation.id);
                    if (positioning_value.listener_routing.position_source.mode == AudioPositioningSettingListenerRoutingPositionSourceMode::user_defined)
                    {
                        exchange_bit_multi<uint8_t>(stream, positioning_value.listener_routing.position_source.automation.play_type,
                                                    positioning_value.listener_routing.position_source.automation.play_mode,
                                                    positioning_value.listener_routing.position_source.automation.pick_new_path_when_sound_start);
                        stream.writeUint32(positioning_value.listener_routing.position_source.automation.transition_time);
                        exchange_list<true, uint32_t>(stream, positioning_value.listener_routing.position_source.automation.point, [](auto &data, auto &value)
                                                      {
                                    data.writeFloat(value.position.x);
                                    data.writeFloat(value.position.z);
                                    data.writeFloat(value.position.y);
                                    data.writeUint32(value.duration); });
                        exchange_list<true, uint32_t>(stream, positioning_value.listener_routing.position_source.automation.path, [](auto &data, auto &value)
                                                      {
                                        data.writeUint32(value.point.begin);
                                        data.writeUint32(value.point.count); });
                        exchange_list<false, uint32_t>(stream, positioning_value.listener_routing.position_source.automation.path, [](auto &data, auto &value)
                                                       {
                                        data.writeFloat(value.random_range.left_right);
                                        data.writeFloat(value.random_range.front_back);
                                        data.writeFloat(value.random_range.up_down); });
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
                        stream.writeUint32(positioning_value.listener_routing.position_source.automation.transition_time);
                        exchange_list<true, uint32_t>(stream, positioning_value.listener_routing.position_source.automation.point, [](auto &data, auto &value)
                                                      {
                                        data.writeFloat(value.position.x);
                                        data.writeFloat(value.position.z);
                                        data.writeFloat(value.position.y);
                                        data.writeUint32(value.duration); });
                        exchange_list<true, uint32_t>(stream, positioning_value.listener_routing.position_source.automation.path, [](auto &data, auto &value)
                                                      {
                                        data.writeUint32(value.point.begin);
                                        data.writeUint32(value.point.count); });
                        exchange_list<false, uint32_t>(stream, positioning_value.listener_routing.position_source.automation.path, [](auto &data, auto &value)
                                                       {
                                        data.writeFloat(value.random_range.left_right);
                                        data.writeFloat(value.random_range.front_back);
                                        data.writeFloat(value.random_range.up_down); });
                    }
                }
            }
            return;
        }

        //------------------------------------

        inline static auto exchange_section_sub(
            DataStreamView &stream,
            AudioAuxiliarySendSetting const &auxiliary_send_value,
            bool const &game_defined_auxiliary_send_override,
            bool const &user_defined_auxiliary_send_override) -> void
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
                    stream.writeUint32(auxiliary_send_value.user_defined.item_1.bus);
                    stream.writeUint32(auxiliary_send_value.user_defined.item_2.bus);
                    stream.writeUint32(auxiliary_send_value.user_defined.item_3.bus);
                    stream.writeUint32(auxiliary_send_value.user_defined.item_4.bus);
                }
            }
            return;
        }

        //------------------------------------

        inline static auto exchange_section_sub(
            DataStreamView &stream,
            AudioAuxiliarySendSetting const &auxiliary_send_value,
            bool const &game_defined_auxiliary_send_override,
            bool const &user_defined_auxiliary_send_override,
            bool const &early_reflection_auxiliary_send_override) -> void
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
                    stream.writeUint32(auxiliary_send_value.user_defined.item_1.bus);
                    stream.writeUint32(auxiliary_send_value.user_defined.item_2.bus);
                    stream.writeUint32(auxiliary_send_value.user_defined.item_3.bus);
                    stream.writeUint32(auxiliary_send_value.user_defined.item_4.bus);
                }
                stream.writeUint32(auxiliary_send_value.early_reflection.bus);
            }
            return;
        }

        //------------------------------------

        inline static auto exchange_section_sub(
            DataStreamView &stream,
            AudioPlaybackLimitSetting const &playback_limit_value,
            bool const &playback_limit_override) -> void
        {
            if (k_version < 112_ui)
            {
                exchange_bit_multi<uint8_t>(stream, playback_limit_value.when_priority_is_equal);
                exchange_bit_multi<uint8_t>(stream, playback_limit_value.when_limit_is_reached);
                stream.writeUint16(playback_limit_value.value);
                exchange_bit_multi<uint8_t>(stream, playback_limit_override);
            }
            return;
        }

        //------------------------------------

        inline static auto exchange_section_sub(
            DataStreamView &stream,
            AudioPlaybackLimitSetting const &playback_limit_value,
            AudioBusMuteForBackgroundMusic const &mute_for_background_music_value,
            bool const &playback_limit_override) -> void
        {
            if (k_version >= 112_ui)
            {
                exchange_bit_multi<uint8_t>(
                    stream,
                    playback_limit_value.when_priority_is_equal,
                    playback_limit_value.when_limit_is_reached,
                    playback_limit_override,
                    mute_for_background_music_value.enable);
                stream.writeUint16(playback_limit_value.value);
            }
            return;
        }

        //------------------------------------

        inline static auto exchange_section_sub(
            DataStreamView &stream,
            AudioPlaybackLimitSetting const &playback_limit_value,
            AudioVirtualVoiceSetting const &virtual_voice_value,
            bool const &playback_limit_override,
            bool const &virtual_voice_override) -> void
        {
            if (k_version < 112_ui)
            {
                exchange_bit_multi<uint8_t>(stream, virtual_voice_value.on_return_to_physical);
                exchange_bit_multi<uint8_t>(stream, playback_limit_value.when_priority_is_equal);
                exchange_bit_multi<uint8_t>(stream, playback_limit_value.when_limit_is_reached);
                stream.writeUint16(playback_limit_value.value);
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
                stream.writeUint16(playback_limit_value.value);
                exchange_bit_multi<uint8_t>(stream, virtual_voice_value.behavior);
            }
            return;
        }

        //------------------------------------

        inline static auto exchange_section_sub(
            DataStreamView &stream,
            AudioVoiceVolumeGainSetting const &voice_volume_gain_value,
            AudioHDRSetting const &hdr_value,
            bool const &voice_volume_loudness_normalization_override,
            bool const &hdr_envelope_tracking_override) -> void
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
            StateSetting const &state_value) -> void
        {
            if (k_version < 125_ui)
            {
                exchange_list<true, uint32_t>(stream, state_value.item, [](auto &data, auto &value)
                                              {
                    data.writeUint32(value.group);
                    exchange_bit_multi<uint8_t>(data, value.change_occur_at);
                    exchange_list<true, uint16_t>(data, value.apply, [] (auto &data, auto &value) {
                        data.writeUint32(value.target);
                        data.writeUint32(value.setting);
                    }); });
            }
            else
            {
                exchange_list<true, uint8_t>(
                    stream, state_value.attribute, [](auto &data, auto &value)
                    {
                        data.writeUint8(value.type);
                        exchange_bit_multi<uint8_t>(data, value.category);
                        if (k_version >= 128_ui) {
                        data.writeUint8(value.u1);
                        } });
                exchange_list<true, uint8_t>(
                    stream,
                    state_value.item, [](auto &data, auto &value)
                    {
                        data.writeUint32(value.group);
                        exchange_bit_multi<uint8_t>(data, value.change_occur_at);
                        exchange_list<true, uint8_t>(data, value.apply, [] (auto &data, auto &value) {
                        data.writeUint32(value.target);
                        data.writeUint32(value.setting);
                    }); });
            }
            return;
        }

        //------------------------------------

        inline static auto exchange_section_sub(
            DataStreamView &stream,
            RealTimeParameterControlSetting const &real_time_parameter_control_value) -> void
        {
            exchange_list<true, uint16_t>(
                stream, real_time_parameter_control_value.item, [](auto &data, auto &value)
                {
                    data.writeUint32(value.parameter.id);
                    if (k_version >= 112_ui) {
                        exchange_bit_multi<uint8_t>(data, value.parameter.category);
                        exchange_bit_multi<uint8_t>(data, value.u1);
                    }
                    if (k_version < 112_ui) {
                        data.writeUint32(value.type);
                    }
                    else {
                        data.writeUint8(value.type);
                    }
                    data.writeUint32(value.u2);
                    exchange_bit_multi<uint8_t>(data, value.mode);
                    exchange_list<true, uint16_t>(data, value.point, [] (auto &data, auto &value) {
                        data.writeFloat(value.position.x);
                        data.writeFloat(value.position.y);
                        exchange_bit_multi<uint32_t>(data, value.curve);
                    }); });
            return;
        }

        //------------------------------------

        inline static auto exchange_section_sub(
            DataStreamView &stream,
            MusicTransitionSettingItemFade const &fade_value) -> void
        {
            stream.writeUint32(fade_value.time);
            stream.writeUint32(fade_value.curve);
            stream.writeUint32(fade_value.offset);
            return;
        }

        //------------------------------------

        inline static auto exchange_section_sub(
            DataStreamView &stream,
            AudioSwitcherSetting const &switcher_value) -> void
        {
            if (k_version < 112_ui)
            {
                exchange_bit_multi<uint32_t>(stream, switcher_value.is_state);
            }
            else
            {
                exchange_bit_multi<uint8_t>(stream, switcher_value.is_state);
            }
            stream.writeUint32(switcher_value.group);
            stream.writeUint32(switcher_value.default_item);
            return;
        }

        //------------------------------------

        inline static auto exchange_section_sub(
            DataStreamView &stream,
            MusicTrackTransitionSetting const &transition_value) -> void
        {
            if (k_version >= 112_ui)
            {
               //stream.writeUint32(transition_value.switcher);
                exchange_section_sub(stream, transition_value.source.fade_out);
                exchange_bit_multi<uint32_t>(stream, transition_value.source.exit_source_at);
                stream.writeUint32(transition_value.source.exit_source_at_custom_cue_match);
                exchange_section_sub(stream, transition_value.destination.fade_in);
            }
            return;
        }

        //------------------------------------

        inline static auto exchange_section_sub(
            DataStreamView &stream,
            MusicTrackTrackType const &track_type_value) -> void
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
            MusicTrackTrackType const &track_type_value,
            AudioSwitcherSetting const &switcher_value,
            MusicTrackTransitionSetting const &transition_value) -> void
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
            MusicTrackStream const &stream_value) -> void
        {
            stream.writeUint16(stream_value.look_ahead_time);
            return;
        }

        //------------------------------------

        inline static auto exchange_section_sub(
            DataStreamView &stream,
            std::vector<EffectU1> const &u1_value) -> void
        {
            if (k_version >= 112_ui)
            {
                exchange_list<true, uint16_t>(stream, u1_value, [](auto &data, auto &value)
                                              {
                            data.writeUint8(value.type);
                    if (k_version >= 128_ui) {
                        exchange_bit_multi<uint8_t>(data, value.mode);
                    }
                    data.writeFloat(value.value); });
            }
            return;
        }

        //------------------------------------

        inline static auto exchange_section_sub(
            DataStreamView &stream,
            BusHDRSetting const &hdr_value) -> void
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
            AudioTimeSetting const &time_setting_value,
            bool const &time_setting_override) -> void
        {
            stream.writeDouble(time_setting_value.time);
            stream.writeDouble(time_setting_value.offset);
            stream.writeFloat(time_setting_value.tempo);
            stream.writeUint8(time_setting_value.signature.first);
            stream.writeUint8(time_setting_value.signature.second);
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
            MusicStingerSetting const &stinger_value) -> void
        {
            exchange_list<true, uint32_t>(stream, stinger_value.item, [](auto &data, auto &value)
                                          {
                            data.writeUint32(value.trigger);
                            data.writeUint32(value.segment_to_play);
							exchange_bit_multi<uint32_t>(data, value.play_at);
                            data.writeUint32(value.cue_name);
                            data.writeUint32(value.do_not_play_this_stinger_again_for);
							exchange_bit_multi<uint32_t>(data, value.allow_playing_stinger_in_next_segment); });
            return;
        }

        //------------------------------------

        inline static auto exchange_section_sub(
            DataStreamView &stream,
            AudioBusConfiguration const &bus_configuration_value) -> void
        {
            if (k_version >= 88_ui)
            {
                stream.writeUint32(bus_configuration_value.u1);
            }
            return;
        }

        //------------------------------------

        inline static auto exchange_section_sub(
            DataStreamView &stream,
            MusicTransitionSetting const &transition_value) -> void
        {
            exchange_list<true, uint32_t>(stream, transition_value.item, [](auto &data, auto &value)
                                          {
                exchange_section_sub<uint32_t>(data, value.source.id);
                            exchange_section_sub<uint32_t>(data, value.destination.id);
                exchange_section_sub(data, value.source.fade_out);
                exchange_bit_multi<uint32_t>(data, value.source.exit_source_at);
                data.writeUint32(value.source.exit_source_at_custom_cue_match);
                if (k_version < 140_ui) {
                    exchange_bit_multi<uint8_t>(data, value.source.play_post_exit); //TODO.
                }
                else {
                    exchange_bit_multi<uint8_t>(data, value.source.play_post_exit);
                }
                exchange_section_sub(data, value.destination.fade_in);
                data.writeUint32(value.destination.custom_cue_filter_match_target);
                data.writeUint32(value.u1);
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
                    data.writeUint32(value.segment.id);
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
                } });
            return;
        }

        //-----------------------------------

        inline static auto exchange_section_sub(
            DataStreamView &stream,
            AudioAssociationSetting const &association_value) -> void
        {
            exchange_list<true, uint32_t>(stream, association_value.argument, [](auto &data, auto &value)
                                          { data.writeUint32(value.id); });
            if (k_version >= 88_ui)
            {
                exchange_list<false, uint32_t>(stream, association_value.argument, [](auto &data, auto &value)
                                               { exchange_bit_multi<uint8_t>(data, value.is_state); });
            }
            auto path_size = association_value.path.size();
            stream.writeUint32(static_cast<size_t>(path_size) * 12_size);

            if (k_version < 88_ui)
            {
                stream.writeUint8(association_value.probability);
            }
            exchange_bit_multi<uint8_t>(stream, association_value.mode);
            exchange_list<false, uint32_t>(stream, association_value.path, [](auto &data, auto &value)
                                           {
                        data.writeUint32(value.key);
                        data.writeUint32(value.object);
                        data.writeUint16(value.weight);
                        data.writeUint16(value.probability); });
            return;
        }

        //-----------------------------------

        inline static auto exchange_section_sub(
            DataStreamView &stream,
            AudioPlayMode const &play_mode_value) -> void
        {
            exchange_bit_multi<uint8_t>(stream, play_mode_value);
            return;
        }

        //-----------------------------------

        inline static auto exchange_section_sub(
            DataStreamView &stream,
            SoundPlaylistContainerScope const &scope_value,
            AudioPlayType const &play_type_value,
            AudioPlayTypeSetting const &play_type_setting_value,
            AudioPlayMode const &play_mode_value,
            AudioPlayModeSetting const &play_mode_setting_value) -> void
        {
            stream.writeUint16(play_mode_setting_value.continuous.loop.value);
            if (k_version >= 88_ui)
            {
                stream.writeUint16(play_mode_setting_value.continuous.loop.minimum_value);
                stream.writeUint16(play_mode_setting_value.continuous.loop.maximum_value);
            }
            stream.writeFloat(play_mode_setting_value.continuous.transition_duration.value);
            stream.writeFloat(play_mode_setting_value.continuous.transition_duration.minimum_value);
            stream.writeFloat(play_mode_setting_value.continuous.transition_duration.maximum_value);
            stream.writeUint16(play_type_setting_value.random.avoid_repeat);
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
            std::vector<SoundPlaylistContainerPlaylistItem> const &playlist_value) -> void
        {
            exchange_list<true, uint16_t>(stream, playlist_value, [](auto &data, auto &value)
                                          {
                            data.writeUint32(value.item);
                            data.writeUint32(value.weight); });
            return;
        }

        //-----------------------------------

        inline static auto exchange_section_sub(
            DataStreamView &stream,
            std::vector<SoundSwitchContainerObjectAttributeItem> const &object_attribute_value) -> void
        {
            exchange_list<true, uint32_t>(stream, object_attribute_value, [](auto &data, auto &value)
                                          {
                            data.writeUint32(value.id);
                if (k_version < 112_ui) {
                    exchange_bit_multi<uint8_t>(data, value.play_first_only);
                    exchange_bit_multi<uint8_t>(data, value.continue_to_play_across_switch);
                }
                else {
                    exchange_bit_multi<uint8_t>(data, value.play_first_only, value.continue_to_play_across_switch);
                }
                if (k_version < 112_ui) {
                    data.writeUint32(value.u1);
                }
                else {
                    data.writeUint8(value.u1);
                }
                data.writeUint32(value.fade_out_time);
                data.writeUint32(value.fade_in_time); });
            return;
        }

        //-----------------------------------

        inline static auto exchange_section_sub(
            DataStreamView &stream,
            std::vector<SoundSwitchContainerObjectAssignItem> const &assigned_object_value) -> void
        {
            exchange_list<true, uint32_t>(stream, assigned_object_value, [](auto &data, auto &value)
                                          {
                            data.writeUint32(value.item);
                exchange_list<true, uint32_t>(data, value.object, [] (auto &data, auto &value) {
                    data.writeUint32(value);
                }); });
            return;
        }

        //-----------------------------------

        inline static auto exchange_section_sub(
            DataStreamView &stream,
            std::vector<SoundBlendContainerTrackItem> const &track_value) -> void
        {
            exchange_list<true, uint32_t>(stream, track_value, [](auto &data, auto &value)
                                          {
                            data.writeUint32(value.id);
                exchange_section_sub(data, value.real_time_parameter_control);
                data.writeUint32(value.cross_fade.id);
                exchange_bit_multi<uint8_t>(data, value.cross_fade.category);
                exchange_list<true, uint32_t>(data, value.child, [] (auto &data, auto &value) {
                    data.writeUint32(value.id);
                    exchange_list<true, uint32_t>(data, value.point, [] (auto &data, auto &value) {
                        data.writeFloat(value.position.x);
                        data.writeFloat(value.position.y);
                         exchange_bit_multi<uint32_t>(data, value.curve);
                    });
                }); });
            return;
        }

        //-----------------------------------

        inline static auto exchange_section_sub(
            DataStreamView &stream,
            MusicSegmentCue const &cue_value) -> void
        {
            exchange_list<true, uint32_t>(stream, cue_value.item, [](auto &data, auto &value)
                                          {
                            data.writeUint32(value.id);
                            data.writeDouble(value.time);
                if (k_version < 140_ui) {
                    data.writeStringByUint32(value.name);
                }
                else {
                    data.writeStringByUint8(value.name);
                } });
            return;
        }

        //-----------------------------------

        inline static auto exchange_section_sub(
            DataStreamView &stream,
            std::vector<MusicPlaylistContainerPlaylistItem> const &playlist_value) -> void
        {
            exchange_list<true, uint32_t>(stream, playlist_value, [](auto &data, auto &value)
                                          {
                            data.writeUint32(value.item);
                            data.writeUint32(value.u1);
                            data.writeUint32(value.child_count);
                exchange_bit_multi<uint32_t>(data, value.play_mode, value.play_type); //TODO.
                data.writeUint16(value.loop);
                if (k_version >= 112_ui) {
                    exchange_raw_constant(data, 0_ui);
                }
                data.writeUint32(value.weight);
                data.writeUint16(value.random_setting.avoid_repeat);
                exchange_bit_multi<uint8_t>(data, value.group);
                exchange_bit_multi<uint8_t>(data, value.random_setting.type); });
            return;
        }

        //-----------------------------------

        inline static auto exchange_section_sub(
            DataStreamView &stream,
            std::vector<MusicSwitchContainerAssociationItem> &association_value) -> void
        {
            if (k_version < 88_ui)
            {
                exchange_list<true, uint32_t>(stream, association_value, [](auto &data, auto &value)
                                              {
                                data.writeUint32(value.item);
                                data.writeUint32(value.child); });
            }
            return;
        }

        //-----------------------------------

        inline static auto exchange_section_sub(
            DataStreamView &stream,
            BusAutomaticDuckingSetting const &automatic_ducking_value) -> void
        {
            stream.writeUint32(automatic_ducking_value.recovery_time);
            stream.writeFloat(automatic_ducking_value.maximum_ducking_volume);
            exchange_list<true, uint32_t>(stream, automatic_ducking_value.bus, [](auto &data, auto &value)
                                          {
                            data.writeUint32(value.id);
                            data.writeFloat(value.volume);
                            data.writeUint32(value.fade_out);
                            data.writeUint32(value.fade_in);
                exchange_bit_multi<uint8_t>(data, value.curve);
                exchange_bit_multi<uint8_t>(data, value.target); });
            return;
        }

        //-----------------------------------

        inline static auto exchange_section(
            DataStreamView &stream,
            EventAction const &value) -> void
        {
            exchange_bit_multi<uint8_t>(stream, value.scope, value.mode);
            auto type_data_begin = stream.write_pos;
            stream.writeNull(1_size);
            stream.writeUint32(value.target);
            stream.writeUint8(value.u1);
            auto exchange_section_sub_of_exception_list = [&]() -> void
            {
                if (k_version < 125)
                {
                    exchange_list<true, uint32_t>(stream, value.exception, [](auto &data, auto &value)
                                                  {
                                    data.writeUint32(value.id);
                        exchange_bit_multi<uint8_t>(data, value.u1); });
                }
                else
                {
                    exchange_list<true, uint8_t>(stream, value.exception, [](auto &data, auto &value)
                                                 {
                                    data.writeUint32(value.id);
                        exchange_bit_multi<uint8_t>(data, value.u1); });
                }
                return;
            };
            auto has_case = false;
            auto type = 0_byte;
            switch (value.action_type)
            {
            case EventActionProperty::Type::play_audio:
            {
                type = 4_byte;
                auto &property_value = value.play_audio;
                exchange_section_sub<EventActionCommonPropertyType>(
                    stream, true, [&](auto &common_property)
                    {
                            convert_common_property_as_randomizable<EventActionCommonPropertyType::delay>(common_property, property_value.delay);
                            convert_common_property_as_randomizable<EventActionCommonPropertyType::fade_time>(common_property, property_value.fade_time);
                            convert_common_property<EventActionCommonPropertyType::probability>(common_property, property_value.probability); });
                exchange_bit_multi<uint8_t>(stream, property_value.fade_curve);
                stream.writeUint32(property_value.sound_bank);
                if (k_version >= 145_ui)
                {
                    exchange_raw_constant(stream, 0_ui);
                }
                has_case = true;
                break;
            }
            case EventActionProperty::Type::stop_audio:
            {
                type = 1_byte;
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
            case EventActionProperty::Type::pause_audio:
            {
                type = 2_byte;
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
            case EventActionProperty::Type::resume_audio:
            {
                type = 3_byte;
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
            case EventActionProperty::Type::break_audio:
            {
                type = 28_byte;
                auto &property_value = value.break_audio;
                exchange_section_sub<EventActionCommonPropertyType>(
                    stream, true, [&](auto &common_property)
                    { convert_common_property_as_randomizable<EventActionCommonPropertyType::delay>(common_property, property_value.delay); });
                has_case = true;
                break;
            }
            case EventActionProperty::Type::seek_audio:
            {
                type = 30_byte;
                auto &property_value = value.seek_audio;
                exchange_section_sub<EventActionCommonPropertyType>(
                    stream, true, [&](auto &common_property)
                    { convert_common_property_as_randomizable<EventActionCommonPropertyType::delay>(common_property, property_value.delay); });
                exchange_bit_multi<uint8_t>(stream, property_value.seek_type);
                stream.writeFloat(property_value.seek_value.value);
                stream.writeFloat(property_value.seek_value.minimum_value);
                stream.writeFloat(property_value.seek_value.maximum_value);
                exchange_bit_multi<uint8_t>(stream, property_value.seek_to_nearest_marker);
                exchange_section_sub_of_exception_list();
                has_case = true;
                break;
            }
            case EventActionProperty::Type::post_event:
            {
                type = 33_byte;
                auto &property_value = value.post_event;
                exchange_section_sub<EventActionCommonPropertyType>(
                    stream, true, [&](auto &common_property)
                    { convert_common_property_as_randomizable<EventActionCommonPropertyType::delay>(common_property, property_value.delay); });
                has_case = true;
                break;
            }
            case EventActionProperty::Type::set_voice_pitch:
            {
                auto &property_value = value.set_voice_pitch;
                type = !property_value.reset ? 8_byte : 9_byte;
                exchange_section_sub<EventActionCommonPropertyType>(
                    stream, true, [&](auto &common_property)
                    {
                            convert_common_property_as_randomizable<EventActionCommonPropertyType::delay>(common_property, property_value.delay);
                            convert_common_property_as_randomizable<EventActionCommonPropertyType::fade_time>(common_property, property_value.fade_time); });
                exchange_bit_multi<uint8_t>(stream, property_value.fade_curve);
                exchange_bit_multi<uint8_t>(stream, property_value.apply_mode);
                stream.writeFloat(property_value.value.value);
                stream.writeFloat(property_value.value.minimum_value);
                stream.writeFloat(property_value.value.maximum_value);
                exchange_section_sub_of_exception_list();
                has_case = true;
                break;
            }
            case EventActionProperty::Type::set_voice_volume:
            {
                auto &property_value = value.set_voice_volume;
                type = !property_value.reset ? 10_byte : 11_byte;
                exchange_section_sub<EventActionCommonPropertyType>(
                    stream, true, [&](auto &common_property)
                    {
                            convert_common_property_as_randomizable<EventActionCommonPropertyType::delay>(common_property, property_value.delay);
                            convert_common_property_as_randomizable<EventActionCommonPropertyType::fade_time>(common_property, property_value.fade_time); });
                exchange_bit_multi<uint8_t>(stream, property_value.fade_curve);
                exchange_bit_multi<uint8_t>(stream, property_value.apply_mode);
                stream.writeFloat(property_value.value.value);
                stream.writeFloat(property_value.value.minimum_value);
                stream.writeFloat(property_value.value.maximum_value);
                exchange_section_sub_of_exception_list();
                has_case = true;
                break;
            }
            case EventActionProperty::Type::set_bus_volume:
            {
                auto &property_value = value.set_bus_volume;
                type = !property_value.reset ? 12_byte : 13_byte;
                exchange_section_sub<EventActionCommonPropertyType>(
                    stream, true, [&](auto &common_property)
                    {
                            convert_common_property_as_randomizable<EventActionCommonPropertyType::delay>(common_property, property_value.delay);
                            convert_common_property_as_randomizable<EventActionCommonPropertyType::fade_time>(common_property, property_value.fade_time); });
                exchange_bit_multi<uint8_t>(stream, property_value.fade_curve);
                exchange_bit_multi<uint8_t>(stream, property_value.apply_mode);
                stream.writeFloat(property_value.value.value);
                stream.writeFloat(property_value.value.minimum_value);
                stream.writeFloat(property_value.value.maximum_value);
                exchange_section_sub_of_exception_list();
                has_case = true;
                break;
            }
            case EventActionProperty::Type::set_voice_low_pass_filter:
            {
                auto &property_value = value.set_voice_low_pass_filter;
                type = !property_value.reset ? 14_byte : 15_byte;
                exchange_section_sub<EventActionCommonPropertyType>(
                    stream, true, [&](auto &common_property)
                    {
                            convert_common_property_as_randomizable<EventActionCommonPropertyType::delay>(common_property, property_value.delay);
                            convert_common_property_as_randomizable<EventActionCommonPropertyType::fade_time>(common_property, property_value.fade_time); });
                exchange_bit_multi<uint8_t>(stream, property_value.fade_curve);
                exchange_bit_multi<uint8_t>(stream, property_value.apply_mode);
                stream.writeFloat(property_value.value.value);
                stream.writeFloat(property_value.value.minimum_value);
                stream.writeFloat(property_value.value.maximum_value);
                exchange_section_sub_of_exception_list();
                has_case = true;
                break;
            }
            case EventActionProperty::Type::set_voice_high_pass_filter:
            {
                if (k_version >= 112_ui)
                {
                    auto &property_value = value.set_voice_high_pass_filter;
                    type = !property_value.reset ? 32_byte : 48_byte;
                    exchange_section_sub<EventActionCommonPropertyType>(
                        stream, true, [&](auto &common_property)
                        {
                            convert_common_property_as_randomizable<EventActionCommonPropertyType::delay>(common_property, property_value.delay);
                            convert_common_property_as_randomizable<EventActionCommonPropertyType::fade_time>(common_property, property_value.fade_time); });
                    exchange_bit_multi<uint8_t>(stream, property_value.fade_curve);
                    exchange_bit_multi<uint8_t>(stream, property_value.apply_mode);
                    stream.writeFloat(property_value.value.value);
                    stream.writeFloat(property_value.value.minimum_value);
                    stream.writeFloat(property_value.value.maximum_value);
                    exchange_section_sub_of_exception_list();
                    has_case = true;
                }
                break;
            }
            case EventActionProperty::Type::set_mute:
            {
                auto &property_value = value.set_mute;
                type = !property_value.reset ? 6_byte : 7_byte;
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
            case EventActionProperty::Type::set_game_parameter:
            {
                auto &property_value = value.set_game_parameter;
                type = !property_value.reset ? 19_byte : 20_byte;
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
                stream.writeFloat(property_value.value.value);
                stream.writeFloat(property_value.value.minimum_value);
                stream.writeFloat(property_value.value.maximum_value);
                exchange_section_sub_of_exception_list();
                has_case = true;
                break;
            }
            case EventActionProperty::Type::set_state_availability:
            {
                auto &property_value = value.set_state_availability;
                type = !property_value.enable ? 17_byte : 16_byte;
                exchange_section_sub<EventActionCommonPropertyType>(
                    stream, true, [&](auto &common_property)
                    { convert_common_property_as_randomizable<EventActionCommonPropertyType::delay>(common_property, property_value.delay); });
                has_case = true;
                break;
            }
            case EventActionProperty::Type::activate_state:
            {
                type = 18_byte;
                auto &property_value = value.activate_state;
                exchange_section_sub<EventActionCommonPropertyType>(
                    stream, true, [&](auto &common_property)
                    { convert_common_property_as_randomizable<EventActionCommonPropertyType::delay>(common_property, property_value.delay); });
                stream.writeUint32(property_value.group);
                stream.writeUint32(property_value.item);
                has_case = true;
                break;
            }
            case EventActionProperty::Type::activate_switch:
            {
                type = 25_byte;
                auto &property_value = value.activate_switch;
                exchange_section_sub<EventActionCommonPropertyType>(
                    stream, true, [&](auto &common_property)
                    { convert_common_property_as_randomizable<EventActionCommonPropertyType::delay>(common_property, property_value.delay); });
                stream.writeUint32(property_value.group);
                stream.writeUint32(property_value.item);
                has_case = true;
                break;
            }
            case EventActionProperty::Type::activate_trigger:
            {
                type = 29_byte;
                auto &property_value = value.activate_trigger;
                exchange_section_sub<EventActionCommonPropertyType>(
                    stream, true, [&](auto &common_property)
                    { convert_common_property_as_randomizable<EventActionCommonPropertyType::delay>(common_property, property_value.delay); });
                has_case = true;
                break;
            }
            case EventActionProperty::Type::set_bypass_effect:
            {
                auto &property_value = value.set_bypass_effect;
                type = !property_value.reset ? 26_byte : 27_byte;
                exchange_section_sub<EventActionCommonPropertyType>(
                    stream, true, [&](auto &common_property)
                    { convert_common_property_as_randomizable<EventActionCommonPropertyType::delay>(common_property, property_value.delay); });
                exchange_bit_multi<uint8_t>(stream, property_value.enable);
                exchange_bit_multi<uint8_t>(stream, property_value.value[0],
                                            property_value.value[1],
                                            property_value.value[2],
                                            property_value.value[3],
                                            property_value.value[4],
                                            property_value.reset,
                                            property_value.reset,
                                            property_value.reset);
                exchange_section_sub_of_exception_list();
                has_case = true;
                break;
            }
            case EventActionProperty::Type::release_envelope:
            {
                if (k_version >= 112_ui)
                {
                    type = 31_byte;
                    auto &property_value = value.release_envelope;
                    exchange_section_sub<EventActionCommonPropertyType>(
                        stream, true, [&](auto &common_property)
                        { convert_common_property_as_randomizable<EventActionCommonPropertyType::delay>(common_property, property_value.delay); });
                    has_case = true;
                }
                break;
            }
            case EventActionProperty::Type::reset_playlist:
            {
                if (k_version >= 113_ui)
                {
                    type = 34_byte;
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
            auto current_pos = stream.write_pos;
            stream.writeUint8(type, type_data_begin);
            stream.write_pos = current_pos;
            return;
        }

        //-----------------------------------

        inline static auto exchange_section(
            DataStreamView &stream,
            Event const &value) -> void
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
            DialogueEvent const &value) -> void
        {
            if (k_version >= 88_ui)
            {
                stream.writeUint8(value.probability);
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
            Attenuation const &value) -> void
        {
            if (k_version >= 140_ui)
            {
                exchange_bit_multi<uint8_t>(stream, value.height_spread);
            }
            exchange_bit_multi<uint8_t>(stream, value.cone.enable);
            if (value.cone.enable)
            {
                stream.writeFloat(value.cone.inner_angle);
                stream.writeFloat(value.cone.outer_angle);
                stream.writeFloat(value.cone.maximum_value);
                stream.writeFloat(value.cone.low_pass_filter);
                if (k_version >= 112_ui)
                {
                    stream.writeFloat(value.cone.high_pass_filter);
                }
            }
            if (k_version < 88_ui)
            {
                stream.writeUint8(value.apply.output_bus_volume);
                stream.writeUint8(value.apply.auxiliary_send_volume);
                stream.writeUint8(value.apply.low_pass_filter);
                stream.writeUint8(value.apply.spread);
            }
            else if (k_version < 112_ui)
            {
                stream.writeUint8(value.apply.output_bus_volume);
                stream.writeUint8(value.apply.game_defined_auxiliary_send_volume);
                stream.writeUint8(value.apply.user_defined_auxiliary_send_volume);
                stream.writeUint8(value.apply.low_pass_filter);
                stream.writeUint8(value.apply.spread);
            }
            else if (k_version < 145_ui)
            {
                stream.writeUint8(value.apply.output_bus_volume);
                stream.writeUint8(value.apply.game_defined_auxiliary_send_volume);
                stream.writeUint8(value.apply.user_defined_auxiliary_send_volume);
                stream.writeUint8(value.apply.low_pass_filter);
                stream.writeUint8(value.apply.high_pass_filter);
                stream.writeUint8(value.apply.spread);
                stream.writeUint8(value.apply.focus);
            }
            else
            {
                stream.writeUint8(value.apply.distance_output_bus_volume);
                stream.writeUint8(value.apply.distance_game_defined_auxiliary_send_volume);
                stream.writeUint8(value.apply.distance_user_defined_auxiliary_send_volume);
                stream.writeUint8(value.apply.distance_low_pass_filter);
                stream.writeUint8(value.apply.distance_high_pass_filter);
                stream.writeUint8(value.apply.distance_spread);
                stream.writeUint8(value.apply.distance_focus);
                stream.writeUint8(value.apply.obstruction_volume);
                stream.writeUint8(value.apply.obstruction_low_pass_filter);
                stream.writeUint8(value.apply.obstruction_high_pass_filter);
                stream.writeUint8(value.apply.occlusion_volume);
                stream.writeUint8(value.apply.occlusion_low_pass_filter);
                stream.writeUint8(value.apply.occlusion_high_pass_filter);
                stream.writeUint8(value.apply.diffraction_volume);
                stream.writeUint8(value.apply.output_bus_volume);
                stream.writeUint8(value.apply.diffraction_low_pass_filter);
                stream.writeUint8(value.apply.diffraction_high_pass_filter);
                stream.writeUint8(value.apply.transmission_volume);
                stream.writeUint8(value.apply.transmission_low_pass_filter);
                stream.writeUint8(value.apply.transmission_high_pass_filter);
            }
            exchange_list<true, uint8_t>(stream, value.curve, [](auto &data, auto &value)
                                         {
                exchange_bit_multi<uint8_t>(data, value.mode);
                exchange_list<true, uint16_t>(data, value.point, [] (auto &data, auto &value) {
                    data.writeFloat(value.position.x);
                    data.writeFloat(value.position.y);
                    exchange_bit_multi<uint32_t>(data, value.curve);
                }); });
            exchange_section_sub(stream, value.real_time_parameter_control);
            return;
        }

        //-----------------------------------

        inline static auto exchange_section(
            DataStreamView &stream,
            LowFrequencyOscillatorModulator const &value) -> void
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
            EnvelopeModulator const &value) -> void
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
            TimeModulator const &value) -> void
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
            Effect const &value) -> void
        {
            stream.writeUint32(value.plug_in);
            exchange_data(stream, value.expand);
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
            Source const &value) -> void
        {
            stream.writeUint32(value.plug_in);
            exchange_data(stream, value.expand);
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
            AudioDevice const &value) -> void
        {
            if (k_version >= 128_ui)
            {
                stream.writeUint32(value.plug_in);
                exchange_data(stream, value.expand);
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
            AudioBus const &value) -> void
        {
            stream.writeUint32(value.parent);
            if (k_version >= 128_ui)
            {
                if (value.parent == 0_ui)
                {
                    stream.writeUint32(value.audio_device);
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
                    auto override_positioning = true;
                    exchange_section_sub(stream, value.positioning, override_positioning);
                    if (k_version < 135_ui)
                    {
                        auto override_game_defined_auxiliary_send = true;
                        auto override_user_defined_auxiliary_send = true;
                        exchange_section_sub(stream, value.auxiliary_send, override_game_defined_auxiliary_send, override_user_defined_auxiliary_send);
                    }
                    else
                    {
                        auto override_game_defined_auxiliary_send = true;
                        auto override_user_defined_auxiliary_send = true;
                        auto override_early_reflection_auxiliary_send = true;
                        exchange_section_sub(stream, value.auxiliary_send, override_game_defined_auxiliary_send, override_user_defined_auxiliary_send, override_early_reflection_auxiliary_send);
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
                    stream.writeUint32(value.mixer);
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
            AuxiliaryAudioBus const &value) -> void
        {
            stream.writeUint32(value.parent);
            if (k_version >= 128_ui)
            {
                if (value.parent == 0_ui)
                {
                    stream.writeUint32(value.audio_device);
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
                    auto override_positioning = true;
                    exchange_section_sub(stream, value.positioning, override_positioning);
                    if (k_version < 135_ui)
                    {
                        auto override_game_defined_auxiliary_send = true;
                        auto override_user_defined_auxiliary_send = true;
                        exchange_section_sub(stream, value.auxiliary_send, override_game_defined_auxiliary_send, override_user_defined_auxiliary_send);
                    }
                    else
                    {
                        auto override_game_defined_auxiliary_send = true;
                        auto override_user_defined_auxiliary_send = true;
                        auto override_early_reflection_auxiliary_send = true;
                        exchange_section_sub(stream, value.auxiliary_send, override_game_defined_auxiliary_send, override_user_defined_auxiliary_send, override_early_reflection_auxiliary_send);
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
                    stream.writeUint32(value.mixer);
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
            Sound const &value) -> void
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
            stream.writeUint32(value.parent);
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
            SoundPlaylistContainer const &value) -> void
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
            stream.writeUint32(value.parent);
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
            SoundSwitchContainer const &value) -> void
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
            stream.writeUint32(value.parent);
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
            SoundBlendContainer const &value) -> void
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
            stream.writeUint32(value.parent);
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
            ActorMixer const &value) -> void
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
            stream.writeUint32(value.parent);
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
            MusicTrack const &value) -> void
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
            stream.writeUint32(value.parent);
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
            MusicSegment const &value) -> void
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
            stream.writeUint32(value.parent);
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
            stream.writeDouble(value.playback_setting.duration);
            exchange_section_sub(stream, value.playback_setting.cue);
            return;
        }

        //------------------------------------

        inline static auto exchange_section(
            DataStreamView &stream,
            MusicPlaylistContainer const &value) -> void
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
            stream.writeUint32(value.parent);
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
            MusicSwitchContainer const &value) -> void
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
            stream.writeUint32(value.parent);
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

        //--------------------------------

        inline static auto exchange_bank_header(
            DataStreamView &stream,
            BankHeader const &value) -> void
        {
            assert_conditional(value.version > 52_ui, String::format(fmt::format("{}", Language::get("wwise.soundbank.decode.unsupported_bank_version")), std::to_string(value.version)), "exchange_bank_header");
            stream.writeUint32(value.version);
            k_version = value.version;
            stream.writeUint32(value.soundbank_id);
            stream.writeUint32(value.language_id);
            stream.writeUint32(value.head_info.values);
            if (k_version > 76_ui)
            {
                stream.writeUint32(value.head_info.project_id);
            }
            if (k_version > 141_ui)
            {
                stream.writeUint32(value.head_info.soundbank_type);
                exchange_data(stream, value.head_info.bank_hash);
            }
            stream.writeNull(value.head_info.padding);
            return;
        }

        inline static auto exchange_media(
            DataStreamView &stream,
            std::vector<uint32_t> const &value,
            ChunkSign &sign,
            std::string_view source) -> void
        {
            chuck_sign_set(stream, sign);
            auto data_stream = DataStreamView{};
            auto data_count = value.size();
            for (auto data_index : Range(data_count))
            {
                auto data = FileSystem::read_binary<uint8_t>(fmt::format("{}/media/{}.wem", source, value[data_index]));
                stream.writeUint32(value[data_index]);
                stream.writeUint32(static_cast<uint32_t>(data_stream.write_pos));
                stream.writeUint32(static_cast<uint32_t>(data.size()));
                data_stream.writeBytes(data);
                if (data_index < data_count - 1_size)
                {
                    data_stream.writeNull(compute_padding_size(data_stream.write_pos, k_data_block_padding_size));
                }
            }
            chuck_sign_write(stream, sign);
            auto data_sign = ChunkSign{
                .id = ChunkSignFlag::data,
                .size = static_cast<uint32_t>(data_stream.size())};
            exchange_chuck_sign(data_sign, stream);
            stream.writeBytes(data_stream.toBytes());
            return;
        }

        inline static auto exchange_plugin_chunk(
            DataStreamView &stream,
            Setting const &value) -> void
        {
            if (k_version >= 118_ui)
            {
                exchange_list<true, uint32_t>(stream, value.plug_in, [](auto &data, auto &value)
                                              {
					data.writeUint32(value.id);
					if (k_version < 140_ui) {	
						auto string_length = value.library.size() + 1_size;
						data.writeUint32(string_length);
						data.writeString(value.library);
						data.writeNull(1_size);
					}
					else {
						data.writeStringByEmpty(value.library);
					} });
            };
            return;
        }

        inline static auto exchange_section(
            DataStreamView &stream,
            StateGroup const &value) -> void
        {
            stream.writeUint32(value.id);
            stream.writeUint32(value.default_transition);
            exchange_list<true, uint32_t>(stream, value.custom_transition, [](auto &data, auto &value)
                                          {
							data.writeUint32(value.from);
							data.writeUint32(value.to);
							data.writeUint32(value.time); });
            return;
        }

        //--------------------------

        inline static auto exchange_section(
            DataStreamView &stream,
            SwitchGroup const &value) -> void
        {
            stream.writeUint32(value.id);
            stream.writeUint32(value.parameter.id);
            if (k_version >= 112_ui)
            {
                exchange_bit_multi<uint8_t>(stream, value.parameter.category);
            }
            exchange_list<true, uint32_t>(stream, value.point, [](auto &data, auto &value)
                                          {
				data.writeFloat(value.position.x);
				data.writeUint32(static_cast<uint32_t>(value.position.y));
                exchange_bit_multi<uint32_t>(data, value.curve); });
            return;
        }

        //---------------------------

        inline static auto exchange_section(
            DataStreamView &stream,
            GameParameter const &value) -> void
        {
            stream.writeUint32(value.id);
            stream.writeFloat(value.range_default);
            if (k_version >= 112_ui)
            {
                exchange_bit_multi<uint32_t>(stream, value.interpolation_mode);
                stream.writeFloat(value.interpolation_attack);
                stream.writeFloat(value.interpolation_release);
                exchange_bit_multi<uint8_t>(stream, value.bind_to_built_in_parameter);
            }
            return;
        }

        //--------------------------

        inline static auto exchange_section(
            DataStreamView &stream,
            GameSynchronizationU1 const &value) -> void
        {
            stream.writeUint32(value.id);
            if (k_version >= 140_ui)
            {
                stream.writeFloat(value.u1);
                stream.writeFloat(value.u2);
                stream.writeFloat(value.u3);
                stream.writeFloat(value.u4);
                stream.writeFloat(value.u5);
                stream.writeFloat(value.u6);
            }
            return;
        }

        //-----------------------

        inline static auto exchange_game_synchronization(
            DataStreamView &stream,
            Setting const &value,
            GameSynchronization const &game_synchronization_value) -> void
        {
            if (k_version >= 145_ui)
            {
                exchange_bit_multi<uint16_t>(stream, value.voice_filter_behavior);
            }
            stream.writeFloat(value.volume_threshold);
            stream.writeUint16(value.maximum_voice_instance);
            if (k_version >= 128_ui)
            {
                exchange_raw_constant(stream, 50_us);
            }
            exchange_list<true, uint32_t>(stream, game_synchronization_value.state_group, [](auto &data, auto &value)
                                          { exchange_section(data, value); });
            exchange_list<true, uint32_t>(stream, game_synchronization_value.switch_group, [](auto &data, auto &value)
                                          { exchange_section(data, value); });
            exchange_list<true, uint32_t>(stream, game_synchronization_value.game_parameter, [](auto &data, auto &value)
                                          { exchange_section(data, value); });
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
                    exchange_list<true, uint32_t>(stream, game_synchronization_value.u1, [](auto &data, auto &value)
                                                  { exchange_section(data, value); });
                }
            }
            return;
        }

        //------------------------------------

        inline static auto exchange_section(
            DataStreamView &stream,
            StatefulPropertySetting const &value) -> void
        {
            if (k_version < 128_ui)
            {
                exchange_list<true, uint8_t>(stream, value.value, [](auto &data, auto &value)
                                             { data.writeUint8(value.type); });
                exchange_list<false, uint8_t>(stream, value.value, [](auto &data, auto &value)
                                              { data.writeFloat(value.value); });
            }
            else
            {
                exchange_list<true, uint16_t>(stream, value.value, [](auto &data, auto &value)
                                              { data.writeUint16(value.type); });
                exchange_list<false, uint16_t>(stream, value.value, [](auto &data, auto &value)
                                               { data.writeFloat(value.value); });
            }
            return;
        }

        //---------------------------------

        inline static auto exchange_hierarchy(
            DataStreamView &stream,
            std::vector<Hierarchy> const &value) -> void
        {
            exchange_list<true, uint32_t>(stream, value, [](auto &data, auto &value)
                                          {
				exchange_bit_multi<uint8_t>(data, value.type);
				auto section_pos = data.write_pos;
				data.writeNull(4_size);
				data.writeUint32(value.id);
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
                            assert_conditional(false,  String::format(fmt::format("{}", Language::get("wwise.soundbank.encode.invalid_hierarchy_type")), magic_enum::enum_name(value.type)), "exchange_hierarchy");
                        }
                    }
				auto current_pos = data.write_pos;
				auto section_length = current_pos - section_pos - k_block_size;
				data.writeUint32(static_cast<uint32_t>(section_length), section_pos);
				data.write_pos = current_pos; });
            return;
        }

        inline static auto exchange_section_sub(
            DataStreamView &stream,
            ObstructionSetting const &value) -> void
        {
            exchange_bit_multi<uint8_t>(stream, value.enable);
            exchange_bit_multi<uint8_t>(stream, value.mode);
            exchange_list<true, uint16_t>(stream, value.point, [](auto &data, auto &value)
                                          {
                data.writeFloat(value.position.x);
				data.writeFloat(value.position.y);
                exchange_bit_multi<uint32_t>(data, value.curve); });
            return;
        }

        inline static auto exchange_environments_settings(
            DataStreamView &stream,
            Setting const &value) -> void
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
            std::vector<SoundBankReference> const &value) -> void
        {
            exchange_raw_constant(stream, 1_ui);
            exchange_list<true, uint32_t>(stream, value, [](auto &data, auto &value)
                                          {
				data.writeUint32(value.id);
				data.writeStringByUint8(value.name); });
            return;
        }

        inline static auto exchange_platform_settings(
            DataStreamView &stream,
            Setting const &value) -> void
        {
            if (k_version >= 113_ui)
            {
                if (k_version < 118_ui)
                {
                    stream.writeStringByUint32(value.platform);
                }
                else if (k_version < 140_ui)
                {
                    auto string_length = value.platform.size() + 1_size;
                    stream.writeUint32(string_length);
                    stream.writeString(value.platform);
                    stream.writeNull(1_size);
                }
                else
                {
                    stream.writeStringByEmpty(value.platform);
                }
            }
            return;
        }

        inline static auto exchange_sound_bank(
            DataStreamView &stream,
            SoundBankInformation const &definition,
            std::string_view source) -> void
        {
            auto sign = ChunkSign{};
            sign.id = ChunkSignFlag::bkhd;
            chuck_sign_set(stream, sign);
            exchange_bank_header(stream, definition.bank_header);
            chuck_sign_write(stream, sign);
            if (definition.has_media)
            {
                sign.id = ChunkSignFlag::didx;
                exchange_media(stream, definition.media, sign, source);
            }
            if (definition.has_setting && k_version >= 118_ui)
            {
                sign.id = ChunkSignFlag::init;
                chuck_sign_set(stream, sign);
                exchange_plugin_chunk(stream, definition.setting);
                chuck_sign_write(stream, sign);
            }
            if (definition.has_game_synchronization)
            {
                sign.id = ChunkSignFlag::stmg;
                chuck_sign_set(stream, sign);
                exchange_game_synchronization(stream, definition.setting, definition.game_synchronization);
                chuck_sign_write(stream, sign);
            }
            if (definition.has_hierarchy)
            {
                sign.id = ChunkSignFlag::hirc;
                chuck_sign_set(stream, sign);
                exchange_hierarchy(stream, definition.hierarchy);
                chuck_sign_write(stream, sign);
            }
            if (definition.has_setting)
            {
                sign.id = ChunkSignFlag::envs;
                chuck_sign_set(stream, sign);
                exchange_environments_settings(stream, definition.setting);
                chuck_sign_write(stream, sign);
            }
            if (definition.has_reference)
            {
                sign.id = ChunkSignFlag::stid;
                chuck_sign_set(stream, sign);
                exchange_string_mapping(stream, definition.reference);
                chuck_sign_write(stream, sign);
            }
            if (definition.has_setting && k_version >= 113_ui)
            {
                sign.id = ChunkSignFlag::plat;
                chuck_sign_set(stream, sign);
                exchange_platform_settings(stream, definition.setting);
                chuck_sign_write(stream, sign);
            }
            return;
        }

    public:
        inline static auto process_whole(
            DataStreamView &stream,
            SoundBankInformation const &definition,
            std::string_view source) -> void
        {
            exchange_sound_bank(stream, definition, source);
            return;
        }

        inline static auto process_fs(
            std::string_view source,
            std::string_view destination) -> void
        {
            auto stream = DataStreamView{};
            auto definition = *FileSystem::read_json(fmt::format("{}/data.json", source));
            process_whole(stream, definition, source);
            stream.out_file(destination);
            return;
        }
    };
}