#pragma once

#include "kernel/definition/library.hpp"
#include "kernel/definition/macro.hpp"

namespace Sen::Kernel::String {

	// using string

	using std::string;

	// using vector

	using std::vector;

	// split by delimeter
	// str: provide string
	// delimiter: provide delimiter to split
	// result: the vector of string after split

	inline auto split(
		string str,
		const string &delimiter
	) -> vector<string> const
	{
		auto result = vector<string>{};
		auto pos = string::size_type(0);
		auto token = string{};
		while ((pos = str.find(delimiter)) != string::npos) {
			token = str.substr(0, pos);
			result.push_back(token);
			str.erase(0, pos + delimiter.length());
		}
		result.push_back(str);
		return result;
	}

	// posix style test: a/b/c/d

	inline auto toPosixStyle(
		string str
	) -> string const
	{
		for(auto &c : str)
		{
			if(c == '\\')
			{
				c = '/';
			}
		}
		return str;
	}

	// windows style test: a\b\c

	inline auto toWindowsStyle(
		string str
	) -> string const
	{
		for(auto &c : str)
		{
			if(c == '/')
			{
				c = '\\';
			}
		}
		return str;
	}

	/*
		Join function between string
		Example: ["s", "a", "m"] -> sam
	*/

	inline auto join(
		const vector<string> &data,
		const string &delimiter
	) -> string const
	{
		auto result = string{};
		for(auto &c : data)
		{
			result += c;
			result += delimiter;
		}
		return result;
	}

	// Span

	using std::span;

	// Type constraints

	using std::is_same_v;

	/*
		T should be unsigned char or char
		str is the string need hash
	*/

	template <typename T>
	inline auto convertStringToSpan(
		string &str
	) -> span<T> const
	{
		static_assert(is_same_v<T, char> || is_same_v<T, unsigned char>,
					"Template argument T must be char or unsigned char"
		);
		auto spanT = span<T>(reinterpret_cast<T*>(str.data()), str.size());
		return spanT;
	}

	// str: string need replace
	// from: from which string
	// to: which string to replace
	// return: replaced string

	inline auto replaceAll(
		string str, 
		const string& from,
		const string& to
	) -> string
	{
		if(from.empty()){
			return str;
		}
		auto start_pos = static_cast<size_t>(0);
		while((start_pos = str.find(from, start_pos)) != string::npos) {
			str.replace(start_pos, from.length(), to);
			start_pos += to.length();
		}
		return str;
	}

	/**
	 * s: string to convert (utf16)
	 * return: utf-8 string
	*/

	inline auto to_utf8(
		const std::u16string &s
		) -> std::string
	{
		std::wstring_convert<std::codecvt_utf8<char16_t>, char16_t> conv;
		return conv.to_bytes(s);
	}

	/**
	 * s: string to convert (utf32)
	 * return: utf8 string
	*/

	inline auto to_utf8(
		const std::u32string &s
	) -> std::string
	{
		std::wstring_convert<std::codecvt_utf8<char32_t>, char32_t> conv;
		return conv.to_bytes(s);
	}

	/**
	 * s: string to convert (utf8)
	 * return: utf16 string
	*/

	inline auto to_utf16(
		const std::string &s
	) -> std::u16string
	{
		std::wstring_convert<std::codecvt_utf8<char16_t>, char16_t> convert;
		return convert.from_bytes(s);
	}

	/**
	 * s: string to convert (utf8)
	 * return: utf32 string
	*/

	inline auto to_utf32(
		const std::string &s
	) -> std::u32string
	{
		std::wstring_convert<std::codecvt_utf8<char32_t>, char32_t> conv;
		return conv.from_bytes(s);
	}

	/**
	 * s: string to convert (utf32)
	 * return: utf16 string
	*/

	inline auto to_utf16(
		const std::u32string &s
	) -> std::u16string
	{
		return to_utf16(to_utf8(s));
	}

	/**
	 * s: string to convert (utf16)
	 * return: utf32 string
	*/

	inline auto to_utf32(
		const std::u16string &s
	) -> std::u32string 
	{
		return to_utf32(to_utf8(s));
	}

	/**
	 * src: source
	 * return: utf32 string
	*/

	inline auto read_with_bom(
		std::istream & src
	) -> std::u32string
	{

		enum encoding {
			encoding_utf32be = 0,
			encoding_utf32le,
			encoding_utf16be,
			encoding_utf16le,
			encoding_utf8,
			encoding_ascii,
		};

		auto boms = std::vector<std::string>{
			std::string("\x00\x00\xFE\xFF", 4),
			std::string("\xFF\xFE\x00\x00", 4),
			std::string("\xFE\xFF", 2),
			std::string("\xFF\xFE", 2),
			std::string("\xEF\xBB\xBF", 3)
		};

		auto buffer = std::string((std::istreambuf_iterator<char>(src)), std::istreambuf_iterator<char>());

		auto enc = encoding_ascii;

		for (auto i = 0; i < boms.size(); ++i) {
			auto testBom = boms[i];
			if (buffer.compare(0, testBom.length(), testBom) == 0) {
				enc = encoding(i);
				buffer = buffer.substr(testBom.length());
				break;
			}
		}

		switch (enc) {
			case encoding_utf32be:
			{
				if (buffer.length() % 4 != 0) {
					throw std::logic_error("size in bytes must be a multiple of 4");
				}
				auto count = buffer.length() / 4;
				auto temp = std::u32string(count, 0);
				for (auto i = 0; i < count; ++i) {
					temp[i] = static_cast<char32_t>(buffer[i * 4 + 3] << 0 | buffer[i * 4 + 2] << 8 | buffer[i * 4 + 1] << 16 | buffer[i * 4 + 0] << 24);
				}
				return temp;
			}
			case encoding_utf32le:
			{
				if (buffer.length() % 4 != 0) {
					throw std::logic_error("size in bytes must be a multiple of 4");
				}
				auto count = buffer.length() / 4;
				auto temp = std::u32string(count, 0);
				for (auto i = 0; i < count; ++i) {
					temp[i] = static_cast<char32_t>(buffer[i * 4 + 0] << 0 | buffer[i * 4 + 1] << 8 | buffer[i * 4 + 2] << 16 | buffer[i * 4 + 3] << 24);
				}
				return temp;
			}
			case encoding_utf16be:
			{
				if (buffer.length() % 2 != 0) {
					throw std::logic_error("size in bytes must be a multiple of 2");
				}
				auto count = buffer.length() / 2;
				auto temp = std::u16string(count, 0);
				for (auto i = 0; i < count; ++i) {
					temp[i] = static_cast<char16_t>(buffer[i * 2 + 1] << 0 | buffer[i * 2 + 0] << 8);
				}
				return to_utf32(temp);
			}
			case encoding_utf16le:
			{
				if (buffer.length() % 2 != 0) {
					throw std::logic_error("size in bytes must be a multiple of 2");
				}
				auto count = buffer.length() / 2;
				auto temp = std::u16string(count, 0);
				for (auto i = 0; i < count; ++i) {
					temp[i] = static_cast<char16_t>(buffer[i * 2 + 0] << 0 | buffer[i * 2 + 1] << 8);
				}
				return to_utf32(temp);
			}
			default:
				return to_utf32(buffer);
			}
	}

}