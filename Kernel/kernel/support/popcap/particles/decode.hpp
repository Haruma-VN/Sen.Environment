#pragma once

#include "kernel/support/popcap/particles/definition.hpp"
#include "kernel/support/popcap/particles/version.hpp"
#include "kernel/support/popcap/particles/common.hpp"
#include "kernel/support/popcap/zlib/uncompress.hpp"

namespace Sen::Kernel::Support::PopCap::Particles {

	#pragma region using

	// using DataStreamView

	using Sen::Kernel::Definition::DataStreamView;


	#pragma endregion

	class Decode : public Common {

		protected:

			using Particles = Sen::Kernel::Support::PopCap::Particles::Definition::Particles;

			using Common = Common;

			using ParticlesField = Sen::Kernel::Support::PopCap::Particles::Definition::ParticlesField;

			using ParticlesTrackNode = Sen::Kernel::Support::PopCap::Particles::Definition::ParticlesTrackNode;

			using Uncompress = Support::PopCap::Zlib::Uncompress;

			using ParticlesEmitter = Sen::Kernel::Support::PopCap::Particles::Definition::ParticlesEmitter;

		public:

			std::unique_ptr<DataStreamView> sen;

			Particles particle;

		protected:

			template <auto uncompress_zlib, typename ...Args> requires (std::is_same<Args, bool>::value && ...)
			inline auto process(
				Args... use_64_bit_variant
			) -> void
			{
				static_assert(uncompress_zlib == true || uncompress_zlib == false, "Uncompress zlib can only be true or false");
				static_assert(sizeof...(Args) == 1 || sizeof...(Args) == 0, "Argument size for use_64_bit_variant can only be 1 or 0");
				if constexpr (uncompress_zlib) {
					static_assert(sizeof...(Args) == 1, "Uncompress zlib is provided but no argument provided for use_64_bit_variant");
					{
						auto zlib = std::make_unique<Uncompress>(std::get<0>(std::make_tuple(use_64_bit_variant...)));
						auto raw_data = zlib->uncompress(sen->getBytes(static_cast<std::size_t>(0), sen->size()));
						sen->close();
						sen->writeBytes(raw_data);
						sen->change_read_pos(static_cast<std::size_t>(0x00));
					}
				}
				sen->change_read_pos(static_cast<std::size_t>(8));
				auto count = sen->readInt32();
				if (count == 0) {
					return;
				}
				particle.emitters_is_null = false;
				particle.emitters.reserve(static_cast<std::size_t>(count));
				if (sen->readInt32() != 0x164)
				{
					throw Exception(fmt::format("{}", Language::get("unsupported_particles")), std::source_location::current(), "process");
				}
				auto field_count = static_cast<std::size_t>(0);
				auto system_field_count = static_cast<std::size_t>(0);
				for (auto i : Range<int>(count))
				{
					particle.emitters.emplace_back(ParticlesEmitter());
					sen->read_pos += 4;
					auto tempInt = sen->readInt32();
					if (tempInt != 0) {
						particle.emitters[i].image_cols = tempInt;
						particle.emitters[i].image_cols_is_null = false;
					}
					tempInt = sen->readInt32();
					if (tempInt != 0) {
						particle.emitters[i].image_rows = tempInt;
						particle.emitters[i].image_rows_is_null = false;
					}
					tempInt = sen->readInt32();
					if (tempInt != 1) {
						particle.emitters[i].image_frames = tempInt;
						particle.emitters[i].image_frames_is_null = false;
					}
					tempInt = sen->readInt32();
					if (tempInt != 0) {
						particle.emitters[i].animated = tempInt;
						particle.emitters[i].animated_is_null = false;
					}
					particle.emitters[i].particles_flag = sen->readInt32();
					tempInt = sen->readInt32();
					if (tempInt != 1) {
						particle.emitters[i].emitter_type = tempInt;
						particle.emitters[i].emitter_type_is_null = false;
					}
					sen->read_pos += 188;
					tempInt = sen->readInt32();
					if (tempInt != 0) {
						particle.emitters[i].field_is_null = false;
						particle.emitters[i].field.reserve(static_cast<std::size_t>(tempInt));
						field_count = static_cast<std::size_t>(tempInt);
					}
					sen->read_pos += 4;
					tempInt = sen->readInt32();
					if (tempInt != 0) {
						particle.emitters[i].system_field_is_null = false;
						particle.emitters[i].system_field.reserve(static_cast<std::size_t>(tempInt));
						system_field_count = static_cast<std::size_t>(tempInt);
					}
					sen->read_pos += 128;
					for (auto i : Range<int>(count))
					{
						auto& emitter = particle.emitters[i];
						auto tempStr = sen->readStringByInt32();
						if (!tempStr.empty()) {
							emitter.image = std::stoi(tempStr);
							emitter.image_is_null = false;
						}
						tempStr = sen->readStringByInt32();
						if (!tempStr.empty()) {
							emitter.name = tempStr;
							emitter.name_is_null = false;
						}
						thiz.read_track_nodes(&emitter.system_duration);
						tempStr = sen->readStringByInt32();
						if (!tempStr.empty()) {
							emitter.on_duration = tempStr;
							emitter.on_duration_is_null = false;
						}
						thiz.read_track_nodes(&emitter.cross_fade_duration);
						if (!emitter.system_duration.empty()) {
							emitter.system_duration_is_null = false;
						}
						thiz.read_track_nodes(&emitter.spawn_rate);
						if (!emitter.spawn_rate.empty()) {
							emitter.spawn_rate_is_null = false;
						}
						thiz.read_track_nodes(&emitter.spawn_min_active);
						if (!emitter.spawn_min_active.empty()) {
							emitter.spawn_min_active_is_null = false;
						}
						thiz.read_track_nodes(&emitter.spawn_max_active);
						if (!emitter.spawn_max_active.empty()) {
							emitter.spawn_max_active_is_null = false;
						}
						thiz.read_track_nodes(&emitter.spawn_max_launched);
						if (!emitter.spawn_max_launched.empty()) {
							emitter.spawn_max_launched_is_null = false;
						}
						thiz.read_track_nodes(&emitter.emitter_radius);
						if (!emitter.emitter_radius.empty()) {
							emitter.emitter_radius_is_null = false;
						}
						thiz.read_track_nodes(&emitter.emitter_offset_x);
						if (!emitter.emitter_offset_x.empty()) {
							emitter.emitter_offset_x_is_null = false;
						}
						thiz.read_track_nodes(&emitter.emitter_offset_y);
						if (!emitter.emitter_offset_y.empty()) {
							emitter.emitter_offset_y_is_null = false;
						}
						thiz.read_track_nodes(&emitter.emitter_box_x);
						if (!emitter.emitter_box_x.empty()) {
							emitter.emitter_box_x_is_null = false;
						}
						thiz.read_track_nodes(&emitter.emitter_box_y);
						if (!emitter.emitter_box_y.empty()) {
							emitter.emitter_box_y_is_null = false;
						}
						thiz.read_track_nodes(&emitter.emitter_path);
						if (!emitter.emitter_path.empty()) {
							emitter.emitter_path_is_null = false;
						}
						thiz.read_track_nodes(&emitter.emitter_skew_x);
						if (!emitter.emitter_skew_x.empty()) {
							emitter.emitter_skew_x_is_null = false;
						}
						thiz.read_track_nodes(&emitter.emitter_skew_y);
						if (!emitter.emitter_skew_y.empty()) {
							emitter.emitter_skew_y_is_null = false;
						}
						thiz.read_track_nodes(&emitter.particle_duration);
						if (!emitter.particle_duration.empty()) {
							emitter.particle_duration_is_null = false;
						}
						thiz.read_track_nodes(&emitter.system_red);
						if (!emitter.system_red.empty()) {
							emitter.system_red_is_null = false;
						}
						thiz.read_track_nodes(&emitter.system_green);
						if (!emitter.system_green.empty()) {
							emitter.system_green_is_null = false;
						}
						thiz.read_track_nodes(&emitter.system_blue);
						if (!emitter.system_blue.empty()) {
							emitter.system_blue_is_null = false;
						}
						thiz.read_track_nodes(&emitter.system_alpha);
						if (!emitter.system_alpha.empty()) {
							emitter.system_alpha_is_null = false;
						}
						thiz.read_track_nodes(&emitter.system_brightness);
						if (!emitter.system_brightness.empty()) {
							emitter.system_brightness_is_null = false;
						}
						thiz.read_track_nodes(&emitter.launch_speed);
						if (!emitter.launch_speed.empty()) {
							emitter.launch_speed_is_null = false;
						}
						thiz.read_track_nodes(&emitter.launch_angle);
						if (!emitter.launch_angle.empty()) {
							emitter.launch_angle_is_null = false;
						}
						if (sen->readInt32() != 0x14)
						{
							throw Exception(fmt::format("{}", Language::get("")), std::source_location::current(), "process");
						}
						auto& fields = emitter.field;
						if (!emitter.field_is_null)
						{
							for (auto k : Range(field_count))
							{
								fields.emplace_back(ParticlesField{});
								auto& field = fields[k];
								auto type = sen->readInt32();
								if (type != 0) {
									field.field_type = type;
									field.field_type_is_null = false;
								}
								sen->read_pos += 16;
							}
							for (auto k : Range(field_count))
							{
								auto& field = fields[k];
								read_track_nodes(&field.x);
								if (!field.x.empty()) {
									field.x_is_null = false;
								}
								read_track_nodes(&field.y);
								if (!field.y.empty()) {
									field.y_is_null = false;
								}
							}
						}
						if (sen->readInt32() != 0x14)
						{
							throw Exception("undefined");
						}
						auto &system_fields = emitter.system_field;
						if (!emitter.system_field_is_null)
						{
							for (auto k : Range(system_field_count))
							{
								system_fields.emplace_back(ParticlesField{});
								auto& field = system_fields[k];
								auto type = sen->readInt32();
								if (type != 0)
								{
									field.field_type = type;
									field.field_type_is_null = false;
								}
								sen->read_pos += 16;
							}
							for (auto k : Range(system_field_count))
							{
								auto& field = fields[k];
								read_track_nodes(&field.x);
								if (!field.x.empty()) {
									field.x_is_null = false;
								}
								read_track_nodes(&field.y);
								if (!field.y.empty()) {
									field.y_is_null = false;
								}
							}
						}
						thiz.read_track_nodes(&emitter.particle_red);
						if (!emitter.particle_red.empty()) {
							emitter.particle_red_is_null = false;
						}
						thiz.read_track_nodes(&emitter.particle_green);
						if (!emitter.particle_green.empty()) {
							emitter.particle_green_is_null = false;
						}
						thiz.read_track_nodes(&emitter.particle_blue);
						if (!emitter.particle_blue.empty()) {
							emitter.particle_blue_is_null = false;
						}
						thiz.read_track_nodes(&emitter.particle_alpha);
						if (!emitter.particle_alpha.empty()) {
							emitter.particle_alpha_is_null = false;
						}
						thiz.read_track_nodes(&emitter.particle_brightness);
						if (!emitter.particle_brightness.empty()) {
							emitter.particle_brightness_is_null = false;
						}
						thiz.read_track_nodes(&emitter.particle_spin_angle);
						if (!emitter.particle_spin_angle.empty()) {
							emitter.particle_spin_angle_is_null = false;
						}
						thiz.read_track_nodes(&emitter.particle_spin_speed);
						if (!emitter.particle_spin_speed.empty()) {
							emitter.particle_spin_speed_is_null = false;
						}
						thiz.read_track_nodes(&emitter.particle_scale);
						if (!emitter.particle_scale.empty()) {
							emitter.particle_scale_is_null = false;
						}
						thiz.read_track_nodes(&emitter.particle_stretch);
						if (!emitter.particle_stretch.empty()) {
							emitter.particle_stretch_is_null = false;
						}
						thiz.read_track_nodes(&emitter.collision_reflect);
						if (!emitter.collision_reflect.empty()) {
							emitter.collision_reflect_is_null = false;
						}
						thiz.read_track_nodes(&emitter.collision_spin);
						if (!emitter.collision_spin.empty()) {
							emitter.collision_spin_is_null = false;
						}
						thiz.read_track_nodes(&emitter.clip_top);
						if (!emitter.clip_top.empty()) {
							emitter.clip_top_is_null = false;
						}
						thiz.read_track_nodes(&emitter.clip_bottom);
						if (!emitter.clip_bottom.empty()) {
							emitter.clip_bottom_is_null = false;
						}
						thiz.read_track_nodes(&emitter.clip_left);
						if (!emitter.clip_left.empty()) {
							emitter.clip_left_is_null = false;
						}
						thiz.read_track_nodes(&emitter.clip_right);
						if (!emitter.clip_right.empty()) {
							emitter.clip_right_is_null = false;
						}
						thiz.read_track_nodes(&emitter.animation_rate);
						if (!emitter.animation_rate.empty()) {
							emitter.animation_rate_is_null = false;
						}
					}
				}
				return;
			}

			inline auto read_track_nodes(
				std::vector<ParticlesTrackNode>* destination
			) const -> void
			{
				auto count = sen->readInt32();
				if (count == 0) {
					return;
				}
				for (auto i : Range<int>(count))
				{
					destination->emplace_back(ParticlesTrackNode{
						sen->readFloat(),
					});
					auto tempfloat = sen->readFloat();
					if (tempfloat != 0) {
						destination->at(i).low_value = tempfloat;
						destination->at(i).low_value_is_null = false;
					}
					tempfloat = sen->readFloat();
					if (tempfloat != 0) {
						destination->at(i).high_value = tempfloat;
						destination->at(i).high_value_is_null = false;
					}
					auto tempint = sen->readInt32();
					if (tempint != 1) {
						destination->at(i).curve_type = tempint;
						destination->at(i).curve_type_is_null = false;
					}
					tempint = sen->readInt32();
					if (tempint != 1) {
						destination->at(i).distribution = tempint;
						destination->at(i).distribution_is_null = false;
					}
				}
				return;
			}

		public:

			Decode(
				Decode &&that
			) = delete;

			Decode(
				DataStreamView & that
			) : sen(&that)
			{

			}

			Decode(
				std::string_view source
			) : sen(std::make_unique<DataStreamView>(source))
			{

			}

			~Decode(

			) = default;

			auto operator=(
				Decode &&that
			) -> Decode & = delete;

			template <auto uncompress_zlib>
			inline static auto process_fs(
				std::string_view source,
				std::string_view destination
			) -> void
			{
				auto decoder = Decode{ source };
				decoder.process<uncompress_zlib, bool>(false);
				FileSystem::write_json(destination, decoder.particle);
				return;
			}
	};
}
