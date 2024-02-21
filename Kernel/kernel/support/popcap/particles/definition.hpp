#pragma once

#include "kernel/definition/utility.hpp"

namespace Sen::Kernel::Support::PopCap::Particles::Definition {
	
	struct ParticlesTrackNode
	{
		float time;
		float low_value;
		bool low_value_is_null;
		float high_value;
		bool high_value_is_null;
		int curve_type;
		bool curve_type_is_null;
		int distribution;
		bool distribution_is_null;

		ParticlesTrackNode(

		) = default;

		ParticlesTrackNode(
			float time
		) : time(time), low_value_is_null(true), high_value_is_null(true), curve_type_is_null(true), distribution_is_null(true)
		{

		}

		~ParticlesTrackNode(

		) = default;
		
	};

	inline auto to_json(
		nlohmann::ordered_json& nlohmann_json_j, 
		const ParticlesTrackNode& nlohmann_json_t
	) -> void
	{
		nlohmann_json_j["time"] = nlohmann_json_t.time; 
		if (nlohmann_json_t.low_value_is_null) {
			nlohmann_json_j["low_value"] = nullptr;
		}
		else {
			nlohmann_json_j["low_value"] = nlohmann_json_t.low_value;
		}
		if (nlohmann_json_t.high_value_is_null) {
			nlohmann_json_j["high_value"] = nullptr;
		}
		else {
			nlohmann_json_j["high_value"] = nlohmann_json_t.high_value;
		}
		if (nlohmann_json_t.curve_type_is_null) {
			nlohmann_json_j["curve_type"] = nullptr;
		}
		else {
			nlohmann_json_j["curve_type"] = nlohmann_json_t.curve_type;
		}
		if (nlohmann_json_t.distribution_is_null) {
			nlohmann_json_j["distribution"] = nullptr;
		}
		else {
			nlohmann_json_j["distribution"] = nlohmann_json_t.distribution;
		}
		return;
	} 
	
	inline auto from_json(
		const nlohmann::ordered_json& nlohmann_json_j, 
		ParticlesTrackNode& nlohmann_json_t
	) -> void
	{
		nlohmann_json_j.at("time").get_to(nlohmann_json_t.time); 
		if (nlohmann_json_j.at("low_value").is_null()) {
			nlohmann_json_t.low_value_is_null = true;
		}
		else {
			nlohmann_json_j.at("low_value").get_to(nlohmann_json_t.low_value);
		}
		if (nlohmann_json_j.at("high_value").is_null()) {
			nlohmann_json_t.high_value_is_null = true;
		}
		else {
			nlohmann_json_j.at("high_value").get_to(nlohmann_json_t.high_value);
		}
		if (nlohmann_json_j.at("curve_type").is_null()) {
			nlohmann_json_t.curve_type_is_null = true;
		}
		else {
			nlohmann_json_j.at("curve_type").get_to(nlohmann_json_t.curve_type);
		}
		if (nlohmann_json_j.at("distribution").is_null()) {
			nlohmann_json_t.distribution_is_null = true;
		}
		else {
			nlohmann_json_j.at("distribution").get_to(nlohmann_json_t.distribution);
		}
		return;
	}

	struct ParticlesField
	{
		int field_type;
		bool field_type_is_null = true;
		std::vector<ParticlesTrackNode> x;
		bool x_is_null = true;
		std::vector<ParticlesTrackNode> y;
		bool y_is_null = true;

		ParticlesField(

		) = default;

		~ParticlesField(

		) = default;
	};

	inline auto to_json(
		nlohmann::ordered_json& nlohmann_json_j, 
		const ParticlesField& nlohmann_json_t
	) -> void
	{
		if (nlohmann_json_t.field_type_is_null) {
			nlohmann_json_j["field_type"] = nullptr;
		}
		else {
			nlohmann_json_j["field_type"] = nlohmann_json_t.field_type;
		}
		if (nlohmann_json_t.x_is_null) {
			nlohmann_json_j["x"] = nullptr;
		}
		else {
			nlohmann_json_j["x"] = nlohmann_json_t.x;
		}
		if (nlohmann_json_t.y_is_null) {
			nlohmann_json_j["y"] = nullptr;
		}
		else {
			nlohmann_json_j["y"] = nlohmann_json_t.y;
		}
		return;
	} 

	inline auto from_json(
		const nlohmann::ordered_json& nlohmann_json_j, 
		ParticlesField& nlohmann_json_t
	) -> void
	{
		if (nlohmann_json_j.at("field_type").is_null()) {
			nlohmann_json_t.field_type_is_null = true;
		}
		else {
			nlohmann_json_j.at("field_type").get_to(nlohmann_json_t.field_type);
		}
		if (nlohmann_json_j.at("x").is_null()) {
			nlohmann_json_t.x_is_null = true;
		}
		else {
			nlohmann_json_j.at("x").get_to(nlohmann_json_t.x);
		}
		if (nlohmann_json_j.at("y").is_null()) {
			nlohmann_json_t.y_is_null = true;
		}
		else {
			nlohmann_json_j.at("y").get_to(nlohmann_json_t.y);
		}
		return;
	}

	struct ParticlesEmitter
	{
		std::string name;
		bool name_is_null = true;
		int image;
		bool image_is_null = true;
		std::string image_path;
		bool image_path_is_null = true;
		int image_cols;
		bool image_cols_is_null = true;
		int image_rows;
		bool image_rows_is_null = true;
		int image_frames;
		bool image_frames_is_null = true;
		int animated;
		bool animated_is_null = true;
		int particles_flag = 0;
		int emitter_type;
		bool emitter_type_is_null = true;
		std::string on_duration;
		bool on_duration_is_null = true;
		std::vector<ParticlesTrackNode> system_duration;
		bool system_duration_is_null = true;
		std::vector<ParticlesTrackNode> cross_fade_duration;
		bool cross_fade_duration_is_null = true;
		std::vector<ParticlesTrackNode> spawn_rate;
		bool spawn_rate_is_null = true;
		std::vector<ParticlesTrackNode> spawn_min_active;
		bool spawn_min_active_is_null = true;
		std::vector<ParticlesTrackNode> spawn_max_active;
		bool spawn_max_active_is_null = true;
		std::vector<ParticlesTrackNode> spawn_max_launched;
		bool spawn_max_launched_is_null = true;
		std::vector<ParticlesTrackNode> emitter_radius;
		bool emitter_radius_is_null = true;
		std::vector<ParticlesTrackNode> emitter_offset_x;
		bool emitter_offset_x_is_null = true;
		std::vector<ParticlesTrackNode> emitter_offset_y;
		bool emitter_offset_y_is_null = true;
		std::vector<ParticlesTrackNode> emitter_box_x; 
		bool emitter_box_x_is_null = true;
		std::vector<ParticlesTrackNode> emitter_box_y; 
		bool emitter_box_y_is_null = true;
		std::vector<ParticlesTrackNode> emitter_path; 
		bool emitter_path_is_null = true;
		std::vector<ParticlesTrackNode> emitter_skew_x; 
		bool emitter_skew_x_is_null = true;
		std::vector<ParticlesTrackNode> emitter_skew_y; 
		bool emitter_skew_y_is_null = true;
		std::vector<ParticlesTrackNode> particle_duration; 
		bool particle_duration_is_null = true;
		std::vector<ParticlesTrackNode> system_red; 
		bool system_red_is_null = true;
		std::vector<ParticlesTrackNode> system_green; 
		bool system_green_is_null = true;
		std::vector<ParticlesTrackNode> system_blue; 
		bool system_blue_is_null = true;
		std::vector<ParticlesTrackNode> system_alpha; 
		bool system_alpha_is_null = true;
		std::vector<ParticlesTrackNode> system_brightness; 
		bool system_brightness_is_null = true;
		std::vector<ParticlesTrackNode> launch_speed; 
		bool launch_speed_is_null = true;
		std::vector<ParticlesTrackNode> launch_angle; 
		bool launch_angle_is_null = true;
		std::vector<ParticlesField> field; 
		bool field_is_null = true;
		std::vector<ParticlesField> system_field; 
		bool system_field_is_null = true;
		std::vector<ParticlesTrackNode> particle_red; 
		bool particle_red_is_null = true;
		std::vector<ParticlesTrackNode> particle_green; 
		bool particle_green_is_null = true;
		std::vector<ParticlesTrackNode> particle_blue; 
		bool particle_blue_is_null = true;
		std::vector<ParticlesTrackNode> particle_alpha; 
		bool particle_alpha_is_null = true;
		std::vector<ParticlesTrackNode> particle_brightness; 
		bool particle_brightness_is_null = true;
		std::vector<ParticlesTrackNode> particle_spin_angle; 
		bool particle_spin_angle_is_null = true;
		std::vector<ParticlesTrackNode> particle_spin_speed; 
		bool particle_spin_speed_is_null = true;
		std::vector<ParticlesTrackNode> particle_scale; 
		bool particle_scale_is_null = true;
		std::vector<ParticlesTrackNode> particle_stretch; 
		bool particle_stretch_is_null = true;
		std::vector<ParticlesTrackNode> collision_reflect; 
		bool collision_reflect_is_null = true;
		std::vector<ParticlesTrackNode> collision_spin; 
		bool collision_spin_is_null = true;
		std::vector<ParticlesTrackNode> clip_top; 
		bool clip_top_is_null = true;
		std::vector<ParticlesTrackNode> clip_bottom; 
		bool clip_bottom_is_null = true;
		std::vector<ParticlesTrackNode> clip_left;
		bool clip_left_is_null = true;
		std::vector<ParticlesTrackNode> clip_right;
		bool clip_right_is_null = true;
		std::vector<ParticlesTrackNode> animation_rate;
		bool animation_rate_is_null = true;

		ParticlesEmitter(

		) = default;

		~ParticlesEmitter(

		) = default;
	};


	inline auto to_json(
		nlohmann::ordered_json& nlohmann_json_j, 
		const ParticlesEmitter& nlohmann_json_t
	) -> void
	{ 
		if (nlohmann_json_t.name_is_null) {
			nlohmann_json_j["name"] = nullptr;
		}
		else {
			nlohmann_json_j["name"] = nlohmann_json_t.name;
		}
		if (nlohmann_json_t.image_is_null) {
			nlohmann_json_j["image"] = nullptr;
		}
		else {
			nlohmann_json_j["image"] = nlohmann_json_t.image;
		}
		if (nlohmann_json_t.image_path_is_null) {
			nlohmann_json_j["image_path"] = nullptr;
		}
		else {
			nlohmann_json_j["image_path"] = nlohmann_json_t.image_path;
		}
		if (nlohmann_json_t.image_cols_is_null) {
			nlohmann_json_j["image_cols"] = nullptr;
		}
		else {
			nlohmann_json_j["image_cols"] = nlohmann_json_t.image_cols;
		}
		if (nlohmann_json_t.image_rows_is_null) {
			nlohmann_json_j["image_rows"] = nullptr;
		}
		else {
			nlohmann_json_j["image_rows"] = nlohmann_json_t.image_rows;
		}
		if (nlohmann_json_t.image_frames_is_null) {
			nlohmann_json_j["image_frames"] = nullptr;
		}
		else {
			nlohmann_json_j["image_frames"] = nlohmann_json_t.image_frames;
		}
		if (nlohmann_json_t.animated_is_null) {
			nlohmann_json_j["animated"] = nullptr;
		}
		else {
			nlohmann_json_j["animated"] = nlohmann_json_t.animated;
		}
		nlohmann_json_j["particles_flag"] = nlohmann_json_t.particles_flag;
		if (nlohmann_json_t.emitter_type_is_null) {
			nlohmann_json_j["emitter_type"] = nullptr;
		}
		else {
			nlohmann_json_j["emitter_type"] = nlohmann_json_t.emitter_type;
		}
		if (nlohmann_json_t.on_duration_is_null) {
			nlohmann_json_j["on_duration"] = nullptr;
		}
		else {
			nlohmann_json_j["on_duration"] = nlohmann_json_t.on_duration;
		}
		if (nlohmann_json_t.system_duration_is_null) {
			nlohmann_json_j["system_duration"] = nullptr;
		}
		else {
			nlohmann_json_j["system_duration"] = nlohmann_json_t.system_duration;
		}
		if (nlohmann_json_t.cross_fade_duration_is_null) {
			nlohmann_json_j["cross_fade_duration"] = nullptr;
		}
		else {
			nlohmann_json_j["cross_fade_duration"] = nlohmann_json_t.cross_fade_duration;
		}
		if (nlohmann_json_t.spawn_rate_is_null) {
			nlohmann_json_j["spawn_rate"] = nullptr;
		}
		else {
			nlohmann_json_j["spawn_rate"] = nlohmann_json_t.spawn_rate;
		}
		if (nlohmann_json_t.spawn_min_active_is_null) {
			nlohmann_json_j["spawn_min_active"] = nullptr;
		}
		else {
			nlohmann_json_j["spawn_min_active"] = nlohmann_json_t.spawn_min_active;
		}
		if (nlohmann_json_t.spawn_max_active_is_null) {
			nlohmann_json_j["spawn_max_active"] = nullptr;
		}
		else {
			nlohmann_json_j["spawn_max_active"] = nlohmann_json_t.spawn_max_active;
		}
		if (nlohmann_json_t.spawn_max_launched_is_null) {
			nlohmann_json_j["spawn_max_launched"] = nullptr;
		}
		else {
			nlohmann_json_j["spawn_max_launched"] = nlohmann_json_t.spawn_max_launched;
		}
		if (nlohmann_json_t.emitter_radius_is_null) {
			nlohmann_json_j["emitter_radius"] = nullptr;
		}
		else {
			nlohmann_json_j["emitter_radius"] = nlohmann_json_t.emitter_radius;
		}
		if (nlohmann_json_t.emitter_offset_x_is_null) {
			nlohmann_json_j["emitter_offset_x"] = nullptr;
		}
		else {
			nlohmann_json_j["emitter_offset_x"] = nlohmann_json_t.emitter_offset_x;
		}
		if (nlohmann_json_t.emitter_offset_y_is_null) {
			nlohmann_json_j["emitter_offset_y"] = nullptr;
		}
		else {
			nlohmann_json_j["emitter_offset_y"] = nlohmann_json_t.emitter_offset_y;
		}
		if (nlohmann_json_t.emitter_box_x_is_null) {
			nlohmann_json_j["emitter_box_x"] = nullptr;
		}
		else {
			nlohmann_json_j["emitter_box_x"] = nlohmann_json_t.emitter_box_x;
		}
		if (nlohmann_json_t.emitter_box_y_is_null) {
			nlohmann_json_j["emitter_box_y"] = nullptr;
		}
		else {
			nlohmann_json_j["emitter_box_y"] = nlohmann_json_t.emitter_box_y;
		}
		if (nlohmann_json_t.emitter_path_is_null) {
			nlohmann_json_j["emitter_path"] = nullptr;
		}
		else {
			nlohmann_json_j["emitter_path"] = nlohmann_json_t.emitter_path;
		}
		if (nlohmann_json_t.emitter_skew_x_is_null) {
			nlohmann_json_j["emitter_skew_x"] = nullptr;
		}
		else {
			nlohmann_json_j["emitter_skew_x"] = nlohmann_json_t.emitter_skew_x;
		}
		if (nlohmann_json_t.emitter_skew_y_is_null) {
			nlohmann_json_j["emitter_skew_y"] = nullptr;
		}
		else {
			nlohmann_json_j["emitter_skew_y"] = nlohmann_json_t.emitter_skew_y;
		}
		if (nlohmann_json_t.particle_duration_is_null) {
			nlohmann_json_j["particle_duration"] = nullptr;
		}
		else {
			nlohmann_json_j["particle_duration"] = nlohmann_json_t.particle_duration;
		}
		if (nlohmann_json_t.system_red_is_null) {
			nlohmann_json_j["system_red"] = nullptr;
		}
		else {
			nlohmann_json_j["system_red"] = nlohmann_json_t.system_red;
		}
		if (nlohmann_json_t.system_green_is_null) {
    		nlohmann_json_j["system_green"] = nullptr;
		}
		else {
    		nlohmann_json_j["system_green"] = nlohmann_json_t.system_green;
		}
		if (nlohmann_json_t.system_blue_is_null) {
    		nlohmann_json_j["system_blue"] = nullptr;
		}
		else {
    		nlohmann_json_j["system_blue"] = nlohmann_json_t.system_blue;
		}
		if (nlohmann_json_t.system_alpha_is_null) {
    		nlohmann_json_j["system_alpha"] = nullptr;
		} else {
    		nlohmann_json_j["system_alpha"] = nlohmann_json_t.system_alpha;
		}
		if (nlohmann_json_t.system_brightness_is_null) {
    		nlohmann_json_j["system_brightness"] = nullptr;
		} else {
    		nlohmann_json_j["system_brightness"] = nlohmann_json_t.system_brightness;
		}
		if (nlohmann_json_t.launch_speed_is_null) {
    		nlohmann_json_j["launch_speed"] = nullptr;
		} else {
    		nlohmann_json_j["launch_speed"] = nlohmann_json_t.launch_speed;
		}
		if (nlohmann_json_t.launch_angle_is_null) {
    		nlohmann_json_j["launch_angle"] = nullptr;
		} else {
    		nlohmann_json_j["launch_angle"] = nlohmann_json_t.launch_angle;
		}
		if (nlohmann_json_t.field_is_null) {
    		nlohmann_json_j["field"] = nullptr;
		} else {
    		nlohmann_json_j["field"] = nlohmann_json_t.field;
		}
		if (nlohmann_json_t.system_field_is_null) {
    		nlohmann_json_j["system_field"] = nullptr;
		} else {
    		nlohmann_json_j["system_field"] = nlohmann_json_t.system_field;
		}
		if (nlohmann_json_t.particle_red_is_null) {
    		nlohmann_json_j["particle_red"] = nullptr;
		} else {
			nlohmann_json_j["particle_red"] = nlohmann_json_t.particle_red;
		}
		if (nlohmann_json_t.particle_green_is_null) {
			nlohmann_json_j["particle_green"] = nullptr;
		} else {
			nlohmann_json_j["particle_green"] = nlohmann_json_t.particle_green;
		}
		if (nlohmann_json_t.particle_blue_is_null) {
			nlohmann_json_j["particle_blue"] = nullptr;
		} else {
			nlohmann_json_j["particle_blue"] = nlohmann_json_t.particle_blue;
		}
		if (nlohmann_json_t.particle_alpha_is_null) {
			nlohmann_json_j["particle_alpha"] = nullptr;
		} else {
			nlohmann_json_j["particle_alpha"] = nlohmann_json_t.particle_alpha;
		}
		if (nlohmann_json_t.particle_brightness_is_null) {
			nlohmann_json_j["particle_brightness"] = nullptr;
		} else {
			nlohmann_json_j["particle_brightness"] = nlohmann_json_t.particle_brightness;
		}
		if (nlohmann_json_t.particle_spin_angle_is_null) {
			nlohmann_json_j["particle_spin_angle"] = nullptr;
		} else {
			nlohmann_json_j["particle_spin_angle"] = nlohmann_json_t.particle_spin_angle;
		}
		if (nlohmann_json_t.particle_spin_speed_is_null) {
			nlohmann_json_j["particle_spin_speed"] = nullptr;
		} else {
			nlohmann_json_j["particle_spin_speed"] = nlohmann_json_t.particle_spin_speed;
		}
		if (nlohmann_json_t.particle_scale_is_null) {
			nlohmann_json_j["particle_scale"] = nullptr;
		} else {
			nlohmann_json_j["particle_scale"] = nlohmann_json_t.particle_scale;
		}
		if (nlohmann_json_t.particle_stretch_is_null) {
			nlohmann_json_j["particle_stretch"] = nullptr;
		} else {
			nlohmann_json_j["particle_stretch"] = nlohmann_json_t.particle_stretch;
		}
		if (nlohmann_json_t.collision_reflect_is_null) {
    		nlohmann_json_j["collision_reflect"] = nullptr;
		} else {
			nlohmann_json_j["collision_reflect"] = nlohmann_json_t.collision_reflect;
		}
		if (nlohmann_json_t.collision_spin_is_null) {
			nlohmann_json_j["collision_spin"] = nullptr;
		} else {
			nlohmann_json_j["collision_spin"] = nlohmann_json_t.collision_spin;
		}
		if (nlohmann_json_t.clip_top_is_null) {
			nlohmann_json_j["clip_top"] = nullptr;
		} else {
			nlohmann_json_j["clip_top"] = nlohmann_json_t.clip_top;
		}
		if (nlohmann_json_t.clip_bottom_is_null) {
			nlohmann_json_j["clip_bottom"] = nullptr;
		} else {
			nlohmann_json_j["clip_bottom"] = nlohmann_json_t.clip_bottom;
		}
		if (nlohmann_json_t.clip_left_is_null) {
			nlohmann_json_j["clip_left"] = nullptr;
		} else {
			nlohmann_json_j["clip_left"] = nlohmann_json_t.clip_left;
		}
		if (nlohmann_json_t.clip_right_is_null) {
			nlohmann_json_j["clip_right"] = nullptr;
		} else {
			nlohmann_json_j["clip_right"] = nlohmann_json_t.clip_right;
		}
		if (nlohmann_json_t.animation_rate_is_null) {
    		nlohmann_json_j["animation_rate"] = nullptr;
		} else {
    		nlohmann_json_j["animation_rate"] = nlohmann_json_t.animation_rate;
		}
		return;
	} 

	inline auto from_json(
		const nlohmann::ordered_json& nlohmann_json_j, 
		ParticlesEmitter& nlohmann_json_t
	) -> void
	{ 
		if (nlohmann_json_j.at("name").is_null()) {
			nlohmann_json_t.name_is_null = true;
		}
		else {
			nlohmann_json_j.at("name").get_to(nlohmann_json_t.name);
		}
		if (nlohmann_json_j.at("image").is_null()) {
    		nlohmann_json_t.image_is_null = true;
		} else {
			nlohmann_json_j.at("image").get_to(nlohmann_json_t.image);
		}
		if (nlohmann_json_j.at("image_path").is_null()) {
			nlohmann_json_t.image_path_is_null = true;
		} else {
			nlohmann_json_j.at("image_path").get_to(nlohmann_json_t.image_path);
		}
		if (nlohmann_json_j.at("image_cols").is_null()) {
			nlohmann_json_t.image_cols_is_null = true;
		} else {
			nlohmann_json_j.at("image_cols").get_to(nlohmann_json_t.image_cols);
		}
		if (nlohmann_json_j.at("image_rows").is_null()) {
    		nlohmann_json_t.image_rows_is_null = true;
		} else {
			nlohmann_json_j.at("image_rows").get_to(nlohmann_json_t.image_rows);
		}
		if (nlohmann_json_j.at("image_frames").is_null()) {
			nlohmann_json_t.image_frames_is_null = true;
		} else {
			nlohmann_json_j.at("image_frames").get_to(nlohmann_json_t.image_frames);
		}
		if (nlohmann_json_j.at("animated").is_null()) {
			nlohmann_json_t.animated_is_null = true;
		} else {
			nlohmann_json_j.at("animated").get_to(nlohmann_json_t.animated);
		}
		nlohmann_json_j.at("particles_flag").get_to(nlohmann_json_t.particles_flag);
		nlohmann_json_j.at("emitter_type").get_to(nlohmann_json_t.emitter_type);
		if (nlohmann_json_j.at("on_duration").is_null()) {
			nlohmann_json_t.on_duration_is_null = true;
		} else {
			nlohmann_json_j.at("on_duration").get_to(nlohmann_json_t.on_duration);
		}
		if (nlohmann_json_j.at("system_duration").is_null()) {
			nlohmann_json_t.system_duration_is_null = true;
		} else {
			nlohmann_json_j.at("system_duration").get_to(nlohmann_json_t.system_duration);
		}
		if (nlohmann_json_j.at("cross_fade_duration").is_null()) {
			nlohmann_json_t.cross_fade_duration_is_null = true;
		} else {
			nlohmann_json_j.at("cross_fade_duration").get_to(nlohmann_json_t.cross_fade_duration);
		}
		if (nlohmann_json_j.at("spawn_rate").is_null()) {
			nlohmann_json_t.spawn_rate_is_null = true;
		} else {
			nlohmann_json_j.at("spawn_rate").get_to(nlohmann_json_t.spawn_rate);
		}
		if (nlohmann_json_j.at("spawn_min_active").is_null()) {
    		nlohmann_json_t.spawn_min_active_is_null = true;
		} else {
    		nlohmann_json_j.at("spawn_min_active").get_to(nlohmann_json_t.spawn_min_active);
		}
		if (nlohmann_json_j.at("spawn_max_active").is_null()) {
			nlohmann_json_t.spawn_max_active_is_null = true;
		} else {
			nlohmann_json_j.at("spawn_max_active").get_to(nlohmann_json_t.spawn_max_active);
		}
		if (nlohmann_json_j.at("spawn_max_launched").is_null()) {
			nlohmann_json_t.spawn_max_launched_is_null = true;
		} else {
			nlohmann_json_j.at("spawn_max_launched").get_to(nlohmann_json_t.spawn_max_launched);
		}
		if (nlohmann_json_j.at("emitter_radius").is_null()) {
			nlohmann_json_t.emitter_radius_is_null = true;
		} else {
			nlohmann_json_j.at("emitter_radius").get_to(nlohmann_json_t.emitter_radius);
		}
		if (nlohmann_json_j.at("emitter_offset_x").is_null()) {
			nlohmann_json_t.emitter_offset_x_is_null = true;
		} else {
			nlohmann_json_j.at("emitter_offset_x").get_to(nlohmann_json_t.emitter_offset_x);
		}
		if (nlohmann_json_j.at("emitter_offset_y").is_null()) {
			nlohmann_json_t.emitter_offset_y_is_null = true;
		} else {
			nlohmann_json_j.at("emitter_offset_y").get_to(nlohmann_json_t.emitter_offset_y);
		}
		if (nlohmann_json_j.at("emitter_box_x").is_null()) {
			nlohmann_json_t.emitter_box_x_is_null = true;
		} else {
			nlohmann_json_j.at("emitter_box_x").get_to(nlohmann_json_t.emitter_box_x);
		}
		if (nlohmann_json_j.at("emitter_box_y").is_null()) {
			nlohmann_json_t.emitter_box_y_is_null = true;
		} else {
			nlohmann_json_j.at("emitter_box_y").get_to(nlohmann_json_t.emitter_box_y);
		}
		if (nlohmann_json_j.at("emitter_path").is_null()) {
			nlohmann_json_t.emitter_path_is_null = true;
		} else {
			nlohmann_json_j.at("emitter_path").get_to(nlohmann_json_t.emitter_path);
		}
		if (nlohmann_json_j.at("emitter_skew_x").is_null()) {
			nlohmann_json_t.emitter_skew_x_is_null = true;
		} else {
			nlohmann_json_j.at("emitter_skew_x").get_to(nlohmann_json_t.emitter_skew_x);
		}
		if (nlohmann_json_j.at("emitter_skew_y").is_null()) {
			nlohmann_json_t.emitter_skew_y_is_null = true;
		} else {
			nlohmann_json_j.at("emitter_skew_y").get_to(nlohmann_json_t.emitter_skew_y);
		}
		if (nlohmann_json_j.at("particle_duration").is_null()) {
			nlohmann_json_t.particle_duration_is_null = true;
		} else {
			nlohmann_json_j.at("particle_duration").get_to(nlohmann_json_t.particle_duration);
		}
		if (nlohmann_json_j.at("system_red").is_null()) {
    		nlohmann_json_t.system_red_is_null = true;
		} else {
			nlohmann_json_j.at("system_red").get_to(nlohmann_json_t.system_red);
		}
		if (nlohmann_json_j.at("system_green").is_null()) {
			nlohmann_json_t.system_green_is_null = true;
		} else {
			nlohmann_json_j.at("system_green").get_to(nlohmann_json_t.system_green);
		}
		if (nlohmann_json_j.at("system_blue").is_null()) {
    		nlohmann_json_t.system_blue_is_null = true;
		} else {
			nlohmann_json_j.at("system_blue").get_to(nlohmann_json_t.system_blue);
		}
		if (nlohmann_json_j.at("system_alpha").is_null()) {
			nlohmann_json_t.system_alpha_is_null = true;
		} else {
			nlohmann_json_j.at("system_alpha").get_to(nlohmann_json_t.system_alpha);
		}
		if (nlohmann_json_j.at("system_brightness").is_null()) {
			nlohmann_json_t.system_brightness_is_null = true;
		} else {
			nlohmann_json_j.at("system_brightness").get_to(nlohmann_json_t.system_brightness);
		}
		if (nlohmann_json_j.at("launch_speed").is_null()) {
			nlohmann_json_t.launch_speed_is_null = true;
		} else {
			nlohmann_json_j.at("launch_speed").get_to(nlohmann_json_t.launch_speed);
		}
		if (nlohmann_json_j.at("launch_angle").is_null()) {
			nlohmann_json_t.launch_angle_is_null = true;
		} else {
			nlohmann_json_j.at("launch_angle").get_to(nlohmann_json_t.launch_angle);
		}
		if (nlohmann_json_j.at("field").is_null()) {
			nlohmann_json_t.field_is_null = true;
		} else {
			nlohmann_json_j.at("field").get_to(nlohmann_json_t.field);
		}
		if (nlohmann_json_j.at("system_field").is_null()) {
			nlohmann_json_t.system_field_is_null = true;
		} else {
			nlohmann_json_j.at("system_field").get_to(nlohmann_json_t.system_field);
		}
		if (nlohmann_json_j.at("particle_red").is_null()) {
			nlohmann_json_t.particle_red_is_null = true;
		} else {
			nlohmann_json_j.at("particle_red").get_to(nlohmann_json_t.particle_red);
		}
		if (nlohmann_json_j.at("particle_green").is_null()) {
			nlohmann_json_t.particle_green_is_null = true;
		} else {
			nlohmann_json_j.at("particle_green").get_to(nlohmann_json_t.particle_green);
		}
		if (nlohmann_json_j.at("particle_blue").is_null()) {
			nlohmann_json_t.particle_blue_is_null = true;
		} else {
			nlohmann_json_j.at("particle_blue").get_to(nlohmann_json_t.particle_blue);
		}
		if (nlohmann_json_j.at("particle_alpha").is_null()) {
			nlohmann_json_t.particle_alpha_is_null = true;
		} else {
			nlohmann_json_j.at("particle_alpha").get_to(nlohmann_json_t.particle_alpha);
		}
		if (nlohmann_json_j.at("particle_brightness").is_null()) {
			nlohmann_json_t.particle_brightness_is_null = true;
		} else {
			nlohmann_json_j.at("particle_brightness").get_to(nlohmann_json_t.particle_brightness);
		}
		if (nlohmann_json_j.at("particle_spin_angle").is_null()) {
			nlohmann_json_t.particle_spin_angle_is_null = true;
		} else {
			nlohmann_json_j.at("particle_spin_angle").get_to(nlohmann_json_t.particle_spin_angle);
		}
		if (nlohmann_json_j.at("particle_spin_speed").is_null()) {
			nlohmann_json_t.particle_spin_speed_is_null = true;
		} else {
			nlohmann_json_j.at("particle_spin_speed").get_to(nlohmann_json_t.particle_spin_speed);
		}
		if (nlohmann_json_j.at("particle_scale").is_null()) {
			nlohmann_json_t.particle_scale_is_null = true;
		} else {
			nlohmann_json_j.at("particle_scale").get_to(nlohmann_json_t.particle_scale);
		}
		if (nlohmann_json_j.at("particle_stretch").is_null()) {
			nlohmann_json_t.particle_stretch_is_null = true;
		} else {
			nlohmann_json_j.at("particle_stretch").get_to(nlohmann_json_t.particle_stretch);
		}
		if (nlohmann_json_j.at("collision_reflect").is_null()) {
			nlohmann_json_t.collision_reflect_is_null = true;
		} else {
			nlohmann_json_j.at("collision_reflect").get_to(nlohmann_json_t.collision_reflect);
		}
		if (nlohmann_json_j.at("collision_spin").is_null()) {
			nlohmann_json_t.collision_spin_is_null = true;
		} else {
			nlohmann_json_j.at("collision_spin").get_to(nlohmann_json_t.collision_spin);
		}
		if (nlohmann_json_j.at("clip_top").is_null()) {
			nlohmann_json_t.clip_top_is_null = true;
		} else {
			nlohmann_json_j.at("clip_top").get_to(nlohmann_json_t.clip_top);
		}
		if (nlohmann_json_j.at("clip_bottom").is_null()) {
			nlohmann_json_t.clip_bottom_is_null = true;
		} else {
			nlohmann_json_j.at("clip_bottom").get_to(nlohmann_json_t.clip_bottom);
		}
		if (nlohmann_json_j.at("clip_left").is_null()) {
			nlohmann_json_t.clip_left_is_null = true;
		} else {
			nlohmann_json_j.at("clip_left").get_to(nlohmann_json_t.clip_left);
		}
		if (nlohmann_json_j.at("clip_right").is_null()) {
			nlohmann_json_t.clip_right_is_null = true;
		} else {
			nlohmann_json_j.at("clip_right").get_to(nlohmann_json_t.clip_right);
		}
		if (nlohmann_json_j.at("animation_rate").is_null()) {
    		nlohmann_json_t.animation_rate_is_null = true;
		} else {
			nlohmann_json_j.at("animation_rate").get_to(nlohmann_json_t.animation_rate);
		}
		return;
	}

	struct Particles
	{
		std::vector<ParticlesEmitter> emitters;
		bool emitters_is_null;
	};

	inline auto to_json(
		nlohmann::ordered_json& nlohmann_json_j, 
		const Particles& nlohmann_json_t
	) -> void
	{
		if (nlohmann_json_t.emitters_is_null) {
			nlohmann_json_j["emitters"] = nullptr;
		}
		else {
			nlohmann_json_j["emitters"] = nlohmann_json_t.emitters;
		}
	} 
	
	inline auto from_json(
		const nlohmann::ordered_json& nlohmann_json_j, 
		Particles& nlohmann_json_t
	) -> void
	{
		if (nlohmann_json_j.at("emitters").is_null()) {
			nlohmann_json_t.emitters_is_null = true;
		}
		else {
			nlohmann_json_j.at("emitters").get_to(nlohmann_json_t.emitters);
		}
		return;
	}
}