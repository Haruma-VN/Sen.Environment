#pragma once

#include "kernel/definition/utility.hpp"
#include "kernel/support/wwise/sound_bank/definition.hpp"
#include "kernel/support/wwise/sound_bank/miscellaneous/definition.hpp"
#include "kernel/support/miscellaneous/shared.hpp"

namespace Sen::Kernel::Support::WWise::SoundBank::Miscellaneous
{
    using namespace Sen::Kernel::Definition;

    using namespace Sen::Kernel::Support::Miscellaneous::Shared;

    struct Support
    {
    public:
        //TODO.

        inline static auto add_music(
            std::string_view source,
            std::string_view global_data_source,
            std::string_view media_source,
            std::vector<uint32_t> const &media_list) -> void
        {
            auto play_music_world_id = Sen::Kernel::Support::WWise::SoundBank::Common::fnv_hash("play_music_world");
            InjectionMusicStruct definition = *FileSystem::read_json(fmt::format("{}/data.json", source));
            SoundBankInformation soundbank = *FileSystem::read_json(fmt::format("{}/data.json", global_data_source));

            auto event_exist = std::unordered_map<uint32_t, bool>{
                std::pair(923409649, false),
                std::pair(892548578, false),
                std::pair(386715391, false),
                std::pair(548737750, false),
                std::pair(440986615, false),
                std::pair(231338071, false),
                std::pair(1045708624, false)};
            auto has_play_music_world = false;
            for (auto &hierarchy : soundbank.hierarchy)
            {
                if (hierarchy.id == play_music_world_id)
                {

                    for (auto &[child_event, exist] : event_exist)
                    {
                        if (std::find(hierarchy.event.child.begin(), hierarchy.event.child.end(), child_event) == hierarchy.event.child.end())
                        {
                            hierarchy.event.child.emplace_back(child_event);
                        }
                        else
                        {
                            event_exist[child_event] = true;
                        }
                    }
                    has_play_music_world = true;
                    break;
                }
            }
            assert_conditional(has_play_music_world, fmt::format("{}", Language::get("wwise.sound_bank.global_data_has_no_play_music_world_event")), "add_music");
            for (auto &[child_event, exist] : event_exist)
            {
                auto add_hierarchy = Hierarchy{};
                switch (child_event)
                {
                case 923409649:
                {
                    if (!exist)
                    {
                        add_hierarchy.type = HierarchyType::event_action;
                        auto &event_action = add_hierarchy.event_action;
                        event_action.type = 1_byte;
                        event_action.target = 840318675_ui;
                        event_action.stop_audio.resume_state_transition = true;
                        event_action.stop_audio.apply_to_dynamic_sequence = true;
                        soundbank.hierarchy.emplace_back(add_hierarchy);
                    }
                    break;
                }
                case 892548578:
                {
                    if (!exist)
                    {
                        add_hierarchy.type = HierarchyType::event_action;
                        auto &event_action = add_hierarchy.event_action;
                        event_action.type = 1_byte;
                        event_action.target = 298153706_ui;
                        event_action.stop_audio.resume_state_transition = true;
                        event_action.stop_audio.apply_to_dynamic_sequence = true;
                        soundbank.hierarchy.emplace_back(add_hierarchy);
                    }
                    break;
                }
                case 386715391:
                {
                    if (!exist)
                    {
                        add_hierarchy.type = HierarchyType::event_action;
                        auto &event_action = add_hierarchy.event_action;
                        event_action.type = 4_byte;
                        event_action.target = 840318675_ui;
                        // TODO.
                        soundbank.hierarchy.emplace_back(add_hierarchy);
                    }
                    break;
                }
                case 548737750:
                {
                }
                case 440986615:
                {
                    if (!exist)
                    {
                        add_hierarchy.type = HierarchyType::event_action;
                        add_hierarchy.event_action.type = 7_byte;
                        soundbank.hierarchy.emplace_back(add_hierarchy);
                    }
                    break;
                }
                case 231338071:
                {
                    if (!exist)
                    {
                        add_hierarchy.type = HierarchyType::event_action;
                        add_hierarchy.event_action.type = 13_byte;
                        soundbank.hierarchy.emplace_back(add_hierarchy);
                    }
                    break;
                }
                case 1045708624:
                {
                    if (!exist)
                    {
                        add_hierarchy.type = HierarchyType::event_action;
                        add_hierarchy.event_action.type = 11_byte;
                        soundbank.hierarchy.emplace_back(add_hierarchy);
                    }
                    break;
                }
                default:
                {
                    assert_conditional(false, String::format(fmt::format("{}", Language::get("wwise.sound_bank.invalid_event")), std::to_string(child_event)), "add_music");
                    break;
                }
                }
            }

            for (auto &music : definition.music)
            {
                auto audio_name_id = Sen::Kernel::Support::WWise::SoundBank::Common::fnv_hash(music.audio_name);
            }
            return;
        }

        /**
         *  This support to create soundbank.
         */

        inline static auto create_soundbank(
            std::string_view source,
            std::string_view destination) -> void
        {
            struct GroupBank
            {
                std::vector<Hierarchy> switches;
                std::vector<Hierarchy> states;
            } group_bank;
            SoundBankCreateStruct definition = *FileSystem::read_json(fmt::format("{}/data.json", source));
            auto exchange_group = [](SoundBankGroup const &data, std::vector<Hierarchy> &value)
            {
                auto hierarchy = Hierarchy{};
            };
            //debug(definition.switches.size());
            group_bank.switches.resize(definition.switches.size());
            group_bank.states.resize(definition.switches.size());
            /*
            for (auto &switch_info :  definition.switches) {
                exchange_group(switch_info, group_bank.switches);
            }
            for (auto &stage_info :  definition.states) {
                exchange_group(stage_info, group_bank.states);
            }
            */
            auto media_encode_id = definition.setting.audio_format == AudioFormat::vorbis ? 65537_ui : 0; // TODO.
            auto media_plug_in = definition.setting.audio_format == AudioFormat::vorbis ? 262145_ui : 0;  // TODO.
            for (auto &soundbank : definition.soundbank)
            {
                auto stream = DataStreamView{};
                auto soundbank_information = SoundBankInformation{};
                auto sounbank_id = Common::fnv_hash(soundbank.name);
                soundbank_information.bank_header.version = definition.setting.bank_version;
                soundbank_information.bank_header.soundbank_id = sounbank_id;
                soundbank_information.bank_header.language_id = Common::fnv_hash("English");
                soundbank_information.bank_header.head_info = HeadValue{
                    .values = 16,
                    .project_id = 15202,
                    .padding = 4};
                soundbank_information.has_hierarchy = true;
                auto sounbank_event_list = std::vector<Hierarchy>{};
                for (auto &soundbank_event : soundbank.events)
                {
                    assert_conditional(soundbank_event.type == EventActionProperty::Type::play_audio, fmt::format("{}", Language::get("wwise.soundbank.encode.only_support_play_audio_action_event")), "create_soundbank");
                    auto event_id = Common::fnv_hash(soundbank_event.name);
                    auto event_hierarchy = Hierarchy{
                        .id = event_id,
                        .type = HierarchyType::event};
                    auto target_id = 0_ui;
                    for (auto &music : soundbank_event.interactive_music)
                    {
                        auto parent_id = 0_ui;
                        auto next_id = 0_ui;
                        auto &position_source = music.position_source;
                        // Common::fnv_hash(fmt::format("switch_{}_{}", switch_group.name, index));
                        for (auto &position : position_source.position)
                        {
                            assert_conditional(position.group_index.size() == position.child_index.size(), fmt::format("{}", Language::get("wwise.soundbank.encode.group_index_and_child_index_must_same_length")), "create_soundbank");
                            auto result = std::reduce(position.group_index.begin(), position.group_index.end());
                            next_id = Common::fnv_hash(fmt::format("position_id_{}", result));
                            for (auto index : Range(position.group_index.size()))
                            {
                                auto group_index = position.group_index[index];
                                auto child_index = position.child_index[index];
                                //debug(fmt::format("{}, {}", group_index, child_index));
                                assert_conditional(group_index < definition.switches.size(), fmt::format("invalid_group_index: {}", group_index), "create_soundbank");
                                auto &switch_group = definition.switches[group_index];
                                assert_conditional(child_index < switch_group.child.size(), fmt::format("invalid_child_index: {}", child_index), "create_soundbank");
                                auto &music_switch_container = !position_source.is_state ? group_bank.switches[group_index] : group_bank.states[group_index];
                                auto exist_bank = music_switch_container.type == HierarchyType::music_switch_container;
                                auto &playback_setting = music_switch_container.music_switch_container.playback_setting;
                                if (!exist_bank)
                                {
                                    music_switch_container.type = HierarchyType::music_switch_container;
                                    playback_setting.association.path.emplace_back(AudioAssociationSettingPath{
                                        .key = 0,
                                        .object = static_cast<uint32_t>(media_encode_id),
                                        .weight = 50,
                                        .probability = 100});
                                    music_switch_container.id = next_id;
                                    music_switch_container.music_switch_container.parent = parent_id;
                                    auto output_bus_id = Common::fnv_hash((!position_source.is_state ? "Music_Bus" : "SFX_Bus"));
                                    music_switch_container.music_switch_container.output_bus.bus = output_bus_id;
                                    music_switch_container.music_switch_container.time_setting.time = 250.0;
                                    music_switch_container.music_switch_container.time_setting.tempo = 120.0;
                                    music_switch_container.music_switch_container.time_setting.signature = AudioTimeSettingSignature{
                                        .first = 4,
                                        .second = 4};
                                    music_switch_container.music_switch_container.positioning.listener_routing.speaker_panning_divsion_spatialization_mix = 100.0;
                                    music_switch_container.music_switch_container.hdr.envelope_tracking.active_range = 12.0;
                                    music_switch_container.music_switch_container.positioning.listener_routing.position_source.mode = AudioPositioningSettingListenerRoutingPositionSourceMode::emitter;
                                    music_switch_container.music_switch_container.virtual_voice.on_return_to_physical = AudioVirtualVoiceSettingOnReturnToPhysical::play_from_elapsed_time;
                                    music_switch_container.music_switch_container.playback_priority.value = 100.0;
                                    music_switch_container.music_switch_container.override_time_setting = true;
                                    music_switch_container.music_switch_container.override_positioning = true;
                                    music_switch_container.music_switch_container.override_midi_target = true;
                                    music_switch_container.music_switch_container.override_midi_clip_tempo = true;
                                    playback_setting.association.argument.emplace_back(AudioAssociationSettingArgument{
                                        .id = Common::fnv_hash(switch_group.name),
                                        .is_state = position_source.is_state});
                                }
                                auto child_id = Common::fnv_hash(switch_group.child[child_index]);
                                if (auto it = std::find_if(playback_setting.association.path.begin(), playback_setting.association.path.end(), [&](auto &element)
                                                           { return element.key == child_id; });
                                    it != playback_setting.association.path.end())
                                {
                                    next_id = it->object;
                                }
                                else
                                {
                                    next_id = Common::fnv_hash(fmt::format("switch_{}_{}", switch_group.child[child_index], child_index));
                                    music_switch_container.music_switch_container.child.emplace_back(next_id);
                                    playback_setting.association.path.emplace_back(AudioAssociationSettingPath{
                                        .key = child_id,
                                        .object = next_id,
                                        .weight = 50,
                                        .probability = 100});
                                }
                                parent_id = music_switch_container.id;
                                if (target_id == 0_ui)
                                {
                                    target_id = parent_id;
                                }
                            }
                        }
                        auto playlist_hierarchy = Hierarchy{
                            .id = Common::fnv_hash(fmt::format("playlist_{}", next_id)),
                            .type = HierarchyType::music_playlist_container};
                        auto &music_playlist_container = playlist_hierarchy.music_playlist_container;
                        music_playlist_container.parent = next_id;
                        music_playlist_container.output_bus.bus = 3803692087_ui;
                        music_playlist_container.time_setting.time = 250.0;
                        music_playlist_container.time_setting.tempo = 120.0;
                        music_playlist_container.time_setting.signature = AudioTimeSettingSignature{
                            .first = 4,
                            .second = 4};
                        music_playlist_container.positioning.listener_routing.enable = true;
                        music_playlist_container.positioning.listener_routing.speaker_panning_divsion_spatialization_mix = 100.0;
                        music_playlist_container.positioning.listener_routing.attenuation.enable = true;
                        music_playlist_container.hdr.envelope_tracking.active_range = 12.0;
                        music_playlist_container.positioning.listener_routing.position_source.mode = AudioPositioningSettingListenerRoutingPositionSourceMode::emitter;
                        music_playlist_container.virtual_voice.on_return_to_physical = AudioVirtualVoiceSettingOnReturnToPhysical::play_from_elapsed_time;
                        music_playlist_container.playback_priority.value = 100.0;
                        music_playlist_container.override_time_setting = true;
                        music_playlist_container.override_positioning = true;
                        music_playlist_container.override_midi_target = true;
                        music_playlist_container.override_midi_clip_tempo = true;
                        auto &playback_playlist_setting = music_playlist_container.playback_setting;
                        playback_playlist_setting.speed = 1.0;
                        playback_playlist_setting.playlist.emplace_back(MusicPlaylistContainerPlaylistItem{
                            .u1 = Common::fnv_hash(fmt::format("start_group_segment_{}", next_id)),
                            .group = true,
                            .child_count = static_cast<uint32_t>(music.media_source.size()),
                            .play_type = AudioPlayType::sequence,
                            .play_mode = AudioPlayMode::step,
                            .weight = 50000,
                            .loop = 0});
                        for (auto &media : music.media_source)
                        {
                            assert_conditional(media.target_index < definition.media.size(), fmt::format("cannot_find_media_index: {}", media.target_index), "create_soundbank");
                            auto media_resource_id = Common::fnv_hash(definition.media[media.target_index]);
                            if (std::find(soundbank_information.media.begin(), soundbank_information.media.end(), media_resource_id) == soundbank_information.media.end())
                            {
                                soundbank_information.media.emplace_back(media_resource_id);
                            }
                            auto music_segment_id = Common::fnv_hash(fmt::format("music_segment_{}", media_resource_id));
                            auto music_track_hierarchy = Hierarchy{
                                .id = Common::fnv_hash(fmt::format("music_track_{}", media_resource_id)),
                                .type = HierarchyType::music_track};
                            music_track_hierarchy.music_track.parent = music_segment_id;
                            music_track_hierarchy.music_track.source.emplace_back(AudioSourceSetting{
                                .plug_in = media_plug_in,
                                .resource = media_resource_id,
                                .resource_size = static_cast<uint32_t>(std::filesystem::file_size(fmt::format("{}/media/{}.wem", source, media_resource_id)))});
                            music_track_hierarchy.music_track.playback_setting.clip.u1 = 1;
                            music_track_hierarchy.music_track.playback_setting.clip.item.emplace_back(MusicTrackClipItem{
                                .source = media_resource_id,
                                .duration = media.duration});
                            soundbank_information.hierarchy.emplace_back(music_track_hierarchy);
                            // play_list
                            auto music_segment_hierarchy = Hierarchy{
                                .id = music_segment_id,
                                .type = HierarchyType::music_segment};
                            music_segment_hierarchy.music_segment.parent = playlist_hierarchy.id;
                            music_segment_hierarchy.music_segment.child.emplace_back(music_track_hierarchy.id);
                            music_segment_hierarchy.music_segment.playback_setting.speed = 1.0;
                            music_segment_hierarchy.music_segment.playback_setting.duration = media.duration;
                            music_segment_hierarchy.music_segment.time_setting.time = 1000.0;
                            music_segment_hierarchy.music_segment.time_setting.tempo = 120.0;
                            music_segment_hierarchy.music_segment.time_setting.signature = AudioTimeSettingSignature{
                                .first = 4,
                                .second = 4};;
                            music_segment_hierarchy.music_segment.positioning.listener_routing.speaker_panning_divsion_spatialization_mix = 100.0;
                            music_segment_hierarchy.music_segment.positioning.listener_routing.position_source.mode = AudioPositioningSettingListenerRoutingPositionSourceMode::emitter;
                            music_segment_hierarchy.music_segment.hdr.envelope_tracking.active_range = 12.0;
                            music_segment_hierarchy.music_segment.virtual_voice.on_return_to_physical = AudioVirtualVoiceSettingOnReturnToPhysical::play_from_elapsed_time;
                            music_segment_hierarchy.music_segment.playback_setting.cue.item.emplace_back(MusicSegmentCueItem{
                                .id = 43573010});
                            music_segment_hierarchy.music_segment.playback_setting.cue.item.emplace_back(MusicSegmentCueItem{
                                .id = 1539036744,
                                .time = media.duration});
                            soundbank_information.hierarchy.emplace_back(music_segment_hierarchy);
                            music_playlist_container.child.emplace_back(music_segment_id);
                            playback_playlist_setting.playlist.emplace_back(MusicPlaylistContainerPlaylistItem{
                                .u1 = Common::fnv_hash(fmt::format("item_segment_{}", media_resource_id)),
                                .group = true,
                                .child_count = static_cast<uint32_t>(music.media_source.size()),
                                .item = music_segment_id,
                                .play_type = AudioPlayType::random,
                                .play_mode = AudioPlayMode::continuous,
                                .weight = 50000,
                                .loop = static_cast<uint16_t>(media.loop)});
                        }
                        soundbank_information.hierarchy.emplace_back(playlist_hierarchy);
                    }
                    auto action_id = Common::fnv_hash(fmt::format("{}_event_action", event_id));
                    auto event_action = Hierarchy{
                        .id = action_id,
                        .type = HierarchyType::event_action};
                    event_action.event_action.type = 4_byte;
                    event_action.event_action.target = target_id;
                    event_action.event_action.play_audio.delay.value = 100;
                    event_action.event_action.play_audio.fade_curve = Curve::linear;
                    event_action.event_action.play_audio.probability = 100.0;
                    event_action.event_action.play_audio.sound_bank = sounbank_id;
                    sounbank_event_list.emplace_back(event_action);
                    event_hierarchy.event.child.emplace_back(action_id);
                    sounbank_event_list.emplace_back(event_hierarchy);
                }
                std::reverse(group_bank.switches.begin(), group_bank.switches.end());
                for (auto &element : group_bank.switches)
                {
                    if (element.type == HierarchyType::music_switch_container)
                    {
                        soundbank_information.hierarchy.emplace_back(element);
                    }
                }
                for (auto &element : group_bank.states)
                {
                    if (element.type == HierarchyType::music_switch_container)
                    {
                        soundbank_information.hierarchy.emplace_back(element);
                    }
                }
                for (auto &element : sounbank_event_list)
                {
                    soundbank_information.hierarchy.emplace_back(element);
                }
                if (soundbank_information.media.size() > k_none_size)
                {
                    soundbank_information.has_media = true;
                }
                write_json(fmt::format("{}/{}.json", destination, soundbank.name), soundbank_information);
                /*
                //--------------------
                Sen::Kernel::Support::WWise::SoundBank::Encode::process_whole(stream, soundbank_information, source);
                stream.out_file(fmt::format("{}/{}.bnk", destination, soundbank.name));
                */
            }
            return;
        }
    };
}