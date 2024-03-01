#pragma once

#include "kernel/support/popcap/animation/definition.hpp"

namespace Sen::Kernel::Support::PopCap::Animation::Convert {

	struct ExtraInfo {
		int version;
		nlohmann::ordered_json group;
	};

	inline auto static to_json(
		nlohmann::ordered_json& nlohmann_json_j, 
		const ExtraInfo& nlohmann_json_t
	) -> void
	{
		nlohmann_json_j["version"] = nlohmann_json_t.version; 
		nlohmann_json_j["group"] = nlohmann_json_t.group;
		return;
	} 
	
	inline auto static from_json(
		const nlohmann::ordered_json& nlohmann_json_j, 
		ExtraInfo& nlohmann_json_t
	) -> void
	{
		nlohmann_json_j.at("version").get_to(nlohmann_json_t.version); 
		nlohmann_json_j.at("group").get_to(nlohmann_json_t.group);
		return;
	};

	struct Common {

		inline static auto constexpr initial_transform = std::array<float, 6> { 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f };

		inline static auto constexpr initial_color = std::array<float, 4>{1.0f, 1.0f, 1.0f, 1.0f};

	};
}