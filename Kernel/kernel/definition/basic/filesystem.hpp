#pragma once 

#include "kernel/definition/library.hpp"
#include "kernel/definition/assert.hpp"
#include "kernel/definition/basic/string.hpp"
#include "kernel/definition/basic/path.hpp"

namespace Sen::Kernel::FileSystem
{

	template <typename T>
	concept CharacterBufferView = std::is_same_v<T, char> || std::is_same_v<T, unsigned char>;

	// give file path to open
	// return: the file data as string

	inline static auto read_file(
		const std::string & filepath
	) -> std::string
	{
        auto file = std::ifstream(filepath);
        if (!file.is_open()) {
            throw Exception("Could not open file: " + filepath);
        }
        auto buffer = std::stringstream{};
        buffer << file.rdbuf();
        return buffer.str();
    }


	// path: file path to open
	// content: content to write
	// return: the file has been written

	inline static auto write_file(
		const std::string &filepath, 
		const std::string &content
	) -> void 
	{
		auto file = std::ofstream(filepath);
		if (!file.is_open()) {
            throw Exception("Could not open file: " + filepath);
        }
		file << content;
		file.close();
		return;
	}

	// Provide file path to read json
	// return: if the json is valid, the json data will be parsed as object

	inline static auto read_json(
		const std::string &filePath
	) -> nlohmann::ordered_json const
	{
		auto file = std::ifstream(filePath);
		if (!file.is_open()) {
            throw Exception("Could not open file: " + filePath);
        }
		auto jsonData = nlohmann::ordered_json::parse(file);
		file.close();
		return jsonData;
	}

	// Provide file path to write
	// Provide json content to serialize & write
	// return: writed json content

	inline static auto write_json(
		const std::string & filePath,
		const nlohmann::ordered_json & content
	) -> void
	{
		auto file = std::ofstream(filePath);
		if (!file.is_open()) {
            throw Exception("Could not open file: " + filePath);
        }
		file << content.dump(1, '\t');
		file.close();
		return;
	}

	// Provide file path to write
	// Provide json content to serialize & write
	// indent: provide indentation
	// indent_char: indentation in segment line
	// return: writed json content

	inline static auto write_json(
		const std::string &filePath,
		const nlohmann::ordered_json &content,
		const int &indent,
		const char &indent_char
	) -> void
	{
		auto file = std::ofstream(filePath);
		if (!file.is_open()) {
            throw Exception("Could not open file: " + filePath);
        }
		file << content.dump(indent, indent_char);
		file.close();
		return;
	}

	// Provide file path to write
	// Provide json content to serialize & write
	// indent_char: indentation in segment line
	// return: writed json content

	inline static auto write_json(
		const string &filePath,
		const nlohmann::ordered_json &content,
		const char &indent_char
	) -> void
	{
		auto file = std::ofstream(filePath);
		if (!file.is_open()) {
            throw Exception("Could not open file: " + filePath);
        }
		file << content.dump(1, indent_char);
		file.close();
		return;
	}

	// Provide file path to write
	// Provide json content to serialize & write
	// indent: provide indentation
	// indent_char: indentation in segment line
	// ensure ascii: will it ensure ascii?
	// return: writed json content

	inline static auto write_json(
		const string &filePath,
		const nlohmann::ordered_json &content,
		const int &indent,
		const char &indent_char,
		const bool &ensureAscii
	) -> void
	{
		auto file = std::ofstream(filePath);
		if (!file.is_open()) {
            throw Exception("Could not open file: " + filePath);
        }
		file << content.dump(indent, indent_char, ensureAscii);
		file.close();
		return;
	}

	// file path: the file uses utf16le encoding
	// return: the utf16le string

	inline static auto readFileByUtf16LE(
		const std::string &filePath
	) -> std::wstring const
	{
		auto wif = std::wifstream(filePath, std::ios::binary);
		try_assert(!wif.fail(), fmt::format("Could not open file: {}", filePath));
		wif.imbue(std::locale(wif.getloc(), new std::codecvt_utf8<wchar_t, 0x10ffff>));
		auto wss = std::wstringstream{};
		wss << wif.rdbuf();
		return wss.str();
	}

	// filePath: the file path to write
	// data: utf16-le charset
	// return: the data has been written

	inline static auto write_fileByUtf16LE(
		const std::string & filePath,
		const std::wstring & data
	) -> void
	{
		auto utf16le_locale = static_cast<std::locale>(std::locale(std::locale::classic(), new std::codecvt_utf16<wchar_t, 0x10ffff, std::little_endian>));
		auto file = std::wofstream(filePath);
		file.imbue(utf16le_locale);
		file << data;
		file.close();
		return;
	}

	// file system

	namespace fs = std::filesystem;

	// directoryPath: folder path
	// return: create directory

	inline static auto createDirectory(
		const std::string &directoryPath
	) -> void
	{
		if(fs::is_directory(directoryPath)){
			return;
		}
		auto status = fs::create_directories(directoryPath);
		return;
	}

	// filePath: the file path to write file
	// content: content to write
	// return: the file has been written

	inline static auto outFile(
		const std::string &filePath,
		const std::string &content
	) -> void
	{
		auto temporary = Path::normalize(filePath);
		auto data = String::split(temporary, "/");
		auto last = data.at(data.size() - 1);
		data.pop_back();
		createDirectory(String::join(data, "/"));
		write_file(filePath, content);
		return;
	}

	// filePath: the file path to write file
	// content: json object
	// return: the file has been written to json

	inline static auto outJson(
		const std::string &filePath,
		const nlohmann::ordered_json &content
	) -> void
	{
		outFile(filePath, content.dump(1, '\t'));
		return;
	}

	
	template <CharacterBufferView T> 
	inline static auto read_binary(
		const std::string &filePath
	) -> std::vector<T> const
	{
		auto file = std::ifstream(filePath, std::ios::binary);
		if(!file)
		{
			throw Exception(fmt::format("Could not open file: {}", filePath));
		}
		file.seekg(0, std::ios::end);
		auto size = static_cast<std::streamsize>(file.tellg());
		file.seekg(0, std::ios::beg);
		auto data = std::vector<T>(size);
		if (!file.read(reinterpret_cast<char*>(data.data()), size))
		{
			throw Exception(fmt::format("Could not read file: {}", filePath));
		}
		file.close();
		return data;	
	}

	// dirPath: directory to read
	// return: everything inside it even directory or file

	inline static auto readDirectory(
		const std::string &directoryPath
	) -> std::vector<std::string> const
	{
		auto result = std::vector<std::string>{};
		for(auto &c : fs::directory_iterator(Path::normalize(directoryPath)))
		{
			result.emplace_back(Path::normalize(c.path().string()));
		}
		return result;
	}

	// dirPath: directory to read
	// return: only files inside

	inline static auto readDirectoryOnlyFile(
		const string &directoryPath
	) -> std::vector<string> const
	{
		auto result = std::vector<string>{};
		for(auto &c : fs::directory_iterator(Path::normalize(directoryPath)))
		{
			if(c.is_regular_file()){
				result.emplace_back(Path::normalize(c.path().string()));
			}
		}
		return result;
	}

	// dirPath: directory to read
	// return: only dirs inside

	inline static auto readDirectoryOnlyDirectory(
		const std::string &directoryPath
	) -> std::vector<std::string> const
	{
		auto result = std::vector<std::string>{};
		for(auto &c : fs::directory_iterator(Path::normalize(directoryPath)))
		{
			if(c.is_directory()){
				result.emplace_back(Path::normalize(c.path().string()));
			}
		}
		return result;
	}


	// dirPath: directory to read
	// return: only files inside nested directories

	inline static auto readWholeDirectory(
		const std::string &directoryPath
	) -> std::vector<std::string> const
	{
		auto result = std::vector<string>{};
		for(auto &c : fs::directory_iterator(Path::normalize(directoryPath)))
		{
			if(c.is_directory()){
				for(auto &e : readWholeDirectory(Path::normalize(c.path().string())))
				{
					result.emplace_back(Path::normalize(e));
				}
			}
			else{
				result.emplace_back(Path::normalize(c.path().string()));
			}
		}
		return result;
	}

	/**
	 * outFile: the output file to write
	 * data: data to write
	 * return: the written file
	*/


	template <CharacterBufferView T>
	inline static auto write_binary(
		const std::string &outFile,
		const std::vector<T> &data
	) -> void
	{
		auto out = std::ofstream(outFile, std::ios::binary);
		if(!out.is_open()){
			throw Exception(fmt::format("Unable to open: {}", outFile));
		}
		out.write(reinterpret_cast<const char *>(data.data()), data.size());
		out.close();
		return;
	}

	/**
	 * file path: the file path to read
	 * return: xml document
	*/

	inline static auto readXML(
		const string &filePath
	) -> tinyxml2::XMLDocument* const
	{
		auto* xml = new tinyxml2::XMLDocument{};
		auto data = FileSystem::read_file(filePath);
		auto eResult = xml->Parse(data.c_str(), data.size());
		try_assert(eResult == tinyxml2::XML_SUCCESS, fmt::format("XML Read error: {}", filePath));
		return xml;
	}

	/**
	 * file path: the file path to write
	 * xml document: the xml document
	 * notice: the function will delete the xml document after write
	 * return: xml dumped data
	*/

	inline static auto writeXML(
		const string &filePath,
		tinyxml2::XMLDocument* &data
	) -> void
	{
		auto printer = tinyxml2::XMLPrinter{};
		data->Print(&printer);
		FileSystem::write_file(filePath, std::string{printer.CStr()});
		delete data;
		return;
	}

}