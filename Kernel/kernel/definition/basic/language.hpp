#pragma once

#include <map>
#include <string>
#include <memory>
#include <cstdio>
#include <vector>
#include <cstdarg>
#include <string_view>
#include "kernel/dependencies/json.hpp"
#include "kernel/definition/assert.hpp"
#include "kernel/dependencies/fmt.hpp"

namespace Sen::Kernel::Language
{

	/**
	 * DO NOT USE THIS, THIS IS LANGUAGE CONTAINER
	*/

	static std::map<std::string, std::string> language;

	/**
	 * Lambda auto close file
	*/

	inline static auto constexpr close_file = [](auto f)
	{
		if(f)
		{
			std::fclose(f);
			f = nullptr;
		}
		return;
	};

	/**
	 * Language file reader
	 * Kernel will parse language before start program
	*/

	inline static auto read_language(
		std::string_view source
	) -> void
	{
		auto file = std::ifstream(source.data());
		if (!file.is_open()) {
			throw Exception(fmt::format("Could not read language file: {}", source), std::source_location::current(), "read_language");
		}
		auto buffer = std::stringstream{};
		buffer << file.rdbuf();
		language = nlohmann::json::parse(buffer.str());
		return;
	}

	/**
	 * Get the string from Kernel
	 * The Script will obtain from here, also inner kernel method
	*/

	inline static auto get(
		std::string_view key
	) -> std::string_view
	{
		if (language.find(key.data()) == language.end()){
			return key;
		}
		return language[key.data()];
	}
}