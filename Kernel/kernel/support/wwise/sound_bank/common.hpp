#pragma once

#include "kernel/support/miscellaneous/shared.hpp"
#include "kernel/support/wwise/sound_bank/definition.hpp"

namespace Sen::Kernel::Support::WWise::SoundBank
{
    using namespace Definition;

    using namespace Sen::Kernel::Support::Miscellaneous::Shared;

    struct Common
    {

        inline static constexpr auto k_hierarchy_event_type_string = "event"_sv;

        inline static constexpr auto k_data_block_padding_size = 0x10_size;

        inline static auto k_bitset_boolean_fill = false;

        struct ChunkSignFlag
        {

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

        struct ChunkSign
        {
            std::string_view id;
            uint32_t pos;
            uint32_t size;
        };

        inline static auto exchange_chuck_sign(
            DataStreamView &stream,
            ChunkSign &value) -> void
        {
            value.id = stream.readString(4);
            value.size = stream.readUint32();
            return;
        }

        inline static auto exchange_chuck_sign(
            ChunkSign const &value,
            DataStreamView &stream) -> void
        {
            stream.writeString(value.id);
            stream.writeUint32(value.size);
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

        enum class EventActionCommonPropertyType : uint8_t
        {
            delay,
            fade_time,
            probability
        };

        enum class ModulatorCommonPropertyType : uint8_t
        { // >= 112
            scope,
            trigger_on,
            depth,
            frequency,
            waveform,
            smoothing,
            pulse_width_modulation,
            attack,
            initial_phase_offset,
            attack_time,
            attack_curve,
            decay_time,
            sustain_level,
            release_time,
            sustain_time,
            initial_delay,
            duration,
            loop,
            playback_rate,
            stop_playback
        };

        enum class AudioCommonPropertyType : uint8_t
        {
            bus_volume,
            output_bus_volume,
            output_bus_low_pass_filter,
            voice_volume,
            voice_pitch,
            voice_low_pass_filter,
            game_defined_auxiliary_send_volume,
            user_defined_auxiliary_send_volume_0,
            user_defined_auxiliary_send_volume_1,
            user_defined_auxiliary_send_volume_2,
            user_defined_auxiliary_send_volume_3,
            positioning_center_percent,
            positioning_speaker_panning_x,
            positioning_speaker_panning_y,
            playback_priority_value,
            playback_priority_offset_at_maximum_distance,
            playback_loop,
            motion_volume_offset,
            motion_low_pass_filter,
            voice_volume_make_up_gain,
            hdr_threshold,
            hdr_ratio,
            hdr_release_time,
            hdr_window_tap_output_game_parameter_id,
            hdr_window_tap_output_game_parameter_minimum,
            hdr_window_tap_output_game_parameter_maximum,
            hdr_envelope_tracking_active_range,
            playback_initial_delay,
            output_bus_high_pass_filter,
            voice_high_pass_filter,
            midi_note_tracking_root_note,
            midi_event_play_on,
            midi_transformation_transposition,
            midi_transformation_velocity_offset,
            midi_filter_key_range_minimum,
            midi_filter_key_range_maximum,
            midi_filter_velocity_minimum,
            midi_filter_velocity_maximum,
            midi_filter_channel,
            midi_clip_tempo_source,
            midi_target_id,
            playback_speed,
            mixer_id,
            game_defined_auxiliary_send_low_pass_filter,
            game_defined_auxiliary_send_high_pass_filter,
            user_defined_auxiliary_send_low_pass_filter_0,
            user_defined_auxiliary_send_low_pass_filter_1,
            user_defined_auxiliary_send_low_pass_filter_2,
            user_defined_auxiliary_send_low_pass_filter_3,
            user_defined_auxiliary_send_high_pass_filter_0,
            user_defined_auxiliary_send_high_pass_filter_1,
            user_defined_auxiliary_send_high_pass_filter_2,
            user_defined_auxiliary_send_high_pass_filter_3,
            positioning_listener_routing_speaker_panning_division_spatialization_mix,
            positioning_listener_routing_attenuation_id,
            early_reflection_auxiliary_send_volume,
            positioning_speaker_panning_z
        };

        template <typename Type>
            requires std::is_enum<Type>::value && true
        struct CommonPropertyMap
        {
            std::vector<std::pair<Type, std::any>> regular;
            std::vector<std::pair<Type, std::array<std::any, 2>>> randomizer;
        };

        // Type is uint8_t || enum
        template <typename InputType, typename ReturnType>
            requires((std::is_enum<InputType>::value || std::is_same<InputType, uint8_t>::value)) && (std::is_enum<ReturnType>::value || std::is_same<ReturnType, uint8_t>::value || std::is_same<ReturnType, size_t>::value) && true
        inline static auto exchange_enumeration(
            InputType const &data) -> ReturnType
        {
            // static_assert(sizeof...(InputType) == 1 || sizeof...(InputType) == 0, "Expected 0 or 1 argument only");
            static_assert(std::is_enum<InputType>::value || std::is_enum<ReturnType>::value, "Must be one enumeration type");
            if constexpr (std::is_same<InputType, Curve>::value || std::is_same<ReturnType, Curve>::value)
            {
                if constexpr (std::is_same<ReturnType, size_t>::value)
                {
                    return 4_size;
                }
                else
                {
                    constexpr auto value_package = std::array<std::pair<uint8_t, Curve>, 10>{
                        std::pair(9, Curve::constant),
                        std::pair(4, Curve::linear),
                        std::pair(5, Curve::s),
                        std::pair(3, Curve::s_inverted),
                        std::pair(1, Curve::sine),
                        std::pair(7, Curve::sine_reciprocal),
                        std::pair(2, Curve::logarithmic_1dot41),
                        std::pair(0, Curve::logarithmic_3dot0),
                        std::pair(6, Curve::exponential_1dot41),
                        std::pair(8, Curve::exponential_3dot0)};

                    //---------------------------------
                    if constexpr (std::is_same<InputType, uint8_t>::value)
                    {
                        static_assert(std::is_enum<ReturnType>::value, "Return type must be enumeration type");
                        if (auto it = std::find_if(value_package.begin(), value_package.end(), [&](auto &element)
                                                   { return element.first == data; });
                            it != value_package.end())
                        {
                            return it->second;
                        }
                    }
                    if constexpr (std::is_enum<InputType>::value)
                    {
                        static_assert(std::is_same<ReturnType, uint8_t>::value, "Return type must be uint8_t");
                        if (auto it = std::find_if(value_package.begin(), value_package.end(), [&](auto &element)
                                                   { return element.second == data; });
                            it != value_package.end())
                        {
                            return it->first;
                        }
                    }
                }
            }
            if constexpr (std::is_same<InputType, AudioSourceType>::value || std::is_same<ReturnType, AudioSourceType>::value)
            {
                if constexpr (std::is_same<ReturnType, size_t>::value)
                {
                    return 2_size;
                }
                else
                {
                    if (k_version < 112_ui)
                    {
                        constexpr auto value_package = {
                            std::pair(0, AudioSourceType::embedded),
                            std::pair(1, AudioSourceType::streamed),
                            std::pair(2, AudioSourceType::streamed_prefetched)};
                        //---------------------------------
                        if constexpr (std::is_same<InputType, uint8_t>::value)
                        {
                            static_assert(std::is_enum<ReturnType>::value, "Return type must be enumeration type");
                            if (auto it = std::find_if(value_package.begin(), value_package.end(), [&](auto &element)
                                                       { return element.first == data; });
                                it != value_package.end())
                            {
                                return it->second;
                            }
                        }
                        if constexpr (std::is_enum<InputType>::value)
                        {
                            static_assert(std::is_same<ReturnType, uint8_t>::value, "Return type must be uint8_t");
                            if (auto it = std::find_if(value_package.begin(), value_package.end(), [&](auto &element)
                                                       { return element.second == data; });
                                it != value_package.end())
                            {
                                return it->first;
                            }
                        }
                    }
                    else
                    {
                        constexpr auto value_package = {
                            std::pair(0, AudioSourceType::embedded),
                            std::pair(2, AudioSourceType::streamed),
                            std::pair(1, AudioSourceType::streamed_prefetched)};
                        //---------------------------------
                        if constexpr (std::is_same<InputType, uint8_t>::value)
                        {
                            static_assert(std::is_enum<ReturnType>::value, "Return type must be enumeration type");
                            if (auto it = std::find_if(value_package.begin(), value_package.end(), [&](auto &element)
                                                       { return element.first == data; });
                                it != value_package.end())
                            {
                                return it->second;
                            }
                        }
                        if constexpr (std::is_enum<InputType>::value)
                        {
                            static_assert(std::is_same<ReturnType, uint8_t>::value, "Return type must be uint8_t");
                            if (auto it = std::find_if(value_package.begin(), value_package.end(), [&](auto &element)
                                                       { return element.second == data; });
                                it != value_package.end())
                            {
                                return it->first;
                            }
                        }
                    }
                }
            }
            if constexpr (std::is_same<InputType, MusicTrackClipCurveItemType>::value || std::is_same<ReturnType, MusicTrackClipCurveItemType>::value)
            {
                if constexpr (std::is_same<ReturnType, size_t>::value)
                {
                    return 3_size;
                }
                else
                {
                    if (k_version < 112_ui)
                    {
                        constexpr auto value_package = std::array<std::pair<uint8_t, MusicTrackClipCurveItemType>, 4>{
                            std::pair(0, MusicTrackClipCurveItemType::voice_volume),
                            std::pair(1, MusicTrackClipCurveItemType::voice_low_pass_filter),
                            std::pair(2, MusicTrackClipCurveItemType::clip_fade_in),
                            std::pair(3, MusicTrackClipCurveItemType::clip_fade_out)};
                        //---------------------------------
                        if constexpr (std::is_same<InputType, uint8_t>::value)
                        {
                            static_assert(std::is_enum<ReturnType>::value, "Return type must be enumeration type");
                            if (auto it = std::find_if(value_package.begin(), value_package.end(), [&](auto &element)
                                                       { return element.first == data; });
                                it != value_package.end())
                            {
                                return it->second;
                            }
                        }
                        if constexpr (std::is_enum<InputType>::value)
                        {
                            static_assert(std::is_same<ReturnType, uint8_t>::value, "Return type must be uint8_t");
                            if (auto it = std::find_if(value_package.begin(), value_package.end(), [&](auto &element)
                                                       { return element.second == data; });
                                it != value_package.end())
                            {
                                return it->first;
                            }
                        }
                    }
                    else
                    {
                        constexpr auto value_package = std::array<std::pair<uint8_t, MusicTrackClipCurveItemType>, 5>{
                            std::pair(0, MusicTrackClipCurveItemType::voice_volume),
                            std::pair(1, MusicTrackClipCurveItemType::voice_low_pass_filter),
                            std::pair(2, MusicTrackClipCurveItemType::voice_high_pass_filter),
                            std::pair(3, MusicTrackClipCurveItemType::clip_fade_in),
                            std::pair(4, MusicTrackClipCurveItemType::clip_fade_out)};
                        //---------------------------------
                        if constexpr (std::is_same<InputType, uint8_t>::value)
                        {
                            static_assert(std::is_enum<ReturnType>::value, "Return type must be enumeration type");
                            if (auto it = std::find_if(value_package.begin(), value_package.end(), [&](auto &element)
                                                       { return element.first == data; });
                                it != value_package.end())
                            {
                                return it->second;
                            }
                        }
                        if constexpr (std::is_enum<InputType>::value)
                        {
                            static_assert(std::is_same<ReturnType, uint8_t>::value, "Return type must be uint8_t");
                            if (auto it = std::find_if(value_package.begin(), value_package.end(), [&](auto &element)
                                                       { return element.second == data; });
                                it != value_package.end())
                            {
                                return it->first;
                            }
                        }
                    }
                }
            }
            if constexpr (std::is_same<InputType, TimePoint>::value || std::is_same<ReturnType, TimePoint>::value)
            {
                if constexpr (std::is_same<ReturnType, size_t>::value)
                {
                    return (k_version < 140_ui) ? 3_size : 4_size;
                }
                else
                {
                    if (k_version < 112_ui)
                    {
                        constexpr auto value_package = std::array<std::pair<uint8_t, TimePoint>, 8>{
                            std::pair(0, TimePoint::immediate),
                            std::pair(1, TimePoint::next_grid),
                            std::pair(2, TimePoint::next_bar),
                            std::pair(3, TimePoint::next_beat),
                            std::pair(4, TimePoint::next_cue),
                            std::pair(5, TimePoint::custom_cue),
                            std::pair(6, TimePoint::entry_cue),
                            std::pair(7, TimePoint::exit_cue)};
                        //---------------------------------
                        if constexpr (std::is_same<InputType, uint8_t>::value)
                        {
                            static_assert(std::is_enum<ReturnType>::value, "Return type must be enumeration type");
                            if (auto it = std::find_if(value_package.begin(), value_package.end(), [&](auto &element)
                                                       { return element.first == data; });
                                it != value_package.end())
                            {
                                return it->second;
                            }
                        }
                        if constexpr (std::is_enum<InputType>::value)
                        {
                            static_assert(std::is_same<ReturnType, uint8_t>::value, "Return type must be uint8_t");
                            if (auto it = std::find_if(value_package.begin(), value_package.end(), [&](auto &element)
                                                       { return element.second == data; });
                                it != value_package.end())
                            {
                                return it->first;
                            }
                        }
                    }
                    else
                    {
                        constexpr auto value_package = std::array<std::pair<uint8_t, TimePoint>, 9>{
                            std::pair(0, TimePoint::immediate),
                            std::pair(1, TimePoint::next_grid),
                            std::pair(2, TimePoint::next_bar),
                            std::pair(3, TimePoint::next_beat),
                            std::pair(4, TimePoint::next_cue),
                            std::pair(5, TimePoint::custom_cue),
                            std::pair(6, TimePoint::entry_cue),
                            std::pair(7, TimePoint::exit_cue),
                            std::pair(9, TimePoint::last_exit_position)};
                        //---------------------------------
                        if constexpr (std::is_same<InputType, uint8_t>::value)
                        {
                            static_assert(std::is_enum<ReturnType>::value, "Return type must be enumeration type");
                            if (auto it = std::find_if(value_package.begin(), value_package.end(), [&](auto &element)
                                                       { return element.first == data; });
                                it != value_package.end())
                            {
                                return it->second;
                            }
                        }
                        if constexpr (std::is_enum<InputType>::value)
                        {
                            static_assert(std::is_same<ReturnType, uint8_t>::value, "Return type must be uint8_t");
                            if (auto it = std::find_if(value_package.begin(), value_package.end(), [&](auto &element)
                                                       { return element.second == data; });
                                it != value_package.end())
                            {
                                return it->first;
                            }
                        }
                    }
                }
            }
            if constexpr (std::is_same<InputType, CoordinateMode>::value || std::is_same<ReturnType, CoordinateMode>::value)
            {
                if constexpr (std::is_same<ReturnType, size_t>::value)
                {
                    return 2_size;
                }
                else
                {
                    constexpr auto value_package = std::array<std::pair<uint8_t, CoordinateMode>, 3>{
                        std::pair(0, CoordinateMode::linear),
                        std::pair(2, CoordinateMode::scaled),
                        std::pair(3, CoordinateMode::scaled_3)};

                    //---------------------------------
                    if constexpr (std::is_same<InputType, uint8_t>::value)
                    {
                        static_assert(std::is_enum<ReturnType>::value, "Return type must be enumeration type");
                        if (auto it = std::find_if(value_package.begin(), value_package.end(), [&](auto &element)
                                                   { return element.first == data; });
                            it != value_package.end())
                        {
                            return it->second;
                        }
                    }
                    if constexpr (std::is_enum<InputType>::value)
                    {
                        static_assert(std::is_same<ReturnType, uint8_t>::value, "Return type must be uint8_t");
                        if (auto it = std::find_if(value_package.begin(), value_package.end(), [&](auto &element)
                                                   { return element.second == data; });
                            it != value_package.end())
                        {
                            return it->first;
                        }
                    }
                }
            }
            if constexpr (std::is_same<InputType, PropertyCategory>::value || std::is_same<ReturnType, PropertyCategory>::value)
            {
                if constexpr (std::is_same<ReturnType, size_t>::value)
                {
                    return (k_version < 128_ui) ? 2_size : 3_size;
                }
                else
                {
                    if (k_version < 128_ui)
                    {
                        constexpr auto value_package = std::array<std::pair<uint8_t, PropertyCategory>, 4>{
                            std::pair(0, PropertyCategory::unidirectional),
                            std::pair(1, PropertyCategory::bidirectional),
                            std::pair(2, PropertyCategory::bidirectional_ranged),
                            std::pair(3, PropertyCategory::boolean)};
                        //---------------------------------
                        if constexpr (std::is_same<InputType, uint8_t>::value)
                        {
                            static_assert(std::is_enum<ReturnType>::value, "Return type must be enumeration type");
                            if (auto it = std::find_if(value_package.begin(), value_package.end(), [&](auto &element)
                                                       { return element.first == data; });
                                it != value_package.end())
                            {
                                return it->second;
                            }
                        }
                        if constexpr (std::is_enum<InputType>::value)
                        {
                            static_assert(std::is_same<ReturnType, uint8_t>::value, "Return type must be uint8_t");
                            if (auto it = std::find_if(value_package.begin(), value_package.end(), [&](auto &element)
                                                       { return element.second == data; });
                                it != value_package.end())
                            {
                                return it->first;
                            }
                        }
                    }
                    else if (k_version < 145_ui)
                    {
                        constexpr auto value_package = std::array<std::pair<uint8_t, PropertyCategory>, 4>{
                            std::pair(1, PropertyCategory::unidirectional),
                            std::pair(2, PropertyCategory::bidirectional),
                            std::pair(3, PropertyCategory::bidirectional_ranged),
                            std::pair(4, PropertyCategory::boolean)};
                        //---------------------------------
                        if constexpr (std::is_same<InputType, uint8_t>::value)
                        {
                            static_assert(std::is_enum<ReturnType>::value, "Return type must be enumeration type");
                            if (auto it = std::find_if(value_package.begin(), value_package.end(), [&](auto &element)
                                                       { return element.first == data; });
                                it != value_package.end())
                            {
                                return it->second;
                            }
                        }
                        if constexpr (std::is_enum<InputType>::value)
                        {
                            static_assert(std::is_same<ReturnType, uint8_t>::value, "Return type must be uint8_t");
                            if (auto it = std::find_if(value_package.begin(), value_package.end(), [&](auto &element)
                                                       { return element.second == data; });
                                it != value_package.end())
                            {
                                return it->first;
                            }
                        }
                    }
                    else
                    {
                        constexpr auto value_package = std::array<std::pair<uint8_t, PropertyCategory>, 5>{
                            std::pair(1, PropertyCategory::unidirectional),
                            std::pair(2, PropertyCategory::bidirectional),
                            std::pair(3, PropertyCategory::bidirectional_ranged),
                            std::pair(6, PropertyCategory::boolean),
                            std::pair(4, PropertyCategory::unknown_6),
                        };
                        //---------------------------------
                        if constexpr (std::is_same<InputType, uint8_t>::value)
                        {
                            static_assert(std::is_enum<ReturnType>::value, "Return type must be enumeration type");
                            if (auto it = std::find_if(value_package.begin(), value_package.end(), [&](auto &element)
                                                       { return element.first == data; });
                                it != value_package.end())
                            {
                                return it->second;
                            }
                        }
                        if constexpr (std::is_enum<InputType>::value)
                        {
                            static_assert(std::is_same<ReturnType, uint8_t>::value, "Return type must be uint8_t");
                            if (auto it = std::find_if(value_package.begin(), value_package.end(), [&](auto &element)
                                                       { return element.second == data; });
                                it != value_package.end())
                            {
                                return it->first;
                            }
                        }
                    }
                }
            }
            if constexpr (std::is_same<InputType, ParameterCategory>::value || std::is_same<ReturnType, ParameterCategory>::value)
            {
                if constexpr (std::is_same<ReturnType, size_t>::value)
                {
                    if (k_version >= 112_ui)
                    {
                        return (k_version < 145_ui) ? 2_size : 3_size;
                    }
                }
                else
                {
                    if (k_version >= 112_ui)
                    {
                        if (k_version < 145_ui)
                        {
                            constexpr auto value_package = std::array<std::pair<uint8_t, ParameterCategory>, 3>{
                                std::pair(0, ParameterCategory::game_parameter),
                                std::pair(1, ParameterCategory::midi_parameter),
                                std::pair(2, ParameterCategory::modulator)};
                            //---------------------------------
                            if constexpr (std::is_same<InputType, uint8_t>::value)
                            {
                                static_assert(std::is_enum<ReturnType>::value, "Return type must be enumeration type");
                                if (auto it = std::find_if(value_package.begin(), value_package.end(), [&](auto &element)
                                                           { return element.first == data; });
                                    it != value_package.end())
                                {
                                    return it->second;
                                }
                            }
                            if constexpr (std::is_enum<InputType>::value)
                            {
                                static_assert(std::is_same<ReturnType, uint8_t>::value, "Return type must be uint8_t");
                                if (auto it = std::find_if(value_package.begin(), value_package.end(), [&](auto &element)
                                                           { return element.second == data; });
                                    it != value_package.end())
                                {
                                    return it->first;
                                }
                            }
                        }
                        else
                        {
                            constexpr auto value_package = std::array<std::pair<uint8_t, ParameterCategory>, 9>{
                                std::pair(0, ParameterCategory::game_parameter),
                                std::pair(1, ParameterCategory::midi_parameter),
                                std::pair(4, ParameterCategory::modulator)};
                            //---------------------------------
                            if constexpr (std::is_same<InputType, uint8_t>::value)
                            {
                                static_assert(std::is_enum<ReturnType>::value, "Return type must be enumeration type");
                                if (auto it = std::find_if(value_package.begin(), value_package.end(), [&](auto &element)
                                                           { return element.first == data; });
                                    it != value_package.end())
                                {
                                    return it->second;
                                }
                            }
                            if constexpr (std::is_enum<InputType>::value)
                            {
                                static_assert(std::is_same<ReturnType, uint8_t>::value, "Return type must be uint8_t");
                                if (auto it = std::find_if(value_package.begin(), value_package.end(), [&](auto &element)
                                                           { return element.second == data; });
                                    it != value_package.end())
                                {
                                    return it->first;
                                }
                            }
                        }
                    }
                }
            }
            if constexpr (std::is_same<InputType, AudioPlayType>::value || std::is_same<ReturnType, AudioPlayType>::value)
            {
                if constexpr (std::is_same<ReturnType, size_t>::value)
                {
                    return 1_size;
                }
                else
                {
                    constexpr auto value_package = std::array<std::pair<uint8_t, AudioPlayType>, 2>{
                        std::pair(0, AudioPlayType::sequence),
                        std::pair(1, AudioPlayType::random)};

                    //---------------------------------
                    if constexpr (std::is_same<InputType, uint8_t>::value)
                    {
                        static_assert(std::is_enum<ReturnType>::value, "Return type must be enumeration type");
                        if (auto it = std::find_if(value_package.begin(), value_package.end(), [&](auto &element)
                                                   { return element.first == data; });
                            it != value_package.end())
                        {
                            return it->second;
                        }
                    }
                    if constexpr (std::is_enum<InputType>::value)
                    {
                        static_assert(std::is_same<ReturnType, uint8_t>::value, "Return type must be uint8_t");
                        if (auto it = std::find_if(value_package.begin(), value_package.end(), [&](auto &element)
                                                   { return element.second == data; });
                            it != value_package.end())
                        {
                            return it->first;
                        }
                    }
                }
            }
            if constexpr (std::is_same<InputType, AudioPlayMode>::value || std::is_same<ReturnType, AudioPlayMode>::value)
            {
                if constexpr (std::is_same<ReturnType, size_t>::value)
                {
                    return 1_size;
                }
                else
                {
                    constexpr auto value_package = std::array<std::pair<uint8_t, AudioPlayMode>, 2>{
                        std::pair(0, AudioPlayMode::step),
                        std::pair(1, AudioPlayMode::continuous)};

                    //---------------------------------
                    if constexpr (std::is_same<InputType, uint8_t>::value)
                    {
                        static_assert(std::is_enum<ReturnType>::value, "Return type must be enumeration type");
                        if (auto it = std::find_if(value_package.begin(), value_package.end(), [&](auto &element)
                                                   { return element.first == data; });
                            it != value_package.end())
                        {
                            return it->second;
                        }
                    }
                    if constexpr (std::is_enum<InputType>::value)
                    {
                        static_assert(std::is_same<ReturnType, uint8_t>::value, "Return type must be uint8_t");
                        if (auto it = std::find_if(value_package.begin(), value_package.end(), [&](auto &element)
                                                   { return element.second == data; });
                            it != value_package.end())
                        {
                            return it->first;
                        }
                    }
                }
            }
            if constexpr (std::is_same<InputType, AudioPositioningSettingListenerRoutingPositionSourceMode>::value || std::is_same<ReturnType, AudioPositioningSettingListenerRoutingPositionSourceMode>::value)
            {
                if constexpr (std::is_same<ReturnType, size_t>::value)
                {
                    return 2_size;
                }
                else
                {
                    if (k_version < 132_ui)
                    {
                        constexpr auto value_package = std::array<std::pair<uint8_t, AudioPositioningSettingListenerRoutingPositionSourceMode>, 2>{
                            std::pair(0, AudioPositioningSettingListenerRoutingPositionSourceMode::user_defined),
                            std::pair(1, AudioPositioningSettingListenerRoutingPositionSourceMode::game_defined)};

                        //---------------------------------
                        if constexpr (std::is_same<InputType, uint8_t>::value)
                        {
                            static_assert(std::is_enum<ReturnType>::value, "Return type must be enumeration type");
                            if (auto it = std::find_if(value_package.begin(), value_package.end(), [&](auto &element)
                                                       { return element.first == data; });
                                it != value_package.end())
                            {
                                return it->second;
                            }
                        }
                        if constexpr (std::is_enum<InputType>::value)
                        {
                            static_assert(std::is_same<ReturnType, uint8_t>::value, "Return type must be uint8_t");
                            if (auto it = std::find_if(value_package.begin(), value_package.end(), [&](auto &element)
                                                       { return element.second == data; });
                                it != value_package.end())
                            {
                                return it->first;
                            }
                        }
                    }
                    else
                    {
                        constexpr auto value_package = std::array<std::pair<uint8_t, AudioPositioningSettingListenerRoutingPositionSourceMode>, 3>{
                            std::pair(0, AudioPositioningSettingListenerRoutingPositionSourceMode::emitter),
                            std::pair(1, AudioPositioningSettingListenerRoutingPositionSourceMode::emitter_with_automation),
                            std::pair(2, AudioPositioningSettingListenerRoutingPositionSourceMode::listener_with_automation)};

                        //---------------------------------
                        if constexpr (std::is_same<InputType, uint8_t>::value)
                        {
                            static_assert(std::is_enum<ReturnType>::value, "Return type must be enumeration type");
                            if (auto it = std::find_if(value_package.begin(), value_package.end(), [&](auto &element)
                                                       { return element.first == data; });
                                it != value_package.end())
                            {
                                return it->second;
                            }
                        }
                        if constexpr (std::is_enum<InputType>::value)
                        {
                            static_assert(std::is_same<ReturnType, uint8_t>::value, "Return type must be uint8_t");
                            if (auto it = std::find_if(value_package.begin(), value_package.end(), [&](auto &element)
                                                       { return element.second == data; });
                                it != value_package.end())
                            {
                                return it->first;
                            }
                        }
                    }
                }
            }
            if constexpr (std::is_same<InputType, AudioPositioningSettingListenerRoutingSpatialization>::value || std::is_same<ReturnType, AudioPositioningSettingListenerRoutingSpatialization>::value)
            {
                if constexpr (std::is_same<ReturnType, size_t>::value)
                {
                    if (k_version >= 128_ui)
                    {
                        return 3_size;
                    }
                }
                else
                {
                    if (k_version >= 128_ui)
                    {
                        constexpr auto value_package = std::array<std::pair<uint8_t, AudioPositioningSettingListenerRoutingSpatialization>, 3>{
                            std::pair(0, AudioPositioningSettingListenerRoutingSpatialization::none),
                            std::pair(1, AudioPositioningSettingListenerRoutingSpatialization::position),
                            std::pair(2, AudioPositioningSettingListenerRoutingSpatialization::position_and_orientation)};

                        //---------------------------------
                        if constexpr (std::is_same<InputType, uint8_t>::value)
                        {
                            static_assert(std::is_enum<ReturnType>::value, "Return type must be enumeration type");
                            if (auto it = std::find_if(value_package.begin(), value_package.end(), [&](auto &element)
                                                       { return element.first == data; });
                                it != value_package.end())
                            {
                                return it->second;
                            }
                        }
                        if constexpr (std::is_enum<InputType>::value)
                        {
                            static_assert(std::is_same<ReturnType, uint8_t>::value, "Return type must be uint8_t");
                            if (auto it = std::find_if(value_package.begin(), value_package.end(), [&](auto &element)
                                                       { return element.second == data; });
                                it != value_package.end())
                            {
                                return it->first;
                            }
                        }
                    }
                }
            }
            if constexpr (std::is_same<InputType, AudioPositioningSettingSpeakerPanningMode>::value || std::is_same<ReturnType, AudioPositioningSettingSpeakerPanningMode>::value)
            {
                if constexpr (std::is_same<ReturnType, size_t>::value)
                {
                    if (k_version >= 132_ui)
                    {
                        return 2_size;
                    }
                }
                else
                {
                    if (k_version >= 132_ui)
                    {
                        if (k_version < 140_ui)
                        {
                            constexpr auto value_package = std::array<std::pair<uint8_t, AudioPositioningSettingSpeakerPanningMode>, 2>{
                                std::pair(0, AudioPositioningSettingSpeakerPanningMode::direct_assignment),
                                std::pair(1, AudioPositioningSettingSpeakerPanningMode::balance_fade)};
                            //---------------------------------
                            if constexpr (std::is_same<InputType, uint8_t>::value)
                            {
                                static_assert(std::is_enum<ReturnType>::value, "Return type must be enumeration type");
                                if (auto it = std::find_if(value_package.begin(), value_package.end(), [&](auto &element)
                                                           { return element.first == data; });
                                    it != value_package.end())
                                {
                                    return it->second;
                                }
                            }
                            if constexpr (std::is_enum<InputType>::value)
                            {
                                static_assert(std::is_same<ReturnType, uint8_t>::value, "Return type must be uint8_t");
                                if (auto it = std::find_if(value_package.begin(), value_package.end(), [&](auto &element)
                                                           { return element.second == data; });
                                    it != value_package.end())
                                {
                                    return it->first;
                                }
                            }
                        }
                        else
                        {
                            constexpr auto value_package = std::array<std::pair<uint8_t, AudioPositioningSettingSpeakerPanningMode>, 3>{
                                std::pair(0, AudioPositioningSettingSpeakerPanningMode::direct_assignment),
                                std::pair(1, AudioPositioningSettingSpeakerPanningMode::balance_fade),
                                std::pair(2, AudioPositioningSettingSpeakerPanningMode::steering)};
                            //---------------------------------
                            if constexpr (std::is_same<InputType, uint8_t>::value)
                            {
                                static_assert(std::is_enum<ReturnType>::value, "Return type must be enumeration type");
                                if (auto it = std::find_if(value_package.begin(), value_package.end(), [&](auto &element)
                                                           { return element.first == data; });
                                    it != value_package.end())
                                {
                                    return it->second;
                                }
                            }
                            if constexpr (std::is_enum<InputType>::value)
                            {
                                static_assert(std::is_same<ReturnType, uint8_t>::value, "Return type must be uint8_t");
                                if (auto it = std::find_if(value_package.begin(), value_package.end(), [&](auto &element)
                                                           { return element.second == data; });
                                    it != value_package.end())
                                {
                                    return it->first;
                                }
                            }
                        }
                    }
                }
            }
            if constexpr (std::is_same<InputType, AudioPositioningSettingType>::value || std::is_same<ReturnType, AudioPositioningSettingType>::value)
            {
                if constexpr (std::is_same<ReturnType, size_t>::value)
                {
                    if (k_version < 132_ui)
                    {
                        return 1_size;
                    }
                }
                else
                {
                    if (k_version < 132_ui)
                    {
                        constexpr auto value_package = std::array<std::pair<uint8_t, AudioPositioningSettingType>, 2>{
                            std::pair(0, AudioPositioningSettingType::two_dimension),
                            std::pair(1, AudioPositioningSettingType::three_dimension)};

                        //---------------------------------
                        if constexpr (std::is_same<InputType, uint8_t>::value)
                        {
                            static_assert(std::is_enum<ReturnType>::value, "Return type must be enumeration type");
                            if (auto it = std::find_if(value_package.begin(), value_package.end(), [&](auto &element)
                                                       { return element.first == data; });
                                it != value_package.end())
                            {
                                return it->second;
                            }
                        }
                        if constexpr (std::is_enum<InputType>::value)
                        {
                            static_assert(std::is_same<ReturnType, uint8_t>::value, "Return type must be uint8_t");
                            if (auto it = std::find_if(value_package.begin(), value_package.end(), [&](auto &element)
                                                       { return element.second == data; });
                                it != value_package.end())
                            {
                                return it->first;
                            }
                        }
                    }
                }
            }
            if constexpr (std::is_same<InputType, BusAutomaticDuckingSettingBusTarget>::value || std::is_same<ReturnType, BusAutomaticDuckingSettingBusTarget>::value)
            {
                if constexpr (std::is_same<ReturnType, size_t>::value)
                {
                    return 3_size;
                }
                else
                {
                    if (k_version < 112_ui)
                    {
                        constexpr auto value_package = std::array<std::pair<uint8_t, BusAutomaticDuckingSettingBusTarget>, 2>{
                            std::pair(0, BusAutomaticDuckingSettingBusTarget::voice_volume),
                            std::pair(4, BusAutomaticDuckingSettingBusTarget::bus_volume)};

                        //---------------------------------
                        if constexpr (std::is_same<InputType, uint8_t>::value)
                        {
                            static_assert(std::is_enum<ReturnType>::value, "Return type must be enumeration type");
                            if (auto it = std::find_if(value_package.begin(), value_package.end(), [&](auto &element)
                                                       { return element.first == data; });
                                it != value_package.end())
                            {
                                return it->second;
                            }
                        }
                        if constexpr (std::is_enum<InputType>::value)
                        {
                            static_assert(std::is_same<ReturnType, uint8_t>::value, "Return type must be uint8_t");
                            if (auto it = std::find_if(value_package.begin(), value_package.end(), [&](auto &element)
                                                       { return element.second == data; });
                                it != value_package.end())
                            {
                                return it->first;
                            }
                        }
                    }
                    else
                    {
                        constexpr auto value_package = std::array<std::pair<uint8_t, BusAutomaticDuckingSettingBusTarget>, 2>{
                            std::pair(0, BusAutomaticDuckingSettingBusTarget::voice_volume),
                            std::pair(5, BusAutomaticDuckingSettingBusTarget::bus_volume)};

                        //---------------------------------
                        if constexpr (std::is_same<InputType, uint8_t>::value)
                        {
                            static_assert(std::is_enum<ReturnType>::value, "Return type must be enumeration type");
                            if (auto it = std::find_if(value_package.begin(), value_package.end(), [&](auto &element)
                                                       { return element.first == data; });
                                it != value_package.end())
                            {
                                return it->second;
                            }
                        }
                        if constexpr (std::is_enum<InputType>::value)
                        {
                            static_assert(std::is_same<ReturnType, uint8_t>::value, "Return type must be uint8_t");
                            if (auto it = std::find_if(value_package.begin(), value_package.end(), [&](auto &element)
                                                       { return element.second == data; });
                                it != value_package.end())
                            {
                                return it->first;
                            }
                        }
                    }
                }
            }
            if constexpr (std::is_same<InputType, MusicTransitionSettingJumpMode>::value || std::is_same<ReturnType, MusicTransitionSettingJumpMode>::value)
            {
                if constexpr (std::is_same<ReturnType, size_t>::value)
                {
                    if (k_version >= 134_ui)
                    {
                        return 2_size;
                    }
                }
                else
                {
                    if (k_version >= 134_ui)
                    {
                        constexpr auto value_package = std::array<std::pair<uint8_t, MusicTransitionSettingJumpMode>, 4>{
                            std::pair(0, MusicTransitionSettingJumpMode::start),
                            std::pair(1, MusicTransitionSettingJumpMode::specific),
                            std::pair(3, MusicTransitionSettingJumpMode::next),
                            std::pair(2, MusicTransitionSettingJumpMode::last_played)};

                        //---------------------------------
                        if constexpr (std::is_same<InputType, uint8_t>::value)
                        {
                            static_assert(std::is_enum<ReturnType>::value, "Return type must be enumeration type");
                            if (auto it = std::find_if(value_package.begin(), value_package.end(), [&](auto &element)
                                                       { return element.first == data; });
                                it != value_package.end())
                            {
                                return it->second;
                            }
                        }
                        if constexpr (std::is_enum<InputType>::value)
                        {
                            static_assert(std::is_same<ReturnType, uint8_t>::value, "Return type must be uint8_t");
                            if (auto it = std::find_if(value_package.begin(), value_package.end(), [&](auto &element)
                                                       { return element.second == data; });
                                it != value_package.end())
                            {
                                return it->first;
                            }
                        }
                    }
                }
            }
            if constexpr (std::is_same<InputType, MusicTransitionSettingSynchronizeMode>::value || std::is_same<ReturnType, MusicTransitionSettingSynchronizeMode>::value)
            {
                if constexpr (std::is_same<ReturnType, size_t>::value)
                {
                    return 2_size;
                }
                else
                {
                    constexpr auto value_package = std::array<std::pair<uint8_t, MusicTransitionSettingSynchronizeMode>, 4>{
                        std::pair(0, MusicTransitionSettingSynchronizeMode::entry_cue),
                        std::pair(2, MusicTransitionSettingSynchronizeMode::random_cue),
                        std::pair(3, MusicTransitionSettingSynchronizeMode::custom_cue),
                        std::pair(1, MusicTransitionSettingSynchronizeMode::same_time_as_playing_segment)};

                    //---------------------------------
                    if constexpr (std::is_same<InputType, uint8_t>::value)
                    {
                        static_assert(std::is_enum<ReturnType>::value, "Return type must be enumeration type");
                        if (auto it = std::find_if(value_package.begin(), value_package.end(), [&](auto &element)
                                                   { return element.first == data; });
                            it != value_package.end())
                        {
                            return it->second;
                        }
                    }
                    if constexpr (std::is_enum<InputType>::value)
                    {
                        static_assert(std::is_same<ReturnType, uint8_t>::value, "Return type must be uint8_t");
                        if (auto it = std::find_if(value_package.begin(), value_package.end(), [&](auto &element)
                                                   { return element.second == data; });
                            it != value_package.end())
                        {
                            return it->first;
                        }
                    }
                }
            }
            if constexpr (std::is_same<InputType, BusHDRSettingDynamicReleaseMode>::value || std::is_same<ReturnType, BusHDRSettingDynamicReleaseMode>::value)
            {
                if constexpr (std::is_same<ReturnType, size_t>::value)
                {
                    if (k_version >= 88_ui)
                    {
                        return 1_size;
                    }
                }
                else
                {
                    if (k_version >= 88_ui)
                    {
                        constexpr auto value_package = std::array<std::pair<uint8_t, BusHDRSettingDynamicReleaseMode>, 2>{
                            std::pair(0, BusHDRSettingDynamicReleaseMode::linear),
                            std::pair(1, BusHDRSettingDynamicReleaseMode::exponential)};

                        //---------------------------------
                        if constexpr (std::is_same<InputType, uint8_t>::value)
                        {
                            static_assert(std::is_enum<ReturnType>::value, "Return type must be enumeration type");
                            if (auto it = std::find_if(value_package.begin(), value_package.end(), [&](auto &element)
                                                       { return element.first == data; });
                                it != value_package.end())
                            {
                                return it->second;
                            }
                        }
                        if constexpr (std::is_enum<InputType>::value)
                        {
                            static_assert(std::is_same<ReturnType, uint8_t>::value, "Return type must be uint8_t");
                            if (auto it = std::find_if(value_package.begin(), value_package.end(), [&](auto &element)
                                                       { return element.second == data; });
                                it != value_package.end())
                            {
                                return it->first;
                            }
                        }
                    }
                }
            }
            if constexpr (std::is_same<InputType, SoundMIDISettingEventPlayOn>::value || std::is_same<ReturnType, SoundMIDISettingEventPlayOn>::value)
            {
                if constexpr (std::is_same<ReturnType, size_t>::value)
                {
                    if (k_version >= 112_ui)
                    {
                        return 2_size;
                    }
                }
                else
                {
                    if (k_version >= 112_ui)
                    {
                        constexpr auto value_package = std::array<std::pair<uint8_t, SoundMIDISettingEventPlayOn>, 2>{
                            std::pair(0, SoundMIDISettingEventPlayOn::note_on),
                            std::pair(2, SoundMIDISettingEventPlayOn::note_off)};

                        //---------------------------------
                        if constexpr (std::is_same<InputType, uint8_t>::value)
                        {
                            static_assert(std::is_enum<ReturnType>::value, "Return type must be enumeration type");
                            if (auto it = std::find_if(value_package.begin(), value_package.end(), [&](auto &element)
                                                       { return element.first == data; });
                                it != value_package.end())
                            {
                                return it->second;
                            }
                        }
                        if constexpr (std::is_enum<InputType>::value)
                        {
                            static_assert(std::is_same<ReturnType, uint8_t>::value, "Return type must be uint8_t");
                            if (auto it = std::find_if(value_package.begin(), value_package.end(), [&](auto &element)
                                                       { return element.second == data; });
                                it != value_package.end())
                            {
                                return it->first;
                            }
                        }
                    }
                }
            }
            if constexpr (std::is_same<InputType, MusicMIDISettingClipTempoSource>::value || std::is_same<ReturnType, MusicMIDISettingClipTempoSource>::value)
            {
                if constexpr (std::is_same<ReturnType, size_t>::value)
                {
                    if (k_version >= 112_ui)
                    {
                        return 1_size;
                    }
                }
                else
                {
                    if (k_version >= 112_ui)
                    {
                        constexpr auto value_package = std::array<std::pair<uint8_t, MusicMIDISettingClipTempoSource>, 2>{
                            std::pair(0, MusicMIDISettingClipTempoSource::hierarchy),
                            std::pair(1, MusicMIDISettingClipTempoSource::file)};

                        //---------------------------------
                        if constexpr (std::is_same<InputType, uint8_t>::value)
                        {
                            static_assert(std::is_enum<ReturnType>::value, "Return type must be enumeration type");
                            if (auto it = std::find_if(value_package.begin(), value_package.end(), [&](auto &element)
                                                       { return element.first == data; });
                                it != value_package.end())
                            {
                                return it->second;
                            }
                        }
                        if constexpr (std::is_enum<InputType>::value)
                        {
                            static_assert(std::is_same<ReturnType, uint8_t>::value, "Return type must be uint8_t");
                            if (auto it = std::find_if(value_package.begin(), value_package.end(), [&](auto &element)
                                                       { return element.second == data; });
                                it != value_package.end())
                            {
                                return it->first;
                            }
                        }
                    }
                }
            }
            if constexpr (std::is_same<InputType, AudioPlaybackLimitSettingScope>::value || std::is_same<ReturnType, AudioPlaybackLimitSettingScope>::value)
            {
                if constexpr (std::is_same<ReturnType, size_t>::value)
                {
                    return 1_size;
                }
                else
                {
                    constexpr auto value_package = std::array<std::pair<uint8_t, AudioPlaybackLimitSettingScope>, 2>{
                        std::pair(0, AudioPlaybackLimitSettingScope::per_game_object),
                        std::pair(1, AudioPlaybackLimitSettingScope::globally)};

                    //---------------------------------
                    if constexpr (std::is_same<InputType, uint8_t>::value)
                    {
                        static_assert(std::is_enum<ReturnType>::value, "Return type must be enumeration type");
                        if (auto it = std::find_if(value_package.begin(), value_package.end(), [&](auto &element)
                                                   { return element.first == data; });
                            it != value_package.end())
                        {
                            return it->second;
                        }
                    }
                    if constexpr (std::is_enum<InputType>::value)
                    {
                        static_assert(std::is_same<ReturnType, uint8_t>::value, "Return type must be uint8_t");
                        if (auto it = std::find_if(value_package.begin(), value_package.end(), [&](auto &element)
                                                   { return element.second == data; });
                            it != value_package.end())
                        {
                            return it->first;
                        }
                    }
                }
            }
            if constexpr (std::is_same<InputType, AudioPlaybackLimitSettingWhenPriorityIsEqual>::value || std::is_same<ReturnType, AudioPlaybackLimitSettingWhenPriorityIsEqual>::value)
            {
                if constexpr (std::is_same<ReturnType, size_t>::value)
                {
                    return 1_size;
                }
                else
                {
                    constexpr auto value_package = std::array<std::pair<uint8_t, AudioPlaybackLimitSettingWhenPriorityIsEqual>, 2>{
                        std::pair(0, AudioPlaybackLimitSettingWhenPriorityIsEqual::discard_oldest_instance),
                        std::pair(1, AudioPlaybackLimitSettingWhenPriorityIsEqual::discard_newest_instance)};

                    //---------------------------------
                    if constexpr (std::is_same<InputType, uint8_t>::value)
                    {
                        static_assert(std::is_enum<ReturnType>::value, "Return type must be enumeration type");
                        if (auto it = std::find_if(value_package.begin(), value_package.end(), [&](auto &element)
                                                   { return element.first == data; });
                            it != value_package.end())
                        {
                            return it->second;
                        }
                    }
                    if constexpr (std::is_enum<InputType>::value)
                    {
                        static_assert(std::is_same<ReturnType, uint8_t>::value, "Return type must be uint8_t");
                        if (auto it = std::find_if(value_package.begin(), value_package.end(), [&](auto &element)
                                                   { return element.second == data; });
                            it != value_package.end())
                        {
                            return it->first;
                        }
                    }
                }
            }
            if constexpr (std::is_same<InputType, AudioPlaybackLimitSettingWhenLimitIsReached>::value || std::is_same<ReturnType, AudioPlaybackLimitSettingWhenLimitIsReached>::value)
            {
                if constexpr (std::is_same<ReturnType, size_t>::value)
                {
                    return 1_size;
                }
                else
                {
                    constexpr auto value_package = std::array<std::pair<uint8_t, AudioPlaybackLimitSettingWhenLimitIsReached>, 2>{
                        std::pair(0, AudioPlaybackLimitSettingWhenLimitIsReached::kill_voice),
                        std::pair(1, AudioPlaybackLimitSettingWhenLimitIsReached::use_virtual_voice_setting)};

                    //---------------------------------
                    if constexpr (std::is_same<InputType, uint8_t>::value)
                    {
                        static_assert(std::is_enum<ReturnType>::value, "Return type must be enumeration type");
                        if (auto it = std::find_if(value_package.begin(), value_package.end(), [&](auto &element)
                                                   { return element.first == data; });
                            it != value_package.end())
                        {
                            return it->second;
                        }
                    }
                    if constexpr (std::is_enum<InputType>::value)
                    {
                        static_assert(std::is_same<ReturnType, uint8_t>::value, "Return type must be uint8_t");
                        if (auto it = std::find_if(value_package.begin(), value_package.end(), [&](auto &element)
                                                   { return element.second == data; });
                            it != value_package.end())
                        {
                            return it->first;
                        }
                    }
                }
            }
            if constexpr (std::is_same<InputType, AudioVirtualVoiceSettingBehavior>::value || std::is_same<ReturnType, AudioVirtualVoiceSettingBehavior>::value)
            {
                if constexpr (std::is_same<ReturnType, size_t>::value)
                {
                    return 2_size;
                }
                else
                {
                    if (k_version < 140_ui)
                    {
                        constexpr auto value_package = std::array<std::pair<uint8_t, AudioVirtualVoiceSettingBehavior>, 3>{
                            std::pair(0, AudioVirtualVoiceSettingBehavior::continue_to_play),
                            std::pair(1, AudioVirtualVoiceSettingBehavior::kill_voice),
                            std::pair(2, AudioVirtualVoiceSettingBehavior::send_to_virtual_voice)};

                        //---------------------------------
                        if constexpr (std::is_same<InputType, uint8_t>::value)
                        {
                            static_assert(std::is_enum<ReturnType>::value, "Return type must be enumeration type");
                            if (auto it = std::find_if(value_package.begin(), value_package.end(), [&](auto &element)
                                                       { return element.first == data; });
                                it != value_package.end())
                            {
                                return it->second;
                            }
                        }
                        if constexpr (std::is_enum<InputType>::value)
                        {
                            static_assert(std::is_same<ReturnType, uint8_t>::value, "Return type must be uint8_t");
                            if (auto it = std::find_if(value_package.begin(), value_package.end(), [&](auto &element)
                                                       { return element.second == data; });
                                it != value_package.end())
                            {
                                return it->first;
                            }
                        }
                    }
                    else
                    {
                        constexpr auto value_package = std::array<std::pair<uint8_t, AudioVirtualVoiceSettingBehavior>, 4>{
                            std::pair(0, AudioVirtualVoiceSettingBehavior::continue_to_play),
                            std::pair(1, AudioVirtualVoiceSettingBehavior::kill_voice),
                            std::pair(2, AudioVirtualVoiceSettingBehavior::send_to_virtual_voice),
                            std::pair(2, AudioVirtualVoiceSettingBehavior::kill_if_finite_else_virtual)};

                        //---------------------------------
                        if constexpr (std::is_same<InputType, uint8_t>::value)
                        {
                            static_assert(std::is_enum<ReturnType>::value, "Return type must be enumeration type");
                            if (auto it = std::find_if(value_package.begin(), value_package.end(), [&](auto &element)
                                                       { return element.first == data; });
                                it != value_package.end())
                            {
                                return it->second;
                            }
                        }
                        if constexpr (std::is_enum<InputType>::value)
                        {
                            static_assert(std::is_same<ReturnType, uint8_t>::value, "Return type must be uint8_t");
                            if (auto it = std::find_if(value_package.begin(), value_package.end(), [&](auto &element)
                                                       { return element.second == data; });
                                it != value_package.end())
                            {
                                return it->first;
                            }
                        }
                    }
                }
            }
            if constexpr (std::is_same<InputType, AudioVirtualVoiceSettingOnReturnToPhysical>::value || std::is_same<ReturnType, AudioVirtualVoiceSettingOnReturnToPhysical>::value)
            {
                if constexpr (std::is_same<ReturnType, size_t>::value)
                {
                    return 2_size;
                }
                else
                {
                    constexpr auto value_package = std::array<std::pair<uint8_t, AudioVirtualVoiceSettingOnReturnToPhysical>, 3>{
                        std::pair(0, AudioVirtualVoiceSettingOnReturnToPhysical::play_from_beginning),
                        std::pair(1, AudioVirtualVoiceSettingOnReturnToPhysical::play_from_elapsed_time),
                        std::pair(2, AudioVirtualVoiceSettingOnReturnToPhysical::resume)};

                    //---------------------------------
                    if constexpr (std::is_same<InputType, uint8_t>::value)
                    {
                        static_assert(std::is_enum<ReturnType>::value, "Return type must be enumeration type");
                        if (auto it = std::find_if(value_package.begin(), value_package.end(), [&](auto &element)
                                                   { return element.first == data; });
                            it != value_package.end())
                        {
                            return it->second;
                        }
                    }
                    if constexpr (std::is_enum<InputType>::value)
                    {
                        static_assert(std::is_same<ReturnType, uint8_t>::value, "Return type must be uint8_t");
                        if (auto it = std::find_if(value_package.begin(), value_package.end(), [&](auto &element)
                                                   { return element.second == data; });
                            it != value_package.end())
                        {
                            return it->first;
                        }
                    }
                }
            }
            if constexpr (std::is_same<InputType, AudioAssociationSettingMode>::value || std::is_same<ReturnType, AudioAssociationSettingMode>::value)
            {
                if constexpr (std::is_same<ReturnType, size_t>::value)
                {
                    return 1_size;
                }
                else
                {
                    constexpr auto value_package = std::array<std::pair<uint8_t, AudioAssociationSettingMode>, 2>{
                        std::pair(0, AudioAssociationSettingMode::best_match),
                        std::pair(1, AudioAssociationSettingMode::weighted)};

                    //---------------------------------
                    if constexpr (std::is_same<InputType, uint8_t>::value)
                    {
                        static_assert(std::is_enum<ReturnType>::value, "Return type must be enumeration type");
                        if (auto it = std::find_if(value_package.begin(), value_package.end(), [&](auto &element)
                                                   { return element.first == data; });
                            it != value_package.end())
                        {
                            return it->second;
                        }
                    }
                    if constexpr (std::is_enum<InputType>::value)
                    {
                        static_assert(std::is_same<ReturnType, uint8_t>::value, "Return type must be uint8_t");
                        if (auto it = std::find_if(value_package.begin(), value_package.end(), [&](auto &element)
                                                   { return element.second == data; });
                            it != value_package.end())
                        {
                            return it->first;
                        }
                    }
                }
            }
            if constexpr (std::is_same<InputType, AudioPlayTypeRandomType>::value || std::is_same<ReturnType, AudioPlayTypeRandomType>::value)
            {
                if constexpr (std::is_same<ReturnType, size_t>::value)
                {
                    return 1_size;
                }
                else
                {
                    constexpr auto value_package = std::array<std::pair<uint8_t, AudioPlayTypeRandomType>, 2>{
                        std::pair(0, AudioPlayTypeRandomType::standard),
                        std::pair(1, AudioPlayTypeRandomType::shuffle)};

                    //---------------------------------
                    if constexpr (std::is_same<InputType, uint8_t>::value)
                    {
                        static_assert(std::is_enum<ReturnType>::value, "Return type must be enumeration type");
                        if (auto it = std::find_if(value_package.begin(), value_package.end(), [&](auto &element)
                                                   { return element.first == data; });
                            it != value_package.end())
                        {
                            return it->second;
                        }
                    }
                    if constexpr (std::is_enum<InputType>::value)
                    {
                        static_assert(std::is_same<ReturnType, uint8_t>::value, "Return type must be uint8_t");
                        if (auto it = std::find_if(value_package.begin(), value_package.end(), [&](auto &element)
                                                   { return element.second == data; });
                            it != value_package.end())
                        {
                            return it->first;
                        }
                    }
                }
            }
            if constexpr (std::is_same<InputType, AudioPlayTypeSequenceAtEndOfPlaylist>::value || std::is_same<ReturnType, AudioPlayTypeSequenceAtEndOfPlaylist>::value)
            {
                if constexpr (std::is_same<ReturnType, size_t>::value)
                {
                    return 1_size;
                }
                else
                {
                    constexpr auto value_package = std::array<std::pair<uint8_t, AudioPlayTypeSequenceAtEndOfPlaylist>, 2>{
                        std::pair(0, AudioPlayTypeSequenceAtEndOfPlaylist::restart),
                        std::pair(1, AudioPlayTypeSequenceAtEndOfPlaylist::play_in_reserve_order)};

                    //---------------------------------
                    if constexpr (std::is_same<InputType, uint8_t>::value)
                    {
                        static_assert(std::is_enum<ReturnType>::value, "Return type must be enumeration type");
                        if (auto it = std::find_if(value_package.begin(), value_package.end(), [&](auto &element)
                                                   { return element.first == data; });
                            it != value_package.end())
                        {
                            return it->second;
                        }
                    }
                    if constexpr (std::is_enum<InputType>::value)
                    {
                        static_assert(std::is_same<ReturnType, uint8_t>::value, "Return type must be uint8_t");
                        if (auto it = std::find_if(value_package.begin(), value_package.end(), [&](auto &element)
                                                   { return element.second == data; });
                            it != value_package.end())
                        {
                            return it->first;
                        }
                    }
                }
            }
            if constexpr (std::is_same<InputType, AudioPlayModeContinuousTransitionType>::value || std::is_same<ReturnType, AudioPlayModeContinuousTransitionType>::value)
            {
                if constexpr (std::is_same<ReturnType, size_t>::value)
                {
                    return 3_size;
                }
                else
                {
                    constexpr auto value_package = std::array<std::pair<uint8_t, AudioPlayModeContinuousTransitionType>, 6>{
                        std::pair(0, AudioPlayModeContinuousTransitionType::none),
                        std::pair(1, AudioPlayModeContinuousTransitionType::xfade_amp),
                        std::pair(2, AudioPlayModeContinuousTransitionType::xfade_power),
                        std::pair(3, AudioPlayModeContinuousTransitionType::delay),
                        std::pair(4, AudioPlayModeContinuousTransitionType::sample_accurate),
                        std::pair(5, AudioPlayModeContinuousTransitionType::trigger_rate)};

                    //---------------------------------
                    if constexpr (std::is_same<InputType, uint8_t>::value)
                    {
                        static_assert(std::is_enum<ReturnType>::value, "Return type must be enumeration type");
                        if (auto it = std::find_if(value_package.begin(), value_package.end(), [&](auto &element)
                                                   { return element.first == data; });
                            it != value_package.end())
                        {
                            return it->second;
                        }
                    }
                    if constexpr (std::is_enum<InputType>::value)
                    {
                        static_assert(std::is_same<ReturnType, uint8_t>::value, "Return type must be uint8_t");
                        if (auto it = std::find_if(value_package.begin(), value_package.end(), [&](auto &element)
                                                   { return element.second == data; });
                            it != value_package.end())
                        {
                            return it->first;
                        }
                    }
                }
            }
            if constexpr (std::is_same<InputType, GameParameterBindToBuiltInParameterMode>::value || std::is_same<ReturnType, GameParameterBindToBuiltInParameterMode>::value)
            {
                if constexpr (std::is_same<ReturnType, size_t>::value)
                {
                    if (k_version >= 112_ui)
                    {
                        return 3_size;
                    }
                }
                else
                {
                    if (k_version >= 112_ui)
                    {
                        if (k_version < 128_ui)
                        {
                            constexpr auto value_package = std::array<std::pair<uint8_t, GameParameterBindToBuiltInParameterMode>, 7>{
                                std::pair(0, GameParameterBindToBuiltInParameterMode::none),
                                std::pair(1, GameParameterBindToBuiltInParameterMode::distance),
                                std::pair(2, GameParameterBindToBuiltInParameterMode::azimuth),
                                std::pair(3, GameParameterBindToBuiltInParameterMode::elevation),
                                std::pair(4, GameParameterBindToBuiltInParameterMode::object_to_listener_angle),
                                std::pair(5, GameParameterBindToBuiltInParameterMode::obstruction),
                                std::pair(6, GameParameterBindToBuiltInParameterMode::occlusion)};

                            //---------------------------------
                            if constexpr (std::is_same<InputType, uint8_t>::value)
                            {
                                static_assert(std::is_enum<ReturnType>::value, "Return type must be enumeration type");
                                if (auto it = std::find_if(value_package.begin(), value_package.end(), [&](auto &element)
                                                           { return element.first == data; });
                                    it != value_package.end())
                                {
                                    return it->second;
                                }
                            }
                            if constexpr (std::is_enum<InputType>::value)
                            {
                                static_assert(std::is_same<ReturnType, uint8_t>::value, "Return type must be uint8_t");
                                if (auto it = std::find_if(value_package.begin(), value_package.end(), [&](auto &element)
                                                           { return element.second == data; });
                                    it != value_package.end())
                                {
                                    return it->first;
                                }
                            }
                        }
                        else
                        {
                            constexpr auto value_package = std::array<std::pair<uint8_t, GameParameterBindToBuiltInParameterMode>, 9>{
                                std::pair(0, GameParameterBindToBuiltInParameterMode::none),
                                std::pair(1, GameParameterBindToBuiltInParameterMode::distance),
                                std::pair(2, GameParameterBindToBuiltInParameterMode::azimuth),
                                std::pair(3, GameParameterBindToBuiltInParameterMode::elevation),
                                std::pair(4, GameParameterBindToBuiltInParameterMode::emitter_cone),
                                std::pair(5, GameParameterBindToBuiltInParameterMode::obstruction),
                                std::pair(6, GameParameterBindToBuiltInParameterMode::occlusion),
                                std::pair(7, GameParameterBindToBuiltInParameterMode::listener_cone),
                                std::pair(8, GameParameterBindToBuiltInParameterMode::diffraction)};

                            //---------------------------------
                            if constexpr (std::is_same<InputType, uint8_t>::value)
                            {
                                static_assert(std::is_enum<ReturnType>::value, "Return type must be enumeration type");
                                if (auto it = std::find_if(value_package.begin(), value_package.end(), [&](auto &element)
                                                           { return element.first == data; });
                                    it != value_package.end())
                                {
                                    return it->second;
                                }
                            }
                            if constexpr (std::is_enum<InputType>::value)
                            {
                                static_assert(std::is_same<ReturnType, uint8_t>::value, "Return type must be uint8_t");
                                if (auto it = std::find_if(value_package.begin(), value_package.end(), [&](auto &element)
                                                           { return element.second == data; });
                                    it != value_package.end())
                                {
                                    return it->first;
                                }
                            }
                        }
                    }
                }
            }
            if constexpr (std::is_same<InputType, GameParameterInterpolationMode>::value || std::is_same<ReturnType, GameParameterInterpolationMode>::value)
            {
                if constexpr (std::is_same<ReturnType, size_t>::value)
                {
                    if (k_version >= 112_ui)
                    {
                        return 2_size;
                    }
                }
                else
                {
                    if (k_version >= 112_ui)
                    {
                        constexpr auto value_package = std::array<std::pair<uint8_t, GameParameterInterpolationMode>, 3>{
                            std::pair(0, GameParameterInterpolationMode::none),
                            std::pair(1, GameParameterInterpolationMode::slew_rate),
                            std::pair(2, GameParameterInterpolationMode::filtering_over_time)};

                        //---------------------------------
                        if constexpr (std::is_same<InputType, uint8_t>::value)
                        {
                            static_assert(std::is_enum<ReturnType>::value, "Return type must be enumeration type");
                            if (auto it = std::find_if(value_package.begin(), value_package.end(), [&](auto &element)
                                                       { return element.first == data; });
                                it != value_package.end())
                            {
                                return it->second;
                            }
                        }
                        if constexpr (std::is_enum<InputType>::value)
                        {
                            static_assert(std::is_same<ReturnType, uint8_t>::value, "Return type must be uint8_t");
                            if (auto it = std::find_if(value_package.begin(), value_package.end(), [&](auto &element)
                                                       { return element.second == data; });
                                it != value_package.end())
                            {
                                return it->first;
                            }
                        }
                    }
                }
            }

            if constexpr (std::is_same<InputType, EventActionProperty::ValueApplyMode>::value || std::is_same<ReturnType, EventActionProperty::ValueApplyMode>::value)
            {
                if constexpr (std::is_same<ReturnType, size_t>::value)
                {
                    return 2_size;
                }
                else
                {
                    constexpr auto value_package = std::array<std::pair<uint8_t, EventActionProperty::ValueApplyMode>, 2>{
                        std::pair(1, EventActionProperty::ValueApplyMode::absolute),
                        std::pair(2, EventActionProperty::ValueApplyMode::relative)};

                    //---------------------------------
                    if constexpr (std::is_same<InputType, uint8_t>::value)
                    {
                        static_assert(std::is_enum<ReturnType>::value, "Return type must be enumeration type");
                        if (auto it = std::find_if(value_package.begin(), value_package.end(), [&](auto &element)
                                                   { return element.first == data; });
                            it != value_package.end())
                        {
                            return it->second;
                        }
                    }
                    if constexpr (std::is_enum<InputType>::value)
                    {
                        static_assert(std::is_same<ReturnType, uint8_t>::value, "Return type must be uint8_t");
                        if (auto it = std::find_if(value_package.begin(), value_package.end(), [&](auto &element)
                                                   { return element.second == data; });
                            it != value_package.end())
                        {
                            return it->first;
                        }
                    }
                }
            }
            if constexpr (std::is_same<InputType, EventActionProperty::SeekType>::value || std::is_same<ReturnType, EventActionProperty::SeekType>::value)
            {
                if constexpr (std::is_same<ReturnType, size_t>::value)
                {
                    return 1_size;
                }
                else
                {
                    constexpr auto value_package = std::array<std::pair<uint8_t, EventActionProperty::SeekType>, 2>{
                        std::pair(0, EventActionProperty::SeekType::time),
                        std::pair(1, EventActionProperty::SeekType::percent)};

                    //---------------------------------
                    if constexpr (std::is_same<InputType, uint8_t>::value)
                    {
                        static_assert(std::is_enum<ReturnType>::value, "Return type must be enumeration type");
                        if (auto it = std::find_if(value_package.begin(), value_package.end(), [&](auto &element)
                                                   { return element.first == data; });
                            it != value_package.end())
                        {
                            return it->second;
                        }
                    }
                    if constexpr (std::is_enum<InputType>::value)
                    {
                        static_assert(std::is_same<ReturnType, uint8_t>::value, "Return type must be uint8_t");
                        if (auto it = std::find_if(value_package.begin(), value_package.end(), [&](auto &element)
                                                   { return element.second == data; });
                            it != value_package.end())
                        {
                            return it->first;
                        }
                    }
                }
            }

            if constexpr (std::is_same<InputType, EventActionMode>::value || std::is_same<ReturnType, EventActionMode>::value)
            {
                if constexpr (std::is_same<ReturnType, size_t>::value)
                {
                    if (k_version < 125_ui)
                    {
                        return 3_size;
                    }
                    else
                    {
                        return 2_size;
                    }
                }
                else
                {
                    if (k_version < 125_ui)
                    {
                        constexpr auto value_package = std::array<std::pair<uint8_t, EventActionMode>, 4>{
                            std::pair(0, EventActionMode::none),
                            std::pair(1, EventActionMode::one),
                            std::pair(2, EventActionMode::all),
                            std::pair(4, EventActionMode::all_except)};

                        //---------------------------------
                        if constexpr (std::is_same<InputType, uint8_t>::value)
                        {
                            static_assert(std::is_enum<ReturnType>::value, "Return type must be enumeration type");
                            if (auto it = std::find_if(value_package.begin(), value_package.end(), [&](auto &element)
                                                       { return element.first == data; });
                                it != value_package.end())
                            {
                                return it->second;
                            }
                        }
                        if constexpr (std::is_enum<InputType>::value)
                        {
                            static_assert(std::is_same<ReturnType, uint8_t>::value, "Return type must be uint8_t");
                            if (auto it = std::find_if(value_package.begin(), value_package.end(), [&](auto &element)
                                                       { return element.second == data; });
                                it != value_package.end())
                            {
                                return it->first;
                            }
                        }
                    }
                    else
                    {
                        constexpr auto value_package = std::array<std::pair<uint8_t, EventActionMode>, 3>{
                            std::pair(0, EventActionMode::none),
                            std::pair(1, EventActionMode::one),
                            std::pair(2, EventActionMode::all)};

                        //---------------------------------
                        if constexpr (std::is_same<InputType, uint8_t>::value)
                        {
                            static_assert(std::is_enum<ReturnType>::value, "Return type must be enumeration type");
                            if (auto it = std::find_if(value_package.begin(), value_package.end(), [&](auto &element)
                                                       { return element.first == data; });
                                it != value_package.end())
                            {
                                return it->second;
                            }
                        }
                        if constexpr (std::is_enum<InputType>::value)
                        {
                            static_assert(std::is_same<ReturnType, uint8_t>::value, "Return type must be uint8_t");
                            if (auto it = std::find_if(value_package.begin(), value_package.end(), [&](auto &element)
                                                       { return element.second == data; });
                                it != value_package.end())
                            {
                                return it->first;
                            }
                        }
                    }
                }
            }
            if constexpr (std::is_same<InputType, EventActionScope>::value || std::is_same<ReturnType, EventActionScope>::value)
            {
                if constexpr (std::is_same<ReturnType, size_t>::value)
                {
                    return 1_size;
                }
                else
                {
                    constexpr auto value_package = std::array<std::pair<uint8_t, EventActionScope>, 2>{
                        std::pair(0, EventActionScope::global),
                        std::pair(1, EventActionScope::game_object)};

                    //---------------------------------
                    if constexpr (std::is_same<InputType, uint8_t>::value)
                    {
                        static_assert(std::is_enum<ReturnType>::value, "Return type must be enumeration type");
                        if (auto it = std::find_if(value_package.begin(), value_package.end(), [&](auto &element)
                                                   { return element.first == data; });
                            it != value_package.end())
                        {
                            return it->second;
                        }
                    }
                    if constexpr (std::is_enum<InputType>::value)
                    {
                        static_assert(std::is_same<ReturnType, uint8_t>::value, "Return type must be uint8_t");
                        if (auto it = std::find_if(value_package.begin(), value_package.end(), [&](auto &element)
                                                   { return element.second == data; });
                            it != value_package.end())
                        {
                            return it->first;
                        }
                    }
                }
            }
            if constexpr (std::is_same<InputType, ModulatorScope>::value || std::is_same<ReturnType, ModulatorScope>::value)
            {
                if constexpr (std::is_same<ReturnType, size_t>::value)
                {
                    if (k_version >= 112_ui)
                    {
                        return 2_size;
                    }
                }
                else
                {
                    if (k_version >= 112_ui)
                    {
                        constexpr auto value_package = std::array<std::pair<uint8_t, ModulatorScope>, 4>{
                            std::pair(0, ModulatorScope::voice),
                            std::pair(1, ModulatorScope::note_or_event),
                            std::pair(2, ModulatorScope::game_object),
                            std::pair(3, ModulatorScope::global)};

                        //---------------------------------
                        if constexpr (std::is_same<InputType, uint8_t>::value)
                        {
                            static_assert(std::is_enum<ReturnType>::value, "Return type must be enumeration type");
                            if (auto it = std::find_if(value_package.begin(), value_package.end(), [&](auto &element)
                                                       { return element.first == data; });
                                it != value_package.end())
                            {
                                return it->second;
                            }
                        }
                        if constexpr (std::is_enum<InputType>::value)
                        {
                            static_assert(std::is_same<ReturnType, uint8_t>::value, "Return type must be uint8_t");
                            if (auto it = std::find_if(value_package.begin(), value_package.end(), [&](auto &element)
                                                       { return element.second == data; });
                                it != value_package.end())
                            {
                                return it->first;
                            }
                        }
                    }
                }
            }
            if constexpr (std::is_same<InputType, ModulatorTriggerOn>::value || std::is_same<ReturnType, ModulatorTriggerOn>::value)
            {
                if constexpr (std::is_same<ReturnType, size_t>::value)
                {
                    if (k_version >= 112_ui)
                    {
                        return 2_size;
                    }
                }
                else
                {
                    if (k_version >= 112_ui)
                    {
                        constexpr auto value_package = std::array<std::pair<uint8_t, ModulatorTriggerOn>, 2>{
                            std::pair(0, ModulatorTriggerOn::play),
                            std::pair(2, ModulatorTriggerOn::note_off)};

                        //---------------------------------
                        if constexpr (std::is_same<InputType, uint8_t>::value)
                        {
                            static_assert(std::is_enum<ReturnType>::value, "Return type must be enumeration type");
                            if (auto it = std::find_if(value_package.begin(), value_package.end(), [&](auto &element)
                                                       { return element.first == data; });
                                it != value_package.end())
                            {
                                return it->second;
                            }
                        }
                        if constexpr (std::is_enum<InputType>::value)
                        {
                            static_assert(std::is_same<ReturnType, uint8_t>::value, "Return type must be uint8_t");
                            if (auto it = std::find_if(value_package.begin(), value_package.end(), [&](auto &element)
                                                       { return element.second == data; });
                                it != value_package.end())
                            {
                                return it->first;
                            }
                        }
                    }
                }
            }
            if constexpr (std::is_same<InputType, ModulatorWaveform>::value || std::is_same<ReturnType, ModulatorWaveform>::value)
            {
                if constexpr (std::is_same<ReturnType, size_t>::value)
                {
                    if (k_version >= 112_ui)
                    {
                        return 3_size;
                    }
                }
                else
                {
                    if (k_version >= 112_ui)
                    {
                        if (k_version < 125_ui)
                        {
                            constexpr auto value_package = std::array<std::pair<uint8_t, ModulatorWaveform>, 5>{
                                std::pair(0, ModulatorWaveform::sine),
                                std::pair(1, ModulatorWaveform::triangle),
                                std::pair(2, ModulatorWaveform::square),
                                std::pair(3, ModulatorWaveform::saw_up),
                                std::pair(4, ModulatorWaveform::saw_down)};

                            //---------------------------------
                            if constexpr (std::is_same<InputType, uint8_t>::value)
                            {
                                static_assert(std::is_enum<ReturnType>::value, "Return type must be enumeration type");
                                if (auto it = std::find_if(value_package.begin(), value_package.end(), [&](auto &element)
                                                           { return element.first == data; });
                                    it != value_package.end())
                                {
                                    return it->second;
                                }
                            }
                            if constexpr (std::is_enum<InputType>::value)
                            {
                                static_assert(std::is_same<ReturnType, uint8_t>::value, "Return type must be uint8_t");
                                if (auto it = std::find_if(value_package.begin(), value_package.end(), [&](auto &element)
                                                           { return element.second == data; });
                                    it != value_package.end())
                                {
                                    return it->first;
                                }
                            }
                        }
                        else
                        {
                            constexpr auto value_package = std::array<std::pair<uint8_t, ModulatorWaveform>, 6>{
                                std::pair(0, ModulatorWaveform::sine),
                                std::pair(1, ModulatorWaveform::triangle),
                                std::pair(2, ModulatorWaveform::square),
                                std::pair(3, ModulatorWaveform::saw_up),
                                std::pair(4, ModulatorWaveform::saw_down),
                                std::pair(5, ModulatorWaveform::random)};

                            //---------------------------------
                            if constexpr (std::is_same<InputType, uint8_t>::value)
                            {
                                static_assert(std::is_enum<ReturnType>::value, "Return type must be enumeration type");
                                if (auto it = std::find_if(value_package.begin(), value_package.end(), [&](auto &element)
                                                           { return element.first == data; });
                                    it != value_package.end())
                                {
                                    return it->second;
                                }
                            }
                            if constexpr (std::is_enum<InputType>::value)
                            {
                                static_assert(std::is_same<ReturnType, uint8_t>::value, "Return type must be uint8_t");
                                if (auto it = std::find_if(value_package.begin(), value_package.end(), [&](auto &element)
                                                           { return element.second == data; });
                                    it != value_package.end())
                                {
                                    return it->first;
                                }
                            }
                        }
                    }
                }
            }
            if constexpr (std::is_same<InputType, SoundPlaylistContainerScope>::value || std::is_same<ReturnType, SoundPlaylistContainerScope>::value)
            {
                if constexpr (std::is_same<ReturnType, size_t>::value)
                {
                    return 1_size;
                }
                else
                {
                        constexpr auto value_package = std::array<std::pair<uint8_t, SoundPlaylistContainerScope>, 2>{
                            std::pair(0, SoundPlaylistContainerScope::game_object),
                            std::pair(1, SoundPlaylistContainerScope::global)};

                        //---------------------------------
                        if constexpr (std::is_same<InputType, uint8_t>::value)
                        {
                            static_assert(std::is_enum<ReturnType>::value, "Return type must be enumeration type");
                            if (auto it = std::find_if(value_package.begin(), value_package.end(), [&](auto &element)
                                                       { return element.first == data; });
                                it != value_package.end())
                            {
                                return it->second;
                            }
                        }
                        if constexpr (std::is_enum<InputType>::value)
                        {
                            static_assert(std::is_same<ReturnType, uint8_t>::value, "Return type must be uint8_t");
                            if (auto it = std::find_if(value_package.begin(), value_package.end(), [&](auto &element)
                                                       { return element.second == data; });
                                it != value_package.end())
                            {
                                return it->first;
                            }
                        }
                }
            }
            if constexpr (std::is_same<InputType, VoiceFilterBehavior>::value || std::is_same<ReturnType, VoiceFilterBehavior>::value)
            {
                if constexpr (std::is_same<ReturnType, size_t>::value)
                {
                    if (k_version >= 145_ui) {
                        return 1_size;
                    }
                }
                else
                {
                    if (k_version >= 145_ui) {
                        constexpr auto value_package = std::array<std::pair<uint8_t, VoiceFilterBehavior>, 2>{
                            std::pair(0, VoiceFilterBehavior::sum_all_value),
                            std::pair(1, VoiceFilterBehavior::use_highest_value)};

                        //---------------------------------
                        if constexpr (std::is_same<InputType, uint8_t>::value)
                        {
                            static_assert(std::is_enum<ReturnType>::value, "Return type must be enumeration type");
                            if (auto it = std::find_if(value_package.begin(), value_package.end(), [&](auto &element)
                                                       { return element.first == data; });
                                it != value_package.end())
                            {
                                return it->second;
                            }
                        }
                        if constexpr (std::is_enum<InputType>::value)
                        {
                            static_assert(std::is_same<ReturnType, uint8_t>::value, "Return type must be uint8_t");
                            if (auto it = std::find_if(value_package.begin(), value_package.end(), [&](auto &element)
                                                       { return element.second == data; });
                                it != value_package.end())
                            {
                                return it->first;
                            }
                        }
                    }
                }
            }
            if constexpr (std::is_same<InputType, MusicTrackTrackType>::value || std::is_same<ReturnType, MusicTrackTrackType>::value)
            {
                if constexpr (std::is_same<ReturnType, size_t>::value)
                {
                    return 2_size;
                }
                else
                {
                    if (k_version < 112_ui)
                    {
                        constexpr auto value_package = std::array<std::pair<uint8_t, MusicTrackTrackType>, 3>{
                            std::pair(0, MusicTrackTrackType::normal),
                            std::pair(1, MusicTrackTrackType::random_step),
                            std::pair(2, MusicTrackTrackType::sequence_step)};

                        //---------------------------------
                        if constexpr (std::is_same<InputType, uint8_t>::value)
                        {
                            static_assert(std::is_enum<ReturnType>::value, "Return type must be enumeration type");
                            if (auto it = std::find_if(value_package.begin(), value_package.end(), [&](auto &element)
                                                       { return element.first == data; });
                                it != value_package.end())
                            {
                                return it->second;
                            }
                        }
                        if constexpr (std::is_enum<InputType>::value)
                        {
                            static_assert(std::is_same<ReturnType, uint8_t>::value, "Return type must be uint8_t");
                            if (auto it = std::find_if(value_package.begin(), value_package.end(), [&](auto &element)
                                                       { return element.second == data; });
                                it != value_package.end())
                            {
                                return it->first;
                            }
                        }
                    }
                    else
                    {
                        constexpr auto value_package = std::array<std::pair<uint8_t, MusicTrackTrackType>, 4>{
                            std::pair(0, MusicTrackTrackType::normal),
                            std::pair(1, MusicTrackTrackType::random_step),
                            std::pair(2, MusicTrackTrackType::sequence_step),
                            std::pair(3, MusicTrackTrackType::switcher)};

                        //---------------------------------
                        if constexpr (std::is_same<InputType, uint8_t>::value)
                        {
                            static_assert(std::is_enum<ReturnType>::value, "Return type must be enumeration type");
                            if (auto it = std::find_if(value_package.begin(), value_package.end(), [&](auto &element)
                                                       { return element.first == data; });
                                it != value_package.end())
                            {
                                return it->second;
                            }
                        }
                        if constexpr (std::is_enum<InputType>::value)
                        {
                            static_assert(std::is_same<ReturnType, uint8_t>::value, "Return type must be uint8_t");
                            if (auto it = std::find_if(value_package.begin(), value_package.end(), [&](auto &element)
                                                       { return element.second == data; });
                                it != value_package.end())
                            {
                                return it->first;
                            }
                        }
                    }
                }
            }
            if constexpr (std::is_same<InputType, HierarchyType>::value || std::is_same<ReturnType, HierarchyType>::value)
            {
                if constexpr (std::is_same<ReturnType, size_t>::value)
                {
                    return 8_size;
                }
                else
                {
                    if (k_version < 112_ui)
                    {
                        constexpr auto value_package = std::array<std::pair<uint8_t, HierarchyType>, 19>{
                            std::pair(0, HierarchyType::unknown),
                            std::pair(1, HierarchyType::stateful_property_setting),
                            std::pair(3, HierarchyType::event_action),
                            std::pair(4, HierarchyType::event),
                            std::pair(15, HierarchyType::dialogue_event),
                            std::pair(14, HierarchyType::attenuation),
                            std::pair(18, HierarchyType::effect),
                            std::pair(19, HierarchyType::source),
                            std::pair(8, HierarchyType::audio_bus),
                            std::pair(20, HierarchyType::auxiliary_audio_bus),
                            std::pair(2, HierarchyType::sound),
                            std::pair(5, HierarchyType::sound_playlist_container),
                            std::pair(6, HierarchyType::sound_switch_container),
                            std::pair(9, HierarchyType::sound_blend_container),
                            std::pair(7, HierarchyType::actor_mixer),
                            std::pair(11, HierarchyType::music_track),
                            std::pair(10, HierarchyType::music_segment),
                            std::pair(13, HierarchyType::music_playlist_container),
                            std::pair(12, HierarchyType::music_switch_container)};

                        //---------------------------------
                        if constexpr (std::is_same<InputType, uint8_t>::value)
                        {
                            static_assert(std::is_enum<ReturnType>::value, "Return type must be enumeration type");
                            if (auto it = std::find_if(value_package.begin(), value_package.end(), [&](auto &element)
                                                       { return element.first == data; });
                                it != value_package.end())
                            {
                                return it->second;
                            }
                        }
                        if constexpr (std::is_enum<InputType>::value)
                        {
                            static_assert(std::is_same<ReturnType, uint8_t>::value, "Return type must be uint8_t");
                            if (auto it = std::find_if(value_package.begin(), value_package.end(), [&](auto &element)
                                                       { return element.second == data; });
                                it != value_package.end())
                            {
                                return it->first;
                            }
                        }
                    }
                    else if (k_version < 128_ui)
                    {
                        constexpr auto value_package = std::array<std::pair<uint8_t, HierarchyType>, 21>{
                            std::pair(0, HierarchyType::unknown),
                            std::pair(1, HierarchyType::stateful_property_setting),
                            std::pair(3, HierarchyType::event_action),
                            std::pair(4, HierarchyType::event),
                            std::pair(15, HierarchyType::dialogue_event),
                            std::pair(14, HierarchyType::attenuation),
                            std::pair(21, HierarchyType::low_frequency_oscillator_modulator),
                            std::pair(22, HierarchyType::envelope_modulator),
                            std::pair(18, HierarchyType::effect),
                            std::pair(19, HierarchyType::source),
                            std::pair(8, HierarchyType::audio_bus),
                            std::pair(20, HierarchyType::auxiliary_audio_bus),
                            std::pair(2, HierarchyType::sound),
                            std::pair(5, HierarchyType::sound_playlist_container),
                            std::pair(6, HierarchyType::sound_switch_container),
                            std::pair(9, HierarchyType::sound_blend_container),
                            std::pair(7, HierarchyType::actor_mixer),
                            std::pair(11, HierarchyType::music_track),
                            std::pair(10, HierarchyType::music_segment),
                            std::pair(13, HierarchyType::music_playlist_container),
                            std::pair(12, HierarchyType::music_switch_container)};

                        //---------------------------------
                        if constexpr (std::is_same<InputType, uint8_t>::value)
                        {
                            static_assert(std::is_enum<ReturnType>::value, "Return type must be enumeration type");
                            if (auto it = std::find_if(value_package.begin(), value_package.end(), [&](auto &element)
                                                       { return element.first == data; });
                                it != value_package.end())
                            {
                                return it->second;
                            }
                        }
                        if constexpr (std::is_enum<InputType>::value)
                        {
                            static_assert(std::is_same<ReturnType, uint8_t>::value, "Return type must be uint8_t");
                            if (auto it = std::find_if(value_package.begin(), value_package.end(), [&](auto &element)
                                                       { return element.second == data; });
                                it != value_package.end())
                            {
                                return it->first;
                            }
                        }
                    }
                    else if (k_version < 132_ui)
                    {
                        constexpr auto value_package = std::array<std::pair<uint8_t, HierarchyType>, 22>{
                            std::pair(0, HierarchyType::unknown),
                            std::pair(1, HierarchyType::stateful_property_setting),
                            std::pair(3, HierarchyType::event_action),
                            std::pair(4, HierarchyType::event),
                            std::pair(15, HierarchyType::dialogue_event),
                            std::pair(14, HierarchyType::attenuation),
                            std::pair(19, HierarchyType::low_frequency_oscillator_modulator),
                            std::pair(20, HierarchyType::envelope_modulator),
                            std::pair(16, HierarchyType::effect),
                            std::pair(17, HierarchyType::source),
                            std::pair(21, HierarchyType::audio_device),
                            std::pair(8, HierarchyType::audio_bus),
                            std::pair(18, HierarchyType::auxiliary_audio_bus),
                            std::pair(2, HierarchyType::sound),
                            std::pair(5, HierarchyType::sound_playlist_container),
                            std::pair(6, HierarchyType::sound_switch_container),
                            std::pair(9, HierarchyType::sound_blend_container),
                            std::pair(7, HierarchyType::actor_mixer),
                            std::pair(11, HierarchyType::music_track),
                            std::pair(10, HierarchyType::music_segment),
                            std::pair(13, HierarchyType::music_playlist_container),
                            std::pair(12, HierarchyType::music_switch_container)};

                        //---------------------------------
                        if constexpr (std::is_same<InputType, uint8_t>::value)
                        {
                            static_assert(std::is_enum<ReturnType>::value, "Return type must be enumeration type");
                            if (auto it = std::find_if(value_package.begin(), value_package.end(), [&](auto &element)
                                                       { return element.first == data; });
                                it != value_package.end())
                            {
                                return it->second;
                            }
                        }
                        if constexpr (std::is_enum<InputType>::value)
                        {
                            static_assert(std::is_same<ReturnType, uint8_t>::value, "Return type must be uint8_t");
                            if (auto it = std::find_if(value_package.begin(), value_package.end(), [&](auto &element)
                                                       { return element.second == data; });
                                it != value_package.end())
                            {
                                return it->first;
                            }
                        }
                    }
                    else
                    {
                        constexpr auto value_package = std::array<std::pair<uint8_t, HierarchyType>, 23>{
                            std::pair(0, HierarchyType::unknown),
                            std::pair(1, HierarchyType::stateful_property_setting),
                            std::pair(3, HierarchyType::event_action),
                            std::pair(4, HierarchyType::event),
                            std::pair(15, HierarchyType::dialogue_event),
                            std::pair(14, HierarchyType::attenuation),
                            std::pair(19, HierarchyType::low_frequency_oscillator_modulator),
                            std::pair(20, HierarchyType::envelope_modulator),
                            std::pair(22, HierarchyType::time_modulator),
                            std::pair(16, HierarchyType::effect),
                            std::pair(17, HierarchyType::source),
                            std::pair(21, HierarchyType::audio_device),
                            std::pair(8, HierarchyType::audio_bus),
                            std::pair(18, HierarchyType::auxiliary_audio_bus),
                            std::pair(2, HierarchyType::sound),
                            std::pair(5, HierarchyType::sound_playlist_container),
                            std::pair(6, HierarchyType::sound_switch_container),
                            std::pair(9, HierarchyType::sound_blend_container),
                            std::pair(7, HierarchyType::actor_mixer),
                            std::pair(11, HierarchyType::music_track),
                            std::pair(10, HierarchyType::music_segment),
                            std::pair(13, HierarchyType::music_playlist_container),
                            std::pair(12, HierarchyType::music_switch_container)};

                        //---------------------------------
                        if constexpr (std::is_same<InputType, uint8_t>::value)
                        {
                            static_assert(std::is_enum<ReturnType>::value, "Return type must be enumeration type");
                            if (auto it = std::find_if(value_package.begin(), value_package.end(), [&](auto &element)
                                                       { return element.first == data; });
                                it != value_package.end())
                            {
                                return it->second;
                            }
                        }
                        if constexpr (std::is_enum<InputType>::value)
                        {
                            static_assert(std::is_same<ReturnType, uint8_t>::value, "Return type must be uint8_t");
                            if (auto it = std::find_if(value_package.begin(), value_package.end(), [&](auto &element)
                                                       { return element.second == data; });
                                it != value_package.end())
                            {
                                return it->first;
                            }
                        }
                    }
                }
            }

            //------------------------------
            // CommonPropertyType
            if constexpr (std::is_same<InputType, EventActionCommonPropertyType>::value || std::is_same<ReturnType, EventActionCommonPropertyType>::value)
            {
                if constexpr (std::is_same<ReturnType, size_t>::value)
                {
                    try_assert(false, "common property type is not return size");
                }
                else
                {
                    if (k_version < 118_ui)
                    {
                        constexpr auto value_package = std::array<std::pair<uint8_t, EventActionCommonPropertyType>, 3>{
                            std::pair(14, EventActionCommonPropertyType::delay),
                            std::pair(15, EventActionCommonPropertyType::fade_time),
                            std::pair(16, EventActionCommonPropertyType::probability)};

                        //---------------------------------
                        if constexpr (std::is_same<InputType, uint8_t>::value)
                        {
                            static_assert(std::is_enum<ReturnType>::value, "Return type must be enumeration type");
                            if (auto it = std::find_if(value_package.begin(), value_package.end(), [&](auto &element)
                                                       { return element.first == data; });
                                it != value_package.end())
                            {
                                return it->second;
                            }
                        }
                        if constexpr (std::is_enum<InputType>::value)
                        {
                            static_assert(std::is_same<ReturnType, uint8_t>::value, "Return type must be uint8_t");
                            if (auto it = std::find_if(value_package.begin(), value_package.end(), [&](auto &element)
                                                       { return element.second == data; });
                                it != value_package.end())
                            {
                                return it->first;
                            }
                        }
                    }
                    else if (k_version < 150_ui)
                    {
                        constexpr auto value_package = std::array<std::pair<uint8_t, EventActionCommonPropertyType>, 3>{
                            std::pair(15, EventActionCommonPropertyType::delay),
                            std::pair(16, EventActionCommonPropertyType::fade_time),
                            std::pair(17, EventActionCommonPropertyType::probability)};

                        //---------------------------------
                        if constexpr (std::is_same<InputType, uint8_t>::value)
                        {
                            static_assert(std::is_enum<ReturnType>::value, "Return type must be enumeration type");
                            if (auto it = std::find_if(value_package.begin(), value_package.end(), [&](auto &element)
                                                       { return element.first == data; });
                                it != value_package.end())
                            {
                                return it->second;
                            }
                        }
                        if constexpr (std::is_enum<InputType>::value)
                        {
                            static_assert(std::is_same<ReturnType, uint8_t>::value, "Return type must be uint8_t");
                            if (auto it = std::find_if(value_package.begin(), value_package.end(), [&](auto &element)
                                                       { return element.second == data; });
                                it != value_package.end())
                            {
                                return it->first;
                            }
                        }
                    }
                    else
                    {
                        constexpr auto value_package = std::array<std::pair<uint8_t, EventActionCommonPropertyType>, 3>{
                            std::pair(57, EventActionCommonPropertyType::delay),
                            std::pair(58, EventActionCommonPropertyType::fade_time),
                            std::pair(59, EventActionCommonPropertyType::probability)};

                        //---------------------------------
                        if constexpr (std::is_same<InputType, uint8_t>::value)
                        {
                            static_assert(std::is_enum<ReturnType>::value, "Return type must be enumeration type");
                            if (auto it = std::find_if(value_package.begin(), value_package.end(), [&](auto &element)
                                                       { return element.first == data; });
                                it != value_package.end())
                            {
                                return it->second;
                            }
                        }
                        if constexpr (std::is_enum<InputType>::value)
                        {
                            static_assert(std::is_same<ReturnType, uint8_t>::value, "Return type must be uint8_t");
                            if (auto it = std::find_if(value_package.begin(), value_package.end(), [&](auto &element)
                                                       { return element.second == data; });
                                it != value_package.end())
                            {
                                return it->first;
                            }
                        }
                    }
                }
            }
            if constexpr (std::is_same<InputType, ModulatorCommonPropertyType>::value || std::is_same<ReturnType, ModulatorCommonPropertyType>::value)
            {
                if constexpr (std::is_same<ReturnType, size_t>::value)
                {
                    if (k_version >= 112_ui)
                    {
                        try_assert(false, "common property type is not return size");
                    }
                }
                else
                {
                    if (k_version >= 112_ui)
                    {
                        if (k_version < 150_ui)
                        {
                            constexpr auto value_package = std::array<std::pair<uint8_t, ModulatorCommonPropertyType>, 20>{
                                std::pair(0, ModulatorCommonPropertyType::scope),
                                std::pair(15, ModulatorCommonPropertyType::trigger_on),
                                std::pair(2, ModulatorCommonPropertyType::depth),
                                std::pair(4, ModulatorCommonPropertyType::frequency),
                                std::pair(5, ModulatorCommonPropertyType::waveform),
                                std::pair(6, ModulatorCommonPropertyType::smoothing),
                                std::pair(7, ModulatorCommonPropertyType::pulse_width_modulation),
                                std::pair(3, ModulatorCommonPropertyType::attack),
                                std::pair(8, ModulatorCommonPropertyType::initial_phase_offset),
                                std::pair(9, ModulatorCommonPropertyType::attack_time),
                                std::pair(10, ModulatorCommonPropertyType::attack_curve),
                                std::pair(11, ModulatorCommonPropertyType::decay_time),
                                std::pair(12, ModulatorCommonPropertyType::sustain_level),
                                std::pair(14, ModulatorCommonPropertyType::release_time),
                                std::pair(13, ModulatorCommonPropertyType::sustain_time),
                                std::pair(19, ModulatorCommonPropertyType::initial_delay),
                                std::pair(16, ModulatorCommonPropertyType::duration),
                                std::pair(17, ModulatorCommonPropertyType::loop),
                                std::pair(18, ModulatorCommonPropertyType::playback_rate),
                                std::pair(1, ModulatorCommonPropertyType::stop_playback)};

                            //---------------------------------
                            if constexpr (std::is_same<InputType, uint8_t>::value)
                            {
                                static_assert(std::is_enum<ReturnType>::value, "Return type must be enumeration type");
                                if (auto it = std::find_if(value_package.begin(), value_package.end(), [&](auto &element)
                                                           { return element.first == data; });
                                    it != value_package.end())
                                {
                                    return it->second;
                                }
                            }
                            if constexpr (std::is_enum<InputType>::value)
                            {
                                static_assert(std::is_same<ReturnType, uint8_t>::value, "Return type must be uint8_t");
                                if (auto it = std::find_if(value_package.begin(), value_package.end(), [&](auto &element)
                                                           { return element.second == data; });
                                    it != value_package.end())
                                {
                                    return it->first;
                                }
                            }
                        }
                        else
                        {
                            constexpr auto value_package = std::array<std::pair<uint8_t, ModulatorCommonPropertyType>, 20>{
                                std::pair(0, ModulatorCommonPropertyType::scope),
                                std::pair(16, ModulatorCommonPropertyType::trigger_on),
                                std::pair(2, ModulatorCommonPropertyType::depth),
                                std::pair(4, ModulatorCommonPropertyType::frequency),
                                std::pair(5, ModulatorCommonPropertyType::waveform),
                                std::pair(6, ModulatorCommonPropertyType::smoothing),
                                std::pair(7, ModulatorCommonPropertyType::pulse_width_modulation),
                                std::pair(3, ModulatorCommonPropertyType::attack),
                                std::pair(8, ModulatorCommonPropertyType::initial_phase_offset),
                                std::pair(10, ModulatorCommonPropertyType::attack_time),
                                std::pair(11, ModulatorCommonPropertyType::attack_curve),
                                std::pair(12, ModulatorCommonPropertyType::decay_time),
                                std::pair(13, ModulatorCommonPropertyType::sustain_level),
                                std::pair(15, ModulatorCommonPropertyType::release_time),
                                std::pair(14, ModulatorCommonPropertyType::sustain_time),
                                std::pair(20, ModulatorCommonPropertyType::initial_delay),
                                std::pair(17, ModulatorCommonPropertyType::duration),
                                std::pair(18, ModulatorCommonPropertyType::loop),
                                std::pair(19, ModulatorCommonPropertyType::playback_rate),
                                std::pair(1, ModulatorCommonPropertyType::stop_playback)};

                            //---------------------------------
                            if constexpr (std::is_same<InputType, uint8_t>::value)
                            {
                                static_assert(std::is_enum<ReturnType>::value, "Return type must be enumeration type");
                                if (auto it = std::find_if(value_package.begin(), value_package.end(), [&](auto &element)
                                                           { return element.first == data; });
                                    it != value_package.end())
                                {
                                    return it->second;
                                }
                            }
                            if constexpr (std::is_enum<InputType>::value)
                            {
                                static_assert(std::is_same<ReturnType, uint8_t>::value, "Return type must be uint8_t");
                                if (auto it = std::find_if(value_package.begin(), value_package.end(), [&](auto &element)
                                                           { return element.second == data; });
                                    it != value_package.end())
                                {
                                    return it->first;
                                }
                            }
                        }
                    }
                }
            }
            if constexpr (std::is_same<InputType, AudioCommonPropertyType>::value || std::is_same<ReturnType, AudioCommonPropertyType>::value)
            {
                if constexpr (std::is_same<ReturnType, size_t>::value)
                {
                    try_assert(false, "common property type is not return size");
                }
                else
                {
                    if (k_version < 88_ui)
                    {
                        constexpr auto value_package = std::array<std::pair<uint8_t, AudioCommonPropertyType>, 19>{
                            std::pair(4, AudioCommonPropertyType::bus_volume),
                            std::pair(23, AudioCommonPropertyType::output_bus_volume),
                            std::pair(24, AudioCommonPropertyType::output_bus_low_pass_filter),
                            std::pair(0, AudioCommonPropertyType::voice_volume),
                            std::pair(2, AudioCommonPropertyType::voice_pitch),
                            std::pair(3, AudioCommonPropertyType::voice_low_pass_filter),
                            std::pair(22, AudioCommonPropertyType::game_defined_auxiliary_send_volume),
                            std::pair(18, AudioCommonPropertyType::user_defined_auxiliary_send_volume_0),
                            std::pair(19, AudioCommonPropertyType::user_defined_auxiliary_send_volume_1),
                            std::pair(20, AudioCommonPropertyType::user_defined_auxiliary_send_volume_2),
                            std::pair(21, AudioCommonPropertyType::user_defined_auxiliary_send_volume_3),
                            std::pair(13, AudioCommonPropertyType::positioning_center_percent),
                            std::pair(11, AudioCommonPropertyType::positioning_speaker_panning_x),
                            std::pair(12, AudioCommonPropertyType::positioning_speaker_panning_y),
                            std::pair(5, AudioCommonPropertyType::playback_priority_value),
                            std::pair(6, AudioCommonPropertyType::playback_priority_offset_at_maximum_distance),
                            std::pair(7, AudioCommonPropertyType::playback_loop),
                            std::pair(8, AudioCommonPropertyType::motion_volume_offset),
                            std::pair(9, AudioCommonPropertyType::motion_low_pass_filter)};

                        //---------------------------------
                        if constexpr (std::is_same<InputType, uint8_t>::value)
                        {
                            static_assert(std::is_enum<ReturnType>::value, "Return type must be enumeration type");
                            if (auto it = std::find_if(value_package.begin(), value_package.end(), [&](auto &element)
                                                       { return element.first == data; });
                                it != value_package.end())
                            {
                                return it->second;
                            }
                        }
                        if constexpr (std::is_enum<InputType>::value)
                        {
                            static_assert(std::is_same<ReturnType, uint8_t>::value, "Return type must be uint8_t");
                            if (auto it = std::find_if(value_package.begin(), value_package.end(), [&](auto &element)
                                                       { return element.second == data; });
                                it != value_package.end())
                            {
                                return it->first;
                            }
                        }
                    }
                    else if (k_version < 112_ui)
                    {
                        constexpr auto value_package = std::array<std::pair<uint8_t, AudioCommonPropertyType>, 28>{
                            std::pair(4, AudioCommonPropertyType::bus_volume),
                            std::pair(23, AudioCommonPropertyType::output_bus_volume),
                            std::pair(24, AudioCommonPropertyType::output_bus_low_pass_filter),
                            std::pair(0, AudioCommonPropertyType::voice_volume),
                            std::pair(2, AudioCommonPropertyType::voice_pitch),
                            std::pair(3, AudioCommonPropertyType::voice_low_pass_filter),
                            std::pair(33, AudioCommonPropertyType::voice_volume_make_up_gain),
                            std::pair(22, AudioCommonPropertyType::game_defined_auxiliary_send_volume),
                            std::pair(18, AudioCommonPropertyType::user_defined_auxiliary_send_volume_0),
                            std::pair(19, AudioCommonPropertyType::user_defined_auxiliary_send_volume_1),
                            std::pair(20, AudioCommonPropertyType::user_defined_auxiliary_send_volume_2),
                            std::pair(21, AudioCommonPropertyType::user_defined_auxiliary_send_volume_3),
                            std::pair(13, AudioCommonPropertyType::positioning_center_percent),
                            std::pair(11, AudioCommonPropertyType::positioning_speaker_panning_x),
                            std::pair(12, AudioCommonPropertyType::positioning_speaker_panning_y),
                            std::pair(26, AudioCommonPropertyType::hdr_threshold),
                            std::pair(27, AudioCommonPropertyType::hdr_ratio),
                            std::pair(28, AudioCommonPropertyType::hdr_release_time),
                            std::pair(29, AudioCommonPropertyType::hdr_window_tap_output_game_parameter_id),
                            std::pair(30, AudioCommonPropertyType::hdr_window_tap_output_game_parameter_minimum),
                            std::pair(31, AudioCommonPropertyType::hdr_window_tap_output_game_parameter_maximum),
                            std::pair(32, AudioCommonPropertyType::hdr_envelope_tracking_active_range),
                            std::pair(5, AudioCommonPropertyType::playback_priority_value),
                            std::pair(6, AudioCommonPropertyType::playback_priority_offset_at_maximum_distance),
                            std::pair(25, AudioCommonPropertyType::playback_initial_delay),
                            std::pair(7, AudioCommonPropertyType::playback_loop),
                            std::pair(8, AudioCommonPropertyType::motion_volume_offset),
                            std::pair(9, AudioCommonPropertyType::motion_low_pass_filter)};

                        //---------------------------------
                        if constexpr (std::is_same<InputType, uint8_t>::value)
                        {
                            static_assert(std::is_enum<ReturnType>::value, "Return type must be enumeration type");
                            if (auto it = std::find_if(value_package.begin(), value_package.end(), [&](auto &element)
                                                       { return element.first == data; });
                                it != value_package.end())
                            {
                                return it->second;
                            }
                        }
                        if constexpr (std::is_enum<InputType>::value)
                        {
                            static_assert(std::is_same<ReturnType, uint8_t>::value, "Return type must be uint8_t");
                            if (auto it = std::find_if(value_package.begin(), value_package.end(), [&](auto &element)
                                                       { return element.second == data; });
                                it != value_package.end())
                            {
                                return it->first;
                            }
                        }
                    }
                    else if (k_version < 118_ui)
                    {
                        constexpr auto value_package = std::array<std::pair<uint8_t, AudioCommonPropertyType>, 43>{
                            std::pair(5, AudioCommonPropertyType::bus_volume),
                            std::pair(23, AudioCommonPropertyType::output_bus_volume),
                            std::pair(25, AudioCommonPropertyType::output_bus_low_pass_filter),
                            std::pair(24, AudioCommonPropertyType::output_bus_high_pass_filter),
                            std::pair(0, AudioCommonPropertyType::voice_volume),
                            std::pair(2, AudioCommonPropertyType::voice_pitch),
                            std::pair(3, AudioCommonPropertyType::voice_low_pass_filter),
                            std::pair(4, AudioCommonPropertyType::voice_high_pass_filter),
                            std::pair(33, AudioCommonPropertyType::voice_volume_make_up_gain),
                            std::pair(22, AudioCommonPropertyType::game_defined_auxiliary_send_volume),
                            std::pair(18, AudioCommonPropertyType::user_defined_auxiliary_send_volume_0),
                            std::pair(19, AudioCommonPropertyType::user_defined_auxiliary_send_volume_1),
                            std::pair(20, AudioCommonPropertyType::user_defined_auxiliary_send_volume_2),
                            std::pair(21, AudioCommonPropertyType::user_defined_auxiliary_send_volume_3),
                            std::pair(13, AudioCommonPropertyType::positioning_center_percent),
                            std::pair(11, AudioCommonPropertyType::positioning_speaker_panning_x),
                            std::pair(12, AudioCommonPropertyType::positioning_speaker_panning_y),
                            std::pair(26, AudioCommonPropertyType::hdr_threshold),
                            std::pair(27, AudioCommonPropertyType::hdr_ratio),
                            std::pair(28, AudioCommonPropertyType::hdr_release_time),
                            std::pair(29, AudioCommonPropertyType::hdr_window_tap_output_game_parameter_id),
                            std::pair(30, AudioCommonPropertyType::hdr_window_tap_output_game_parameter_minimum),
                            std::pair(31, AudioCommonPropertyType::hdr_window_tap_output_game_parameter_maximum),
                            std::pair(32, AudioCommonPropertyType::hdr_envelope_tracking_active_range),
                            std::pair(45, AudioCommonPropertyType::midi_note_tracking_root_note),
                            std::pair(46, AudioCommonPropertyType::midi_event_play_on),
                            std::pair(47, AudioCommonPropertyType::midi_transformation_transposition),
                            std::pair(48, AudioCommonPropertyType::midi_transformation_velocity_offset),
                            std::pair(49, AudioCommonPropertyType::midi_filter_key_range_minimum),
                            std::pair(50, AudioCommonPropertyType::midi_filter_key_range_maximum),
                            std::pair(51, AudioCommonPropertyType::midi_filter_velocity_minimum),
                            std::pair(52, AudioCommonPropertyType::midi_filter_velocity_maximum),
                            std::pair(53, AudioCommonPropertyType::midi_filter_channel),
                            std::pair(55, AudioCommonPropertyType::midi_clip_tempo_source),
                            std::pair(56, AudioCommonPropertyType::midi_target_id),
                            std::pair(6, AudioCommonPropertyType::playback_priority_value),
                            std::pair(7, AudioCommonPropertyType::playback_priority_offset_at_maximum_distance),
                            std::pair(59, AudioCommonPropertyType::playback_initial_delay),
                            std::pair(58, AudioCommonPropertyType::playback_loop),
                            std::pair(54, AudioCommonPropertyType::playback_speed),
                            std::pair(8, AudioCommonPropertyType::motion_volume_offset),
                            std::pair(9, AudioCommonPropertyType::motion_low_pass_filter),
                            std::pair(57, AudioCommonPropertyType::mixer_id)};

                        //---------------------------------
                        if constexpr (std::is_same<InputType, uint8_t>::value)
                        {
                            static_assert(std::is_enum<ReturnType>::value, "Return type must be enumeration type");
                            if (auto it = std::find_if(value_package.begin(), value_package.end(), [&](auto &element)
                                                       { return element.first == data; });
                                it != value_package.end())
                            {
                                return it->second;
                            }
                        }
                        if constexpr (std::is_enum<InputType>::value)
                        {
                            static_assert(std::is_same<ReturnType, uint8_t>::value, "Return type must be uint8_t");
                            if (auto it = std::find_if(value_package.begin(), value_package.end(), [&](auto &element)
                                                       { return element.second == data; });
                                it != value_package.end())
                            {
                                return it->first;
                            }
                        }
                    }
                    else if (k_version < 128_ui)
                    {
                        constexpr auto value_package = std::array<std::pair<uint8_t, AudioCommonPropertyType>, 43>{
                            std::pair(5, AudioCommonPropertyType::bus_volume),
                            std::pair(24, AudioCommonPropertyType::output_bus_volume),
                            std::pair(26, AudioCommonPropertyType::output_bus_low_pass_filter),
                            std::pair(25, AudioCommonPropertyType::output_bus_high_pass_filter),
                            std::pair(0, AudioCommonPropertyType::voice_volume),
                            std::pair(2, AudioCommonPropertyType::voice_pitch),
                            std::pair(3, AudioCommonPropertyType::voice_low_pass_filter),
                            std::pair(4, AudioCommonPropertyType::voice_high_pass_filter),
                            std::pair(6, AudioCommonPropertyType::voice_volume_make_up_gain),
                            std::pair(23, AudioCommonPropertyType::game_defined_auxiliary_send_volume),
                            std::pair(19, AudioCommonPropertyType::user_defined_auxiliary_send_volume_0),
                            std::pair(20, AudioCommonPropertyType::user_defined_auxiliary_send_volume_1),
                            std::pair(21, AudioCommonPropertyType::user_defined_auxiliary_send_volume_2),
                            std::pair(22, AudioCommonPropertyType::user_defined_auxiliary_send_volume_3),
                            std::pair(14, AudioCommonPropertyType::positioning_center_percent),
                            std::pair(12, AudioCommonPropertyType::positioning_speaker_panning_x),
                            std::pair(13, AudioCommonPropertyType::positioning_speaker_panning_y),
                            std::pair(27, AudioCommonPropertyType::hdr_threshold),
                            std::pair(28, AudioCommonPropertyType::hdr_ratio),
                            std::pair(29, AudioCommonPropertyType::hdr_release_time),
                            std::pair(30, AudioCommonPropertyType::hdr_window_tap_output_game_parameter_id),
                            std::pair(31, AudioCommonPropertyType::hdr_window_tap_output_game_parameter_minimum),
                            std::pair(32, AudioCommonPropertyType::hdr_window_tap_output_game_parameter_maximum),
                            std::pair(33, AudioCommonPropertyType::hdr_envelope_tracking_active_range),
                            std::pair(45, AudioCommonPropertyType::midi_note_tracking_root_note),
                            std::pair(46, AudioCommonPropertyType::midi_event_play_on),
                            std::pair(47, AudioCommonPropertyType::midi_transformation_transposition),
                            std::pair(48, AudioCommonPropertyType::midi_transformation_velocity_offset),
                            std::pair(49, AudioCommonPropertyType::midi_filter_key_range_minimum),
                            std::pair(50, AudioCommonPropertyType::midi_filter_key_range_maximum),
                            std::pair(51, AudioCommonPropertyType::midi_filter_velocity_minimum),
                            std::pair(52, AudioCommonPropertyType::midi_filter_velocity_maximum),
                            std::pair(53, AudioCommonPropertyType::midi_filter_channel),
                            std::pair(55, AudioCommonPropertyType::midi_clip_tempo_source),
                            std::pair(56, AudioCommonPropertyType::midi_target_id),
                            std::pair(7, AudioCommonPropertyType::playback_priority_value),
                            std::pair(8, AudioCommonPropertyType::playback_priority_offset_at_maximum_distance),
                            std::pair(59, AudioCommonPropertyType::playback_initial_delay),
                            std::pair(58, AudioCommonPropertyType::playback_loop),
                            std::pair(54, AudioCommonPropertyType::playback_speed),
                            std::pair(9, AudioCommonPropertyType::motion_volume_offset),
                            std::pair(10, AudioCommonPropertyType::motion_low_pass_filter),
                            std::pair(57, AudioCommonPropertyType::mixer_id)};

                        //---------------------------------
                        if constexpr (std::is_same<InputType, uint8_t>::value)
                        {
                            static_assert(std::is_enum<ReturnType>::value, "Return type must be enumeration type");
                            if (auto it = std::find_if(value_package.begin(), value_package.end(), [&](auto &element)
                                                       { return element.first == data; });
                                it != value_package.end())
                            {
                                return it->second;
                            }
                        }
                        if constexpr (std::is_enum<InputType>::value)
                        {
                            static_assert(std::is_same<ReturnType, uint8_t>::value, "Return type must be uint8_t");
                            if (auto it = std::find_if(value_package.begin(), value_package.end(), [&](auto &element)
                                                       { return element.second == data; });
                                it != value_package.end())
                            {
                                return it->first;
                            }
                        }
                    }
                    else if (k_version < 132_ui)
                    {
                        constexpr auto value_package = std::array<std::pair<uint8_t, AudioCommonPropertyType>, 51>{
                            std::pair(5, AudioCommonPropertyType::bus_volume),
                            std::pair(24, AudioCommonPropertyType::output_bus_volume),
                            std::pair(26, AudioCommonPropertyType::output_bus_low_pass_filter),
                            std::pair(25, AudioCommonPropertyType::output_bus_high_pass_filter),
                            std::pair(0, AudioCommonPropertyType::voice_volume),
                            std::pair(2, AudioCommonPropertyType::voice_pitch),
                            std::pair(3, AudioCommonPropertyType::voice_low_pass_filter),
                            std::pair(4, AudioCommonPropertyType::voice_high_pass_filter),
                            std::pair(6, AudioCommonPropertyType::voice_volume_make_up_gain),
                            std::pair(23, AudioCommonPropertyType::game_defined_auxiliary_send_volume),
                            std::pair(68, AudioCommonPropertyType::game_defined_auxiliary_send_low_pass_filter),
                            std::pair(69, AudioCommonPropertyType::game_defined_auxiliary_send_high_pass_filter),
                            std::pair(19, AudioCommonPropertyType::user_defined_auxiliary_send_volume_0),
                            std::pair(20, AudioCommonPropertyType::user_defined_auxiliary_send_volume_1),
                            std::pair(21, AudioCommonPropertyType::user_defined_auxiliary_send_volume_2),
                            std::pair(22, AudioCommonPropertyType::user_defined_auxiliary_send_volume_3),
                            std::pair(60, AudioCommonPropertyType::user_defined_auxiliary_send_low_pass_filter_0),
                            std::pair(61, AudioCommonPropertyType::user_defined_auxiliary_send_low_pass_filter_1),
                            std::pair(62, AudioCommonPropertyType::user_defined_auxiliary_send_low_pass_filter_2),
                            std::pair(63, AudioCommonPropertyType::user_defined_auxiliary_send_low_pass_filter_3),
                            std::pair(64, AudioCommonPropertyType::user_defined_auxiliary_send_high_pass_filter_0),
                            std::pair(65, AudioCommonPropertyType::user_defined_auxiliary_send_high_pass_filter_1),
                            std::pair(66, AudioCommonPropertyType::user_defined_auxiliary_send_high_pass_filter_2),
                            std::pair(67, AudioCommonPropertyType::user_defined_auxiliary_send_high_pass_filter_3),
                            std::pair(14, AudioCommonPropertyType::positioning_center_percent),
                            std::pair(12, AudioCommonPropertyType::positioning_speaker_panning_x),
                            std::pair(13, AudioCommonPropertyType::positioning_speaker_panning_y),
                            std::pair(27, AudioCommonPropertyType::hdr_threshold),
                            std::pair(28, AudioCommonPropertyType::hdr_ratio),
                            std::pair(29, AudioCommonPropertyType::hdr_release_time),
                            std::pair(30, AudioCommonPropertyType::hdr_window_tap_output_game_parameter_id),
                            std::pair(31, AudioCommonPropertyType::hdr_window_tap_output_game_parameter_minimum),
                            std::pair(32, AudioCommonPropertyType::hdr_window_tap_output_game_parameter_maximum),
                            std::pair(33, AudioCommonPropertyType::hdr_envelope_tracking_active_range),
                            std::pair(45, AudioCommonPropertyType::midi_note_tracking_root_note),
                            std::pair(46, AudioCommonPropertyType::midi_event_play_on),
                            std::pair(47, AudioCommonPropertyType::midi_transformation_transposition),
                            std::pair(48, AudioCommonPropertyType::midi_transformation_velocity_offset),
                            std::pair(49, AudioCommonPropertyType::midi_filter_key_range_minimum),
                            std::pair(50, AudioCommonPropertyType::midi_filter_key_range_maximum),
                            std::pair(51, AudioCommonPropertyType::midi_filter_velocity_minimum),
                            std::pair(52, AudioCommonPropertyType::midi_filter_velocity_maximum),
                            std::pair(53, AudioCommonPropertyType::midi_filter_channel),
                            std::pair(55, AudioCommonPropertyType::midi_clip_tempo_source),
                            std::pair(56, AudioCommonPropertyType::midi_target_id),
                            std::pair(7, AudioCommonPropertyType::playback_priority_value),
                            std::pair(8, AudioCommonPropertyType::playback_priority_offset_at_maximum_distance),
                            std::pair(59, AudioCommonPropertyType::playback_initial_delay),
                            std::pair(58, AudioCommonPropertyType::playback_loop),
                            std::pair(54, AudioCommonPropertyType::playback_speed),
                            std::pair(57, AudioCommonPropertyType::mixer_id)};

                        //---------------------------------
                        if constexpr (std::is_same<InputType, uint8_t>::value)
                        {
                            static_assert(std::is_enum<ReturnType>::value, "Return type must be enumeration type");
                            if (auto it = std::find_if(value_package.begin(), value_package.end(), [&](auto &element)
                                                       { return element.first == data; });
                                it != value_package.end())
                            {
                                return it->second;
                            }
                        }
                        if constexpr (std::is_enum<InputType>::value)
                        {
                            static_assert(std::is_same<ReturnType, uint8_t>::value, "Return type must be uint8_t");
                            if (auto it = std::find_if(value_package.begin(), value_package.end(), [&](auto &element)
                                                       { return element.second == data; });
                                it != value_package.end())
                            {
                                return it->first;
                            }
                        }
                    }
                    else if (k_version < 135_ui)
                    {
                        constexpr auto value_package = std::array<std::pair<uint8_t, AudioCommonPropertyType>, 53>{
                            std::pair(5, AudioCommonPropertyType::bus_volume),
                            std::pair(24, AudioCommonPropertyType::output_bus_volume),
                            std::pair(26, AudioCommonPropertyType::output_bus_low_pass_filter),
                            std::pair(25, AudioCommonPropertyType::output_bus_high_pass_filter),
                            std::pair(0, AudioCommonPropertyType::voice_volume),
                            std::pair(2, AudioCommonPropertyType::voice_pitch),
                            std::pair(3, AudioCommonPropertyType::voice_low_pass_filter),
                            std::pair(4, AudioCommonPropertyType::voice_high_pass_filter),
                            std::pair(6, AudioCommonPropertyType::voice_volume_make_up_gain),
                            std::pair(23, AudioCommonPropertyType::game_defined_auxiliary_send_volume),
                            std::pair(68, AudioCommonPropertyType::game_defined_auxiliary_send_low_pass_filter),
                            std::pair(69, AudioCommonPropertyType::game_defined_auxiliary_send_high_pass_filter),
                            std::pair(19, AudioCommonPropertyType::user_defined_auxiliary_send_volume_0),
                            std::pair(20, AudioCommonPropertyType::user_defined_auxiliary_send_volume_1),
                            std::pair(21, AudioCommonPropertyType::user_defined_auxiliary_send_volume_2),
                            std::pair(22, AudioCommonPropertyType::user_defined_auxiliary_send_volume_3),
                            std::pair(60, AudioCommonPropertyType::user_defined_auxiliary_send_low_pass_filter_0),
                            std::pair(61, AudioCommonPropertyType::user_defined_auxiliary_send_low_pass_filter_1),
                            std::pair(62, AudioCommonPropertyType::user_defined_auxiliary_send_low_pass_filter_2),
                            std::pair(63, AudioCommonPropertyType::user_defined_auxiliary_send_low_pass_filter_3),
                            std::pair(64, AudioCommonPropertyType::user_defined_auxiliary_send_high_pass_filter_0),
                            std::pair(65, AudioCommonPropertyType::user_defined_auxiliary_send_high_pass_filter_1),
                            std::pair(66, AudioCommonPropertyType::user_defined_auxiliary_send_high_pass_filter_2),
                            std::pair(67, AudioCommonPropertyType::user_defined_auxiliary_send_high_pass_filter_3),
                            std::pair(14, AudioCommonPropertyType::positioning_center_percent),
                            std::pair(12, AudioCommonPropertyType::positioning_speaker_panning_x),
                            std::pair(13, AudioCommonPropertyType::positioning_speaker_panning_y),
                            std::pair(71, AudioCommonPropertyType::positioning_listener_routing_speaker_panning_division_spatialization_mix),
                            std::pair(70, AudioCommonPropertyType::positioning_listener_routing_attenuation_id),
                            std::pair(27, AudioCommonPropertyType::hdr_threshold),
                            std::pair(28, AudioCommonPropertyType::hdr_ratio),
                            std::pair(29, AudioCommonPropertyType::hdr_release_time),
                            std::pair(30, AudioCommonPropertyType::hdr_window_tap_output_game_parameter_id),
                            std::pair(31, AudioCommonPropertyType::hdr_window_tap_output_game_parameter_minimum),
                            std::pair(32, AudioCommonPropertyType::hdr_window_tap_output_game_parameter_maximum),
                            std::pair(33, AudioCommonPropertyType::hdr_envelope_tracking_active_range),
                            std::pair(45, AudioCommonPropertyType::midi_note_tracking_root_note),
                            std::pair(46, AudioCommonPropertyType::midi_event_play_on),
                            std::pair(47, AudioCommonPropertyType::midi_transformation_transposition),
                            std::pair(48, AudioCommonPropertyType::midi_transformation_velocity_offset),
                            std::pair(49, AudioCommonPropertyType::midi_filter_key_range_minimum),
                            std::pair(50, AudioCommonPropertyType::midi_filter_key_range_maximum),
                            std::pair(51, AudioCommonPropertyType::midi_filter_velocity_minimum),
                            std::pair(52, AudioCommonPropertyType::midi_filter_velocity_maximum),
                            std::pair(53, AudioCommonPropertyType::midi_filter_channel),
                            std::pair(55, AudioCommonPropertyType::midi_clip_tempo_source),
                            std::pair(56, AudioCommonPropertyType::midi_target_id),
                            std::pair(7, AudioCommonPropertyType::playback_priority_value),
                            std::pair(8, AudioCommonPropertyType::playback_priority_offset_at_maximum_distance),
                            std::pair(59, AudioCommonPropertyType::playback_initial_delay),
                            std::pair(58, AudioCommonPropertyType::playback_loop),
                            std::pair(54, AudioCommonPropertyType::playback_speed),
                            std::pair(57, AudioCommonPropertyType::mixer_id)};

                        //---------------------------------
                        if constexpr (std::is_same<InputType, uint8_t>::value)
                        {
                            static_assert(std::is_enum<ReturnType>::value, "Return type must be enumeration type");
                            if (auto it = std::find_if(value_package.begin(), value_package.end(), [&](auto &element)
                                                       { return element.first == data; });
                                it != value_package.end())
                            {
                                return it->second;
                            }
                        }
                        if constexpr (std::is_enum<InputType>::value)
                        {
                            static_assert(std::is_same<ReturnType, uint8_t>::value, "Return type must be uint8_t");
                            if (auto it = std::find_if(value_package.begin(), value_package.end(), [&](auto &element)
                                                       { return element.second == data; });
                                it != value_package.end())
                            {
                                return it->first;
                            }
                        }
                    }
                    else if (k_version < 140_ui)
                    {
                        constexpr auto value_package = std::array<std::pair<uint8_t, AudioCommonPropertyType>, 54>{
                            std::pair(5, AudioCommonPropertyType::bus_volume),
                            std::pair(24, AudioCommonPropertyType::output_bus_volume),
                            std::pair(26, AudioCommonPropertyType::output_bus_low_pass_filter),
                            std::pair(25, AudioCommonPropertyType::output_bus_high_pass_filter),
                            std::pair(0, AudioCommonPropertyType::voice_volume),
                            std::pair(2, AudioCommonPropertyType::voice_pitch),
                            std::pair(3, AudioCommonPropertyType::voice_low_pass_filter),
                            std::pair(4, AudioCommonPropertyType::voice_high_pass_filter),
                            std::pair(6, AudioCommonPropertyType::voice_volume_make_up_gain),
                            std::pair(23, AudioCommonPropertyType::game_defined_auxiliary_send_volume),
                            std::pair(68, AudioCommonPropertyType::game_defined_auxiliary_send_low_pass_filter),
                            std::pair(69, AudioCommonPropertyType::game_defined_auxiliary_send_high_pass_filter),
                            std::pair(19, AudioCommonPropertyType::user_defined_auxiliary_send_volume_0),
                            std::pair(20, AudioCommonPropertyType::user_defined_auxiliary_send_volume_1),
                            std::pair(21, AudioCommonPropertyType::user_defined_auxiliary_send_volume_2),
                            std::pair(22, AudioCommonPropertyType::user_defined_auxiliary_send_volume_3),
                            std::pair(60, AudioCommonPropertyType::user_defined_auxiliary_send_low_pass_filter_0),
                            std::pair(61, AudioCommonPropertyType::user_defined_auxiliary_send_low_pass_filter_1),
                            std::pair(62, AudioCommonPropertyType::user_defined_auxiliary_send_low_pass_filter_2),
                            std::pair(63, AudioCommonPropertyType::user_defined_auxiliary_send_low_pass_filter_3),
                            std::pair(64, AudioCommonPropertyType::user_defined_auxiliary_send_high_pass_filter_0),
                            std::pair(65, AudioCommonPropertyType::user_defined_auxiliary_send_high_pass_filter_1),
                            std::pair(66, AudioCommonPropertyType::user_defined_auxiliary_send_high_pass_filter_2),
                            std::pair(72, AudioCommonPropertyType::user_defined_auxiliary_send_high_pass_filter_3),
                            std::pair(67, AudioCommonPropertyType::early_reflection_auxiliary_send_volume),
                            std::pair(14, AudioCommonPropertyType::positioning_center_percent),
                            std::pair(12, AudioCommonPropertyType::positioning_speaker_panning_x),
                            std::pair(13, AudioCommonPropertyType::positioning_speaker_panning_y),
                            std::pair(71, AudioCommonPropertyType::positioning_listener_routing_speaker_panning_division_spatialization_mix),
                            std::pair(70, AudioCommonPropertyType::positioning_listener_routing_attenuation_id),
                            std::pair(27, AudioCommonPropertyType::hdr_threshold),
                            std::pair(28, AudioCommonPropertyType::hdr_ratio),
                            std::pair(29, AudioCommonPropertyType::hdr_release_time),
                            std::pair(30, AudioCommonPropertyType::hdr_window_tap_output_game_parameter_id),
                            std::pair(31, AudioCommonPropertyType::hdr_window_tap_output_game_parameter_minimum),
                            std::pair(32, AudioCommonPropertyType::hdr_window_tap_output_game_parameter_maximum),
                            std::pair(33, AudioCommonPropertyType::hdr_envelope_tracking_active_range),
                            std::pair(45, AudioCommonPropertyType::midi_note_tracking_root_note),
                            std::pair(46, AudioCommonPropertyType::midi_event_play_on),
                            std::pair(47, AudioCommonPropertyType::midi_transformation_transposition),
                            std::pair(48, AudioCommonPropertyType::midi_transformation_velocity_offset),
                            std::pair(49, AudioCommonPropertyType::midi_filter_key_range_minimum),
                            std::pair(50, AudioCommonPropertyType::midi_filter_key_range_maximum),
                            std::pair(51, AudioCommonPropertyType::midi_filter_velocity_minimum),
                            std::pair(52, AudioCommonPropertyType::midi_filter_velocity_maximum),
                            std::pair(53, AudioCommonPropertyType::midi_filter_channel),
                            std::pair(55, AudioCommonPropertyType::midi_clip_tempo_source),
                            std::pair(56, AudioCommonPropertyType::midi_target_id),
                            std::pair(7, AudioCommonPropertyType::playback_priority_value),
                            std::pair(8, AudioCommonPropertyType::playback_priority_offset_at_maximum_distance),
                            std::pair(59, AudioCommonPropertyType::playback_initial_delay),
                            std::pair(58, AudioCommonPropertyType::playback_loop),
                            std::pair(54, AudioCommonPropertyType::playback_speed),
                            std::pair(57, AudioCommonPropertyType::mixer_id)};

                        //---------------------------------
                        if constexpr (std::is_same<InputType, uint8_t>::value)
                        {
                            static_assert(std::is_enum<ReturnType>::value, "Return type must be enumeration type");
                            if (auto it = std::find_if(value_package.begin(), value_package.end(), [&](auto &element)
                                                       { return element.first == data; });
                                it != value_package.end())
                            {
                                return it->second;
                            }
                        }
                        if constexpr (std::is_enum<InputType>::value)
                        {
                            static_assert(std::is_same<ReturnType, uint8_t>::value, "Return type must be uint8_t");
                            if (auto it = std::find_if(value_package.begin(), value_package.end(), [&](auto &element)
                                                       { return element.second == data; });
                                it != value_package.end())
                            {
                                return it->first;
                            }
                        }
                    }
                    else if (k_version < 150_ui)
                    {
                        constexpr auto value_package = std::array<std::pair<uint8_t, AudioCommonPropertyType>, 55>{
                            std::pair(5, AudioCommonPropertyType::bus_volume),
                            std::pair(24, AudioCommonPropertyType::output_bus_volume),
                            std::pair(26, AudioCommonPropertyType::output_bus_low_pass_filter),
                            std::pair(25, AudioCommonPropertyType::output_bus_high_pass_filter),
                            std::pair(0, AudioCommonPropertyType::voice_volume),
                            std::pair(2, AudioCommonPropertyType::voice_pitch),
                            std::pair(3, AudioCommonPropertyType::voice_low_pass_filter),
                            std::pair(4, AudioCommonPropertyType::voice_high_pass_filter),
                            std::pair(6, AudioCommonPropertyType::voice_volume_make_up_gain),
                            std::pair(23, AudioCommonPropertyType::game_defined_auxiliary_send_volume),
                            std::pair(68, AudioCommonPropertyType::game_defined_auxiliary_send_low_pass_filter),
                            std::pair(69, AudioCommonPropertyType::game_defined_auxiliary_send_high_pass_filter),
                            std::pair(19, AudioCommonPropertyType::user_defined_auxiliary_send_volume_0),
                            std::pair(20, AudioCommonPropertyType::user_defined_auxiliary_send_volume_1),
                            std::pair(21, AudioCommonPropertyType::user_defined_auxiliary_send_volume_2),
                            std::pair(22, AudioCommonPropertyType::user_defined_auxiliary_send_volume_3),
                            std::pair(60, AudioCommonPropertyType::user_defined_auxiliary_send_low_pass_filter_0),
                            std::pair(61, AudioCommonPropertyType::user_defined_auxiliary_send_low_pass_filter_1),
                            std::pair(62, AudioCommonPropertyType::user_defined_auxiliary_send_low_pass_filter_2),
                            std::pair(63, AudioCommonPropertyType::user_defined_auxiliary_send_low_pass_filter_3),
                            std::pair(64, AudioCommonPropertyType::user_defined_auxiliary_send_high_pass_filter_0),
                            std::pair(65, AudioCommonPropertyType::user_defined_auxiliary_send_high_pass_filter_1),
                            std::pair(66, AudioCommonPropertyType::user_defined_auxiliary_send_high_pass_filter_2),
                            std::pair(72, AudioCommonPropertyType::user_defined_auxiliary_send_high_pass_filter_3),
                            std::pair(67, AudioCommonPropertyType::early_reflection_auxiliary_send_volume),
                            std::pair(14, AudioCommonPropertyType::positioning_center_percent),
                            std::pair(12, AudioCommonPropertyType::positioning_speaker_panning_x),
                            std::pair(13, AudioCommonPropertyType::positioning_speaker_panning_y),
                            std::pair(73, AudioCommonPropertyType::positioning_speaker_panning_z),
                            std::pair(71, AudioCommonPropertyType::positioning_listener_routing_speaker_panning_division_spatialization_mix),
                            std::pair(70, AudioCommonPropertyType::positioning_listener_routing_attenuation_id),
                            std::pair(27, AudioCommonPropertyType::hdr_threshold),
                            std::pair(28, AudioCommonPropertyType::hdr_ratio),
                            std::pair(29, AudioCommonPropertyType::hdr_release_time),
                            std::pair(30, AudioCommonPropertyType::hdr_window_tap_output_game_parameter_id),
                            std::pair(31, AudioCommonPropertyType::hdr_window_tap_output_game_parameter_minimum),
                            std::pair(32, AudioCommonPropertyType::hdr_window_tap_output_game_parameter_maximum),
                            std::pair(33, AudioCommonPropertyType::hdr_envelope_tracking_active_range),
                            std::pair(45, AudioCommonPropertyType::midi_note_tracking_root_note),
                            std::pair(46, AudioCommonPropertyType::midi_event_play_on),
                            std::pair(47, AudioCommonPropertyType::midi_transformation_transposition),
                            std::pair(48, AudioCommonPropertyType::midi_transformation_velocity_offset),
                            std::pair(49, AudioCommonPropertyType::midi_filter_key_range_minimum),
                            std::pair(50, AudioCommonPropertyType::midi_filter_key_range_maximum),
                            std::pair(51, AudioCommonPropertyType::midi_filter_velocity_minimum),
                            std::pair(52, AudioCommonPropertyType::midi_filter_velocity_maximum),
                            std::pair(53, AudioCommonPropertyType::midi_filter_channel),
                            std::pair(55, AudioCommonPropertyType::midi_clip_tempo_source),
                            std::pair(56, AudioCommonPropertyType::midi_target_id),
                            std::pair(7, AudioCommonPropertyType::playback_priority_value),
                            std::pair(8, AudioCommonPropertyType::playback_priority_offset_at_maximum_distance),
                            std::pair(59, AudioCommonPropertyType::playback_initial_delay),
                            std::pair(58, AudioCommonPropertyType::playback_loop),
                            std::pair(54, AudioCommonPropertyType::playback_speed),
                            std::pair(57, AudioCommonPropertyType::mixer_id)};

                        //---------------------------------
                        if constexpr (std::is_same<InputType, uint8_t>::value)
                        {
                            static_assert(std::is_enum<ReturnType>::value, "Return type must be enumeration type");
                            if (auto it = std::find_if(value_package.begin(), value_package.end(), [&](auto &element)
                                                       { return element.first == data; });
                                it != value_package.end())
                            {
                                return it->second;
                            }
                        }
                        if constexpr (std::is_enum<InputType>::value)
                        {
                            static_assert(std::is_same<ReturnType, uint8_t>::value, "Return type must be uint8_t");
                            if (auto it = std::find_if(value_package.begin(), value_package.end(), [&](auto &element)
                                                       { return element.second == data; });
                                it != value_package.end())
                            {
                                return it->first;
                            }
                        }
                    }
                    else
                    {
                        constexpr auto value_package = std::array<std::pair<uint8_t, AudioCommonPropertyType>, 54>{
                            std::pair(4, AudioCommonPropertyType::bus_volume),
                            std::pair(13, AudioCommonPropertyType::output_bus_volume),
                            std::pair(15, AudioCommonPropertyType::output_bus_low_pass_filter),
                            std::pair(14, AudioCommonPropertyType::output_bus_high_pass_filter),
                            std::pair(0, AudioCommonPropertyType::voice_volume),
                            std::pair(1, AudioCommonPropertyType::voice_pitch),
                            std::pair(2, AudioCommonPropertyType::voice_low_pass_filter),
                            std::pair(3, AudioCommonPropertyType::voice_high_pass_filter),
                            std::pair(5, AudioCommonPropertyType::voice_volume_make_up_gain),
                            std::pair(12, AudioCommonPropertyType::game_defined_auxiliary_send_volume),
                            std::pair(24, AudioCommonPropertyType::game_defined_auxiliary_send_low_pass_filter),
                            std::pair(25, AudioCommonPropertyType::game_defined_auxiliary_send_high_pass_filter),
                            std::pair(8, AudioCommonPropertyType::user_defined_auxiliary_send_volume_0),
                            std::pair(9, AudioCommonPropertyType::user_defined_auxiliary_send_volume_1),
                            std::pair(10, AudioCommonPropertyType::user_defined_auxiliary_send_volume_2),
                            std::pair(11, AudioCommonPropertyType::user_defined_auxiliary_send_volume_3),
                            std::pair(16, AudioCommonPropertyType::user_defined_auxiliary_send_low_pass_filter_0),
                            std::pair(17, AudioCommonPropertyType::user_defined_auxiliary_send_low_pass_filter_1),
                            std::pair(18, AudioCommonPropertyType::user_defined_auxiliary_send_low_pass_filter_2),
                            std::pair(19, AudioCommonPropertyType::user_defined_auxiliary_send_low_pass_filter_3),
                            std::pair(20, AudioCommonPropertyType::user_defined_auxiliary_send_high_pass_filter_0),
                            std::pair(21, AudioCommonPropertyType::user_defined_auxiliary_send_high_pass_filter_1),
                            std::pair(22, AudioCommonPropertyType::user_defined_auxiliary_send_high_pass_filter_2),
                            std::pair(23, AudioCommonPropertyType::user_defined_auxiliary_send_high_pass_filter_3),
                            std::pair(26, AudioCommonPropertyType::early_reflection_auxiliary_send_volume),
                            std::pair(41, AudioCommonPropertyType::positioning_center_percent),
                            std::pair(35, AudioCommonPropertyType::positioning_speaker_panning_x),
                            std::pair(36, AudioCommonPropertyType::positioning_speaker_panning_y),
                            std::pair(37, AudioCommonPropertyType::positioning_speaker_panning_z),
                            std::pair(42, AudioCommonPropertyType::positioning_listener_routing_speaker_panning_division_spatialization_mix),
                            std::pair(85, AudioCommonPropertyType::positioning_listener_routing_attenuation_id),
                            std::pair(27, AudioCommonPropertyType::hdr_threshold),
                            std::pair(28, AudioCommonPropertyType::hdr_ratio),
                            std::pair(29, AudioCommonPropertyType::hdr_release_time),
                            std::pair(61, AudioCommonPropertyType::hdr_window_tap_output_game_parameter_id),
                            std::pair(62, AudioCommonPropertyType::hdr_window_tap_output_game_parameter_minimum),
                            std::pair(63, AudioCommonPropertyType::hdr_window_tap_output_game_parameter_maximum),
                            std::pair(30, AudioCommonPropertyType::hdr_envelope_tracking_active_range),
                            std::pair(75, AudioCommonPropertyType::midi_note_tracking_root_note),
                            std::pair(76, AudioCommonPropertyType::midi_event_play_on),
                            std::pair(31, AudioCommonPropertyType::midi_transformation_transposition),
                            std::pair(32, AudioCommonPropertyType::midi_transformation_velocity_offset),
                            std::pair(77, AudioCommonPropertyType::midi_filter_key_range_minimum),
                            std::pair(78, AudioCommonPropertyType::midi_filter_key_range_maximum),
                            std::pair(79, AudioCommonPropertyType::midi_filter_velocity_minimum),
                            std::pair(80, AudioCommonPropertyType::midi_filter_velocity_maximum),
                            std::pair(81, AudioCommonPropertyType::midi_filter_channel),
                            std::pair(82, AudioCommonPropertyType::midi_clip_tempo_source),
                            std::pair(83, AudioCommonPropertyType::midi_target_id),
                            std::pair(6, AudioCommonPropertyType::playback_priority_value),
                            std::pair(56, AudioCommonPropertyType::playback_priority_offset_at_maximum_distance),
                            std::pair(34, AudioCommonPropertyType::playback_initial_delay),
                            std::pair(84, AudioCommonPropertyType::playback_loop),
                            std::pair(33, AudioCommonPropertyType::playback_speed)};

                        //---------------------------------
                        if constexpr (std::is_same<InputType, uint8_t>::value)
                        {
                            static_assert(std::is_enum<ReturnType>::value, "Return type must be enumeration type");
                            if (auto it = std::find_if(value_package.begin(), value_package.end(), [&](auto &element)
                                                       { return element.first == data; });
                                it != value_package.end())
                            {
                                return it->second;
                            }
                        }
                        if constexpr (std::is_enum<InputType>::value)
                        {
                            static_assert(std::is_same<ReturnType, uint8_t>::value, "Return type must be uint8_t");
                            if (auto it = std::find_if(value_package.begin(), value_package.end(), [&](auto &element)
                                                       { return element.second == data; });
                                it != value_package.end())
                            {
                                return it->first;
                            }
                        }
                    }
                }
            }

            //------------------------------
            if constexpr (std::is_same<ReturnType, size_t>::value)
            {
                return k_none_size;
            }
            if constexpr (std::is_same<InputType, uint8_t>::value)
            {
                static_assert(std::is_enum<ReturnType>::value, "Return type must be enumeration type");
                auto type = std::string_view{std::to_string(data)};
                assert_conditional(false, String::format(fmt::format("{}", Language::get("miscellaneous.shared.failed_cast_enum_index")), type, magic_enum::enum_type_name<ReturnType>()), "exchange_enumeration");
            }
        }

        template <typename Type>
            requires(std::is_enum<Type>::value)
        inline static auto declare_common_property_type(
            Type const &type) -> std::any
        {
            if constexpr (std::is_same<Type, EventActionCommonPropertyType>::value)
            {
                if (k_version < 118_ui)
                {
                    auto value_package = std::array<std::pair<EventActionCommonPropertyType, std::any>, 3>{
                        std::pair(EventActionCommonPropertyType::delay, std::make_any<uint32_t>(0_ui)),
                        std::pair(EventActionCommonPropertyType::fade_time, std::make_any<uint32_t>(0_ui)),
                        std::pair(EventActionCommonPropertyType::probability, std::make_any<float>(100.0f))};

                    //---------------------------------
                    if (auto it = std::find_if(value_package.begin(), value_package.end(), [&](auto &element)
                                               { return element.first == type; });
                        it != value_package.end())
                    {
                        return it->second;
                    }
                }
                else if (k_version < 150_ui)
                {
                    auto value_package = std::array<std::pair<EventActionCommonPropertyType, std::any>, 3>{
                        std::pair(EventActionCommonPropertyType::delay, std::make_any<uint32_t>(0_ui)),
                        std::pair(EventActionCommonPropertyType::fade_time, std::make_any<uint32_t>(0_ui)),
                        std::pair(EventActionCommonPropertyType::probability, std::make_any<float>(100.0f))};

                    //---------------------------------
                    if (auto it = std::find_if(value_package.begin(), value_package.end(), [&](auto &element)
                                               { return element.first == type; });
                        it != value_package.end())
                    {
                        return it->second;
                    }
                }
                else
                {
                    auto value_package = std::array<std::pair<EventActionCommonPropertyType, std::any>, 3>{
                        std::pair(EventActionCommonPropertyType::delay, std::make_any<uint32_t>(0_ui)),
                        std::pair(EventActionCommonPropertyType::fade_time, std::make_any<uint32_t>(0_ui)),
                        std::pair(EventActionCommonPropertyType::probability, std::make_any<float>(100.0f))};

                    //---------------------------------
                    if (auto it = std::find_if(value_package.begin(), value_package.end(), [&](auto &element)
                                               { return element.first == type; });
                        it != value_package.end())
                    {
                        return it->second;
                    }
                }
            }
            if constexpr (std::is_same<Type, ModulatorCommonPropertyType>::value)
            {
                if (k_version >= 112_ui)
                {
                    if (k_version < 150_ui)
                    {
                        auto value_package = std::array<std::pair<ModulatorCommonPropertyType, std::any>, 20>{
                            std::pair(ModulatorCommonPropertyType::scope, std::make_any<uint32_t>(0_ui)),
                            std::pair(ModulatorCommonPropertyType::trigger_on, std::make_any<uint32_t>(0_ui)),
                            std::pair(ModulatorCommonPropertyType::depth, std::make_any<float>(100.0f)),
                            std::pair(ModulatorCommonPropertyType::frequency, std::make_any<float>(1.0f)),
                            std::pair(ModulatorCommonPropertyType::waveform, std::make_any<uint32_t>(0_ui)),
                            std::pair(ModulatorCommonPropertyType::smoothing, std::make_any<float>(0.0f)),
                            std::pair(ModulatorCommonPropertyType::pulse_width_modulation, std::make_any<float>(50.0f)),
                            std::pair(ModulatorCommonPropertyType::attack, std::make_any<float>(0.0f)),
                            std::pair(ModulatorCommonPropertyType::initial_phase_offset, std::make_any<float>(0.0f)),
                            std::pair(ModulatorCommonPropertyType::attack_time, std::make_any<float>(0.2f)),
                            std::pair(ModulatorCommonPropertyType::attack_curve, std::make_any<float>(50.0f)),
                            std::pair(ModulatorCommonPropertyType::decay_time, std::make_any<float>(0.2f)),
                            std::pair(ModulatorCommonPropertyType::sustain_level, std::make_any<float>(100.0f)),
                            std::pair(ModulatorCommonPropertyType::release_time, std::make_any<float>(0.5f)),
                            std::pair(ModulatorCommonPropertyType::sustain_time, std::make_any<float>(0.0f)),
                            std::pair(ModulatorCommonPropertyType::initial_delay, std::make_any<float>(0.0f)),
                            std::pair(ModulatorCommonPropertyType::duration, std::make_any<float>(1.0f)),
                            std::pair(ModulatorCommonPropertyType::loop, std::make_any<uint32_t>(1_ui)),
                            std::pair(ModulatorCommonPropertyType::playback_rate, std::make_any<float>(1.0f)),
                            std::pair(ModulatorCommonPropertyType::stop_playback, std::make_any<bool>(true))};

                        //---------------------------------
                        if (auto it = std::find_if(value_package.begin(), value_package.end(), [&](auto &element)
                                                   { return element.first == type; });
                            it != value_package.end())
                        {
                            return it->second;
                        }
                    }
                    else
                    {
                        auto value_package = std::array<std::pair<ModulatorCommonPropertyType, std::any>, 20>{
                            std::pair(ModulatorCommonPropertyType::scope, std::make_any<uint32_t>(0_ui)),
                            std::pair(ModulatorCommonPropertyType::trigger_on, std::make_any<uint32_t>(0_ui)),
                            std::pair(ModulatorCommonPropertyType::depth, std::make_any<float>(100.0f)),
                            std::pair(ModulatorCommonPropertyType::frequency, std::make_any<float>(1.0f)),
                            std::pair(ModulatorCommonPropertyType::waveform, std::make_any<uint32_t>(0_ui)),
                            std::pair(ModulatorCommonPropertyType::smoothing, std::make_any<float>(0.0f)),
                            std::pair(ModulatorCommonPropertyType::pulse_width_modulation, std::make_any<float>(50.0f)),
                            std::pair(ModulatorCommonPropertyType::attack, std::make_any<float>(0.0f)),
                            std::pair(ModulatorCommonPropertyType::initial_phase_offset, std::make_any<float>(0.0f)),
                            std::pair(ModulatorCommonPropertyType::attack_time, std::make_any<float>(0.2f)),
                            std::pair(ModulatorCommonPropertyType::attack_curve, std::make_any<float>(50.0f)),
                            std::pair(ModulatorCommonPropertyType::decay_time, std::make_any<float>(0.2f)),
                            std::pair(ModulatorCommonPropertyType::sustain_level, std::make_any<float>(100.0f)),
                            std::pair(ModulatorCommonPropertyType::release_time, std::make_any<float>(0.5f)),
                            std::pair(ModulatorCommonPropertyType::sustain_time, std::make_any<float>(0.0f)),
                            std::pair(ModulatorCommonPropertyType::initial_delay, std::make_any<float>(0.0f)),
                            std::pair(ModulatorCommonPropertyType::duration, std::make_any<float>(1.0f)),
                            std::pair(ModulatorCommonPropertyType::loop, std::make_any<uint32_t>(1_ui)),
                            std::pair(ModulatorCommonPropertyType::playback_rate, std::make_any<float>(1.0f)),
                            std::pair(ModulatorCommonPropertyType::stop_playback, std::make_any<bool>(true))};

                        //---------------------------------
                        if (auto it = std::find_if(value_package.begin(), value_package.end(), [&](auto &element)
                                                   { return element.first == type; });
                            it != value_package.end())
                        {
                            return it->second;
                        }
                    }
                }
            }
            if constexpr (std::is_same<Type, AudioCommonPropertyType>::value)
            {
                if (k_version < 88_ui)
                {
                    auto value_package = std::array<std::pair<AudioCommonPropertyType, std::any>, 19>{
                        std::pair(AudioCommonPropertyType::bus_volume, std::make_any<float>(0.0f)),
                        std::pair(AudioCommonPropertyType::output_bus_volume, std::make_any<float>(0.0f)),
                        std::pair(AudioCommonPropertyType::output_bus_low_pass_filter, std::make_any<float>(0.0f)),
                        std::pair(AudioCommonPropertyType::voice_volume, std::make_any<float>(0.0f)),
                        std::pair(AudioCommonPropertyType::voice_pitch, std::make_any<float>(0.0f)),
                        std::pair(AudioCommonPropertyType::voice_low_pass_filter, std::make_any<float>(0.0f)),
                        std::pair(AudioCommonPropertyType::game_defined_auxiliary_send_volume, std::make_any<float>(0.0f)),
                        std::pair(AudioCommonPropertyType::user_defined_auxiliary_send_volume_0, std::make_any<float>(0.0f)),
                        std::pair(AudioCommonPropertyType::user_defined_auxiliary_send_volume_1, std::make_any<float>(0.0f)),
                        std::pair(AudioCommonPropertyType::user_defined_auxiliary_send_volume_2, std::make_any<float>(0.0f)),
                        std::pair(AudioCommonPropertyType::user_defined_auxiliary_send_volume_3, std::make_any<float>(0.0f)),
                        std::pair(AudioCommonPropertyType::positioning_center_percent, std::make_any<float>(0.0f)),
                        std::pair(AudioCommonPropertyType::positioning_speaker_panning_x, std::make_any<float>(0.0f)),
                        std::pair(AudioCommonPropertyType::positioning_speaker_panning_y, std::make_any<float>(0.0f)),
                        std::pair(AudioCommonPropertyType::playback_priority_value, std::make_any<float>(0.0f)),
                        std::pair(AudioCommonPropertyType::playback_priority_offset_at_maximum_distance, std::make_any<float>(0.0f)),
                        std::pair(AudioCommonPropertyType::playback_loop, std::make_any<uint32_t>(0_ui)),
                        std::pair(AudioCommonPropertyType::motion_volume_offset, std::make_any<float>(0.0f)),
                        std::pair(AudioCommonPropertyType::motion_low_pass_filter, std::make_any<float>(0.0f))};

                    //---------------------------------
                    if (auto it = std::find_if(value_package.begin(), value_package.end(), [&](auto &element)
                                               { return element.first == type; });
                        it != value_package.end())
                    {
                        return it->second;
                    }
                }
                else if (k_version < 112_ui)
                {
                    auto value_package = std::array<std::pair<AudioCommonPropertyType, std::any>, 28>{
                        std::pair(AudioCommonPropertyType::bus_volume, std::make_any<float>(0.0f)),
                        std::pair(AudioCommonPropertyType::output_bus_volume, std::make_any<float>(0.0f)),
                        std::pair(AudioCommonPropertyType::output_bus_low_pass_filter, std::make_any<float>(0.0f)),
                        std::pair(AudioCommonPropertyType::voice_volume, std::make_any<float>(0.0f)),
                        std::pair(AudioCommonPropertyType::voice_pitch, std::make_any<float>(0.0f)),
                        std::pair(AudioCommonPropertyType::voice_low_pass_filter, std::make_any<float>(0.0f)),
                        std::pair(AudioCommonPropertyType::voice_volume_make_up_gain, std::make_any<float>(0.0f)),
                        std::pair(AudioCommonPropertyType::game_defined_auxiliary_send_volume, std::make_any<float>(0.0f)),
                        std::pair(AudioCommonPropertyType::user_defined_auxiliary_send_volume_0, std::make_any<float>(0.0f)),
                        std::pair(AudioCommonPropertyType::user_defined_auxiliary_send_volume_1, std::make_any<float>(0.0f)),
                        std::pair(AudioCommonPropertyType::user_defined_auxiliary_send_volume_2, std::make_any<float>(0.0f)),
                        std::pair(AudioCommonPropertyType::user_defined_auxiliary_send_volume_3, std::make_any<float>(0.0f)),
                        std::pair(AudioCommonPropertyType::positioning_center_percent, std::make_any<float>(0.0f)),
                        std::pair(AudioCommonPropertyType::positioning_speaker_panning_x, std::make_any<float>(0.0f)),
                        std::pair(AudioCommonPropertyType::positioning_speaker_panning_y, std::make_any<float>(0.0f)),
                        std::pair(AudioCommonPropertyType::hdr_threshold, std::make_any<float>(0.0f)),
                        std::pair(AudioCommonPropertyType::hdr_ratio, std::make_any<float>(0.0f)),
                        std::pair(AudioCommonPropertyType::hdr_release_time, std::make_any<float>(0.0f)),
                        std::pair(AudioCommonPropertyType::hdr_window_tap_output_game_parameter_id, std::make_any<uint32_t>(0_ui)),
                        std::pair(AudioCommonPropertyType::hdr_window_tap_output_game_parameter_minimum, std::make_any<float>(0.0f)),
                        std::pair(AudioCommonPropertyType::hdr_window_tap_output_game_parameter_maximum, std::make_any<float>(0.0f)),
                        std::pair(AudioCommonPropertyType::hdr_envelope_tracking_active_range, std::make_any<float>(12.0f)),
                        std::pair(AudioCommonPropertyType::playback_priority_value, std::make_any<float>(0.0f)),
                        std::pair(AudioCommonPropertyType::playback_priority_offset_at_maximum_distance, std::make_any<float>(0.0f)),
                        std::pair(AudioCommonPropertyType::playback_initial_delay, std::make_any<float>(0.0f)),
                        std::pair(AudioCommonPropertyType::playback_loop, std::make_any<uint32_t>(0_ui)),
                        std::pair(AudioCommonPropertyType::motion_volume_offset, std::make_any<float>(0.0f)),
                        std::pair(AudioCommonPropertyType::motion_low_pass_filter, std::make_any<float>(0.0f))};

                    //---------------------------------
                    if (auto it = std::find_if(value_package.begin(), value_package.end(), [&](auto &element)
                                               { return element.first == type; });
                        it != value_package.end())
                    {
                        return it->second;
                    }
                }

                else if (k_version < 118_ui)
                {
                    auto value_package = std::array<std::pair<AudioCommonPropertyType, std::any>, 43>{
                        std::pair(AudioCommonPropertyType::bus_volume, std::make_any<float>(0.0f)),
                        std::pair(AudioCommonPropertyType::output_bus_volume, std::make_any<float>(0.0f)),
                        std::pair(AudioCommonPropertyType::output_bus_low_pass_filter, std::make_any<float>(0.0f)),
                        std::pair(AudioCommonPropertyType::output_bus_high_pass_filter, std::make_any<float>(0.0f)),
                        std::pair(AudioCommonPropertyType::voice_volume, std::make_any<float>(0.0f)),
                        std::pair(AudioCommonPropertyType::voice_pitch, std::make_any<float>(0.0f)),
                        std::pair(AudioCommonPropertyType::voice_low_pass_filter, std::make_any<float>(0.0f)),
                        std::pair(AudioCommonPropertyType::voice_high_pass_filter, std::make_any<float>(0.0f)),
                        std::pair(AudioCommonPropertyType::voice_volume_make_up_gain, std::make_any<float>(0.0f)),
                        std::pair(AudioCommonPropertyType::game_defined_auxiliary_send_volume, std::make_any<float>(0.0f)),
                        std::pair(AudioCommonPropertyType::user_defined_auxiliary_send_volume_0, std::make_any<float>(0.0f)),
                        std::pair(AudioCommonPropertyType::user_defined_auxiliary_send_volume_1, std::make_any<float>(0.0f)),
                        std::pair(AudioCommonPropertyType::user_defined_auxiliary_send_volume_2, std::make_any<float>(0.0f)),
                        std::pair(AudioCommonPropertyType::user_defined_auxiliary_send_volume_3, std::make_any<float>(0.0f)),
                        std::pair(AudioCommonPropertyType::positioning_center_percent, std::make_any<float>(0.0f)),
                        std::pair(AudioCommonPropertyType::positioning_speaker_panning_x, std::make_any<float>(0.0f)),
                        std::pair(AudioCommonPropertyType::positioning_speaker_panning_y, std::make_any<float>(0.0f)),
                        std::pair(AudioCommonPropertyType::hdr_threshold, std::make_any<float>(0.0f)),
                        std::pair(AudioCommonPropertyType::hdr_ratio, std::make_any<float>(0.0f)),
                        std::pair(AudioCommonPropertyType::hdr_release_time, std::make_any<float>(0.0f)),
                        std::pair(AudioCommonPropertyType::hdr_window_tap_output_game_parameter_id, std::make_any<uint32_t>(0_ui)),
                        std::pair(AudioCommonPropertyType::hdr_window_tap_output_game_parameter_minimum, std::make_any<float>(0.0f)),
                        std::pair(AudioCommonPropertyType::hdr_window_tap_output_game_parameter_maximum, std::make_any<float>(0.0f)),
                        std::pair(AudioCommonPropertyType::hdr_envelope_tracking_active_range, std::make_any<float>(12.0f)),
                        std::pair(AudioCommonPropertyType::midi_note_tracking_root_note, std::make_any<uint32_t>(60_ui)),
                        std::pair(AudioCommonPropertyType::midi_event_play_on, std::make_any<uint32_t>(0_ui)), // TODO.
                        std::pair(AudioCommonPropertyType::midi_transformation_transposition, std::make_any<uint32_t>(0_ui)),
                        std::pair(AudioCommonPropertyType::midi_transformation_velocity_offset, std::make_any<uint32_t>(0_ui)),
                        std::pair(AudioCommonPropertyType::midi_filter_key_range_minimum, std::make_any<uint32_t>(0_ui)),
                        std::pair(AudioCommonPropertyType::midi_filter_key_range_maximum, std::make_any<uint32_t>(127_ui)),
                        std::pair(AudioCommonPropertyType::midi_filter_velocity_minimum, std::make_any<uint32_t>(0_ui)),
                        std::pair(AudioCommonPropertyType::midi_filter_velocity_maximum, std::make_any<uint32_t>(127_ui)),
                        std::pair(AudioCommonPropertyType::midi_filter_channel, std::make_any<uint32_t>(65535_ui)),
                        std::pair(AudioCommonPropertyType::midi_clip_tempo_source, std::make_any<uint32_t>(0_ui)), // TODO.
                        std::pair(AudioCommonPropertyType::midi_target_id, std::make_any<uint32_t>(0_ui)),
                        std::pair(AudioCommonPropertyType::playback_priority_value, std::make_any<float>(0.0f)),
                        std::pair(AudioCommonPropertyType::playback_priority_offset_at_maximum_distance, std::make_any<float>(0.0f)),
                        std::pair(AudioCommonPropertyType::playback_initial_delay, std::make_any<float>(0.0f)),
                        std::pair(AudioCommonPropertyType::playback_loop, std::make_any<uint32_t>(0_ui)),
                        std::pair(AudioCommonPropertyType::playback_speed, std::make_any<float>(1.0f)),
                        std::pair(AudioCommonPropertyType::motion_volume_offset, std::make_any<float>(0.0f)),
                        std::pair(AudioCommonPropertyType::motion_low_pass_filter, std::make_any<float>(0.0f)),
                        std::pair(AudioCommonPropertyType::mixer_id, std::make_any<uint32_t>(0_ui))};

                    //---------------------------------
                    if (auto it = std::find_if(value_package.begin(), value_package.end(), [&](auto &element)
                                               { return element.first == type; });
                        it != value_package.end())
                    {
                        return it->second;
                    }
                }
                else if (k_version < 128_ui)
                {
                    auto value_package = std::array<std::pair<AudioCommonPropertyType, std::any>, 43>{
                        std::pair(AudioCommonPropertyType::bus_volume, std::make_any<float>(0.0f)),
                        std::pair(AudioCommonPropertyType::output_bus_volume, std::make_any<float>(0.0f)),
                        std::pair(AudioCommonPropertyType::output_bus_low_pass_filter, std::make_any<float>(0.0f)),
                        std::pair(AudioCommonPropertyType::output_bus_high_pass_filter, std::make_any<float>(0.0f)),
                        std::pair(AudioCommonPropertyType::voice_volume, std::make_any<float>(0.0f)),
                        std::pair(AudioCommonPropertyType::voice_pitch, std::make_any<float>(0.0f)),
                        std::pair(AudioCommonPropertyType::voice_low_pass_filter, std::make_any<float>(0.0f)),
                        std::pair(AudioCommonPropertyType::voice_high_pass_filter, std::make_any<float>(0.0f)),
                        std::pair(AudioCommonPropertyType::voice_volume_make_up_gain, std::make_any<float>(0.0f)),
                        std::pair(AudioCommonPropertyType::game_defined_auxiliary_send_volume, std::make_any<float>(0.0f)),
                        std::pair(AudioCommonPropertyType::user_defined_auxiliary_send_volume_0, std::make_any<float>(0.0f)),
                        std::pair(AudioCommonPropertyType::user_defined_auxiliary_send_volume_1, std::make_any<float>(0.0f)),
                        std::pair(AudioCommonPropertyType::user_defined_auxiliary_send_volume_2, std::make_any<float>(0.0f)),
                        std::pair(AudioCommonPropertyType::user_defined_auxiliary_send_volume_3, std::make_any<float>(0.0f)),
                        std::pair(AudioCommonPropertyType::positioning_center_percent, std::make_any<float>(0.0f)),
                        std::pair(AudioCommonPropertyType::positioning_speaker_panning_x, std::make_any<float>(0.0f)),
                        std::pair(AudioCommonPropertyType::positioning_speaker_panning_y, std::make_any<float>(0.0f)),
                        std::pair(AudioCommonPropertyType::hdr_threshold, std::make_any<float>(0.0f)),
                        std::pair(AudioCommonPropertyType::hdr_ratio, std::make_any<float>(0.0f)),
                        std::pair(AudioCommonPropertyType::hdr_release_time, std::make_any<float>(0.0f)),
                        std::pair(AudioCommonPropertyType::hdr_window_tap_output_game_parameter_id, std::make_any<uint32_t>(0_ui)),
                        std::pair(AudioCommonPropertyType::hdr_window_tap_output_game_parameter_minimum, std::make_any<float>(0.0f)),
                        std::pair(AudioCommonPropertyType::hdr_window_tap_output_game_parameter_maximum, std::make_any<float>(0.0f)),
                        std::pair(AudioCommonPropertyType::hdr_envelope_tracking_active_range, std::make_any<float>(12.0f)),
                        std::pair(AudioCommonPropertyType::midi_note_tracking_root_note, std::make_any<uint32_t>(60_ui)),
                        std::pair(AudioCommonPropertyType::midi_event_play_on, std::make_any<uint32_t>(0_ui)),
                        std::pair(AudioCommonPropertyType::midi_transformation_transposition, std::make_any<uint32_t>(0_ui)),
                        std::pair(AudioCommonPropertyType::midi_transformation_velocity_offset, std::make_any<uint32_t>(0_ui)),
                        std::pair(AudioCommonPropertyType::midi_filter_key_range_minimum, std::make_any<uint32_t>(0_ui)),
                        std::pair(AudioCommonPropertyType::midi_filter_key_range_maximum, std::make_any<uint32_t>(127_ui)),
                        std::pair(AudioCommonPropertyType::midi_filter_velocity_minimum, std::make_any<uint32_t>(0_ui)),
                        std::pair(AudioCommonPropertyType::midi_filter_velocity_maximum, std::make_any<uint32_t>(127_ui)),
                        std::pair(AudioCommonPropertyType::midi_filter_channel, std::make_any<uint32_t>(65535_ui)),
                        std::pair(AudioCommonPropertyType::midi_clip_tempo_source, std::make_any<uint32_t>(0_ui)),
                        std::pair(AudioCommonPropertyType::midi_target_id, std::make_any<uint32_t>(0_ui)),
                        std::pair(AudioCommonPropertyType::playback_priority_value, std::make_any<float>(0.0f)),
                        std::pair(AudioCommonPropertyType::playback_priority_offset_at_maximum_distance, std::make_any<float>(0.0f)),
                        std::pair(AudioCommonPropertyType::playback_initial_delay, std::make_any<float>(0.0f)),
                        std::pair(AudioCommonPropertyType::playback_loop, std::make_any<uint32_t>(0_ui)),
                        std::pair(AudioCommonPropertyType::playback_speed, std::make_any<float>(1.0f)),
                        std::pair(AudioCommonPropertyType::motion_volume_offset, std::make_any<float>(0.0f)),
                        std::pair(AudioCommonPropertyType::motion_low_pass_filter, std::make_any<float>(0.0f)),
                        std::pair(AudioCommonPropertyType::mixer_id, std::make_any<uint32_t>(0_ui))};

                    //---------------------------------
                    if (auto it = std::find_if(value_package.begin(), value_package.end(), [&](auto &element)
                                               { return element.first == type; });
                        it != value_package.end())
                    {
                        return it->second;
                    }
                }
                else if (k_version < 132_ui)
                {
                    auto value_package = std::array<std::pair<AudioCommonPropertyType, std::any>, 51>{
                        std::pair(AudioCommonPropertyType::bus_volume, std::make_any<float>(0.0f)),
                        std::pair(AudioCommonPropertyType::output_bus_volume, std::make_any<float>(0.0f)),
                        std::pair(AudioCommonPropertyType::output_bus_low_pass_filter, std::make_any<float>(0.0f)),
                        std::pair(AudioCommonPropertyType::output_bus_high_pass_filter, std::make_any<float>(0.0f)),
                        std::pair(AudioCommonPropertyType::voice_volume, std::make_any<float>(0.0f)),
                        std::pair(AudioCommonPropertyType::voice_pitch, std::make_any<float>(0.0f)),
                        std::pair(AudioCommonPropertyType::voice_low_pass_filter, std::make_any<float>(0.0f)),
                        std::pair(AudioCommonPropertyType::voice_high_pass_filter, std::make_any<float>(0.0f)),
                        std::pair(AudioCommonPropertyType::voice_volume_make_up_gain, std::make_any<float>(0.0f)),
                        std::pair(AudioCommonPropertyType::game_defined_auxiliary_send_volume, std::make_any<float>(0.0f)),
                        std::pair(AudioCommonPropertyType::game_defined_auxiliary_send_low_pass_filter, std::make_any<float>(0.0f)),
                        std::pair(AudioCommonPropertyType::game_defined_auxiliary_send_high_pass_filter, std::make_any<float>(0.0f)),
                        std::pair(AudioCommonPropertyType::user_defined_auxiliary_send_volume_0, std::make_any<float>(0.0f)),
                        std::pair(AudioCommonPropertyType::user_defined_auxiliary_send_volume_1, std::make_any<float>(0.0f)),
                        std::pair(AudioCommonPropertyType::user_defined_auxiliary_send_volume_2, std::make_any<float>(0.0f)),
                        std::pair(AudioCommonPropertyType::user_defined_auxiliary_send_volume_3, std::make_any<float>(0.0f)),
                        std::pair(AudioCommonPropertyType::user_defined_auxiliary_send_low_pass_filter_0, std::make_any<float>(0.0f)),
                        std::pair(AudioCommonPropertyType::user_defined_auxiliary_send_low_pass_filter_1, std::make_any<float>(0.0f)),
                        std::pair(AudioCommonPropertyType::user_defined_auxiliary_send_low_pass_filter_2, std::make_any<float>(0.0f)),
                        std::pair(AudioCommonPropertyType::user_defined_auxiliary_send_low_pass_filter_3, std::make_any<float>(0.0f)),
                        std::pair(AudioCommonPropertyType::user_defined_auxiliary_send_high_pass_filter_0, std::make_any<float>(0.0f)),
                        std::pair(AudioCommonPropertyType::user_defined_auxiliary_send_high_pass_filter_1, std::make_any<float>(0.0f)),
                        std::pair(AudioCommonPropertyType::user_defined_auxiliary_send_high_pass_filter_2, std::make_any<float>(0.0f)),
                        std::pair(AudioCommonPropertyType::user_defined_auxiliary_send_high_pass_filter_3, std::make_any<float>(0.0f)),
                        std::pair(AudioCommonPropertyType::positioning_center_percent, std::make_any<float>(0.0f)),
                        std::pair(AudioCommonPropertyType::positioning_speaker_panning_x, std::make_any<float>(0.0f)),
                        std::pair(AudioCommonPropertyType::positioning_speaker_panning_y, std::make_any<float>(0.0f)),
                        std::pair(AudioCommonPropertyType::hdr_threshold, std::make_any<float>(0.0f)),
                        std::pair(AudioCommonPropertyType::hdr_ratio, std::make_any<float>(0.0f)),
                        std::pair(AudioCommonPropertyType::hdr_release_time, std::make_any<float>(0.0f)),
                        std::pair(AudioCommonPropertyType::hdr_window_tap_output_game_parameter_id, std::make_any<uint32_t>(0_ui)),
                        std::pair(AudioCommonPropertyType::hdr_window_tap_output_game_parameter_minimum, std::make_any<float>(0.0f)),
                        std::pair(AudioCommonPropertyType::hdr_window_tap_output_game_parameter_maximum, std::make_any<float>(0.0f)),
                        std::pair(AudioCommonPropertyType::hdr_envelope_tracking_active_range, std::make_any<float>(12.0f)),
                        std::pair(AudioCommonPropertyType::midi_note_tracking_root_note, std::make_any<uint32_t>(60_ui)),
                        std::pair(AudioCommonPropertyType::midi_event_play_on, std::make_any<uint32_t>(0_ui)),
                        std::pair(AudioCommonPropertyType::midi_transformation_transposition, std::make_any<uint32_t>(0_ui)),
                        std::pair(AudioCommonPropertyType::midi_transformation_velocity_offset, std::make_any<uint32_t>(0_ui)),
                        std::pair(AudioCommonPropertyType::midi_filter_key_range_minimum, std::make_any<uint32_t>(0_ui)),
                        std::pair(AudioCommonPropertyType::midi_filter_key_range_maximum, std::make_any<uint32_t>(127_ui)),
                        std::pair(AudioCommonPropertyType::midi_filter_velocity_minimum, std::make_any<uint32_t>(0_ui)),
                        std::pair(AudioCommonPropertyType::midi_filter_velocity_maximum, std::make_any<uint32_t>(127_ui)),
                        std::pair(AudioCommonPropertyType::midi_filter_channel, std::make_any<uint32_t>(65535_ui)),
                        std::pair(AudioCommonPropertyType::midi_clip_tempo_source, std::make_any<uint32_t>(0_ui)),
                        std::pair(AudioCommonPropertyType::midi_target_id, std::make_any<uint32_t>(0_ui)),
                        std::pair(AudioCommonPropertyType::playback_priority_value, std::make_any<float>(0.0f)),
                        std::pair(AudioCommonPropertyType::playback_priority_offset_at_maximum_distance, std::make_any<float>(0.0f)),
                        std::pair(AudioCommonPropertyType::playback_initial_delay, std::make_any<float>(0.0f)),
                        std::pair(AudioCommonPropertyType::playback_loop, std::make_any<uint32_t>(0_ui)),
                        std::pair(AudioCommonPropertyType::playback_speed, std::make_any<float>(1.0f)),
                        std::pair(AudioCommonPropertyType::mixer_id, std::make_any<uint32_t>(0_ui))};

                    //---------------------------------
                    if (auto it = std::find_if(value_package.begin(), value_package.end(), [&](auto &element)
                                               { return element.first == type; });
                        it != value_package.end())
                    {
                        return it->second;
                    }
                }
                else if (k_version < 135_ui)
                {
                    auto value_package = std::array<std::pair<AudioCommonPropertyType, std::any>, 53>{
                        std::pair(AudioCommonPropertyType::bus_volume, std::make_any<float>(0.0f)),
                        std::pair(AudioCommonPropertyType::output_bus_volume, std::make_any<float>(0.0f)),
                        std::pair(AudioCommonPropertyType::output_bus_low_pass_filter, std::make_any<float>(0.0f)),
                        std::pair(AudioCommonPropertyType::output_bus_high_pass_filter, std::make_any<float>(0.0f)),
                        std::pair(AudioCommonPropertyType::voice_volume, std::make_any<float>(0.0f)),
                        std::pair(AudioCommonPropertyType::voice_pitch, std::make_any<float>(0.0f)),
                        std::pair(AudioCommonPropertyType::voice_low_pass_filter, std::make_any<float>(0.0f)),
                        std::pair(AudioCommonPropertyType::voice_high_pass_filter, std::make_any<float>(0.0f)),
                        std::pair(AudioCommonPropertyType::voice_volume_make_up_gain, std::make_any<float>(0.0f)),
                        std::pair(AudioCommonPropertyType::game_defined_auxiliary_send_volume, std::make_any<float>(0.0f)),
                        std::pair(AudioCommonPropertyType::game_defined_auxiliary_send_low_pass_filter, std::make_any<float>(0.0f)),
                        std::pair(AudioCommonPropertyType::game_defined_auxiliary_send_high_pass_filter, std::make_any<float>(0.0f)),
                        std::pair(AudioCommonPropertyType::user_defined_auxiliary_send_volume_0, std::make_any<float>(0.0f)),
                        std::pair(AudioCommonPropertyType::user_defined_auxiliary_send_volume_1, std::make_any<float>(0.0f)),
                        std::pair(AudioCommonPropertyType::user_defined_auxiliary_send_volume_2, std::make_any<float>(0.0f)),
                        std::pair(AudioCommonPropertyType::user_defined_auxiliary_send_volume_3, std::make_any<float>(0.0f)),
                        std::pair(AudioCommonPropertyType::user_defined_auxiliary_send_low_pass_filter_0, std::make_any<float>(0.0f)),
                        std::pair(AudioCommonPropertyType::user_defined_auxiliary_send_low_pass_filter_1, std::make_any<float>(0.0f)),
                        std::pair(AudioCommonPropertyType::user_defined_auxiliary_send_low_pass_filter_2, std::make_any<float>(0.0f)),
                        std::pair(AudioCommonPropertyType::user_defined_auxiliary_send_low_pass_filter_3, std::make_any<float>(0.0f)),
                        std::pair(AudioCommonPropertyType::user_defined_auxiliary_send_high_pass_filter_0, std::make_any<float>(0.0f)),
                        std::pair(AudioCommonPropertyType::user_defined_auxiliary_send_high_pass_filter_1, std::make_any<float>(0.0f)),
                        std::pair(AudioCommonPropertyType::user_defined_auxiliary_send_high_pass_filter_2, std::make_any<float>(0.0f)),
                        std::pair(AudioCommonPropertyType::user_defined_auxiliary_send_high_pass_filter_3, std::make_any<float>(0.0f)),
                        std::pair(AudioCommonPropertyType::positioning_center_percent, std::make_any<float>(0.0f)),
                        std::pair(AudioCommonPropertyType::positioning_speaker_panning_x, std::make_any<float>(0.0f)),
                        std::pair(AudioCommonPropertyType::positioning_speaker_panning_y, std::make_any<float>(0.0f)),
                        std::pair(AudioCommonPropertyType::positioning_listener_routing_speaker_panning_division_spatialization_mix, std::make_any<float>(100.0f)),
                        std::pair(AudioCommonPropertyType::positioning_listener_routing_attenuation_id, std::make_any<uint32_t>(0_ui)),
                        std::pair(AudioCommonPropertyType::hdr_threshold, std::make_any<float>(0.0f)),
                        std::pair(AudioCommonPropertyType::hdr_ratio, std::make_any<float>(0.0f)),
                        std::pair(AudioCommonPropertyType::hdr_release_time, std::make_any<float>(0.0f)),
                        std::pair(AudioCommonPropertyType::hdr_window_tap_output_game_parameter_id, std::make_any<uint32_t>(0_ui)),
                        std::pair(AudioCommonPropertyType::hdr_window_tap_output_game_parameter_minimum, std::make_any<float>(0.0f)),
                        std::pair(AudioCommonPropertyType::hdr_window_tap_output_game_parameter_maximum, std::make_any<float>(0.0f)),
                        std::pair(AudioCommonPropertyType::hdr_envelope_tracking_active_range, std::make_any<float>(12.0f)),
                        std::pair(AudioCommonPropertyType::midi_note_tracking_root_note, std::make_any<uint32_t>(60_ui)),
                        std::pair(AudioCommonPropertyType::midi_event_play_on, std::make_any<uint32_t>(0_ui)),
                        std::pair(AudioCommonPropertyType::midi_transformation_transposition, std::make_any<uint32_t>(0_ui)),
                        std::pair(AudioCommonPropertyType::midi_transformation_velocity_offset, std::make_any<uint32_t>(0_ui)),
                        std::pair(AudioCommonPropertyType::midi_filter_key_range_minimum, std::make_any<uint32_t>(0_ui)),
                        std::pair(AudioCommonPropertyType::midi_filter_key_range_maximum, std::make_any<uint32_t>(127_ui)),
                        std::pair(AudioCommonPropertyType::midi_filter_velocity_minimum, std::make_any<uint32_t>(0_ui)),
                        std::pair(AudioCommonPropertyType::midi_filter_velocity_maximum, std::make_any<uint32_t>(127_ui)),
                        std::pair(AudioCommonPropertyType::midi_filter_channel, std::make_any<uint32_t>(65535_ui)),
                        std::pair(AudioCommonPropertyType::midi_clip_tempo_source, std::make_any<uint32_t>(0_ui)),
                        std::pair(AudioCommonPropertyType::midi_target_id, std::make_any<uint32_t>(0_ui)),
                        std::pair(AudioCommonPropertyType::playback_priority_value, std::make_any<float>(0.0f)),
                        std::pair(AudioCommonPropertyType::playback_priority_offset_at_maximum_distance, std::make_any<float>(0.0f)),
                        std::pair(AudioCommonPropertyType::playback_initial_delay, std::make_any<float>(0.0f)),
                        std::pair(AudioCommonPropertyType::playback_loop, std::make_any<uint32_t>(0_ui)),
                        std::pair(AudioCommonPropertyType::playback_speed, std::make_any<float>(1.0f)),
                        std::pair(AudioCommonPropertyType::mixer_id, std::make_any<uint32_t>(0_ui))};

                    //---------------------------------
                    if (auto it = std::find_if(value_package.begin(), value_package.end(), [&](auto &element)
                                               { return element.first == type; });
                        it != value_package.end())
                    {
                        return it->second;
                    }
                }
                else if (k_version < 140_ui)
                {
                    auto value_package = std::array<std::pair<AudioCommonPropertyType, std::any>, 54>{
                        std::pair(AudioCommonPropertyType::bus_volume, std::make_any<float>(0.0f)),
                        std::pair(AudioCommonPropertyType::output_bus_volume, std::make_any<float>(0.0f)),
                        std::pair(AudioCommonPropertyType::output_bus_low_pass_filter, std::make_any<float>(0.0f)),
                        std::pair(AudioCommonPropertyType::output_bus_high_pass_filter, std::make_any<float>(0.0f)),
                        std::pair(AudioCommonPropertyType::voice_volume, std::make_any<float>(0.0f)),
                        std::pair(AudioCommonPropertyType::voice_pitch, std::make_any<float>(0.0f)),
                        std::pair(AudioCommonPropertyType::voice_low_pass_filter, std::make_any<float>(0.0f)),
                        std::pair(AudioCommonPropertyType::voice_high_pass_filter, std::make_any<float>(0.0f)),
                        std::pair(AudioCommonPropertyType::voice_volume_make_up_gain, std::make_any<float>(0.0f)),
                        std::pair(AudioCommonPropertyType::game_defined_auxiliary_send_volume, std::make_any<float>(0.0f)),
                        std::pair(AudioCommonPropertyType::game_defined_auxiliary_send_low_pass_filter, std::make_any<float>(0.0f)),
                        std::pair(AudioCommonPropertyType::game_defined_auxiliary_send_high_pass_filter, std::make_any<float>(0.0f)),
                        std::pair(AudioCommonPropertyType::user_defined_auxiliary_send_volume_0, std::make_any<float>(0.0f)),
                        std::pair(AudioCommonPropertyType::user_defined_auxiliary_send_volume_1, std::make_any<float>(0.0f)),
                        std::pair(AudioCommonPropertyType::user_defined_auxiliary_send_volume_2, std::make_any<float>(0.0f)),
                        std::pair(AudioCommonPropertyType::user_defined_auxiliary_send_volume_3, std::make_any<float>(0.0f)),
                        std::pair(AudioCommonPropertyType::user_defined_auxiliary_send_low_pass_filter_0, std::make_any<float>(0.0f)),
                        std::pair(AudioCommonPropertyType::user_defined_auxiliary_send_low_pass_filter_1, std::make_any<float>(0.0f)),
                        std::pair(AudioCommonPropertyType::user_defined_auxiliary_send_low_pass_filter_2, std::make_any<float>(0.0f)),
                        std::pair(AudioCommonPropertyType::user_defined_auxiliary_send_low_pass_filter_3, std::make_any<float>(0.0f)),
                        std::pair(AudioCommonPropertyType::user_defined_auxiliary_send_high_pass_filter_0, std::make_any<float>(0.0f)),
                        std::pair(AudioCommonPropertyType::user_defined_auxiliary_send_high_pass_filter_1, std::make_any<float>(0.0f)),
                        std::pair(AudioCommonPropertyType::user_defined_auxiliary_send_high_pass_filter_2, std::make_any<float>(0.0f)),
                        std::pair(AudioCommonPropertyType::user_defined_auxiliary_send_high_pass_filter_3, std::make_any<float>(0.0f)),
                        std::pair(AudioCommonPropertyType::early_reflection_auxiliary_send_volume, std::make_any<float>(0.0f)),
                        std::pair(AudioCommonPropertyType::positioning_center_percent, std::make_any<float>(0.0f)),
                        std::pair(AudioCommonPropertyType::positioning_speaker_panning_x, std::make_any<float>(0.0f)),
                        std::pair(AudioCommonPropertyType::positioning_speaker_panning_y, std::make_any<float>(0.0f)),
                        std::pair(AudioCommonPropertyType::positioning_listener_routing_speaker_panning_division_spatialization_mix, std::make_any<float>(100.0f)),
                        std::pair(AudioCommonPropertyType::positioning_listener_routing_attenuation_id, 0_ui),
                        std::pair(AudioCommonPropertyType::hdr_threshold, std::make_any<float>(0.0f)),
                        std::pair(AudioCommonPropertyType::hdr_ratio, std::make_any<float>(0.0f)),
                        std::pair(AudioCommonPropertyType::hdr_release_time, std::make_any<float>(0.0f)),
                        std::pair(AudioCommonPropertyType::hdr_window_tap_output_game_parameter_id, std::make_any<uint32_t>(0_ui)),
                        std::pair(AudioCommonPropertyType::hdr_window_tap_output_game_parameter_minimum, std::make_any<float>(0.0f)),
                        std::pair(AudioCommonPropertyType::hdr_window_tap_output_game_parameter_maximum, std::make_any<float>(0.0f)),
                        std::pair(AudioCommonPropertyType::hdr_envelope_tracking_active_range, std::make_any<float>(12.0f)),
                        std::pair(AudioCommonPropertyType::midi_note_tracking_root_note, std::make_any<uint32_t>(60_ui)),
                        std::pair(AudioCommonPropertyType::midi_event_play_on, std::make_any<uint32_t>(0_ui)),
                        std::pair(AudioCommonPropertyType::midi_transformation_transposition, std::make_any<uint32_t>(0_ui)),
                        std::pair(AudioCommonPropertyType::midi_transformation_velocity_offset, std::make_any<uint32_t>(0_ui)),
                        std::pair(AudioCommonPropertyType::midi_filter_key_range_minimum, std::make_any<uint32_t>(0_ui)),
                        std::pair(AudioCommonPropertyType::midi_filter_key_range_maximum, std::make_any<uint32_t>(127_ui)),
                        std::pair(AudioCommonPropertyType::midi_filter_velocity_minimum, std::make_any<uint32_t>(0_ui)),
                        std::pair(AudioCommonPropertyType::midi_filter_velocity_maximum, std::make_any<uint32_t>(127_ui)),
                        std::pair(AudioCommonPropertyType::midi_filter_channel, std::make_any<uint32_t>(65535_ui)),
                        std::pair(AudioCommonPropertyType::midi_clip_tempo_source, std::make_any<uint32_t>(0_ui)),
                        std::pair(AudioCommonPropertyType::midi_target_id, std::make_any<uint32_t>(0_ui)),
                        std::pair(AudioCommonPropertyType::playback_priority_value, std::make_any<float>(0.0f)),
                        std::pair(AudioCommonPropertyType::playback_priority_offset_at_maximum_distance, std::make_any<float>(0.0f)),
                        std::pair(AudioCommonPropertyType::playback_initial_delay, std::make_any<float>(0.0f)),
                        std::pair(AudioCommonPropertyType::playback_loop, std::make_any<uint32_t>(0_ui)),
                        std::pair(AudioCommonPropertyType::playback_speed, std::make_any<float>(1.0f)),
                        std::pair(AudioCommonPropertyType::mixer_id, std::make_any<uint32_t>(0_ui))};

                    //---------------------------------
                    if (auto it = std::find_if(value_package.begin(), value_package.end(), [&](auto &element)
                                               { return element.first == type; });
                        it != value_package.end())
                    {
                        return it->second;
                    }
                }
                else if (k_version < 150_ui)
                {
                    auto value_package = std::array<std::pair<AudioCommonPropertyType, std::any>, 55>{
                        std::pair(AudioCommonPropertyType::bus_volume, std::make_any<float>(0.0f)),
                        std::pair(AudioCommonPropertyType::output_bus_volume, std::make_any<float>(0.0f)),
                        std::pair(AudioCommonPropertyType::output_bus_low_pass_filter, std::make_any<float>(0.0f)),
                        std::pair(AudioCommonPropertyType::output_bus_high_pass_filter, std::make_any<float>(0.0f)),
                        std::pair(AudioCommonPropertyType::voice_volume, std::make_any<float>(0.0f)),
                        std::pair(AudioCommonPropertyType::voice_pitch, std::make_any<float>(0.0f)),
                        std::pair(AudioCommonPropertyType::voice_low_pass_filter, std::make_any<float>(0.0f)),
                        std::pair(AudioCommonPropertyType::voice_high_pass_filter, std::make_any<float>(0.0f)),
                        std::pair(AudioCommonPropertyType::voice_volume_make_up_gain, std::make_any<float>(0.0f)),
                        std::pair(AudioCommonPropertyType::game_defined_auxiliary_send_volume, std::make_any<float>(0.0f)),
                        std::pair(AudioCommonPropertyType::game_defined_auxiliary_send_low_pass_filter, std::make_any<float>(0.0f)),
                        std::pair(AudioCommonPropertyType::game_defined_auxiliary_send_high_pass_filter, std::make_any<float>(0.0f)),
                        std::pair(AudioCommonPropertyType::user_defined_auxiliary_send_volume_0, std::make_any<float>(0.0f)),
                        std::pair(AudioCommonPropertyType::user_defined_auxiliary_send_volume_1, std::make_any<float>(0.0f)),
                        std::pair(AudioCommonPropertyType::user_defined_auxiliary_send_volume_2, std::make_any<float>(0.0f)),
                        std::pair(AudioCommonPropertyType::user_defined_auxiliary_send_volume_3, std::make_any<float>(0.0f)),
                        std::pair(AudioCommonPropertyType::user_defined_auxiliary_send_low_pass_filter_0, std::make_any<float>(0.0f)),
                        std::pair(AudioCommonPropertyType::user_defined_auxiliary_send_low_pass_filter_1, std::make_any<float>(0.0f)),
                        std::pair(AudioCommonPropertyType::user_defined_auxiliary_send_low_pass_filter_2, std::make_any<float>(0.0f)),
                        std::pair(AudioCommonPropertyType::user_defined_auxiliary_send_low_pass_filter_3, std::make_any<float>(0.0f)),
                        std::pair(AudioCommonPropertyType::user_defined_auxiliary_send_high_pass_filter_0, std::make_any<float>(0.0f)),
                        std::pair(AudioCommonPropertyType::user_defined_auxiliary_send_high_pass_filter_1, std::make_any<float>(0.0f)),
                        std::pair(AudioCommonPropertyType::user_defined_auxiliary_send_high_pass_filter_2, std::make_any<float>(0.0f)),
                        std::pair(AudioCommonPropertyType::user_defined_auxiliary_send_high_pass_filter_3, std::make_any<float>(0.0f)),
                        std::pair(AudioCommonPropertyType::early_reflection_auxiliary_send_volume, std::make_any<float>(0.0f)),
                        std::pair(AudioCommonPropertyType::positioning_center_percent, std::make_any<float>(0.0f)),
                        std::pair(AudioCommonPropertyType::positioning_speaker_panning_x, std::make_any<float>(0.0f)),
                        std::pair(AudioCommonPropertyType::positioning_speaker_panning_y, std::make_any<float>(0.0f)),
                        std::pair(AudioCommonPropertyType::positioning_speaker_panning_z, std::make_any<float>(0.0f)),
                        std::pair(AudioCommonPropertyType::positioning_listener_routing_speaker_panning_division_spatialization_mix, std::make_any<float>(100.0f)),
                        std::pair(AudioCommonPropertyType::positioning_listener_routing_attenuation_id, std::make_any<uint32_t>(0_ui)),
                        std::pair(AudioCommonPropertyType::hdr_threshold, std::make_any<float>(0.0f)),
                        std::pair(AudioCommonPropertyType::hdr_ratio, std::make_any<float>(0.0f)),
                        std::pair(AudioCommonPropertyType::hdr_release_time, std::make_any<float>(0.0f)),
                        std::pair(AudioCommonPropertyType::hdr_window_tap_output_game_parameter_id, std::make_any<uint32_t>(0_ui)),
                        std::pair(AudioCommonPropertyType::hdr_window_tap_output_game_parameter_minimum, std::make_any<float>(0.0f)),
                        std::pair(AudioCommonPropertyType::hdr_window_tap_output_game_parameter_maximum, std::make_any<float>(0.0f)),
                        std::pair(AudioCommonPropertyType::hdr_envelope_tracking_active_range, std::make_any<float>(12.0f)),
                        std::pair(AudioCommonPropertyType::midi_note_tracking_root_note, std::make_any<uint32_t>(60_ui)),
                        std::pair(AudioCommonPropertyType::midi_event_play_on, std::make_any<uint32_t>(0_ui)),
                        std::pair(AudioCommonPropertyType::midi_transformation_transposition, std::make_any<uint32_t>(0_ui)),
                        std::pair(AudioCommonPropertyType::midi_transformation_velocity_offset, std::make_any<uint32_t>(0_ui)),
                        std::pair(AudioCommonPropertyType::midi_filter_key_range_minimum, std::make_any<uint32_t>(0_ui)),
                        std::pair(AudioCommonPropertyType::midi_filter_key_range_maximum, std::make_any<uint32_t>(127_ui)),
                        std::pair(AudioCommonPropertyType::midi_filter_velocity_minimum, std::make_any<uint32_t>(0_ui)),
                        std::pair(AudioCommonPropertyType::midi_filter_velocity_maximum, std::make_any<uint32_t>(127_ui)),
                        std::pair(AudioCommonPropertyType::midi_filter_channel, std::make_any<uint32_t>(65535_ui)),
                        std::pair(AudioCommonPropertyType::midi_clip_tempo_source, std::make_any<uint32_t>(0_ui)),
                        std::pair(AudioCommonPropertyType::midi_target_id, std::make_any<uint32_t>(0_ui)),
                        std::pair(AudioCommonPropertyType::playback_priority_value, std::make_any<float>(0.0f)),
                        std::pair(AudioCommonPropertyType::playback_priority_offset_at_maximum_distance, std::make_any<float>(0.0f)),
                        std::pair(AudioCommonPropertyType::playback_initial_delay, std::make_any<float>(0.0f)),
                        std::pair(AudioCommonPropertyType::playback_loop, std::make_any<uint32_t>(0_ui)),
                        std::pair(AudioCommonPropertyType::playback_speed, std::make_any<float>(1.0f)),
                        std::pair(AudioCommonPropertyType::mixer_id, std::make_any<uint32_t>(0_ui))};

                    //---------------------------------
                    if (auto it = std::find_if(value_package.begin(), value_package.end(), [&](auto &element)
                                               { return element.first == type; });
                        it != value_package.end())
                    {
                        return it->second;
                    }
                }
                else
                {
                    auto value_package = std::array<std::pair<AudioCommonPropertyType, std::any>, 54>{
                        std::pair(AudioCommonPropertyType::bus_volume, std::make_any<float>(0.0f)),
                        std::pair(AudioCommonPropertyType::output_bus_volume, std::make_any<float>(0.0f)),
                        std::pair(AudioCommonPropertyType::output_bus_low_pass_filter, std::make_any<float>(0.0f)),
                        std::pair(AudioCommonPropertyType::output_bus_high_pass_filter, std::make_any<float>(0.0f)),
                        std::pair(AudioCommonPropertyType::voice_volume, std::make_any<float>(0.0f)),
                        std::pair(AudioCommonPropertyType::voice_pitch, std::make_any<float>(0.0f)),
                        std::pair(AudioCommonPropertyType::voice_low_pass_filter, std::make_any<float>(0.0f)),
                        std::pair(AudioCommonPropertyType::voice_high_pass_filter, std::make_any<float>(0.0f)),
                        std::pair(AudioCommonPropertyType::voice_volume_make_up_gain, std::make_any<float>(0.0f)),
                        std::pair(AudioCommonPropertyType::game_defined_auxiliary_send_volume, std::make_any<float>(0.0f)),
                        std::pair(AudioCommonPropertyType::game_defined_auxiliary_send_low_pass_filter, std::make_any<float>(0.0f)),
                        std::pair(AudioCommonPropertyType::game_defined_auxiliary_send_high_pass_filter, std::make_any<float>(0.0f)),
                        std::pair(AudioCommonPropertyType::user_defined_auxiliary_send_volume_0, std::make_any<float>(0.0f)),
                        std::pair(AudioCommonPropertyType::user_defined_auxiliary_send_volume_1, std::make_any<float>(0.0f)),
                        std::pair(AudioCommonPropertyType::user_defined_auxiliary_send_volume_2, std::make_any<float>(0.0f)),
                        std::pair(AudioCommonPropertyType::user_defined_auxiliary_send_volume_3, std::make_any<float>(0.0f)),
                        std::pair(AudioCommonPropertyType::user_defined_auxiliary_send_low_pass_filter_0, std::make_any<float>(0.0f)),
                        std::pair(AudioCommonPropertyType::user_defined_auxiliary_send_low_pass_filter_1, std::make_any<float>(0.0f)),
                        std::pair(AudioCommonPropertyType::user_defined_auxiliary_send_low_pass_filter_2, std::make_any<float>(0.0f)),
                        std::pair(AudioCommonPropertyType::user_defined_auxiliary_send_low_pass_filter_3, std::make_any<float>(0.0f)),
                        std::pair(AudioCommonPropertyType::user_defined_auxiliary_send_high_pass_filter_0, std::make_any<float>(0.0f)),
                        std::pair(AudioCommonPropertyType::user_defined_auxiliary_send_high_pass_filter_1, std::make_any<float>(0.0f)),
                        std::pair(AudioCommonPropertyType::user_defined_auxiliary_send_high_pass_filter_2, std::make_any<float>(0.0f)),
                        std::pair(AudioCommonPropertyType::user_defined_auxiliary_send_high_pass_filter_3, std::make_any<float>(0.0f)),
                        std::pair(AudioCommonPropertyType::early_reflection_auxiliary_send_volume, std::make_any<float>(0.0f)),
                        std::pair(AudioCommonPropertyType::positioning_center_percent, std::make_any<float>(0.0f)),
                        std::pair(AudioCommonPropertyType::positioning_speaker_panning_x, std::make_any<float>(0.0f)),
                        std::pair(AudioCommonPropertyType::positioning_speaker_panning_y, std::make_any<float>(0.0f)),
                        std::pair(AudioCommonPropertyType::positioning_speaker_panning_z, std::make_any<float>(0.0f)),
                        std::pair(AudioCommonPropertyType::positioning_listener_routing_speaker_panning_division_spatialization_mix, std::make_any<float>(100.0f)),
                        std::pair(AudioCommonPropertyType::positioning_listener_routing_attenuation_id, std::make_any<uint32_t>(0_ui)),
                        std::pair(AudioCommonPropertyType::hdr_threshold, std::make_any<float>(0.0f)),
                        std::pair(AudioCommonPropertyType::hdr_ratio, std::make_any<float>(0.0f)),
                        std::pair(AudioCommonPropertyType::hdr_release_time, std::make_any<float>(0.0f)),
                        std::pair(AudioCommonPropertyType::hdr_window_tap_output_game_parameter_id, std::make_any<uint32_t>(0_ui)),
                        std::pair(AudioCommonPropertyType::hdr_window_tap_output_game_parameter_minimum, std::make_any<float>(0.0f)),
                        std::pair(AudioCommonPropertyType::hdr_window_tap_output_game_parameter_maximum, std::make_any<float>(0.0f)),
                        std::pair(AudioCommonPropertyType::hdr_envelope_tracking_active_range, std::make_any<float>(12.0f)),
                        std::pair(AudioCommonPropertyType::midi_note_tracking_root_note, std::make_any<float>(0.0f)),
                        std::pair(AudioCommonPropertyType::midi_event_play_on, std::make_any<uint32_t>(60_ui)),
                        std::pair(AudioCommonPropertyType::midi_transformation_transposition, std::make_any<uint32_t>(0_ui)),
                        std::pair(AudioCommonPropertyType::midi_transformation_velocity_offset, std::make_any<uint32_t>(0_ui)),
                        std::pair(AudioCommonPropertyType::midi_filter_key_range_minimum, std::make_any<uint32_t>(0_ui)),
                        std::pair(AudioCommonPropertyType::midi_filter_key_range_maximum, std::make_any<uint32_t>(0_ui)),
                        std::pair(AudioCommonPropertyType::midi_filter_velocity_minimum, std::make_any<uint32_t>(0_ui)),
                        std::pair(AudioCommonPropertyType::midi_filter_velocity_maximum, std::make_any<uint32_t>(0_ui)),
                        std::pair(AudioCommonPropertyType::midi_filter_channel, std::make_any<uint32_t>(65535_ui)),
                        std::pair(AudioCommonPropertyType::midi_clip_tempo_source, std::make_any<uint32_t>(0_ui)),
                        std::pair(AudioCommonPropertyType::midi_target_id, std::make_any<uint32_t>(0_ui)),
                        std::pair(AudioCommonPropertyType::playback_priority_value, std::make_any<float>(0.0f)),
                        std::pair(AudioCommonPropertyType::playback_priority_offset_at_maximum_distance, std::make_any<float>(0.0f)),
                        std::pair(AudioCommonPropertyType::playback_initial_delay, std::make_any<float>(0.0f)),
                        std::pair(AudioCommonPropertyType::playback_loop, std::make_any<uint32_t>(0_ui)),
                        std::pair(AudioCommonPropertyType::playback_speed, std::make_any<float>(1.0f))};

                    //---------------------------------
                    if (auto it = std::find_if(value_package.begin(), value_package.end(), [&](auto &element)
                                               { return element.first == type; });
                        it != value_package.end())
                    {
                        return it->second;
                    }
                }
            }
        }
    };
}