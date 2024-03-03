#pragma once

#include <string>
#include <locale>
#include <codecvt>

namespace Sen::Shell {

	inline static auto utf8_to_utf16(
		const std::string& str
	) -> std::wstring
	{
		auto myconv = std::wstring_convert<std::codecvt_utf8<wchar_t>>{};
		return myconv.from_bytes(str);
	}

	inline auto utf16_to_utf8(
		std::u16string_view const& source
	) -> std::u8string {
		auto converter = std::wstring_convert<std::codecvt_utf8_utf16<char16_t>, char16_t>{};
		auto result = converter.to_bytes(
			source.data(),
			source.data() + source.size()
		);
		if (converter.converted() != source.size()) {
			throw std::runtime_error("utf16 string is not equals to utf8");
		}
		return reinterpret_cast<std::u8string&>(result);
	}

	inline static auto utf16_to_utf8(
		const std::wstring& wstr
	) -> std::string
	{
		auto myconv = std::wstring_convert<std::codecvt_utf8<wchar_t>>{};
		return myconv.to_bytes(wstr);
	}

}