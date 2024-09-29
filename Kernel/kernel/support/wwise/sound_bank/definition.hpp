#pragma once

#include "kernel/definition/utility.hpp"

namespace Sen::Kernel::Support::WWise::SoundBank
{

    using namespace Sen::Kernel::Definition;

    using namespace Sen::Kernel::Support::Miscellaneous::Shared;

    inline static auto k_version = 0_ui;

    struct HeadValue
    {
        uint32_t values;
        uint32_t project_id;
        uint32_t soundbank_type = 0;
        std::string bank_hash;
        uint32_t padding = 0;
    };

    inline auto to_json(
        nlohmann::ordered_json &nlohmann_json_j,
        const HeadValue &nlohmann_json_t) -> void
    {
        nlohmann_json_j["values"] = nlohmann_json_t.values;
        nlohmann_json_j["project_id"] = nlohmann_json_t.project_id;
        if (!nlohmann_json_t.soundbank_type != 0)
        {
            nlohmann_json_j["soundbank_type"] = nlohmann_json_t.soundbank_type;
        }
        if (!nlohmann_json_t.bank_hash.empty())
        {
            nlohmann_json_j["bank_hash"] = nlohmann_json_t.bank_hash;
        }
        nlohmann_json_j["padding"] = nlohmann_json_t.padding;
        return;
    }

    inline auto from_json(
        const nlohmann::ordered_json &nlohmann_json_j,
        HeadValue &nlohmann_json_t) -> void
    {
        nlohmann_json_j.at("values").get_to(nlohmann_json_t.values);
        nlohmann_json_j.at("project_id").get_to(nlohmann_json_t.project_id);
        try
        {
            nlohmann_json_j.at("soundbank_type").get_to(nlohmann_json_t.soundbank_type);
        }
        catch (nlohmann::ordered_json::exception &e)
        {
        }
        try
        {
            nlohmann_json_j.at("bank_hash").get_to(nlohmann_json_t.bank_hash);
        }
        catch (nlohmann::ordered_json::exception &e)
        {
        }
        nlohmann_json_j.at("padding").get_to(nlohmann_json_t.padding);
        return;
    }

    struct BankHeader
    {
        uint32_t version;
        uint32_t soundbank_id;
        uint32_t language_id;
        HeadValue head_info;
    };

    inline auto to_json(
        nlohmann::ordered_json &nlohmann_json_j,
        const BankHeader &nlohmann_json_t) -> void
    {
        nlohmann_json_j["version"] = nlohmann_json_t.version;
        k_version = nlohmann_json_t.version;
        nlohmann_json_j["soundbank_id"] = nlohmann_json_t.soundbank_id;
        nlohmann_json_j["language_id"] = nlohmann_json_t.language_id;
        nlohmann_json_j["head_info"] = nlohmann_json_t.head_info;
        return;
    }

    inline auto from_json(
        const nlohmann::ordered_json &nlohmann_json_j,
        BankHeader &nlohmann_json_t) -> void
    {
        nlohmann_json_j.at("version").get_to(nlohmann_json_t.version);
        k_version = nlohmann_json_t.version;
        nlohmann_json_j.at("soundbank_id").get_to(nlohmann_json_t.soundbank_id);
        nlohmann_json_j.at("language_id").get_to(nlohmann_json_t.language_id);
        nlohmann_json_j.at("head_info").get_to(nlohmann_json_t.head_info);
        return;
    }

    struct DATA
    {
        uint32_t pos;
        uint32_t size;
    };

#pragma region Enumeration

    //-----------------------------

    enum class HierarchyType : uint8_t
    {
        unknown,
        stateful_property_setting,
        event_action,
        event,
        dialogue_event,
        attenuation,
        effect,
        source,
        audio_bus,
        auxiliary_audio_bus,
        sound,
        sound_playlist_container,
        sound_switch_container,
        sound_blend_container,
        actor_mixer,
        music_track,
        music_segment,
        music_playlist_container,
        music_switch_container,
        low_frequency_oscillator_modulator,
        envelope_modulator,
        audio_device,
        time_modulator
    };

    // ----------------------------

    enum class AudioSourceType : uint8_t
    {
        embedded,
        streamed,
        streamed_prefetched
    };

    // -------------------------------------

    enum class Curve : uint8_t
    {
        constant,
        linear,
        s,
        s_inverted,
        sine,
        sine_reciprocal,
        logarithmic_1dot41,
        logarithmic_3dot0,
        exponential_1dot41,
        exponential_3dot0,
    };

    // -------------------------------------

    enum class TimePoint : uint8_t
    {
        immediate,
        next_grid,
        next_bar,
        next_beat,
        next_cue,
        custom_cue,
        entry_cue,
        exit_cue,
        last_exit_position, // >= 140
    };

    // -------------------------------------

    enum class MusicTrackClipCurveItemType : uint8_t
    {
        voice_volume,
        voice_low_pass_filter,
        voice_high_pass_filter, // >= 112
        clip_fade_in,
        clip_fade_out
    };

    // ----------------------------------

    enum class MusicTrackTrackType : uint8_t
    {
        normal,
        random_step,
        sequence_step,
        switcher // >= 112
    };

    //------------------------------

    enum class AudioPositioningSettingType : uint8_t // < 132
    {
        two_dimension,
        three_dimension
    };

    //------------------------------

    enum class AudioPositioningSettingSpeakerPanningMode : uint8_t // >= 132 && < 140
    {
        direct_assignment,
        balance_fade,
        steering // >= 140
    };

    //-----------------------------
    enum AudioPositioningSettingListenerRoutingPositionSourceMode
    {
        user_defined,
        game_defined, // < 132

        emitter, // >= 132
        emitter_with_automation,
        listener_with_automation
    };

    //-----------------------------

    enum AudioPlayType
    {
        sequence,
        random
    };

    //-----------------------------

    enum BusAutomaticDuckingSettingBusTarget
    {
        voice_volume,
        bus_volume
    };

    //-----------------------------

    enum AudioPlayMode
    {
        step,
        continuous,
    };

    //-----------------------------

    enum AudioPositioningSettingListenerRoutingSpatialization
    {
        none,
        position,
        position_and_orientation,
    };

    //------------------------------

    enum ParameterCategory
    { // >= 112
        game_parameter,
        midi_parameter,
        modulator
    };

    //------------------------------

    enum class CoordinateMode : uint8_t
    {
        linear,
        scaled,
        scaled_3
    };

    //------------------------------

    enum class PropertyCategory : uint8_t
    {
        unidirectional,
        bidirectional,
        bidirectional_ranged,
        boolean,
        unknown_6, //>= 145
    };

    //------------------------------

    enum class SoundMIDISettingEventPlayOn : uint8_t
    {
        note_on,
        note_off
    };

    //------------------------------

    enum class MusicTransitionSettingSynchronizeMode : uint8_t
    {
        entry_cue,
        random_cue,
        custom_cue,
        same_time_as_playing_segment
    };

    //------------------------------

    enum class MusicTransitionSettingJumpMode : uint8_t
    {
        start,
        specific,
        next,
        last_played
    };

    //-----------------------------

    enum class MusicMIDISettingClipTempoSource : uint8_t
    {
        hierarchy,
        file
    };

    //------------------------------

    enum class AudioPlaybackLimitSettingScope : uint8_t
    {
        per_game_object,
        globally
    };

    //------------------------------

    enum class AudioPlaybackLimitSettingWhenLimitIsReached : uint8_t
    {
        kill_voice,
        use_virtual_voice_setting
    };

    //------------------------------

    enum class AudioPlaybackLimitSettingWhenPriorityIsEqual : uint8_t
    {
        discard_oldest_instance,
        discard_newest_instance
    };

    //------------------------------

    enum class AudioPlayTypeRandomType : uint8_t
    {
        standard,
        shuffle
    };

    //------------------------------

    enum class GameParameterInterpolationMode : uint8_t
    {
        none,
        slew_rate,
        filtering_over_time
    };

    //------------------------------

    enum class AudioPlayModeContinuousTransitionType : uint8_t
    {
        none,
        xfade_amp,
        xfade_power,
        delay,
        sample_accurate,
        trigger_rate
    };

    //------------------------------

    enum class GameParameterBindToBuiltInParameterMode : uint8_t
    {
        none,
        distance,
        azimuth,
        elevation,
        object_to_listener_angle,
        emitter_cone,
        obstruction,
        occlusion,
        listener_cone,
        diffraction
    };

    //------------------------------

    enum class AudioPlayTypeSequenceAtEndOfPlaylist : uint8_t
    {
        restart,
        play_in_reserve_order
    };

    //----------------------------

    enum class AudioVirtualVoiceSettingBehavior : uint8_t
    {
        continue_to_play,
        kill_voice,
        send_to_virtual_voice,
        kill_if_finite_else_virtual // >= 140
    };

    //----------------------------

    enum class AudioVirtualVoiceSettingOnReturnToPhysical : uint8_t
    {
        play_from_beginning,
        play_from_elapsed_time,
        resume
    };

    //----------------------------

    enum class EventActionMode : uint8_t
    {
        none,
        one,
        all,
        all_except
    };

    //----------------------------

    enum class ModulatorWaveform : uint8_t
    {
        sine,
        triangle,
        square,
        saw_up,
        saw_down,
        random
    };

    //----------------------------

    enum class ModulatorScope : uint8_t
    {
        voice,
        note_or_event,
        game_object,
        global
    };

    //----------------------------

    enum class EventActionScope : uint8_t
    {
        global,
        game_object
    };

    //----------------------------

    enum class SoundPlaylistContainerScope : uint8_t
    {
        global,
        game_object
    };

    //----------------------------

    enum class ModulatorTriggerOn : uint8_t
    {
        play,
        note_off
    };

    //----------------------------

    enum class BusHDRSettingDynamicReleaseMode : uint8_t
    {
        linear,
        exponential
    };

    //----------------------------

    enum class AudioAssociationSettingMode : uint8_t
    {
        best_match,
        weighted
    };

    //----------------------------
#pragma endregion

    //----------------------------

    struct AudioSwitcherSetting
    {
        bool is_state;
        uint32_t group;
        uint32_t default_item;
        std::vector<uint32_t> association;
    };

    inline auto to_json(
        nlohmann::ordered_json &nlohmann_json_j,
        const AudioSwitcherSetting &nlohmann_json_t) -> void
    {
        nlohmann_json_j["is_state"] = nlohmann_json_t.is_state;
        nlohmann_json_j["group"] = nlohmann_json_t.group;
        nlohmann_json_j["default_item"] = nlohmann_json_t.default_item;
        nlohmann_json_j["association"] = nlohmann_json_t.association;
        return;
    }

    inline auto from_json(
        const nlohmann::ordered_json &nlohmann_json_j,
        AudioSwitcherSetting &nlohmann_json_t) -> void
    {
        nlohmann_json_j.at("is_state").get_to(nlohmann_json_t.is_state);
        nlohmann_json_j.at("group").get_to(nlohmann_json_t.group);
        nlohmann_json_j.at("default_item").get_to(nlohmann_json_t.default_item);
        nlohmann_json_j.at("association").get_to(nlohmann_json_t.association);
        return;
    }

    //----------------------------

    struct AudioSourceSetting
    {
        uint32_t plug_in;
        AudioSourceType type;
        uint32_t source;
        uint32_t resource;
        uint32_t resource_offset;
        uint32_t resource_size;
        bool is_voice;
        bool non_cachable_stream;
    };

    inline auto to_json(
        nlohmann::ordered_json &nlohmann_json_j,
        const AudioSourceSetting &nlohmann_json_t) -> void
    {
        nlohmann_json_j["plug_in"] = nlohmann_json_t.plug_in;
        nlohmann_json_j["type"] = magic_enum::enum_name(nlohmann_json_t.type);
        nlohmann_json_j["source"] = nlohmann_json_t.source;
        nlohmann_json_j["resource"] = nlohmann_json_t.resource;
        nlohmann_json_j["resource_offset"] = nlohmann_json_t.resource_offset;
        nlohmann_json_j["resource_size"] = nlohmann_json_t.resource_size;
        nlohmann_json_j["is_voice"] = nlohmann_json_t.is_voice;
        if (k_version >= 122_ui)
        {
            nlohmann_json_j["non_cachable_stream"] = nlohmann_json_t.non_cachable_stream;
        }
        return;
    }

    inline auto from_json(
        const nlohmann::ordered_json &nlohmann_json_j,
        AudioSourceSetting &nlohmann_json_t) -> void
    {
        nlohmann_json_j.at("plug_in").get_to(nlohmann_json_t.plug_in);
        nlohmann_json_t.type = magic_enum_cast<AudioSourceType>(nlohmann_json_j.at("type").get<std::string>());
        nlohmann_json_j.at("source").get_to(nlohmann_json_t.source);
        nlohmann_json_j.at("resource").get_to(nlohmann_json_t.resource);
        nlohmann_json_j.at("resource_offset").get_to(nlohmann_json_t.resource_offset);
        nlohmann_json_j.at("resource_size").get_to(nlohmann_json_t.resource_size);
        nlohmann_json_j.at("is_voice").get_to(nlohmann_json_t.is_voice);
        if (k_version >= 122_ui)
        {
            nlohmann_json_j.at("non_cachable_stream").get_to(nlohmann_json_t.non_cachable_stream);
        }
        return;
    }

    //-------------------------------------

    struct Position
    {
        float x;
        float y;
        float z;
    };

    inline auto to_json(
        nlohmann::ordered_json &nlohmann_json_j,
        const Position &nlohmann_json_t) -> void
    {
        nlohmann_json_j["x"] = nlohmann_json_t.x;
        nlohmann_json_j["y"] = nlohmann_json_t.y;
        if (k_version >= 132_ui)
        {
            nlohmann_json_j["z"] = nlohmann_json_t.z;
        }
        return;
    }

    inline auto from_json(
        const nlohmann::ordered_json &nlohmann_json_j,
        Position &nlohmann_json_t) -> void
    {
        nlohmann_json_j.at("x").get_to(nlohmann_json_t.x);
        nlohmann_json_j.at("y").get_to(nlohmann_json_t.y);
        if (k_version >= 132_ui)
        {
            nlohmann_json_j.at("z").get_to(nlohmann_json_t.z);
        }
        return;
    }

    //-------------------------------------

    struct Position2
    {
        float x;
        float y;
    };

    inline auto to_json(
        nlohmann::ordered_json &nlohmann_json_j,
        const Position2 &nlohmann_json_t) -> void
    {
        nlohmann_json_j["x"] = nlohmann_json_t.x;
        nlohmann_json_j["y"] = nlohmann_json_t.y;
        return;
    }

    inline auto from_json(
        const nlohmann::ordered_json &nlohmann_json_j,
        Position2 &nlohmann_json_t) -> void
    {
        nlohmann_json_j.at("x").get_to(nlohmann_json_t.x);
        nlohmann_json_j.at("y").get_to(nlohmann_json_t.y);
        return;
    }

    //-------------------------------------

    struct Position2ID
    {
        float x;
        uint32_t y;
    };

    inline auto to_json(
        nlohmann::ordered_json &nlohmann_json_j,
        const Position2ID &nlohmann_json_t) -> void
    {
        nlohmann_json_j["x"] = nlohmann_json_t.x;
        nlohmann_json_j["y"] = nlohmann_json_t.y;
        return;
    }

    inline auto from_json(
        const nlohmann::ordered_json &nlohmann_json_j,
        Position2ID &nlohmann_json_t) -> void
    {
        nlohmann_json_j.at("x").get_to(nlohmann_json_t.x);
        nlohmann_json_j.at("y").get_to(nlohmann_json_t.y);
        return;
    }

    // -------------------------------------

    struct CoordinatePoint
    {
        Position2 position;
        Curve curve;
    };

    inline auto to_json(
        nlohmann::ordered_json &nlohmann_json_j,
        const CoordinatePoint &nlohmann_json_t) -> void
    {
        nlohmann_json_j["position"] = nlohmann_json_t.position;
        nlohmann_json_j["curve"] = magic_enum::enum_name(nlohmann_json_t.curve);
        return;
    }

    inline auto from_json(
        const nlohmann::ordered_json &nlohmann_json_j,
        CoordinatePoint &nlohmann_json_t) -> void
    {
        nlohmann_json_j.at("position").get_to(nlohmann_json_t.position);
        nlohmann_json_t.curve = magic_enum_cast<Curve>(nlohmann_json_j.at("curve").get<std::string>());
        return;
    }

    //--------------------------------------

    struct CoordinateIDPoint
    {
        Position2ID position;
        Curve curve;
    };

    inline auto to_json(
        nlohmann::ordered_json &nlohmann_json_j,
        const CoordinateIDPoint &nlohmann_json_t) -> void
    {
        nlohmann_json_j["position"] = nlohmann_json_t.position;
        nlohmann_json_j["curve"] = magic_enum::enum_name(nlohmann_json_t.curve);
        return;
    }

    inline auto from_json(
        const nlohmann::ordered_json &nlohmann_json_j,
        CoordinateIDPoint &nlohmann_json_t) -> void
    {
        nlohmann_json_j.at("position").get_to(nlohmann_json_t.position);
        nlohmann_json_t.curve = magic_enum_cast<Curve>(nlohmann_json_j.at("curve").get<std::string>());
        return;
    }

    // -------------------------------------

    struct MusicTransitionSettingItemFade
    {
        uint32_t time;
        uint32_t curve;
        uint32_t offset;
    };

    inline auto to_json(
        nlohmann::ordered_json &nlohmann_json_j,
        const MusicTransitionSettingItemFade &nlohmann_json_t) -> void
    {
        nlohmann_json_j["time"] = nlohmann_json_t.time;
        nlohmann_json_j["curve"] = nlohmann_json_t.curve;
        nlohmann_json_j["offset"] = nlohmann_json_t.offset;
        return;
    }

    inline auto from_json(
        const nlohmann::ordered_json &nlohmann_json_j,
        MusicTransitionSettingItemFade &nlohmann_json_t) -> void
    {
        nlohmann_json_j.at("time").get_to(nlohmann_json_t.time);
        nlohmann_json_j.at("curve").get_to(nlohmann_json_t.curve);
        nlohmann_json_j.at("offset").get_to(nlohmann_json_t.offset);
        return;
    }

    // -------------------------------------

    struct MusicTrackTransitionSettingItemSource
    {
        TimePoint exit_source_at;
        uint32_t exit_source_at_custom_cue_match;
        MusicTransitionSettingItemFade fade_out;
    };

    inline auto to_json(
        nlohmann::ordered_json &nlohmann_json_j,
        const MusicTrackTransitionSettingItemSource &nlohmann_json_t) -> void
    {
        if (k_version >= 112_ui)
        {
            nlohmann_json_j["exit_source_at"] = magic_enum::enum_name(nlohmann_json_t.exit_source_at);
            nlohmann_json_j["exit_source_at_custom_cue_match"] = nlohmann_json_t.exit_source_at_custom_cue_match;
            nlohmann_json_j["fade_out"] = nlohmann_json_t.fade_out;
        }
        return;
    }

    inline auto from_json(
        const nlohmann::ordered_json &nlohmann_json_j,
        MusicTrackTransitionSettingItemSource &nlohmann_json_t) -> void
    {
        if (k_version >= 112_ui)
        {
            nlohmann_json_t.exit_source_at = magic_enum_cast<TimePoint>(nlohmann_json_j.at("exit_source_at").get<std::string>());
            nlohmann_json_j.at("exit_source_at_custom_cue_match").get_to(nlohmann_json_t.exit_source_at_custom_cue_match);
            nlohmann_json_j.at("fade_out").get_to(nlohmann_json_t.fade_out);
        }
        return;
    }

    // -------------------------------------

    struct MusicTrackTransitionSettingItemDestination
    {
        MusicTransitionSettingItemFade fade_in;
    };

    inline auto to_json(
        nlohmann::ordered_json &nlohmann_json_j,
        const MusicTrackTransitionSettingItemDestination &nlohmann_json_t) -> void
    {
        if (k_version >= 112_ui)
        {
            nlohmann_json_j["fade_in"] = nlohmann_json_t.fade_in;
        }
        return;
    }

    inline auto from_json(
        const nlohmann::ordered_json &nlohmann_json_j,
        MusicTrackTransitionSettingItemDestination &nlohmann_json_t) -> void
    {
        if (k_version >= 112_ui)
        {
            nlohmann_json_j.at("fade_in").get_to(nlohmann_json_t.fade_in);
        }
        return;
    }

    // -------------------------------------

    struct MusicTrackTransitionSetting
    {
        //uint32_t switcher;
        MusicTrackTransitionSettingItemSource source;
        MusicTrackTransitionSettingItemDestination destination;
    };

    inline auto to_json(
        nlohmann::ordered_json &nlohmann_json_j,
        const MusicTrackTransitionSetting &nlohmann_json_t) -> void
    {
        if (k_version >= 112_ui)
        {
           // nlohmann_json_j["switcher"] = nlohmann_json_t.switcher;
            nlohmann_json_j["source"] = nlohmann_json_t.source;
            nlohmann_json_j["destination"] = nlohmann_json_t.destination;
        }
        return;
    }

    inline auto from_json(
        const nlohmann::ordered_json &nlohmann_json_j,
        MusicTrackTransitionSetting &nlohmann_json_t) -> void
    {
        if (k_version >= 112_ui)
        {
           // nlohmann_json_j.at("switcher").get_to(nlohmann_json_t.switcher);
            nlohmann_json_j.at("source").get_to(nlohmann_json_t.source);
            nlohmann_json_j.at("destination").get_to(nlohmann_json_t.destination);
        }
        return;
    }

    // -----------------------------------

    struct MusicTrackClipCurveItem
    {
        uint32_t index;
        MusicTrackClipCurveItemType type;
        std::vector<CoordinatePoint> point;
    };

    inline auto to_json(
        nlohmann::ordered_json &nlohmann_json_j,
        const MusicTrackClipCurveItem &nlohmann_json_t) -> void
    {
        nlohmann_json_j["index"] = nlohmann_json_t.index;
        nlohmann_json_j["type"] = magic_enum::enum_name(nlohmann_json_t.type);
        nlohmann_json_j["point"] = nlohmann_json_t.point;
        return;
    }

    inline auto from_json(
        const nlohmann::ordered_json &nlohmann_json_j,
        MusicTrackClipCurveItem &nlohmann_json_t) -> void
    {
        nlohmann_json_j.at("index").get_to(nlohmann_json_t.index);
        nlohmann_json_t.type = magic_enum_cast<MusicTrackClipCurveItemType>(nlohmann_json_j.at("type").get<std::string>());
        nlohmann_json_j.at("point").get_to(nlohmann_json_t.point);
        return;
    }

    // -----------------------------------

    struct MusicTrackClipItem
    {
        uint32_t u1;
        uint32_t source;
        uint32_t event;
        double offset;
        double begin;
        double end;
        double duration;
    };

    inline auto to_json(
        nlohmann::ordered_json &nlohmann_json_j,
        const MusicTrackClipItem &nlohmann_json_t) -> void
    {
        nlohmann_json_j["u1"] = nlohmann_json_t.u1;
        nlohmann_json_j["source"] = nlohmann_json_t.source;
        if (k_version >= 140_ui)
        {
            nlohmann_json_j["event"] = nlohmann_json_t.event;
        }
        nlohmann_json_j["offset"] = nlohmann_json_t.offset;
        nlohmann_json_j["begin"] = nlohmann_json_t.begin;
        nlohmann_json_j["end"] = nlohmann_json_t.end;
        nlohmann_json_j["duration"] = nlohmann_json_t.duration;
        return;
    }

    inline auto from_json(
        const nlohmann::ordered_json &nlohmann_json_j,
        MusicTrackClipItem &nlohmann_json_t) -> void
    {
        nlohmann_json_j.at("u1").get_to(nlohmann_json_t.u1);
        nlohmann_json_j.at("source").get_to(nlohmann_json_t.source);
        if (k_version >= 140_ui)
        {
            nlohmann_json_j.at("event").get_to(nlohmann_json_t.event);
        }
        nlohmann_json_j.at("offset").get_to(nlohmann_json_t.offset);
        nlohmann_json_j.at("begin").get_to(nlohmann_json_t.begin);
        nlohmann_json_j.at("end").get_to(nlohmann_json_t.end);
        nlohmann_json_j.at("duration").get_to(nlohmann_json_t.duration);
        return;
    }

    // -----------------------------------

    struct MusicTrackClip
    {
        uint32_t u1;
        std::vector<MusicTrackClipItem> item;
        std::vector<MusicTrackClipCurveItem> curve;
    };

    inline auto to_json(
        nlohmann::ordered_json &nlohmann_json_j,
        const MusicTrackClip &nlohmann_json_t) -> void
    {
        nlohmann_json_j["u1"] = nlohmann_json_t.u1;
        nlohmann_json_j["item"] = nlohmann_json_t.item;
        nlohmann_json_j["curve"] = nlohmann_json_t.curve;
        return;
    }

    inline auto from_json(
        const nlohmann::ordered_json &nlohmann_json_j,
        MusicTrackClip &nlohmann_json_t) -> void
    {
        nlohmann_json_j.at("u1").get_to(nlohmann_json_t.u1);
        nlohmann_json_j.at("item").get_to(nlohmann_json_t.item);
        nlohmann_json_j.at("curve").get_to(nlohmann_json_t.curve);
        return;
    }

    // -----------------------------------

    struct MusicTrackPlaybackSetting
    {
        MusicTrackClip clip;
        MusicTrackTrackType type;
        AudioSwitcherSetting switcher;
        MusicTrackTransitionSetting transition;
    };

    inline auto to_json(
        nlohmann::ordered_json &nlohmann_json_j,
        const MusicTrackPlaybackSetting &nlohmann_json_t) -> void
    {
        nlohmann_json_j["clip"] = nlohmann_json_t.clip;
        nlohmann_json_j["type"] = magic_enum::enum_name(nlohmann_json_t.type);
        if (k_version > 112)
        {
            nlohmann_json_j["switcher"] = nlohmann_json_t.switcher;
            nlohmann_json_j["transition"] = nlohmann_json_t.transition;
        }
        return;
    }

    inline auto from_json(
        const nlohmann::ordered_json &nlohmann_json_j,
        MusicTrackPlaybackSetting &nlohmann_json_t) -> void
    {
        nlohmann_json_j.at("clip").get_to(nlohmann_json_t.clip);
        nlohmann_json_t.type = magic_enum_cast<MusicTrackTrackType>(nlohmann_json_j.at("type").get<std::string>());
        if (k_version >= 112_ui)
        {
            nlohmann_json_j.at("switcher").get_to(nlohmann_json_t.switcher);
            nlohmann_json_j.at("transition").get_to(nlohmann_json_t.transition);
        }
        return;
    }

    //---------------------------------

    struct MusicTrackStream
    {
        uint32_t look_ahead_time;
    };

    inline auto to_json(
        nlohmann::ordered_json &nlohmann_json_j,
        const MusicTrackStream &nlohmann_json_t) -> void
    {
        nlohmann_json_j["look_ahead_time"] = nlohmann_json_t.look_ahead_time;
        return;
    }

    inline auto from_json(
        const nlohmann::ordered_json &nlohmann_json_j,
        MusicTrackStream &nlohmann_json_t) -> void
    {
        nlohmann_json_j.at("look_ahead_time").get_to(nlohmann_json_t.look_ahead_time);
        return;
    }

    // ------------------------------

    template <typename T>
        requires std::integral<T> or std::floating_point<T>
    struct RandomizableValue
    {
        T value;
        T minimum_value;
        T maximum_value;
    };

    template <typename T>
        requires std::integral<T> or std::floating_point<T>
    inline auto to_json(
        nlohmann::ordered_json &nlohmann_json_j,
        const RandomizableValue<T> &nlohmann_json_t) -> void
    {
        nlohmann_json_j["value"] = nlohmann_json_t.value;
        nlohmann_json_j["min"] = nlohmann_json_t.minimum_value;
        nlohmann_json_j["max"] = nlohmann_json_t.maximum_value;
        return;
    }

    template <typename T>
        requires std::integral<T> or std::floating_point<T>
    inline auto from_json(
        const nlohmann::ordered_json &nlohmann_json_j,
        RandomizableValue<T> &nlohmann_json_t) -> void
    {
        nlohmann_json_j.at("value").get_to(nlohmann_json_t.value);
        nlohmann_json_j.at("min").get_to(nlohmann_json_t.minimum_value);
        nlohmann_json_j.at("max").get_to(nlohmann_json_t.maximum_value);
        return;
    }

    // ------------------------------

    struct BusVoiceSetting
    {
        float volume;
        float pitch;
        float low_pass_filter;
        float high_pass_filter;
    };

    inline auto to_json(
        nlohmann::ordered_json &nlohmann_json_j,
        const BusVoiceSetting &nlohmann_json_t) -> void
    {
        nlohmann_json_j["volume"] = nlohmann_json_t.volume;
        nlohmann_json_j["pitch"] = nlohmann_json_t.pitch;
        nlohmann_json_j["low_pass_filter"] = nlohmann_json_t.low_pass_filter;
        if (k_version >= 112_ui)
        {
            nlohmann_json_j["high_pass_filter"] = nlohmann_json_t.high_pass_filter;
        }
        return;
    }

    inline auto from_json(
        const nlohmann::ordered_json &nlohmann_json_j,
        BusVoiceSetting &nlohmann_json_t) -> void
    {
        nlohmann_json_j.at("volume").get_to(nlohmann_json_t.volume);
        nlohmann_json_j.at("pitch").get_to(nlohmann_json_t.pitch);
        nlohmann_json_j.at("low_pass_filter").get_to(nlohmann_json_t.low_pass_filter);
        if (k_version >= 112_ui)
        {
            nlohmann_json_j.at("high_pass_filter").get_to(nlohmann_json_t.high_pass_filter);
        }
        return;
    }

    // ------------------------------

    struct AudioVoice
    {
        RandomizableValue<float> volume;
        RandomizableValue<float> pitch;
        RandomizableValue<float> low_pass_filter;
        RandomizableValue<float> high_pass_filter;
    };

    inline auto to_json(
        nlohmann::ordered_json &nlohmann_json_j,
        const AudioVoice &nlohmann_json_t) -> void
    {
        nlohmann_json_j["volume"] = nlohmann_json_t.volume;
        nlohmann_json_j["pitch"] = nlohmann_json_t.pitch;
        nlohmann_json_j["low_pass_filter"] = nlohmann_json_t.low_pass_filter;
        if (k_version >= 112_ui)
        {
            nlohmann_json_j["high_pass_filter"] = nlohmann_json_t.high_pass_filter;
        }
        return;
    }

    inline auto from_json(
        const nlohmann::ordered_json &nlohmann_json_j,
        AudioVoice &nlohmann_json_t) -> void
    {
        nlohmann_json_j.at("volume").get_to(nlohmann_json_t.volume);
        nlohmann_json_j.at("pitch").get_to(nlohmann_json_t.pitch);
        nlohmann_json_j.at("low_pass_filter").get_to(nlohmann_json_t.low_pass_filter);
        if (k_version >= 112_ui)
        {
            nlohmann_json_j.at("high_pass_filter").get_to(nlohmann_json_t.high_pass_filter);
        }
        return;
    }

    // -----------------------------

    struct AudioOutputBusSetting
    {
        uint32_t bus;
        float volume;
        float low_pass_filter;
        float high_pass_filter;
    };

    inline auto to_json(
        nlohmann::ordered_json &nlohmann_json_j,
        const AudioOutputBusSetting &nlohmann_json_t) -> void
    {
        nlohmann_json_j["bus"] = nlohmann_json_t.bus;
        nlohmann_json_j["volume"] = nlohmann_json_t.volume;
        nlohmann_json_j["low_pass_filter"] = nlohmann_json_t.low_pass_filter;
        if (k_version >= 112_ui)
        {
            nlohmann_json_j["high_pass_filter"] = nlohmann_json_t.high_pass_filter;
        }
        return;
    }

    inline auto from_json(
        const nlohmann::ordered_json &nlohmann_json_j,
        AudioOutputBusSetting &nlohmann_json_t) -> void
    {
        nlohmann_json_j.at("bus").get_to(nlohmann_json_t.bus);
        nlohmann_json_j.at("volume").get_to(nlohmann_json_t.volume);
        nlohmann_json_j.at("low_pass_filter").get_to(nlohmann_json_t.low_pass_filter);
        if (k_version >= 112_ui)
        {
            nlohmann_json_j.at("high_pass_filter").get_to(nlohmann_json_t.high_pass_filter);
        }
        return;
    }

    // ------------------------------

    struct AudioGameDefinedAuxiliarySendSetting
    {
        bool enable;
        float volume;
        float low_pass_filter;
        float high_pass_filter;
    };

    inline auto to_json(
        nlohmann::ordered_json &nlohmann_json_j,
        const AudioGameDefinedAuxiliarySendSetting &nlohmann_json_t) -> void
    {
        nlohmann_json_j["enable"] = nlohmann_json_t.enable;
        nlohmann_json_j["volume"] = nlohmann_json_t.volume;
        if (k_version >= 128_ui)
        {
            nlohmann_json_j["low_pass_filter"] = nlohmann_json_t.low_pass_filter;
            nlohmann_json_j["high_pass_filter"] = nlohmann_json_t.high_pass_filter;
        }
        return;
    }

    inline auto from_json(
        const nlohmann::ordered_json &nlohmann_json_j,
        AudioGameDefinedAuxiliarySendSetting &nlohmann_json_t) -> void
    {
        nlohmann_json_j.at("enable").get_to(nlohmann_json_t.enable);
        nlohmann_json_j.at("volume").get_to(nlohmann_json_t.volume);
        if (k_version >= 128_ui)
        {
            nlohmann_json_j.at("low_pass_filter").get_to(nlohmann_json_t.low_pass_filter);
            nlohmann_json_j.at("high_pass_filter").get_to(nlohmann_json_t.high_pass_filter);
        }
        return;
    }

    // ------------------------------

    struct AudioUserDefinedAuxiliarySendSettingItem
    {
        uint32_t bus;
        float volume;
        float low_pass_filter;
        float high_pass_filter;
    };

    inline auto to_json(
        nlohmann::ordered_json &nlohmann_json_j,
        const AudioUserDefinedAuxiliarySendSettingItem &nlohmann_json_t) -> void
    {
        nlohmann_json_j["bus"] = nlohmann_json_t.bus;
        nlohmann_json_j["volume"] = nlohmann_json_t.volume;
        if (k_version >= 128_ui)
        {
            nlohmann_json_j["low_pass_filter"] = nlohmann_json_t.low_pass_filter;
            nlohmann_json_j["high_pass_filter"] = nlohmann_json_t.high_pass_filter;
        }
        return;
    }

    inline auto from_json(
        const nlohmann::ordered_json &nlohmann_json_j,
        AudioUserDefinedAuxiliarySendSettingItem &nlohmann_json_t) -> void
    {
        nlohmann_json_j.at("bus").get_to(nlohmann_json_t.bus);
        nlohmann_json_j.at("volume").get_to(nlohmann_json_t.volume);
        if (k_version >= 128_ui)
        {
            nlohmann_json_j.at("low_pass_filter").get_to(nlohmann_json_t.low_pass_filter);
            nlohmann_json_j.at("high_pass_filter").get_to(nlohmann_json_t.high_pass_filter);
        }
        return;
    }

    // ------------------------------

    struct AudioUserDefinedAuxiliarySendSetting
    {
        bool enable;
        AudioUserDefinedAuxiliarySendSettingItem item_1;
        AudioUserDefinedAuxiliarySendSettingItem item_2;
        AudioUserDefinedAuxiliarySendSettingItem item_3;
        AudioUserDefinedAuxiliarySendSettingItem item_4;
    };

    inline auto to_json(
        nlohmann::ordered_json &nlohmann_json_j,
        const AudioUserDefinedAuxiliarySendSetting &nlohmann_json_t) -> void
    {
        nlohmann_json_j["enable"] = nlohmann_json_t.enable;
        nlohmann_json_j["item_1"] = nlohmann_json_t.item_1;
        nlohmann_json_j["item_2"] = nlohmann_json_t.item_2;
        nlohmann_json_j["item_3"] = nlohmann_json_t.item_3;
        nlohmann_json_j["item_4"] = nlohmann_json_t.item_4;
        return;
    }

    inline auto from_json(
        const nlohmann::ordered_json &nlohmann_json_j,
        AudioUserDefinedAuxiliarySendSetting &nlohmann_json_t) -> void
    {
        nlohmann_json_j.at("enable").get_to(nlohmann_json_t.enable);
        nlohmann_json_j.at("item_1").get_to(nlohmann_json_t.item_1);
        nlohmann_json_j.at("item_2").get_to(nlohmann_json_t.item_2);
        nlohmann_json_j.at("item_3").get_to(nlohmann_json_t.item_3);
        nlohmann_json_j.at("item_4").get_to(nlohmann_json_t.item_4);
        return;
    }

    // ------------------------------

    struct AudioEarlyReflectionAuxiliarySendSetting
    {
        uint32_t bus;
        float volume;
    };

    inline auto to_json(
        nlohmann::ordered_json &nlohmann_json_j,
        const AudioEarlyReflectionAuxiliarySendSetting &nlohmann_json_t) -> void
    {
        nlohmann_json_j["bus"] = nlohmann_json_t.bus;
        nlohmann_json_j["volume"] = nlohmann_json_t.volume;
        return;
    }

    inline auto from_json(
        const nlohmann::ordered_json &nlohmann_json_j,
        AudioEarlyReflectionAuxiliarySendSetting &nlohmann_json_t) -> void
    {
        nlohmann_json_j.at("bus").get_to(nlohmann_json_t.bus);
        nlohmann_json_j.at("volume").get_to(nlohmann_json_t.volume);
        return;
    }

    // ------------------------------

    struct AudioAuxiliarySendSetting
    {
        AudioGameDefinedAuxiliarySendSetting game_defined;
        AudioUserDefinedAuxiliarySendSetting user_defined;
        AudioEarlyReflectionAuxiliarySendSetting early_reflection;
    };

    inline auto to_json(
        nlohmann::ordered_json &nlohmann_json_j,
        const AudioAuxiliarySendSetting &nlohmann_json_t) -> void
    {
        nlohmann_json_j["game_defined"] = nlohmann_json_t.game_defined;
        nlohmann_json_j["user_defined"] = nlohmann_json_t.user_defined;
        if (k_version >= 135_ui)
        {
            nlohmann_json_j["early_reflection"] = nlohmann_json_t.early_reflection;
        }
        return;
    }

    inline auto from_json(
        const nlohmann::ordered_json &nlohmann_json_j,
        AudioAuxiliarySendSetting &nlohmann_json_t) -> void
    {
        nlohmann_json_j.at("game_defined").get_to(nlohmann_json_t.game_defined);
        nlohmann_json_j.at("user_defined").get_to(nlohmann_json_t.user_defined);
        if (k_version >= 135_ui)
        {
            nlohmann_json_j.at("early_reflection").get_to(nlohmann_json_t.early_reflection);
        }
        return;
    }

    // ------------------------------

    struct AudioEffectSettingItem
    {
        uint32_t index;
        uint32_t id;
        bool use_share_set;
        bool u1;
        bool bypass;
    };

    inline auto to_json(
        nlohmann::ordered_json &nlohmann_json_j,
        const AudioEffectSettingItem &nlohmann_json_t) -> void
    {
        nlohmann_json_j["index"] = nlohmann_json_t.index;
        nlohmann_json_j["id"] = nlohmann_json_t.id;
        nlohmann_json_j["use_share_set"] = nlohmann_json_t.use_share_set;
        if (k_version < 150_ui)
        {
            nlohmann_json_j["u1"] = nlohmann_json_t.u1;
        }
        else
        {
            nlohmann_json_j["bypass"] = nlohmann_json_t.bypass;
        }
        return;
    }

    inline auto from_json(
        const nlohmann::ordered_json &nlohmann_json_j,
        AudioEffectSettingItem &nlohmann_json_t) -> void
    {
        nlohmann_json_j.at("index").get_to(nlohmann_json_t.index);
        nlohmann_json_j.at("id").get_to(nlohmann_json_t.id);
        nlohmann_json_j.at("use_share_set").get_to(nlohmann_json_t.use_share_set);
        if (k_version < 150_ui)
        {
            nlohmann_json_j.at("u1").get_to(nlohmann_json_t.u1);
        }
        else
        {
            nlohmann_json_j.at("bypass").get_to(nlohmann_json_t.bypass);
        }
        return;
    }

    // -----------------------------

    struct AudioEffectSetting
    {
        std::array<bool, 5> bypass;
        std::vector<AudioEffectSettingItem> item;
    };

    inline auto to_json(
        nlohmann::ordered_json &nlohmann_json_j,
        const AudioEffectSetting &nlohmann_json_t) -> void
    {
        if (k_version < 150_ui)
        {
            nlohmann_json_j["bypass"] = nlohmann_json_t.bypass;
        }
        else
        {
            nlohmann_json_j["bypass"] = nlohmann_json_t.bypass[0];
        }
        nlohmann_json_j["item"] = nlohmann_json_t.item;
        return;
    }

    inline auto from_json(
        const nlohmann::ordered_json &nlohmann_json_j,
        AudioEffectSetting &nlohmann_json_t) -> void
    {
        if (k_version < 150_ui)
        {
            nlohmann_json_j.at("bypass").get_to(nlohmann_json_t.bypass);
        }
        else
        {
            nlohmann_json_j.at("bypass").get_to(nlohmann_json_t.bypass[0]);
        }
        nlohmann_json_j.at("item").get_to(nlohmann_json_t.item);
        return;
    }

    //------------------------------

    struct AudioMetadataSettingItem
    {
        uint32_t index;
        uint32_t id;
        bool use_share_set;
    };

    inline auto to_json(
        nlohmann::ordered_json &nlohmann_json_j,
        const AudioMetadataSettingItem &nlohmann_json_t) -> void
    {
        if (k_version >= 140_ui)
        {
            nlohmann_json_j["index"] = nlohmann_json_t.index;
            nlohmann_json_j["id"] = nlohmann_json_t.id;
            nlohmann_json_j["use_share_set"] = nlohmann_json_t.use_share_set;
        }

        return;
    }

    inline auto from_json(
        const nlohmann::ordered_json &nlohmann_json_j,
        AudioMetadataSettingItem &nlohmann_json_t) -> void
    {
        if (k_version >= 140_ui)
        {
            nlohmann_json_j.at("index").get_to(nlohmann_json_t.index);
            nlohmann_json_j.at("id").get_to(nlohmann_json_t.id);
            nlohmann_json_j.at("use_share_set").get_to(nlohmann_json_t.use_share_set);
        }
        return;
    }

    //------------------------------

    struct AudioMetadataSetting
    {
        std::vector<AudioMetadataSettingItem> item;
    };

    inline auto to_json(
        nlohmann::ordered_json &nlohmann_json_j,
        const AudioMetadataSetting &nlohmann_json_t) -> void
    {
        if (k_version >= 140)
        {
            nlohmann_json_j["item"] = nlohmann_json_t.item;
        }
        return;
    }

    inline auto from_json(
        const nlohmann::ordered_json &nlohmann_json_j,
        AudioMetadataSetting &nlohmann_json_t) -> void
    {
        if (k_version >= 140_ui)
        {
            nlohmann_json_j.at("item").get_to(nlohmann_json_t.item);
        }
        return;
    }

    //------------------------------

    struct AudioPositioningSettingSpeakerPanning
    {
        bool enable;
        AudioPositioningSettingSpeakerPanningMode mode;
        Position position;
    };

    inline auto to_json(
        nlohmann::ordered_json &nlohmann_json_j,
        const AudioPositioningSettingSpeakerPanning &nlohmann_json_t) -> void
    {
        if (k_version < 132_ui)
        {
            nlohmann_json_j["enable"] = nlohmann_json_t.enable;
        }
        else
        {
            nlohmann_json_j["mode"] = magic_enum::enum_name(nlohmann_json_t.mode);
        }
        nlohmann_json_j["position"] = nlohmann_json_t.position;
        return;
    }

    inline auto from_json(
        const nlohmann::ordered_json &nlohmann_json_j,
        AudioPositioningSettingSpeakerPanning &nlohmann_json_t) -> void
    {
        if (k_version < 132_ui)
        {
            nlohmann_json_j.at("enable").get_to(nlohmann_json_t.enable);
        }
        else
        {
            nlohmann_json_t.mode = magic_enum_cast<AudioPositioningSettingSpeakerPanningMode>(nlohmann_json_j.at("mode").get<std::string>());
        }
        nlohmann_json_j.at("position").get_to(nlohmann_json_t.position);
        return;
    }

    //-----------------------------

    struct AudioPositioningSettingListenerRoutingAttenuation
    {
        bool enable;
        uint32_t id;
    };

    inline auto to_json(
        nlohmann::ordered_json &nlohmann_json_j,
        const AudioPositioningSettingListenerRoutingAttenuation &nlohmann_json_t) -> void
    {
        if (k_version >= 134_ui)
        {
            nlohmann_json_j["enable"] = nlohmann_json_t.enable;
        }
        nlohmann_json_j["id"] = nlohmann_json_t.id;
        return;
    }

    inline auto from_json(
        const nlohmann::ordered_json &nlohmann_json_j,
        AudioPositioningSettingListenerRoutingAttenuation &nlohmann_json_t) -> void
    {
        if (k_version >= 134_ui)
        {
            nlohmann_json_j.at("enable").get_to(nlohmann_json_t.enable);
        }
        nlohmann_json_j.at("id").get_to(nlohmann_json_t.id);
        return;
    }

    //----------------------------

    struct AudioPositioningSettingListenerRoutingPositionSourceAutomationPoint
    {
        Position position;
        uint32_t duration;
    };

    inline auto to_json(
        nlohmann::ordered_json &nlohmann_json_j,
        const AudioPositioningSettingListenerRoutingPositionSourceAutomationPoint &nlohmann_json_t) -> void
    {
        nlohmann_json_j["position"] = nlohmann_json_t.position;
        nlohmann_json_j["duration"] = nlohmann_json_t.duration;
        return;
    }

    inline auto from_json(
        const nlohmann::ordered_json &nlohmann_json_j,
        AudioPositioningSettingListenerRoutingPositionSourceAutomationPoint &nlohmann_json_t) -> void
    {
        nlohmann_json_j.at("position").get_to(nlohmann_json_t.position);
        nlohmann_json_j.at("duration").get_to(nlohmann_json_t.duration);
        return;
    }

    struct AudioPositioningSettingListenerRoutingPositionSourceAutomationPathPoint
    {
        uint32_t begin;
        uint32_t count;
    };

    inline auto to_json(
        nlohmann::ordered_json &nlohmann_json_j,
        const AudioPositioningSettingListenerRoutingPositionSourceAutomationPathPoint &nlohmann_json_t) -> void
    {
        nlohmann_json_j["begin"] = nlohmann_json_t.begin;
        nlohmann_json_j["count"] = nlohmann_json_t.count;
        return;
    }

    inline auto from_json(
        const nlohmann::ordered_json &nlohmann_json_j,
        AudioPositioningSettingListenerRoutingPositionSourceAutomationPathPoint &nlohmann_json_t) -> void
    {
        nlohmann_json_j.at("begin").get_to(nlohmann_json_t.begin);
        nlohmann_json_j.at("count").get_to(nlohmann_json_t.count);
        return;
    }

    struct AudioPositioningSettingListenerRoutingPositionSourceAutomationPathRandomRange
    {
        float left_right;
        float front_back;
        float up_down;
    };

    inline auto to_json(
        nlohmann::ordered_json &nlohmann_json_j,
        const AudioPositioningSettingListenerRoutingPositionSourceAutomationPathRandomRange &nlohmann_json_t) -> void
    {
        nlohmann_json_j["left_right"] = nlohmann_json_t.left_right;
        nlohmann_json_j["front_back"] = nlohmann_json_t.front_back;
        if (k_version >= 112_ui)
        {
            nlohmann_json_j["up_down"] = nlohmann_json_t.up_down;
        }
        return;
    }

    inline auto from_json(
        const nlohmann::ordered_json &nlohmann_json_j,
        AudioPositioningSettingListenerRoutingPositionSourceAutomationPathRandomRange &nlohmann_json_t) -> void
    {
        nlohmann_json_j.at("left_right").get_to(nlohmann_json_t.left_right);
        nlohmann_json_j.at("front_back").get_to(nlohmann_json_t.front_back);
        if (k_version >= 112_ui)
        {
            nlohmann_json_j.at("up_down").get_to(nlohmann_json_t.up_down);
        }
        return;
    }

    //-----------------------------

    struct AudioPositioningSettingListenerRoutingPositionSourceAutomationPath
    {
        AudioPositioningSettingListenerRoutingPositionSourceAutomationPathPoint point;
        AudioPositioningSettingListenerRoutingPositionSourceAutomationPathRandomRange random_range;
    };

    inline auto to_json(
        nlohmann::ordered_json &nlohmann_json_j,
        const AudioPositioningSettingListenerRoutingPositionSourceAutomationPath &nlohmann_json_t) -> void
    {
        nlohmann_json_j["point"] = nlohmann_json_t.point;
        nlohmann_json_j["random_range"] = nlohmann_json_t.random_range;
        return;
    }

    inline auto from_json(
        const nlohmann::ordered_json &nlohmann_json_j,
        AudioPositioningSettingListenerRoutingPositionSourceAutomationPath &nlohmann_json_t) -> void
    {
        nlohmann_json_j.at("point").get_to(nlohmann_json_t.point);
        nlohmann_json_j.at("random_range").get_to(nlohmann_json_t.random_range);
        return;
    }

    //-----------------------------

    struct AudioPositioningSettingListenerRoutingPositionSourceAutomation
    {
        AudioPlayType play_type;
        AudioPlayMode play_mode;
        bool pick_new_path_when_sound_start;
        bool loop;
        uint32_t transition_time;
        std::vector<AudioPositioningSettingListenerRoutingPositionSourceAutomationPoint> point;
        std::vector<AudioPositioningSettingListenerRoutingPositionSourceAutomationPath> path;
    };

    inline auto to_json(
        nlohmann::ordered_json &nlohmann_json_j,
        const AudioPositioningSettingListenerRoutingPositionSourceAutomation &nlohmann_json_t) -> void
    {
        nlohmann_json_j["play_type"] = magic_enum::enum_name(nlohmann_json_t.play_type);
        nlohmann_json_j["play_mode"] = magic_enum::enum_name(nlohmann_json_t.play_mode);
        nlohmann_json_j["pick_new_path_when_sound_start"] = nlohmann_json_t.pick_new_path_when_sound_start;
        nlohmann_json_j["loop"] = nlohmann_json_t.loop;
        nlohmann_json_j["transition_time"] = nlohmann_json_t.transition_time;
        nlohmann_json_j["point"] = nlohmann_json_t.point;
        nlohmann_json_j["path"] = nlohmann_json_t.path;
        return;
    }

    inline auto from_json(
        const nlohmann::ordered_json &nlohmann_json_j,
        AudioPositioningSettingListenerRoutingPositionSourceAutomation &nlohmann_json_t) -> void
    {
        nlohmann_json_t.play_type = magic_enum_cast<AudioPlayType>(nlohmann_json_j.at("play_type").get<std::string>());
        nlohmann_json_t.play_mode = magic_enum_cast<AudioPlayMode>(nlohmann_json_j.at("play_mode").get<std::string>());
        nlohmann_json_j.at("pick_new_path_when_sound_start").get_to(nlohmann_json_t.pick_new_path_when_sound_start);
        nlohmann_json_j.at("loop").get_to(nlohmann_json_t.loop);
        nlohmann_json_j.at("transition_time").get_to(nlohmann_json_t.transition_time);
        nlohmann_json_j.at("point").get_to(nlohmann_json_t.point);
        nlohmann_json_j.at("path").get_to(nlohmann_json_t.path);
        return;
    }

    //-----------------------------

    struct AudioPositioningSettingListenerRoutingPositionSource
    {
        AudioPositioningSettingListenerRoutingPositionSourceMode mode;
        bool hold_listener_orientation;
        bool update_at_each_frame;
        bool hold_emitter_position_and_orientation;
        bool diffraction_and_transmission;
        AudioPositioningSettingListenerRoutingPositionSourceAutomation automation;
    };

    inline auto to_json(
        nlohmann::ordered_json &nlohmann_json_j,
        const AudioPositioningSettingListenerRoutingPositionSource &nlohmann_json_t) -> void
    {
        nlohmann_json_j["mode"] = magic_enum::enum_name(nlohmann_json_t.mode);
        nlohmann_json_j["hold_listener_orientation"] = nlohmann_json_t.hold_listener_orientation;
        if (k_version < 132_ui)
        {
            nlohmann_json_j["update_at_each_frame"] = nlohmann_json_t.update_at_each_frame;
        }
        else
        {
            nlohmann_json_j["hold_emitter_position_and_orientation"] = nlohmann_json_t.hold_emitter_position_and_orientation;
        }
        if (k_version >= 140_ui)
        {
            nlohmann_json_j["diffraction_and_transmission"] = nlohmann_json_t.diffraction_and_transmission;
        }
        nlohmann_json_j["automation"] = nlohmann_json_t.automation;
        return;
    }

    inline auto from_json(
        const nlohmann::ordered_json &nlohmann_json_j,
        AudioPositioningSettingListenerRoutingPositionSource &nlohmann_json_t) -> void
    {
        nlohmann_json_t.mode = magic_enum_cast<AudioPositioningSettingListenerRoutingPositionSourceMode>(nlohmann_json_j.at("mode").get<std::string>());
        nlohmann_json_j.at("hold_listener_orientation").get_to(nlohmann_json_t.hold_listener_orientation);
        if (k_version < 132_ui)
        {
            nlohmann_json_j.at("update_at_each_frame").get_to(nlohmann_json_t.update_at_each_frame);
        }
        else
        {
            nlohmann_json_j.at("hold_emitter_position_and_orientation").get_to(nlohmann_json_t.hold_emitter_position_and_orientation);
        }
        if (k_version >= 140_ui)
        {
            nlohmann_json_j.at("diffraction_and_transmission").get_to(nlohmann_json_t.diffraction_and_transmission);
        }
        nlohmann_json_j.at("automation").get_to(nlohmann_json_t.automation);
        return;
    }

    //-----------------------------

    struct AudioPositioningSettingListenerRouting
    {
        bool enable;
        bool spatialization_bool;
        AudioPositioningSettingListenerRoutingSpatialization spatialization;
        float speaker_panning_divsion_spatialization_mix;
        AudioPositioningSettingListenerRoutingAttenuation attenuation;
        AudioPositioningSettingListenerRoutingPositionSource position_source;
    };

    inline auto to_json(
        nlohmann::ordered_json &nlohmann_json_j,
        const AudioPositioningSettingListenerRouting &nlohmann_json_t) -> void
    {
        if (k_version < 128_ui)
        {
            nlohmann_json_j["spatialization_bool"] = nlohmann_json_t.spatialization_bool;
        }
        else
        {
            if (k_version >= 132_ui)
            {
                nlohmann_json_j["enable"] = nlohmann_json_t.enable;
                nlohmann_json_j["speaker_panning_divsion_spatialization_mix"] = nlohmann_json_t.speaker_panning_divsion_spatialization_mix;
            }
            nlohmann_json_j["spatialization"] = magic_enum::enum_name(nlohmann_json_t.spatialization);
        }
        nlohmann_json_j["attenuation"] = nlohmann_json_t.attenuation;
        nlohmann_json_j["position_source"] = nlohmann_json_t.position_source;
        return;
    }

    inline auto from_json(
        const nlohmann::ordered_json &nlohmann_json_j,
        AudioPositioningSettingListenerRouting &nlohmann_json_t) -> void
    {
        if (k_version < 128_ui)
        {
            nlohmann_json_j.at("spatialization_bool").get_to(nlohmann_json_t.spatialization_bool);
        }
        else
        {
            if (k_version >= 132_ui)
            {
                nlohmann_json_j.at("enable").get_to(nlohmann_json_t.enable);
                nlohmann_json_j.at("speaker_panning_divsion_spatialization_mix").get_to(nlohmann_json_t.speaker_panning_divsion_spatialization_mix);
            }
            nlohmann_json_t.spatialization = magic_enum_cast<AudioPositioningSettingListenerRoutingSpatialization>(nlohmann_json_j.at("spatialization").get<std::string>());
        }
        nlohmann_json_j.at("attenuation").get_to(nlohmann_json_t.attenuation);
        nlohmann_json_j.at("position_source").get_to(nlohmann_json_t.position_source);
        return;
    }

    //------------------------------

    struct AudioPositioningSetting
    {
        bool enable;
        AudioPositioningSettingType type;
        float center_percent;
        AudioPositioningSettingSpeakerPanning speaker_panning;
        AudioPositioningSettingListenerRouting listener_routing;
    };

    inline auto to_json(
        nlohmann::ordered_json &nlohmann_json_j,
        const AudioPositioningSetting &nlohmann_json_t) -> void
    {
        if (k_version >= 125_ui && k_version < 132_ui)
        {
            nlohmann_json_j["enable"] = nlohmann_json_t.enable;
        }
        if (k_version < 132_ui)
        {
            nlohmann_json_j["type"] = magic_enum::enum_name(nlohmann_json_t.type);
        }
        nlohmann_json_j["center_percent"] = nlohmann_json_t.center_percent;
        nlohmann_json_j["speaker_panning"] = nlohmann_json_t.speaker_panning;
        nlohmann_json_j["listener_routing"] = nlohmann_json_t.listener_routing;
        return;
    }

    inline auto from_json(
        const nlohmann::ordered_json &nlohmann_json_j,
        AudioPositioningSetting &nlohmann_json_t) -> void
    {
        if (k_version >= 125_ui && k_version < 132_ui)
        {
            nlohmann_json_j.at("enable").get_to(nlohmann_json_t.enable);
        }
        if (k_version < 132_ui)
        {
            nlohmann_json_t.type = magic_enum_cast<AudioPositioningSettingType>(nlohmann_json_j.at("type").get<std::string>());
        }
        nlohmann_json_j.at("center_percent").get_to(nlohmann_json_t.center_percent);
        nlohmann_json_j.at("speaker_panning").get_to(nlohmann_json_t.speaker_panning);
        nlohmann_json_j.at("listener_routing").get_to(nlohmann_json_t.listener_routing);
        return;
    }

    //------------------------------

    struct AudioMotionSetting
    {
        RandomizableValue<float> volume_offset;
        RandomizableValue<float> low_pass_filter;
    };

    inline auto to_json(
        nlohmann::ordered_json &nlohmann_json_j,
        const AudioMotionSetting &nlohmann_json_t) -> void
    {
        nlohmann_json_j["volume_offset"] = nlohmann_json_t.volume_offset;
        nlohmann_json_j["low_pass_filter"] = nlohmann_json_t.low_pass_filter;
        return;
    }

    inline auto from_json(
        const nlohmann::ordered_json &nlohmann_json_j,
        AudioMotionSetting &nlohmann_json_t) -> void
    {
        nlohmann_json_j.at("volume_offset").get_to(nlohmann_json_t.volume_offset);
        nlohmann_json_j.at("low_pass_filter").get_to(nlohmann_json_t.low_pass_filter);
        return;
    }

    //------------------------------

    struct Parameter
    {
        uint32_t id;
        ParameterCategory category;
    };

    inline auto to_json(
        nlohmann::ordered_json &nlohmann_json_j,
        const Parameter &nlohmann_json_t) -> void
    {
        nlohmann_json_j["id"] = nlohmann_json_t.id;
        if (k_version >= 112_ui)
        {
            nlohmann_json_j["category"] = magic_enum::enum_name(nlohmann_json_t.category);
        }
        return;
    }

    inline auto from_json(
        const nlohmann::ordered_json &nlohmann_json_j,
        Parameter &nlohmann_json_t) -> void
    {
        nlohmann_json_j.at("id").get_to(nlohmann_json_t.id);
        if (k_version >= 112_ui)
        {
            nlohmann_json_t.category = magic_enum_cast<ParameterCategory>(nlohmann_json_j.at("category").get<std::string>());
        }
        return;
    }

    //------------------------------

    struct RealTimeParameterControlSettingItem
    {
        uint32_t type;
        Parameter parameter;
        PropertyCategory u1;
        uint32_t u2;
        CoordinateMode mode;
        std::vector<CoordinatePoint> point;
    };

    inline auto to_json(
        nlohmann::ordered_json &nlohmann_json_j,
        const RealTimeParameterControlSettingItem &nlohmann_json_t) -> void
    {
        nlohmann_json_j["type"] = nlohmann_json_t.type;
        nlohmann_json_j["parameter"] = nlohmann_json_t.parameter;
        if (k_version >= 112)
        {
            nlohmann_json_j["u1"] = magic_enum::enum_name(nlohmann_json_t.u1);
        }
        nlohmann_json_j["u2"] = nlohmann_json_t.u2;
        nlohmann_json_j["mode"] = magic_enum::enum_name(nlohmann_json_t.mode);
        nlohmann_json_j["point"] = nlohmann_json_t.point;
        return;
    }

    inline auto from_json(
        const nlohmann::ordered_json &nlohmann_json_j,
        RealTimeParameterControlSettingItem &nlohmann_json_t) -> void
    {
        nlohmann_json_j.at("type").get_to(nlohmann_json_t.type);
        nlohmann_json_j.at("parameter").get_to(nlohmann_json_t.parameter);
        if (k_version >= 112)
        {
            nlohmann_json_t.u1 = magic_enum_cast<PropertyCategory>(nlohmann_json_j.at("u1").get<std::string>());
        }
        nlohmann_json_j.at("u2").get_to(nlohmann_json_t.u2);
        nlohmann_json_t.mode = magic_enum_cast<CoordinateMode>(nlohmann_json_j.at("mode").get<std::string>());
        nlohmann_json_j.at("point").get_to(nlohmann_json_t.point);
        return;
    }

    //------------------------------

    struct RealTimeParameterControlSetting
    {
        std::vector<RealTimeParameterControlSettingItem> item;
    };

    inline auto to_json(
        nlohmann::ordered_json &nlohmann_json_j,
        const RealTimeParameterControlSetting &nlohmann_json_t) -> void
    {
        nlohmann_json_j["item"] = nlohmann_json_t.item;
        return;
    }

    inline auto from_json(
        const nlohmann::ordered_json &nlohmann_json_j,
        RealTimeParameterControlSetting &nlohmann_json_t) -> void
    {
        nlohmann_json_j.at("item").get_to(nlohmann_json_t.item);
        return;
    }

    //------------------------------

    struct StateSettingApplyItem
    {
        uint32_t target;
        uint32_t setting;
    };

    inline auto to_json(
        nlohmann::ordered_json &nlohmann_json_j,
        const StateSettingApplyItem &nlohmann_json_t) -> void
    {
        nlohmann_json_j["target"] = nlohmann_json_t.target;
        nlohmann_json_j["setting"] = nlohmann_json_t.setting;
        return;
    }

    inline auto from_json(
        const nlohmann::ordered_json &nlohmann_json_j,
        StateSettingApplyItem &nlohmann_json_t) -> void
    {
        nlohmann_json_j.at("target").get_to(nlohmann_json_t.target);
        nlohmann_json_j.at("setting").get_to(nlohmann_json_t.setting);
        return;
    }

    //------------------------------

    struct StateSettingAttribute
    {
        uint32_t type;
        PropertyCategory category;
        uint32_t u1;
    };

    inline auto to_json(
        nlohmann::ordered_json &nlohmann_json_j,
        const StateSettingAttribute &nlohmann_json_t) -> void
    {
        if (k_version >= 125_ui)
        {
            nlohmann_json_j["type"] = nlohmann_json_t.type;
            nlohmann_json_j["category"] = nlohmann_json_t.category;
        }
        if (k_version >= 128_ui)
        {
            nlohmann_json_j["u1"] = nlohmann_json_t.u1;
        }
        return;
    }

    inline auto from_json(
        const nlohmann::ordered_json &nlohmann_json_j,
        StateSettingAttribute &nlohmann_json_t) -> void
    {
        if (k_version >= 125_ui)
        {
            nlohmann_json_j.at("type").get_to(nlohmann_json_t.type);
            nlohmann_json_j.at("category").get_to(nlohmann_json_t.category);
        }
        if (k_version >= 128_ui)
        {
            nlohmann_json_j.at("u1").get_to(nlohmann_json_t.u1);
        }
        return;
    }

    //------------------------------

    struct StateSettingItem
    {
        uint32_t group;
        TimePoint change_occur_at;
        std::vector<StateSettingApplyItem> apply;
    };

    inline auto to_json(
        nlohmann::ordered_json &nlohmann_json_j,
        const StateSettingItem &nlohmann_json_t) -> void
    {
        nlohmann_json_j["group"] = nlohmann_json_t.group;
        nlohmann_json_j["change_occur_at"] = magic_enum::enum_name(nlohmann_json_t.change_occur_at);
        nlohmann_json_j["apply"] = nlohmann_json_t.apply;
        return;
    }

    inline auto from_json(
        const nlohmann::ordered_json &nlohmann_json_j,
        StateSettingItem &nlohmann_json_t) -> void
    {
        nlohmann_json_j.at("group").get_to(nlohmann_json_t.group);
        nlohmann_json_t.change_occur_at = magic_enum_cast<TimePoint>(nlohmann_json_j.at("change_occur_at").get<std::string>());
        nlohmann_json_j.at("apply").get_to(nlohmann_json_t.apply);
        return;
    }

    //------------------------------

    struct StateSetting
    {
        std::vector<StateSettingAttribute> attribute;
        std::vector<StateSettingItem> item;
    };

    inline auto to_json(
        nlohmann::ordered_json &nlohmann_json_j,
        const StateSetting &nlohmann_json_t) -> void
    {
        if (k_version >= 125_ui)
        {
            nlohmann_json_j["attribute"] = nlohmann_json_t.attribute;
        }
        nlohmann_json_j["item"] = nlohmann_json_t.item;
        return;
    }

    inline auto from_json(
        const nlohmann::ordered_json &nlohmann_json_j,
        StateSetting &nlohmann_json_t) -> void
    {
        if (k_version >= 125_ui)
        {
            nlohmann_json_j.at("attribute").get_to(nlohmann_json_t.attribute);
        }
        nlohmann_json_j.at("item").get_to(nlohmann_json_t.item);
        return;
    }

    //-----------------------------

    struct AudioHDRSettingEnvelopeTracking
    {
        bool enable;
        float active_range;
    };

    inline auto to_json(
        nlohmann::ordered_json &nlohmann_json_j,
        const AudioHDRSettingEnvelopeTracking &nlohmann_json_t) -> void
    {
        if (k_version >= 88_ui)
        {
            nlohmann_json_j["enable"] = nlohmann_json_t.enable;
            nlohmann_json_j["active_range"] = nlohmann_json_t.active_range;
        }
        return;
    }

    inline auto from_json(
        const nlohmann::ordered_json &nlohmann_json_j,
        AudioHDRSettingEnvelopeTracking &nlohmann_json_t) -> void
    {
        if (k_version >= 88_ui)
        {
            nlohmann_json_j.at("enable").get_to(nlohmann_json_t.enable);
            nlohmann_json_j.at("active_range").get_to(nlohmann_json_t.active_range);
        }
        return;
    }

    //-----------------------------

    struct AudioHDRSetting
    {
        AudioHDRSettingEnvelopeTracking envelope_tracking;
    };

    inline auto to_json(
        nlohmann::ordered_json &nlohmann_json_j,
        const AudioHDRSetting &nlohmann_json_t) -> void
    {
        if (k_version >= 88_ui)
        {
            nlohmann_json_j["envelope_tracking"] = nlohmann_json_t.envelope_tracking;
        }
        return;
    }

    inline auto from_json(
        const nlohmann::ordered_json &nlohmann_json_j,
        AudioHDRSetting &nlohmann_json_t) -> void
    {
        if (k_version >= 88_ui)
        {
            nlohmann_json_j.at("envelope_tracking").get_to(nlohmann_json_t.envelope_tracking);
        }
        return;
    }

    //------------------------------

    struct SoundMIDISettingEvent
    {
        SoundMIDISettingEventPlayOn play_on;
        bool break_on_note_off;
    };

    inline auto to_json(
        nlohmann::ordered_json &nlohmann_json_j,
        const SoundMIDISettingEvent &nlohmann_json_t) -> void
    {
        if (k_version >= 112_ui)
        {
            nlohmann_json_j["play_on"] = magic_enum::enum_name(nlohmann_json_t.play_on);
            nlohmann_json_j["break_on_note_off"] = nlohmann_json_t.break_on_note_off;
        }
        return;
    }

    inline auto from_json(
        const nlohmann::ordered_json &nlohmann_json_j,
        SoundMIDISettingEvent &nlohmann_json_t) -> void
    {
        if (k_version >= 112_ui)
        {
            nlohmann_json_t.play_on = magic_enum_cast<SoundMIDISettingEventPlayOn>(nlohmann_json_j.at("play_on").get<std::string>());
            nlohmann_json_j.at("break_on_note_off").get_to(nlohmann_json_t.break_on_note_off);
        }
        return;
    }

    //------------------------------

    struct SoundMIDISettingNoteTracking
    {
        bool enable;
        uint32_t root_note;
    };

    inline auto to_json(
        nlohmann::ordered_json &nlohmann_json_j,
        const SoundMIDISettingNoteTracking &nlohmann_json_t) -> void
    {
        if (k_version >= 112_ui)
        {
            nlohmann_json_j["enable"] = nlohmann_json_t.enable;
            nlohmann_json_j["root_note"] = nlohmann_json_t.root_note;
        }
        return;
    }

    inline auto from_json(
        const nlohmann::ordered_json &nlohmann_json_j,
        SoundMIDISettingNoteTracking &nlohmann_json_t) -> void
    {
        if (k_version >= 112_ui)
        {
            nlohmann_json_j.at("enable").get_to(nlohmann_json_t.enable);
            nlohmann_json_j.at("root_note").get_to(nlohmann_json_t.root_note);
        }
        return;
    }

    //------------------------------

    struct SoundMIDISettingTransformation
    {
        uint32_t transposition;
        uint32_t velocity_offset;
    };

    inline auto to_json(
        nlohmann::ordered_json &nlohmann_json_j,
        const SoundMIDISettingTransformation &nlohmann_json_t) -> void
    {
        if (k_version >= 112_ui)
        {
            nlohmann_json_j["transposition"] = nlohmann_json_t.transposition;
            nlohmann_json_j["velocity_offset"] = nlohmann_json_t.velocity_offset;
        }
        return;
    }

    inline auto from_json(
        const nlohmann::ordered_json &nlohmann_json_j,
        SoundMIDISettingTransformation &nlohmann_json_t) -> void
    {
        if (k_version >= 112_ui)
        {
            nlohmann_json_j.at("transposition").get_to(nlohmann_json_t.transposition);
            nlohmann_json_j.at("velocity_offset").get_to(nlohmann_json_t.velocity_offset);
        }
        return;
    }

    //------------------------------

    struct SoundMIDISettingFilter
    {
        uint32_t key_range_minimum;
        uint32_t key_range_maximum;
        uint32_t velocity_minimum;
        uint32_t velocity_maximum;
        uint32_t channel;
    };

    inline auto to_json(
        nlohmann::ordered_json &nlohmann_json_j,
        const SoundMIDISettingFilter &nlohmann_json_t) -> void
    {
        if (k_version >= 112_ui)
        {
            nlohmann_json_j["key_range_minimum"] = nlohmann_json_t.key_range_minimum;
            nlohmann_json_j["key_range_maximum"] = nlohmann_json_t.key_range_maximum;
            nlohmann_json_j["velocity_minimum"] = nlohmann_json_t.velocity_minimum;
            nlohmann_json_j["velocity_maximum"] = nlohmann_json_t.velocity_maximum;
            nlohmann_json_j["channel"] = nlohmann_json_t.channel;
        }
        return;
    }

    inline auto from_json(
        const nlohmann::ordered_json &nlohmann_json_j,
        SoundMIDISettingFilter &nlohmann_json_t) -> void
    {
        if (k_version >= 112_ui)
        {
            nlohmann_json_j.at("key_range_minimum").get_to(nlohmann_json_t.key_range_minimum);
            nlohmann_json_j.at("key_range_maximum").get_to(nlohmann_json_t.key_range_maximum);
            nlohmann_json_j.at("velocity_minimum").get_to(nlohmann_json_t.velocity_minimum);
            nlohmann_json_j.at("velocity_maximum").get_to(nlohmann_json_t.velocity_maximum);
            nlohmann_json_j.at("channel").get_to(nlohmann_json_t.channel);
        }
        return;
    }

    //------------------------------

    struct SoundMIDISetting
    {
        SoundMIDISettingEvent event;
        SoundMIDISettingNoteTracking note_tracking;
        SoundMIDISettingTransformation transformation;
        SoundMIDISettingFilter filter;
    };

    inline auto to_json(
        nlohmann::ordered_json &nlohmann_json_j,
        const SoundMIDISetting &nlohmann_json_t) -> void
    {
        if (k_version >= 112_ui)
        {
            nlohmann_json_j["event"] = nlohmann_json_t.event;
            nlohmann_json_j["note_tracking"] = nlohmann_json_t.note_tracking;
            nlohmann_json_j["transformation"] = nlohmann_json_t.transformation;
            nlohmann_json_j["filter"] = nlohmann_json_t.filter;
        }
        return;
    }

    inline auto from_json(
        const nlohmann::ordered_json &nlohmann_json_j,
        SoundMIDISetting &nlohmann_json_t) -> void
    {
        if (k_version >= 112_ui)
        {
            nlohmann_json_j.at("event").get_to(nlohmann_json_t.event);
            nlohmann_json_j.at("note_tracking").get_to(nlohmann_json_t.note_tracking);
            nlohmann_json_j.at("transformation").get_to(nlohmann_json_t.transformation);
            nlohmann_json_j.at("filter").get_to(nlohmann_json_t.filter);
        }
        return;
    }

    //-----------------------------

    struct MusicTransitionSettingItemSource
    {
        std::vector<uint32_t> id;
        TimePoint exit_source_at;
        uint32_t exit_source_at_custom_cue_match;
        bool play_post_exit;
        MusicTransitionSettingItemFade fade_out;
    };

    inline auto to_json(
        nlohmann::ordered_json &nlohmann_json_j,
        const MusicTransitionSettingItemSource &nlohmann_json_t) -> void
    {
        nlohmann_json_j["id"] = nlohmann_json_t.id;
        nlohmann_json_j["exit_source_at"] = magic_enum::enum_name(nlohmann_json_t.exit_source_at);
        nlohmann_json_j["exit_source_at_custom_cue_match"] = nlohmann_json_t.exit_source_at_custom_cue_match;
        nlohmann_json_j["play_post_exit"] = nlohmann_json_t.play_post_exit;
        nlohmann_json_j["fade_out"] = nlohmann_json_t.fade_out;
        return;
    }

    inline auto from_json(
        const nlohmann::ordered_json &nlohmann_json_j,
        MusicTransitionSettingItemSource &nlohmann_json_t) -> void
    {
        nlohmann_json_j.at("id").get_to(nlohmann_json_t.id);
        nlohmann_json_t.exit_source_at = magic_enum_cast<TimePoint>(nlohmann_json_j.at("exit_source_at").get<std::string>());
        nlohmann_json_j.at("exit_source_at_custom_cue_match").get_to(nlohmann_json_t.exit_source_at_custom_cue_match);
        nlohmann_json_j.at("play_post_exit").get_to(nlohmann_json_t.play_post_exit);
        nlohmann_json_j.at("fade_out").get_to(nlohmann_json_t.fade_out);
        return;
    }

    //------------------------------

    struct MusicTransitionSettingItemDestination
    {
        std::vector<uint32_t> id;
        MusicTransitionSettingJumpMode jump_to;
        MusicTransitionSettingSynchronizeMode synchronize_to;
        bool play_pre_entry;
        bool custom_cue_filter_match_source_cue_name;
        uint32_t custom_cue_filter_match_target;
        MusicTransitionSettingItemFade fade_in;
    };

    inline auto to_json(
        nlohmann::ordered_json &nlohmann_json_j,
        const MusicTransitionSettingItemDestination &nlohmann_json_t) -> void
    {
        nlohmann_json_j["id"] = nlohmann_json_t.id;
        if (k_version >= 134_ui)
        {
            nlohmann_json_j["jump_to"] = magic_enum::enum_name(nlohmann_json_t.jump_to);
        }
        nlohmann_json_j["synchronize_to"] = magic_enum::enum_name(nlohmann_json_t.synchronize_to);
        nlohmann_json_j["play_pre_entry"] = nlohmann_json_t.play_pre_entry;
        nlohmann_json_j["custom_cue_filter_match_source_cue_name"] = nlohmann_json_t.custom_cue_filter_match_source_cue_name;
        nlohmann_json_j["custom_cue_filter_match_target"] = nlohmann_json_t.custom_cue_filter_match_target;
        nlohmann_json_j["fade_in"] = nlohmann_json_t.fade_in;
        return;
    }

    inline auto from_json(
        const nlohmann::ordered_json &nlohmann_json_j,
        MusicTransitionSettingItemDestination &nlohmann_json_t) -> void
    {
        nlohmann_json_j.at("id").get_to(nlohmann_json_t.id);
        if (k_version >= 134_ui)
        {
            nlohmann_json_t.jump_to = magic_enum_cast<MusicTransitionSettingJumpMode>(nlohmann_json_j.at("jump_to").get<std::string>());
        }
        nlohmann_json_t.synchronize_to = magic_enum_cast<MusicTransitionSettingSynchronizeMode>(nlohmann_json_j.at("synchronize_to").get<std::string>());
        nlohmann_json_j.at("play_pre_entry").get_to(nlohmann_json_t.play_pre_entry);
        nlohmann_json_j.at("custom_cue_filter_match_source_cue_name").get_to(nlohmann_json_t.custom_cue_filter_match_source_cue_name);
        nlohmann_json_j.at("custom_cue_filter_match_target").get_to(nlohmann_json_t.custom_cue_filter_match_target);
        nlohmann_json_j.at("fade_in").get_to(nlohmann_json_t.fade_in);
        return;
    }

    //------------------------------

    struct MusicTransitionSettingItemSegment
    {
        bool enable;
        uint32_t id;
        bool play_pre_entry;
        MusicTransitionSettingItemFade fade_in;
        bool play_post_exit;
        MusicTransitionSettingItemFade fade_out;
    };

    inline auto to_json(
        nlohmann::ordered_json &nlohmann_json_j,
        const MusicTransitionSettingItemSegment &nlohmann_json_t) -> void
    {
        nlohmann_json_j["enable"] = nlohmann_json_t.enable;
        nlohmann_json_j["id"] = nlohmann_json_t.id;
        nlohmann_json_j["play_pre_entry"] = nlohmann_json_t.play_pre_entry;
        nlohmann_json_j["fade_in"] = nlohmann_json_t.fade_in;
        nlohmann_json_j["play_post_exit"] = nlohmann_json_t.play_post_exit;
        nlohmann_json_j["fade_out"] = nlohmann_json_t.fade_out;
        return;
    }

    inline auto from_json(
        const nlohmann::ordered_json &nlohmann_json_j,
        MusicTransitionSettingItemSegment &nlohmann_json_t) -> void
    {
        nlohmann_json_j.at("enable").get_to(nlohmann_json_t.enable);
        nlohmann_json_j.at("id").get_to(nlohmann_json_t.id);
        nlohmann_json_j.at("play_pre_entry").get_to(nlohmann_json_t.play_pre_entry);
        nlohmann_json_j.at("fade_in").get_to(nlohmann_json_t.fade_in);
        nlohmann_json_j.at("play_post_exit").get_to(nlohmann_json_t.play_post_exit);
        nlohmann_json_j.at("fade_out").get_to(nlohmann_json_t.fade_out);
        return;
    }

    //------------------------------

    struct MusicTransitionSettingItem
    {
        uint32_t u1;
        MusicTransitionSettingItemSource source;
        MusicTransitionSettingItemDestination destination;
        MusicTransitionSettingItemSegment segment;
    };

    inline auto to_json(
        nlohmann::ordered_json &nlohmann_json_j,
        const MusicTransitionSettingItem &nlohmann_json_t) -> void
    {
        nlohmann_json_j["u1"] = nlohmann_json_t.u1;
        nlohmann_json_j["source"] = nlohmann_json_t.source;
        nlohmann_json_j["destination"] = nlohmann_json_t.destination;
        nlohmann_json_j["segment"] = nlohmann_json_t.segment;
        return;
    }

    inline auto from_json(
        const nlohmann::ordered_json &nlohmann_json_j,
        MusicTransitionSettingItem &nlohmann_json_t) -> void
    {
        nlohmann_json_j.at("u1").get_to(nlohmann_json_t.u1);
        nlohmann_json_j.at("source").get_to(nlohmann_json_t.source);
        nlohmann_json_j.at("destination").get_to(nlohmann_json_t.destination);
        nlohmann_json_j.at("segment").get_to(nlohmann_json_t.segment);
        return;
    }

    //------------------------------

    struct MusicTransitionSetting
    {
        std::vector<MusicTransitionSettingItem> item;
    };

    inline auto to_json(
        nlohmann::ordered_json &nlohmann_json_j,
        const MusicTransitionSetting &nlohmann_json_t) -> void
    {
        nlohmann_json_j["item"] = nlohmann_json_t.item;
        return;
    }

    inline auto from_json(
        const nlohmann::ordered_json &nlohmann_json_j,
        MusicTransitionSetting &nlohmann_json_t) -> void
    {
        nlohmann_json_j.at("item").get_to(nlohmann_json_t.item);
        return;
    }

    //-----------------------------

    struct MusicMIDISettingTarget
    {
        uint32_t id;
    };

    inline auto to_json(
        nlohmann::ordered_json &nlohmann_json_j,
        const MusicMIDISettingTarget &nlohmann_json_t) -> void
    {
        if (k_version >= 112_ui)
        {
            nlohmann_json_j["id"] = nlohmann_json_t.id;
        }
        return;
    }

    inline auto from_json(
        const nlohmann::ordered_json &nlohmann_json_j,
        MusicMIDISettingTarget &nlohmann_json_t) -> void
    {
        if (k_version >= 112_ui)
        {
            nlohmann_json_j.at("id").get_to(nlohmann_json_t.id);
        }
        return;
    }

    //-----------------------------

    struct MusicMIDISettingClipTempo
    {
        MusicMIDISettingClipTempoSource source;
    };

    inline auto to_json(
        nlohmann::ordered_json &nlohmann_json_j,
        const MusicMIDISettingClipTempo &nlohmann_json_t) -> void
    {
        if (k_version >= 112)
        {
            nlohmann_json_j["source"] = magic_enum::enum_name(nlohmann_json_t.source);
        }
        return;
    }

    inline auto from_json(
        const nlohmann::ordered_json &nlohmann_json_j,
        MusicMIDISettingClipTempo &nlohmann_json_t) -> void
    {
        if (k_version >= 112_ui)
        {
            nlohmann_json_t.source = magic_enum_cast<MusicMIDISettingClipTempoSource>(nlohmann_json_j.at("source").get<std::string>());
        }
        return;
    }

    //-----------------------------

    struct MusicMIDISetting
    {
        MusicMIDISettingTarget target;
        MusicMIDISettingClipTempo clip_tempo;
    };

    inline auto to_json(
        nlohmann::ordered_json &nlohmann_json_j,
        const MusicMIDISetting &nlohmann_json_t) -> void
    {
        if (k_version >= 112_ui)
        {
            nlohmann_json_j["target"] = nlohmann_json_t.target;
            nlohmann_json_j["clip_tempo"] = nlohmann_json_t.clip_tempo;
        }
        return;
    }

    inline auto from_json(
        const nlohmann::ordered_json &nlohmann_json_j,
        MusicMIDISetting &nlohmann_json_t) -> void
    {
        if (k_version >= 112_ui)
        {
            nlohmann_json_j.at("target").get_to(nlohmann_json_t.target);
            nlohmann_json_j.at("clip_tempo").get_to(nlohmann_json_t.clip_tempo);
        }
        return;
    }

    //------------------------------

    struct AudioBusMuteForBackgroundMusic
    {
        bool enable;
    };

    inline auto to_json(
        nlohmann::ordered_json &nlohmann_json_j,
        const AudioBusMuteForBackgroundMusic &nlohmann_json_t) -> void
    {
        if (k_version >= 112_ui)
        {
            nlohmann_json_j["enable"] = nlohmann_json_t.enable;
        }
        return;
    }

    inline auto from_json(
        const nlohmann::ordered_json &nlohmann_json_j,
        AudioBusMuteForBackgroundMusic &nlohmann_json_t) -> void
    {
        if (k_version >= 112_ui)
        {
            nlohmann_json_j.at("enable").get_to(nlohmann_json_t.enable);
        }
        return;
    }

    //------------------------------

    struct AudioPlaybackLimitSetting
    {
        uint16_t value;
        AudioPlaybackLimitSettingScope scope;
        AudioPlaybackLimitSettingWhenLimitIsReached when_limit_is_reached;
        AudioPlaybackLimitSettingWhenPriorityIsEqual when_priority_is_equal;
    };

    inline auto to_json(
        nlohmann::ordered_json &nlohmann_json_j,
        const AudioPlaybackLimitSetting &nlohmann_json_t) -> void
    {
        nlohmann_json_j["value"] = nlohmann_json_t.value;
        nlohmann_json_j["scope"] = magic_enum::enum_name(nlohmann_json_t.scope);
        nlohmann_json_j["when_limit_is_reached"] = magic_enum::enum_name(nlohmann_json_t.when_limit_is_reached);
        nlohmann_json_j["when_priority_is_equal"] = magic_enum::enum_name(nlohmann_json_t.when_priority_is_equal);
        return;
    }

    inline auto from_json(
        const nlohmann::ordered_json &nlohmann_json_j,
        AudioPlaybackLimitSetting &nlohmann_json_t) -> void
    {
        nlohmann_json_j.at("value").get_to(nlohmann_json_t.value);
        nlohmann_json_t.scope = magic_enum_cast<AudioPlaybackLimitSettingScope>(nlohmann_json_j.at("scope").get<std::string>());
        nlohmann_json_t.when_limit_is_reached = magic_enum_cast<AudioPlaybackLimitSettingWhenLimitIsReached>(nlohmann_json_j.at("when_limit_is_reached").get<std::string>());
        nlohmann_json_t.when_priority_is_equal = magic_enum_cast<AudioPlaybackLimitSettingWhenPriorityIsEqual>(nlohmann_json_j.at("when_priority_is_equal").get<std::string>());
        return;
    }

    //-----------------------------

    struct AudioVirtualVoiceSetting
    {
        AudioVirtualVoiceSettingBehavior behavior;
        AudioVirtualVoiceSettingOnReturnToPhysical on_return_to_physical;
    };

    inline auto to_json(
        nlohmann::ordered_json &nlohmann_json_j,
        const AudioVirtualVoiceSetting &nlohmann_json_t) -> void
    {
        nlohmann_json_j["behavior"] = magic_enum::enum_name(nlohmann_json_t.behavior);
        nlohmann_json_j["on_return_to_physical"] = magic_enum::enum_name(nlohmann_json_t.on_return_to_physical);
        return;
    }

    inline auto from_json(
        const nlohmann::ordered_json &nlohmann_json_j,
        AudioVirtualVoiceSetting &nlohmann_json_t) -> void
    {
        nlohmann_json_t.behavior = magic_enum_cast<AudioVirtualVoiceSettingBehavior>(nlohmann_json_j.at("behavior").get<std::string>());
        nlohmann_json_t.on_return_to_physical = magic_enum_cast<AudioVirtualVoiceSettingOnReturnToPhysical>(nlohmann_json_j.at("on_return_to_physical").get<std::string>());
        return;
    }

    //-----------------------------

    struct AudioPlaybackPrioritySetting
    {
        float value;
        bool use_distance_factor;
        float offset_at_maximum_distance;
    };

    inline auto to_json(
        nlohmann::ordered_json &nlohmann_json_j,
        const AudioPlaybackPrioritySetting &nlohmann_json_t) -> void
    {
        nlohmann_json_j["value"] = nlohmann_json_t.value;
        nlohmann_json_j["use_distance_factor"] = nlohmann_json_t.use_distance_factor;
        nlohmann_json_j["offset_at_maximum_distance"] = nlohmann_json_t.offset_at_maximum_distance;
        return;
    }

    inline auto from_json(
        const nlohmann::ordered_json &nlohmann_json_j,
        AudioPlaybackPrioritySetting &nlohmann_json_t) -> void
    {
        nlohmann_json_j.at("value").get_to(nlohmann_json_t.value);
        nlohmann_json_j.at("use_distance_factor").get_to(nlohmann_json_t.use_distance_factor);
        nlohmann_json_j.at("offset_at_maximum_distance").get_to(nlohmann_json_t.offset_at_maximum_distance);
        return;
    }

    //-----------------------------

    struct AudioMixerSetting
    {
        uint32_t id;
    };

    inline auto to_json(
        nlohmann::ordered_json &nlohmann_json_j,
        const AudioMixerSetting &nlohmann_json_t) -> void
    {
        if (k_version >= 112 && k_version < 150)
        {
            nlohmann_json_j["id"] = nlohmann_json_t.id;
        }
        return;
    }

    inline auto from_json(
        const nlohmann::ordered_json &nlohmann_json_j,
        AudioMixerSetting &nlohmann_json_t) -> void
    {
        if (k_version >= 112 && k_version < 150)
        {
            nlohmann_json_j.at("id").get_to(nlohmann_json_t.id);
        }
        return;
    }

    //------------------------------

    struct AudioVoiceVolumeGainSetting
    {
        bool normalization;
        float make_up_value;
        RandomizableValue<float> make_up;
    };

    inline auto to_json(
        nlohmann::ordered_json &nlohmann_json_j,
        const AudioVoiceVolumeGainSetting &nlohmann_json_t) -> void
    {
        nlohmann_json_j["normalization"] = nlohmann_json_t.normalization;
        if (k_version >= 112_ui)
        {
            nlohmann_json_j["make_up"] = nlohmann_json_t.make_up;
        }
        else
        {
            nlohmann_json_j["make_up"] = nlohmann_json_t.make_up_value;
        }
        return;
    }

    inline auto from_json(
        const nlohmann::ordered_json &nlohmann_json_j,
        AudioVoiceVolumeGainSetting &nlohmann_json_t) -> void
    {
        nlohmann_json_j.at("normalization").get_to(nlohmann_json_t.normalization);
        if (k_version >= 112_ui)
        {
            nlohmann_json_j.at("make_up").get_to(nlohmann_json_t.make_up);
        }
        else
        {
            nlohmann_json_j.at("make_up").get_to(nlohmann_json_t.make_up_value);
        }
        return;
    }

    //------------------------------

#pragma region music track

    struct MusicTrack
    {
        uint32_t parent;
        std::vector<AudioSourceSetting> source;
        MusicTrackPlaybackSetting playback_setting;
        MusicTrackStream stream;
        AudioVoice voice;
        AudioVoiceVolumeGainSetting voice_volume_gain;
        AudioOutputBusSetting output_bus;
        AudioAuxiliarySendSetting auxiliary_send;
        AudioEffectSetting effect;
        AudioMetadataSetting metadata;
        AudioPositioningSetting positioning;
        RealTimeParameterControlSetting real_time_parameter_control;
        StateSetting state;
        AudioHDRSetting hdr;
        MusicMIDISetting midi;
        AudioPlaybackLimitSetting playback_limit;
        AudioVirtualVoiceSetting virtual_voice;
        AudioPlaybackPrioritySetting playback_priority;
        AudioMotionSetting motion;
        AudioMixerSetting mixer;
        bool override_voice_volume_loudness_normalization;
        bool override_game_defined_auxiliary_send;
        bool override_user_defined_auxiliary_send;
        bool override_early_reflection_auxiliary_send;
        bool override_effect;
        bool override_metadata;
        bool override_positioning;
        bool override_hdr_envelope_tracking;
        bool override_midi_target;
        bool override_midi_clip_tempo;
        bool override_playback_limit;
        bool override_virtual_voice;
        bool override_playback_priority;
        bool override_mixer;
    };

    inline auto to_json(
        nlohmann::ordered_json &nlohmann_json_j,
        const MusicTrack &nlohmann_json_t) -> void
    {
        nlohmann_json_j["parent"] = nlohmann_json_t.parent;
        nlohmann_json_j["source"] = nlohmann_json_t.source;
        nlohmann_json_j["playback_setting"] = nlohmann_json_t.playback_setting;
        nlohmann_json_j["stream"] = nlohmann_json_t.stream;
        nlohmann_json_j["voice"] = nlohmann_json_t.voice;
        if (k_version >= 88_ui) {
            nlohmann_json_j["voice_volume_gain"] = nlohmann_json_t.voice_volume_gain;
        }
        nlohmann_json_j["output_bus"] = nlohmann_json_t.output_bus;
        nlohmann_json_j["auxiliary_send"] = nlohmann_json_t.auxiliary_send;
        nlohmann_json_j["effect"] = nlohmann_json_t.effect;
        if (k_version >= 140_ui)
        {
            nlohmann_json_j["metadata"] = nlohmann_json_t.metadata;
        }
        nlohmann_json_j["positioning"] = nlohmann_json_t.positioning;
        nlohmann_json_j["real_time_parameter_control"] = nlohmann_json_t.real_time_parameter_control;
        nlohmann_json_j["state"] = nlohmann_json_t.state;
        if (k_version >= 88_ui)
        {
            nlohmann_json_j["hdr"] = nlohmann_json_t.hdr;
        }
        if (k_version >= 128_ui)
        {
            nlohmann_json_j["midi"] = nlohmann_json_t.midi;
        }
        nlohmann_json_j["playback_limit"] = nlohmann_json_t.playback_limit;
        nlohmann_json_j["virtual_voice"] = nlohmann_json_t.virtual_voice;
        nlohmann_json_j["playback_priority"] = nlohmann_json_t.playback_priority;
        if (k_version < 128_ui)
        {
            nlohmann_json_j["motion"] = nlohmann_json_t.motion;
        }
        if (k_version >= 112_ui)
        {
            nlohmann_json_j["mixer"] = nlohmann_json_t.mixer;
        }
        if (k_version >= 88_ui)
        {
            nlohmann_json_j["override_voice_volume_loudness_normalization"] = nlohmann_json_t.override_voice_volume_loudness_normalization;
        }
        nlohmann_json_j["override_game_defined_auxiliary_send"] = nlohmann_json_t.override_game_defined_auxiliary_send;
        nlohmann_json_j["override_user_defined_auxiliary_send"] = nlohmann_json_t.override_user_defined_auxiliary_send;
        if (k_version >= 135_ui)
        {
            nlohmann_json_j["override_early_reflection_auxiliary_send"] = nlohmann_json_t.override_early_reflection_auxiliary_send;
        }
        nlohmann_json_j["override_effect"] = nlohmann_json_t.override_effect;
        if (k_version >= 140_ui)
        {
            nlohmann_json_j["override_metadata"] = nlohmann_json_t.override_metadata;
        }
        nlohmann_json_j["override_positioning"] = nlohmann_json_t.override_positioning;
        if (k_version >= 88_ui)
        {
            nlohmann_json_j["override_hdr_envelope_tracking"] = nlohmann_json_t.override_hdr_envelope_tracking;
        }
        if (k_version >= 112_ui)
        {
            nlohmann_json_j["override_midi_target"] = nlohmann_json_t.override_midi_target;
            nlohmann_json_j["override_midi_clip_tempo"] = nlohmann_json_t.override_midi_clip_tempo;
        }
        nlohmann_json_j["override_playback_limit"] = nlohmann_json_t.override_playback_limit;
        nlohmann_json_j["override_virtual_voice"] = nlohmann_json_t.override_virtual_voice;
        nlohmann_json_j["override_playback_priority"] = nlohmann_json_t.override_playback_priority;
        if (k_version >= 112_ui && k_version < 150_ui)
        {
            nlohmann_json_j["override_mixer"] = nlohmann_json_t.override_mixer;
        }
        return;
    }

    inline auto from_json(
        const nlohmann::ordered_json &nlohmann_json_j,
        MusicTrack &nlohmann_json_t) -> void
    {
        nlohmann_json_j.at("parent").get_to(nlohmann_json_t.parent);
        nlohmann_json_j.at("source").get_to(nlohmann_json_t.source);
        nlohmann_json_j.at("playback_setting").get_to(nlohmann_json_t.playback_setting);
        nlohmann_json_j.at("stream").get_to(nlohmann_json_t.stream);
        nlohmann_json_j.at("voice").get_to(nlohmann_json_t.voice);
        if (k_version >= 88_ui) {
            nlohmann_json_j.at("voice_volume_gain").get_to(nlohmann_json_t.voice_volume_gain);
        }
        nlohmann_json_j.at("output_bus").get_to(nlohmann_json_t.output_bus);
        nlohmann_json_j.at("auxiliary_send").get_to(nlohmann_json_t.auxiliary_send);
        nlohmann_json_j.at("effect").get_to(nlohmann_json_t.effect);
        if (k_version >= 140_ui)
        {
            nlohmann_json_j.at("metadata").get_to(nlohmann_json_t.metadata);
        }
        nlohmann_json_j.at("positioning").get_to(nlohmann_json_t.positioning);
        nlohmann_json_j.at("real_time_parameter_control").get_to(nlohmann_json_t.real_time_parameter_control);
        nlohmann_json_j.at("state").get_to(nlohmann_json_t.state);
        if (k_version >= 88_ui)
        {
            nlohmann_json_j.at("hdr").get_to(nlohmann_json_t.hdr);
        }
        if (k_version >= 128_ui)
        {
            nlohmann_json_j.at("midi").get_to(nlohmann_json_t.midi);
        }
        nlohmann_json_j.at("playback_limit").get_to(nlohmann_json_t.playback_limit);
        nlohmann_json_j.at("virtual_voice").get_to(nlohmann_json_t.virtual_voice);
        nlohmann_json_j.at("playback_priority").get_to(nlohmann_json_t.playback_priority);
        
        if (k_version < 128_ui)
        {
            nlohmann_json_j.at("motion").get_to(nlohmann_json_t.motion);
        }
        if (k_version >= 112_ui)
        {
            nlohmann_json_j.at("mixer").get_to(nlohmann_json_t.mixer);
        }
        if (k_version >= 88_ui)
        {
            nlohmann_json_j.at("override_voice_volume_loudness_normalization").get_to(nlohmann_json_t.override_voice_volume_loudness_normalization);
        }
        nlohmann_json_j.at("override_game_defined_auxiliary_send").get_to(nlohmann_json_t.override_game_defined_auxiliary_send);
        nlohmann_json_j.at("override_user_defined_auxiliary_send").get_to(nlohmann_json_t.override_user_defined_auxiliary_send);
        if (k_version >= 135_ui)
        {
            nlohmann_json_j.at("override_early_reflection_auxiliary_send").get_to(nlohmann_json_t.override_early_reflection_auxiliary_send);
        }
        nlohmann_json_j.at("override_effect").get_to(nlohmann_json_t.override_effect);
        if (k_version >= 140_ui)
        {
            nlohmann_json_j.at("override_metadata").get_to(nlohmann_json_t.override_metadata);
        }
        nlohmann_json_j.at("override_positioning").get_to(nlohmann_json_t.override_positioning);
        
        if (k_version >= 88_ui) 
        {
            nlohmann_json_j.at("override_hdr_envelope_tracking").get_to(nlohmann_json_t.override_hdr_envelope_tracking);
        }
        if (k_version >= 112_ui) {
            nlohmann_json_j.at("override_midi_target").get_to(nlohmann_json_t.override_midi_target);
            nlohmann_json_j.at("override_midi_clip_tempo").get_to(nlohmann_json_t.override_midi_clip_tempo);
        }
        nlohmann_json_j.at("override_playback_limit").get_to(nlohmann_json_t.override_playback_limit);
        nlohmann_json_j.at("override_virtual_voice").get_to(nlohmann_json_t.override_virtual_voice);
        nlohmann_json_j.at("override_playback_priority").get_to(nlohmann_json_t.override_playback_priority);
        if (k_version >= 112_ui && k_version < 150_ui)
        {
            nlohmann_json_j.at("override_mixer").get_to(nlohmann_json_t.override_mixer);
        }
        return;
    }

#pragma region stateful property setting

    struct StatefulPropertySettingItem
    {
        uint16_t type;
        float value;
    };

    inline auto to_json(
        nlohmann::ordered_json &nlohmann_json_j,
        const StatefulPropertySettingItem &nlohmann_json_t) -> void
    {
        nlohmann_json_j["type"] = nlohmann_json_t.type;
        nlohmann_json_j["value"] = nlohmann_json_t.value;
        return;
    }

    inline auto from_json(
        const nlohmann::ordered_json &nlohmann_json_j,
        StatefulPropertySettingItem &nlohmann_json_t) -> void
    {
        nlohmann_json_j.at("type").get_to(nlohmann_json_t.type);
        nlohmann_json_j.at("value").get_to(nlohmann_json_t.value);
        return;
    }

    struct StatefulPropertySetting
    {
        std::vector<StatefulPropertySettingItem> value;
    };

    inline auto to_json(
        nlohmann::ordered_json &nlohmann_json_j,
        const StatefulPropertySetting &nlohmann_json_t) -> void
    {
        nlohmann_json_j["value"] = nlohmann_json_t.value;
        return;
    }

    inline auto from_json(
        const nlohmann::ordered_json &nlohmann_json_j,
        StatefulPropertySetting &nlohmann_json_t) -> void
    {
        nlohmann_json_j.at("value").get_to(nlohmann_json_t.value);
        return;
    }

#pragma endregion

#pragma region event action

    struct EventActionProperty
    {
        enum class ValueApplyMode : uint8_t
        {
            absolute,
            relative
        };

        enum class SeekType : uint8_t
        {
            time,
            percent,
        };

        enum class Type : uint8_t
        {
            play_audio,
            stop_audio,
            pause_audio,
            resume_audio,
            break_audio,
            seek_audio,
            set_bus_volume,
            set_voice_volume,
            set_voice_pitch,
            set_voice_low_pass_filter,
            set_mute,
            set_game_parameter,
            set_state_availability,
            activate_state,
            activate_switch,
            activate_trigger,
            set_bypass_effect,
            set_voice_high_pass_filter,
            release_envelope,
            post_event,
            reset_playlist
        };
    };

    struct EventActionException
    {
        uint32_t id;
        bool u1;
    };

    inline auto to_json(
        nlohmann::ordered_json &nlohmann_json_j,
        const EventActionException &nlohmann_json_t) -> void
    {
        nlohmann_json_j["id"] = nlohmann_json_t.id;
        nlohmann_json_j["u1"] = nlohmann_json_t.u1;
        return;
    }

    inline auto from_json(
        const nlohmann::ordered_json &nlohmann_json_j,
        EventActionException &nlohmann_json_t) -> void
    {
        nlohmann_json_j.at("id").get_to(nlohmann_json_t.id);
        nlohmann_json_j.at("u1").get_to(nlohmann_json_t.u1);
        return;
    }

    //------------------------------

    struct PlayAudio
    {
        RandomizableValue<uint32_t> delay;
        RandomizableValue<uint32_t> fade_time;
        Curve fade_curve;
        float probability;
        uint32_t sound_bank;
    };

    inline auto to_json(
        nlohmann::ordered_json &nlohmann_json_j,
        const PlayAudio &nlohmann_json_t) -> void
    {
        nlohmann_json_j["delay"] = nlohmann_json_t.delay;
        nlohmann_json_j["fade_time"] = nlohmann_json_t.fade_time;
        nlohmann_json_j["fade_curve"] = magic_enum::enum_name(nlohmann_json_t.fade_curve);
        nlohmann_json_j["probability"] = nlohmann_json_t.probability;
        nlohmann_json_j["sound_bank"] = nlohmann_json_t.sound_bank;
        return;
    }

    inline auto from_json(
        const nlohmann::ordered_json &nlohmann_json_j,
        PlayAudio &nlohmann_json_t) -> void
    {
        nlohmann_json_j.at("delay").get_to(nlohmann_json_t.delay);
        nlohmann_json_j.at("fade_time").get_to(nlohmann_json_t.fade_time);
        nlohmann_json_t.fade_curve = magic_enum_cast<Curve>(nlohmann_json_j.at("fade_curve").get<std::string>());
        nlohmann_json_j.at("probability").get_to(nlohmann_json_t.probability);
        nlohmann_json_j.at("sound_bank").get_to(nlohmann_json_t.sound_bank);
        return;
    }

    struct StopAudio
    {
        RandomizableValue<uint32_t> delay;
        RandomizableValue<uint32_t> fade_time;
        Curve fade_curve;
        bool resume_state_transition; //>= 125_ui;
        bool apply_to_dynamic_sequence;
    };

    inline auto to_json(
        nlohmann::ordered_json &nlohmann_json_j,
        const StopAudio &nlohmann_json_t) -> void
    {
        nlohmann_json_j["delay"] = nlohmann_json_t.delay;
        nlohmann_json_j["fade_time"] = nlohmann_json_t.fade_time;
        nlohmann_json_j["fade_curve"] = magic_enum::enum_name(nlohmann_json_t.fade_curve);
        if (k_version >= 125_ui)
        {
            nlohmann_json_j["resume_state_transition"] = nlohmann_json_t.resume_state_transition;
            nlohmann_json_j["apply_to_dynamic_sequence"] = nlohmann_json_t.apply_to_dynamic_sequence;
        }
        return;
    }

    inline auto from_json(
        const nlohmann::ordered_json &nlohmann_json_j,
        StopAudio &nlohmann_json_t) -> void
    {
        nlohmann_json_j.at("delay").get_to(nlohmann_json_t.delay);
        nlohmann_json_j.at("fade_time").get_to(nlohmann_json_t.fade_time);
        nlohmann_json_t.fade_curve = magic_enum_cast<Curve>(nlohmann_json_j.at("fade_curve").get<std::string>());
        if (k_version >= 125_ui)
        {
            nlohmann_json_j.at("resume_state_transition").get_to(nlohmann_json_t.resume_state_transition);
            nlohmann_json_j.at("apply_to_dynamic_sequence").get_to(nlohmann_json_t.apply_to_dynamic_sequence);
        }
        return;
    }

    struct PauseAudio
    {
        RandomizableValue<uint32_t> delay;
        RandomizableValue<uint32_t> fade_time;
        Curve fade_curve;
        bool include_delayed_resume_action;
        bool resume_state_transition;
        bool apply_to_dynamic_sequence;
    };

    inline auto to_json(
        nlohmann::ordered_json &nlohmann_json_j,
        const PauseAudio &nlohmann_json_t) -> void
    {
        nlohmann_json_j["delay"] = nlohmann_json_t.delay;
        nlohmann_json_j["fade_time"] = nlohmann_json_t.fade_time;
        nlohmann_json_j["fade_curve"] = magic_enum::enum_name(nlohmann_json_t.fade_curve);
        nlohmann_json_j["include_delayed_resume_action"] = nlohmann_json_t.include_delayed_resume_action;
        if (k_version >= 125_ui)
        {
            nlohmann_json_j["resume_state_transition"] = nlohmann_json_t.resume_state_transition;
            nlohmann_json_j["apply_to_dynamic_sequence"] = nlohmann_json_t.apply_to_dynamic_sequence;
        }
        return;
    }

    inline auto from_json(
        const nlohmann::ordered_json &nlohmann_json_j,
        PauseAudio &nlohmann_json_t) -> void
    {
        nlohmann_json_j.at("delay").get_to(nlohmann_json_t.delay);
        nlohmann_json_j.at("fade_time").get_to(nlohmann_json_t.fade_time);
        nlohmann_json_t.fade_curve = magic_enum_cast<Curve>(nlohmann_json_j.at("fade_curve").get<std::string>());
        nlohmann_json_j.at("include_delayed_resume_action").get_to(nlohmann_json_t.include_delayed_resume_action);
        if (k_version >= 125_ui)
        {
            nlohmann_json_j.at("resume_state_transition").get_to(nlohmann_json_t.resume_state_transition);
            nlohmann_json_j.at("apply_to_dynamic_sequence").get_to(nlohmann_json_t.apply_to_dynamic_sequence);
        }
        return;
    }

    struct ResumeAudio
    {
        RandomizableValue<uint32_t> delay;
        RandomizableValue<uint32_t> fade_time;
        Curve fade_curve;
        bool master_resume;
        bool resume_state_transition;
        bool apply_to_dynamic_sequence;
    };

    inline auto to_json(
        nlohmann::ordered_json &nlohmann_json_j,
        const ResumeAudio &nlohmann_json_t) -> void
    {
        nlohmann_json_j["delay"] = nlohmann_json_t.delay;
        nlohmann_json_j["fade_time"] = nlohmann_json_t.fade_time;
        nlohmann_json_j["fade_curve"] = magic_enum::enum_name(nlohmann_json_t.fade_curve);
        nlohmann_json_j["master_resume"] = nlohmann_json_t.master_resume;
        if (k_version >= 125_ui)
        {
            nlohmann_json_j["resume_state_transition"] = nlohmann_json_t.resume_state_transition;
            nlohmann_json_j["apply_to_dynamic_sequence"] = nlohmann_json_t.apply_to_dynamic_sequence;
        }
        return;
    }

    inline auto from_json(
        const nlohmann::ordered_json &nlohmann_json_j,
        ResumeAudio &nlohmann_json_t) -> void
    {
        nlohmann_json_j.at("delay").get_to(nlohmann_json_t.delay);
        nlohmann_json_j.at("fade_time").get_to(nlohmann_json_t.fade_time);
        nlohmann_json_t.fade_curve = magic_enum_cast<Curve>(nlohmann_json_j.at("fade_curve").get<std::string>());
        nlohmann_json_j.at("master_resume").get_to(nlohmann_json_t.master_resume);
        if (k_version >= 125_ui)
        {
            nlohmann_json_j.at("resume_state_transition").get_to(nlohmann_json_t.resume_state_transition);
            nlohmann_json_j.at("apply_to_dynamic_sequence").get_to(nlohmann_json_t.apply_to_dynamic_sequence);
        }
        return;
    }

    struct BreakAudio
    {
        RandomizableValue<uint32_t> delay;
    };

    inline auto to_json(
        nlohmann::ordered_json &nlohmann_json_j,
        const BreakAudio &nlohmann_json_t) -> void
    {
        nlohmann_json_j["delay"] = nlohmann_json_t.delay;
        return;
    }

    inline auto from_json(
        const nlohmann::ordered_json &nlohmann_json_j,
        BreakAudio &nlohmann_json_t) -> void
    {
        nlohmann_json_j.at("delay").get_to(nlohmann_json_t.delay);
        return;
    }

    struct SeekAudio
    {
        RandomizableValue<uint32_t> delay;
        EventActionProperty::SeekType seek_type;
        RandomizableValue<float> seek_value;
        bool seek_to_nearest_marker;
    };

    inline auto to_json(
        nlohmann::ordered_json &nlohmann_json_j,
        const SeekAudio &nlohmann_json_t) -> void
    {
        nlohmann_json_j["delay"] = nlohmann_json_t.delay;
        nlohmann_json_j["seek_type"] = nlohmann_json_t.seek_type;
        nlohmann_json_j["seek_value"] = nlohmann_json_t.seek_value;
        nlohmann_json_j["seek_to_nearest_marker"] = nlohmann_json_t.seek_to_nearest_marker;
        return;
    }

    inline auto from_json(
        const nlohmann::ordered_json &nlohmann_json_j,
        SeekAudio &nlohmann_json_t) -> void
    {
        nlohmann_json_j.at("delay").get_to(nlohmann_json_t.delay);
        nlohmann_json_j.at("seek_type").get_to(nlohmann_json_t.seek_type);
        nlohmann_json_j.at("seek_value").get_to(nlohmann_json_t.seek_value);
        nlohmann_json_j.at("seek_to_nearest_marker").get_to(nlohmann_json_t.seek_to_nearest_marker);
        return;
    }

    struct PostEvent
    {
        RandomizableValue<uint32_t> delay;
    };

    inline auto to_json(
        nlohmann::ordered_json &nlohmann_json_j,
        const PostEvent &nlohmann_json_t) -> void
    {
        if (k_version >= 113_ui)
        {
            nlohmann_json_j["delay"] = nlohmann_json_t.delay;
        }
        return;
    }

    inline auto from_json(
        const nlohmann::ordered_json &nlohmann_json_j,
        PostEvent &nlohmann_json_t) -> void
    {
        if (k_version >= 113_ui)
        {
            nlohmann_json_j.at("delay").get_to(nlohmann_json_t.delay);
        }
        return;
    }

    struct SetBusVolume
    {
        bool reset;
        RandomizableValue<uint32_t> delay;
        RandomizableValue<uint32_t> fade_time;
        Curve fade_curve;
        EventActionProperty::ValueApplyMode apply_mode;
        RandomizableValue<float> value;
    };

    inline auto to_json(
        nlohmann::ordered_json &nlohmann_json_j,
        const SetBusVolume &nlohmann_json_t) -> void
    {
        nlohmann_json_j["reset"] = nlohmann_json_t.reset;
        nlohmann_json_j["delay"] = nlohmann_json_t.delay;
        nlohmann_json_j["fade_time"] = nlohmann_json_t.fade_time;
        nlohmann_json_j["fade_curve"] = magic_enum::enum_name(nlohmann_json_t.fade_curve);
        nlohmann_json_j["apply_mode"] = nlohmann_json_t.apply_mode;
        nlohmann_json_j["value"] = nlohmann_json_t.value;
        return;
    }

    inline auto from_json(
        const nlohmann::ordered_json &nlohmann_json_j,
        SetBusVolume &nlohmann_json_t) -> void
    {
        nlohmann_json_j.at("reset").get_to(nlohmann_json_t.reset);
        nlohmann_json_j.at("delay").get_to(nlohmann_json_t.delay);
        nlohmann_json_j.at("fade_time").get_to(nlohmann_json_t.fade_time);
        nlohmann_json_t.fade_curve = magic_enum_cast<Curve>(nlohmann_json_j.at("fade_curve").get<std::string>());
        nlohmann_json_j.at("apply_mode").get_to(nlohmann_json_t.apply_mode);
        nlohmann_json_j.at("value").get_to(nlohmann_json_t.value);
        return;
    }

    struct SetVoiceVolume
    {
        bool reset;
        RandomizableValue<uint32_t> delay;
        RandomizableValue<uint32_t> fade_time;
        Curve fade_curve;
        EventActionProperty::ValueApplyMode apply_mode;
        RandomizableValue<float> value;
    };

    inline auto to_json(
        nlohmann::ordered_json &nlohmann_json_j,
        const SetVoiceVolume &nlohmann_json_t) -> void
    {
        nlohmann_json_j["reset"] = nlohmann_json_t.reset;
        nlohmann_json_j["delay"] = nlohmann_json_t.delay;
        nlohmann_json_j["fade_time"] = nlohmann_json_t.fade_time;
        nlohmann_json_j["fade_curve"] = magic_enum::enum_name(nlohmann_json_t.fade_curve);
        nlohmann_json_j["apply_mode"] = nlohmann_json_t.apply_mode;
        nlohmann_json_j["value"] = nlohmann_json_t.value;
        return;
    }

    inline auto from_json(
        const nlohmann::ordered_json &nlohmann_json_j,
        SetVoiceVolume &nlohmann_json_t) -> void
    {
        nlohmann_json_j.at("reset").get_to(nlohmann_json_t.reset);
        nlohmann_json_j.at("delay").get_to(nlohmann_json_t.delay);
        nlohmann_json_j.at("fade_time").get_to(nlohmann_json_t.fade_time);
        nlohmann_json_t.fade_curve = magic_enum_cast<Curve>(nlohmann_json_j.at("fade_curve").get<std::string>());
        nlohmann_json_j.at("apply_mode").get_to(nlohmann_json_t.apply_mode);
        nlohmann_json_j.at("value").get_to(nlohmann_json_t.value);
        return;
    }

    struct SetVolumePitch
    {
        bool reset;
        RandomizableValue<uint32_t> delay;
        RandomizableValue<uint32_t> fade_time;
        Curve fade_curve;
        EventActionProperty::ValueApplyMode apply_mode;
        RandomizableValue<float> value;
    };

    inline auto to_json(
        nlohmann::ordered_json &nlohmann_json_j,
        const SetVolumePitch &nlohmann_json_t) -> void
    {
        nlohmann_json_j["reset"] = nlohmann_json_t.reset;
        nlohmann_json_j["delay"] = nlohmann_json_t.delay;
        nlohmann_json_j["fade_time"] = nlohmann_json_t.fade_time;
        nlohmann_json_j["fade_curve"] = magic_enum::enum_name(nlohmann_json_t.fade_curve);
        nlohmann_json_j["apply_mode"] = nlohmann_json_t.apply_mode;
        nlohmann_json_j["value"] = nlohmann_json_t.value;
        return;
    }

    inline auto from_json(
        const nlohmann::ordered_json &nlohmann_json_j,
        SetVolumePitch &nlohmann_json_t) -> void
    {
        nlohmann_json_j.at("reset").get_to(nlohmann_json_t.reset);
        nlohmann_json_j.at("delay").get_to(nlohmann_json_t.delay);
        nlohmann_json_j.at("fade_time").get_to(nlohmann_json_t.fade_time);
        nlohmann_json_t.fade_curve = magic_enum_cast<Curve>(nlohmann_json_j.at("fade_curve").get<std::string>());
        nlohmann_json_j.at("apply_mode").get_to(nlohmann_json_t.apply_mode);
        nlohmann_json_j.at("value").get_to(nlohmann_json_t.value);
        return;
    }

    struct SetVolumeLowPassFilter
    {
        bool reset;
        RandomizableValue<uint32_t> delay;
        RandomizableValue<uint32_t> fade_time;
        Curve fade_curve;
        EventActionProperty::ValueApplyMode apply_mode;
        RandomizableValue<float> value;
    };

    inline auto to_json(
        nlohmann::ordered_json &nlohmann_json_j,
        const SetVolumeLowPassFilter &nlohmann_json_t) -> void
    {
        nlohmann_json_j["reset"] = nlohmann_json_t.reset;
        nlohmann_json_j["delay"] = nlohmann_json_t.delay;
        nlohmann_json_j["fade_time"] = nlohmann_json_t.fade_time;
        nlohmann_json_j["fade_curve"] = magic_enum::enum_name(nlohmann_json_t.fade_curve);
        nlohmann_json_j["apply_mode"] = nlohmann_json_t.apply_mode;
        nlohmann_json_j["value"] = nlohmann_json_t.value;
        return;
    }

    inline auto from_json(
        const nlohmann::ordered_json &nlohmann_json_j,
        SetVolumeLowPassFilter &nlohmann_json_t) -> void
    {
        nlohmann_json_j.at("reset").get_to(nlohmann_json_t.reset);
        nlohmann_json_j.at("delay").get_to(nlohmann_json_t.delay);
        nlohmann_json_j.at("fade_time").get_to(nlohmann_json_t.fade_time);
        nlohmann_json_t.fade_curve = magic_enum_cast<Curve>(nlohmann_json_j.at("fade_curve").get<std::string>());
        nlohmann_json_j.at("apply_mode").get_to(nlohmann_json_t.apply_mode);
        nlohmann_json_j.at("value").get_to(nlohmann_json_t.value);
        return;
    }

    struct SetVolumeHighPassFilter
    {
        bool reset;
        RandomizableValue<uint32_t> delay;
        RandomizableValue<uint32_t> fade_time;
        Curve fade_curve;
        EventActionProperty::ValueApplyMode apply_mode;
        RandomizableValue<float> value;
    };

    inline auto to_json(
        nlohmann::ordered_json &nlohmann_json_j,
        const SetVolumeHighPassFilter &nlohmann_json_t) -> void
    {
        nlohmann_json_j["reset"] = nlohmann_json_t.reset;
        nlohmann_json_j["delay"] = nlohmann_json_t.delay;
        nlohmann_json_j["fade_time"] = nlohmann_json_t.fade_time;
        nlohmann_json_j["fade_curve"] = magic_enum::enum_name(nlohmann_json_t.fade_curve);
        nlohmann_json_j["apply_mode"] = nlohmann_json_t.apply_mode;
        nlohmann_json_j["value"] = nlohmann_json_t.value;
        return;
    }

    inline auto from_json(
        const nlohmann::ordered_json &nlohmann_json_j,
        SetVolumeHighPassFilter &nlohmann_json_t) -> void
    {
        nlohmann_json_j.at("reset").get_to(nlohmann_json_t.reset);
        nlohmann_json_j.at("delay").get_to(nlohmann_json_t.delay);
        nlohmann_json_j.at("fade_time").get_to(nlohmann_json_t.fade_time);
        nlohmann_json_t.fade_curve = magic_enum_cast<Curve>(nlohmann_json_j.at("fade_curve").get<std::string>());
        nlohmann_json_j.at("apply_mode").get_to(nlohmann_json_t.apply_mode);
        nlohmann_json_j.at("value").get_to(nlohmann_json_t.value);
        return;
    }

    struct SetMute
    {
        bool reset;
        RandomizableValue<uint32_t> delay;
        RandomizableValue<uint32_t> fade_time;
        Curve fade_curve;
    };

    inline auto to_json(
        nlohmann::ordered_json &nlohmann_json_j,
        const SetMute &nlohmann_json_t) -> void
    {
        nlohmann_json_j["reset"] = nlohmann_json_t.reset;
        nlohmann_json_j["delay"] = nlohmann_json_t.delay;
        nlohmann_json_j["fade_time"] = nlohmann_json_t.fade_time;
        nlohmann_json_j["fade_curve"] = magic_enum::enum_name(nlohmann_json_t.fade_curve);
        return;
    }

    inline auto from_json(
        const nlohmann::ordered_json &nlohmann_json_j,
        SetMute &nlohmann_json_t) -> void
    {
        nlohmann_json_j.at("reset").get_to(nlohmann_json_t.reset);
        nlohmann_json_j.at("delay").get_to(nlohmann_json_t.delay);
        nlohmann_json_j.at("fade_time").get_to(nlohmann_json_t.fade_time);
        nlohmann_json_t.fade_curve = magic_enum_cast<Curve>(nlohmann_json_j.at("fade_curve").get<std::string>());
        return;
    }

    struct SetGameParameter
    {
        bool reset;
        RandomizableValue<uint32_t> delay;
        RandomizableValue<uint32_t> fade_time;
        Curve fade_curve;
        EventActionProperty::ValueApplyMode apply_mode;
        RandomizableValue<float> value;
        bool bypass_game_parameter_interpolation;
    };

    inline auto to_json(
        nlohmann::ordered_json &nlohmann_json_j,
        const SetGameParameter &nlohmann_json_t) -> void
    {
        nlohmann_json_j["reset"] = nlohmann_json_t.reset;
        nlohmann_json_j["delay"] = nlohmann_json_t.delay;
        nlohmann_json_j["fade_time"] = nlohmann_json_t.fade_time;
        nlohmann_json_j["fade_curve"] = magic_enum::enum_name(nlohmann_json_t.fade_curve);
        nlohmann_json_j["apply_mode"] = nlohmann_json_t.apply_mode;
        nlohmann_json_j["value"] = nlohmann_json_t.value;
        if (k_version >= 112_ui)
        {
            nlohmann_json_j["bypass_game_parameter_interpolation"] = nlohmann_json_t.bypass_game_parameter_interpolation;
        }
        return;
    }

    inline auto from_json(
        const nlohmann::ordered_json &nlohmann_json_j,
        SetGameParameter &nlohmann_json_t) -> void
    {
        nlohmann_json_j.at("reset").get_to(nlohmann_json_t.reset);
        nlohmann_json_j.at("delay").get_to(nlohmann_json_t.delay);
        nlohmann_json_j.at("fade_time").get_to(nlohmann_json_t.fade_time);
        nlohmann_json_t.fade_curve = magic_enum_cast<Curve>(nlohmann_json_j.at("fade_curve").get<std::string>());
        nlohmann_json_j.at("apply_mode").get_to(nlohmann_json_t.apply_mode);
        nlohmann_json_j.at("value").get_to(nlohmann_json_t.value);
        if (k_version >= 112_ui)
        {
            nlohmann_json_j.at("bypass_game_parameter_interpolation").get_to(nlohmann_json_t.bypass_game_parameter_interpolation);
        }
        return;
    }

    struct SetStateAvailability
    {
        bool enable;
        RandomizableValue<uint32_t> delay;
    };

    inline auto to_json(
        nlohmann::ordered_json &nlohmann_json_j,
        const SetStateAvailability &nlohmann_json_t) -> void
    {
        nlohmann_json_j["enable"] = nlohmann_json_t.enable;
        nlohmann_json_j["delay"] = nlohmann_json_t.delay;
        return;
    }

    inline auto from_json(
        const nlohmann::ordered_json &nlohmann_json_j,
        SetStateAvailability &nlohmann_json_t) -> void
    {
        nlohmann_json_j.at("enable").get_to(nlohmann_json_t.enable);
        nlohmann_json_j.at("delay").get_to(nlohmann_json_t.delay);
        return;
    }

    struct ActivateState
    {
        RandomizableValue<uint32_t> delay;
        uint32_t group;
        uint32_t item;
    };

    inline auto to_json(
        nlohmann::ordered_json &nlohmann_json_j,
        const ActivateState &nlohmann_json_t) -> void
    {
        nlohmann_json_j["delay"] = nlohmann_json_t.delay;
        nlohmann_json_j["group"] = nlohmann_json_t.group;
        nlohmann_json_j["item"] = nlohmann_json_t.item;
        return;
    }

    inline auto from_json(
        const nlohmann::ordered_json &nlohmann_json_j,
        ActivateState &nlohmann_json_t) -> void
    {
        nlohmann_json_j.at("delay").get_to(nlohmann_json_t.delay);
        nlohmann_json_j.at("group").get_to(nlohmann_json_t.group);
        nlohmann_json_j.at("item").get_to(nlohmann_json_t.item);
        return;
    }

    struct ActivateSwitch
    {
        RandomizableValue<uint32_t> delay;
        uint32_t group;
        uint32_t item;
    };

    inline auto to_json(
        nlohmann::ordered_json &nlohmann_json_j,
        const ActivateSwitch &nlohmann_json_t) -> void
    {
        nlohmann_json_j["delay"] = nlohmann_json_t.delay;
        nlohmann_json_j["group"] = nlohmann_json_t.group;
        nlohmann_json_j["item"] = nlohmann_json_t.item;
        return;
    }

    inline auto from_json(
        const nlohmann::ordered_json &nlohmann_json_j,
        ActivateSwitch &nlohmann_json_t) -> void
    {
        nlohmann_json_j.at("delay").get_to(nlohmann_json_t.delay);
        nlohmann_json_j.at("group").get_to(nlohmann_json_t.group);
        nlohmann_json_j.at("item").get_to(nlohmann_json_t.item);
        return;
    }

    struct ActivateTrigger
    {
        RandomizableValue<uint32_t> delay;
    };

    inline auto to_json(
        nlohmann::ordered_json &nlohmann_json_j,
        const ActivateTrigger &nlohmann_json_t) -> void
    {
        nlohmann_json_j["delay"] = nlohmann_json_t.delay;
        return;
    }

    inline auto from_json(
        const nlohmann::ordered_json &nlohmann_json_j,
        ActivateTrigger &nlohmann_json_t) -> void
    {
        nlohmann_json_j.at("delay").get_to(nlohmann_json_t.delay);
        return;
    }

    struct SetBypassEffect
    {
        bool reset;
        bool enable;
        RandomizableValue<uint32_t> delay;
        std::array<bool, 5> value;
    };

    inline auto to_json(
        nlohmann::ordered_json &nlohmann_json_j,
        const SetBypassEffect &nlohmann_json_t) -> void
    {
        nlohmann_json_j["reset"] = nlohmann_json_t.reset;
        nlohmann_json_j["enable"] = nlohmann_json_t.enable;
        nlohmann_json_j["delay"] = nlohmann_json_t.delay;
        nlohmann_json_j["value"] = nlohmann_json_t.value;
        return;
    }

    inline auto from_json(
        const nlohmann::ordered_json &nlohmann_json_j,
        SetBypassEffect &nlohmann_json_t) -> void
    {
        nlohmann_json_j.at("reset").get_to(nlohmann_json_t.reset);
        nlohmann_json_j.at("enable").get_to(nlohmann_json_t.enable);
        nlohmann_json_j.at("delay").get_to(nlohmann_json_t.delay);
        nlohmann_json_j.at("value").get_to(nlohmann_json_t.value);
        return;
    }

    struct ReleaseEnvelope
    {
        RandomizableValue<uint32_t> delay;
    };

    inline auto to_json(
        nlohmann::ordered_json &nlohmann_json_j,
        const ReleaseEnvelope &nlohmann_json_t) -> void
    {
        if (k_version >= 112_ui)
        {
            nlohmann_json_j["delay"] = nlohmann_json_t.delay;
        }
        return;
    }

    inline auto from_json(
        const nlohmann::ordered_json &nlohmann_json_j,
        ReleaseEnvelope &nlohmann_json_t) -> void
    {
        if (k_version >= 112_ui)
        {
            nlohmann_json_j.at("delay").get_to(nlohmann_json_t.delay);
        }
        return;
    }

    struct ResetPlaylist
    {
        RandomizableValue<uint32_t> delay;
    };

    inline auto to_json(
        nlohmann::ordered_json &nlohmann_json_j,
        const ResetPlaylist &nlohmann_json_t) -> void
    {
        if (k_version >= 113_ui)
        {
            nlohmann_json_j["delay"] = nlohmann_json_t.delay;
        }
        return;
    }

    inline auto from_json(
        const nlohmann::ordered_json &nlohmann_json_j,
        ResetPlaylist &nlohmann_json_t) -> void
    {
        if (k_version >= 113_ui)
        {
            nlohmann_json_j.at("delay").get_to(nlohmann_json_t.delay);
        }
        return;
    }

    //-------------------------

    struct EventAction
    {
        uint32_t target;
        EventActionMode mode;
        std::vector<EventActionException> exception;
        EventActionScope scope;
        uint32_t u1;

        //------------------
        uint8_t type;
        EventActionProperty::Type action_type;
        PlayAudio play_audio;
        StopAudio stop_audio;
        PauseAudio pause_audio;
        ResumeAudio resume_audio;
        BreakAudio break_audio;
        SeekAudio seek_audio;
        PostEvent post_event;
        SetVolumePitch set_voice_pitch;
        SetVoiceVolume set_voice_volume;
        SetBusVolume set_bus_volume;
        SetVolumeLowPassFilter set_voice_low_pass_filter;
        SetVolumeHighPassFilter set_voice_high_pass_filter;
        SetMute set_mute;
        SetGameParameter set_game_parameter;
        SetStateAvailability set_state_availability;
        ActivateState activate_state;
        ActivateSwitch activate_switch;
        ActivateTrigger activate_trigger;
        SetBypassEffect set_bypass_effect;
        ReleaseEnvelope release_envelope;
        ResetPlaylist reset_playlist;
    };

    inline auto to_json(
        nlohmann::ordered_json &nlohmann_json_j,
        const EventAction &nlohmann_json_t) -> void
    {
        nlohmann_json_j["target"] = nlohmann_json_t.target;
        nlohmann_json_j["mode"] = magic_enum::enum_name(nlohmann_json_t.mode);
        nlohmann_json_j["exception"] = nlohmann_json_t.exception;
        nlohmann_json_j["scope"] = magic_enum::enum_name(nlohmann_json_t.scope);
        nlohmann_json_j["u1"] = nlohmann_json_t.u1;
        nlohmann_json_j["property"] = nlohmann::ordered_json{
            {"type", magic_enum::enum_name(nlohmann_json_t.action_type)},
            {"value", nlohmann::ordered_json{}}};
        switch (nlohmann_json_t.type)
        {
        case 4_byte:
        {
            nlohmann_json_j["property"]["value"] = nlohmann_json_t.play_audio;
            break;
        }
        case 1_byte:
        {
            nlohmann_json_j["property"]["value"] = nlohmann_json_t.stop_audio;
            break;
        }
        case 2_byte:
        {
            nlohmann_json_j["property"]["value"] = nlohmann_json_t.pause_audio;
            break;
        }
        case 3_byte:
        {
            nlohmann_json_j["property"]["value"] = nlohmann_json_t.resume_audio;
            break;
        }
        case 28_byte:
        {
            nlohmann_json_j["property"]["value"] = nlohmann_json_t.break_audio;
            break;
        }
        case 30_byte:
        {
            nlohmann_json_j["property"]["value"] = nlohmann_json_t.seek_audio;
            break;
        }
        case 33_byte:
        {
            nlohmann_json_j["property"]["value"] = nlohmann_json_t.post_event;
            break;
        }
        case 8_byte:
        case 9_byte:
        {
            nlohmann_json_j["property"]["value"] = nlohmann_json_t.set_voice_pitch;
            break;
        }
        case 10_byte:
        case 11_byte:
        {
            nlohmann_json_j["property"]["value"] = nlohmann_json_t.set_voice_volume;
            break;
        }
        case 12_byte:
        case 13_byte:
        {
            nlohmann_json_j["property"]["value"] = nlohmann_json_t.set_bus_volume;
            break;
        }
        case 14_byte:
        case 15_byte:
        {
            nlohmann_json_j["property"]["value"] = nlohmann_json_t.set_voice_low_pass_filter;
            break;
        }
        case 32_byte:
        case 48_byte:
        {
            nlohmann_json_j["property"]["value"] = nlohmann_json_t.set_voice_high_pass_filter;
            break;
        }
        case 6_byte:
        case 7_byte:
        {
            nlohmann_json_j["property"]["value"] = nlohmann_json_t.set_mute;
            break;
        }
        case 19_byte:
        case 20_byte:
        {
            nlohmann_json_j["property"]["value"] = nlohmann_json_t.set_game_parameter;
            break;
        }
        case 16_byte:
        case 17_byte:
        {
            nlohmann_json_j["property"]["value"] = nlohmann_json_t.set_state_availability;
            break;
        }
        case 18_byte:
        {
            nlohmann_json_j["property"]["value"] = nlohmann_json_t.activate_state;
            break;
        }
        case 25_byte:
        {
            nlohmann_json_j["property"]["value"] = nlohmann_json_t.activate_switch;
            break;
        }
        case 29_byte:
        {
            nlohmann_json_j["property"]["value"] = nlohmann_json_t.activate_trigger;
            break;
        }
        case 26_byte:
        case 27_byte:
        {
            nlohmann_json_j["property"]["value"] = nlohmann_json_t.set_bypass_effect;
            break;
        }
        case 31_byte:
        {
            if (k_version >= 112_ui)
            {
                nlohmann_json_j["property"]["value"] = nlohmann_json_t.release_envelope;
            }

            break;
        }
        case 34_byte:
        {
            if (k_version >= 113_ui)
            {
                nlohmann_json_j["property"]["value"] = nlohmann_json_t.reset_playlist;
            }
            break;
        }
        }
        return;
    }

    inline auto from_json(
        const nlohmann::ordered_json &nlohmann_json_j,
        EventAction &nlohmann_json_t) -> void
    {
        nlohmann_json_j.at("target").get_to(nlohmann_json_t.target);
        nlohmann_json_t.mode = magic_enum_cast<EventActionMode>(nlohmann_json_j.at("mode").get<std::string>());
        nlohmann_json_j.at("exception").get_to(nlohmann_json_t.exception);
        nlohmann_json_t.scope = magic_enum_cast<EventActionScope>(nlohmann_json_j.at("scope").get<std::string>());
        nlohmann_json_j.at("u1").get_to(nlohmann_json_t.u1);
        nlohmann_json_t.action_type = magic_enum_cast<EventActionProperty::Type>(nlohmann_json_j.at("property").at("type").get<std::string>());
        switch (nlohmann_json_t.action_type)
        {
        case EventActionProperty::Type::play_audio:
        {
            nlohmann_json_j.at("property").at("value").get_to(nlohmann_json_t.play_audio);
            break;
        }
        case EventActionProperty::Type::stop_audio:
        {
            nlohmann_json_j.at("property").at("value").get_to(nlohmann_json_t.stop_audio);
            break;
        }
        case EventActionProperty::Type::pause_audio:
        {
            nlohmann_json_j.at("property").at("value").get_to(nlohmann_json_t.pause_audio);
            break;
        }
        case EventActionProperty::Type::resume_audio:
        {
            nlohmann_json_j.at("property").at("value").get_to(nlohmann_json_t.resume_audio);
            break;
        }
        case EventActionProperty::Type::break_audio:
        {
            nlohmann_json_j.at("property").at("value").get_to(nlohmann_json_t.break_audio);
            break;
        }
        case EventActionProperty::Type::seek_audio:
        {
            nlohmann_json_j.at("property").at("value").get_to(nlohmann_json_t.seek_audio);
            break;
        }
        case EventActionProperty::Type::post_event:
        {
            nlohmann_json_j.at("property").at("value").get_to(nlohmann_json_t.post_event);
            break;
        }
        case EventActionProperty::Type::set_voice_pitch:
        {
            nlohmann_json_j.at("property").at("value").get_to(nlohmann_json_t.set_voice_pitch);
            break;
        }
        case EventActionProperty::Type::set_voice_volume:
        {
            nlohmann_json_j.at("property").at("value").get_to(nlohmann_json_t.set_voice_volume);
            break;
        }
        case EventActionProperty::Type::set_bus_volume:
        {
            nlohmann_json_j.at("property").at("value").get_to(nlohmann_json_t.set_bus_volume);
            break;
        }
        case EventActionProperty::Type::set_voice_low_pass_filter:
        {
            nlohmann_json_j.at("property").at("value").get_to(nlohmann_json_t.set_voice_low_pass_filter);
            break;
        }
        case EventActionProperty::Type::set_voice_high_pass_filter:
        {
            nlohmann_json_j.at("property").at("value").get_to(nlohmann_json_t.set_voice_high_pass_filter);
            break;
        }
        case EventActionProperty::Type::set_mute:
        {
            nlohmann_json_j.at("property").at("value").get_to(nlohmann_json_t.set_mute);
            break;
        }
        case EventActionProperty::Type::set_game_parameter:
        {
            nlohmann_json_j.at("property").at("value").get_to(nlohmann_json_t.set_game_parameter);
            break;
        }
        case EventActionProperty::Type::set_state_availability:
        {
            nlohmann_json_j.at("property").at("value").get_to(nlohmann_json_t.set_state_availability);
            break;
        }
        case EventActionProperty::Type::activate_state:
        {
            nlohmann_json_j.at("property").at("value").get_to(nlohmann_json_t.activate_state);
            break;
        }
        case EventActionProperty::Type::activate_switch:
        {
            nlohmann_json_j.at("property").at("value").get_to(nlohmann_json_t.activate_switch);
            break;
        }
        case EventActionProperty::Type::activate_trigger:
        {
            nlohmann_json_j.at("property").at("value").get_to(nlohmann_json_t.activate_trigger);
            break;
        }
        case EventActionProperty::Type::set_bypass_effect:
        {
            nlohmann_json_j.at("property").at("value").get_to(nlohmann_json_t.set_bypass_effect);
            break;
        }
        case EventActionProperty::Type::release_envelope:
        {
            nlohmann_json_j.at("property").at("value").get_to(nlohmann_json_t.release_envelope);
            break;
        }
        case EventActionProperty::Type::reset_playlist:
        {
            nlohmann_json_j.at("property").at("value").get_to(nlohmann_json_t.reset_playlist);
            break;
        }
        }
        return;
    }

#pragma endregion

    //------------------------------------

    struct Event
    {
        std::vector<uint32_t> child;
    };

    inline auto to_json(
        nlohmann::ordered_json &nlohmann_json_j,
        const Event &nlohmann_json_t) -> void
    {
        nlohmann_json_j["child"] = nlohmann_json_t.child;
        return;
    }

    inline auto from_json(
        const nlohmann::ordered_json &nlohmann_json_j,
        Event &nlohmann_json_t) -> void
    {
        nlohmann_json_j.at("child").get_to(nlohmann_json_t.child);
        return;
    }

    //--------------------------

    struct AudioAssociationSettingArgument
    {
        uint32_t id;
        bool is_state;
    };

    inline auto to_json(
        nlohmann::ordered_json &nlohmann_json_j,
        const AudioAssociationSettingArgument &nlohmann_json_t) -> void
    {
        nlohmann_json_j["id"] = nlohmann_json_t.id;
        if (k_version >= 88_ui)
        {
            nlohmann_json_j["is_state"] = nlohmann_json_t.is_state;
        }
        return;
    }

    inline auto from_json(
        const nlohmann::ordered_json &nlohmann_json_j,
        AudioAssociationSettingArgument &nlohmann_json_t) -> void
    {
        nlohmann_json_j.at("id").get_to(nlohmann_json_t.id);
        if (k_version >= 88_ui)
        {
            nlohmann_json_j.at("is_state").get_to(nlohmann_json_t.is_state);
        }
        return;
    }

    //---------------------------

    struct AudioAssociationSettingPath
    {
        uint32_t key;
        uint32_t object;
        uint32_t weight;
        uint32_t probability;
    };

    inline auto to_json(
        nlohmann::ordered_json &nlohmann_json_j,
        const AudioAssociationSettingPath &nlohmann_json_t) -> void
    {
        nlohmann_json_j["key"] = nlohmann_json_t.key;
        nlohmann_json_j["object"] = nlohmann_json_t.object;
        nlohmann_json_j["weight"] = nlohmann_json_t.weight;
        nlohmann_json_j["probability"] = nlohmann_json_t.probability;
        return;
    }

    inline auto from_json(
        const nlohmann::ordered_json &nlohmann_json_j,
        AudioAssociationSettingPath &nlohmann_json_t) -> void
    {
        nlohmann_json_j.at("key").get_to(nlohmann_json_t.key);
        nlohmann_json_j.at("object").get_to(nlohmann_json_t.object);
        nlohmann_json_j.at("weight").get_to(nlohmann_json_t.weight);
        nlohmann_json_j.at("probability").get_to(nlohmann_json_t.probability);
        return;
    }

    //---------------------------

    struct AudioAssociationSetting
    {
        uint32_t probability;
        AudioAssociationSettingMode mode;
        std::vector<AudioAssociationSettingArgument> argument;
        std::vector<AudioAssociationSettingPath> path;
    };

    inline auto to_json(
        nlohmann::ordered_json &nlohmann_json_j,
        const AudioAssociationSetting &nlohmann_json_t) -> void
    {
        if (k_version < 88_ui)
        {
            nlohmann_json_j["probability"] = nlohmann_json_t.probability;
        }
        nlohmann_json_j["mode"] = magic_enum::enum_name(nlohmann_json_t.mode);
        nlohmann_json_j["argument"] = nlohmann_json_t.argument;
        nlohmann_json_j["path"] = nlohmann_json_t.path;
        return;
    }

    inline auto from_json(
        const nlohmann::ordered_json &nlohmann_json_j,
        AudioAssociationSetting &nlohmann_json_t) -> void
    {
        if (k_version < 88_ui)
        {
            nlohmann_json_j.at("probability").get_to(nlohmann_json_t.probability);
        }
        nlohmann_json_t.mode = magic_enum_cast<AudioAssociationSettingMode>(nlohmann_json_j.at("mode").get<std::string>());
        nlohmann_json_j.at("argument").get_to(nlohmann_json_t.argument);
        nlohmann_json_j.at("path").get_to(nlohmann_json_t.path);
        return;
    }

    //---------------------------

    struct DialogueEvent
    {
        uint32_t probability;
        AudioAssociationSetting association;
    };

    inline auto to_json(
        nlohmann::ordered_json &nlohmann_json_j,
        const DialogueEvent &nlohmann_json_t) -> void
    {
        nlohmann_json_j["probability"] = nlohmann_json_t.probability;
        if (k_version >= 88_ui)
        {
            nlohmann_json_j["association"] = nlohmann_json_t.association;
        }
        return;
    }

    inline auto from_json(
        const nlohmann::ordered_json &nlohmann_json_j,
        DialogueEvent &nlohmann_json_t) -> void
    {
        nlohmann_json_j.at("probability").get_to(nlohmann_json_t.probability);
        if (k_version >= 88_ui)
        {
            nlohmann_json_j.at("association").get_to(nlohmann_json_t.association);
        }
        return;
    }

    //--------------------------

    struct AttenuationApplySetting
    {
        uint32_t output_bus_volume;
        uint32_t auxiliary_send_volume;
        uint32_t low_pass_filter;
        uint32_t spread;
        uint32_t game_defined_auxiliary_send_volume;
        uint32_t user_defined_auxiliary_send_volume;
        uint32_t high_pass_filter;
        uint32_t focus;
        //
        uint32_t distance_output_bus_volume;
        uint32_t distance_game_defined_auxiliary_send_volume;
        uint32_t distance_user_defined_auxiliary_send_volume;
        uint32_t distance_low_pass_filter;
        uint32_t distance_high_pass_filter;
        uint32_t distance_spread;
        uint32_t distance_focus;
        uint32_t obstruction_volume;
        uint32_t obstruction_low_pass_filter;
        uint32_t obstruction_high_pass_filter;
        uint32_t occlusion_volume;
        uint32_t occlusion_low_pass_filter;
        uint32_t occlusion_high_pass_filter;
        uint32_t diffraction_volume;
        uint32_t diffraction_low_pass_filter;
        uint32_t diffraction_high_pass_filter;
        uint32_t transmission_volume;
        uint32_t transmission_low_pass_filter;
        uint32_t transmission_high_pass_filter;
    };

    inline auto to_json(
        nlohmann::ordered_json &nlohmann_json_j,
        const AttenuationApplySetting &nlohmann_json_t) -> void
    {
        if (k_version < 88_ui)
        {
            nlohmann_json_j["output_bus_volume"] = nlohmann_json_t.output_bus_volume;
            nlohmann_json_j["auxiliary_send_volume"] = nlohmann_json_t.auxiliary_send_volume;
            nlohmann_json_j["low_pass_filter"] = nlohmann_json_t.low_pass_filter;
            nlohmann_json_j["spread"] = nlohmann_json_t.spread;
        }
        else if (k_version < 112_ui)
        {
            nlohmann_json_j["output_bus_volume"] = nlohmann_json_t.output_bus_volume;
            nlohmann_json_j["game_defined_auxiliary_send_volume"] = nlohmann_json_t.game_defined_auxiliary_send_volume;
            nlohmann_json_j["user_defined_auxiliary_send_volume"] = nlohmann_json_t.user_defined_auxiliary_send_volume;
            nlohmann_json_j["low_pass_filter"] = nlohmann_json_t.low_pass_filter;
            nlohmann_json_j["spread"] = nlohmann_json_t.spread;
        }
        else if (k_version < 145_ui)
        {
            nlohmann_json_j["output_bus_volume"] = nlohmann_json_t.output_bus_volume;
            nlohmann_json_j["game_defined_auxiliary_send_volume"] = nlohmann_json_t.game_defined_auxiliary_send_volume;
            nlohmann_json_j["user_defined_auxiliary_send_volume"] = nlohmann_json_t.user_defined_auxiliary_send_volume;
            nlohmann_json_j["low_pass_filter"] = nlohmann_json_t.low_pass_filter;
            nlohmann_json_j["high_pass_filter"] = nlohmann_json_t.high_pass_filter;
            nlohmann_json_j["spread"] = nlohmann_json_t.spread;
            nlohmann_json_j["focus"] = nlohmann_json_t.focus;
        }
        else
        {
            nlohmann_json_j["distance_output_bus_volume"] = nlohmann_json_t.distance_output_bus_volume;
            nlohmann_json_j["distance_game_defined_auxiliary_send_volume"] = nlohmann_json_t.distance_game_defined_auxiliary_send_volume;
            nlohmann_json_j["distance_user_defined_auxiliary_send_volume"] = nlohmann_json_t.distance_user_defined_auxiliary_send_volume;
            nlohmann_json_j["distance_low_pass_filter"] = nlohmann_json_t.distance_low_pass_filter;
            nlohmann_json_j["distance_high_pass_filter"] = nlohmann_json_t.distance_high_pass_filter;
            nlohmann_json_j["distance_spread"] = nlohmann_json_t.distance_spread;
            nlohmann_json_j["distance_focus"] = nlohmann_json_t.distance_focus;
            nlohmann_json_j["obstruction_volume"] = nlohmann_json_t.obstruction_volume;
            nlohmann_json_j["obstruction_low_pass_filter"] = nlohmann_json_t.obstruction_low_pass_filter;
            nlohmann_json_j["obstruction_high_pass_filter"] = nlohmann_json_t.obstruction_high_pass_filter;
            nlohmann_json_j["occlusion_volume"] = nlohmann_json_t.occlusion_volume;
            nlohmann_json_j["occlusion_low_pass_filter"] = nlohmann_json_t.occlusion_low_pass_filter;
            nlohmann_json_j["occlusion_high_pass_filter"] = nlohmann_json_t.occlusion_high_pass_filter;
            nlohmann_json_j["diffraction_volume"] = nlohmann_json_t.diffraction_volume;
            nlohmann_json_j["diffraction_low_pass_filter"] = nlohmann_json_t.diffraction_low_pass_filter;
            nlohmann_json_j["diffraction_high_pass_filter"] = nlohmann_json_t.diffraction_high_pass_filter;
            nlohmann_json_j["transmission_volume"] = nlohmann_json_t.transmission_volume;
            nlohmann_json_j["transmission_low_pass_filter"] = nlohmann_json_t.transmission_low_pass_filter;
            nlohmann_json_j["transmission_high_pass_filter"] = nlohmann_json_t.transmission_high_pass_filter;
        }
        return;
    }

    inline auto from_json(
        const nlohmann::ordered_json &nlohmann_json_j,
        AttenuationApplySetting &nlohmann_json_t) -> void
    {
        if (k_version < 88_ui)
        {
            nlohmann_json_j.at("output_bus_volume").get_to(nlohmann_json_t.output_bus_volume);
            nlohmann_json_j.at("auxiliary_send_volume").get_to(nlohmann_json_t.auxiliary_send_volume);
            nlohmann_json_j.at("low_pass_filter").get_to(nlohmann_json_t.low_pass_filter);
            nlohmann_json_j.at("spread").get_to(nlohmann_json_t.spread);
        }
        else if (k_version < 112_ui)
        {
            nlohmann_json_j.at("output_bus_volume").get_to(nlohmann_json_t.output_bus_volume);
            nlohmann_json_j.at("game_defined_auxiliary_send_volume").get_to(nlohmann_json_t.game_defined_auxiliary_send_volume);
            nlohmann_json_j.at("user_defined_auxiliary_send_volume").get_to(nlohmann_json_t.user_defined_auxiliary_send_volume);
            nlohmann_json_j.at("low_pass_filter").get_to(nlohmann_json_t.low_pass_filter);
            nlohmann_json_j.at("spread").get_to(nlohmann_json_t.spread);
        }
        else if (k_version < 145_ui)
        {
            nlohmann_json_j.at("output_bus_volume").get_to(nlohmann_json_t.output_bus_volume);
            nlohmann_json_j.at("game_defined_auxiliary_send_volume").get_to(nlohmann_json_t.game_defined_auxiliary_send_volume);
            nlohmann_json_j.at("user_defined_auxiliary_send_volume").get_to(nlohmann_json_t.user_defined_auxiliary_send_volume);
            nlohmann_json_j.at("low_pass_filter").get_to(nlohmann_json_t.low_pass_filter);
            nlohmann_json_j.at("high_pass_filter").get_to(nlohmann_json_t.high_pass_filter);
            nlohmann_json_j.at("spread").get_to(nlohmann_json_t.spread);
            nlohmann_json_j.at("focus").get_to(nlohmann_json_t.focus);
        }
        else
        {
            nlohmann_json_j.at("distance_output_bus_volume").get_to(nlohmann_json_t.distance_output_bus_volume);
            nlohmann_json_j.at("distance_game_defined_auxiliary_send_volume").get_to(nlohmann_json_t.distance_game_defined_auxiliary_send_volume);
            nlohmann_json_j.at("distance_user_defined_auxiliary_send_volume").get_to(nlohmann_json_t.distance_user_defined_auxiliary_send_volume);
            nlohmann_json_j.at("distance_low_pass_filter").get_to(nlohmann_json_t.distance_low_pass_filter);
            nlohmann_json_j.at("distance_high_pass_filter").get_to(nlohmann_json_t.distance_high_pass_filter);
            nlohmann_json_j.at("distance_spread").get_to(nlohmann_json_t.distance_spread);
            nlohmann_json_j.at("distance_focus").get_to(nlohmann_json_t.distance_focus);
            nlohmann_json_j.at("obstruction_volume").get_to(nlohmann_json_t.obstruction_volume);
            nlohmann_json_j.at("obstruction_low_pass_filter").get_to(nlohmann_json_t.obstruction_low_pass_filter);
            nlohmann_json_j.at("obstruction_high_pass_filter").get_to(nlohmann_json_t.obstruction_high_pass_filter);
            nlohmann_json_j.at("occlusion_volume").get_to(nlohmann_json_t.occlusion_volume);
            nlohmann_json_j.at("occlusion_low_pass_filter").get_to(nlohmann_json_t.occlusion_low_pass_filter);
            nlohmann_json_j.at("occlusion_high_pass_filter").get_to(nlohmann_json_t.occlusion_high_pass_filter);
            nlohmann_json_j.at("diffraction_volume").get_to(nlohmann_json_t.diffraction_volume);
            nlohmann_json_j.at("diffraction_low_pass_filter").get_to(nlohmann_json_t.diffraction_low_pass_filter);
            nlohmann_json_j.at("diffraction_high_pass_filter").get_to(nlohmann_json_t.diffraction_high_pass_filter);
            nlohmann_json_j.at("transmission_volume").get_to(nlohmann_json_t.transmission_volume);
            nlohmann_json_j.at("transmission_low_pass_filter").get_to(nlohmann_json_t.transmission_low_pass_filter);
            nlohmann_json_j.at("transmission_high_pass_filter").get_to(nlohmann_json_t.transmission_high_pass_filter);
        }
        return;
    }

    //--------------------------

    struct AttenuationCurve
    {
        CoordinateMode mode;
        std::vector<CoordinatePoint> point;
    };

    inline auto to_json(
        nlohmann::ordered_json &nlohmann_json_j,
        const AttenuationCurve &nlohmann_json_t) -> void
    {
        nlohmann_json_j["mode"] = nlohmann_json_t.mode;
        nlohmann_json_j["point"] = nlohmann_json_t.point;
        return;
    }

    inline auto from_json(
        const nlohmann::ordered_json &nlohmann_json_j,
        AttenuationCurve &nlohmann_json_t) -> void
    {
        nlohmann_json_j.at("mode").get_to(nlohmann_json_t.mode);
        nlohmann_json_j.at("point").get_to(nlohmann_json_t.point);
        return;
    }

    //--------------------------

    struct AttenuationCone
    {
        bool enable;
        float inner_angle;
        float outer_angle;
        float maximum_value;
        float low_pass_filter;
        float high_pass_filter;
    };

    inline auto to_json(
        nlohmann::ordered_json &nlohmann_json_j,
        const AttenuationCone &nlohmann_json_t) -> void
    {
        if (k_version < 112_ui)
        {
            nlohmann_json_j["enable"] = nlohmann_json_t.enable;
            nlohmann_json_j["inner_angle"] = nlohmann_json_t.inner_angle;
            nlohmann_json_j["outer_angle"] = nlohmann_json_t.outer_angle;
            nlohmann_json_j["maximum_value"] = nlohmann_json_t.maximum_value;
            nlohmann_json_j["low_pass_filter"] = nlohmann_json_t.low_pass_filter;
        }
        nlohmann_json_j["high_pass_filter"] = nlohmann_json_t.high_pass_filter;
        return;
    }

    inline auto from_json(
        const nlohmann::ordered_json &nlohmann_json_j,
        AttenuationCone &nlohmann_json_t) -> void
    {
        if (k_version < 112_ui)
        {
            nlohmann_json_j.at("enable").get_to(nlohmann_json_t.enable);
            nlohmann_json_j.at("inner_angle").get_to(nlohmann_json_t.inner_angle);
            nlohmann_json_j.at("outer_angle").get_to(nlohmann_json_t.outer_angle);
            nlohmann_json_j.at("maximum_value").get_to(nlohmann_json_t.maximum_value);
            nlohmann_json_j.at("low_pass_filter").get_to(nlohmann_json_t.low_pass_filter);
        }
        nlohmann_json_j.at("high_pass_filter").get_to(nlohmann_json_t.high_pass_filter);
        return;
    }

    //--------------------------

    struct Attenuation
    {
        AttenuationApplySetting apply;
        std::vector<AttenuationCurve> curve;
        bool height_spread;
        AttenuationCone cone;
        RealTimeParameterControlSetting real_time_parameter_control;
    };

    inline auto to_json(
        nlohmann::ordered_json &nlohmann_json_j,
        const Attenuation &nlohmann_json_t) -> void
    {
        nlohmann_json_j["apply"] = nlohmann_json_t.apply;
        nlohmann_json_j["curve"] = nlohmann_json_t.curve;
        if (k_version >= 140_ui)
        {
            nlohmann_json_j["height_spread"] = nlohmann_json_t.height_spread;
        }
        nlohmann_json_j["cone"] = nlohmann_json_t.cone;
        nlohmann_json_j["real_time_parameter_control"] = nlohmann_json_t.real_time_parameter_control;
        return;
    }

    inline auto from_json(
        const nlohmann::ordered_json &nlohmann_json_j,
        Attenuation &nlohmann_json_t) -> void
    {
        nlohmann_json_j.at("apply").get_to(nlohmann_json_t.apply);
        nlohmann_json_j.at("curve").get_to(nlohmann_json_t.curve);
        if (k_version >= 140_ui)
        {
            nlohmann_json_j.at("height_spread").get_to(nlohmann_json_t.height_spread);
        }
        nlohmann_json_j.at("cone").get_to(nlohmann_json_t.cone);
        nlohmann_json_j.at("real_time_parameter_control").get_to(nlohmann_json_t.real_time_parameter_control);
        return;
    }

    //--------------------------

    struct LowFrequencyOscillatorModulator
    {
        RandomizableValue<float> depth;
        RandomizableValue<float> frequency;
        ModulatorWaveform waveform;
        RandomizableValue<float> smoothing;
        RandomizableValue<float> pulse_width_modulation;
        RandomizableValue<float> attack;
        RandomizableValue<float> initial_phase_offset;
        ModulatorScope scope;
        RealTimeParameterControlSetting real_time_parameter_control;
    };

    inline auto to_json(
        nlohmann::ordered_json &nlohmann_json_j,
        const LowFrequencyOscillatorModulator &nlohmann_json_t) -> void
    {
        if (k_version >= 112_ui)
        {
            nlohmann_json_j["depth"] = nlohmann_json_t.depth;
            nlohmann_json_j["frequency"] = nlohmann_json_t.frequency;
            nlohmann_json_j["waveform"] = nlohmann_json_t.waveform;
            nlohmann_json_j["smoothing"] = nlohmann_json_t.smoothing;
            nlohmann_json_j["pulse_width_modulation"] = nlohmann_json_t.pulse_width_modulation;
            nlohmann_json_j["attack"] = nlohmann_json_t.attack;
            nlohmann_json_j["initial_phase_offset"] = nlohmann_json_t.initial_phase_offset;
            nlohmann_json_j["scope"] = magic_enum::enum_name(nlohmann_json_t.scope);
            nlohmann_json_j["real_time_parameter_control"] = nlohmann_json_t.real_time_parameter_control;
        }
        return;
    }

    inline auto from_json(
        const nlohmann::ordered_json &nlohmann_json_j,
        LowFrequencyOscillatorModulator &nlohmann_json_t) -> void
    {
        if (k_version >= 112_ui)
        {
            nlohmann_json_j.at("depth").get_to(nlohmann_json_t.depth);
            nlohmann_json_j.at("frequency").get_to(nlohmann_json_t.frequency);
            nlohmann_json_j.at("waveform").get_to(nlohmann_json_t.waveform);
            nlohmann_json_j.at("smoothing").get_to(nlohmann_json_t.smoothing);
            nlohmann_json_j.at("pulse_width_modulation").get_to(nlohmann_json_t.pulse_width_modulation);
            nlohmann_json_j.at("attack").get_to(nlohmann_json_t.attack);
            nlohmann_json_j.at("initial_phase_offset").get_to(nlohmann_json_t.initial_phase_offset);
            nlohmann_json_j.at("scope").get_to(nlohmann_json_t.scope);
            nlohmann_json_j.at("real_time_parameter_control").get_to(nlohmann_json_t.real_time_parameter_control);
        }
        return;
    }

    //--------------------------

    struct EnvelopeModulator
    {
        RandomizableValue<float> attack_time;
        RandomizableValue<float> attack_curve;
        RandomizableValue<float> decay_time;
        RandomizableValue<float> sustain_level;
        RandomizableValue<float> release_time;
        ModulatorScope scope;
        ModulatorTriggerOn trigger_on;
        RandomizableValue<float> sustain_time;
        bool stop_playback_after_release;
        RealTimeParameterControlSetting real_time_parameter_control;
    };

    inline auto to_json(
        nlohmann::ordered_json &nlohmann_json_j,
        const EnvelopeModulator &nlohmann_json_t) -> void
    {
        if (k_version >= 112_ui)
        {
            nlohmann_json_j["attack_time"] = nlohmann_json_t.attack_time;
            nlohmann_json_j["attack_curve"] = nlohmann_json_t.attack_curve;
            nlohmann_json_j["decay_time"] = nlohmann_json_t.decay_time;
            nlohmann_json_j["sustain_level"] = nlohmann_json_t.sustain_level;
            nlohmann_json_j["release_time"] = nlohmann_json_t.release_time;
            nlohmann_json_j["scope"] = magic_enum::enum_name(nlohmann_json_t.scope);
            nlohmann_json_j["trigger_on"] = magic_enum::enum_name(nlohmann_json_t.trigger_on);
            nlohmann_json_j["sustain_time"] = nlohmann_json_t.sustain_time;
            nlohmann_json_j["stop_playback_after_release"] = nlohmann_json_t.stop_playback_after_release;
            nlohmann_json_j["real_time_parameter_control"] = nlohmann_json_t.real_time_parameter_control;
        }
        return;
    }

    inline auto from_json(
        const nlohmann::ordered_json &nlohmann_json_j,
        EnvelopeModulator &nlohmann_json_t) -> void
    {
        if (k_version >= 112_ui)
        {
            nlohmann_json_j.at("attack_time").get_to(nlohmann_json_t.attack_time);
            nlohmann_json_j.at("attack_curve").get_to(nlohmann_json_t.attack_curve);
            nlohmann_json_j.at("decay_time").get_to(nlohmann_json_t.decay_time);
            nlohmann_json_j.at("sustain_level").get_to(nlohmann_json_t.sustain_level);
            nlohmann_json_j.at("release_time").get_to(nlohmann_json_t.release_time);
            nlohmann_json_t.scope = magic_enum_cast<ModulatorScope>(nlohmann_json_j.at("scope").get<std::string>());
            nlohmann_json_t.trigger_on = magic_enum_cast<ModulatorTriggerOn>(nlohmann_json_j.at("trigger_on").get<std::string>());
            nlohmann_json_j.at("sustain_time").get_to(nlohmann_json_t.sustain_time);
            nlohmann_json_j.at("stop_playback_after_release").get_to(nlohmann_json_t.stop_playback_after_release);
            nlohmann_json_j.at("real_time_parameter_control").get_to(nlohmann_json_t.real_time_parameter_control);
        }
        return;
    }

    //--------------------------

    struct TimeModulator
    {
        RandomizableValue<float> initial_delay;
        float duration;
        RandomizableValue<uint32_t> loop;
        RandomizableValue<float> playback_rate;
        ModulatorScope scope;
        ModulatorTriggerOn trigger_on;
        bool stop_playback_at_end;
        RealTimeParameterControlSetting real_time_parameter_control;
    };

    inline auto to_json(
        nlohmann::ordered_json &nlohmann_json_j,
        const TimeModulator &nlohmann_json_t) -> void
    {
        if (k_version >= 132_ui)
        {
            nlohmann_json_j["initial_delay"] = nlohmann_json_t.initial_delay;
            nlohmann_json_j["duration"] = nlohmann_json_t.duration;
            nlohmann_json_j["loop"] = nlohmann_json_t.loop;
            nlohmann_json_j["playback_rate"] = nlohmann_json_t.playback_rate;
            nlohmann_json_j["scope"] = magic_enum::enum_name(nlohmann_json_t.scope);
            nlohmann_json_j["trigger_on"] = magic_enum::enum_name(nlohmann_json_t.trigger_on);
            nlohmann_json_j["stop_playback_at_end"] = nlohmann_json_t.stop_playback_at_end;
            nlohmann_json_j["real_time_parameter_control"] = nlohmann_json_t.real_time_parameter_control;
        }
        return;
    }

    inline auto from_json(
        const nlohmann::ordered_json &nlohmann_json_j,
        TimeModulator &nlohmann_json_t) -> void
    {
        if (k_version >= 132_ui)
        {
            nlohmann_json_j.at("initial_delay").get_to(nlohmann_json_t.initial_delay);
            nlohmann_json_j.at("duration").get_to(nlohmann_json_t.duration);
            nlohmann_json_j.at("loop").get_to(nlohmann_json_t.loop);
            nlohmann_json_j.at("playback_rate").get_to(nlohmann_json_t.playback_rate);
            nlohmann_json_t.scope = magic_enum_cast<ModulatorScope>(nlohmann_json_j.at("scope").get<std::string>());
            nlohmann_json_t.trigger_on = magic_enum_cast<ModulatorTriggerOn>(nlohmann_json_j.at("trigger_on").get<std::string>());
            nlohmann_json_j.at("stop_playback_at_end").get_to(nlohmann_json_t.stop_playback_at_end);
            nlohmann_json_j.at("real_time_parameter_control").get_to(nlohmann_json_t.real_time_parameter_control);
        }
        return;
    }

    //--------------------------

    struct EffectU1
    {
        uint32_t type;
        CoordinateMode mode;
        float value;
    };

    inline auto to_json(
        nlohmann::ordered_json &nlohmann_json_j,
        const EffectU1 &nlohmann_json_t) -> void
    {
        if (k_version >= 112_ui)
        {
            if (k_version < 128_ui)
            {
                nlohmann_json_j["type"] = nlohmann_json_t.type;
                nlohmann_json_j["value"] = nlohmann_json_t.value;
            }
            else
            {
                nlohmann_json_j["type"] = nlohmann_json_t.type;
                nlohmann_json_j["mode"] = magic_enum::enum_name(nlohmann_json_t.mode);
                nlohmann_json_j["value"] = nlohmann_json_t.value;
            }
        }
        return;
    }

    inline auto from_json(
        const nlohmann::ordered_json &nlohmann_json_j,
        EffectU1 &nlohmann_json_t) -> void
    {
        if (k_version >= 112_ui)
        {
            if (k_version < 128_ui)
            {
                nlohmann_json_j.at("type").get_to(nlohmann_json_t.type);
                nlohmann_json_j.at("value").get_to(nlohmann_json_t.value);
            }
            else
            {
                nlohmann_json_j.at("type").get_to(nlohmann_json_t.type);
                nlohmann_json_t.mode = magic_enum_cast<CoordinateMode>(nlohmann_json_j.at("mode").get<std::string>());
                nlohmann_json_j.at("value").get_to(nlohmann_json_t.value);
            }
        }
    }

    // ----------------------------

    struct Effect
    {
        uint32_t plug_in;
        std::string expand;
        RealTimeParameterControlSetting real_time_parameter_control;
        StateSetting state;
        std::vector<EffectU1> u1;
    };

    inline auto to_json(
        nlohmann::ordered_json &nlohmann_json_j,
        const Effect &nlohmann_json_t) -> void
    {
        nlohmann_json_j["plug_in"] = nlohmann_json_t.plug_in;
        nlohmann_json_j["expand"] = nlohmann_json_t.expand;
        nlohmann_json_j["real_time_parameter_control"] = nlohmann_json_t.real_time_parameter_control;
        if (k_version >= 112_ui)
        {
            nlohmann_json_j["state"] = nlohmann_json_t.state;
        }
        if (k_version >= 128_ui)
        {
            nlohmann_json_j["u1"] = nlohmann_json_t.u1;
        }
        return;
    }

    inline auto from_json(
        const nlohmann::ordered_json &nlohmann_json_j,
        Effect &nlohmann_json_t) -> void
    {
        nlohmann_json_j.at("plug_in").get_to(nlohmann_json_t.plug_in);
        nlohmann_json_j.at("expand").get_to(nlohmann_json_t.expand);
        nlohmann_json_j.at("real_time_parameter_control").get_to(nlohmann_json_t.real_time_parameter_control);
        if (k_version >= 112_ui)
        {
            nlohmann_json_j.at("state").get_to(nlohmann_json_t.state);
        }
        if (k_version >= 128_ui)
        {
            nlohmann_json_j.at("u1").get_to(nlohmann_json_t.u1);
        }
        return;
    }

    // ----------------------------

    struct Source
    {
        uint32_t plug_in;
        std::string expand;
        RealTimeParameterControlSetting real_time_parameter_control;
        StateSetting state;
        std::vector<EffectU1> u1;
    };

    inline auto to_json(
        nlohmann::ordered_json &nlohmann_json_j,
        const Source &nlohmann_json_t) -> void
    {
        nlohmann_json_j["plug_in"] = nlohmann_json_t.plug_in;
        nlohmann_json_j["expand"] = nlohmann_json_t.expand;
        nlohmann_json_j["real_time_parameter_control"] = nlohmann_json_t.real_time_parameter_control;
        if (k_version >= 112_ui)
        {
            nlohmann_json_j["state"] = nlohmann_json_t.state;
        }
        if (k_version >= 128_ui)
        {
            nlohmann_json_j["u1"] = nlohmann_json_t.u1;
        }
        return;
    }

    inline auto from_json(
        const nlohmann::ordered_json &nlohmann_json_j,
        Source &nlohmann_json_t) -> void
    {
        nlohmann_json_j.at("plug_in").get_to(nlohmann_json_t.plug_in);
        nlohmann_json_j.at("expand").get_to(nlohmann_json_t.expand);
        nlohmann_json_j.at("real_time_parameter_control").get_to(nlohmann_json_t.real_time_parameter_control);
        if (k_version >= 112_ui)
        {
            nlohmann_json_j.at("state").get_to(nlohmann_json_t.state);
        }
        if (k_version >= 128_ui)
        {
            nlohmann_json_j.at("u1").get_to(nlohmann_json_t.u1);
        }
        return;
    }

    // ----------------------------

    struct AudioDevice
    {
        uint32_t plug_in;
        std::string expand;
        RealTimeParameterControlSetting real_time_parameter_control;
        StateSetting state;
        std::vector<EffectU1> u1;
        AudioEffectSetting effect;
    };

    inline auto to_json(
        nlohmann::ordered_json &nlohmann_json_j,
        const AudioDevice &nlohmann_json_t) -> void
    {
        if (k_version >= 128_ui)
        {
            nlohmann_json_j["plug_in"] = nlohmann_json_t.plug_in;
            nlohmann_json_j["expand"] = nlohmann_json_t.expand;
            nlohmann_json_j["real_time_parameter_control"] = nlohmann_json_t.real_time_parameter_control;
            nlohmann_json_j["state"] = nlohmann_json_t.state;
            nlohmann_json_j["u1"] = nlohmann_json_t.u1;
            if (k_version >= 140_ui)
            {
                nlohmann_json_j["effect"] = nlohmann_json_t.effect;
            }
        }
        return;
    }

    inline auto from_json(
        const nlohmann::ordered_json &nlohmann_json_j,
        AudioDevice &nlohmann_json_t) -> void
    {
        if (k_version >= 128_ui)
        {
            nlohmann_json_j.at("plug_in").get_to(nlohmann_json_t.plug_in);
            nlohmann_json_j.at("expand").get_to(nlohmann_json_t.expand);
            nlohmann_json_j.at("real_time_parameter_control").get_to(nlohmann_json_t.real_time_parameter_control);
            nlohmann_json_j.at("state").get_to(nlohmann_json_t.state);
            nlohmann_json_j.at("u1").get_to(nlohmann_json_t.u1);
            if (k_version >= 140_ui)
            {
                nlohmann_json_j.at("effect").get_to(nlohmann_json_t.effect);
            }
        }
        return;
    }

    //--------------------------

    struct AudioBusConfiguration
    {
        uint32_t u1;
    };

    inline auto to_json(
        nlohmann::ordered_json &nlohmann_json_j,
        const AudioBusConfiguration &nlohmann_json_t) -> void
    {
        if (k_version >= 88_ui)
        {
            nlohmann_json_j["u1"] = nlohmann_json_t.u1;
        }
        return;
    }

    inline auto from_json(
        const nlohmann::ordered_json &nlohmann_json_j,
        AudioBusConfiguration &nlohmann_json_t) -> void
    {
        if (k_version >= 88_ui)
        {
            nlohmann_json_j.at("u1").get_to(nlohmann_json_t.u1);
        }
        return;
    }

    //--------------------------

    struct BusVoiceVolumeGainSetting
    {
        float make_up;
    };

    inline auto to_json(
        nlohmann::ordered_json &nlohmann_json_j,
        const BusVoiceVolumeGainSetting &nlohmann_json_t) -> void
    {
        if (k_version >= 125_ui)
        {
            nlohmann_json_j["make_up"] = nlohmann_json_t.make_up;
        }
        return;
    }

    inline auto from_json(
        const nlohmann::ordered_json &nlohmann_json_j,
        BusVoiceVolumeGainSetting &nlohmann_json_t) -> void
    {
        if (k_version >= 125_ui)
        {
            nlohmann_json_j.at("make_up").get_to(nlohmann_json_t.make_up);
        }
        return;
    }

    //--------------------------

    struct BusBusSetting
    {
        float volume;
    };

    inline auto to_json(
        nlohmann::ordered_json &nlohmann_json_j,
        const BusBusSetting &nlohmann_json_t) -> void
    {
        nlohmann_json_j["volume"] = nlohmann_json_t.volume;
        return;
    }

    inline auto from_json(
        const nlohmann::ordered_json &nlohmann_json_j,
        BusBusSetting &nlohmann_json_t) -> void
    {
        nlohmann_json_j.at("volume").get_to(nlohmann_json_t.volume);
        return;
    }

    //------------------------

    struct BusOutputBusSetting
    {
        float volume;
        float low_pass_filter;
        float high_pass_filter;
    };

    inline auto to_json(
        nlohmann::ordered_json &nlohmann_json_j,
        const BusOutputBusSetting &nlohmann_json_t) -> void
    {
        if (k_version >= 128_ui)
        {
            nlohmann_json_j["volume"] = nlohmann_json_t.volume;
            nlohmann_json_j["low_pass_filter"] = nlohmann_json_t.low_pass_filter;
            nlohmann_json_j["high_pass_filter"] = nlohmann_json_t.high_pass_filter;
        }
        return;
    }

    inline auto from_json(
        const nlohmann::ordered_json &nlohmann_json_j,
        BusOutputBusSetting &nlohmann_json_t) -> void
    {
        if (k_version >= 128_ui)
        {
            nlohmann_json_j.at("volume").get_to(nlohmann_json_t.volume);
            nlohmann_json_j.at("low_pass_filter").get_to(nlohmann_json_t.low_pass_filter);
            nlohmann_json_j.at("high_pass_filter").get_to(nlohmann_json_t.high_pass_filter);
        }
        return;
    }

    //--------------------------

    struct BusAutomaticDuckingSettingBus
    {
        uint32_t id;
        float volume;
        uint32_t fade_out;
        uint32_t fade_in;
        Curve curve;
        BusAutomaticDuckingSettingBusTarget target;
    };

    inline auto to_json(
        nlohmann::ordered_json &nlohmann_json_j,
        const BusAutomaticDuckingSettingBus &nlohmann_json_t) -> void
    {
        nlohmann_json_j["id"] = nlohmann_json_t.id;
        nlohmann_json_j["volume"] = nlohmann_json_t.volume;
        nlohmann_json_j["fade_out"] = nlohmann_json_t.fade_out;
        nlohmann_json_j["fade_in"] = nlohmann_json_t.fade_in;
        nlohmann_json_j["curve"] = magic_enum::enum_name(nlohmann_json_t.curve);
        nlohmann_json_j["target"] = magic_enum::enum_name(nlohmann_json_t.target);
        return;
    }

    inline auto from_json(
        const nlohmann::ordered_json &nlohmann_json_j,
        BusAutomaticDuckingSettingBus &nlohmann_json_t) -> void
    {
        nlohmann_json_j.at("id").get_to(nlohmann_json_t.id);
        nlohmann_json_j.at("volume").get_to(nlohmann_json_t.volume);
        nlohmann_json_j.at("fade_out").get_to(nlohmann_json_t.fade_out);
        nlohmann_json_j.at("fade_in").get_to(nlohmann_json_t.fade_in);
        nlohmann_json_t.curve = magic_enum_cast<Curve>(nlohmann_json_j.at("curve").get<std::string>());
        nlohmann_json_t.target = magic_enum_cast<BusAutomaticDuckingSettingBusTarget>(nlohmann_json_j.at("target").get<std::string>());
        return;
    }

    //--------------------------

    struct BusAutomaticDuckingSetting
    {
        uint32_t recovery_time;
        float maximum_ducking_volume;
        std::vector<BusAutomaticDuckingSettingBus> bus;
    };

    inline auto to_json(
        nlohmann::ordered_json &nlohmann_json_j,
        const BusAutomaticDuckingSetting &nlohmann_json_t) -> void
    {
        nlohmann_json_j["recovery_time"] = nlohmann_json_t.recovery_time;
        nlohmann_json_j["maximum_ducking_volume"] = nlohmann_json_t.maximum_ducking_volume;
        nlohmann_json_j["bus"] = nlohmann_json_t.bus;
        return;
    }

    inline auto from_json(
        const nlohmann::ordered_json &nlohmann_json_j,
        BusAutomaticDuckingSetting &nlohmann_json_t) -> void
    {
        nlohmann_json_j.at("recovery_time").get_to(nlohmann_json_t.recovery_time);
        nlohmann_json_j.at("maximum_ducking_volume").get_to(nlohmann_json_t.maximum_ducking_volume);
        nlohmann_json_j.at("bus").get_to(nlohmann_json_t.bus);
        return;
    }

    //--------------------------

    struct BusHDRSettingDynamic
    {
        float threshold;
        float ratio;
        float release_time;
        BusHDRSettingDynamicReleaseMode release_mode;
    };

    inline auto to_json(
        nlohmann::ordered_json &nlohmann_json_j,
        const BusHDRSettingDynamic &nlohmann_json_t) -> void
    {
        if (k_version >= 88_ui)
        {
            nlohmann_json_j["threshold"] = nlohmann_json_t.threshold;
            nlohmann_json_j["ratio"] = nlohmann_json_t.ratio;
            nlohmann_json_j["release_time"] = nlohmann_json_t.release_time;
            nlohmann_json_j["release_mode"] = magic_enum::enum_name(nlohmann_json_t.release_mode);
        }
        return;
    }

    inline auto from_json(
        const nlohmann::ordered_json &nlohmann_json_j,
        BusHDRSettingDynamic &nlohmann_json_t) -> void
    {
        if (k_version >= 88_ui)
        {
            nlohmann_json_j.at("threshold").get_to(nlohmann_json_t.threshold);
            nlohmann_json_j.at("ratio").get_to(nlohmann_json_t.ratio);
            nlohmann_json_j.at("release_time").get_to(nlohmann_json_t.release_time);
            nlohmann_json_t.release_mode = magic_enum_cast<BusHDRSettingDynamicReleaseMode>(nlohmann_json_j.at("release_mode").get<std::string>());
        }
        return;
    }

    //--------------------------

    struct BusHDRSettingWindowTopOutputGameParameter
    {
        uint32_t id;
        float minimum;
        float maximum;
    };

    inline auto to_json(
        nlohmann::ordered_json &nlohmann_json_j,
        const BusHDRSettingWindowTopOutputGameParameter &nlohmann_json_t) -> void
    {
        if (k_version >= 88_ui)
        {
            nlohmann_json_j["id"] = nlohmann_json_t.id;
            nlohmann_json_j["minimum"] = nlohmann_json_t.minimum;
            nlohmann_json_j["maximum"] = nlohmann_json_t.maximum;
        }
        return;
    }

    inline auto from_json(
        const nlohmann::ordered_json &nlohmann_json_j,
        BusHDRSettingWindowTopOutputGameParameter &nlohmann_json_t) -> void
    {
        if (k_version >= 88_ui)
        {
            nlohmann_json_j.at("id").get_to(nlohmann_json_t.id);
            nlohmann_json_j.at("minimum").get_to(nlohmann_json_t.minimum);
            nlohmann_json_j.at("maximum").get_to(nlohmann_json_t.maximum);
        }
        return;
    }

    //--------------------------

    struct BusHDRSetting
    {
        bool enable;
        bool u1;
        BusHDRSettingDynamic dynamic;
        BusHDRSettingWindowTopOutputGameParameter window_top_output_game_parameter;
    };

    inline auto to_json(
        nlohmann::ordered_json &nlohmann_json_j,
        const BusHDRSetting &nlohmann_json_t) -> void
    {
        if (k_version >= 88_ui)
        {
            nlohmann_json_j["enable"] = nlohmann_json_t.enable;
            nlohmann_json_j["u1"] = nlohmann_json_t.u1;
            nlohmann_json_j["dynamic"] = nlohmann_json_t.dynamic;
            nlohmann_json_j["window_top_output_game_parameter"] = nlohmann_json_t.window_top_output_game_parameter;
        }
        return;
    }

    inline auto from_json(
        const nlohmann::ordered_json &nlohmann_json_j,
        BusHDRSetting &nlohmann_json_t) -> void
    {
        if (k_version >= 88_ui)
        {
            nlohmann_json_j.at("enable").get_to(nlohmann_json_t.enable);
            nlohmann_json_j.at("u1").get_to(nlohmann_json_t.u1);
            nlohmann_json_j.at("dynamic").get_to(nlohmann_json_t.dynamic);
            nlohmann_json_j.at("window_top_output_game_parameter").get_to(nlohmann_json_t.window_top_output_game_parameter);
        }
        return;
    }

    //--------------------------

    struct AudioBus
    {
        uint32_t parent;
        uint32_t audio_device;
        uint32_t mixer;
        AudioBusConfiguration bus_configuration;
        AudioBusMuteForBackgroundMusic mute_for_background_music;
        BusVoiceSetting voice;
        BusVoiceVolumeGainSetting voice_volume_gain;
        BusBusSetting bus;
        BusOutputBusSetting output_bus;
        AudioAuxiliarySendSetting auxiliary_send;
        AudioEffectSetting effect;
        AudioMetadataSetting metadata;
        AudioPositioningSetting positioning;
        RealTimeParameterControlSetting real_time_parameter_control;
        StateSetting state;
        BusAutomaticDuckingSetting automatic_ducking;
        BusHDRSetting hdr;
        AudioPlaybackLimitSetting playback_limit;
        bool override_positioning;
        bool override_playback_limit;
    };

    inline auto to_json(
        nlohmann::ordered_json &nlohmann_json_j,
        const AudioBus &nlohmann_json_t) -> void
    {
        nlohmann_json_j["parent"] = nlohmann_json_t.parent;
        if (k_version >= 128_ui)
        {
            nlohmann_json_j["audio_device"] = nlohmann_json_t.audio_device;
        }
        if (k_version >= 112_ui)
        {
            nlohmann_json_j["mixer"] = nlohmann_json_t.mixer;
        }
        if (k_version >= 88_ui)
        {
            nlohmann_json_j["bus_configuration"] = nlohmann_json_t.bus_configuration;
        }
        if (k_version >= 112_ui)
        {
            nlohmann_json_j["mute_for_background_music"] = nlohmann_json_t.mute_for_background_music;
        }
        nlohmann_json_j["voice"] = nlohmann_json_t.voice;
        if (k_version >= 125_ui)
        {
            nlohmann_json_j["voice_volume_gain"] = nlohmann_json_t.voice_volume_gain;
        }
        nlohmann_json_j["bus"] = nlohmann_json_t.bus;
        if (k_version >= 128_ui)
        {
            nlohmann_json_j["output_bus"] = nlohmann_json_t.output_bus;
        }
        if (k_version >= 125_ui)
        {
            nlohmann_json_j["auxiliary_send"] = nlohmann_json_t.auxiliary_send;
        }
        if (k_version >= 112_ui)
        {
            nlohmann_json_j["effect"] = nlohmann_json_t.effect;
        }
        if (k_version >= 88_ui)
        {
            nlohmann_json_j["positioning"] = nlohmann_json_t.positioning;
        }
        nlohmann_json_j["real_time_parameter_control"] = nlohmann_json_t.real_time_parameter_control;
        nlohmann_json_j["state"] = nlohmann_json_t.state;
        nlohmann_json_j["automatic_ducking"] = nlohmann_json_t.automatic_ducking;
        if (k_version >= 88_ui)
        {
            nlohmann_json_j["hdr"] = nlohmann_json_t.hdr;
        }
        nlohmann_json_j["playback_limit"] = nlohmann_json_t.playback_limit;
        if (k_version >= 88_ui && k_version < 125_ui)
        {
            nlohmann_json_j["override_positioning"] = nlohmann_json_t.override_positioning;
        }
        nlohmann_json_j["override_playback_limit"] = nlohmann_json_t.override_playback_limit;
        return;
    }

    inline auto from_json(
        const nlohmann::ordered_json &nlohmann_json_j,
        AudioBus &nlohmann_json_t) -> void
    {
        nlohmann_json_j.at("parent").get_to(nlohmann_json_t.parent);
        if (k_version >= 128_ui)
        {
            nlohmann_json_j.at("audio_device").get_to(nlohmann_json_t.audio_device);
        }
        if (k_version >= 112_ui)
        {
            nlohmann_json_j.at("mixer").get_to(nlohmann_json_t.mixer);
        }
        if (k_version >= 88_ui)
        {
            nlohmann_json_j.at("bus_configuration").get_to(nlohmann_json_t.bus_configuration);
        }
        if (k_version >= 112_ui)
        {
            nlohmann_json_j.at("mute_for_background_music").get_to(nlohmann_json_t.mute_for_background_music);
        }
        nlohmann_json_j.at("voice").get_to(nlohmann_json_t.voice);
        if (k_version >= 125_ui)
        {
            nlohmann_json_j.at("voice_volume_gain").get_to(nlohmann_json_t.voice_volume_gain);
        }
        nlohmann_json_j.at("bus").get_to(nlohmann_json_t.bus);
        if (k_version >= 128_ui)
        {
            nlohmann_json_j.at("output_bus").get_to(nlohmann_json_t.output_bus);
        }
        if (k_version >= 125_ui)
        {
            nlohmann_json_j.at("auxiliary_send").get_to(nlohmann_json_t.auxiliary_send);
        }
        if (k_version >= 112_ui)
        {
            nlohmann_json_j.at("effect").get_to(nlohmann_json_t.effect);
        }
        if (k_version >= 88_ui)
        {
            nlohmann_json_j.at("positioning").get_to(nlohmann_json_t.positioning);
        }
        nlohmann_json_j.at("real_time_parameter_control").get_to(nlohmann_json_t.real_time_parameter_control);
        nlohmann_json_j.at("state").get_to(nlohmann_json_t.state);
        nlohmann_json_j.at("automatic_ducking").get_to(nlohmann_json_t.automatic_ducking);
        if (k_version >= 88_ui)
        {
            nlohmann_json_j.at("hdr").get_to(nlohmann_json_t.hdr);
        }
        nlohmann_json_j.at("playback_limit").get_to(nlohmann_json_t.playback_limit);
        if (k_version >= 88_ui && k_version < 125_ui)
        {
            nlohmann_json_j.at("override_positioning").get_to(nlohmann_json_t.override_positioning);
        }
        nlohmann_json_j.at("override_playback_limit").get_to(nlohmann_json_t.override_playback_limit);
        return;
    }

    //--------------------------

    struct AuxiliaryAudioBus
    {
        uint32_t parent;
        uint32_t audio_device;
        uint32_t mixer;
        AudioBusConfiguration bus_configuration;
        AudioBusMuteForBackgroundMusic mute_for_background_music;
        BusVoiceSetting voice;
        BusVoiceVolumeGainSetting voice_volume_gain;
        BusBusSetting bus;
        BusOutputBusSetting output_bus;
        AudioAuxiliarySendSetting auxiliary_send;
        AudioEffectSetting effect;
        AudioMetadataSetting metadata;
        AudioPositioningSetting positioning;
        RealTimeParameterControlSetting real_time_parameter_control;
        StateSetting state;
        BusAutomaticDuckingSetting automatic_ducking;
        BusHDRSetting hdr;
        AudioPlaybackLimitSetting playback_limit;
        bool override_positioning;
        bool override_playback_limit;
    };

    inline auto to_json(
        nlohmann::ordered_json &nlohmann_json_j,
        const AuxiliaryAudioBus &nlohmann_json_t) -> void
    {
        nlohmann_json_j["parent"] = nlohmann_json_t.parent;
        if (k_version >= 128_ui)
        {
            nlohmann_json_j["audio_device"] = nlohmann_json_t.audio_device;
        }
        if (k_version >= 112_ui)
        {
            nlohmann_json_j["mixer"] = nlohmann_json_t.mixer;
        }
        if (k_version >= 88_ui)
        {
            nlohmann_json_j["bus_configuration"] = nlohmann_json_t.bus_configuration;
        }
        if (k_version >= 112_ui)
        {
            nlohmann_json_j["mute_for_background_music"] = nlohmann_json_t.mute_for_background_music;
        }
        nlohmann_json_j["voice"] = nlohmann_json_t.voice;
        if (k_version >= 125_ui)
        {
            nlohmann_json_j["voice_volume_gain"] = nlohmann_json_t.voice_volume_gain;
        }
        nlohmann_json_j["bus"] = nlohmann_json_t.bus;
        if (k_version >= 128_ui)
        {
            nlohmann_json_j["output_bus"] = nlohmann_json_t.output_bus;
        }
        if (k_version >= 125_ui)
        {
            nlohmann_json_j["auxiliary_send"] = nlohmann_json_t.auxiliary_send;
        }
        if (k_version >= 112_ui)
        {
            nlohmann_json_j["effect"] = nlohmann_json_t.effect;
        }
        if (k_version >= 88_ui)
        {
            nlohmann_json_j["positioning"] = nlohmann_json_t.positioning;
        }
        nlohmann_json_j["real_time_parameter_control"] = nlohmann_json_t.real_time_parameter_control;
        nlohmann_json_j["state"] = nlohmann_json_t.state;
        nlohmann_json_j["automatic_ducking"] = nlohmann_json_t.automatic_ducking;
        if (k_version >= 88_ui)
        {
            nlohmann_json_j["hdr"] = nlohmann_json_t.hdr;
        }
        nlohmann_json_j["playback_limit"] = nlohmann_json_t.playback_limit;
        if (k_version >= 88_ui && k_version < 125_ui)
        {
            nlohmann_json_j["override_positioning"] = nlohmann_json_t.override_positioning;
        }
        nlohmann_json_j["override_playback_limit"] = nlohmann_json_t.override_playback_limit;
        return;
    }

    inline auto from_json(
        const nlohmann::ordered_json &nlohmann_json_j,
        AuxiliaryAudioBus &nlohmann_json_t) -> void
    {
        nlohmann_json_j.at("parent").get_to(nlohmann_json_t.parent);
        if (k_version >= 128_ui)
        {
            nlohmann_json_j.at("audio_device").get_to(nlohmann_json_t.audio_device);
        }
        if (k_version >= 112_ui)
        {
            nlohmann_json_j.at("mixer").get_to(nlohmann_json_t.mixer);
        }
        if (k_version >= 88_ui)
        {
            nlohmann_json_j.at("bus_configuration").get_to(nlohmann_json_t.bus_configuration);
        }
        if (k_version >= 112_ui)
        {
            nlohmann_json_j.at("mute_for_background_music").get_to(nlohmann_json_t.mute_for_background_music);
        }
        nlohmann_json_j.at("voice").get_to(nlohmann_json_t.voice);
        if (k_version >= 125_ui)
        {
            nlohmann_json_j.at("voice_volume_gain").get_to(nlohmann_json_t.voice_volume_gain);
        }
        nlohmann_json_j.at("bus").get_to(nlohmann_json_t.bus);
        if (k_version >= 128_ui)
        {
            nlohmann_json_j.at("output_bus").get_to(nlohmann_json_t.output_bus);
        }
        if (k_version >= 125_ui)
        {
            nlohmann_json_j.at("auxiliary_send").get_to(nlohmann_json_t.auxiliary_send);
        }
        if (k_version >= 112_ui)
        {
            nlohmann_json_j.at("effect").get_to(nlohmann_json_t.effect);
        }
        if (k_version >= 88_ui)
        {
            nlohmann_json_j.at("positioning").get_to(nlohmann_json_t.positioning);
        }
        nlohmann_json_j.at("real_time_parameter_control").get_to(nlohmann_json_t.real_time_parameter_control);
        nlohmann_json_j.at("state").get_to(nlohmann_json_t.state);
        nlohmann_json_j.at("automatic_ducking").get_to(nlohmann_json_t.automatic_ducking);
        if (k_version >= 88_ui)
        {
            nlohmann_json_j.at("hdr").get_to(nlohmann_json_t.hdr);
        }
        nlohmann_json_j.at("playback_limit").get_to(nlohmann_json_t.playback_limit);
        if (k_version >= 88_ui && k_version < 125_ui)
        {
            nlohmann_json_j.at("override_positioning").get_to(nlohmann_json_t.override_positioning);
        }
        nlohmann_json_j.at("override_playback_limit").get_to(nlohmann_json_t.override_playback_limit);
        return;
    }

    //--------------------------

    struct SoundPlaybackSetting
    {
        RandomizableValue<float> initial_delay;
        RandomizableValue<uint32_t> loop;
    };

    inline auto to_json(
        nlohmann::ordered_json &nlohmann_json_j,
        const SoundPlaybackSetting &nlohmann_json_t) -> void
    {
        if (k_version >= 88_ui)
        {
            nlohmann_json_j["initial_delay"] = nlohmann_json_t.initial_delay;
        }
        nlohmann_json_j["loop"] = nlohmann_json_t.loop;
        return;
    }

    inline auto from_json(
        const nlohmann::ordered_json &nlohmann_json_j,
        SoundPlaybackSetting &nlohmann_json_t) -> void
    {
        if (k_version >= 88_ui)
        {
            nlohmann_json_j.at("initial_delay").get_to(nlohmann_json_t.initial_delay);
        }
        nlohmann_json_j.at("loop").get_to(nlohmann_json_t.loop);
        return;
    }

    //--------------------------

    struct Sound
    {
        uint32_t parent;
        AudioSourceSetting source;
        SoundPlaybackSetting playback_setting;
        AudioVoice voice;
        AudioVoiceVolumeGainSetting voice_volume_gain;
        AudioOutputBusSetting output_bus;
        AudioAuxiliarySendSetting auxiliary_send;
        AudioEffectSetting effect;
        AudioMetadataSetting metadata;
        AudioPositioningSetting positioning;
        RealTimeParameterControlSetting real_time_parameter_control;
        StateSetting state;
        AudioHDRSetting hdr;
        SoundMIDISetting midi;
        AudioPlaybackLimitSetting playback_limit;
        AudioVirtualVoiceSetting virtual_voice;
        AudioPlaybackPrioritySetting playback_priority;
        AudioMotionSetting motion;
        AudioMixerSetting mixer;
        bool override_voice_volume_loudness_normalization;
        bool override_game_defined_auxiliary_send;
        bool override_user_defined_auxiliary_send;
        bool override_early_reflection_auxiliary_send;
        bool override_effect;
        bool override_metadata;
        bool override_positioning;
        bool override_hdr_envelope_tracking;
        bool override_midi_event;
        bool override_midi_note_tracking;
        bool override_playback_limit;
        bool override_virtual_voice;
        bool override_playback_priority;
        bool override_mixer;
    };

    inline auto to_json(
        nlohmann::ordered_json &nlohmann_json_j,
        const Sound &nlohmann_json_t) -> void
    {
        nlohmann_json_j["parent"] = nlohmann_json_t.parent;
        nlohmann_json_j["source"] = nlohmann_json_t.source;
        nlohmann_json_j["playback_setting"] = nlohmann_json_t.playback_setting;
        nlohmann_json_j["voice"] = nlohmann_json_t.voice;
        if (k_version >= 88_ui)
        {
            nlohmann_json_j["voice_volume_gain"] = nlohmann_json_t.voice_volume_gain;
        }
        nlohmann_json_j["output_bus"] = nlohmann_json_t.output_bus;
        nlohmann_json_j["auxiliary_send"] = nlohmann_json_t.auxiliary_send;
        nlohmann_json_j["effect"] = nlohmann_json_t.effect;
        if (k_version >= 140_ui)
        {
            nlohmann_json_j["metadata"] = nlohmann_json_t.metadata;
        }
        nlohmann_json_j["positioning"] = nlohmann_json_t.positioning;
        nlohmann_json_j["real_time_parameter_control"] = nlohmann_json_t.real_time_parameter_control;
        nlohmann_json_j["state"] = nlohmann_json_t.state;
        if (k_version >= 88_ui)
        {
            nlohmann_json_j["hdr"] = nlohmann_json_t.hdr;
        }
        if (k_version >= 112_ui)
        {
            nlohmann_json_j["midi"] = nlohmann_json_t.midi;
        }
        nlohmann_json_j["playback_limit"] = nlohmann_json_t.playback_limit;
        nlohmann_json_j["virtual_voice"] = nlohmann_json_t.virtual_voice;
        nlohmann_json_j["playback_priority"] = nlohmann_json_t.playback_priority;
        if (k_version < 128_ui)
        {
            nlohmann_json_j["motion"] = nlohmann_json_t.motion;
        }
        if (k_version >= 112_ui)
        {
            nlohmann_json_j["mixer"] = nlohmann_json_t.mixer;
        }
        if (k_version >= 88_ui)
        {
            nlohmann_json_j["override_voice_volume_loudness_normalization"] = nlohmann_json_t.override_voice_volume_loudness_normalization;
        }
        nlohmann_json_j["override_game_defined_auxiliary_send"] = nlohmann_json_t.override_game_defined_auxiliary_send;
        nlohmann_json_j["override_user_defined_auxiliary_send"] = nlohmann_json_t.override_user_defined_auxiliary_send;
        if (k_version >= 135_ui)
        {
            nlohmann_json_j["override_early_reflection_auxiliary_send"] = nlohmann_json_t.override_early_reflection_auxiliary_send;
        }
        nlohmann_json_j["override_effect"] = nlohmann_json_t.override_effect;
        if (k_version >= 140_ui)
        {
            nlohmann_json_j["override_metadata"] = nlohmann_json_t.override_metadata;
        }
        nlohmann_json_j["override_positioning"] = nlohmann_json_t.override_positioning;
        if (k_version >= 88_ui)
        {
            nlohmann_json_j["override_hdr_envelope_tracking"] = nlohmann_json_t.override_hdr_envelope_tracking;
        }
        if (k_version >= 112_ui) {
            nlohmann_json_j["override_midi_event"] = nlohmann_json_t.override_midi_event;
            nlohmann_json_j["override_midi_note_tracking"] = nlohmann_json_t.override_midi_note_tracking;
        }
        nlohmann_json_j["override_playback_limit"] = nlohmann_json_t.override_playback_limit;
        nlohmann_json_j["override_virtual_voice"] = nlohmann_json_t.override_virtual_voice;
        nlohmann_json_j["override_playback_priority"] = nlohmann_json_t.override_playback_priority;
        if (k_version >= 112_ui && k_version < 150_ui)
        {
            nlohmann_json_j["override_mixer"] = nlohmann_json_t.override_mixer;
        }
        return;
    }

    inline auto from_json(
        const nlohmann::ordered_json &nlohmann_json_j,
        Sound &nlohmann_json_t) -> void
    {
        nlohmann_json_j.at("parent").get_to(nlohmann_json_t.parent);
        nlohmann_json_j.at("source").get_to(nlohmann_json_t.source);
        nlohmann_json_j.at("playback_setting").get_to(nlohmann_json_t.playback_setting);
        nlohmann_json_j.at("voice").get_to(nlohmann_json_t.voice);
        if (k_version >= 88_ui)
        {
            nlohmann_json_j.at("voice_volume_gain").get_to(nlohmann_json_t.voice_volume_gain);
        }
        nlohmann_json_j.at("output_bus").get_to(nlohmann_json_t.output_bus);
        nlohmann_json_j.at("auxiliary_send").get_to(nlohmann_json_t.auxiliary_send);
        nlohmann_json_j.at("effect").get_to(nlohmann_json_t.effect);
        if (k_version >= 140_ui)
        {
            nlohmann_json_j.at("metadata").get_to(nlohmann_json_t.metadata);
        }
        nlohmann_json_j.at("positioning").get_to(nlohmann_json_t.positioning);
        nlohmann_json_j.at("real_time_parameter_control").get_to(nlohmann_json_t.real_time_parameter_control);
        nlohmann_json_j.at("state").get_to(nlohmann_json_t.state);
        if (k_version >= 88_ui)
        {
            nlohmann_json_j.at("hdr").get_to(nlohmann_json_t.hdr);
        }
        if (k_version >= 112_ui)
        {
            nlohmann_json_j.at("midi").get_to(nlohmann_json_t.midi);
        }
        nlohmann_json_j.at("playback_limit").get_to(nlohmann_json_t.playback_limit);
        nlohmann_json_j.at("virtual_voice").get_to(nlohmann_json_t.virtual_voice);
        nlohmann_json_j.at("playback_priority").get_to(nlohmann_json_t.playback_priority);
        if (k_version < 128_ui)
        {
            nlohmann_json_j.at("motion").get_to(nlohmann_json_t.motion);
        }
        if (k_version >= 112_ui)
        {
            nlohmann_json_j.at("mixer").get_to(nlohmann_json_t.mixer);
        }
        if (k_version >= 88_ui)
        {
            nlohmann_json_j.at("override_voice_volume_loudness_normalization").get_to(nlohmann_json_t.override_voice_volume_loudness_normalization);
        }
        nlohmann_json_j.at("override_game_defined_auxiliary_send").get_to(nlohmann_json_t.override_game_defined_auxiliary_send);
        nlohmann_json_j.at("override_user_defined_auxiliary_send").get_to(nlohmann_json_t.override_user_defined_auxiliary_send);
        if (k_version >= 135_ui)
        {
            nlohmann_json_j.at("override_early_reflection_auxiliary_send").get_to(nlohmann_json_t.override_early_reflection_auxiliary_send);
        }
        nlohmann_json_j.at("override_effect").get_to(nlohmann_json_t.override_effect);
        if (k_version >= 140_ui)
        {
            nlohmann_json_j.at("override_metadata").get_to(nlohmann_json_t.override_metadata);
        }
        nlohmann_json_j.at("override_positioning").get_to(nlohmann_json_t.override_positioning);
        if (k_version >= 88_ui) 
        {
            nlohmann_json_j.at("override_hdr_envelope_tracking").get_to(nlohmann_json_t.override_hdr_envelope_tracking);
        }
        if (k_version >= 112_ui) {
            nlohmann_json_j.at("override_midi_event").get_to(nlohmann_json_t.override_midi_event);
            nlohmann_json_j.at("override_midi_note_tracking").get_to(nlohmann_json_t.override_midi_note_tracking);
        }
        nlohmann_json_j.at("override_playback_limit").get_to(nlohmann_json_t.override_playback_limit);
        nlohmann_json_j.at("override_virtual_voice").get_to(nlohmann_json_t.override_virtual_voice);
        nlohmann_json_j.at("override_playback_priority").get_to(nlohmann_json_t.override_playback_priority);
        if (k_version >= 112_ui && k_version < 150_ui)
        {
            nlohmann_json_j.at("override_mixer").get_to(nlohmann_json_t.override_mixer);
        }
        return;
    }

    //--------------------------

    struct SoundPlaylistContainerPlaylistItem
    {
        uint32_t item;
        uint32_t weight;
    };

    inline auto to_json(
        nlohmann::ordered_json &nlohmann_json_j,
        const SoundPlaylistContainerPlaylistItem &nlohmann_json_t) -> void
    {
        nlohmann_json_j["item"] = nlohmann_json_t.item;
        nlohmann_json_j["weight"] = nlohmann_json_t.weight;
        return;
    }

    inline auto from_json(
        const nlohmann::ordered_json &nlohmann_json_j,
        SoundPlaylistContainerPlaylistItem &nlohmann_json_t) -> void
    {
        nlohmann_json_j.at("item").get_to(nlohmann_json_t.item);
        nlohmann_json_j.at("weight").get_to(nlohmann_json_t.weight);
        return;
    }

    //--------------------------

    struct AudioPlayTypeRandomSetting
    {
        AudioPlayTypeRandomType type;
        uint16_t avoid_repeat;
    };

    inline auto to_json(
        nlohmann::ordered_json &nlohmann_json_j,
        const AudioPlayTypeRandomSetting &nlohmann_json_t) -> void
    {
        nlohmann_json_j["type"] = magic_enum::enum_name(nlohmann_json_t.type);
        nlohmann_json_j["avoid_repeat"] = nlohmann_json_t.avoid_repeat;
        return;
    }

    inline auto from_json(
        const nlohmann::ordered_json &nlohmann_json_j,
        AudioPlayTypeRandomSetting &nlohmann_json_t) -> void
    {
        nlohmann_json_t.type = magic_enum_cast<AudioPlayTypeRandomType>(nlohmann_json_j.at("type").get<std::string>());
        nlohmann_json_j.at("avoid_repeat").get_to(nlohmann_json_t.avoid_repeat);
        return;
    }

    //--------------------------

    struct AudioPlayTypeSequenceSetting
    {
        AudioPlayTypeSequenceAtEndOfPlaylist at_end_of_playlist;
    };

    inline auto to_json(
        nlohmann::ordered_json &nlohmann_json_j,
        const AudioPlayTypeSequenceSetting &nlohmann_json_t) -> void
    {
        nlohmann_json_j["at_end_of_playlist"] = magic_enum::enum_name(nlohmann_json_t.at_end_of_playlist);
        return;
    }

    inline auto from_json(
        const nlohmann::ordered_json &nlohmann_json_j,
        AudioPlayTypeSequenceSetting &nlohmann_json_t) -> void
    {
        nlohmann_json_t.at_end_of_playlist = magic_enum_cast<AudioPlayTypeSequenceAtEndOfPlaylist>(nlohmann_json_j.at("at_end_of_playlist").get<std::string>());
        return;
    }

    //--------------------------

    struct AudioPlayTypeSetting
    {
        AudioPlayTypeRandomSetting random;
        AudioPlayTypeSequenceSetting sequence;
    };

    inline auto to_json(
        nlohmann::ordered_json &nlohmann_json_j,
        const AudioPlayTypeSetting &nlohmann_json_t) -> void
    {
        nlohmann_json_j["random"] = nlohmann_json_t.random;
        nlohmann_json_j["sequence"] = nlohmann_json_t.sequence;
        return;
    }

    inline auto from_json(
        const nlohmann::ordered_json &nlohmann_json_j,
        AudioPlayTypeSetting &nlohmann_json_t) -> void
    {
        nlohmann_json_j.at("random").get_to(nlohmann_json_t.random);
        nlohmann_json_j.at("sequence").get_to(nlohmann_json_t.sequence);
        return;
    }

    //--------------------------

    struct AudioPlayModeStepSetting
    {
    };

    inline auto to_json(
        nlohmann::ordered_json &nlohmann_json_j,
        const AudioPlayModeStepSetting &nlohmann_json_t) -> void
    {
        return;
    }

    inline auto from_json(
        const nlohmann::ordered_json &nlohmann_json_j,
        AudioPlayModeStepSetting &nlohmann_json_t) -> void
    {
        return;
    }

    //--------------------------

    struct AudioPlayModeContinuousSetting
    {
        bool always_reset_playlist;
        RandomizableValue<uint16_t> loop;
        AudioPlayModeContinuousTransitionType transition_type;
        RandomizableValue<float> transition_duration;
    };

    inline auto to_json(
        nlohmann::ordered_json &nlohmann_json_j,
        const AudioPlayModeContinuousSetting &nlohmann_json_t) -> void
    {
        nlohmann_json_j["always_reset_playlist"] = nlohmann_json_t.always_reset_playlist;
        if (k_version < 88_ui)
        {
            nlohmann_json_j["loop"] = nlohmann_json_t.loop.value;
        }
        else
        {
            nlohmann_json_j["loop"] = nlohmann_json_t.loop;
        }
        nlohmann_json_j["transition_type"] = nlohmann_json_t.transition_type;
        nlohmann_json_j["transition_duration"] = nlohmann_json_t.transition_duration;
        return;
    }

    inline auto from_json(
        const nlohmann::ordered_json &nlohmann_json_j,
        AudioPlayModeContinuousSetting &nlohmann_json_t) -> void
    {
        nlohmann_json_j.at("always_reset_playlist").get_to(nlohmann_json_t.always_reset_playlist);
        if (k_version < 88_ui) {
            nlohmann_json_j.at("loop").get_to(nlohmann_json_t.loop.value);
        }
        else {
            nlohmann_json_j.at("loop").get_to(nlohmann_json_t.loop);
        }
        nlohmann_json_j.at("transition_type").get_to(nlohmann_json_t.transition_type);
        nlohmann_json_j.at("transition_duration").get_to(nlohmann_json_t.transition_duration);
        return;
    }

    //--------------------------

    struct AudioPlayModeSetting
    {
        AudioPlayModeStepSetting step;
        AudioPlayModeContinuousSetting continuous;
    };

    inline auto to_json(
        nlohmann::ordered_json &nlohmann_json_j,
        const AudioPlayModeSetting &nlohmann_json_t) -> void
    {
        nlohmann_json_j["step"] = nlohmann_json_t.step;
        nlohmann_json_j["continuous"] = nlohmann_json_t.continuous;
        return;
    }

    inline auto from_json(
        const nlohmann::ordered_json &nlohmann_json_j,
        AudioPlayModeSetting &nlohmann_json_t) -> void
    {
        nlohmann_json_j.at("step").get_to(nlohmann_json_t.step);
        nlohmann_json_j.at("continuous").get_to(nlohmann_json_t.continuous);
        return;
    }

    //--------------------------

    struct SoundPlaylistContainerPlaybackSetting
    {
        RandomizableValue<float> initial_delay;
        SoundPlaylistContainerScope scope;
        AudioPlayType type;
        AudioPlayTypeSetting type_setting;
        AudioPlayMode mode;
        AudioPlayModeSetting mode_setting;
        std::vector<SoundPlaylistContainerPlaylistItem> playlist;
    };

    inline auto to_json(
        nlohmann::ordered_json &nlohmann_json_j,
        const SoundPlaylistContainerPlaybackSetting &nlohmann_json_t) -> void
    {
        if (k_version >= 88_ui)
        {
            nlohmann_json_j["initial_delay"] = nlohmann_json_t.initial_delay;
        }
        nlohmann_json_j["scope"] = magic_enum::enum_name(nlohmann_json_t.scope);
        nlohmann_json_j["type"] = magic_enum::enum_name(nlohmann_json_t.type);
        nlohmann_json_j["type_setting"] = nlohmann_json_t.type_setting;
        nlohmann_json_j["mode"] = magic_enum::enum_name(nlohmann_json_t.mode);
        nlohmann_json_j["mode_setting"] = nlohmann_json_t.mode_setting;
        nlohmann_json_j["playlist"] = nlohmann_json_t.playlist;
        return;
    }

    inline auto from_json(
        const nlohmann::ordered_json &nlohmann_json_j,
        SoundPlaylistContainerPlaybackSetting &nlohmann_json_t) -> void
    {
        if (k_version >= 88_ui) {
            nlohmann_json_j.at("initial_delay").get_to(nlohmann_json_t.initial_delay);
        }
        nlohmann_json_t.scope = magic_enum_cast<SoundPlaylistContainerScope>(nlohmann_json_j.at("scope").get<std::string>());
        nlohmann_json_t.type = magic_enum_cast<AudioPlayType>(nlohmann_json_j.at("type").get<std::string>());
        nlohmann_json_j.at("type_setting").get_to(nlohmann_json_t.type_setting);
        nlohmann_json_t.mode = magic_enum_cast<AudioPlayMode>(nlohmann_json_j.at("mode").get<std::string>());
        nlohmann_json_j.at("mode_setting").get_to(nlohmann_json_t.mode_setting);
        nlohmann_json_j.at("playlist").get_to(nlohmann_json_t.playlist);
        return;
    }

    //--------------------------

    struct SoundPlaylistContainer
    {
        uint32_t parent;
        AudioSourceSetting source;
        std::vector<uint32_t> child;
        SoundPlaylistContainerPlaybackSetting playback_setting;
        AudioVoice voice;
        AudioVoiceVolumeGainSetting voice_volume_gain;
        AudioOutputBusSetting output_bus;
        AudioAuxiliarySendSetting auxiliary_send;
        AudioEffectSetting effect;
        AudioMetadataSetting metadata;
        AudioPositioningSetting positioning;
        RealTimeParameterControlSetting real_time_parameter_control;
        StateSetting state;
        AudioHDRSetting hdr;
        SoundMIDISetting midi;
        AudioPlaybackLimitSetting playback_limit;
        AudioVirtualVoiceSetting virtual_voice;
        AudioPlaybackPrioritySetting playback_priority;
        AudioMotionSetting motion;
        AudioMixerSetting mixer;
        bool override_voice_volume_loudness_normalization;
        bool override_game_defined_auxiliary_send;
        bool override_user_defined_auxiliary_send;
        bool override_early_reflection_auxiliary_send;
        bool override_effect;
        bool override_metadata;
        bool override_positioning;
        bool override_hdr_envelope_tracking;
        bool override_midi_event;
        bool override_midi_note_tracking;
        bool override_playback_limit;
        bool override_virtual_voice;
        bool override_playback_priority;
        bool override_mixer;
    };

    inline auto to_json(
        nlohmann::ordered_json &nlohmann_json_j,
        const SoundPlaylistContainer &nlohmann_json_t) -> void
    {
        nlohmann_json_j["parent"] = nlohmann_json_t.parent;
        nlohmann_json_j["source"] = nlohmann_json_t.source;
        nlohmann_json_j["child"] = nlohmann_json_t.child;
        nlohmann_json_j["playback_setting"] = nlohmann_json_t.playback_setting;
        nlohmann_json_j["voice"] = nlohmann_json_t.voice;
        if (k_version >= 88_ui)
        {
            nlohmann_json_j["voice_volume_gain"] = nlohmann_json_t.voice_volume_gain;
        }
        nlohmann_json_j["output_bus"] = nlohmann_json_t.output_bus;
        nlohmann_json_j["auxiliary_send"] = nlohmann_json_t.auxiliary_send;
        nlohmann_json_j["effect"] = nlohmann_json_t.effect;
        if (k_version >= 140_ui)
        {
            nlohmann_json_j["metadata"] = nlohmann_json_t.metadata;
        }
        nlohmann_json_j["positioning"] = nlohmann_json_t.positioning;
        nlohmann_json_j["real_time_parameter_control"] = nlohmann_json_t.real_time_parameter_control;
        nlohmann_json_j["state"] = nlohmann_json_t.state;
        if (k_version >= 88_ui)
        {
            nlohmann_json_j["hdr"] = nlohmann_json_t.hdr;
        }
        if (k_version >= 112_ui)
        {
            nlohmann_json_j["midi"] = nlohmann_json_t.midi;
        }
        nlohmann_json_j["playback_limit"] = nlohmann_json_t.playback_limit;
        nlohmann_json_j["virtual_voice"] = nlohmann_json_t.virtual_voice;
        nlohmann_json_j["playback_priority"] = nlohmann_json_t.playback_priority;
        if (k_version < 128_ui)
        {
            nlohmann_json_j["motion"] = nlohmann_json_t.motion;
        }
        if (k_version >= 112_ui)
        {
            nlohmann_json_j["mixer"] = nlohmann_json_t.mixer;
        }
        if (k_version >= 88_ui)
        {
            nlohmann_json_j["override_voice_volume_loudness_normalization"] = nlohmann_json_t.override_voice_volume_loudness_normalization;
        }
        nlohmann_json_j["override_game_defined_auxiliary_send"] = nlohmann_json_t.override_game_defined_auxiliary_send;
        nlohmann_json_j["override_user_defined_auxiliary_send"] = nlohmann_json_t.override_user_defined_auxiliary_send;
        if (k_version >= 135_ui)
        {
            nlohmann_json_j["override_early_reflection_auxiliary_send"] = nlohmann_json_t.override_early_reflection_auxiliary_send;
        }
        nlohmann_json_j["override_effect"] = nlohmann_json_t.override_effect;
        if (k_version >= 140_ui)
        {
            nlohmann_json_j["override_metadata"] = nlohmann_json_t.override_metadata;
        }
        nlohmann_json_j["override_positioning"] = nlohmann_json_t.override_positioning;
        if (k_version >= 88_ui)
        {
            nlohmann_json_j["override_hdr_envelope_tracking"] = nlohmann_json_t.override_hdr_envelope_tracking;
        }
        if (k_version >= 112_ui) {
            nlohmann_json_j["override_midi_event"] = nlohmann_json_t.override_midi_event;
            nlohmann_json_j["override_midi_note_tracking"] = nlohmann_json_t.override_midi_note_tracking;
        }
        nlohmann_json_j["override_playback_limit"] = nlohmann_json_t.override_playback_limit;
        nlohmann_json_j["override_virtual_voice"] = nlohmann_json_t.override_virtual_voice;
        nlohmann_json_j["override_playback_priority"] = nlohmann_json_t.override_playback_priority;
        if (k_version >= 112_ui && k_version < 150_ui)
        {
            nlohmann_json_j["override_mixer"] = nlohmann_json_t.override_mixer;
        }
        return;
    }

    inline auto from_json(
        const nlohmann::ordered_json &nlohmann_json_j,
        SoundPlaylistContainer &nlohmann_json_t) -> void
    {
        nlohmann_json_j.at("parent").get_to(nlohmann_json_t.parent);
        nlohmann_json_j.at("source").get_to(nlohmann_json_t.source);
        nlohmann_json_j.at("child").get_to(nlohmann_json_t.child);
        nlohmann_json_j.at("playback_setting").get_to(nlohmann_json_t.playback_setting);
        nlohmann_json_j.at("voice").get_to(nlohmann_json_t.voice);
        if (k_version >= 88_ui)
        {
            nlohmann_json_j.at("voice_volume_gain").get_to(nlohmann_json_t.voice_volume_gain);
        }
        nlohmann_json_j.at("output_bus").get_to(nlohmann_json_t.output_bus);
        nlohmann_json_j.at("auxiliary_send").get_to(nlohmann_json_t.auxiliary_send);
        nlohmann_json_j.at("effect").get_to(nlohmann_json_t.effect);
        if (k_version >= 140_ui)
        {
            nlohmann_json_j.at("metadata").get_to(nlohmann_json_t.metadata);
        }
        nlohmann_json_j.at("positioning").get_to(nlohmann_json_t.positioning);
        nlohmann_json_j.at("real_time_parameter_control").get_to(nlohmann_json_t.real_time_parameter_control);
        nlohmann_json_j.at("state").get_to(nlohmann_json_t.state);
        if (k_version >= 88_ui)
        {
            nlohmann_json_j.at("hdr").get_to(nlohmann_json_t.hdr);
        }
        if (k_version >= 112_ui)
        {
            nlohmann_json_j.at("midi").get_to(nlohmann_json_t.midi);
        }
        nlohmann_json_j.at("playback_limit").get_to(nlohmann_json_t.playback_limit);
        nlohmann_json_j.at("virtual_voice").get_to(nlohmann_json_t.virtual_voice);
        nlohmann_json_j.at("playback_priority").get_to(nlohmann_json_t.playback_priority);
        if (k_version < 128_ui)
        {
            nlohmann_json_j.at("motion").get_to(nlohmann_json_t.motion);
        }
        if (k_version >= 112_ui)
        {
            nlohmann_json_j.at("mixer").get_to(nlohmann_json_t.mixer);
        }
        if (k_version >= 88_ui)
        {
            nlohmann_json_j.at("override_voice_volume_loudness_normalization").get_to(nlohmann_json_t.override_voice_volume_loudness_normalization);
        }
        nlohmann_json_j.at("override_game_defined_auxiliary_send").get_to(nlohmann_json_t.override_game_defined_auxiliary_send);
        nlohmann_json_j.at("override_user_defined_auxiliary_send").get_to(nlohmann_json_t.override_user_defined_auxiliary_send);
        if (k_version >= 135_ui)
        {
            nlohmann_json_j.at("override_early_reflection_auxiliary_send").get_to(nlohmann_json_t.override_early_reflection_auxiliary_send);
        }
        nlohmann_json_j.at("override_effect").get_to(nlohmann_json_t.override_effect);
        if (k_version >= 140_ui)
        {
            nlohmann_json_j.at("override_metadata").get_to(nlohmann_json_t.override_metadata);
        }
        nlohmann_json_j.at("override_positioning").get_to(nlohmann_json_t.override_positioning);
        if (k_version >= 88_ui) 
        {
            nlohmann_json_j.at("override_hdr_envelope_tracking").get_to(nlohmann_json_t.override_hdr_envelope_tracking);
        }
        if (k_version >= 112_ui) {
            nlohmann_json_j.at("override_midi_event").get_to(nlohmann_json_t.override_midi_event);
            nlohmann_json_j.at("override_midi_note_tracking").get_to(nlohmann_json_t.override_midi_note_tracking);
        }
        nlohmann_json_j.at("override_playback_limit").get_to(nlohmann_json_t.override_playback_limit);
        nlohmann_json_j.at("override_virtual_voice").get_to(nlohmann_json_t.override_virtual_voice);
        nlohmann_json_j.at("override_playback_priority").get_to(nlohmann_json_t.override_playback_priority);
        if (k_version >= 112_ui && k_version < 150_ui)
        {
            nlohmann_json_j.at("override_mixer").get_to(nlohmann_json_t.override_mixer);
        }
        return;
    }

    //--------------------------

    struct SoundSwitchContainerObjectAttributeItem
    {
        uint32_t id;
        bool play_first_only;
        bool continue_to_play_across_switch;
        uint32_t u1;
        uint32_t fade_out_time;
        uint32_t fade_in_time;
    };

    inline auto to_json(
        nlohmann::ordered_json &nlohmann_json_j,
        const SoundSwitchContainerObjectAttributeItem &nlohmann_json_t) -> void
    {
        nlohmann_json_j["id"] = nlohmann_json_t.id;
        nlohmann_json_j["play_first_only"] = nlohmann_json_t.play_first_only;
        nlohmann_json_j["continue_to_play_across_switch"] = nlohmann_json_t.continue_to_play_across_switch;
        nlohmann_json_j["u1"] = nlohmann_json_t.u1;
        nlohmann_json_j["fade_out_time"] = nlohmann_json_t.fade_out_time;
        nlohmann_json_j["fade_in_time"] = nlohmann_json_t.fade_in_time;
        return;
    }

    inline auto from_json(
        const nlohmann::ordered_json &nlohmann_json_j,
        SoundSwitchContainerObjectAttributeItem &nlohmann_json_t) -> void
    {
        nlohmann_json_j.at("id").get_to(nlohmann_json_t.id);
        nlohmann_json_j.at("play_first_only").get_to(nlohmann_json_t.play_first_only);
        nlohmann_json_j.at("continue_to_play_across_switch").get_to(nlohmann_json_t.continue_to_play_across_switch);
        nlohmann_json_j.at("u1").get_to(nlohmann_json_t.u1);
        nlohmann_json_j.at("fade_out_time").get_to(nlohmann_json_t.fade_out_time);
        nlohmann_json_j.at("fade_in_time").get_to(nlohmann_json_t.fade_in_time);
        return;
    }

    //--------------------------

    struct SoundSwitchContainerObjectAssignItem
    {
        uint32_t item;
        std::vector<uint32_t> object;
    };

    inline auto to_json(
        nlohmann::ordered_json &nlohmann_json_j,
        const SoundSwitchContainerObjectAssignItem &nlohmann_json_t) -> void
    {
        nlohmann_json_j["item"] = nlohmann_json_t.item;
        nlohmann_json_j["object"] = nlohmann_json_t.object;
        return;
    }

    inline auto from_json(
        const nlohmann::ordered_json &nlohmann_json_j,
        SoundSwitchContainerObjectAssignItem &nlohmann_json_t) -> void
    {
        nlohmann_json_j.at("item").get_to(nlohmann_json_t.item);
        nlohmann_json_j.at("object").get_to(nlohmann_json_t.object);
        return;
    }

    //--------------------------

    struct SoundSwitchContainerPlaybackSetting
    {
        RandomizableValue<float> initial_delay;
        AudioPlayMode mode;
        AudioSwitcherSetting switcher;
        std::vector<SoundSwitchContainerObjectAttributeItem> object_attribute;
        std::vector<SoundSwitchContainerObjectAssignItem> object_assign;
    };

    inline auto to_json(
        nlohmann::ordered_json &nlohmann_json_j,
        const SoundSwitchContainerPlaybackSetting &nlohmann_json_t) -> void
    {
        if (k_version >= 88_ui)
        {
            nlohmann_json_j["initial_delay"] = nlohmann_json_t.initial_delay;
        }
        nlohmann_json_j["mode"] = magic_enum::enum_name(nlohmann_json_t.mode);
        nlohmann_json_j["switcher"] = nlohmann_json_t.switcher;
        nlohmann_json_j["object_attribute"] = nlohmann_json_t.object_attribute;
        nlohmann_json_j["object_assign"] = nlohmann_json_t.object_assign;
        return;
    }

    inline auto from_json(
        const nlohmann::ordered_json &nlohmann_json_j,
        SoundSwitchContainerPlaybackSetting &nlohmann_json_t) -> void
    {
        if (k_version >= 88_ui) {
            nlohmann_json_j.at("initial_delay").get_to(nlohmann_json_t.initial_delay);
        }
        nlohmann_json_t.mode = magic_enum_cast<AudioPlayMode>(nlohmann_json_j.at("mode").get<std::string>());
        nlohmann_json_j.at("switcher").get_to(nlohmann_json_t.switcher);
        nlohmann_json_j.at("object_attribute").get_to(nlohmann_json_t.object_attribute);
        nlohmann_json_j.at("object_assign").get_to(nlohmann_json_t.object_assign);
        return;
    }

    //--------------------------

    //--------------------------

    struct SoundSwitchContainer
    {
        uint32_t parent;
        AudioSourceSetting source;
        std::vector<uint32_t> child;
        SoundSwitchContainerPlaybackSetting playback_setting;
        AudioVoice voice;
        AudioVoiceVolumeGainSetting voice_volume_gain;
        AudioOutputBusSetting output_bus;
        AudioAuxiliarySendSetting auxiliary_send;
        AudioEffectSetting effect;
        AudioMetadataSetting metadata;
        AudioPositioningSetting positioning;
        RealTimeParameterControlSetting real_time_parameter_control;
        StateSetting state;
        AudioHDRSetting hdr;
        SoundMIDISetting midi;
        AudioPlaybackLimitSetting playback_limit;
        AudioVirtualVoiceSetting virtual_voice;
        AudioPlaybackPrioritySetting playback_priority;
        AudioMotionSetting motion;
        AudioMixerSetting mixer;
        bool override_voice_volume_loudness_normalization;
        bool override_game_defined_auxiliary_send;
        bool override_user_defined_auxiliary_send;
        bool override_early_reflection_auxiliary_send;
        bool override_effect;
        bool override_metadata;
        bool override_positioning;
        bool override_hdr_envelope_tracking;
        bool override_midi_event;
        bool override_midi_note_tracking;
        bool override_playback_limit;
        bool override_virtual_voice;
        bool override_playback_priority;
        bool override_mixer;
    };

    inline auto to_json(
        nlohmann::ordered_json &nlohmann_json_j,
        const SoundSwitchContainer &nlohmann_json_t) -> void
    {
        nlohmann_json_j["parent"] = nlohmann_json_t.parent;
        nlohmann_json_j["source"] = nlohmann_json_t.source;
        nlohmann_json_j["child"] = nlohmann_json_t.child;
        nlohmann_json_j["playback_setting"] = nlohmann_json_t.playback_setting;
        nlohmann_json_j["voice"] = nlohmann_json_t.voice;
        if (k_version >= 88_ui)
        {
            nlohmann_json_j["voice_volume_gain"] = nlohmann_json_t.voice_volume_gain;
        }
        nlohmann_json_j["output_bus"] = nlohmann_json_t.output_bus;
        nlohmann_json_j["auxiliary_send"] = nlohmann_json_t.auxiliary_send;
        nlohmann_json_j["effect"] = nlohmann_json_t.effect;
        if (k_version >= 140_ui)
        {
            nlohmann_json_j["metadata"] = nlohmann_json_t.metadata;
        }
        nlohmann_json_j["positioning"] = nlohmann_json_t.positioning;
        nlohmann_json_j["real_time_parameter_control"] = nlohmann_json_t.real_time_parameter_control;
        nlohmann_json_j["state"] = nlohmann_json_t.state;
        if (k_version >= 88_ui)
        {
            nlohmann_json_j["hdr"] = nlohmann_json_t.hdr;
        }
        if (k_version >= 112_ui)
        {
            nlohmann_json_j["midi"] = nlohmann_json_t.midi;
        }
        nlohmann_json_j["playback_limit"] = nlohmann_json_t.playback_limit;
        nlohmann_json_j["virtual_voice"] = nlohmann_json_t.virtual_voice;
        nlohmann_json_j["playback_priority"] = nlohmann_json_t.playback_priority;
        if (k_version < 128_ui)
        {
            nlohmann_json_j["motion"] = nlohmann_json_t.motion;
        }
        if (k_version >= 112_ui)
        {
            nlohmann_json_j["mixer"] = nlohmann_json_t.mixer;
        }
        if (k_version >= 88_ui)
        {
            nlohmann_json_j["override_voice_volume_loudness_normalization"] = nlohmann_json_t.override_voice_volume_loudness_normalization;
        }
        nlohmann_json_j["override_game_defined_auxiliary_send"] = nlohmann_json_t.override_game_defined_auxiliary_send;
        nlohmann_json_j["override_user_defined_auxiliary_send"] = nlohmann_json_t.override_user_defined_auxiliary_send;
        if (k_version >= 135_ui)
        {
            nlohmann_json_j["override_early_reflection_auxiliary_send"] = nlohmann_json_t.override_early_reflection_auxiliary_send;
        }
        nlohmann_json_j["override_effect"] = nlohmann_json_t.override_effect;
        if (k_version >= 140_ui)
        {
            nlohmann_json_j["override_metadata"] = nlohmann_json_t.override_metadata;
        }
        nlohmann_json_j["override_positioning"] = nlohmann_json_t.override_positioning;
        if (k_version >= 88_ui)
        {
            nlohmann_json_j["override_hdr_envelope_tracking"] = nlohmann_json_t.override_hdr_envelope_tracking;
        }
        if (k_version >= 112_ui) {
            nlohmann_json_j["override_midi_event"] = nlohmann_json_t.override_midi_event;
            nlohmann_json_j["override_midi_note_tracking"] = nlohmann_json_t.override_midi_note_tracking;
        }
        nlohmann_json_j["override_playback_limit"] = nlohmann_json_t.override_playback_limit;
        nlohmann_json_j["override_virtual_voice"] = nlohmann_json_t.override_virtual_voice;
        nlohmann_json_j["override_playback_priority"] = nlohmann_json_t.override_playback_priority;
        if (k_version >= 112_ui && k_version < 150_ui)
        {
            nlohmann_json_j["override_mixer"] = nlohmann_json_t.override_mixer;
        }
        return;
    }

    inline auto from_json(
        const nlohmann::ordered_json &nlohmann_json_j,
        SoundSwitchContainer &nlohmann_json_t) -> void
    {
        nlohmann_json_j.at("parent").get_to(nlohmann_json_t.parent);
        nlohmann_json_j.at("source").get_to(nlohmann_json_t.source);
        nlohmann_json_j.at("child").get_to(nlohmann_json_t.child);
        nlohmann_json_j.at("playback_setting").get_to(nlohmann_json_t.playback_setting);
        nlohmann_json_j.at("voice").get_to(nlohmann_json_t.voice);
        if (k_version >= 88_ui)
        {
            nlohmann_json_j.at("voice_volume_gain").get_to(nlohmann_json_t.voice_volume_gain);
        }
        nlohmann_json_j.at("output_bus").get_to(nlohmann_json_t.output_bus);
        nlohmann_json_j.at("auxiliary_send").get_to(nlohmann_json_t.auxiliary_send);
        nlohmann_json_j.at("effect").get_to(nlohmann_json_t.effect);
        if (k_version >= 140_ui)
        {
            nlohmann_json_j.at("metadata").get_to(nlohmann_json_t.metadata);
        }
        nlohmann_json_j.at("positioning").get_to(nlohmann_json_t.positioning);
        nlohmann_json_j.at("real_time_parameter_control").get_to(nlohmann_json_t.real_time_parameter_control);
        nlohmann_json_j.at("state").get_to(nlohmann_json_t.state);
        if (k_version >= 88_ui)
        {
            nlohmann_json_j.at("hdr").get_to(nlohmann_json_t.hdr);
        }
        if (k_version >= 112_ui)
        {
            nlohmann_json_j.at("midi").get_to(nlohmann_json_t.midi);
        }
        nlohmann_json_j.at("playback_limit").get_to(nlohmann_json_t.playback_limit);
        nlohmann_json_j.at("virtual_voice").get_to(nlohmann_json_t.virtual_voice);
        nlohmann_json_j.at("playback_priority").get_to(nlohmann_json_t.playback_priority);
        if (k_version < 128_ui)
        {
            nlohmann_json_j.at("motion").get_to(nlohmann_json_t.motion);
        }
        if (k_version >= 112_ui)
        {
            nlohmann_json_j.at("mixer").get_to(nlohmann_json_t.mixer);
        }
        if (k_version >= 88_ui)
        {
            nlohmann_json_j.at("override_voice_volume_loudness_normalization").get_to(nlohmann_json_t.override_voice_volume_loudness_normalization);
        }
        nlohmann_json_j.at("override_game_defined_auxiliary_send").get_to(nlohmann_json_t.override_game_defined_auxiliary_send);
        nlohmann_json_j.at("override_user_defined_auxiliary_send").get_to(nlohmann_json_t.override_user_defined_auxiliary_send);
        if (k_version >= 135_ui)
        {
            nlohmann_json_j.at("override_early_reflection_auxiliary_send").get_to(nlohmann_json_t.override_early_reflection_auxiliary_send);
        }
        nlohmann_json_j.at("override_effect").get_to(nlohmann_json_t.override_effect);
        if (k_version >= 140_ui)
        {
            nlohmann_json_j.at("override_metadata").get_to(nlohmann_json_t.override_metadata);
        }
        nlohmann_json_j.at("override_positioning").get_to(nlohmann_json_t.override_positioning);
        if (k_version >= 88_ui) 
        {
            nlohmann_json_j.at("override_hdr_envelope_tracking").get_to(nlohmann_json_t.override_hdr_envelope_tracking);
        }
        if (k_version >= 112_ui) {
            nlohmann_json_j.at("override_midi_event").get_to(nlohmann_json_t.override_midi_event);
            nlohmann_json_j.at("override_midi_note_tracking").get_to(nlohmann_json_t.override_midi_note_tracking);
        }
        nlohmann_json_j.at("override_playback_limit").get_to(nlohmann_json_t.override_playback_limit);
        nlohmann_json_j.at("override_virtual_voice").get_to(nlohmann_json_t.override_virtual_voice);
        nlohmann_json_j.at("override_playback_priority").get_to(nlohmann_json_t.override_playback_priority);
        if (k_version >= 112_ui && k_version < 150_ui)
        {
            nlohmann_json_j.at("override_mixer").get_to(nlohmann_json_t.override_mixer);
        }
        return;
    }

    //--------------------------

    struct SoundBlendContainerTrackChildItem
    {
        uint32_t id;
        std::vector<CoordinatePoint> point;
    };

    inline auto to_json(
        nlohmann::ordered_json &nlohmann_json_j,
        const SoundBlendContainerTrackChildItem &nlohmann_json_t) -> void
    {
        nlohmann_json_j["id"] = nlohmann_json_t.id;
        nlohmann_json_j["point"] = nlohmann_json_t.point;
        return;
    }

    inline auto from_json(
        const nlohmann::ordered_json &nlohmann_json_j,
        SoundBlendContainerTrackChildItem &nlohmann_json_t) -> void
    {
        nlohmann_json_j.at("id").get_to(nlohmann_json_t.id);
        nlohmann_json_j.at("point").get_to(nlohmann_json_t.point);
        return;
    }

    //--------------------------

    struct SoundBlendContainerTrackItem
    {
        uint32_t id;
        RealTimeParameterControlSetting real_time_parameter_control;
        Parameter cross_fade;
        std::vector<SoundBlendContainerTrackChildItem> child;
    };

    inline auto to_json(
        nlohmann::ordered_json &nlohmann_json_j,
        const SoundBlendContainerTrackItem &nlohmann_json_t) -> void
    {
        nlohmann_json_j["id"] = nlohmann_json_t.id;
        nlohmann_json_j["real_time_parameter_control"] = nlohmann_json_t.real_time_parameter_control;
        nlohmann_json_j["cross_fade"] = nlohmann_json_t.cross_fade;
        nlohmann_json_j["child"] = nlohmann_json_t.child;
        return;
    }

    inline auto from_json(
        const nlohmann::ordered_json &nlohmann_json_j,
        SoundBlendContainerTrackItem &nlohmann_json_t) -> void
    {
        nlohmann_json_j.at("id").get_to(nlohmann_json_t.id);
        nlohmann_json_j.at("real_time_parameter_control").get_to(nlohmann_json_t.real_time_parameter_control);
        nlohmann_json_j.at("cross_fade").get_to(nlohmann_json_t.cross_fade);
        nlohmann_json_j.at("child").get_to(nlohmann_json_t.child);
        return;
    }

    //--------------------------

    struct SoundBlendContainerPlaybackSetting
    {
        RandomizableValue<float> initial_delay;
        AudioPlayMode mode;
        std::vector<SoundBlendContainerTrackItem> track;
    };

    inline auto to_json(
        nlohmann::ordered_json &nlohmann_json_j,
        const SoundBlendContainerPlaybackSetting &nlohmann_json_t) -> void
    {
        if (k_version >= 88_ui)
        {
            nlohmann_json_j["initial_delay"] = nlohmann_json_t.initial_delay;
        }
        if (k_version >= 120_ui)
        {
            nlohmann_json_j["mode"] = magic_enum::enum_name(nlohmann_json_t.mode);
        }
        nlohmann_json_j["track"] = nlohmann_json_t.track;
        return;
    }

    inline auto from_json(
        const nlohmann::ordered_json &nlohmann_json_j,
        SoundBlendContainerPlaybackSetting &nlohmann_json_t) -> void
    {
        if (k_version >= 88_ui)
        {
            nlohmann_json_j.at("initial_delay").get_to(nlohmann_json_t.initial_delay);
        }
        if (k_version >= 120_ui)
        {
            nlohmann_json_t.mode = magic_enum_cast<AudioPlayMode>(nlohmann_json_j.at("mode").get<std::string>());
        }
        nlohmann_json_j.at("track").get_to(nlohmann_json_t.track);
        return;
    }

    //--------------------------

    struct SoundBlendContainer
    {
        uint32_t parent;
        AudioSourceSetting source;
        std::vector<uint32_t> child;
        SoundBlendContainerPlaybackSetting playback_setting;
        AudioVoice voice;
        AudioVoiceVolumeGainSetting voice_volume_gain;
        AudioOutputBusSetting output_bus;
        AudioAuxiliarySendSetting auxiliary_send;
        AudioEffectSetting effect;
        AudioMetadataSetting metadata;
        AudioPositioningSetting positioning;
        RealTimeParameterControlSetting real_time_parameter_control;
        StateSetting state;
        AudioHDRSetting hdr;
        SoundMIDISetting midi;
        AudioPlaybackLimitSetting playback_limit;
        AudioVirtualVoiceSetting virtual_voice;
        AudioPlaybackPrioritySetting playback_priority;
        AudioMotionSetting motion;
        AudioMixerSetting mixer;
        bool override_voice_volume_loudness_normalization;
        bool override_game_defined_auxiliary_send;
        bool override_user_defined_auxiliary_send;
        bool override_early_reflection_auxiliary_send;
        bool override_effect;
        bool override_metadata;
        bool override_positioning;
        bool override_hdr_envelope_tracking;
        bool override_midi_event;
        bool override_midi_note_tracking;
        bool override_playback_limit;
        bool override_virtual_voice;
        bool override_playback_priority;
        bool override_mixer;
    };

    inline auto to_json(
        nlohmann::ordered_json &nlohmann_json_j,
        const SoundBlendContainer &nlohmann_json_t) -> void
    {
        nlohmann_json_j["parent"] = nlohmann_json_t.parent;
        nlohmann_json_j["source"] = nlohmann_json_t.source;
        nlohmann_json_j["child"] = nlohmann_json_t.child;
        nlohmann_json_j["playback_setting"] = nlohmann_json_t.playback_setting;
        nlohmann_json_j["voice"] = nlohmann_json_t.voice;
        if (k_version >= 88_ui)
        {
            nlohmann_json_j["voice_volume_gain"] = nlohmann_json_t.voice_volume_gain;
        }
        nlohmann_json_j["output_bus"] = nlohmann_json_t.output_bus;
        nlohmann_json_j["auxiliary_send"] = nlohmann_json_t.auxiliary_send;
        nlohmann_json_j["effect"] = nlohmann_json_t.effect;
        if (k_version >= 140_ui)
        {
            nlohmann_json_j["metadata"] = nlohmann_json_t.metadata;
        }
        nlohmann_json_j["positioning"] = nlohmann_json_t.positioning;
        nlohmann_json_j["real_time_parameter_control"] = nlohmann_json_t.real_time_parameter_control;
        nlohmann_json_j["state"] = nlohmann_json_t.state;
        if (k_version >= 88_ui)
        {
            nlohmann_json_j["hdr"] = nlohmann_json_t.hdr;
        }
        if (k_version >= 112_ui)
        {
            nlohmann_json_j["midi"] = nlohmann_json_t.midi;
        }
        nlohmann_json_j["playback_limit"] = nlohmann_json_t.playback_limit;
        nlohmann_json_j["virtual_voice"] = nlohmann_json_t.virtual_voice;
        nlohmann_json_j["playback_priority"] = nlohmann_json_t.playback_priority;
        if (k_version < 128_ui)
        {
            nlohmann_json_j["motion"] = nlohmann_json_t.motion;
        }
        if (k_version >= 112_ui)
        {
            nlohmann_json_j["mixer"] = nlohmann_json_t.mixer;
        }
        if (k_version >= 88_ui)
        {
            nlohmann_json_j["override_voice_volume_loudness_normalization"] = nlohmann_json_t.override_voice_volume_loudness_normalization;
        }
        nlohmann_json_j["override_game_defined_auxiliary_send"] = nlohmann_json_t.override_game_defined_auxiliary_send;
        nlohmann_json_j["override_user_defined_auxiliary_send"] = nlohmann_json_t.override_user_defined_auxiliary_send;
        if (k_version >= 135_ui)
        {
            nlohmann_json_j["override_early_reflection_auxiliary_send"] = nlohmann_json_t.override_early_reflection_auxiliary_send;
        }
        nlohmann_json_j["override_effect"] = nlohmann_json_t.override_effect;
        if (k_version >= 140_ui)
        {
            nlohmann_json_j["override_metadata"] = nlohmann_json_t.override_metadata;
        }
        nlohmann_json_j["override_positioning"] = nlohmann_json_t.override_positioning;
        if (k_version >= 88_ui)
        {
            nlohmann_json_j["override_hdr_envelope_tracking"] = nlohmann_json_t.override_hdr_envelope_tracking;
        }
        if (k_version >= 112_ui) {
            nlohmann_json_j["override_midi_event"] = nlohmann_json_t.override_midi_event;
            nlohmann_json_j["override_midi_note_tracking"] = nlohmann_json_t.override_midi_note_tracking;
        }
        nlohmann_json_j["override_playback_limit"] = nlohmann_json_t.override_playback_limit;
        nlohmann_json_j["override_virtual_voice"] = nlohmann_json_t.override_virtual_voice;
        nlohmann_json_j["override_playback_priority"] = nlohmann_json_t.override_playback_priority;
        if (k_version >= 112_ui && k_version < 150_ui)
        {
            nlohmann_json_j["override_mixer"] = nlohmann_json_t.override_mixer;
        }
        return;
    }

    inline auto from_json(
        const nlohmann::ordered_json &nlohmann_json_j,
        SoundBlendContainer &nlohmann_json_t) -> void
    {
        nlohmann_json_j.at("parent").get_to(nlohmann_json_t.parent);
        nlohmann_json_j.at("source").get_to(nlohmann_json_t.source);
        nlohmann_json_j.at("child").get_to(nlohmann_json_t.child);
        nlohmann_json_j.at("playback_setting").get_to(nlohmann_json_t.playback_setting);
        nlohmann_json_j.at("voice").get_to(nlohmann_json_t.voice);
        if (k_version >= 88_ui)
        {
            nlohmann_json_j.at("voice_volume_gain").get_to(nlohmann_json_t.voice_volume_gain);
        }
        nlohmann_json_j.at("output_bus").get_to(nlohmann_json_t.output_bus);
        nlohmann_json_j.at("auxiliary_send").get_to(nlohmann_json_t.auxiliary_send);
        nlohmann_json_j.at("effect").get_to(nlohmann_json_t.effect);
        if (k_version >= 140_ui)
        {
            nlohmann_json_j.at("metadata").get_to(nlohmann_json_t.metadata);
        }
        nlohmann_json_j.at("positioning").get_to(nlohmann_json_t.positioning);
        nlohmann_json_j.at("real_time_parameter_control").get_to(nlohmann_json_t.real_time_parameter_control);
        nlohmann_json_j.at("state").get_to(nlohmann_json_t.state);
        if (k_version >= 88_ui)
        {
            nlohmann_json_j.at("hdr").get_to(nlohmann_json_t.hdr);
        }
        if (k_version >= 112_ui)
        {
            nlohmann_json_j.at("midi").get_to(nlohmann_json_t.midi);
        }
        nlohmann_json_j.at("playback_limit").get_to(nlohmann_json_t.playback_limit);
        nlohmann_json_j.at("virtual_voice").get_to(nlohmann_json_t.virtual_voice);
        nlohmann_json_j.at("playback_priority").get_to(nlohmann_json_t.playback_priority);
        if (k_version < 128_ui)
        {
            nlohmann_json_j.at("motion").get_to(nlohmann_json_t.motion);
        }
        if (k_version >= 112_ui)
        {
            nlohmann_json_j.at("mixer").get_to(nlohmann_json_t.mixer);
        }
        if (k_version >= 88_ui)
        {
            nlohmann_json_j.at("override_voice_volume_loudness_normalization").get_to(nlohmann_json_t.override_voice_volume_loudness_normalization);
        }
        nlohmann_json_j.at("override_game_defined_auxiliary_send").get_to(nlohmann_json_t.override_game_defined_auxiliary_send);
        nlohmann_json_j.at("override_user_defined_auxiliary_send").get_to(nlohmann_json_t.override_user_defined_auxiliary_send);
        if (k_version >= 135_ui)
        {
            nlohmann_json_j.at("override_early_reflection_auxiliary_send").get_to(nlohmann_json_t.override_early_reflection_auxiliary_send);
        }
        nlohmann_json_j.at("override_effect").get_to(nlohmann_json_t.override_effect);
        if (k_version >= 140_ui)
        {
            nlohmann_json_j.at("override_metadata").get_to(nlohmann_json_t.override_metadata);
        }
        nlohmann_json_j.at("override_positioning").get_to(nlohmann_json_t.override_positioning);
        if (k_version >= 88_ui) 
        {
            nlohmann_json_j.at("override_hdr_envelope_tracking").get_to(nlohmann_json_t.override_hdr_envelope_tracking);
        }
        if (k_version >= 112_ui) {
            nlohmann_json_j.at("override_midi_event").get_to(nlohmann_json_t.override_midi_event);
            nlohmann_json_j.at("override_midi_note_tracking").get_to(nlohmann_json_t.override_midi_note_tracking);
        }
        nlohmann_json_j.at("override_playback_limit").get_to(nlohmann_json_t.override_playback_limit);
        nlohmann_json_j.at("override_virtual_voice").get_to(nlohmann_json_t.override_virtual_voice);
        nlohmann_json_j.at("override_playback_priority").get_to(nlohmann_json_t.override_playback_priority);
        if (k_version >= 112_ui && k_version < 150_ui)
        {
            nlohmann_json_j.at("override_mixer").get_to(nlohmann_json_t.override_mixer);
        }
        return;
    }

    //--------------------------

    struct ActorMixerPlaybackSetting
    {
    };

    inline auto to_json(
        nlohmann::ordered_json &nlohmann_json_j,
        const ActorMixerPlaybackSetting &nlohmann_json_t) -> void
    {
        return;
    }

    inline auto from_json(
        const nlohmann::ordered_json &nlohmann_json_j,
        ActorMixerPlaybackSetting &nlohmann_json_t) -> void
    {
        return;
    }

    //--------------------------

    struct ActorMixer
    {
        uint32_t parent;
        AudioSourceSetting source;
        std::vector<uint32_t> child;
        ActorMixerPlaybackSetting playback_setting;
        AudioVoice voice;
        AudioVoiceVolumeGainSetting voice_volume_gain;
        AudioOutputBusSetting output_bus;
        AudioAuxiliarySendSetting auxiliary_send;
        AudioEffectSetting effect;
        AudioMetadataSetting metadata;
        AudioPositioningSetting positioning;
        RealTimeParameterControlSetting real_time_parameter_control;
        StateSetting state;
        AudioHDRSetting hdr;
        SoundMIDISetting midi;
        AudioPlaybackLimitSetting playback_limit;
        AudioVirtualVoiceSetting virtual_voice;
        AudioPlaybackPrioritySetting playback_priority;
        AudioMotionSetting motion;
        AudioMixerSetting mixer;
        bool override_voice_volume_loudness_normalization;
        bool override_game_defined_auxiliary_send;
        bool override_user_defined_auxiliary_send;
        bool override_early_reflection_auxiliary_send;
        bool override_effect;
        bool override_metadata;
        bool override_positioning;
        bool override_hdr_envelope_tracking;
        bool override_midi_event;
        bool override_midi_note_tracking;
        bool override_playback_limit;
        bool override_virtual_voice;
        bool override_playback_priority;
        bool override_mixer;
    };

    inline auto to_json(
        nlohmann::ordered_json &nlohmann_json_j,
        const ActorMixer &nlohmann_json_t) -> void
    {
        nlohmann_json_j["parent"] = nlohmann_json_t.parent;
        nlohmann_json_j["source"] = nlohmann_json_t.source;
        nlohmann_json_j["child"] = nlohmann_json_t.child;
        nlohmann_json_j["playback_setting"] = nlohmann_json_t.playback_setting;
        nlohmann_json_j["voice"] = nlohmann_json_t.voice;
        if (k_version >= 88_ui)
        {
            nlohmann_json_j["voice_volume_gain"] = nlohmann_json_t.voice_volume_gain;
        }
        nlohmann_json_j["output_bus"] = nlohmann_json_t.output_bus;
        nlohmann_json_j["auxiliary_send"] = nlohmann_json_t.auxiliary_send;
        nlohmann_json_j["effect"] = nlohmann_json_t.effect;
        if (k_version >= 140_ui)
        {
            nlohmann_json_j["metadata"] = nlohmann_json_t.metadata;
        }
        nlohmann_json_j["positioning"] = nlohmann_json_t.positioning;
        nlohmann_json_j["real_time_parameter_control"] = nlohmann_json_t.real_time_parameter_control;
        nlohmann_json_j["state"] = nlohmann_json_t.state;
        if (k_version >= 88_ui)
        {
            nlohmann_json_j["hdr"] = nlohmann_json_t.hdr;
        }
        if (k_version >= 112_ui)
        {
            nlohmann_json_j["midi"] = nlohmann_json_t.midi;
        }
        nlohmann_json_j["playback_limit"] = nlohmann_json_t.playback_limit;
        nlohmann_json_j["virtual_voice"] = nlohmann_json_t.virtual_voice;
        nlohmann_json_j["playback_priority"] = nlohmann_json_t.playback_priority;
        if (k_version < 128_ui)
        {
            nlohmann_json_j["motion"] = nlohmann_json_t.motion;
        }
        if (k_version >= 112_ui)
        {
            nlohmann_json_j["mixer"] = nlohmann_json_t.mixer;
        }
        if (k_version >= 88_ui)
        {
            nlohmann_json_j["override_voice_volume_loudness_normalization"] = nlohmann_json_t.override_voice_volume_loudness_normalization;
        }
        nlohmann_json_j["override_game_defined_auxiliary_send"] = nlohmann_json_t.override_game_defined_auxiliary_send;
        nlohmann_json_j["override_user_defined_auxiliary_send"] = nlohmann_json_t.override_user_defined_auxiliary_send;
        if (k_version >= 135_ui)
        {
            nlohmann_json_j["override_early_reflection_auxiliary_send"] = nlohmann_json_t.override_early_reflection_auxiliary_send;
        }
        nlohmann_json_j["override_effect"] = nlohmann_json_t.override_effect;
        if (k_version >= 140_ui)
        {
            nlohmann_json_j["override_metadata"] = nlohmann_json_t.override_metadata;
        }
        nlohmann_json_j["override_positioning"] = nlohmann_json_t.override_positioning;
        if (k_version >= 88_ui)
        {
            nlohmann_json_j["override_hdr_envelope_tracking"] = nlohmann_json_t.override_hdr_envelope_tracking;
        }
        if (k_version >= 112_ui) {
            nlohmann_json_j["override_midi_event"] = nlohmann_json_t.override_midi_event;
            nlohmann_json_j["override_midi_note_tracking"] = nlohmann_json_t.override_midi_note_tracking;
        }
        nlohmann_json_j["override_playback_limit"] = nlohmann_json_t.override_playback_limit;
        nlohmann_json_j["override_virtual_voice"] = nlohmann_json_t.override_virtual_voice;
        nlohmann_json_j["override_playback_priority"] = nlohmann_json_t.override_playback_priority;
        if (k_version >= 112_ui && k_version < 150_ui)
        {
            nlohmann_json_j["override_mixer"] = nlohmann_json_t.override_mixer;
        }
        return;
    }

    inline auto from_json(
        const nlohmann::ordered_json &nlohmann_json_j,
        ActorMixer &nlohmann_json_t) -> void
    {
        nlohmann_json_j.at("parent").get_to(nlohmann_json_t.parent);
        nlohmann_json_j.at("source").get_to(nlohmann_json_t.source);
        nlohmann_json_j.at("child").get_to(nlohmann_json_t.child);
        nlohmann_json_j.at("playback_setting").get_to(nlohmann_json_t.playback_setting);
        nlohmann_json_j.at("voice").get_to(nlohmann_json_t.voice);
        if (k_version >= 88_ui)
        {
            nlohmann_json_j.at("voice_volume_gain").get_to(nlohmann_json_t.voice_volume_gain);
        }
        nlohmann_json_j.at("output_bus").get_to(nlohmann_json_t.output_bus);
        nlohmann_json_j.at("auxiliary_send").get_to(nlohmann_json_t.auxiliary_send);
        nlohmann_json_j.at("effect").get_to(nlohmann_json_t.effect);
        if (k_version >= 140_ui)
        {
            nlohmann_json_j.at("metadata").get_to(nlohmann_json_t.metadata);
        }
        nlohmann_json_j.at("positioning").get_to(nlohmann_json_t.positioning);
        nlohmann_json_j.at("real_time_parameter_control").get_to(nlohmann_json_t.real_time_parameter_control);
        nlohmann_json_j.at("state").get_to(nlohmann_json_t.state);
        if (k_version >= 88_ui)
        {
            nlohmann_json_j.at("hdr").get_to(nlohmann_json_t.hdr);
        }
        if (k_version >= 112_ui)
        {
            nlohmann_json_j.at("midi").get_to(nlohmann_json_t.midi);
        }
        nlohmann_json_j.at("playback_limit").get_to(nlohmann_json_t.playback_limit);
        nlohmann_json_j.at("virtual_voice").get_to(nlohmann_json_t.virtual_voice);
        nlohmann_json_j.at("playback_priority").get_to(nlohmann_json_t.playback_priority);
        if (k_version < 128_ui)
        {
            nlohmann_json_j.at("motion").get_to(nlohmann_json_t.motion);
        }
        if (k_version >= 112_ui)
        {
            nlohmann_json_j.at("mixer").get_to(nlohmann_json_t.mixer);
        }
        if (k_version >= 88_ui)
        {
            nlohmann_json_j.at("override_voice_volume_loudness_normalization").get_to(nlohmann_json_t.override_voice_volume_loudness_normalization);
        }
        nlohmann_json_j.at("override_game_defined_auxiliary_send").get_to(nlohmann_json_t.override_game_defined_auxiliary_send);
        nlohmann_json_j.at("override_user_defined_auxiliary_send").get_to(nlohmann_json_t.override_user_defined_auxiliary_send);
        if (k_version >= 135_ui)
        {
            nlohmann_json_j.at("override_early_reflection_auxiliary_send").get_to(nlohmann_json_t.override_early_reflection_auxiliary_send);
        }
        nlohmann_json_j.at("override_effect").get_to(nlohmann_json_t.override_effect);
        if (k_version >= 140_ui)
        {
            nlohmann_json_j.at("override_metadata").get_to(nlohmann_json_t.override_metadata);
        }
        nlohmann_json_j.at("override_positioning").get_to(nlohmann_json_t.override_positioning);
        if (k_version >= 88_ui) 
        {
            nlohmann_json_j.at("override_hdr_envelope_tracking").get_to(nlohmann_json_t.override_hdr_envelope_tracking);
        }
        if (k_version >= 112_ui) {
            nlohmann_json_j.at("override_midi_event").get_to(nlohmann_json_t.override_midi_event);
            nlohmann_json_j.at("override_midi_note_tracking").get_to(nlohmann_json_t.override_midi_note_tracking);
        }
        nlohmann_json_j.at("override_playback_limit").get_to(nlohmann_json_t.override_playback_limit);
        nlohmann_json_j.at("override_virtual_voice").get_to(nlohmann_json_t.override_virtual_voice);
        nlohmann_json_j.at("override_playback_priority").get_to(nlohmann_json_t.override_playback_priority);
        if (k_version >= 112_ui && k_version < 150_ui)
        {
            nlohmann_json_j.at("override_mixer").get_to(nlohmann_json_t.override_mixer);
        }
        return;
    }

    //----------------------

    struct MusicSegmentCueItem
    {
        uint32_t id;
        double time;
        std::string name;
    };

    inline auto to_json(
        nlohmann::ordered_json &nlohmann_json_j,
        const MusicSegmentCueItem &nlohmann_json_t) -> void
    {
        nlohmann_json_j["id"] = nlohmann_json_t.id;
        nlohmann_json_j["time"] = nlohmann_json_t.time;
        nlohmann_json_j["name"] = nlohmann_json_t.name;
        return;
    }

    inline auto from_json(
        const nlohmann::ordered_json &nlohmann_json_j,
        MusicSegmentCueItem &nlohmann_json_t) -> void
    {
        nlohmann_json_j.at("id").get_to(nlohmann_json_t.id);
        nlohmann_json_j.at("time").get_to(nlohmann_json_t.time);
        nlohmann_json_j.at("name").get_to(nlohmann_json_t.name);
        return;
    }

    //----------------------

    struct MusicSegmentCue
    {
        std::vector<MusicSegmentCueItem> item;
    };

    inline auto to_json(
        nlohmann::ordered_json &nlohmann_json_j,
        const MusicSegmentCue &nlohmann_json_t) -> void
    {
        nlohmann_json_j["item"] = nlohmann_json_t.item;
        return;
    }

    inline auto from_json(
        const nlohmann::ordered_json &nlohmann_json_j,
        MusicSegmentCue &nlohmann_json_t) -> void
    {
        nlohmann_json_j.at("item").get_to(nlohmann_json_t.item);
        return;
    }

    //----------------------

    struct MusicSegmentPlaybackSetting
    {
        float speed;
        double duration;
        MusicSegmentCue cue;
    };

    inline auto to_json(
        nlohmann::ordered_json &nlohmann_json_j,
        const MusicSegmentPlaybackSetting &nlohmann_json_t) -> void
    {
        if (k_version >= 112_ui)
        {
            nlohmann_json_j["speed"] = nlohmann_json_t.speed;
        }
        nlohmann_json_j["duration"] = nlohmann_json_t.duration;
        nlohmann_json_j["cue"] = nlohmann_json_t.cue;
        return;
    }

    inline auto from_json(
        const nlohmann::ordered_json &nlohmann_json_j,
        MusicSegmentPlaybackSetting &nlohmann_json_t) -> void
    {
        if (k_version >= 112_ui)
        {
            nlohmann_json_j.at("speed").get_to(nlohmann_json_t.speed);
        }
        nlohmann_json_j.at("duration").get_to(nlohmann_json_t.duration);
        nlohmann_json_j.at("cue").get_to(nlohmann_json_t.cue);
        return;
    }

    //---------------------

    struct AudioTimeSettingSignature
    {
        uint8_t first;
        uint8_t second;
    };

    inline auto to_json(
        nlohmann::ordered_json &nlohmann_json_j,
        const AudioTimeSettingSignature &nlohmann_json_t) -> void
    {
        nlohmann_json_j["first"] = nlohmann_json_t.first;
        nlohmann_json_j["second"] = nlohmann_json_t.second;
        return;
    }

    inline auto from_json(
        const nlohmann::ordered_json &nlohmann_json_j,
        AudioTimeSettingSignature &nlohmann_json_t) -> void
    {
        nlohmann_json_j.at("first").get_to(nlohmann_json_t.first);
        nlohmann_json_j.at("second").get_to(nlohmann_json_t.second);
        return;
    }

    //---------------------

    struct AudioTimeSetting
    {
        double time;
        double offset;
        float tempo;
        AudioTimeSettingSignature signature;
    };

    inline auto to_json(
        nlohmann::ordered_json &nlohmann_json_j,
        const AudioTimeSetting &nlohmann_json_t) -> void
    {
        nlohmann_json_j["time"] = nlohmann_json_t.time;
        nlohmann_json_j["offset"] = nlohmann_json_t.offset;
        nlohmann_json_j["tempo"] = nlohmann_json_t.tempo;
        nlohmann_json_j["signature"] = nlohmann_json_t.signature;
        return;
    }

    inline auto from_json(
        const nlohmann::ordered_json &nlohmann_json_j,
        AudioTimeSetting &nlohmann_json_t) -> void
    {
        nlohmann_json_j.at("time").get_to(nlohmann_json_t.time);
        nlohmann_json_j.at("offset").get_to(nlohmann_json_t.offset);
        nlohmann_json_j.at("tempo").get_to(nlohmann_json_t.tempo);
        nlohmann_json_j.at("signature").get_to(nlohmann_json_t.signature);
        return;
    }

    //---------------------

    struct MusicStingerSettingItem
    {
        uint32_t trigger;
        uint32_t segment_to_play;
        TimePoint play_at;
        uint32_t cue_name;
        uint32_t do_not_play_this_stinger_again_for;
        bool allow_playing_stinger_in_next_segment;
    };

    inline auto to_json(
        nlohmann::ordered_json &nlohmann_json_j,
        const MusicStingerSettingItem &nlohmann_json_t) -> void
    {
        nlohmann_json_j["trigger"] = nlohmann_json_t.trigger;
        nlohmann_json_j["segment_to_play"] = nlohmann_json_t.segment_to_play;
        nlohmann_json_j["play_at"] = magic_enum::enum_name(nlohmann_json_t.play_at);
        nlohmann_json_j["cue_name"] = nlohmann_json_t.cue_name;
        nlohmann_json_j["do_not_play_this_stinger_again_for"] = nlohmann_json_t.do_not_play_this_stinger_again_for;
        nlohmann_json_j["allow_playing_stinger_in_next_segment"] = nlohmann_json_t.allow_playing_stinger_in_next_segment;
        return;
    }

    inline auto from_json(
        const nlohmann::ordered_json &nlohmann_json_j,
        MusicStingerSettingItem &nlohmann_json_t) -> void
    {
        nlohmann_json_j.at("trigger").get_to(nlohmann_json_t.trigger);
        nlohmann_json_j.at("segment_to_play").get_to(nlohmann_json_t.segment_to_play);
        nlohmann_json_t.play_at = magic_enum_cast<TimePoint>(nlohmann_json_j.at("play_at").get<std::string>());
        nlohmann_json_j.at("cue_name").get_to(nlohmann_json_t.cue_name);
        nlohmann_json_j.at("do_not_play_this_stinger_again_for").get_to(nlohmann_json_t.do_not_play_this_stinger_again_for);
        nlohmann_json_j.at("allow_playing_stinger_in_next_segment").get_to(nlohmann_json_t.allow_playing_stinger_in_next_segment);
        return;
    }

    //---------------------

    struct MusicStingerSetting
    {
        std::vector<MusicStingerSettingItem> item;
    };

    inline auto to_json(
        nlohmann::ordered_json &nlohmann_json_j,
        const MusicStingerSetting &nlohmann_json_t) -> void
    {
        nlohmann_json_j["item"] = nlohmann_json_t.item;
        return;
    }

    inline auto from_json(
        const nlohmann::ordered_json &nlohmann_json_j,
        MusicStingerSetting &nlohmann_json_t) -> void
    {
        nlohmann_json_j.at("item").get_to(nlohmann_json_t.item);
        return;
    }

    //---------------------

    struct MusicSegment
    {
        uint32_t parent;
        std::vector<uint32_t> child;
        MusicSegmentPlaybackSetting playback_setting;
        AudioTimeSetting time_setting;
        AudioVoice voice;
        AudioVoiceVolumeGainSetting voice_volume_gain;
        AudioOutputBusSetting output_bus;
        AudioAuxiliarySendSetting auxiliary_send;
        AudioEffectSetting effect;
        AudioMetadataSetting metadata;
        AudioPositioningSetting positioning;
        RealTimeParameterControlSetting real_time_parameter_control;
        StateSetting state;
        MusicStingerSetting stinger;
        AudioHDRSetting hdr;
        MusicMIDISetting midi;
        AudioPlaybackLimitSetting playback_limit;
        AudioVirtualVoiceSetting virtual_voice;
        AudioPlaybackPrioritySetting playback_priority;
        AudioMotionSetting motion;
        AudioMixerSetting mixer;
        bool override_time_setting;
        bool override_voice_volume_loudness_normalization;
        bool override_game_defined_auxiliary_send;
        bool override_user_defined_auxiliary_send;
        bool override_early_reflection_auxiliary_send;
        bool override_effect;
        bool override_metadata;
        bool override_positioning;
        bool override_hdr_envelope_tracking;
        bool override_midi_target;
        bool override_midi_clip_tempo;
        bool override_playback_limit;
        bool override_virtual_voice;
        bool override_playback_priority;
        bool override_mixer;
    };

    inline auto to_json(
        nlohmann::ordered_json &nlohmann_json_j,
        const MusicSegment &nlohmann_json_t) -> void
    {
        nlohmann_json_j["parent"] = nlohmann_json_t.parent;
        nlohmann_json_j["child"] = nlohmann_json_t.child;
        nlohmann_json_j["playback_setting"] = nlohmann_json_t.playback_setting;
        nlohmann_json_j["time_setting"] = nlohmann_json_t.time_setting;
        nlohmann_json_j["voice"] = nlohmann_json_t.voice;
        if (k_version >= 88_ui) {
            nlohmann_json_j["voice_volume_gain"] = nlohmann_json_t.voice_volume_gain;
        }
        nlohmann_json_j["output_bus"] = nlohmann_json_t.output_bus;
        nlohmann_json_j["auxiliary_send"] = nlohmann_json_t.auxiliary_send;
        nlohmann_json_j["effect"] = nlohmann_json_t.effect;
        if (k_version >= 140_ui)
        {
            nlohmann_json_j["metadata"] = nlohmann_json_t.metadata;
        }
        nlohmann_json_j["positioning"] = nlohmann_json_t.positioning;
        nlohmann_json_j["real_time_parameter_control"] = nlohmann_json_t.real_time_parameter_control;
        nlohmann_json_j["state"] = nlohmann_json_t.state;
        nlohmann_json_j["stinger"] = nlohmann_json_t.stinger;
        if (k_version >= 88_ui)
        {
            nlohmann_json_j["hdr"] = nlohmann_json_t.hdr;
        }
        if (k_version >= 128_ui)
        {
            nlohmann_json_j["midi"] = nlohmann_json_t.midi;
        }
        nlohmann_json_j["playback_limit"] = nlohmann_json_t.playback_limit;
        nlohmann_json_j["virtual_voice"] = nlohmann_json_t.virtual_voice;
        nlohmann_json_j["playback_priority"] = nlohmann_json_t.playback_priority;
        if (k_version < 128_ui)
        {
            nlohmann_json_j["motion"] = nlohmann_json_t.motion;
        }
        if (k_version >= 112_ui)
        {
            nlohmann_json_j["mixer"] = nlohmann_json_t.mixer;
        }
        nlohmann_json_j["override_time_setting"] = nlohmann_json_t.override_time_setting;
        if (k_version >= 88_ui)
        {
            nlohmann_json_j["override_voice_volume_loudness_normalization"] = nlohmann_json_t.override_voice_volume_loudness_normalization;
        }
        nlohmann_json_j["override_game_defined_auxiliary_send"] = nlohmann_json_t.override_game_defined_auxiliary_send;
        nlohmann_json_j["override_user_defined_auxiliary_send"] = nlohmann_json_t.override_user_defined_auxiliary_send;
        if (k_version >= 135_ui)
        {
            nlohmann_json_j["override_early_reflection_auxiliary_send"] = nlohmann_json_t.override_early_reflection_auxiliary_send;
        }
        nlohmann_json_j["override_effect"] = nlohmann_json_t.override_effect;
        if (k_version >= 140_ui)
        {
            nlohmann_json_j["override_metadata"] = nlohmann_json_t.override_metadata;
        }
        nlohmann_json_j["override_positioning"] = nlohmann_json_t.override_positioning;
        if (k_version >= 88_ui)
        {
            nlohmann_json_j["override_hdr_envelope_tracking"] = nlohmann_json_t.override_hdr_envelope_tracking;
        }
        if (k_version >= 112_ui)
        {
            nlohmann_json_j["override_midi_target"] = nlohmann_json_t.override_midi_target;
            nlohmann_json_j["override_midi_clip_tempo"] = nlohmann_json_t.override_midi_clip_tempo;
        }
        nlohmann_json_j["override_playback_limit"] = nlohmann_json_t.override_playback_limit;
        nlohmann_json_j["override_virtual_voice"] = nlohmann_json_t.override_virtual_voice;
        nlohmann_json_j["override_playback_priority"] = nlohmann_json_t.override_playback_priority;
        if (k_version >= 112_ui && k_version < 150_ui)
        {
            nlohmann_json_j["override_mixer"] = nlohmann_json_t.override_mixer;
        }
        return;
    }

    inline auto from_json(
        const nlohmann::ordered_json &nlohmann_json_j,
        MusicSegment &nlohmann_json_t) -> void
    {
        nlohmann_json_j.at("parent").get_to(nlohmann_json_t.parent);
        nlohmann_json_j.at("child").get_to(nlohmann_json_t.child);
        nlohmann_json_j.at("playback_setting").get_to(nlohmann_json_t.playback_setting);
        nlohmann_json_j.at("time_setting").get_to(nlohmann_json_t.time_setting);
        nlohmann_json_j.at("voice").get_to(nlohmann_json_t.voice);
        if (k_version >= 88_ui)
        {
            nlohmann_json_j.at("voice_volume_gain").get_to(nlohmann_json_t.voice_volume_gain);
        }
        nlohmann_json_j.at("output_bus").get_to(nlohmann_json_t.output_bus);
        nlohmann_json_j.at("auxiliary_send").get_to(nlohmann_json_t.auxiliary_send);
        nlohmann_json_j.at("effect").get_to(nlohmann_json_t.effect);
        if (k_version >= 140_ui)
        {
            nlohmann_json_j.at("metadata").get_to(nlohmann_json_t.metadata);
        }
        nlohmann_json_j.at("positioning").get_to(nlohmann_json_t.positioning);
        nlohmann_json_j.at("real_time_parameter_control").get_to(nlohmann_json_t.real_time_parameter_control);
        nlohmann_json_j.at("state").get_to(nlohmann_json_t.state);
        nlohmann_json_j.at("stinger").get_to(nlohmann_json_t.stinger);
        if (k_version >= 88_ui)
        {
            nlohmann_json_j.at("hdr").get_to(nlohmann_json_t.hdr);
        }
        if (k_version >= 112_ui)
        {
            nlohmann_json_j.at("midi").get_to(nlohmann_json_t.midi);
        }
        nlohmann_json_j.at("playback_limit").get_to(nlohmann_json_t.playback_limit);
        nlohmann_json_j.at("virtual_voice").get_to(nlohmann_json_t.virtual_voice);
        nlohmann_json_j.at("playback_priority").get_to(nlohmann_json_t.playback_priority);
        if (k_version < 128_ui)
        {
            nlohmann_json_j.at("motion").get_to(nlohmann_json_t.motion);
        }
        if (k_version >= 112_ui)
        {
            nlohmann_json_j.at("mixer").get_to(nlohmann_json_t.mixer);
        }
        nlohmann_json_j.at("override_time_setting").get_to(nlohmann_json_t.override_time_setting);
        if (k_version >= 88_ui)
        {
            nlohmann_json_j.at("override_voice_volume_loudness_normalization").get_to(nlohmann_json_t.override_voice_volume_loudness_normalization);
        }
        nlohmann_json_j.at("override_game_defined_auxiliary_send").get_to(nlohmann_json_t.override_game_defined_auxiliary_send);
        nlohmann_json_j.at("override_user_defined_auxiliary_send").get_to(nlohmann_json_t.override_user_defined_auxiliary_send);
        if (k_version >= 135_ui)
        {
            nlohmann_json_j.at("override_early_reflection_auxiliary_send").get_to(nlohmann_json_t.override_early_reflection_auxiliary_send);
        }
        nlohmann_json_j.at("override_effect").get_to(nlohmann_json_t.override_effect);
        if (k_version >= 140_ui)
        {
            nlohmann_json_j.at("override_metadata").get_to(nlohmann_json_t.override_metadata);
        }
        nlohmann_json_j.at("override_positioning").get_to(nlohmann_json_t.override_positioning);
        if (k_version >= 88_ui) 
        {
            nlohmann_json_j.at("override_hdr_envelope_tracking").get_to(nlohmann_json_t.override_hdr_envelope_tracking);
        }
        if (k_version >= 112_ui) {
            nlohmann_json_j.at("override_midi_target").get_to(nlohmann_json_t.override_midi_target);
            nlohmann_json_j.at("override_midi_clip_tempo").get_to(nlohmann_json_t.override_midi_clip_tempo);
        }
        nlohmann_json_j.at("override_playback_limit").get_to(nlohmann_json_t.override_playback_limit);
        nlohmann_json_j.at("override_virtual_voice").get_to(nlohmann_json_t.override_virtual_voice);
        nlohmann_json_j.at("override_playback_priority").get_to(nlohmann_json_t.override_playback_priority);
        if (k_version >= 112_ui && k_version < 150_ui)
        {
            nlohmann_json_j.at("override_mixer").get_to(nlohmann_json_t.override_mixer);
        }
        return;
    }

    //--------------------------

    struct MusicPlaylistContainerPlaylistItem
    {
        uint32_t u1;
        bool group;
        uint32_t child_count;
        uint32_t item;
        AudioPlayType play_type;
        AudioPlayMode play_mode;
        AudioPlayTypeRandomSetting random_setting;
        uint32_t weight;
        uint16_t loop;
    };

    inline auto to_json(
        nlohmann::ordered_json &nlohmann_json_j,
        const MusicPlaylistContainerPlaylistItem &nlohmann_json_t) -> void
    {
        nlohmann_json_j["u1"] = nlohmann_json_t.u1;
        nlohmann_json_j["group"] = nlohmann_json_t.group;
        nlohmann_json_j["child_count"] = nlohmann_json_t.child_count;
        nlohmann_json_j["item"] = nlohmann_json_t.item;
        nlohmann_json_j["play_type"] = magic_enum::enum_name(nlohmann_json_t.play_type);
        nlohmann_json_j["play_mode"] = magic_enum::enum_name(nlohmann_json_t.play_mode);
        nlohmann_json_j["random_setting"] = nlohmann_json_t.random_setting;
        nlohmann_json_j["weight"] = nlohmann_json_t.weight;
        nlohmann_json_j["loop"] = nlohmann_json_t.loop;
        return;
    }

    inline auto from_json(
        const nlohmann::ordered_json &nlohmann_json_j,
        MusicPlaylistContainerPlaylistItem &nlohmann_json_t) -> void
    {
        nlohmann_json_j.at("u1").get_to(nlohmann_json_t.u1);
        nlohmann_json_j.at("group").get_to(nlohmann_json_t.group);
        nlohmann_json_j.at("child_count").get_to(nlohmann_json_t.child_count);
        nlohmann_json_j.at("item").get_to(nlohmann_json_t.item);
        nlohmann_json_t.play_type = magic_enum_cast<AudioPlayType>(nlohmann_json_j.at("play_type").get<std::string>());
        nlohmann_json_t.play_mode = magic_enum_cast<AudioPlayMode>(nlohmann_json_j.at("play_mode").get<std::string>());
        nlohmann_json_j.at("random_setting").get_to(nlohmann_json_t.random_setting);
        nlohmann_json_j.at("weight").get_to(nlohmann_json_t.weight);
        nlohmann_json_j.at("loop").get_to(nlohmann_json_t.loop);
        return;
    }

    //--------------------------

    struct MusicPlaylistContainerPlaybackSetting
    {
        float speed;
        std::vector<MusicPlaylistContainerPlaylistItem> playlist;
    };

    inline auto to_json(
        nlohmann::ordered_json &nlohmann_json_j,
        const MusicPlaylistContainerPlaybackSetting &nlohmann_json_t) -> void
    {
        if (k_version >= 112_ui)
        {
            nlohmann_json_j["speed"] = nlohmann_json_t.speed;
        }
        nlohmann_json_j["playlist"] = nlohmann_json_t.playlist;
        return;
    }

    inline auto from_json(
        const nlohmann::ordered_json &nlohmann_json_j,
        MusicPlaylistContainerPlaybackSetting &nlohmann_json_t) -> void
    {
        if (k_version >= 112_ui)
        {
            nlohmann_json_j.at("speed").get_to(nlohmann_json_t.speed);
        }
        nlohmann_json_j.at("playlist").get_to(nlohmann_json_t.playlist);
        return;
    }

    //---------------------

    struct MusicPlaylistContainer
    {
        uint32_t parent;
        std::vector<uint32_t> child;
        MusicPlaylistContainerPlaybackSetting playback_setting;
        AudioTimeSetting time_setting;
        AudioVoice voice;
        AudioVoiceVolumeGainSetting voice_volume_gain;
        AudioOutputBusSetting output_bus;
        AudioAuxiliarySendSetting auxiliary_send;
        AudioEffectSetting effect;
        AudioMetadataSetting metadata;
        AudioPositioningSetting positioning;
        RealTimeParameterControlSetting real_time_parameter_control;
        StateSetting state;
        MusicTransitionSetting transition;
        MusicStingerSetting stinger;
        AudioHDRSetting hdr;
        MusicMIDISetting midi;
        AudioPlaybackLimitSetting playback_limit;
        AudioVirtualVoiceSetting virtual_voice;
        AudioPlaybackPrioritySetting playback_priority;
        AudioMotionSetting motion;
        AudioMixerSetting mixer;
        bool override_time_setting;
        bool override_voice_volume_loudness_normalization;
        bool override_game_defined_auxiliary_send;
        bool override_user_defined_auxiliary_send;
        bool override_early_reflection_auxiliary_send;
        bool override_effect;
        bool override_metadata;
        bool override_positioning;
        bool override_hdr_envelope_tracking;
        bool override_midi_target;
        bool override_midi_clip_tempo;
        bool override_playback_limit;
        bool override_virtual_voice;
        bool override_playback_priority;
        bool override_mixer;
    };

    inline auto to_json(
        nlohmann::ordered_json &nlohmann_json_j,
        const MusicPlaylistContainer &nlohmann_json_t) -> void
    {
        nlohmann_json_j["parent"] = nlohmann_json_t.parent;
        nlohmann_json_j["child"] = nlohmann_json_t.child;
        nlohmann_json_j["playback_setting"] = nlohmann_json_t.playback_setting;
        nlohmann_json_j["time_setting"] = nlohmann_json_t.time_setting;
        nlohmann_json_j["voice"] = nlohmann_json_t.voice;
        if (k_version >= 88_ui) {
            nlohmann_json_j["voice_volume_gain"] = nlohmann_json_t.voice_volume_gain;
        }
        nlohmann_json_j["output_bus"] = nlohmann_json_t.output_bus;
        nlohmann_json_j["auxiliary_send"] = nlohmann_json_t.auxiliary_send;
        nlohmann_json_j["effect"] = nlohmann_json_t.effect;
        if (k_version >= 140_ui)
        {
            nlohmann_json_j["metadata"] = nlohmann_json_t.metadata;
        }
        nlohmann_json_j["positioning"] = nlohmann_json_t.positioning;
        nlohmann_json_j["real_time_parameter_control"] = nlohmann_json_t.real_time_parameter_control;
        nlohmann_json_j["state"] = nlohmann_json_t.state;
        nlohmann_json_j["transition"] = nlohmann_json_t.transition;
        nlohmann_json_j["stinger"] = nlohmann_json_t.stinger;
        if (k_version >= 88_ui)
        {
            nlohmann_json_j["hdr"] = nlohmann_json_t.hdr;
        }
        if (k_version >= 128_ui)
        {
            nlohmann_json_j["midi"] = nlohmann_json_t.midi;
        }
        nlohmann_json_j["playback_limit"] = nlohmann_json_t.playback_limit;
        nlohmann_json_j["virtual_voice"] = nlohmann_json_t.virtual_voice;
        nlohmann_json_j["playback_priority"] = nlohmann_json_t.playback_priority;
        if (k_version < 128_ui)
        {
            nlohmann_json_j["motion"] = nlohmann_json_t.motion;
        }
        if (k_version >= 112_ui)
        {
            nlohmann_json_j["mixer"] = nlohmann_json_t.mixer;
        }
        nlohmann_json_j["override_time_setting"] = nlohmann_json_t.override_time_setting;
        if (k_version >= 88_ui)
        {
            nlohmann_json_j["override_voice_volume_loudness_normalization"] = nlohmann_json_t.override_voice_volume_loudness_normalization;
        }
        nlohmann_json_j["override_game_defined_auxiliary_send"] = nlohmann_json_t.override_game_defined_auxiliary_send;
        nlohmann_json_j["override_user_defined_auxiliary_send"] = nlohmann_json_t.override_user_defined_auxiliary_send;
        if (k_version >= 135_ui)
        {
            nlohmann_json_j["override_early_reflection_auxiliary_send"] = nlohmann_json_t.override_early_reflection_auxiliary_send;
        }
        nlohmann_json_j["override_effect"] = nlohmann_json_t.override_effect;
        if (k_version >= 140_ui)
        {
            nlohmann_json_j["override_metadata"] = nlohmann_json_t.override_metadata;
        }
        nlohmann_json_j["override_positioning"] = nlohmann_json_t.override_positioning;
        if (k_version >= 88_ui)
        {
            nlohmann_json_j["override_hdr_envelope_tracking"] = nlohmann_json_t.override_hdr_envelope_tracking;
        }
        if (k_version >= 112_ui)
        {
            nlohmann_json_j["override_midi_target"] = nlohmann_json_t.override_midi_target;
            nlohmann_json_j["override_midi_clip_tempo"] = nlohmann_json_t.override_midi_clip_tempo;
        }
        nlohmann_json_j["override_playback_limit"] = nlohmann_json_t.override_playback_limit;
        nlohmann_json_j["override_virtual_voice"] = nlohmann_json_t.override_virtual_voice;
        nlohmann_json_j["override_playback_priority"] = nlohmann_json_t.override_playback_priority;
        if (k_version >= 112_ui && k_version < 150_ui)
        {
            nlohmann_json_j["override_mixer"] = nlohmann_json_t.override_mixer;
        }
        return;
    }

    inline auto from_json(
        const nlohmann::ordered_json &nlohmann_json_j,
        MusicPlaylistContainer &nlohmann_json_t) -> void
    {
        nlohmann_json_j.at("parent").get_to(nlohmann_json_t.parent);
        nlohmann_json_j.at("child").get_to(nlohmann_json_t.child);
        nlohmann_json_j.at("playback_setting").get_to(nlohmann_json_t.playback_setting);
        nlohmann_json_j.at("time_setting").get_to(nlohmann_json_t.time_setting);
        nlohmann_json_j.at("voice").get_to(nlohmann_json_t.voice);
        if (k_version >= 88_ui)
        {
            nlohmann_json_j.at("voice_volume_gain").get_to(nlohmann_json_t.voice_volume_gain);
        }
        nlohmann_json_j.at("output_bus").get_to(nlohmann_json_t.output_bus);
        nlohmann_json_j.at("auxiliary_send").get_to(nlohmann_json_t.auxiliary_send);
        nlohmann_json_j.at("effect").get_to(nlohmann_json_t.effect);
        if (k_version >= 140_ui)
        {
            nlohmann_json_j.at("metadata").get_to(nlohmann_json_t.metadata);
        }
        nlohmann_json_j.at("positioning").get_to(nlohmann_json_t.positioning);
        nlohmann_json_j.at("real_time_parameter_control").get_to(nlohmann_json_t.real_time_parameter_control);
        nlohmann_json_j.at("state").get_to(nlohmann_json_t.state);
        nlohmann_json_j.at("transition").get_to(nlohmann_json_t.transition);
        nlohmann_json_j.at("stinger").get_to(nlohmann_json_t.stinger);
        if (k_version >= 88_ui)
        {
            nlohmann_json_j.at("hdr").get_to(nlohmann_json_t.hdr);
        }
        if (k_version >= 112_ui)
        {
            nlohmann_json_j.at("midi").get_to(nlohmann_json_t.midi);
        }
        nlohmann_json_j.at("playback_limit").get_to(nlohmann_json_t.playback_limit);
        nlohmann_json_j.at("virtual_voice").get_to(nlohmann_json_t.virtual_voice);
        nlohmann_json_j.at("playback_priority").get_to(nlohmann_json_t.playback_priority);
        if (k_version < 128_ui)
        {
            nlohmann_json_j.at("motion").get_to(nlohmann_json_t.motion);
        }
        if (k_version >= 112_ui)
        {
            nlohmann_json_j.at("mixer").get_to(nlohmann_json_t.mixer);
        }
        nlohmann_json_j.at("override_time_setting").get_to(nlohmann_json_t.override_time_setting);
        if (k_version >= 88_ui)
        {
            nlohmann_json_j.at("override_voice_volume_loudness_normalization").get_to(nlohmann_json_t.override_voice_volume_loudness_normalization);
        }
        nlohmann_json_j.at("override_game_defined_auxiliary_send").get_to(nlohmann_json_t.override_game_defined_auxiliary_send);
        nlohmann_json_j.at("override_user_defined_auxiliary_send").get_to(nlohmann_json_t.override_user_defined_auxiliary_send);
        if (k_version >= 135_ui)
        {
            nlohmann_json_j.at("override_early_reflection_auxiliary_send").get_to(nlohmann_json_t.override_early_reflection_auxiliary_send);
        }
        nlohmann_json_j.at("override_effect").get_to(nlohmann_json_t.override_effect);
        if (k_version >= 140_ui)
        {
            nlohmann_json_j.at("override_metadata").get_to(nlohmann_json_t.override_metadata);
        }
        nlohmann_json_j.at("override_positioning").get_to(nlohmann_json_t.override_positioning);
        if (k_version >= 88_ui) 
        {
            nlohmann_json_j.at("override_hdr_envelope_tracking").get_to(nlohmann_json_t.override_hdr_envelope_tracking);
        }
        if (k_version >= 112_ui) {
            nlohmann_json_j.at("override_midi_target").get_to(nlohmann_json_t.override_midi_target);
            nlohmann_json_j.at("override_midi_clip_tempo").get_to(nlohmann_json_t.override_midi_clip_tempo);
        }
        nlohmann_json_j.at("override_playback_limit").get_to(nlohmann_json_t.override_playback_limit);
        nlohmann_json_j.at("override_virtual_voice").get_to(nlohmann_json_t.override_virtual_voice);
        nlohmann_json_j.at("override_playback_priority").get_to(nlohmann_json_t.override_playback_priority);
        if (k_version >= 112_ui && k_version < 150_ui)
        {
            nlohmann_json_j.at("override_mixer").get_to(nlohmann_json_t.override_mixer);
        }
        return;
    }

    //--------------------------

    struct MusicSwitchContainerAssociationItem
    {
        uint32_t item;
        uint32_t child;
    };

    inline auto to_json(
        nlohmann::ordered_json &nlohmann_json_j,
        const MusicSwitchContainerAssociationItem &nlohmann_json_t) -> void
    {
        if (k_version < 88_ui)
        {
            nlohmann_json_j["item"] = nlohmann_json_t.item;
            nlohmann_json_j["child"] = nlohmann_json_t.child;
        }
        return;
    }

    inline auto from_json(
        const nlohmann::ordered_json &nlohmann_json_j,
        MusicSwitchContainerAssociationItem &nlohmann_json_t) -> void
    {
        if (k_version < 88_ui)
        {
            nlohmann_json_j.at("item").get_to(nlohmann_json_t.item);
            nlohmann_json_j.at("child").get_to(nlohmann_json_t.child);
        }
        return;
    }

    //--------------------------

    struct MusicSwitchContainerPlaybackSetting
    {
        float speed;
        bool continue_playing_on_switch_change;
        AudioSwitcherSetting switcher;
        std::vector<MusicSwitchContainerAssociationItem> association_list;
        AudioAssociationSetting association;
    };

    inline auto to_json(
        nlohmann::ordered_json &nlohmann_json_j,
        const MusicSwitchContainerPlaybackSetting &nlohmann_json_t) -> void
    {
        if (k_version < 88_ui)
        {
            nlohmann_json_j["continue_playing_on_switch_change"] = nlohmann_json_t.continue_playing_on_switch_change;
            nlohmann_json_j["switcher"] = nlohmann_json_t.switcher;
            nlohmann_json_j["association"] = nlohmann_json_t.association_list;
        }
        else
        {
            nlohmann_json_j["speed"] = nlohmann_json_t.speed;
            nlohmann_json_j["continue_playing_on_switch_change"] = nlohmann_json_t.continue_playing_on_switch_change;
            nlohmann_json_j["association"] = nlohmann_json_t.association;
        }
        return;
    }

    inline auto from_json(
        const nlohmann::ordered_json &nlohmann_json_j,
        MusicSwitchContainerPlaybackSetting &nlohmann_json_t) -> void
    {
        if (k_version < 88_ui)
        {
            nlohmann_json_j.at("continue_playing_on_switch_change").get_to(nlohmann_json_t.continue_playing_on_switch_change);
            nlohmann_json_j.at("switcher").get_to(nlohmann_json_t.switcher);
            nlohmann_json_j.at("association").get_to(nlohmann_json_t.association);
        }
        else
        {
            nlohmann_json_j.at("speed").get_to(nlohmann_json_t.speed);
            nlohmann_json_j.at("continue_playing_on_switch_change").get_to(nlohmann_json_t.continue_playing_on_switch_change);
            nlohmann_json_j.at("association").get_to(nlohmann_json_t.association);
        }
        return;
    }

    //---------------------

    struct MusicSwitchContainer
    {
        uint32_t parent;
        std::vector<uint32_t> child;
        MusicSwitchContainerPlaybackSetting playback_setting;
        AudioTimeSetting time_setting;
        AudioVoice voice;
        AudioVoiceVolumeGainSetting voice_volume_gain;
        AudioOutputBusSetting output_bus;
        AudioAuxiliarySendSetting auxiliary_send;
        AudioEffectSetting effect;
        AudioMetadataSetting metadata;
        AudioPositioningSetting positioning;
        RealTimeParameterControlSetting real_time_parameter_control;
        StateSetting state;
        MusicTransitionSetting transition;
        MusicStingerSetting stinger;
        AudioHDRSetting hdr;
        MusicMIDISetting midi;
        AudioPlaybackLimitSetting playback_limit;
        AudioVirtualVoiceSetting virtual_voice;
        AudioPlaybackPrioritySetting playback_priority;
        AudioMotionSetting motion;
        AudioMixerSetting mixer;
        bool override_time_setting;
        bool override_voice_volume_loudness_normalization;
        bool override_game_defined_auxiliary_send;
        bool override_user_defined_auxiliary_send;
        bool override_early_reflection_auxiliary_send;
        bool override_effect;
        bool override_metadata;
        bool override_positioning;
        bool override_hdr_envelope_tracking;
        bool override_midi_target;
        bool override_midi_clip_tempo;
        bool override_playback_limit;
        bool override_virtual_voice;
        bool override_playback_priority;
        bool override_mixer;
    };

    inline auto to_json(
        nlohmann::ordered_json &nlohmann_json_j,
        const MusicSwitchContainer &nlohmann_json_t) -> void
    {
        nlohmann_json_j["parent"] = nlohmann_json_t.parent;
        nlohmann_json_j["child"] = nlohmann_json_t.child;
        nlohmann_json_j["playback_setting"] = nlohmann_json_t.playback_setting;
        nlohmann_json_j["time_setting"] = nlohmann_json_t.time_setting;
        nlohmann_json_j["voice"] = nlohmann_json_t.voice;
        if (k_version >= 88_ui) {
            nlohmann_json_j["voice_volume_gain"] = nlohmann_json_t.voice_volume_gain;
        }
        nlohmann_json_j["output_bus"] = nlohmann_json_t.output_bus;
        nlohmann_json_j["auxiliary_send"] = nlohmann_json_t.auxiliary_send;
        nlohmann_json_j["effect"] = nlohmann_json_t.effect;
        if (k_version >= 140_ui)
        {
            nlohmann_json_j["metadata"] = nlohmann_json_t.metadata;
        }
        nlohmann_json_j["positioning"] = nlohmann_json_t.positioning;
        nlohmann_json_j["real_time_parameter_control"] = nlohmann_json_t.real_time_parameter_control;
        nlohmann_json_j["state"] = nlohmann_json_t.state;
        nlohmann_json_j["transition"] = nlohmann_json_t.transition;
        nlohmann_json_j["stinger"] = nlohmann_json_t.stinger;
        if (k_version >= 88_ui)
        {
            nlohmann_json_j["hdr"] = nlohmann_json_t.hdr;
        }
        if (k_version >= 128_ui)
        {
            nlohmann_json_j["midi"] = nlohmann_json_t.midi;
        }
        nlohmann_json_j["playback_limit"] = nlohmann_json_t.playback_limit;
        nlohmann_json_j["virtual_voice"] = nlohmann_json_t.virtual_voice;
        nlohmann_json_j["playback_priority"] = nlohmann_json_t.playback_priority;
        if (k_version < 128_ui)
        {
            nlohmann_json_j["motion"] = nlohmann_json_t.motion;
        }
        if (k_version >= 112_ui)
        {
            nlohmann_json_j["mixer"] = nlohmann_json_t.mixer;
        }
        nlohmann_json_j["override_time_setting"] = nlohmann_json_t.override_time_setting;
        if (k_version >= 88_ui)
        {
            nlohmann_json_j["override_voice_volume_loudness_normalization"] = nlohmann_json_t.override_voice_volume_loudness_normalization;
        }
        nlohmann_json_j["override_game_defined_auxiliary_send"] = nlohmann_json_t.override_game_defined_auxiliary_send;
        nlohmann_json_j["override_user_defined_auxiliary_send"] = nlohmann_json_t.override_user_defined_auxiliary_send;
        if (k_version >= 135_ui)
        {
            nlohmann_json_j["override_early_reflection_auxiliary_send"] = nlohmann_json_t.override_early_reflection_auxiliary_send;
        }
        nlohmann_json_j["override_effect"] = nlohmann_json_t.override_effect;
        if (k_version >= 140_ui)
        {
            nlohmann_json_j["override_metadata"] = nlohmann_json_t.override_metadata;
        }
        nlohmann_json_j["override_positioning"] = nlohmann_json_t.override_positioning;
        if (k_version >= 88_ui)
        {
            nlohmann_json_j["override_hdr_envelope_tracking"] = nlohmann_json_t.override_hdr_envelope_tracking;
        }
        if (k_version >= 112_ui)
        {
            nlohmann_json_j["override_midi_target"] = nlohmann_json_t.override_midi_target;
            nlohmann_json_j["override_midi_clip_tempo"] = nlohmann_json_t.override_midi_clip_tempo;
        }
        nlohmann_json_j["override_playback_limit"] = nlohmann_json_t.override_playback_limit;
        nlohmann_json_j["override_virtual_voice"] = nlohmann_json_t.override_virtual_voice;
        nlohmann_json_j["override_playback_priority"] = nlohmann_json_t.override_playback_priority;
        if (k_version >= 112_ui && k_version < 150_ui)
        {
            nlohmann_json_j["override_mixer"] = nlohmann_json_t.override_mixer;
        }
        return;
    }

    inline auto from_json(
        const nlohmann::ordered_json &nlohmann_json_j,
        MusicSwitchContainer &nlohmann_json_t) -> void
    {
        nlohmann_json_j.at("parent").get_to(nlohmann_json_t.parent);
        nlohmann_json_j.at("child").get_to(nlohmann_json_t.child);
        nlohmann_json_j.at("playback_setting").get_to(nlohmann_json_t.playback_setting);
        nlohmann_json_j.at("time_setting").get_to(nlohmann_json_t.time_setting);
        nlohmann_json_j.at("voice").get_to(nlohmann_json_t.voice);
        if (k_version >= 88_ui)
        {
            nlohmann_json_j.at("voice_volume_gain").get_to(nlohmann_json_t.voice_volume_gain);
        }
        nlohmann_json_j.at("output_bus").get_to(nlohmann_json_t.output_bus);
        nlohmann_json_j.at("auxiliary_send").get_to(nlohmann_json_t.auxiliary_send);
        nlohmann_json_j.at("effect").get_to(nlohmann_json_t.effect);
        if (k_version >= 140_ui)
        {
            nlohmann_json_j.at("metadata").get_to(nlohmann_json_t.metadata);
        }
        nlohmann_json_j.at("positioning").get_to(nlohmann_json_t.positioning);
        nlohmann_json_j.at("real_time_parameter_control").get_to(nlohmann_json_t.real_time_parameter_control);
        nlohmann_json_j.at("state").get_to(nlohmann_json_t.state);
        nlohmann_json_j.at("transition").get_to(nlohmann_json_t.transition);
        nlohmann_json_j.at("stinger").get_to(nlohmann_json_t.stinger);
        if (k_version >= 88_ui)
        {
            nlohmann_json_j.at("hdr").get_to(nlohmann_json_t.hdr);
        }
        if (k_version >= 112_ui)
        {
            nlohmann_json_j.at("midi").get_to(nlohmann_json_t.midi);
        }
        nlohmann_json_j.at("playback_limit").get_to(nlohmann_json_t.playback_limit);
        nlohmann_json_j.at("virtual_voice").get_to(nlohmann_json_t.virtual_voice);
        nlohmann_json_j.at("playback_priority").get_to(nlohmann_json_t.playback_priority);
        if (k_version < 128_ui)
        {
            nlohmann_json_j.at("motion").get_to(nlohmann_json_t.motion);
        }
        if (k_version >= 112_ui)
        {
            nlohmann_json_j.at("mixer").get_to(nlohmann_json_t.mixer);
        }
        nlohmann_json_j.at("override_time_setting").get_to(nlohmann_json_t.override_time_setting);
        if (k_version >= 88_ui)
        {
            nlohmann_json_j.at("override_voice_volume_loudness_normalization").get_to(nlohmann_json_t.override_voice_volume_loudness_normalization);
        }
        nlohmann_json_j.at("override_game_defined_auxiliary_send").get_to(nlohmann_json_t.override_game_defined_auxiliary_send);
        nlohmann_json_j.at("override_user_defined_auxiliary_send").get_to(nlohmann_json_t.override_user_defined_auxiliary_send);
        if (k_version >= 135_ui)
        {
            nlohmann_json_j.at("override_early_reflection_auxiliary_send").get_to(nlohmann_json_t.override_early_reflection_auxiliary_send);
        }
        nlohmann_json_j.at("override_effect").get_to(nlohmann_json_t.override_effect);
        if (k_version >= 140_ui)
        {
            nlohmann_json_j.at("override_metadata").get_to(nlohmann_json_t.override_metadata);
        }
        nlohmann_json_j.at("override_positioning").get_to(nlohmann_json_t.override_positioning);
        if (k_version >= 88_ui) 
        {
            nlohmann_json_j.at("override_hdr_envelope_tracking").get_to(nlohmann_json_t.override_hdr_envelope_tracking);
        }
        if (k_version >= 112_ui) {
            nlohmann_json_j.at("override_midi_target").get_to(nlohmann_json_t.override_midi_target);
            nlohmann_json_j.at("override_midi_clip_tempo").get_to(nlohmann_json_t.override_midi_clip_tempo);
        }
        nlohmann_json_j.at("override_playback_limit").get_to(nlohmann_json_t.override_playback_limit);
        nlohmann_json_j.at("override_virtual_voice").get_to(nlohmann_json_t.override_virtual_voice);
        nlohmann_json_j.at("override_playback_priority").get_to(nlohmann_json_t.override_playback_priority);
        if (k_version >= 112_ui && k_version < 150_ui)
        {
            nlohmann_json_j.at("override_mixer").get_to(nlohmann_json_t.override_mixer);
        }
        return;
    }

    /*
    //--------------------------

    struct EffectSine {
        float frequency;
        float gain;
        float duration;
        uint32_t channel;
    };

    inline auto to_json(
        nlohmann::ordered_json &nlohmann_json_j,
        const EffectSine &nlohmann_json_t) -> void
    {
        nlohmann_json_j["frequency"] = nlohmann_json_t.frequency;
        nlohmann_json_j["gain"] = nlohmann_json_t.gain;
        nlohmann_json_j["duration"] = nlohmann_json_t.duration;
        nlohmann_json_j["channel"] = nlohmann_json_t.channel;
        return;
    }

    inline auto from_json(
        const nlohmann::ordered_json &nlohmann_json_j,
        EffectSine &nlohmann_json_t) -> void
    {
    }

    //--------------------------

    struct EffectSlience {
        float duration;
        float randomized_length_minus;
        float randomized_length_plus;
    };

    inline auto to_json(
        nlohmann::ordered_json &nlohmann_json_j,
        const EffectSlience &nlohmann_json_t) -> void
    {
        nlohmann_json_j["duration"] = nlohmann_json_t.duration;
        nlohmann_json_j["randomized_length_minus"] = nlohmann_json_t.randomized_length_minus;
        nlohmann_json_j["randomized_length_plus"] = nlohmann_json_t.randomized_length_plus;
        return;
    }

    inline auto from_json(
        const nlohmann::ordered_json &nlohmann_json_j,
        EffectSlience &nlohmann_json_t) -> void
    {
    }

    //--------------------------

    enum class ToneGenSweep : uint8_t {
        lin,
        log
    };

    //--------------------------

    enum class ToneGenType : uint8_t {
        sine,
        triangle,
        square,
        saw_tooth,
        white_noise,
        pink_noise
    };

    //--------------------------

    enum class ToneGenMode : uint8_t {
        fix,
        environment,
    };

    //--------------------------

    struct ToneGenSetting {
        float gain;
        float start_frequency;
        float stop_frequency;
        uint8_t frequency_sweep;
        ToneGenSweep tone_gen_sweep;
        float stop_frequency_randomized_min;
        float stop_frequency_randomized_max;
        ToneGenType tone_gen_type;
        ToneGenMode tone_gen_mode;
        float fix_duration;
        float attack_duration;
        float decay_duration;
        float sustain_duration;
        float sustain_value;
        float release_duration;
        uint32_t channel;
    };

    inline auto to_json(
        nlohmann::ordered_json &nlohmann_json_j,
        const ToneGenSetting &nlohmann_json_t) -> void
    {
        nlohmann_json_j["gain"] = nlohmann_json_t.gain;
        nlohmann_json_j["start_frequency"] = nlohmann_json_t.start_frequency;
        nlohmann_json_j["stop_frequency"] = nlohmann_json_t.stop_frequency;
        nlohmann_json_j["frequency_sweep"] = nlohmann_json_t.frequency_sweep;
        nlohmann_json_j["tone_gen_sweep"] = magic_enum::enum_name(nlohmann_json_t.tone_gen_sweep);
        nlohmann_json_j["stop_frequency_randomized_min"] = nlohmann_json_t.stop_frequency_randomized_min;
        nlohmann_json_j["stop_frequency_randomized_max"] = nlohmann_json_t.stop_frequency_randomized_max;
        nlohmann_json_j["tone_gen_type"] = magic_enum::enum_name(nlohmann_json_t.tone_gen_type);
        nlohmann_json_j["tone_gen_mode"] = magic_enum::enum_name(nlohmann_json_t.tone_gen_mode);
        nlohmann_json_j["fix_duration"] = nlohmann_json_t.fix_duration;
        nlohmann_json_j["attack_duration"] = nlohmann_json_t.attack_duration;
        nlohmann_json_j["decay_duration"] = nlohmann_json_t.decay_duration;
        nlohmann_json_j["sustain_duration"] = nlohmann_json_t.sustain_duration;
        nlohmann_json_j["sustain_value"] = nlohmann_json_t.sustain_value;
        nlohmann_json_j["release_duration"] = nlohmann_json_t.release_duration;
        nlohmann_json_j["channel"] = nlohmann_json_t.channel;
        return;
    }

    inline auto from_json(
        const nlohmann::ordered_json &nlohmann_json_j,
        ToneGenSetting &nlohmann_json_t) -> void
    {
    }

    //--------------------------

    enum class ParameterEQFilterType : uint8_t {
        low_pass_filter,
        high_pass_filter,
        band_pass_filter,
        notch,
        low_shelf,
        high_shelf,
        peaking_eq
    };


    //--------------------------

    struct ParameterEQModule {
        ParameterEQFilterType filter_type;
        float gain;
        float frequency;
        float factor;
        bool enable;
    };

    inline auto to_json(
        nlohmann::ordered_json &nlohmann_json_j,
        const ParameterEQModule &nlohmann_json_t) -> void
    {
        nlohmann_json_j["filter_type"] = magic_enum::enum_name(nlohmann_json_t.filter_type);
        nlohmann_json_j["gain"] = nlohmann_json_t.gain;
        nlohmann_json_j["frequency"] = nlohmann_json_t.frequency;
        nlohmann_json_j["factor"] = nlohmann_json_t.factor;
        nlohmann_json_j["enable"] = nlohmann_json_t.enable;
        return;
    }

    inline auto from_json(
        const nlohmann::ordered_json &nlohmann_json_j,
        ParameterEQModule &nlohmann_json_t) -> void
    {
    }

    struct ParameterEQ {
        ParameterEQModule band;
        float output_level;
        bool low_pass_enable;
    };

    inline auto to_json(
        nlohmann::ordered_json &nlohmann_json_j,
        const ParameterEQ &nlohmann_json_t) -> void
    {
        nlohmann_json_j["band"] = nlohmann_json_t.band;
        nlohmann_json_j["output_level"] = nlohmann_json_t.output_level;
        nlohmann_json_j["low_pass_enable"] = nlohmann_json_t.low_pass_enable;
        return;
    }

    inline auto from_json(
        const nlohmann::ordered_json &nlohmann_json_j,
        ParameterEQ &nlohmann_json_t) -> void
    {
    }

    //--------------------------

    struct RealTimeParameterControlEffectSetting {
        float feedback;
        float wet_dry_mix;
        float output_level;
    };

    inline auto to_json(
        nlohmann::ordered_json &nlohmann_json_j,
        const RealTimeParameterControlEffectSetting &nlohmann_json_t) -> void
    {
        nlohmann_json_j["feedback"] = nlohmann_json_t.feedback;
        nlohmann_json_j["wet_dry_mix"] = nlohmann_json_t.wet_dry_mix;
        nlohmann_json_j["output_level"] = nlohmann_json_t.output_level;
        return;
    }

    inline auto from_json(
        const nlohmann::ordered_json &nlohmann_json_j,
        RealTimeParameterControlEffectSetting &nlohmann_json_t) -> void
    {
    }

    //--------------------------

    struct DelayEffect {
        float delay_time;
        RealTimeParameterControlEffectSetting effect;
        bool low_pass_enable;
    };

    inline auto to_json(
        nlohmann::ordered_json &nlohmann_json_j,
        const DelayEffect &nlohmann_json_t) -> void
    {
        nlohmann_json_j["delay_time"] = nlohmann_json_t.delay_time;
        nlohmann_json_j["effect"] = nlohmann_json_t.effect;
        nlohmann_json_j["low_pass_enable"] = nlohmann_json_t.low_pass_enable;
        return;
    }

    inline auto from_json(
        const nlohmann::ordered_json &nlohmann_json_j,
        DelayEffect &nlohmann_json_t) -> void
    {
    }
    */

    //--------------------------

    struct Hierarchy
    {
        uint32_t id;
        std::string name;
        HierarchyType type;
        StatefulPropertySetting stateful_property_setting;
        EventAction event_action;
        Event event;
        DialogueEvent dialogue_event;
        Attenuation attenuation;
        LowFrequencyOscillatorModulator low_frequency_oscillator_modulator;
        EnvelopeModulator envelope_modulator;
        TimeModulator time_modulator;
        Effect effect;
        Source source;
        AudioDevice audio_device;
        AudioBus audio_bus;
        AuxiliaryAudioBus auxiliary_audio_bus;
        Sound sound;
        SoundPlaylistContainer sound_playlist_container;
        SoundSwitchContainer sound_switch_container;
        SoundBlendContainer sound_blend_container;
        ActorMixer actor_mixer;
        MusicTrack music_track;
        MusicSegment music_segment;
        MusicPlaylistContainer music_playlist_container;
        MusicSwitchContainer music_switch_container;
    };

    inline auto to_json(
        nlohmann::ordered_json &nlohmann_json_j,
        const Hierarchy &nlohmann_json_t) -> void
    {
        nlohmann_json_j["id"] = nlohmann_json_t.id;
        if (!nlohmann_json_t.name.empty())
        {
            nlohmann_json_j["#name"] = nlohmann_json_t.name;
        }
        nlohmann_json_j["type"] = magic_enum::enum_name(nlohmann_json_t.type);
        switch (nlohmann_json_t.type)
        {
        case HierarchyType::stateful_property_setting:
        {
            nlohmann_json_j["value"] = nlohmann_json_t.stateful_property_setting.value;
            break;
        }
        case HierarchyType::event_action:
        {
            nlohmann_json_j["value"] = nlohmann_json_t.event_action;
            break;
        }
        case HierarchyType::event:
        {
            nlohmann_json_j["value"] = nlohmann_json_t.event;
            break;
        }
        case HierarchyType::dialogue_event:
        {
            nlohmann_json_j["value"] = nlohmann_json_t.dialogue_event;
            break;
        }
        case HierarchyType::attenuation:
        {
            nlohmann_json_j["value"] = nlohmann_json_t.attenuation;
            break;
        }
        case HierarchyType::low_frequency_oscillator_modulator:
        {
            nlohmann_json_j["value"] = nlohmann_json_t.low_frequency_oscillator_modulator;
            break;
        }
        case HierarchyType::envelope_modulator:
        {
            nlohmann_json_j["value"] = nlohmann_json_t.envelope_modulator;
            break;
        }
        case HierarchyType::time_modulator:
        {
            nlohmann_json_j["value"] = nlohmann_json_t.time_modulator;
            break;
        }
        case HierarchyType::effect:
        {
            nlohmann_json_j["value"] = nlohmann_json_t.effect;
            break;
        }
        case HierarchyType::source:
        {
            nlohmann_json_j["value"] = nlohmann_json_t.source;
            break;
        }
        case HierarchyType::audio_device:
        {
            nlohmann_json_j["value"] = nlohmann_json_t.audio_device;
            break;
        }
        case HierarchyType::audio_bus:
        {
            nlohmann_json_j["value"] = nlohmann_json_t.audio_bus;
            break;
        }
        case HierarchyType::auxiliary_audio_bus:
        {
            nlohmann_json_j["value"] = nlohmann_json_t.auxiliary_audio_bus;
            break;
        }
        case HierarchyType::sound:
        {
            nlohmann_json_j["value"] = nlohmann_json_t.sound;
            break;
        }
        case HierarchyType::sound_playlist_container:
        {
            nlohmann_json_j["value"] = nlohmann_json_t.sound_playlist_container;
            break;
        }
        case HierarchyType::sound_switch_container:
        {
            nlohmann_json_j["value"] = nlohmann_json_t.sound_switch_container;
            break;
        }
        case HierarchyType::sound_blend_container:
        {
            nlohmann_json_j["value"] = nlohmann_json_t.sound_blend_container;
            break;
        }
        case HierarchyType::actor_mixer:
        {
            nlohmann_json_j["value"] = nlohmann_json_t.actor_mixer;
            break;
        }
        case HierarchyType::music_track:
        {
            nlohmann_json_j["value"] = nlohmann_json_t.music_track;
            break;
        }
        case HierarchyType::music_segment:
        {
            nlohmann_json_j["value"] = nlohmann_json_t.music_segment;
            break;
        }
        case HierarchyType::music_playlist_container:
        {
            nlohmann_json_j["value"] = nlohmann_json_t.music_playlist_container;
            break;
        }
        case HierarchyType::music_switch_container:
        {
            nlohmann_json_j["value"] = nlohmann_json_t.music_switch_container;
            break;
        }
        default:
            break;
        }
        return;
    }

    inline auto from_json(
        const nlohmann::ordered_json &nlohmann_json_j,
        Hierarchy &nlohmann_json_t) -> void
    {
        nlohmann_json_j.at("id").get_to(nlohmann_json_t.id);
        nlohmann_json_t.type = magic_enum_cast<HierarchyType>(nlohmann_json_j.at("type").get<std::string>());
        switch (nlohmann_json_t.type)
        {
        case HierarchyType::stateful_property_setting:
        {
            nlohmann_json_j.at("value").get_to(nlohmann_json_t.stateful_property_setting.value);
            break;
        }
        case HierarchyType::event_action:
        {
            nlohmann_json_j.at("value").get_to(nlohmann_json_t.event_action);
            break;
        }
        case HierarchyType::event:
        {
            nlohmann_json_j.at("value").get_to(nlohmann_json_t.event);
            break;
        }
        case HierarchyType::dialogue_event:
        {
            nlohmann_json_j.at("value").get_to(nlohmann_json_t.dialogue_event);
            break;
        }
        case HierarchyType::attenuation:
        {
            nlohmann_json_j.at("value").get_to(nlohmann_json_t.attenuation);
            break;
        }
        case HierarchyType::low_frequency_oscillator_modulator:
        {
            nlohmann_json_j.at("value").get_to(nlohmann_json_t.low_frequency_oscillator_modulator);
            break;
        }
        case HierarchyType::envelope_modulator:
        {
            nlohmann_json_j.at("value").get_to(nlohmann_json_t.envelope_modulator);
            break;
        }
        case HierarchyType::time_modulator:
        {
            nlohmann_json_j.at("value").get_to(nlohmann_json_t.time_modulator);
            break;
        }
        case HierarchyType::effect:
        {
            nlohmann_json_j.at("value").get_to(nlohmann_json_t.effect);
            break;
        }
        case HierarchyType::source:
        {
            nlohmann_json_j.at("value").get_to(nlohmann_json_t.source);
            break;
        }
        case HierarchyType::audio_device:
        {
            nlohmann_json_j.at("value").get_to(nlohmann_json_t.audio_device);
            break;
        }
        case HierarchyType::audio_bus:
        {
            nlohmann_json_j.at("value").get_to(nlohmann_json_t.audio_bus);
            break;
        }
        case HierarchyType::auxiliary_audio_bus:
        {
            nlohmann_json_j.at("value").get_to(nlohmann_json_t.auxiliary_audio_bus);
            break;
        }
        case HierarchyType::sound:
        {
            nlohmann_json_j.at("value").get_to(nlohmann_json_t.sound);
            break;
        }
        case HierarchyType::sound_playlist_container:
        {
            nlohmann_json_j.at("value").get_to(nlohmann_json_t.sound_playlist_container);
            break;
        }
        case HierarchyType::sound_switch_container:
        {
            nlohmann_json_j.at("value").get_to(nlohmann_json_t.sound_switch_container);
            break;
        }
        case HierarchyType::sound_blend_container:
        {
            nlohmann_json_j.at("value").get_to(nlohmann_json_t.sound_blend_container);
            break;
        }
        case HierarchyType::actor_mixer:
        {
            nlohmann_json_j.at("value").get_to(nlohmann_json_t.actor_mixer);
            break;
        }
        case HierarchyType::music_track:
        {
            nlohmann_json_j.at("value").get_to(nlohmann_json_t.music_track);
            break;
        }
        case HierarchyType::music_segment:
        {
            nlohmann_json_j.at("value").get_to(nlohmann_json_t.music_segment);
            break;
        }
        case HierarchyType::music_playlist_container:
        {
            nlohmann_json_j.at("value").get_to(nlohmann_json_t.music_playlist_container);
            break;
        }
        case HierarchyType::music_switch_container:
        {
            nlohmann_json_j.at("value").get_to(nlohmann_json_t.music_switch_container);
            break;
        }
        default:
            break;
        }
        return;
    }

    //---------------------------------

    struct SoundBankReference
    {
        uint32_t id;
        std::string name;
    };

    inline auto to_json(
        nlohmann::ordered_json &nlohmann_json_j,
        const SoundBankReference &nlohmann_json_t) -> void
    {
        nlohmann_json_j["id"] = nlohmann_json_t.id;
        nlohmann_json_j["name"] = nlohmann_json_t.name;
        return;
    }

    inline auto from_json(
        const nlohmann::ordered_json &nlohmann_json_j,
        SoundBankReference &nlohmann_json_t) -> void
    {
        nlohmann_json_j.at("id").get_to(nlohmann_json_t.id);
        nlohmann_json_j.at("name").get_to(nlohmann_json_t.name);
        return;
    }

    //---------------------------------

    struct PlugInReference
    {
        uint32_t id;
        std::string library;
    };

    inline auto to_json(
        nlohmann::ordered_json &nlohmann_json_j,
        const PlugInReference &nlohmann_json_t) -> void
    {
        if (k_version >= 118_ui)
        {
            nlohmann_json_j["id"] = nlohmann_json_t.id;
            nlohmann_json_j["library"] = nlohmann_json_t.library;
        }
        return;
    }

    inline auto from_json(
        const nlohmann::ordered_json &nlohmann_json_j,
        PlugInReference &nlohmann_json_t) -> void
    {
        if (k_version >= 118_ui)
        {
            nlohmann_json_j.at("id").get_to(nlohmann_json_t.id);
            nlohmann_json_j.at("library").get_to(nlohmann_json_t.library);
        }
        return;
    }

    //--------------------------------

    enum class VoiceFilterBehavior : uint8_t
    {
        sum_all_value,
        use_highest_value
    };

    //--------------------------------

    struct ObstructionSetting
    {
        bool enable;
        CoordinateMode mode;
        std::vector<CoordinatePoint> point;
    };

    inline auto to_json(
        nlohmann::ordered_json &nlohmann_json_j,
        const ObstructionSetting &nlohmann_json_t) -> void
    {
        nlohmann_json_j["enable"] = nlohmann_json_t.enable;
        nlohmann_json_j["mode"] = magic_enum::enum_name(nlohmann_json_t.mode);
        nlohmann_json_j["point"] = nlohmann_json_t.point;
        return;
    }

    inline auto from_json(
        const nlohmann::ordered_json &nlohmann_json_j,
        ObstructionSetting &nlohmann_json_t) -> void
    {
        nlohmann_json_j.at("enable").get_to(nlohmann_json_t.enable);
        nlohmann_json_t.mode = magic_enum_cast<CoordinateMode>(nlohmann_json_j.at("mode").get<std::string>());
        nlohmann_json_j.at("point").get_to(nlohmann_json_t.point);
        return;
    }

    //--------------------------------

    struct ObstructionSettingBundle
    {
        ObstructionSetting volume;
        ObstructionSetting low_pass_filter;
        ObstructionSetting high_pass_filter;
    };

    inline auto to_json(
        nlohmann::ordered_json &nlohmann_json_j,
        const ObstructionSettingBundle &nlohmann_json_t) -> void
    {
        nlohmann_json_j["volume"] = nlohmann_json_t.volume;
        nlohmann_json_j["low_pass_filter"] = nlohmann_json_t.low_pass_filter;
        if (k_version >= 112_ui)
        {
            nlohmann_json_j["high_pass_filter"] = nlohmann_json_t.high_pass_filter;
        }
        return;
    }

    inline auto from_json(
        const nlohmann::ordered_json &nlohmann_json_j,
        ObstructionSettingBundle &nlohmann_json_t) -> void
    {
        nlohmann_json_j.at("volume").get_to(nlohmann_json_t.volume);
        nlohmann_json_j.at("low_pass_filter").get_to(nlohmann_json_t.low_pass_filter);
        if (k_version >= 112_ui)
        {
            nlohmann_json_j.at("high_pass_filter").get_to(nlohmann_json_t.high_pass_filter);
        }
        return;
    }

    //--------------------------------

    struct Setting
    {
        std::string platform;
        VoiceFilterBehavior voice_filter_behavior;
        float volume_threshold;
        uint16_t maximum_voice_instance; //TODO.
        ObstructionSettingBundle obstruction;
        ObstructionSettingBundle occlusion;
        std::vector<PlugInReference> plug_in;
    };

    inline auto to_json(
        nlohmann::ordered_json &nlohmann_json_j,
        const Setting &nlohmann_json_t) -> void
    {
        if (k_version >= 113_ui)
        {
            nlohmann_json_j["platform"] = nlohmann_json_t.platform;
        }
        if (k_version >= 145_ui)
        {
            nlohmann_json_j["voice_filter_behavior"] = nlohmann_json_t.voice_filter_behavior;
        }
        nlohmann_json_j["volume_threshold"] = nlohmann_json_t.volume_threshold;
        nlohmann_json_j["maximum_voice_instance"] = nlohmann_json_t.maximum_voice_instance;
        nlohmann_json_j["obstruction"] = nlohmann_json_t.obstruction;
        nlohmann_json_j["occlusion"] = nlohmann_json_t.occlusion;
        if (k_version >= 118_ui)
        {
            nlohmann_json_j["plug_in"] = nlohmann_json_t.plug_in;
        }
        return;
    }

    inline auto from_json(
        const nlohmann::ordered_json &nlohmann_json_j,
        Setting &nlohmann_json_t) -> void
    {
        if (k_version >= 113_ui)
        {
            nlohmann_json_j.at("platform").get_to(nlohmann_json_t.platform);
        }
        if (k_version >= 145_ui)
        {
            nlohmann_json_j.at("voice_filter_behavior").get_to(nlohmann_json_t.voice_filter_behavior);
        }
        nlohmann_json_j.at("volume_threshold").get_to(nlohmann_json_t.volume_threshold);
        nlohmann_json_j.at("maximum_voice_instance").get_to(nlohmann_json_t.maximum_voice_instance);
        nlohmann_json_j.at("obstruction").get_to(nlohmann_json_t.obstruction);
        nlohmann_json_j.at("occlusion").get_to(nlohmann_json_t.occlusion);
        if (k_version >= 118_ui)
        {
            nlohmann_json_j.at("plug_in").get_to(nlohmann_json_t.plug_in);
        }
        return;
    }

    //--------------------------------

    struct StateGroupCustomTransition
    {
        uint32_t from;
        uint32_t to;
        uint32_t time;
    };

    inline auto to_json(
        nlohmann::ordered_json &nlohmann_json_j,
        const StateGroupCustomTransition &nlohmann_json_t) -> void
    {
        nlohmann_json_j["from"] = nlohmann_json_t.from;
        nlohmann_json_j["to"] = nlohmann_json_t.to;
        nlohmann_json_j["time"] = nlohmann_json_t.time;
        return;
    }

    inline auto from_json(
        const nlohmann::ordered_json &nlohmann_json_j,
        StateGroupCustomTransition &nlohmann_json_t) -> void
    {
        nlohmann_json_j.at("from").get_to(nlohmann_json_t.from);
        nlohmann_json_j.at("to").get_to(nlohmann_json_t.to);
        nlohmann_json_j.at("time").get_to(nlohmann_json_t.time);
        return;
    }

    //--------------------------------

    struct StateGroup
    {
        uint32_t id;
        uint32_t default_transition;
        std::vector<StateGroupCustomTransition> custom_transition;
    };

    inline auto to_json(
        nlohmann::ordered_json &nlohmann_json_j,
        const StateGroup &nlohmann_json_t) -> void
    {
        nlohmann_json_j["id"] = nlohmann_json_t.id;
        nlohmann_json_j["default_transition"] = nlohmann_json_t.default_transition;
        nlohmann_json_j["custom_transition"] = nlohmann_json_t.custom_transition;
        return;
    }

    inline auto from_json(
        const nlohmann::ordered_json &nlohmann_json_j,
        StateGroup &nlohmann_json_t) -> void
    {
        nlohmann_json_j.at("id").get_to(nlohmann_json_t.id);
        nlohmann_json_j.at("default_transition").get_to(nlohmann_json_t.default_transition);
        nlohmann_json_j.at("custom_transition").get_to(nlohmann_json_t.custom_transition);
        return;
    }

    //--------------------------------

    struct SwitchGroup
    {
        uint32_t id;
        Parameter parameter;
        std::vector<CoordinateIDPoint> point;
    };

    inline auto to_json(
        nlohmann::ordered_json &nlohmann_json_j,
        const SwitchGroup &nlohmann_json_t) -> void
    {
        nlohmann_json_j["id"] = nlohmann_json_t.id;
        nlohmann_json_j["parameter"] = nlohmann_json_t.parameter;
        nlohmann_json_j["point"] = nlohmann_json_t.point;
        return;
    }

    inline auto from_json(
        const nlohmann::ordered_json &nlohmann_json_j,
        SwitchGroup &nlohmann_json_t) -> void
    {
        nlohmann_json_j.at("id").get_to(nlohmann_json_t.id);
        nlohmann_json_j.at("parameter").get_to(nlohmann_json_t.parameter);
        nlohmann_json_j.at("point").get_to(nlohmann_json_t.point);
        return;
    }

    //--------------------------------

    struct GameParameter
    {
        uint32_t id;
        float range_default;
        GameParameterInterpolationMode interpolation_mode;
        float interpolation_attack;
        float interpolation_release;
        GameParameterBindToBuiltInParameterMode bind_to_built_in_parameter;
    };

    inline auto to_json(
        nlohmann::ordered_json &nlohmann_json_j,
        const GameParameter &nlohmann_json_t) -> void
    {
        
            nlohmann_json_j["id"] = nlohmann_json_t.id;
            nlohmann_json_j["range_default"] = nlohmann_json_t.range_default;
        if (k_version >= 112_ui)
        {
            nlohmann_json_j["interpolation_mode"] = magic_enum::enum_name(nlohmann_json_t.interpolation_mode);
            nlohmann_json_j["interpolation_attack"] = nlohmann_json_t.interpolation_attack;
            nlohmann_json_j["interpolation_release"] = nlohmann_json_t.interpolation_release;
            nlohmann_json_j["bind_to_built_in_parameter"] = magic_enum::enum_name(nlohmann_json_t.bind_to_built_in_parameter);
        }
        return;
    }

    inline auto from_json(
        const nlohmann::ordered_json &nlohmann_json_j,
        GameParameter &nlohmann_json_t) -> void
    {
        
            nlohmann_json_j.at("id").get_to(nlohmann_json_t.id);
            nlohmann_json_j.at("range_default").get_to(nlohmann_json_t.range_default);
        if (k_version >= 112_ui)
        {
            nlohmann_json_t.interpolation_mode = magic_enum_cast<GameParameterInterpolationMode>(nlohmann_json_j.at("interpolation_mode").get<std::string>());
            nlohmann_json_j.at("interpolation_attack").get_to(nlohmann_json_t.interpolation_attack);
            nlohmann_json_j.at("interpolation_release").get_to(nlohmann_json_t.interpolation_release);
            nlohmann_json_t.bind_to_built_in_parameter = magic_enum_cast<GameParameterBindToBuiltInParameterMode>(nlohmann_json_j.at("bind_to_built_in_parameter").get<std::string>());
        }
        return;
    }

    //--------------------------------

    struct GameSynchronizationU1
    {
        uint32_t id;
        float u1;
        float u2;
        float u3;
        float u4;
        float u5;
        float u6;
    };

    inline auto to_json(
        nlohmann::ordered_json &nlohmann_json_j,
        const GameSynchronizationU1 &nlohmann_json_t) -> void
    {
        if (k_version >= 140_ui)
        {
            nlohmann_json_j["id"] = nlohmann_json_t.id;
            nlohmann_json_j["u1"] = nlohmann_json_t.u1;
            nlohmann_json_j["u2"] = nlohmann_json_t.u2;
            nlohmann_json_j["u3"] = nlohmann_json_t.u3;
            nlohmann_json_j["u4"] = nlohmann_json_t.u4;
            nlohmann_json_j["u5"] = nlohmann_json_t.u5;
            nlohmann_json_j["u6"] = nlohmann_json_t.u6;
        }
        return;
    }

    inline auto from_json(
        const nlohmann::ordered_json &nlohmann_json_j,
        GameSynchronizationU1 &nlohmann_json_t) -> void
    {
        if (k_version >= 140_ui)
        {
            nlohmann_json_j.at("u1").get_to(nlohmann_json_t.u1);
            nlohmann_json_j.at("u2").get_to(nlohmann_json_t.u2);
            nlohmann_json_j.at("u3").get_to(nlohmann_json_t.u3);
            nlohmann_json_j.at("u4").get_to(nlohmann_json_t.u4);
            nlohmann_json_j.at("u5").get_to(nlohmann_json_t.u5);
            nlohmann_json_j.at("u6").get_to(nlohmann_json_t.u6);
        }
        return;
    }

    //--------------------------------

    struct GameSynchronization
    {
        std::vector<StateGroup> state_group;
        std::vector<SwitchGroup> switch_group;
        std::vector<GameParameter> game_parameter;
        std::vector<GameSynchronizationU1> u1;
    };

    inline auto to_json(
        nlohmann::ordered_json &nlohmann_json_j,
        const GameSynchronization &nlohmann_json_t) -> void
    {
        nlohmann_json_j["state_group"] = nlohmann_json_t.state_group;
        nlohmann_json_j["switch_group"] = nlohmann_json_t.switch_group;
        nlohmann_json_j["game_parameter"] = nlohmann_json_t.game_parameter;
        if (k_version >= 140_ui)
        {
            nlohmann_json_j["u1"] = nlohmann_json_t.u1;
        }
        return;
    }

    inline auto from_json(
        const nlohmann::ordered_json &nlohmann_json_j,
        GameSynchronization &nlohmann_json_t) -> void
    {
        nlohmann_json_j.at("state_group").get_to(nlohmann_json_t.state_group);
        nlohmann_json_j.at("switch_group").get_to(nlohmann_json_t.switch_group);
        nlohmann_json_j.at("game_parameter").get_to(nlohmann_json_t.game_parameter);
        if (k_version >= 140_ui)
        {
            nlohmann_json_j.at("u1").get_to(nlohmann_json_t.u1);
        }
        return;
    }

    //--------------------------------

    struct SoundBankInformation
    {
        BankHeader bank_header;
        std::vector<uint32_t> media;
        std::vector<SoundBankReference> reference;
        Setting setting;
        GameSynchronization game_synchronization;
        std::vector<Hierarchy> hierarchy;
        bool has_media = false;
        bool has_reference = false;
        bool has_setting = false;
        bool has_game_synchronization = false;
        bool has_hierarchy = false;
    };

    inline auto to_json(
        nlohmann::ordered_json &nlohmann_json_j,
        const SoundBankInformation &nlohmann_json_t) -> void
    {
        nlohmann_json_j["bank_header"] = nlohmann_json_t.bank_header;
#define sound_bank_has_or(check, _property, __property, or_value) \
    if (nlohmann_json_t.check)                                    \
    {                                                             \
        nlohmann_json_j[_property] = nlohmann_json_t.__property;  \
    }                                                             \
    else                                                          \
    {                                                             \
        nlohmann_json_j[_property] = or_value;                    \
    }
        sound_bank_has_or(has_media, "media", media, nullptr);
        sound_bank_has_or(has_reference, "reference", reference, nullptr);
        sound_bank_has_or(has_setting, "setting", setting, nullptr);
        sound_bank_has_or(has_game_synchronization, "game_synchronization", game_synchronization, nullptr);
        sound_bank_has_or(has_hierarchy, "hierarchy", hierarchy, nullptr);
        return;
    }

    inline auto from_json(
        const nlohmann::ordered_json &nlohmann_json_j,
        SoundBankInformation &nlohmann_json_t) -> void
    {
        nlohmann_json_j.at("bank_header").get_to(nlohmann_json_t.bank_header);
        if (nlohmann_json_j["media"].is_null())
        {
            nlohmann_json_t.has_media = false;
        }
        else
        {
            nlohmann_json_t.has_media = true;
            nlohmann_json_j.at("media").get_to(nlohmann_json_t.media);
        }
        if (nlohmann_json_j["reference"].is_null())
        {
            nlohmann_json_t.has_reference = false;
        }
        else
        {
            nlohmann_json_t.has_reference = true;
            nlohmann_json_j.at("reference").get_to(nlohmann_json_t.reference);
        }
        if (nlohmann_json_j["game_synchronization"].is_null())
        {
            nlohmann_json_t.has_game_synchronization = false;
        }
        else
        {
            nlohmann_json_t.has_game_synchronization = true;
            nlohmann_json_j.at("game_synchronization").get_to(nlohmann_json_t.game_synchronization);
        }
        if (nlohmann_json_j["setting"].is_null())
        {
            nlohmann_json_t.has_setting = false;
        }
        else
        {
            nlohmann_json_t.has_setting = true;
            nlohmann_json_j.at("setting").get_to(nlohmann_json_t.setting);
        }
        if (nlohmann_json_j["hierarchy"].is_null())
        {
            nlohmann_json_t.has_hierarchy = false;
        }
        else
        {
            nlohmann_json_t.has_hierarchy = true;
            nlohmann_json_j.at("hierarchy").get_to(nlohmann_json_t.hierarchy);
        }
        return;
    }
}