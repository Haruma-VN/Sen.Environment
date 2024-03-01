#pragma once

#include "kernel/support/popcap/animation/definition.hpp"

namespace Sen::Kernel::Support::PopCap::Animation::Convert
{

	struct ExtraInfo
	{
		int version;
		nlohmann::ordered_json group;
	};

	inline auto static to_json(
		nlohmann::ordered_json &nlohmann_json_j,
		const ExtraInfo &nlohmann_json_t) -> void
	{
		nlohmann_json_j["version"] = nlohmann_json_t.version;
		nlohmann_json_j["group"] = nlohmann_json_t.group;
		return;
	}

	inline auto static from_json(
		const nlohmann::ordered_json &nlohmann_json_j,
		ExtraInfo &nlohmann_json_t) -> void
	{
		nlohmann_json_j.at("version").get_to(nlohmann_json_t.version);
		nlohmann_json_j.at("group").get_to(nlohmann_json_t.group);
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
		int8_t state;
		int resource;
		bool sprite;
		int frame_start;
		int frame_duration;
		std::array<double, 6> transform{1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f};
		std::array<double, 4> color{1.0f, 1.0f, 1.0f, 1.0f};

		explicit Model(

			) = default;

		explicit constexpr Model(
			int index,
			int8_t state,
			int resource,
			bool sprite,
			int frame_start,
			int frame_duration) : index(index), state(state), resource(resource), sprite(sprite),
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

		explicit FrameNode(
			int index,
			int duration,
			int resource,
			bool sprite,
			int first_frame
			) : index(index), duration(duration), resource(resource), sprite(sprite),
													first_frame(first_frame)
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
		int duration;
		std::vector<int> frame_index;

		explicit Action(

			) = default;

		explicit constexpr Action(
			int start_index,
			int duration) : start_index(start_index), duration(duration)
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
}