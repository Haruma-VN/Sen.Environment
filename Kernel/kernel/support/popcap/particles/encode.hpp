#pragma once

#include "kernel/support/popcap/particles/definition.hpp"
#include "kernel/support/popcap/particles/version.hpp"
#include "kernel/support/popcap/particles/common.hpp"
#include "kernel/support/popcap/zlib/compress.hpp"

namespace Sen::Kernel::Support::PopCap::Particles {

	#pragma region using

	// using DataStreamView

	using Sen::Kernel::Definition::DataStreamView;


	#pragma endregion

	class Encode : public Common {

		protected:

			using Particles = Sen::Kernel::Support::PopCap::Particles::Definition::Particles;

			using Common = Common;

			using ParticlesField = Sen::Kernel::Support::PopCap::Particles::Definition::ParticlesField;

			using ParticlesTrackNode = Sen::Kernel::Support::PopCap::Particles::Definition::ParticlesTrackNode;

		public:

			std::unique_ptr<DataStreamView> sen;

			Particles particle;

		protected:

			template <auto compress_zlib>
			inline auto process(

			) const -> void
			{
				sen->writeInt32(Common::magic);
				sen->writeInt32(0);
				auto count = particle.emitters.size();
				sen->writeInt32(count);
				sen->writeInt32(Common::default_byte);
				if (particle.emitters_is_null) {
					return;
				}
				for (auto & emitter : particle.emitters)
				{
					sen->writeInt32(0);
					if (emitter.image_cols_is_null) {
						sen->writeInt32(0);
					}
					else {
						sen->writeInt32(emitter.image_cols);
					}
					if (emitter.image_rows_is_null) {
						sen->writeInt32(0);
					}
					else {
						sen->writeInt32(emitter.image_rows);
					}
					if (emitter.image_frames_is_null) {
						sen->writeInt32(1);
					}
					else {
						sen->writeInt32(emitter.image_frames);
					}
					if (emitter.animated_is_null) {
						sen->writeInt32(0);
					}
					else {
						sen->writeInt32(emitter.animated);
					}
					sen->writeInt32(emitter.particles_flag);
					if (emitter.emitter_type_is_null) {
						sen->writeInt32(1);
					}
					else {
						sen->writeInt32(emitter.emitter_type);
					}
					sen->writeInt32(0);
					sen->writeInt32(0);
					for (auto j : Range<int>(22))
					{
						sen->writeInt32(0);
						sen->writeInt32(0);
					}
					sen->writeInt32(0);
					if (emitter.field_is_null) {
						sen->writeInt32(0);
					}
					else {
						sen->writeInt32(emitter.field.size());
					}
					sen->writeInt32(0);
					if (emitter.system_field_is_null) {
						sen->writeInt32(0);
					}
					else {
						sen->writeInt32(emitter.system_field.size());
					}
					for (auto j = 24; j < 40; ++j)
					{
						sen->writeInt32(0);
						sen->writeInt32(0);
					}
				}
				for (auto & emitter : particle.emitters)
				{
					sen->writeStringByEmpty(std::to_string(emitter.image));
					sen->writeStringByEmpty(emitter.name);
					write_track_nodes(emitter.system_duration);
					sen->writeStringByEmpty(emitter.on_duration);
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
					write_fields(emitter.field);
					write_fields(emitter.system_field);
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
				if constexpr (compress_zlib) {
					auto zlib = std::make_unique<PopCap::Zlib::Compress>(false);
					auto bytes = zlib->compress(sen->getBytes(static_cast<std::size_t>(0), static_cast<std::size_t>(sen->size())));
					sen->close();
					sen->writeBytes(bytes);
				}
				return;
			}

			inline auto write_fields(
				const std::vector<ParticlesField>& fields
			) const -> void
			{
				sen->writeInt32(0x14);
				if (fields.empty())
				{
					return;
				}
				for (auto i : Range<std::size_t>(fields.size()))
				{
					if (fields[i].field_type_is_null) {
						sen->writeInt32(0);
					}
					else {
						sen->writeInt32(fields[i].field_type);
					}
					sen->writeInt32(0);
					sen->writeInt32(0);
					sen->writeInt32(0);
					sen->writeInt32(0);
				}
				for (auto i : Range<std::size_t>(fields.size()))
				{
					write_track_nodes(fields[i].x);
					write_track_nodes(fields[i].y);
				}
				return;
			}

			inline auto write_track_nodes(
				const std::vector<ParticlesTrackNode> & nodes
			) const -> void
			{
				if (nodes.empty())
				{
					sen->writeInt32(0);
					return;
				}
				sen->writeInt32(static_cast<std::uint32_t>(nodes.size()));
				for (auto & node : nodes)
				{
					sen->writeDouble(node.time);
					if (node.low_value_is_null) {
						sen->writeDouble(0.0f);
					}
					else {
						sen->writeDouble(node.low_value);
					}
					if (node.high_value_is_null) {
						sen->writeDouble(0.0f);
					}
					else {
						sen->writeDouble(node.high_value);
					}
					if (node.curve_type_is_null) {
						sen->writeInt32(1);
					}
					else {
						sen->writeInt32(node.curve_type);
					}
					if (node.distribution_is_null) {
						sen->writeInt32(1);
					}
					else {
						sen->writeInt32(node.distribution);
					}
				}
				return;
			}

		public:

			Encode(
				Encode &&that
			) = delete;

			Encode(
				DataStreamView & that
			) : sen(&that)
			{

			}

			Encode(
				std::string_view source
			) : particle(FileSystem::read_json(source)), sen(std::make_unique<DataStreamView>())
			{

			}

			~Encode(

			) = default;

			auto operator=(
				Encode &&that
			) -> Encode & = delete;

			template <auto compress_zlib>
			inline static auto process_fs(
				std::string_view source,
				std::string_view destination
			) -> void
			{
				auto encoder = Encode{source};
				encoder.process<compress_zlib>();
				encoder.sen->out_file(destination);
				return;
			}
	};
}
