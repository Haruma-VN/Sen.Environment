#pragma once

#include "kernel/support/popcap/animation/definition.hpp"

namespace Sen::Kernel::Support::PopCap::Animation::Convert
{

	struct ImageInfo {
		std::string name;
		AnimationSize size;
	};

	inline auto static to_json(
		nlohmann::ordered_json &nlohmann_json_j,
		const ImageInfo &nlohmann_json_t) -> void
	{
		nlohmann_json_j["name"] = nlohmann_json_t.name;
		nlohmann_json_j["size"] = nlohmann_json_t.size;
		return;
	}

	inline auto static from_json(
		const nlohmann::ordered_json &nlohmann_json_j,
		ImageInfo &nlohmann_json_t) -> void
	{
		nlohmann_json_j.at("name").get_to(nlohmann_json_t.name);
		nlohmann_json_j.at("size").get_to(nlohmann_json_t.size);
		return;
	};

	struct SpriteInfo {

	};

	struct RecordInfo
	{
		int version;
		int resolution;
		std::map<std::string, ImageInfo> image;
		std::map<std::string, std::vector<std::string>> sprite;
	};

	inline auto static to_json(
		nlohmann::ordered_json &nlohmann_json_j,
		const RecordInfo &nlohmann_json_t) -> void
	{
		nlohmann_json_j["version"] = nlohmann_json_t.version;
		nlohmann_json_j["resolution"] = nlohmann_json_t.resolution;
		nlohmann_json_j["image"] = nlohmann_json_t.image;
		nlohmann_json_j["sprite"] = nlohmann_json_t.sprite;
		return;
	}

	inline auto static from_json(
		const nlohmann::ordered_json &nlohmann_json_j,
		RecordInfo &nlohmann_json_t) -> void
	{
		nlohmann_json_j.at("version").get_to(nlohmann_json_t.version);
		nlohmann_json_j.at("resolution").get_to(nlohmann_json_t.resolution);
		nlohmann_json_j.at("image").get_to(nlohmann_json_t.image);
		nlohmann_json_j.at("sprite").get_to(nlohmann_json_t.sprite);
		return;
	};

	struct Common
	{

		inline static auto constexpr initial_transform = std::array<double, 6>{1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f};

		inline static auto constexpr initial_color = std::array<double, 4>{1.0f, 1.0f, 1.0f, 1.0f};
	};

	struct Model
	{
	public:
		int index;
		bool use_model;
		uint8_t state;
		int resource;
		bool sprite;
		int frame_start;
		int frame_duration;
		std::array<double, 6> transform{1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f};
		std::array<double, 4> color{1.0f, 1.0f, 1.0f, 1.0f};

		explicit Model(

			) = default;

		explicit constexpr Model(
			uint8_t state,
			int resource,
			bool sprite,
			int frame_start,
			int frame_duration) : state(state), resource(resource), sprite(sprite),
								  frame_start(frame_start), frame_duration(frame_duration)
		{
		}

		~Model(

			) = default;
	};

	struct FrameNode
	{
	public:
		int index;
		int duration;
		int resource;
		bool sprite;
		int first_frame;
		std::array<double, 6> transform{1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f};
		std::array<double, 4> color{1.0f, 1.0f, 1.0f, 1.0f};

		explicit FrameNode(

			) = default;

		explicit constexpr FrameNode(
			int index,
			int duration,
			int resource
			) : index(index), duration(duration), resource(resource)
		{
		}

		~FrameNode(

			) = default;
	};

	struct ActionFrameNode
	{
	public:
		int index;
		int duration;
		bool use_transform;
		std::vector<double> transform;
		bool use_color;
		std::array<double, 4> color;
	};

	struct Action
	{
	public:
		int start_index;
		int end_index;
		std::vector<int> frame_index;

		explicit Action(

			) = default;

		explicit constexpr Action(
			int start_index,
			int end_index) : start_index(start_index), end_index(end_index)
		{
		}

		~Action(

			) = default;
	};

	struct Instance
	{
		int resource;
		bool sprite;
	};

	struct FrameList {
		
		std::map<int, std::vector<FrameNode>> frame_node_list;
		std::map<std::string, Action> action_list;
		std::vector<std::string> action_name_list;

		FrameList(

		) = default;

		~FrameList(

		) = default;

		FrameList(
			FrameList&& that
		) = delete;

		auto operator =(
			FrameList&& that
		) -> FrameList& = delete;
	};
}