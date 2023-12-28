#pragma once

#include "kernel/definition/library.hpp"
#include "kernel/definition/macro.hpp"

namespace Sen::Kernel::Definition::Object {

	/**
	 * data: Object
	 * return: keys
	*/

	static auto keys(
		const nlohmann::ordered_json &data
	) -> std::vector<std::string>
	{
		auto result = std::vector<std::string>();
		for(auto &[key, value] : data.items()){
			result.push_back(key);
		}
		return result;
	}

	/**
	 * data: Object
	 * return: array of value
	*/

	static auto values(
		const nlohmann::ordered_json &data
	) -> nlohmann::ordered_json::array_t
	{
		auto result = nlohmann::ordered_json::array_t{};
		for(auto & [key, value] : data.items()){
			result.push_back(value);
		}
		return result;
	}


}