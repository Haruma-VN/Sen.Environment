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

}