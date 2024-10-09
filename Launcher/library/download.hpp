#pragma once
#include "library/utility.hpp"

class Download {

public:

	Download(
	) = default;

	~Download(
	) = default;

	static auto fetch_github(
        std::string &result
	) -> void
	{
        #ifdef _WIN32
                auto curl_command = std::string{ "curl -s https://api.github.com/repos/Haruma-VN/Sen.Environment/releases/tags/release" };
        #else
                auto curl_command = std::string{ "curl -s https://api.github.com/repos/Haruma-VN/Sen.Environment/releases/tags/release" };
        #endif
        auto buffer = std::array<char, 128>{};
        #ifdef _WIN32
                auto pipe = std::unique_ptr<FILE, decltype(&_pclose)>(_popen(curl_command.c_str(), "r"), _pclose);
        #else
                auto pipe = std::unique_ptr<FILE, decltype(&pclose)>(popen(curl_command.c_str(), "r"), pclose);
        #endif
        if (pipe == nullptr) {
           throw std::runtime_error{ "Failed to open pipe." };
        }
        while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr) {
            result += buffer.data();
        }
        return;
	}

    static auto download(
        const std::string& url,      
        const std::string& output     
    ) -> void
    {
        #ifdef _WIN32
                auto curl_command = "curl -s -L -o \"" + output + "\" \"" + url + "\"";
        #else
                auto curl_command = "curl -s -L -o \"" + output + "\" \"" + url + "\"";
        #endif
        auto status = std::system(curl_command.c_str());
        if (status != 0) {
            throw std::runtime_error{ "Curl command failed with status " + std::to_string(status) };
        }
        struct stat buffer;
        if (stat(output.c_str(), &buffer) != 0) {
            throw std::runtime_error{ "Failed to stat file: " + output };
        }
        if (buffer.st_size == 0) {
            throw std::runtime_error{ "Downloaded file is empty: " + output };
        }
        return;
    }


};