#pragma once 

#include "kernel/definition/library.hpp"
#include "kernel/definition/assert.hpp"
#include "kernel/definition/string/common.hpp"

namespace Sen::Kernel::FileSystem {

	// ifstream is using without namespace std

	using std::ifstream;

	// ofstream is using without namespace std

	using std::ofstream;

	// string is using without namespace std

	using std::string;

	// string stream is buffer

	using std::stringstream;

	// give file path to open
	// return: the file data as string

	inline auto readFile(
		string filepath
	) -> string
	{
		auto file = ifstream{};
		file.open(filepath);
		try_assert(!file.fail(), fmt::format("Could not open file: {}", filepath));
		auto buffer = stringstream{};
		buffer << file.rdbuf();
		auto s = buffer.str();
		file.close();
		return s;
	}	


	// path: file path to open
	// content: content to write
	// return: the file has been written

	inline auto writeFile(
		string filepath, 
		string content
	) -> void 
	{
		auto file = ofstream(filepath);
		file << content;
		file.close();
		return;
	}

	// using namespace nlohmann -> ignore nlohmann::
	
	using namespace nlohmann;

	// Provide file path to read json
	// return: if the json is valid, the json data will be parsed as object

	inline auto readJson(
		string filePath
	) -> json 
	{
		auto file = ifstream(filePath);
		auto jsonData = json::parse(file);
		file.close();
		return jsonData;
	}

	// Provide file path to write
	// Provide json content to serialize & write
	// return: writed json content

	inline auto writeJson(
		string filePath,
		json content
	) -> void
	{
		writeFile(filePath, content.dump(1, '\t'));
		return;
	}

	// Provide file path to write
	// Provide json content to serialize & write
	// indent: provide indentation
	// indent_char: indentation in segment line
	// return: writed json content

	inline auto writeJson(
		string filePath,
		json content,
		int indent,
		char indent_char
	) -> void
	{
		writeFile(filePath, content.dump(indent, indent_char));
		return;
	}

	// Provide file path to write
	// Provide json content to serialize & write
	// indent_char: indentation in segment line
	// return: writed json content

	inline auto writeJson(
		string filePath,
		json content,
		char indent_char
	) -> void
	{
		writeFile(filePath, content.dump(1, indent_char));
		return;
	}

	// Provide file path to write
	// Provide json content to serialize & write
	// indent: provide indentation
	// indent_char: indentation in segment line
	// ensure ascii: will it ensure ascii?
	// return: writed json content

	inline auto writeJson(
		string filePath,
		json content,
		int indent,
		char indent_char,
		bool ensureAscii
	) -> void
	{
		writeFile(filePath, content.dump(indent, indent_char, ensureAscii));
		return;
	}

	// utf16-le 

	using std::wstring;

	// wstring

	using std::wifstream;

	// file input stream

	using std::ios;

	// convert

	using std::codecvt_utf8;

	// wstring stream

	using std::wstringstream;

	// locale

	using std::locale;

	// vector
	using std::vector;

	// file path: the file uses utf16le encoding
	// return: the utf16le string

	inline auto readFileByUtf16LE(
		string filePath
	) -> wstring
	{
		auto wif = wifstream(filePath, ios::binary);
		try_assert(!wif.fail(), fmt::format("Could not open file: {}", filePath));
		wif.imbue(locale(wif.getloc(), new codecvt_utf8<wchar_t, 0x10ffff>));
		auto wss = wstringstream{};
		wss << wif.rdbuf();
		return wss.str();
	}

	// Utf16-LE

	using std::codecvt_utf16;

	// Little endian

	using std::little_endian;

	// Output stream UTF16-LE

	using std::wofstream;

	// filePath: the file path to write
	// data: utf16-le charset
	// return: the data has been written

	inline auto writeFileByUtf16LE(
		string filePath,
		wstring data
	) -> void
	{
		auto utf16le_locale = static_cast<locale>(locale(locale::classic(), new codecvt_utf16<wchar_t, 0x10ffff, little_endian>));
		auto file = wofstream(filePath);
		file.imbue(utf16le_locale);
		file << data;
		file.close();
		return;
	}

	// file system

	namespace fs = std::filesystem;

	// directoryPath: folder path
	// return: create directory

	inline auto createDirectory(
		string directoryPath
	) -> void
	{
		auto status = fs::create_directories(directoryPath);
		try_assert(!status, fmt::format("Cannot make directory: {}", directoryPath));
		return;
	}

	// filePath: the file path to write file
	// content: content to write
	// return: the file has been written

	inline auto outFile(
		string filePath,
		string content
	) -> void
	{
		filePath = String::toPosixStyle(filePath);
		auto data = String::split(filePath, "/");
		auto last = data.at(data.size() - 1);
		data.pop_back();
		createDirectory(String::join(data, "/"));
		writeFile(filePath, content);
		return;
	}

	// filePath: the file path to write file
	// content: json object
	// return: the file has been written to json

	inline auto outJson(
		string filePath,
		json content
	) -> void
	{
		outFile(filePath, content.dump(1, '\t'));
		return;
	}

	// using runtime error exception

	using std::runtime_error;

	// stream size

	using std::streamsize;

	// filePath: the path to read
	// return: the binary file readed

	
	template <typename T>
	inline auto readBinary(
		string filePath
	) -> vector<T>
	{
		auto file = ifstream(filePath, ios::binary);
		if(!file)
		{
			throw runtime_error(fmt::format("Could not open file: {}", filePath));
		}
		file.seekg(0, ios::end);
		auto size = static_cast<streamsize>(file.tellg());
		file.seekg(0, ios::beg);
		auto data = vector<T>(size);
		if (!file.read(reinterpret_cast<char*>(data.data()), size))
		{
			throw runtime_error(fmt::format("Could not read file: {}", filePath));
		}
		file.close();
		return data;	
	}

	// dirPath: directory to read
	// return: everything inside it even directory or file

	inline auto readDirectory(
		string directoryPath
	) -> vector<string>
	{
		auto result = vector<string>{};
		for(auto &c : fs::directory_iterator(String::toPosixStyle(directoryPath)))
		{
			result.push_back(String::toPosixStyle(c.path().string()));
		}
		return result;
	}

	// dirPath: directory to read
	// return: only files inside

	inline auto readDirectoryOnlyFile(
		string directoryPath
	) -> vector<string>
	{
		auto result = vector<string>{};
		for(auto &c : fs::directory_iterator(String::toPosixStyle(directoryPath)))
		{
			if(c.is_regular_file()){
				result.push_back(String::toPosixStyle(c.path().string()));
			}
		}
		return result;
	}

	// dirPath: directory to read
	// return: only dirs inside

	inline auto readDirectoryOnlyDirectory(
		string directoryPath
	) -> vector<string>
	{
		auto result = vector<string>{};
		for(auto &c : fs::directory_iterator(String::toPosixStyle(directoryPath)))
		{
			if(c.is_directory()){
				result.push_back(String::toPosixStyle(c.path().string()));
			}
		}
		return result;
	}


	// dirPath: directory to read
	// return: only files inside nested directories

	inline auto readWholeDirectory(
		string directoryPath
	) -> vector<string>
	{
		auto result = vector<string>{};
		for(auto &c : fs::directory_iterator(String::toPosixStyle(directoryPath)))
		{
			if(c.is_directory()){
				for(auto &e : readWholeDirectory(String::toPosixStyle(c.path().string())))
				{
					result.push_back(String::toPosixStyle(e));
				}
			}
			else{
				result.push_back(String::toPosixStyle(c.path().string()));
			}
		}
		return result;
	}

}