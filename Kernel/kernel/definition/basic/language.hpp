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

	inline static auto constexpr close_file = [](FILE* f)
	{
		if(f != nullptr)
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

	inline static auto read_language (
		std::string_view source
	) -> void {
		#if _WIN32
		auto static constexpr utf8_to_utf16 = [](std::string_view str) -> std::wstring
		{
			auto myconv = std::wstring_convert<std::codecvt_utf8<wchar_t>>{};
			return myconv.from_bytes(str.data(), str.data() + str.size());
		};
		auto file = std::ifstream(utf8_to_utf16(source));
		#else
		auto file = std::ifstream(source.data());
		#endif
		if (!(file.is_open())) {
			#if _WIN32
			auto path = std::string{source.data(), source.size()};
			std::replace(path.begin(), path.end(), '\\', '/');
			throw Exception(fmt::format("Could not read language file: {}", path), std::source_location::current(), "read_language");
			#else
			throw Exception(fmt::format("Could not read language file: {}", source), std::source_location::current(), "read_language");
			#endif
		}
		auto buffer = std::string((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
		language = nlohmann::ordered_json::parse(buffer);
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