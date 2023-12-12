#pragma once

#include "kernel/definition/library.hpp"
#include "kernel/definition/macro.hpp"

namespace Sen::Kernel {

	// using string

	using std::string;

	// using vector

	using std::vector;

	// Span

	using std::span;

	// Type constraints

	using std::is_same_v;

	// Match object

	struct MatchObject {
		public:
			// source string
			std::string source;

			// index
			long index;

			// input string
			std::string input;

			// constructor

			explicit MatchObject(
				const std::string & source,
				long index,
				const std::string & input
			) : source(source), index(index), input(input)
			{

			}

			// destructor
			~MatchObject(

			) = default;
	};

	struct Common {

		public:

			inline auto static const whitespace = std::string{" \t\n\r\f\v"};
	};

	struct String : public Common {

		public:

			// string value

			std::string value;

			// constructor

			explicit String(
				const std::string & source
			) : value(source)
			{

			}

			// destructor

			~String(

			) = default;

			/**
			 * return char at position 0
			*/

			auto charAt(

			) -> char
			{
				return thiz.value.at(0);
			}

			/**
			 * index: The index for character
			   return: '\0' if the index is out of range.
			*/

			auto charAt(
				size_t index
			) -> char
			{
				if((index < 0) or index > thiz.value.length()){
					return '\0';
				}
				return thiz.value.at(index);
			}

			/**
			 * return char code at position 0
			*/

			auto charCodeAt(

			) -> short
			{
				return static_cast<short>(thiz.value.at(0));
			}

			/**
			 * index: The index for character
			   return: the char code at index
			*/

			auto charCodeAt(
				size_t index
			) -> short
			{
				if((index < 0) or index > thiz.value.length()){
					throw std::runtime_error(fmt::format("Does not have the index {} in the current string", index));
				}
				return thiz.value.at(index);
			}

			/**
			 * The endsWith() method returns true if a string ends with a specified string.

				Otherwise it returns false.

				The endsWith() method is case sensitive.
			*/

			auto endsWith(
				std::string & source
			) -> bool
			{
				if (source.length() > thiz.value.length()) 
				{
					return false;
				}
    			return std::equal(source.rbegin(), source.rend(), thiz.value.rbegin());
			}

			/**
			 * The endsWith() method returns true if a string ends with a specified string.

				Otherwise it returns false.

				The endsWith() method is case sensitive.
			*/

			auto endsWith(
				String & that
			) -> bool
			{
				if (that.value.length() > thiz.value.length()) 
				{
					return false;
				}
    			return std::equal(that.value.rbegin(), that.value.rend(), thiz.value.rbegin());
			}

			/**
			 * The String.fromCharCode() method converts Unicode values to characters.

				The String.fromCharCode() is a static method of the String object.

				The syntax is always String.fromCharCode().

				You cannot use myString.fromCharCode().
			*/

			static auto fromCharCode(
				short charCode
			) -> String
			{
				return String{std::string{static_cast<char>(charCode)}};
			}

			/**
			 * The String.fromCharCode() method converts Unicode values to characters.

				The String.fromCharCode() is a static method of the String object.

				The syntax is always String.fromCharCode().

				You cannot use myString.fromCharCode().
			*/

			static auto fromCharCode(
				std::initializer_list<short> charCodes
			) -> String
			{
				auto value = std::string{};
				for(auto & c : charCodes){
					value += c;
				}
				return String{value};
			}

			/**
			 * The includes() method returns true if a string contains a specified string.

				Otherwise it returns false.

				The includes() method is case sensitive.
			*/

			auto includes(
				const std::string & source
			) -> bool
			{
				return thiz.value.find(source) != std::string::npos;
			}

			/**
			 * The includes() method returns true if a string contains a specified string.

				Otherwise it returns false.

				The includes() method is case sensitive.
			*/

			auto includes(
				const String & that,
				size_t start
			) -> bool
			{
				return thiz.value.find(that.value, start) != std::string::npos;
			}

			/**
			 * The includes() method returns true if a string contains a specified string.

				Otherwise it returns false.

				The includes() method is case sensitive.
			*/

			auto includes(
				const std::string & source,
				size_t start
			) -> bool
			{
				return thiz.value.find(source, start) != std::string::npos;
			}

			/**
			 * The indexOf() method returns the position of the first occurrence of a value in a string.

			   The indexOf() method returns -1 if the value is not found.

			   The indexOf() method is case sensitive.
			*/

			auto indexOf(
				const std::string & source
			) -> long
			{
				auto pos = thiz.value.find(source);
				if (pos == std::string::npos){
					return -1;
				}
				return static_cast<long>(pos);
			}

			/**
			 * The indexOf() method returns the position of the first occurrence of a value in a string.

			   The indexOf() method returns -1 if the value is not found.

			   The indexOf() method is case sensitive.
			*/

			auto indexOf(
				const String & that
			) -> long
			{
				auto pos = thiz.value.find(that.value);
				if (pos == std::string::npos){
					return -1;
				}
				return static_cast<long>(pos);
			}

			/**
			 * The indexOf() method returns the position of the first occurrence of a value in a string.

			   The indexOf() method returns -1 if the value is not found.

			   The indexOf() method is case sensitive.
			*/

			auto indexOf(
				const std::string & source,
				size_t start
			) -> long
			{
				auto pos = thiz.value.find(source, start);
				if (pos == std::string::npos){
					return -1;
				}
				return static_cast<long>(pos);
			}

			/**
			 * The indexOf() method returns the position of the first occurrence of a value in a string.

			   The indexOf() method returns -1 if the value is not found.

			   The indexOf() method is case sensitive.
			*/

			auto indexOf(
				const String & that,
				size_t start
			) -> long
			{
				auto pos = thiz.value.find(that.value, start);
				if (pos == std::string::npos){
					return -1;
				}
				return static_cast<long>(pos);
			}

			/**
			 * The lastIndexOf() method returns the index (position) of the last occurrence of a specified value in a string.

				The lastIndexOf() method searches the string from the end to the beginning.

				The lastIndexOf() method returns the index from the beginning (position 0).

				The lastIndexOf() method returns -1 if the value is not found.

				The lastIndexOf() method is case sensitive.
			*/

			auto lastIndexOf(
				const String & that
			) -> long
			{
				auto pos = thiz.value.find(that.value);
				if(pos == std::string::npos){
					return -1;
				}
				while (thiz.value.find(that.value, pos) != std::string::npos){
					pos = thiz.value.find(that.value, pos);
				}
				return pos;
			}

			/**
			 * The lastIndexOf() method returns the index (position) of the last occurrence of a specified value in a string.

				The lastIndexOf() method searches the string from the end to the beginning.

				The lastIndexOf() method returns the index from the beginning (position 0).

				The lastIndexOf() method returns -1 if the value is not found.

				The lastIndexOf() method is case sensitive.
			*/

			auto lastIndexOf(
				const std::string & source
			) -> long
			{
				auto pos = thiz.value.find(source);
				if(pos == std::string::npos){
					return -1;
				}
				while (thiz.value.find(source, pos) != std::string::npos){
					pos = thiz.value.find(source, pos);
				}
				return pos;
			}

			/**
			 * return the current string size
			*/

			auto length(

			) -> size_t
			{
				return thiz.value.length();
			}

			// https://www.w3schools.com/jsref/jsref_match.asp

			/**
			 * The match() method matches a string against a regular expression **

				The match() method returns an array with the matches.

				The match() method returns null if no match is found.
			*/

			auto match(
				const std::string & source
			) -> MatchObject const
			{
				auto pos = thiz.value.find(source);
				if(pos != std::string::npos){
					return MatchObject{
						source,
						static_cast<long>(pos),
						thiz.value
					};
				}
				return MatchObject{
					source,
					-1,
					thiz.value
				};
			}

			/**
			 * The match() method matches a string against a regular expression **

				The match() method returns an array with the matches.

				The match() method returns null if no match is found.
			*/

			auto match(
				const String & that
			) -> MatchObject const
			{
				auto pos = thiz.indexOf(that);
				return MatchObject{
					that.value,
					pos,
					thiz.value
				};
			}

			/**
			 * 
			 * The repeat() method returns a string with a number of copies of a string.

				The repeat() method returns a new string.

				The repeat() method does not change the original string.
			 * 
			*/

			auto repeat(
				size_t count
			) -> String
			{
				auto result = String{thiz.value};
				for(auto i : Range<size_t>(count)){
					result.value += thiz.value;
				}
				return result;
			}

			/**
			 * The replace() method searches a string for a value or a regular expression.

				The replace() method returns a new string with the value(s) replaced.

				The replace() method does not change the original string.
			*/

			auto replace(
				const std::string & from,
				const std::string & to
			) -> std::string
			{
				auto start_pos = thiz.value.find(from);
				auto str = thiz.value;
				if(start_pos == std::string::npos)
				{
					return str;
				}
				str.replace(start_pos, from.length(), to);
				return str;
			}

			/**
			 * The replace() method searches a string for a value or a regular expression.

				The replace() method returns a new string with the value(s) replaced.

				The replace() method does not change the original string.
			*/

			auto replace(
				const String & from,
				const String & to
			) -> std::string
			{
				auto start_pos = thiz.value.find(from.value);
				auto str = thiz.value;
				if(start_pos == std::string::npos)
				{
					return str;
				}
				str.replace(start_pos, from.value.length(), to.value);
				return str;
			}

			/**
			 * The replaceAll() method searches a string for a value or a regular expression.

				The replaceAll() method returns a new string with all values replaced.

				The replaceAll() method does not change the original string.

				The replaceAll() method was introduced in JavaScript 2021.

				The replaceAll() method does not work in Internet Explorer.
			*/
			
			auto replaceAll(
				const std::string & from,
				const std::string & to
			) -> std::string
			{
				auto pos = thiz.value.find(from);
				auto str = thiz.value;
				while(pos != std::string::npos){
					str.replace(pos, from.length(), to);
					pos = thiz.value.find(from);
				}
				return str;
			}

			/**
			 * The replaceAll() method searches a string for a value or a regular expression.

				The replaceAll() method returns a new string with all values replaced.

				The replaceAll() method does not change the original string.

				The replaceAll() method was introduced in JavaScript 2021.

				The replaceAll() method does not work in Internet Explorer.
			*/

			auto replaceAll(
				const String & from,
				const String & to
			) -> std::string
			{
				auto pos = thiz.value.find(from.value);
				auto str = thiz.value;
				while(pos != std::string::npos){
					str.replace(pos, from.value.length(), to.value);
					pos = thiz.value.find(from.value);
				}
				return str;
			}

			/**
			 * The startsWith() method returns true if a string starts with a specified string.

				Otherwise it returns false.

				The startsWith() method is case sensitive.
			*/

			auto startsWith(
				const std::string & prefix
			) -> bool
			{
				if (prefix.length() > thiz.value.length()) 
				{
					return false;
				}
				return std::equal(prefix.begin(), prefix.end(), thiz.value.begin());
			}

			/**
			 * The startsWith() method returns true if a string starts with a specified string.

				Otherwise it returns false.

				The startsWith() method is case sensitive.
			*/

			auto startsWith(
				String & prefix
			) -> bool
			{
				if (prefix.length() > thiz.value.length()) 
				{
					return false;
				}
				return std::equal(prefix.value.begin(), prefix.value.end(), thiz.value.begin());
			}

			/**
			 * The slice() method extracts a part of a string.

				The slice() method returns the extracted part in a new string.

				The slice() method does not change the original string.

				The start and end parameters specifies the part of the string to extract.

				The first position is 0, the second is 1, ...

				A negative number selects from the end of the string.
			*/

			auto slice(
				size_t from
			) -> String
			{
				if(from < 0){
					return String{thiz.value.substr(thiz.value.length() + from, thiz.value.size())};
				}
				return String{thiz.value.substr(thiz.value.length() + from, thiz.value.size())};
			}

			/**
			 * The slice() method extracts a part of a string.

				The slice() method returns the extracted part in a new string.

				The slice() method does not change the original string.

				The start and end parameters specifies the part of the string to extract.

				The first position is 0, the second is 1, ...

				A negative number selects from the end of the string.
			*/

			auto slice(
				size_t from,
				size_t to
			) -> String
			{
				if(from < 0 and to < 0){
					return String{thiz.value.substr(thiz.length() + from, thiz.value.size() + to)};
				}
				if(from > 0 and to < 0){
					return String{thiz.value.substr(from, thiz.value.size() + to)};
				}
				if(from < 0 and to > 0){
					return String{thiz.value.substr(thiz.length() + from, to)};
				}
				return String{thiz.value.substr(from, to)};
			}

			/**
			 * The toUpperCase() method converts a string to uppercase letters.

			   The toUpperCase() method does not change the original string.
			*/

			auto toUpperCase(

			) -> String
			{
				auto result = String{thiz.value};
				std::transform(result.value.begin(), result.value.end(), result.value.begin(), ::toupper);
				return result;
			}

			/**
			 * The toLowerCase() method converts a string to lowercase letters.

			   The toLowerCase() method does not change the original string.
			*/

			auto toLowerCase(

			) -> String
			{
				auto result = String{thiz.value};
				std::transform(result.value.begin(), result.value.end(), result.value.begin(), ::toupper);
				return result;
			}

			/**
			 * Returns a string with removed whitespaces
			*/

			auto trim(

			) -> String
			{
				auto start = thiz.value.find_first_not_of(String::whitespace);
				auto end = thiz.value.find_last_not_of(String::whitespace);
				return (start == std::string::npos) ? String{std::string{""}} : String{thiz.value.substr(start, end - start + 1)};
			}

			/**
			 * Returns a string with removed whitespaces from the start
			*/

			auto trimStart(

			) -> String
			{
				auto start = thiz.value.find_first_not_of(String::whitespace);
				return (start == std::string::npos) ? String{std::string{""}} : String{thiz.value.substr(start)};
			}

			/**
			 * Returns a string with removed whitespaces from the end
			*/

			auto trimEnd(

			) -> String
			{
				auto end = thiz.value.find_last_not_of(whitespace);
				return (end == std::string::npos) ? String{std::string{""}} : String{thiz.value.substr(0, end + 1)};
			}
			
			// split by delimeter
			// str: provide string
			// delimiter: provide delimiter to split
			// result: the vector of string after split

			static auto split(
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

			static auto toPosixStyle(
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

			static auto toWindowsStyle(
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

			static auto join(
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

			/*
				T should be unsigned char or char
				str is the string need hash
			*/

			template <typename T>
			static auto convertStringToSpan(
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

			static auto replaceAll(
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

			static auto to_utf8(
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

			static auto to_utf8(
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

			static auto to_utf16(
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

			static auto to_utf32(
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

			static auto to_utf16(
				const std::u32string &s
			) -> std::u16string
			{
				return to_utf16(to_utf8(s));
			}

			/**
			 * s: string to convert (utf16)
			 * return: utf32 string
			*/

			static auto to_utf32(
				const std::u16string &s
			) -> std::u32string 
			{
				return to_utf32(to_utf8(s));
			}

			/**
			 * src: source
			 * return: utf32 string
			*/

			static auto read_with_bom(
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

	};

}