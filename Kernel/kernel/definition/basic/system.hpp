#pragma once

#include "kernel/definition/macro.hpp"
#include "kernel/definition/library.hpp"

namespace Sen::Kernel {

	/**
	 * Architecture test
	*/

	enum Architecture {
		X64,
		ARM,
		INTEL,
		X86,
		UNKNOWN,
		ARM64,
	};

	/**
	 * Process call
	*/

	struct Process {

		/**
		 * Method only launch process, will not return result
		*/

		inline static auto run(
			const std::string & command
		) -> void
		{
			std::system(command.c_str());
			return;
		}

		/**
		 * Test if something is exists in path environment
		*/

		inline static auto is_exists_in_path_environment(
			const std::string & str
		) -> bool
		{
    		return std::getenv(str.c_str()) != nullptr;
		}

		/**
		 * Method launch process and return a string if the result is returned
		*/

		inline static auto execute(
			const std::string & command
		) -> std::string
		{
			#if WINDOWS
				auto buffer = std::array<char, 128>{};
				auto result = std::string{};
				std::unique_ptr<FILE, decltype(&_pclose)> pipe(_popen(command.c_str(), "r"), _pclose);
				if (!pipe) {
					throw std::runtime_error("_popen() failed!");
				}
				while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr) {
					result += buffer.data();
				}
				return result;
			#else
				auto buffer = std::array<char, 128>{};
				auto result = std::string{};
				std::unique_ptr<FILE, decltype(&pclose)> pipe(popen(command.c_str(), "r"), pclose);
				if (!pipe) {
					throw std::runtime_error("open process failed");
				}
				while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr) {
					result += buffer.data();
				}
				return result;
			#endif
		}
	};
}
