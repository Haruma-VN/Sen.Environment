#pragma once
#include <string>
#include <codecvt>

inline static auto utf16_to_utf8(
	const std::wstring& wstr
) -> std::string
{
	auto myconv = std::wstring_convert<std::codecvt_utf8<wchar_t>>{};
	return myconv.to_bytes(wstr.data(), wstr.data() + wstr.size());
}

inline static auto utf8_to_utf16(
	const std::string& str
) -> std::wstring
{
	auto myconv = std::wstring_convert<std::codecvt_utf8<wchar_t>>{};
	return myconv.from_bytes(str.data(), str.data() + str.size());
}