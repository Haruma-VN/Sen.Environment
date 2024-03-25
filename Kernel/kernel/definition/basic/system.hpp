#pragma once

#include "kernel/definition/macro.hpp"
#include "kernel/definition/library.hpp"

namespace Sen::Kernel {

	/**
	 * Architecture test
	*/

	enum class Architecture {
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
			std::string_view command
		) -> void
		{
			std::system(command.data());
			return;
		}

		/**
		 * Test if something is exists in path environment
		*/

		inline static auto is_exists_in_path_environment(
			std::string_view str
		) -> bool
		{
    		return std::getenv(str.data()) != nullptr;
		}

		// TODO add function


			/**
		 * Get path environment
		*/

		inline static auto get_path_environment(
			std::string_view str
		) -> std::string
		{
    		return std::string{std::getenv(str.data())};
		}

		/**
		 * Method launch process and return a string if the result is returned
		*/

		inline static auto execute(
			std::string_view command
		) -> std::string
		{
			#if WINDOWS
				auto buffer = std::array<char, 128>{};
				auto result = std::string{};
				std::unique_ptr<FILE, decltype(&_pclose)> pipe(_popen(command.data(), "r"), _pclose);
				if (!pipe) {
					throw Exception("_popen() failed!", std::source_location::current(), "execute");
				}
				while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr) {
					result += buffer.data();
				}
				return result;
			#else
				auto buffer = std::array<char, 128>{};
				auto result = std::string{};
				auto pipe = std::unique_ptr<FILE, decltype(&pclose)>(popen(command.data(), "r"), pclose);
				if (!pipe) {
					throw Exception("open process failed", std::source_location::current(), "execute");
				}
				while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr) {
					result += buffer.data();
				}
				return result;
			#endif
		}
	};
}
