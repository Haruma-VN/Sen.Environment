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
	) -> void {
		#if _WIN32
		auto myconv = std::wstring_convert<std::codecvt_utf8<wchar_t>>{};
		auto file = std::unique_ptr<FILE, decltype(close_file)>(_wfopen(myconv.from_bytes(source.data(), source.data() + source.size()).c_str(), L"r"), close_file);
		#else
		auto file = std::unique_ptr<FILE, decltype(close_file)>(std::fopen(source.data(), "r"), close_file);
		#endif
		if (!file) {
			throw Exception(fmt::format("Could not read language file: {}", source), std::source_location::current(), "read_language");
		}
		std::fseek(file.get(), 0, SEEK_END);
		auto fsize = std::ftell(file.get());
		std::fseek(file.get(), 0, SEEK_SET);
		auto buffer = std::string(fsize, ' ');
		std::fread(buffer.data(), 1, fsize, file.get());
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