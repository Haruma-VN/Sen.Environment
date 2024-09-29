#pragma once

#include "kernel/definition/utility.hpp"
#include "kernel/support/popcap/particles/definition.hpp"
#include "kernel/support/popcap/particles/common.hpp"
#include "kernel/support/popcap/zlib/compress.hpp"

namespace Sen::Kernel::Support::PopCap::Particles
{
	using namespace Definition;

	class Encode : Common
	{
	private:
		DataStreamView stream;

		template <auto UseVariant>
		inline auto encode_zlib(
			const std::vector<unsigned char> &data
		) -> std::vector<unsigned char>
		{
			static_assert(sizeof(UseVariant) == sizeof(bool));
			static_assert(UseVariant == true or UseVariant == false);
			auto compress = Sen::Kernel::Support::PopCap::Zlib::Compress<UseVariant>{};
            return compress.compress(data);
		}

		inline auto write_track_nodes(
			const std::vector<ParticlesTrackNode> &data) -> void
		{
			stream.writeInt32(static_cast<int32_t>(data.size()));
			for (auto &node : data)
			{
				stream.writeFloat(node.time);
				stream.writeFloat(node.low_value_is_null ? 0.0 : node.low_value);
				stream.writeFloat(node.high_value_is_null ? 0.0 : node.high_value);
				stream.writeInt32(node.curve_type_is_null ? 0x1 : node.curve_type);
				stream.writeInt32(node.distribution_is_null ? 0x1 : node.distribution);
			}
			return;
		}

		inline auto write_track_nodes_big(
			DataStreamViewBigEndian &stream_big,
			const std::vector<ParticlesTrackNode> &data) -> void
		{
			stream_big.writeInt32(static_cast<int32_t>(data.size()));
			for (auto &node : data)
			{
				stream_big.writeFloat(node.time);
				stream_big.writeFloat(node.low_value_is_null ? 0.0 : node.low_value);
				stream_big.writeFloat(node.high_value_is_null ? 0.0 : node.high_value);
				stream_big.writeInt32(node.curve_type_is_null ? 0x1 : node.curve_type);
				stream_big.writeInt32(node.distribution_is_null ? 0x1 : node.distribution);
			}
			return;
		}

		inline auto encode_pc(
			Particles const &definition) -> void
		{
			stream.writeInt32(k_32bit_magic);
			stream.writeInt32(0x0);
			stream.writeInt32(static_cast<int32_t>(definition.emitters.size()));
			stream.writeInt32(k_32bit_block_byte_1);
			for (auto &emitter : definition.emitters)
			{
				stream.writeInt32(0x0);
				stream.writeInt32(emitter.image_cols_is_null ? 0x0 : emitter.image_cols);
				stream.writeInt32(emitter.image_rows_is_null ? 0x0 : emitter.image_rows);
				stream.writeInt32(emitter.image_frames_is_null ? 0x1 : emitter.image_frames);
				stream.writeInt32(emitter.animated_is_null ? 0x0 : emitter.animated);
				stream.writeInt32(emitter.particles_flag);
				stream.writeInt32(emitter.emitter_type_is_null ? 0x1 : emitter.emitter_type);
				stream.writeNull(188_size);
				stream.writeInt32(static_cast<int32_t>(emitter.field.size()));
				stream.writeInt32(0x0);
				stream.writeInt32(static_cast<int32_t>(emitter.system_field.size()));
				stream.writeNull(128_size);
			}
			for (auto &emitter : definition.emitters)
			{
				stream.writeStringByInt32(emitter.image);
				stream.writeStringByInt32(emitter.name);
				write_track_nodes(emitter.system_duration);
				stream.writeStringByInt32(emitter.on_duration);
				write_track_nodes(emitter.cross_fade_duration);
				write_track_nodes(emitter.spawn_rate);
				write_track_nodes(emitter.spawn_min_active);
				write_track_nodes(emitter.spawn_max_active);
				write_track_nodes(emitter.spawn_max_launched);
				write_track_nodes(emitter.emitter_radius);
				write_track_nodes(emitter.emitter_offset_x);
				write_track_nodes(emitter.emitter_offset_y);
				write_track_nodes(emitter.emitter_box_x);
				write_track_nodes(emitter.emitter_box_y);
				write_track_nodes(emitter.emitter_path);
				write_track_nodes(emitter.emitter_skew_x);
				write_track_nodes(emitter.emitter_skew_y);
				write_track_nodes(emitter.particle_duration);
				write_track_nodes(emitter.system_red);
				write_track_nodes(emitter.system_green);
				write_track_nodes(emitter.system_blue);
				write_track_nodes(emitter.system_alpha);
				write_track_nodes(emitter.system_brightness);
				write_track_nodes(emitter.launch_speed);
				write_track_nodes(emitter.launch_angle);
				stream.writeInt32(k_32bit_block_byte_2);
				if (!emitter.field_is_null)
				{
					for (auto &field : emitter.field)
					{
						stream.writeInt32(field.field_type_is_null ? 0x0 : field.field_type);
						stream.writeNull(16_size);
					}
					for (auto &field : emitter.field)
					{
						write_track_nodes(field.x);
						write_track_nodes(field.y);
					}
				}
				stream.writeInt32(k_32bit_block_byte_2);
				if (!emitter.system_field_is_null)
				{
					for (auto &field : emitter.system_field)
					{
						stream.writeInt32(field.field_type_is_null ? 0x0 : field.field_type);
						stream.writeNull(16_size);
					}
					for (auto &field : emitter.system_field)
					{
						write_track_nodes(field.x);
						write_track_nodes(field.y);
					}
				}
				write_track_nodes(emitter.particle_red);
				write_track_nodes(emitter.particle_green);
				write_track_nodes(emitter.particle_blue);
				write_track_nodes(emitter.particle_alpha);
				write_track_nodes(emitter.particle_brightness);
				write_track_nodes(emitter.particle_spin_angle);
				write_track_nodes(emitter.particle_spin_speed);
				write_track_nodes(emitter.particle_scale);
				write_track_nodes(emitter.particle_stretch);
				write_track_nodes(emitter.collision_reflect);
				write_track_nodes(emitter.collision_spin);
				write_track_nodes(emitter.clip_top);
				write_track_nodes(emitter.clip_bottom);
				write_track_nodes(emitter.clip_left);
				write_track_nodes(emitter.clip_right);
				write_track_nodes(emitter.animation_rate);
			}
			auto data = encode_zlib<false>(stream.toBytes());
			stream.close();
			stream.writeBytes(data);
			return;
		}

		inline auto encode_tv(
			Particles const &definition) -> void
		{
			stream.writeInt32(0x0);
			stream.writeInt32(0x0);
			stream.writeInt32(static_cast<int32_t>(definition.emitters.size()));
			stream.writeInt32(k_32bit_block_byte_1);
			for (auto &emitter : definition.emitters)
			{
				stream.writeInt32(0x0);
				stream.writeInt32(emitter.image_cols_is_null ? 0x0 : emitter.image_cols);
				stream.writeInt32(emitter.image_rows_is_null ? 0x0 : emitter.image_rows);
				stream.writeInt32(emitter.image_frames_is_null ? 0x1 : emitter.image_frames);
				stream.writeInt32(emitter.animated_is_null ? 0x0 : emitter.animated);
				stream.writeInt32(emitter.particles_flag);
				stream.writeInt32(emitter.emitter_type_is_null ? 0x1 : emitter.emitter_type);
				stream.writeNull(188_size);
				stream.writeInt32(static_cast<int32_t>(emitter.field.size()));
				stream.writeInt32(0x0);
				stream.writeInt32(static_cast<int32_t>(emitter.system_field.size()));
				stream.writeNull(128_size);
			}
			for (auto &emitter : definition.emitters)
			{
				stream.writeStringByInt32(emitter.image);
				stream.writeStringByInt32(emitter.image_path);
				stream.writeStringByInt32(emitter.name);
				write_track_nodes(emitter.system_duration);
				stream.writeStringByInt32(emitter.on_duration);
				write_track_nodes(emitter.cross_fade_duration);
				write_track_nodes(emitter.spawn_rate);
				write_track_nodes(emitter.spawn_min_active);
				write_track_nodes(emitter.spawn_max_active);
				write_track_nodes(emitter.spawn_max_launched);
				write_track_nodes(emitter.emitter_radius);
				write_track_nodes(emitter.emitter_offset_x);
				write_track_nodes(emitter.emitter_offset_y);
				write_track_nodes(emitter.emitter_box_x);
				write_track_nodes(emitter.emitter_box_y);
				write_track_nodes(emitter.emitter_path);
				write_track_nodes(emitter.emitter_skew_x);
				write_track_nodes(emitter.emitter_skew_y);
				write_track_nodes(emitter.particle_duration);
				write_track_nodes(emitter.system_red);
				write_track_nodes(emitter.system_green);
				write_track_nodes(emitter.system_blue);
				write_track_nodes(emitter.system_alpha);
				write_track_nodes(emitter.system_brightness);
				write_track_nodes(emitter.launch_speed);
				write_track_nodes(emitter.launch_angle);
				stream.writeInt32(k_32bit_block_byte_2);
				if (!emitter.field_is_null)
				{
					for (auto &field : emitter.field)
					{
						stream.writeInt32(field.field_type_is_null ? 0x0 : field.field_type);
						stream.writeNull(16_size);
					}
					for (auto &field : emitter.field)
					{
						write_track_nodes(field.x);
						write_track_nodes(field.y);
					}
				}
				stream.writeInt32(k_32bit_block_byte_2);
				if (!emitter.system_field_is_null)
				{
					for (auto &field : emitter.system_field)
					{
						stream.writeInt32(field.field_type_is_null ? 0x0 : field.field_type);
						stream.writeNull(16_size);
					}
					for (auto &field : emitter.system_field)
					{
						write_track_nodes(field.x);
						write_track_nodes(field.y);
					}
				}
				write_track_nodes(emitter.particle_red);
				write_track_nodes(emitter.particle_green);
				write_track_nodes(emitter.particle_blue);
				write_track_nodes(emitter.particle_alpha);
				write_track_nodes(emitter.particle_brightness);
				write_track_nodes(emitter.particle_spin_angle);
				write_track_nodes(emitter.particle_spin_speed);
				write_track_nodes(emitter.particle_scale);
				write_track_nodes(emitter.particle_stretch);
				write_track_nodes(emitter.collision_reflect);
				write_track_nodes(emitter.collision_spin);
				write_track_nodes(emitter.clip_top);
				write_track_nodes(emitter.clip_bottom);
				write_track_nodes(emitter.clip_left);
				write_track_nodes(emitter.clip_right);
				write_track_nodes(emitter.animation_rate);
			}
			auto data = encode_zlib<false>(stream.toBytes());
			stream.close();
			stream.writeBytes(data);
			return;
		}

		inline auto encode_phone_32(
			Particles const &definition) -> void
		{
			stream.writeInt32(k_32bit_magic);
			stream.writeInt32(0x0);
			stream.writeInt32(static_cast<int32_t>(definition.emitters.size()));
			stream.writeInt32(k_32bit_block_byte_1);
			for (auto &emitter : definition.emitters)
			{
				stream.writeInt32(0x0);
				stream.writeInt32(emitter.image_cols_is_null ? 0x0 : emitter.image_cols);
				stream.writeInt32(emitter.image_rows_is_null ? 0x0 : emitter.image_rows);
				stream.writeInt32(emitter.image_frames_is_null ? 0x1 : emitter.image_frames);
				stream.writeInt32(emitter.animated_is_null ? 0x0 : emitter.animated);
				stream.writeInt32(emitter.particles_flag);
				stream.writeInt32(emitter.emitter_type_is_null ? 0x1 : emitter.emitter_type);
				stream.writeNull(188_size);
				stream.writeInt32(static_cast<int32_t>(emitter.field.size()));
				stream.writeInt32(0x0);
				stream.writeInt32(static_cast<int32_t>(emitter.system_field.size()));
				stream.writeNull(128_size);
			}
			for (auto &emitter : definition.emitters)
			{
				stream.writeInt32(std::stoi(emitter.image));
				stream.writeStringByInt32(emitter.name);
				write_track_nodes(emitter.system_duration);
				stream.writeStringByInt32(emitter.on_duration);
				write_track_nodes(emitter.cross_fade_duration);
				write_track_nodes(emitter.spawn_rate);
				write_track_nodes(emitter.spawn_min_active);
				write_track_nodes(emitter.spawn_max_active);
				write_track_nodes(emitter.spawn_max_launched);
				write_track_nodes(emitter.emitter_radius);
				write_track_nodes(emitter.emitter_offset_x);
				write_track_nodes(emitter.emitter_offset_y);
				write_track_nodes(emitter.emitter_box_x);
				write_track_nodes(emitter.emitter_box_y);
				write_track_nodes(emitter.emitter_path);
				write_track_nodes(emitter.emitter_skew_x);
				write_track_nodes(emitter.emitter_skew_y);
				write_track_nodes(emitter.particle_duration);
				write_track_nodes(emitter.system_red);
				write_track_nodes(emitter.system_green);
				write_track_nodes(emitter.system_blue);
				write_track_nodes(emitter.system_alpha);
				write_track_nodes(emitter.system_brightness);
				write_track_nodes(emitter.launch_speed);
				write_track_nodes(emitter.launch_angle);
				stream.writeInt32(k_32bit_block_byte_2);
				if (!emitter.field_is_null)
				{
					for (auto &field : emitter.field)
					{
						stream.writeInt32(field.field_type_is_null ? 0x0 : field.field_type);
						stream.writeNull(16_size);
					}
					for (auto &field : emitter.field)
					{
						write_track_nodes(field.x);
						write_track_nodes(field.y);
					}
				}
				stream.writeInt32(k_32bit_block_byte_2);
				if (!emitter.system_field_is_null)
				{
					for (auto &field : emitter.system_field)
					{
						stream.writeInt32(field.field_type_is_null ? 0x0 : field.field_type);
						stream.writeNull(16_size);
					}
					for (auto &field : emitter.system_field)
					{
						write_track_nodes(field.x);
						write_track_nodes(field.y);
					}
				}
				write_track_nodes(emitter.particle_red);
				write_track_nodes(emitter.particle_green);
				write_track_nodes(emitter.particle_blue);
				write_track_nodes(emitter.particle_alpha);
				write_track_nodes(emitter.particle_brightness);
				write_track_nodes(emitter.particle_spin_angle);
				write_track_nodes(emitter.particle_spin_speed);
				write_track_nodes(emitter.particle_scale);
				write_track_nodes(emitter.particle_stretch);
				write_track_nodes(emitter.collision_reflect);
				write_track_nodes(emitter.collision_spin);
				write_track_nodes(emitter.clip_top);
				write_track_nodes(emitter.clip_bottom);
				write_track_nodes(emitter.clip_left);
				write_track_nodes(emitter.clip_right);
				write_track_nodes(emitter.animation_rate);
			}
			auto data = encode_zlib<false>(stream.toBytes());
			stream.close();
			stream.writeBytes(data);
			return;
		}

		inline auto encode_phone_64(
			Particles const &definition) -> void
		{
			stream.writeInt32(k_64bit_magic);
			stream.writeInt32(0x0);
			stream.writeInt32(0x0);
			stream.writeInt32(static_cast<int32_t>(definition.emitters.size()));
			stream.writeInt32(0x0);
			stream.writeInt32(k_64bit_block_byte_1);
			for (auto &emitter : definition.emitters)
			{
				stream.writeInt32(0x0);
				stream.writeInt32(0x0);
				stream.writeInt32(emitter.image_cols_is_null ? 0x0 : emitter.image_cols);
				stream.writeInt32(emitter.image_rows_is_null ? 0x0 : emitter.image_rows);
				stream.writeInt32(emitter.image_frames_is_null ? 0x1 : emitter.image_frames);
				stream.writeInt32(emitter.animated_is_null ? 0x0 : emitter.animated);
				stream.writeInt32(emitter.particles_flag);
				stream.writeInt32(emitter.emitter_type_is_null ? 0x1 : emitter.emitter_type);
				stream.writeNull(376_size);
				stream.writeInt32(static_cast<int32_t>(emitter.field.size()));
				stream.writeNull(12_size);
				stream.writeInt32(static_cast<int32_t>(emitter.system_field.size()));
				stream.writeNull(260_size);
			}
			for (auto &emitter : definition.emitters)
			{
				stream.writeInt32(std::stoi(emitter.image));
				stream.writeStringByInt32(emitter.name);
				write_track_nodes(emitter.system_duration);
				stream.writeStringByInt32(emitter.on_duration);
				write_track_nodes(emitter.cross_fade_duration);
				write_track_nodes(emitter.spawn_rate);
				write_track_nodes(emitter.spawn_min_active);
				write_track_nodes(emitter.spawn_max_active);
				write_track_nodes(emitter.spawn_max_launched);
				write_track_nodes(emitter.emitter_radius);
				write_track_nodes(emitter.emitter_offset_x);
				write_track_nodes(emitter.emitter_offset_y);
				write_track_nodes(emitter.emitter_box_x);
				write_track_nodes(emitter.emitter_box_y);
				write_track_nodes(emitter.emitter_path);
				write_track_nodes(emitter.emitter_skew_x);
				write_track_nodes(emitter.emitter_skew_y);
				write_track_nodes(emitter.particle_duration);
				write_track_nodes(emitter.system_red);
				write_track_nodes(emitter.system_green);
				write_track_nodes(emitter.system_blue);
				write_track_nodes(emitter.system_alpha);
				write_track_nodes(emitter.system_brightness);
				write_track_nodes(emitter.launch_speed);
				write_track_nodes(emitter.launch_angle);
				stream.writeInt32(k_32bit_block_byte_2);
				if (!emitter.field_is_null)
				{
					for (auto &field : emitter.field)
					{
						stream.writeInt32(field.field_type_is_null ? 0x0 : field.field_type);
						stream.writeNull(36_size);
					}
					for (auto &field : emitter.field)
					{
						write_track_nodes(field.x);
						write_track_nodes(field.y);
					}
				}
				stream.writeInt32(k_32bit_block_byte_2);
				if (!emitter.system_field_is_null)
				{
					for (auto &field : emitter.system_field)
					{
						stream.writeInt32(field.field_type_is_null ? 0x0 : field.field_type);
						stream.writeNull(36_size);
					}
					for (auto &field : emitter.system_field)
					{
						write_track_nodes(field.x);
						write_track_nodes(field.y);
					}
				}
				write_track_nodes(emitter.particle_red);
				write_track_nodes(emitter.particle_green);
				write_track_nodes(emitter.particle_blue);
				write_track_nodes(emitter.particle_alpha);
				write_track_nodes(emitter.particle_brightness);
				write_track_nodes(emitter.particle_spin_angle);
				write_track_nodes(emitter.particle_spin_speed);
				write_track_nodes(emitter.particle_scale);
				write_track_nodes(emitter.particle_stretch);
				write_track_nodes(emitter.collision_reflect);
				write_track_nodes(emitter.collision_spin);
				write_track_nodes(emitter.clip_top);
				write_track_nodes(emitter.clip_bottom);
				write_track_nodes(emitter.clip_left);
				write_track_nodes(emitter.clip_right);
				write_track_nodes(emitter.animation_rate);
			}
			auto data = encode_zlib<false>(stream.toBytes());
			stream.close();
			stream.writeBytes(data);
			return;
		}

		inline auto encode_game_console(
			Particles const &definition) -> void
		{
			auto stream_big = DataStreamViewBigEndian{};
			stream_big.writeInt32(k_32bit_magic);
			stream_big.writeInt32(0x0);
			stream_big.writeInt32(static_cast<int32_t>(definition.emitters.size()));
			stream_big.writeInt32(k_32bit_block_byte_1);
			for (auto &emitter : definition.emitters)
			{
				stream_big.writeInt32(0x0);
				stream_big.writeInt32(emitter.image_cols_is_null ? 0x0 : emitter.image_cols);
				stream_big.writeInt32(emitter.image_rows_is_null ? 0x0 : emitter.image_rows);
				stream_big.writeInt32(emitter.image_frames_is_null ? 0x1 : emitter.image_frames);
				stream_big.writeInt32(emitter.animated_is_null ? 0x0 : emitter.animated);
				stream_big.writeInt32(emitter.particles_flag);
				stream_big.writeInt32(emitter.emitter_type_is_null ? 0x1 : emitter.emitter_type);
				stream_big.writeNull(188_size);
				stream_big.writeInt32(static_cast<int32_t>(emitter.field.size()));
				stream_big.writeInt32(0x0);
				stream_big.writeInt32(static_cast<int32_t>(emitter.system_field.size()));
				stream_big.writeNull(128_size);
			}
			for (auto &emitter : definition.emitters)
			{
				stream_big.writeStringByInt32(emitter.image);
				stream_big.writeStringByInt32(emitter.name);
				write_track_nodes_big(stream_big, emitter.system_duration);
				stream_big.writeStringByInt32(emitter.on_duration);
				write_track_nodes_big(stream_big, emitter.cross_fade_duration);
				write_track_nodes_big(stream_big, emitter.spawn_rate);
				write_track_nodes_big(stream_big, emitter.spawn_min_active);
				write_track_nodes_big(stream_big, emitter.spawn_max_active);
				write_track_nodes_big(stream_big, emitter.spawn_max_launched);
				write_track_nodes_big(stream_big, emitter.emitter_radius);
				write_track_nodes_big(stream_big, emitter.emitter_offset_x);
				write_track_nodes_big(stream_big, emitter.emitter_offset_y);
				write_track_nodes_big(stream_big, emitter.emitter_box_x);
				write_track_nodes_big(stream_big, emitter.emitter_box_y);
				write_track_nodes_big(stream_big, emitter.emitter_path);
				write_track_nodes_big(stream_big, emitter.emitter_skew_x);
				write_track_nodes_big(stream_big, emitter.emitter_skew_y);
				write_track_nodes_big(stream_big, emitter.particle_duration);
				write_track_nodes_big(stream_big, emitter.system_red);
				write_track_nodes_big(stream_big, emitter.system_green);
				write_track_nodes_big(stream_big, emitter.system_blue);
				write_track_nodes_big(stream_big, emitter.system_alpha);
				write_track_nodes_big(stream_big, emitter.system_brightness);
				write_track_nodes_big(stream_big, emitter.launch_speed);
				write_track_nodes_big(stream_big, emitter.launch_angle);
				stream_big.writeInt32(k_32bit_block_byte_2);
				if (!emitter.field_is_null)
				{
					for (auto &field : emitter.field)
					{
						stream_big.writeInt32(field.field_type_is_null ? 0x0 : field.field_type);
						stream_big.writeNull(16_size);
					}
					for (auto &field : emitter.field)
					{
						write_track_nodes_big(stream_big, field.x);
						write_track_nodes_big(stream_big, field.y);
					}
				}
				stream_big.writeInt32(k_32bit_block_byte_2);
				if (!emitter.system_field_is_null)
				{
					for (auto &field : emitter.system_field)
					{
						stream_big.writeInt32(field.field_type_is_null ? 0x0 : field.field_type);
						stream_big.writeNull(16_size);
					}
					for (auto &field : emitter.system_field)
					{
						write_track_nodes_big(stream_big, field.x);
						write_track_nodes_big(stream_big, field.y);
					}
				}
				write_track_nodes_big(stream_big, emitter.particle_red);
				write_track_nodes_big(stream_big, emitter.particle_green);
				write_track_nodes_big(stream_big, emitter.particle_blue);
				write_track_nodes_big(stream_big, emitter.particle_alpha);
				write_track_nodes_big(stream_big, emitter.particle_brightness);
				write_track_nodes_big(stream_big, emitter.particle_spin_angle);
				write_track_nodes_big(stream_big, emitter.particle_spin_speed);
				write_track_nodes_big(stream_big, emitter.particle_scale);
				write_track_nodes_big(stream_big, emitter.particle_stretch);
				write_track_nodes_big(stream_big, emitter.collision_reflect);
				write_track_nodes_big(stream_big, emitter.collision_spin);
				write_track_nodes_big(stream_big, emitter.clip_top);
				write_track_nodes_big(stream_big, emitter.clip_bottom);
				write_track_nodes_big(stream_big, emitter.clip_left);
				write_track_nodes_big(stream_big, emitter.clip_right);
				write_track_nodes_big(stream_big, emitter.animation_rate);
			}
			stream.writeBytes(stream_big.toBytes());
			return;
		}

	public:
		explicit Encode(

			) = default;

		~Encode(

			) = default;

		inline auto process(
			Particles const &definition,
			ParticlesPlatform platform) -> void
		{
			switch (platform)
			{
			case ParticlesPlatform::PC_Compile:
			{
				encode_pc(definition);
				return;
			}
			case ParticlesPlatform::TV_Compile:
			{
				encode_tv(definition);
				return;
			}
			case ParticlesPlatform::Phone32_Compile:
			{
				encode_phone_32(definition);
				return;
			}
			case ParticlesPlatform::Phone64_Compile:
			{
				encode_phone_64(definition);
				return;
			}
			case ParticlesPlatform::GameConsole_Compile:
			{
				encode_game_console(definition);
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
			auto encode = Encode{};
			auto definition = *FileSystem::read_json(source);
			encode.process(definition, platform);
			encode.stream.out_file(destination);
			return;
		}
	};
}
