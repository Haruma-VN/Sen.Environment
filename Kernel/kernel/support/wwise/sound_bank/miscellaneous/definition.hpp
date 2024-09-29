#pragma once

#include "kernel/definition/utility.hpp"
#include "kernel/support/miscellaneous/shared.hpp"

namespace Sen::Kernel::Support::WWise::SoundBank::Miscellaneous
{
    using namespace Sen::Kernel::Support::Miscellaneous::Shared;
    enum class AudioFormat {
        pcm,
        adpcm,
        vorbis,
        wemopus
    };

    enum class MusicType {
        main_path 
    };

    struct MusicSetting {
        int bank_version;
        AudioFormat audio_format;
    };

    inline auto to_json(
        nlohmann::ordered_json &nlohmann_json_j,
        const MusicSetting &nlohmann_json_t) -> void
    {
        nlohmann_json_j["bank_version"] = nlohmann_json_t.bank_version;
        nlohmann_json_j["audio_format"] = magic_enum::enum_name(nlohmann_json_t.audio_format);
        return;
    }

    inline auto from_json(
        const nlohmann::ordered_json &nlohmann_json_j,
        MusicSetting &nlohmann_json_t) -> void
    {
        nlohmann_json_j.at("bank_version").get_to(nlohmann_json_t.bank_version);
        nlohmann_json_t.audio_format = magic_enum_cast<AudioFormat>(nlohmann_json_j.at("audio_format").get<std::string>());
        return;
    }

    struct MediaSource {
        int target_index;
        double volume;
        double duration;
        int loop;
    };

    inline auto to_json(
        nlohmann::ordered_json &nlohmann_json_j,
        const MediaSource &nlohmann_json_t) -> void
    {
        nlohmann_json_j["target_index"] = nlohmann_json_t.target_index;
        nlohmann_json_j["volume"] = nlohmann_json_t.volume;
        nlohmann_json_j["duration"] = nlohmann_json_t.duration;
        nlohmann_json_j["loop"] = nlohmann_json_t.loop;
        return;
    }

    inline auto from_json(
        const nlohmann::ordered_json &nlohmann_json_j,
        MediaSource &nlohmann_json_t) -> void
    {
        nlohmann_json_j.at("target_index").get_to(nlohmann_json_t.target_index);
        nlohmann_json_j.at("volume").get_to(nlohmann_json_t.volume);
        nlohmann_json_j.at("duration").get_to(nlohmann_json_t.duration);
        nlohmann_json_j.at("loop").get_to(nlohmann_json_t.loop);
        return;
    }

    struct MusicChild {
        std::string switches_name;
        std::vector<MediaSource> child;
    };

    inline auto to_json(
        nlohmann::ordered_json &nlohmann_json_j,
        const MusicChild &nlohmann_json_t) -> void
    {
        nlohmann_json_j["switches_name"] = nlohmann_json_t.switches_name;
        nlohmann_json_j["child"] = nlohmann_json_t.child;
        return;
    }

    inline auto from_json(
        const nlohmann::ordered_json &nlohmann_json_j,
        MusicChild &nlohmann_json_t) -> void
    {
        nlohmann_json_j.at("switches_name").get_to(nlohmann_json_t.switches_name);
        nlohmann_json_j.at("child").get_to(nlohmann_json_t.child);
        return;
    }

    struct MusicInformation {
        std::string audio_name;
        MusicType type;
        std::vector<MusicChild> child;
    };

    inline auto to_json(
        nlohmann::ordered_json &nlohmann_json_j,
        const MusicInformation &nlohmann_json_t) -> void
    {
        nlohmann_json_j["audio_name"] = nlohmann_json_t.audio_name;
        nlohmann_json_j["type"] = magic_enum::enum_name(nlohmann_json_t.type);
        nlohmann_json_j["child"] = nlohmann_json_t.child;
        return;
    }

    inline auto from_json(
        const nlohmann::ordered_json &nlohmann_json_j,
        MusicInformation &nlohmann_json_t) -> void
    {
        nlohmann_json_j.at("audio_name").get_to(nlohmann_json_t.audio_name);
        nlohmann_json_t.type = magic_enum_cast<MusicType>(nlohmann_json_j.at("type").get<std::string>());
        nlohmann_json_j.at("child").get_to(nlohmann_json_t.child);
        return;
    }

    struct InjectionMusicStruct
    {
        MusicSetting setting;
        std::vector<std::string> media;
        std::vector<MusicInformation> music;
    };

    inline auto to_json(
        nlohmann::ordered_json &nlohmann_json_j,
        const InjectionMusicStruct &nlohmann_json_t) -> void
    {
        nlohmann_json_j["setting"] = nlohmann_json_t.setting;
        nlohmann_json_j["media"] = nlohmann_json_t.media;
        nlohmann_json_j["music"] = nlohmann_json_t.music;
        return;
    }

    inline auto from_json(
        const nlohmann::ordered_json &nlohmann_json_j,
        InjectionMusicStruct &nlohmann_json_t) -> void
    {
        nlohmann_json_j.at("setting").get_to(nlohmann_json_t.setting);
        nlohmann_json_j.at("media").get_to(nlohmann_json_t.media);
        nlohmann_json_j.at("music").get_to(nlohmann_json_t.music);
        return;
    }


    //----------------------------

    struct SoundBankGroup {
        std::string name;
        std::vector<std::string> child;
    };

    inline auto to_json(
        nlohmann::ordered_json &nlohmann_json_j,
        const SoundBankGroup &nlohmann_json_t) -> void
    {
        nlohmann_json_j["name"] = nlohmann_json_t.name;
        nlohmann_json_j["child"] = nlohmann_json_t.child;
        return;
    }

    inline auto from_json(
        const nlohmann::ordered_json &nlohmann_json_j,
        SoundBankGroup &nlohmann_json_t) -> void
    {
        nlohmann_json_j.at("name").get_to(nlohmann_json_t.name);
        nlohmann_json_j.at("child").get_to(nlohmann_json_t.child);
        return;
    }

    struct Position {
        std::vector<int> group_index;
        std::vector<int> child_index;
    };

    inline auto to_json(
        nlohmann::ordered_json &nlohmann_json_j,
        const Position &nlohmann_json_t) -> void
    {
        nlohmann_json_j["group_index"] = nlohmann_json_t.group_index;
        nlohmann_json_j["child_index"] = nlohmann_json_t.child_index;
        return;
    }

    inline auto from_json(
        const nlohmann::ordered_json &nlohmann_json_j,
        Position &nlohmann_json_t) -> void
    {
        nlohmann_json_j.at("group_index").get_to(nlohmann_json_t.group_index);
        nlohmann_json_j.at("child_index").get_to(nlohmann_json_t.child_index);
        return;
    }

    struct PostitionSource {
        bool is_state;
        AudioSourceType media_type;
        std::vector<Position> position;
    };

    inline auto to_json(
        nlohmann::ordered_json &nlohmann_json_j,
        const PostitionSource &nlohmann_json_t) -> void
    {
        nlohmann_json_j["is_state"] = nlohmann_json_t.is_state;
        nlohmann_json_j["media_type"] = magic_enum::enum_name(nlohmann_json_t.media_type);
        nlohmann_json_j["position"] = nlohmann_json_t.position;
        return;
    }

    inline auto from_json(
        const nlohmann::ordered_json &nlohmann_json_j,
        PostitionSource &nlohmann_json_t) -> void
    {
        nlohmann_json_j.at("is_state").get_to(nlohmann_json_t.is_state);
        nlohmann_json_t.media_type = magic_enum_cast<AudioSourceType>(nlohmann_json_j.at("media_type").get<std::string>());
        nlohmann_json_j.at("position").get_to(nlohmann_json_t.position);
        return;
    }

    struct InteractiveMusic {
        PostitionSource position_source;
        std::vector<MediaSource> media_source;
    };

    inline auto to_json(
        nlohmann::ordered_json &nlohmann_json_j,
        const InteractiveMusic &nlohmann_json_t) -> void
    {
        nlohmann_json_j["position_source"] = nlohmann_json_t.position_source;
        nlohmann_json_j["media_source"] = nlohmann_json_t.media_source;
        return;
    }

    inline auto from_json(
        const nlohmann::ordered_json &nlohmann_json_j,
        InteractiveMusic &nlohmann_json_t) -> void
    {
        nlohmann_json_j.at("position_source").get_to(nlohmann_json_t.position_source);
        nlohmann_json_j.at("media_source").get_to(nlohmann_json_t.media_source);
        return;
    }

    struct SoundBankEvent {
        std::string name;
        EventActionProperty::Type type;
        std::vector<InteractiveMusic> interactive_music;
    };

    inline auto to_json(
        nlohmann::ordered_json &nlohmann_json_j,
        const SoundBankEvent &nlohmann_json_t) -> void
    {
        nlohmann_json_j["name"] = nlohmann_json_t.name;
        nlohmann_json_j["type"] = magic_enum::enum_name(nlohmann_json_t.type);
        nlohmann_json_j["interactive_music"] = nlohmann_json_t.interactive_music;
        return;
    }

    inline auto from_json(
        const nlohmann::ordered_json &nlohmann_json_j,
        SoundBankEvent &nlohmann_json_t) -> void
    {
        nlohmann_json_j.at("name").get_to(nlohmann_json_t.name);
        nlohmann_json_t.type = magic_enum_cast<EventActionProperty::Type>(nlohmann_json_j.at("type").get<std::string>());
        nlohmann_json_j.at("interactive_music").get_to(nlohmann_json_t.interactive_music);
        return;
    }

    struct SoundBankCreateInfo {
        std::string name;
        std::vector<SoundBankEvent> events;
    };

    inline auto to_json(
        nlohmann::ordered_json &nlohmann_json_j,
        const SoundBankCreateInfo &nlohmann_json_t) -> void
    {
        nlohmann_json_j["name"] = nlohmann_json_t.name;
        nlohmann_json_j["events"] = nlohmann_json_t.events;
        return;
    }

    inline auto from_json(
        const nlohmann::ordered_json &nlohmann_json_j,
        SoundBankCreateInfo &nlohmann_json_t) -> void
    {
        nlohmann_json_j.at("name").get_to(nlohmann_json_t.name);
        nlohmann_json_j.at("events").get_to(nlohmann_json_t.events);
        return;
    }

    struct SoundBankCreateStruct {
        MusicSetting setting;
        std::vector<std::string> media;
        std::vector<SoundBankGroup> switches;
        std::vector<SoundBankGroup> states;
        std::vector<SoundBankCreateInfo> soundbank;
    };

    inline auto to_json(
        nlohmann::ordered_json &nlohmann_json_j,
        const SoundBankCreateStruct &nlohmann_json_t) -> void
    {
        nlohmann_json_j["setting"] = nlohmann_json_t.setting;
        nlohmann_json_j["media"] = nlohmann_json_t.media;
        nlohmann_json_j["switches"] = nlohmann_json_t.switches;
        nlohmann_json_j["states"] = nlohmann_json_t.states;
        nlohmann_json_j["soundbank"] = nlohmann_json_t.soundbank;
        return;
    }

    inline auto from_json(
        const nlohmann::ordered_json &nlohmann_json_j,
        SoundBankCreateStruct &nlohmann_json_t) -> void
    {
        nlohmann_json_j.at("setting").get_to(nlohmann_json_t.setting);
        nlohmann_json_j.at("media").get_to(nlohmann_json_t.media);
        debug("switches");
        nlohmann_json_j.at("switches").get_to(nlohmann_json_t.switches);
        debug("states");
        nlohmann_json_j.at("states").get_to(nlohmann_json_t.states);
        debug("soundbank");
        nlohmann_json_j.at("soundbank").get_to(nlohmann_json_t.soundbank);
        return;
    }
}