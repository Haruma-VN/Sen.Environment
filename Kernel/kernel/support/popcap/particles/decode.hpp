#pragma once

#include "kernel/definition/utility.hpp"
#include "kernel/support/popcap/particles/common.hpp"
#include "kernel/support/popcap/particles/definition.hpp"
#include "kernel/support/popcap/zlib/uncompress.hpp"

namespace Sen::Kernel::Support::PopCap::Particles
{

    using namespace Definition;

    class Decode : Common
    {

    private:
        DataStreamView stream;

        inline auto read_track_nodes(
            std::vector<ParticlesTrackNode> &value) -> bool
        {
            auto count = static_cast<size_t>(stream.readInt32());
            if (count == 0_size)
            {
                return true;
            }
            value.resize(count);
            for (auto &node : value)
            {
                node.time = stream.readFloat();
                node.low_value = stream.readFloat();
                if (node.low_value != 0.0)
                {
                    node.low_value_is_null = false;
                }
                node.high_value = stream.readFloat();
                if (node.high_value != 0.0)
                {
                    node.high_value_is_null = false;
                }
                node.curve_type = stream.readInt32();
                if (node.curve_type != 1)
                {
                    node.curve_type_is_null = false;
                }
                node.distribution = stream.readInt32();
                if (node.distribution != 1)
                {
                    node.distribution_is_null = false;
                }
            }
            return false;
        }

        inline auto read_track_nodes_big(
            DataStreamViewBigEndian &stream_big,
            std::vector<ParticlesTrackNode> &value) -> bool
        {
            auto count = static_cast<size_t>(stream_big.readInt32());
            if (count == 0_size)
            {
                return true;
            }
            value.resize(count);
            for (auto &node : value)
            {
                node.time = stream_big.readFloat();
                node.low_value = stream_big.readFloat();
                if (node.low_value != 0.0)
                {
                    node.low_value_is_null = false;
                }
                node.high_value = stream_big.readFloat();
                if (node.high_value != 0.0)
                {
                    node.high_value_is_null = false;
                }
                node.curve_type = stream_big.readInt32();
                if (node.curve_type != 1)
                {
                    node.curve_type_is_null = false;
                }
                node.distribution = stream_big.readInt32();
                if (node.distribution != 1)
                {
                    node.distribution_is_null = false;
                }
            }
            return false;
        }

        template <auto UseVariant>
        inline auto decode_zlib() -> void
        {
            static_assert(sizeof(UseVariant) == sizeof(bool));
			static_assert(UseVariant == true or UseVariant == false);
            auto magic = stream.readInt32();
            stream.read_pos = 0_size;
            if (magic == zlib_magic || magic == zlib_magic_big)
            {
                auto uncompress = Sen::Kernel::Support::PopCap::Zlib::Uncompress<UseVariant>{};
                auto data = stream.toBytes();
                stream.close();
                stream.writeBytes(uncompress.uncompress(data));
            }
            return;
        }

        inline auto decode_pc(
            Particles &definition) -> void
        {
            decode_zlib<false>();
            stream.read_pos = 0x8_size;
            definition.emitters.resize(static_cast<size_t>(stream.readInt32()));
            assert_conditional(stream.readInt32() == k_32bit_block_byte_1, fmt::format("{}", Language::get("popcap.particles.invalid_block_1")), "decode_pc");
            if (definition.emitters.size() != 0_size)
            {
                definition.emitters_is_null = false;
            }
            for (auto &emitter : definition.emitters)
            {
                stream.read_pos += 4_size;
                emitter.image_cols = stream.readInt32();
                if (emitter.image_cols != 0)
                {
                    emitter.image_cols_is_null = false;
                }
                emitter.image_rows = stream.readInt32();
                if (emitter.image_rows != 0)
                {
                    emitter.image_rows_is_null = false;
                }
                emitter.image_frames = stream.readInt32();
                if (emitter.image_frames != 1)
                {
                    emitter.image_frames_is_null = false;
                }
                emitter.animated = stream.readInt32();
                if (emitter.animated != 0)
                {
                    emitter.animated_is_null = false;
                }
                emitter.particles_flag = stream.readInt32();
                emitter.emitter_type = stream.readInt32();
                if (emitter.emitter_type != 1)
                {
                    emitter.emitter_type_is_null = false;
                }
                stream.read_pos += 188_size;
                emitter.field.resize(static_cast<size_t>(stream.readInt32()));
                if (emitter.field.size() != 0_size)
                {
                    emitter.field_is_null = false;
                }
                stream.read_pos += 4_size;
                emitter.system_field.resize(static_cast<size_t>(stream.readInt32()));
                if (emitter.system_field.size() != 0_size)
                {
                    emitter.system_field_is_null = false;
                }
                stream.read_pos += 128_size;
            }
            for (auto &emitter : definition.emitters)
            {
                emitter.image = stream.readStringByInt32();
                if (!emitter.image.empty())
                {
                    emitter.image_is_null = false;
                }
                emitter.name = stream.readStringByInt32();
                if (!emitter.name.empty())
                {
                    emitter.name_is_null = false;
                }
                emitter.system_duration_is_null = read_track_nodes(emitter.system_duration);
                emitter.on_duration = stream.readStringByInt32();
                if (!emitter.on_duration.empty())
                {
                    emitter.on_duration_is_null = false;
                }
                emitter.cross_fade_duration_is_null = read_track_nodes(emitter.cross_fade_duration);
                emitter.spawn_rate_is_null = read_track_nodes(emitter.spawn_rate);
                emitter.spawn_min_active_is_null = read_track_nodes(emitter.spawn_min_active);
                emitter.spawn_max_active_is_null = read_track_nodes(emitter.spawn_max_active);
                emitter.spawn_max_launched_is_null = read_track_nodes(emitter.spawn_max_launched);
                emitter.emitter_radius_is_null = read_track_nodes(emitter.emitter_radius);
                emitter.emitter_offset_x_is_null = read_track_nodes(emitter.emitter_offset_x);
                emitter.emitter_offset_y_is_null = read_track_nodes(emitter.emitter_offset_y);
                emitter.emitter_box_x_is_null = read_track_nodes(emitter.emitter_box_x);
                emitter.emitter_box_y_is_null = read_track_nodes(emitter.emitter_box_y);
                emitter.emitter_path_is_null = read_track_nodes(emitter.emitter_path);
                emitter.emitter_skew_x_is_null = read_track_nodes(emitter.emitter_skew_x);
                emitter.emitter_skew_y_is_null = read_track_nodes(emitter.emitter_skew_y);
                emitter.particle_duration_is_null = read_track_nodes(emitter.particle_duration);
                emitter.system_red_is_null = read_track_nodes(emitter.system_red);
                emitter.system_green_is_null = read_track_nodes(emitter.system_green);
                emitter.system_blue_is_null = read_track_nodes(emitter.system_blue);
                emitter.system_alpha_is_null = read_track_nodes(emitter.system_alpha);
                emitter.system_brightness_is_null = read_track_nodes(emitter.system_brightness);
                emitter.launch_speed_is_null = read_track_nodes(emitter.launch_speed);
                emitter.launch_angle_is_null = read_track_nodes(emitter.launch_angle);
                assert_conditional(stream.readInt32() == k_32bit_block_byte_2, fmt::format("{}", Language::get("popcap.particles.invalid_block_2")), "decode_pc");
                if (!emitter.field_is_null)
                {
                    for (auto &field : emitter.field)
                    {
                        field.field_type = stream.readInt32();
                        if (field.field_type != 0)
                        {
                            field.field_type_is_null = false;
                        }
                        stream.read_pos += 16_size;
                    }
                    for (auto &field : emitter.field)
                    {
                        field.x_is_null = read_track_nodes(field.x);
                        field.y_is_null = read_track_nodes(field.y);
                    }
                }
                assert_conditional(stream.readInt32() == k_32bit_block_byte_2, fmt::format("{}", Language::get("popcap.particles.invalid_block_2")), "decode_pc");
                if (!emitter.system_field_is_null)
                {
                    for (auto &field : emitter.system_field)
                    {
                        field.field_type = stream.readInt32();
                        if (field.field_type != 0)
                        {
                            field.field_type_is_null = false;
                        }
                        stream.read_pos += 16_size;
                    }
                    for (auto &field : emitter.field)
                    {
                        field.x_is_null = read_track_nodes(field.x);
                        field.y_is_null = read_track_nodes(field.y);
                    }
                }
                emitter.particle_red_is_null = read_track_nodes(emitter.particle_red);
                emitter.particle_green_is_null = read_track_nodes(emitter.particle_green);
                emitter.particle_blue_is_null = read_track_nodes(emitter.particle_blue);
                emitter.particle_alpha_is_null = read_track_nodes(emitter.particle_alpha);
                emitter.particle_brightness_is_null = read_track_nodes(emitter.particle_brightness);
                emitter.particle_spin_angle_is_null = read_track_nodes(emitter.particle_spin_angle);
                emitter.particle_spin_speed_is_null = read_track_nodes(emitter.particle_spin_speed);
                emitter.particle_scale_is_null = read_track_nodes(emitter.particle_scale);
                emitter.particle_stretch_is_null = read_track_nodes(emitter.particle_stretch);
                emitter.collision_reflect_is_null = read_track_nodes(emitter.collision_reflect);
                emitter.collision_spin_is_null = read_track_nodes(emitter.collision_spin);
                emitter.clip_top_is_null = read_track_nodes(emitter.clip_top);
                emitter.clip_bottom_is_null = read_track_nodes(emitter.clip_bottom);
                emitter.clip_left_is_null = read_track_nodes(emitter.clip_left);
                emitter.clip_right_is_null = read_track_nodes(emitter.clip_right);
                emitter.animation_rate_is_null = read_track_nodes(emitter.animation_rate);
            }
            return;
        }

        inline auto decode_tv(
            Particles &definition) -> void
        {
            decode_zlib<false>();
            stream.read_pos = 0x8_size;
            definition.emitters.resize(static_cast<size_t>(stream.readInt32()));
            assert_conditional(stream.readInt32() == k_32bit_block_byte_1, fmt::format("{}", Language::get("popcap.particles.invalid_block_1")), "decode_tv");
            if (definition.emitters.size() != 0_size)
            {
                definition.emitters_is_null = false;
            }
            for (auto &emitter : definition.emitters)
            {
                stream.read_pos += 4_size;
                emitter.image_cols = stream.readInt32();
                if (emitter.image_cols != 0)
                {
                    emitter.image_cols_is_null = false;
                }
                emitter.image_rows = stream.readInt32();
                if (emitter.image_rows != 0)
                {
                    emitter.image_rows_is_null = false;
                }
                emitter.image_frames = stream.readInt32();
                if (emitter.image_frames != 1)
                {
                    emitter.image_frames_is_null = false;
                }
                emitter.animated = stream.readInt32();
                if (emitter.animated != 0)
                {
                    emitter.animated_is_null = false;
                }
                emitter.particles_flag = stream.readInt32();
                emitter.emitter_type = stream.readInt32();
                if (emitter.emitter_type != 1)
                {
                    emitter.emitter_type_is_null = false;
                }
                stream.read_pos += 188_size;
                emitter.field.resize(static_cast<size_t>(stream.readInt32()));
                if (emitter.field.size() != 0_size)
                {
                    emitter.field_is_null = false;
                }
                stream.read_pos += 4_size;
                emitter.system_field.resize(static_cast<size_t>(stream.readInt32()));
                if (emitter.system_field.size() != 0_size)
                {
                    emitter.system_field_is_null = false;
                }
                stream.read_pos += 128_size;
            }
            for (auto &emitter : definition.emitters)
            {
                emitter.image = stream.readStringByInt32();
                if (!emitter.image.empty())
                {
                    emitter.image_is_null = false;
                }
                emitter.image_path = stream.readStringByInt32();
                if (!emitter.image_path.empty())
                {
                    emitter.image_path_is_null = false;
                }
                emitter.name = stream.readStringByInt32();
                if (!emitter.name.empty())
                {
                    emitter.name_is_null = false;
                }
                emitter.system_duration_is_null = read_track_nodes(emitter.system_duration);
                emitter.on_duration = stream.readStringByInt32();
                if (!emitter.on_duration.empty())
                {
                    emitter.on_duration_is_null = false;
                }
                emitter.cross_fade_duration_is_null = read_track_nodes(emitter.cross_fade_duration);
                emitter.spawn_rate_is_null = read_track_nodes(emitter.spawn_rate);
                emitter.spawn_min_active_is_null = read_track_nodes(emitter.spawn_min_active);
                emitter.spawn_max_active_is_null = read_track_nodes(emitter.spawn_max_active);
                emitter.spawn_max_launched_is_null = read_track_nodes(emitter.spawn_max_launched);
                emitter.emitter_radius_is_null = read_track_nodes(emitter.emitter_radius);
                emitter.emitter_offset_x_is_null = read_track_nodes(emitter.emitter_offset_x);
                emitter.emitter_offset_y_is_null = read_track_nodes(emitter.emitter_offset_y);
                emitter.emitter_box_x_is_null = read_track_nodes(emitter.emitter_box_x);
                emitter.emitter_box_y_is_null = read_track_nodes(emitter.emitter_box_y);
                emitter.emitter_path_is_null = read_track_nodes(emitter.emitter_path);
                emitter.emitter_skew_x_is_null = read_track_nodes(emitter.emitter_skew_x);
                emitter.emitter_skew_y_is_null = read_track_nodes(emitter.emitter_skew_y);
                emitter.particle_duration_is_null = read_track_nodes(emitter.particle_duration);
                emitter.system_red_is_null = read_track_nodes(emitter.system_red);
                emitter.system_green_is_null = read_track_nodes(emitter.system_green);
                emitter.system_blue_is_null = read_track_nodes(emitter.system_blue);
                emitter.system_alpha_is_null = read_track_nodes(emitter.system_alpha);
                emitter.system_brightness_is_null = read_track_nodes(emitter.system_brightness);
                emitter.launch_speed_is_null = read_track_nodes(emitter.launch_speed);
                emitter.launch_angle_is_null = read_track_nodes(emitter.launch_angle);
                assert_conditional(stream.readInt32() == k_32bit_block_byte_2, fmt::format("{}", Language::get("popcap.particles.invalid_block_2")), "decode_tv");
                if (!emitter.field_is_null)
                {
                    for (auto &field : emitter.field)
                    {
                        field.field_type = stream.readInt32();
                        if (field.field_type != 0)
                        {
                            field.field_type_is_null = false;
                        }
                        stream.read_pos += 16_size;
                    }
                    for (auto &field : emitter.field)
                    {
                        field.x_is_null = read_track_nodes(field.x);
                        field.y_is_null = read_track_nodes(field.y);
                    }
                }
                assert_conditional(stream.readInt32() == k_32bit_block_byte_2, fmt::format("{}", Language::get("popcap.particles.invalid_block_2")), "decode_tv");
                if (!emitter.system_field_is_null)
                {
                    for (auto &field : emitter.system_field)
                    {
                        field.field_type = stream.readInt32();
                        if (field.field_type != 0)
                        {
                            field.field_type_is_null = false;
                        }
                        stream.read_pos += 16_size;
                    }
                    for (auto &field : emitter.field)
                    {
                        field.x_is_null = read_track_nodes(field.x);
                        field.y_is_null = read_track_nodes(field.y);
                    }
                }
                emitter.particle_red_is_null = read_track_nodes(emitter.particle_red);
                emitter.particle_green_is_null = read_track_nodes(emitter.particle_green);
                emitter.particle_blue_is_null = read_track_nodes(emitter.particle_blue);
                emitter.particle_alpha_is_null = read_track_nodes(emitter.particle_alpha);
                emitter.particle_brightness_is_null = read_track_nodes(emitter.particle_brightness);
                emitter.particle_spin_angle_is_null = read_track_nodes(emitter.particle_spin_angle);
                emitter.particle_spin_speed_is_null = read_track_nodes(emitter.particle_spin_speed);
                emitter.particle_scale_is_null = read_track_nodes(emitter.particle_scale);
                emitter.particle_stretch_is_null = read_track_nodes(emitter.particle_stretch);
                emitter.collision_reflect_is_null = read_track_nodes(emitter.collision_reflect);
                emitter.collision_spin_is_null = read_track_nodes(emitter.collision_spin);
                emitter.clip_top_is_null = read_track_nodes(emitter.clip_top);
                emitter.clip_bottom_is_null = read_track_nodes(emitter.clip_bottom);
                emitter.clip_left_is_null = read_track_nodes(emitter.clip_left);
                emitter.clip_right_is_null = read_track_nodes(emitter.clip_right);
                emitter.animation_rate_is_null = read_track_nodes(emitter.animation_rate);
            }
            return;
        }

        inline auto decode_phone_32(
            Particles &definition) -> void
        {
            decode_zlib<false>();
            stream.read_pos = 0x8_size;
            definition.emitters.resize(static_cast<size_t>(stream.readInt32()));
            assert_conditional(stream.readInt32() == k_32bit_block_byte_1, fmt::format("{}", Language::get("popcap.particles.invalid_block_1")), "decode_phone_32");
            if (definition.emitters.size() != 0_size)
            {
                definition.emitters_is_null = false;
            }
            for (auto &emitter : definition.emitters)
            {
                stream.read_pos += 4_size;
                emitter.image_cols = stream.readInt32();
                if (emitter.image_cols != 0)
                {
                    emitter.image_cols_is_null = false;
                }
                emitter.image_rows = stream.readInt32();
                if (emitter.image_rows != 0)
                {
                    emitter.image_rows_is_null = false;
                }
                emitter.image_frames = stream.readInt32();
                if (emitter.image_frames != 1)
                {
                    emitter.image_frames_is_null = false;
                }
                emitter.animated = stream.readInt32();
                if (emitter.animated != 0)
                {
                    emitter.animated_is_null = false;
                }
                emitter.particles_flag = stream.readInt32();
                emitter.emitter_type = stream.readInt32();
                if (emitter.emitter_type != 1)
                {
                    emitter.emitter_type_is_null = false;
                }
                stream.read_pos += 188_size;
                emitter.field.resize(static_cast<size_t>(stream.readInt32()));
                if (emitter.field.size() != 0_size)
                {
                    emitter.field_is_null = false;
                }
                stream.read_pos += 4_size;
                emitter.system_field.resize(static_cast<size_t>(stream.readInt32()));
                if (emitter.system_field.size() != 0_size)
                {
                    emitter.system_field_is_null = false;
                }
                stream.read_pos += 128_size;
            }
            for (auto &emitter : definition.emitters)
            {
                auto image = stream.readInt32();
                if (image != -1)
                {
                    emitter.image = std::to_string(image);
                    emitter.image_is_null = false;
                }
                emitter.name = stream.readStringByInt32();
                if (!emitter.name.empty())
                {
                    emitter.name_is_null = false;
                }
                emitter.system_duration_is_null = read_track_nodes(emitter.system_duration);
                emitter.on_duration = stream.readStringByInt32();
                if (!emitter.on_duration.empty())
                {
                    emitter.on_duration_is_null = false;
                }
                emitter.cross_fade_duration_is_null = read_track_nodes(emitter.cross_fade_duration);
                emitter.spawn_rate_is_null = read_track_nodes(emitter.spawn_rate);
                emitter.spawn_min_active_is_null = read_track_nodes(emitter.spawn_min_active);
                emitter.spawn_max_active_is_null = read_track_nodes(emitter.spawn_max_active);
                emitter.spawn_max_launched_is_null = read_track_nodes(emitter.spawn_max_launched);
                emitter.emitter_radius_is_null = read_track_nodes(emitter.emitter_radius);
                emitter.emitter_offset_x_is_null = read_track_nodes(emitter.emitter_offset_x);
                emitter.emitter_offset_y_is_null = read_track_nodes(emitter.emitter_offset_y);
                emitter.emitter_box_x_is_null = read_track_nodes(emitter.emitter_box_x);
                emitter.emitter_box_y_is_null = read_track_nodes(emitter.emitter_box_y);
                emitter.emitter_path_is_null = read_track_nodes(emitter.emitter_path);
                emitter.emitter_skew_x_is_null = read_track_nodes(emitter.emitter_skew_x);
                emitter.emitter_skew_y_is_null = read_track_nodes(emitter.emitter_skew_y);
                emitter.particle_duration_is_null = read_track_nodes(emitter.particle_duration);
                emitter.system_red_is_null = read_track_nodes(emitter.system_red);
                emitter.system_green_is_null = read_track_nodes(emitter.system_green);
                emitter.system_blue_is_null = read_track_nodes(emitter.system_blue);
                emitter.system_alpha_is_null = read_track_nodes(emitter.system_alpha);
                emitter.system_brightness_is_null = read_track_nodes(emitter.system_brightness);
                emitter.launch_speed_is_null = read_track_nodes(emitter.launch_speed);
                emitter.launch_angle_is_null = read_track_nodes(emitter.launch_angle);
                assert_conditional(stream.readInt32() == k_32bit_block_byte_2, fmt::format("{}", Language::get("popcap.particles.invalid_block_2")), "decode_phone_32");
                if (!emitter.field_is_null)
                {
                    for (auto &field : emitter.field)
                    {
                        field.field_type = stream.readInt32();
                        if (field.field_type != 0)
                        {
                            field.field_type_is_null = false;
                        }
                        stream.read_pos += 16_size;
                    }
                    for (auto &field : emitter.field)
                    {
                        field.x_is_null = read_track_nodes(field.x);
                        field.y_is_null = read_track_nodes(field.y);
                    }
                }
                assert_conditional(stream.readInt32() == k_32bit_block_byte_2, fmt::format("{}", Language::get("popcap.particles.invalid_block_2")), "decode_phone_32");
                if (!emitter.system_field_is_null)
                {
                    for (auto &field : emitter.system_field)
                    {
                        field.field_type = stream.readInt32();
                        if (field.field_type != 0)
                        {
                            field.field_type_is_null = false;
                        }
                        stream.read_pos += 16_size;
                    }
                    for (auto &field : emitter.field)
                    {
                        field.x_is_null = read_track_nodes(field.x);
                        field.y_is_null = read_track_nodes(field.y);
                    }
                }
                emitter.particle_red_is_null = read_track_nodes(emitter.particle_red);
                emitter.particle_green_is_null = read_track_nodes(emitter.particle_green);
                emitter.particle_blue_is_null = read_track_nodes(emitter.particle_blue);
                emitter.particle_alpha_is_null = read_track_nodes(emitter.particle_alpha);
                emitter.particle_brightness_is_null = read_track_nodes(emitter.particle_brightness);
                emitter.particle_spin_angle_is_null = read_track_nodes(emitter.particle_spin_angle);
                emitter.particle_spin_speed_is_null = read_track_nodes(emitter.particle_spin_speed);
                emitter.particle_scale_is_null = read_track_nodes(emitter.particle_scale);
                emitter.particle_stretch_is_null = read_track_nodes(emitter.particle_stretch);
                emitter.collision_reflect_is_null = read_track_nodes(emitter.collision_reflect);
                emitter.collision_spin_is_null = read_track_nodes(emitter.collision_spin);
                emitter.clip_top_is_null = read_track_nodes(emitter.clip_top);
                emitter.clip_bottom_is_null = read_track_nodes(emitter.clip_bottom);
                emitter.clip_left_is_null = read_track_nodes(emitter.clip_left);
                emitter.clip_right_is_null = read_track_nodes(emitter.clip_right);
                emitter.animation_rate_is_null = read_track_nodes(emitter.animation_rate);
            }
            return;
        }

        inline auto decode_phone_64(
            Particles &definition) -> void
        {
            decode_zlib<true>();
            stream.read_pos = 12_size;
            definition.emitters.resize(static_cast<size_t>(stream.readInt32()));
            stream.read_pos += 0x4_size;
            assert_conditional(stream.readInt32() == k_64bit_block_byte_1, fmt::format("{}", Language::get("popcap.particles.invalid_block_1")), "decode_phone_64");
            if (definition.emitters.size() != 0_size)
            {
                definition.emitters_is_null = false;
            }
            for (auto &emitter : definition.emitters)
            {
                stream.read_pos += 8_size;
                emitter.image_cols = stream.readInt32();
                if (emitter.image_cols != 0)
                {
                    emitter.image_cols_is_null = false;
                }
                emitter.image_rows = stream.readInt32();
                if (emitter.image_rows != 0)
                {
                    emitter.image_rows_is_null = false;
                }
                emitter.image_frames = stream.readInt32();
                if (emitter.image_frames != 1)
                {
                    emitter.image_frames_is_null = false;
                }
                emitter.animated = stream.readInt32();
                if (emitter.animated != 0)
                {
                    emitter.animated_is_null = false;
                }
                emitter.particles_flag = stream.readInt32();
                emitter.emitter_type = stream.readInt32();
                if (emitter.emitter_type != 1)
                {
                    emitter.emitter_type_is_null = false;
                }
                stream.read_pos += 376_size;
                emitter.field.resize(static_cast<size_t>(stream.readInt32()));
                if (emitter.field.size() != 0_size)
                {
                    emitter.field_is_null = false;
                }
                stream.read_pos += 12_size;
                emitter.system_field.resize(static_cast<size_t>(stream.readInt32()));
                if (emitter.system_field.size() != 0_size)
                {
                    emitter.system_field_is_null = false;
                }
                stream.read_pos += 260_size;
            }
            for (auto &emitter : definition.emitters)
            {
                auto image = stream.readInt32();
                if (image != -1)
                {
                    emitter.image = std::to_string(image);
                    emitter.image_is_null = false;
                }
                emitter.name = stream.readStringByInt32();
                if (!emitter.name.empty())
                {
                    emitter.name_is_null = false;
                }
                emitter.system_duration_is_null = read_track_nodes(emitter.system_duration);
                emitter.on_duration = stream.readStringByInt32();
                if (!emitter.on_duration.empty())
                {
                    emitter.on_duration_is_null = false;
                }
                emitter.cross_fade_duration_is_null = read_track_nodes(emitter.cross_fade_duration);
                emitter.spawn_rate_is_null = read_track_nodes(emitter.spawn_rate);
                emitter.spawn_min_active_is_null = read_track_nodes(emitter.spawn_min_active);
                emitter.spawn_max_active_is_null = read_track_nodes(emitter.spawn_max_active);
                emitter.spawn_max_launched_is_null = read_track_nodes(emitter.spawn_max_launched);
                emitter.emitter_radius_is_null = read_track_nodes(emitter.emitter_radius);
                emitter.emitter_offset_x_is_null = read_track_nodes(emitter.emitter_offset_x);
                emitter.emitter_offset_y_is_null = read_track_nodes(emitter.emitter_offset_y);
                emitter.emitter_box_x_is_null = read_track_nodes(emitter.emitter_box_x);
                emitter.emitter_box_y_is_null = read_track_nodes(emitter.emitter_box_y);
                emitter.emitter_path_is_null = read_track_nodes(emitter.emitter_path);
                emitter.emitter_skew_x_is_null = read_track_nodes(emitter.emitter_skew_x);
                emitter.emitter_skew_y_is_null = read_track_nodes(emitter.emitter_skew_y);
                emitter.particle_duration_is_null = read_track_nodes(emitter.particle_duration);
                emitter.system_red_is_null = read_track_nodes(emitter.system_red);
                emitter.system_green_is_null = read_track_nodes(emitter.system_green);
                emitter.system_blue_is_null = read_track_nodes(emitter.system_blue);
                emitter.system_alpha_is_null = read_track_nodes(emitter.system_alpha);
                emitter.system_brightness_is_null = read_track_nodes(emitter.system_brightness);
                emitter.launch_speed_is_null = read_track_nodes(emitter.launch_speed);
                emitter.launch_angle_is_null = read_track_nodes(emitter.launch_angle);
                assert_conditional(stream.readInt32() == k_64bit_block_byte_2, fmt::format("{}", Language::get("popcap.particles.invalid_block_2")), "decode_phone_64");
                if (!emitter.field_is_null)
                {
                    for (auto &field : emitter.field)
                    {
                        field.field_type = stream.readInt32();
                        if (field.field_type != 0)
                        {
                            field.field_type_is_null = false;
                        }
                        stream.read_pos += 36_size;
                    }
                    for (auto &field : emitter.field)
                    {
                        field.x_is_null = read_track_nodes(field.x);
                        field.y_is_null = read_track_nodes(field.y);
                    }
                }
                assert_conditional(stream.readInt32() == k_64bit_block_byte_2, fmt::format("{}", Language::get("popcap.particles.invalid_block_3")), "decode_phone_64");
                if (!emitter.system_field_is_null)
                {
                    for (auto &field : emitter.system_field)
                    {
                        field.field_type = stream.readInt32();
                        if (field.field_type != 0)
                        {
                            field.field_type_is_null = false;
                        }
                        stream.read_pos += 36_size;
                    }
                    for (auto &field : emitter.field)
                    {
                        field.x_is_null = read_track_nodes(field.x);
                        field.y_is_null = read_track_nodes(field.y);
                    }
                }
                emitter.particle_red_is_null = read_track_nodes(emitter.particle_red);
                emitter.particle_green_is_null = read_track_nodes(emitter.particle_green);
                emitter.particle_blue_is_null = read_track_nodes(emitter.particle_blue);
                emitter.particle_alpha_is_null = read_track_nodes(emitter.particle_alpha);
                emitter.particle_brightness_is_null = read_track_nodes(emitter.particle_brightness);
                emitter.particle_spin_angle_is_null = read_track_nodes(emitter.particle_spin_angle);
                emitter.particle_spin_speed_is_null = read_track_nodes(emitter.particle_spin_speed);
                emitter.particle_scale_is_null = read_track_nodes(emitter.particle_scale);
                emitter.particle_stretch_is_null = read_track_nodes(emitter.particle_stretch);
                emitter.collision_reflect_is_null = read_track_nodes(emitter.collision_reflect);
                emitter.collision_spin_is_null = read_track_nodes(emitter.collision_spin);
                emitter.clip_top_is_null = read_track_nodes(emitter.clip_top);
                emitter.clip_bottom_is_null = read_track_nodes(emitter.clip_bottom);
                emitter.clip_left_is_null = read_track_nodes(emitter.clip_left);
                emitter.clip_right_is_null = read_track_nodes(emitter.clip_right);
                emitter.animation_rate_is_null = read_track_nodes(emitter.animation_rate);
            }
            return;
        }

        inline auto decode_game_console(
            Particles &definition) -> void
        {
            decode_zlib<false>();
            auto stream_big = DataStreamViewBigEndian{};
            stream_big.writeBytes(stream.toBytes());
            stream_big.read_pos = 0x8_size;
            definition.emitters.resize(static_cast<size_t>(stream_big.readInt32()));
            assert_conditional(stream_big.readInt32() == k_32bit_block_byte_1, fmt::format("{}", Language::get("popcap.particles.invalid_block_1")), "decode_game_console");
            if (definition.emitters.size() != 0_size)
            {
                definition.emitters_is_null = false;
            }
            for (auto &emitter : definition.emitters)
            {
                stream_big.read_pos += 4_size;
                emitter.image_cols = stream_big.readInt32();
                if (emitter.image_cols != 0)
                {
                    emitter.image_cols_is_null = false;
                }
                emitter.image_rows = stream_big.readInt32();
                if (emitter.image_rows != 0)
                {
                    emitter.image_rows_is_null = false;
                }
                emitter.image_frames = stream_big.readInt32();
                if (emitter.image_frames != 1)
                {
                    emitter.image_frames_is_null = false;
                }
                emitter.animated = stream_big.readInt32();
                if (emitter.animated != 0)
                {
                    emitter.animated_is_null = false;
                }
                emitter.particles_flag = stream_big.readInt32();
                emitter.emitter_type = stream_big.readInt32();
                if (emitter.emitter_type != 1)
                {
                    emitter.emitter_type_is_null = false;
                }
                stream_big.read_pos += 188_size;
                emitter.field.resize(static_cast<size_t>(stream_big.readInt32()));
                if (emitter.field.size() != 0_size)
                {
                    emitter.field_is_null = false;
                }
                stream_big.read_pos += 4_size;
                emitter.system_field.resize(static_cast<size_t>(stream_big.readInt32()));
                if (emitter.system_field.size() != 0_size)
                {
                    emitter.system_field_is_null = false;
                }
                stream_big.read_pos += 128_size;
            }
            for (auto &emitter : definition.emitters)
            {
                emitter.image = stream_big.readStringByInt32();
                if (!emitter.image.empty())
                {
                    emitter.image_is_null = false;
                }
                emitter.name = stream_big.readStringByInt32();
                if (!emitter.name.empty())
                {
                    emitter.name_is_null = false;
                }
                emitter.system_duration_is_null = read_track_nodes_big(stream_big, emitter.system_duration);
                emitter.on_duration = stream_big.readStringByInt32();
                if (!emitter.on_duration.empty())
                {
                    emitter.on_duration_is_null = false;
                }
                emitter.cross_fade_duration_is_null = read_track_nodes_big(stream_big, emitter.cross_fade_duration);
                emitter.spawn_rate_is_null = read_track_nodes_big(stream_big, emitter.spawn_rate);
                emitter.spawn_min_active_is_null = read_track_nodes_big(stream_big, emitter.spawn_min_active);
                emitter.spawn_max_active_is_null = read_track_nodes_big(stream_big, emitter.spawn_max_active);
                emitter.spawn_max_launched_is_null = read_track_nodes_big(stream_big, emitter.spawn_max_launched);
                emitter.emitter_radius_is_null = read_track_nodes_big(stream_big, emitter.emitter_radius);
                emitter.emitter_offset_x_is_null = read_track_nodes_big(stream_big, emitter.emitter_offset_x);
                emitter.emitter_offset_y_is_null = read_track_nodes_big(stream_big, emitter.emitter_offset_y);
                emitter.emitter_box_x_is_null = read_track_nodes_big(stream_big, emitter.emitter_box_x);
                emitter.emitter_box_y_is_null = read_track_nodes_big(stream_big, emitter.emitter_box_y);
                emitter.emitter_path_is_null = read_track_nodes_big(stream_big, emitter.emitter_path);
                emitter.emitter_skew_x_is_null = read_track_nodes_big(stream_big, emitter.emitter_skew_x);
                emitter.emitter_skew_y_is_null = read_track_nodes_big(stream_big, emitter.emitter_skew_y);
                emitter.particle_duration_is_null = read_track_nodes_big(stream_big, emitter.particle_duration);
                emitter.system_red_is_null = read_track_nodes_big(stream_big, emitter.system_red);
                emitter.system_green_is_null = read_track_nodes_big(stream_big, emitter.system_green);
                emitter.system_blue_is_null = read_track_nodes_big(stream_big, emitter.system_blue);
                emitter.system_alpha_is_null = read_track_nodes_big(stream_big, emitter.system_alpha);
                emitter.system_brightness_is_null = read_track_nodes_big(stream_big, emitter.system_brightness);
                emitter.launch_speed_is_null = read_track_nodes_big(stream_big, emitter.launch_speed);
                emitter.launch_angle_is_null = read_track_nodes_big(stream_big, emitter.launch_angle);
                assert_conditional(stream_big.readInt32() == k_32bit_block_byte_2, fmt::format("{}", Language::get("popcap.particles.invalid_block_2")), "decode_game_console");
                if (!emitter.field_is_null)
                {
                    for (auto &field : emitter.field)
                    {
                        field.field_type = stream_big.readInt32();
                        if (field.field_type != 0)
                        {
                            field.field_type_is_null = false;
                        }
                        stream_big.read_pos += 16_size;
                    }
                    for (auto &field : emitter.field)
                    {
                        field.x_is_null = read_track_nodes_big(stream_big, field.x);
                        field.y_is_null = read_track_nodes_big(stream_big, field.y);
                    }
                }
                assert_conditional(stream_big.readInt32() == k_32bit_block_byte_2, fmt::format("{}", Language::get("popcap.particles.invalid_block_2")), "decode_game_console");
                if (!emitter.system_field_is_null)
                {
                    for (auto &field : emitter.system_field)
                    {
                        field.field_type = stream_big.readInt32();
                        if (field.field_type != 0)
                        {
                            field.field_type_is_null = false;
                        }
                        stream_big.read_pos += 16_size;
                    }
                    for (auto &field : emitter.field)
                    {
                        field.x_is_null = read_track_nodes_big(stream_big, field.x);
                        field.y_is_null = read_track_nodes_big(stream_big, field.y);
                    }
                }
                emitter.particle_red_is_null = read_track_nodes_big(stream_big, emitter.particle_red);
                emitter.particle_green_is_null = read_track_nodes_big(stream_big, emitter.particle_green);
                emitter.particle_blue_is_null = read_track_nodes_big(stream_big, emitter.particle_blue);
                emitter.particle_alpha_is_null = read_track_nodes_big(stream_big, emitter.particle_alpha);
                emitter.particle_brightness_is_null = read_track_nodes_big(stream_big, emitter.particle_brightness);
                emitter.particle_spin_angle_is_null = read_track_nodes_big(stream_big, emitter.particle_spin_angle);
                emitter.particle_spin_speed_is_null = read_track_nodes_big(stream_big, emitter.particle_spin_speed);
                emitter.particle_scale_is_null = read_track_nodes_big(stream_big, emitter.particle_scale);
                emitter.particle_stretch_is_null = read_track_nodes_big(stream_big, emitter.particle_stretch);
                emitter.collision_reflect_is_null = read_track_nodes_big(stream_big, emitter.collision_reflect);
                emitter.collision_spin_is_null = read_track_nodes_big(stream_big, emitter.collision_spin);
                emitter.clip_top_is_null = read_track_nodes_big(stream_big, emitter.clip_top);
                emitter.clip_bottom_is_null = read_track_nodes_big(stream_big, emitter.clip_bottom);
                emitter.clip_left_is_null = read_track_nodes_big(stream_big, emitter.clip_left);
                emitter.clip_right_is_null = read_track_nodes_big(stream_big, emitter.clip_right);
                emitter.animation_rate_is_null = read_track_nodes_big(stream_big, emitter.animation_rate);
            }
            return;
        }

    public:
        explicit Decode(
            std::string_view source) : stream(source)
        {
        }

        explicit Decode(
            std::vector<uint8_t> const &it) : stream(it)
        {
        }

        ~Decode(

            ) = default;

        inline auto process(
            Particles &definition,
            ParticlesPlatform const &platform) -> void
        {
            switch (platform)
            {
            case ParticlesPlatform::PC_Compile:
            {
                decode_pc(definition);
                return;
            }
            case ParticlesPlatform::TV_Compile:
            {
                decode_tv(definition);
                return;
            }
            case ParticlesPlatform::Phone32_Compile:
            {
                decode_phone_32(definition);
                return;
            }
            case ParticlesPlatform::Phone64_Compile:
            {
                decode_phone_64(definition);
                return;
            }
            case ParticlesPlatform::GameConsole_Compile:
            {
                decode_game_console(definition);
                return;
            }
            default:
                assert_conditional(false, fmt::format("{}", Language::get("popcap.particles.decode.invalid_particles_platform")), "process");
            }
            return;
        }

        inline static auto process_fs(
            std::string_view source,
            std::string_view destination,
            ParticlesPlatform platform) -> void
        {
            auto decode = Decode{source};
            auto definition = Particles{};
            decode.process(definition, platform);
            FileSystem::write_json(destination, definition);
            return;
        }
    };
}
