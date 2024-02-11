#pragma once

#include "kernel/definition/macro.hpp"
#include "kernel/definition/library.hpp"
#include "kernel/definition/basic/string.hpp"

namespace Sen::Kernel::Path 
{

	// file system C++

	namespace fs = std::filesystem;

	/**
	 * Convert to: "a\\b\\c" if it's "a/b/c"
	*/
	
	inline auto toWindowsStyle(
		const std::string & source
	) -> std::string const
	{
		return String::toWindowsStyle(source);
	}

	
	/**
	 * Convert to: "a/b/c" if it's "a\\b\\c"
	*/

	inline auto toPosixStyle(
		const std::string & source
	) -> std::string const
	{
		return String::toPosixStyle(source);
	}

	/**
	 * Return the absolute path
	 * Example: "./test.js" -> "{DISK CONTAIN}/what/test.js"
	*/

	inline auto absolute(
		const std::string & source
	) -> std::string const
	{
		return fs::absolute(source).string();
	}

	/**
	 * Get all the parent directory: 
	 * Example: "{DISK_CONTAIN}/what/hello/main.js" -> "{DISK_CONTAIN}/what/hello"
	*/

	inline auto getParents(
		const std::string &source
	) -> std::string const
	{
		return fs::path(source).parent_path().string();
	}

	/**
	 * Get the file name of the path
	 * Example: "{DISK_CONTAIN}/what/hello/main.js" -> "main.js"
	*/


	inline auto getFileName(
		const std::string &source
	) -> std::string const
	{
		return fs::path(source).filename().string();
	}

	/**
	 * Get the file name of the path
	 * Example: "{DISK_CONTAIN}/what/hello/main.js" -> "hello"
	*/

	inline auto getNearestParent(
		const std::string &source
	) -> std::string const
	{
		return fs::path(source).parent_path().filename().string();
	}

	/**
	 * Get extension only
	 * Example: "{DISK_CONTAIN}/what/hello/main.js" -> "js"
	*/

	inline auto getExtension(
		const std::string &source
	) -> std::string const
	{
		return fs::path(source).extension().string();
	}

	/**
	 * Get file name only
	 * Example: "{DISK_CONTAIN}/what/hello/main.js" -> "main"
	*/

	inline auto getFileNameWithoutExtension(
		const std::string &source
	) -> std::string const
	{
		return fs::path(source).stem().string();
	}


	/**
	 * Convert to posix type
	*/

	inline auto normalize(
		const std::string &source
	) -> std::string const
	{
		return Path::absolute(Path::toPosixStyle(source));
	}

	/**
	 * Convert list of path to path
	*/

	inline auto join(
		const std::string & source
	) -> std::string
	{
		return fs::path(source).string();
	}

	/**
	 * Convert list of path to path
	*/

	inline auto join(
		std::initializer_list<std::string> list
	) -> std::string
	{
		auto path = fs::path(std::string{});
		for(auto & element : list){
			path.append(element);
		}
		return path.string();
	}

	struct Format {

		public:

			std::string dir;

			std::string base;

			explicit Format(
				const std::string & dir,
				const std::string & base
			) : dir(dir), base(base)
			{

			}

			~Format(

			) = default;
	};

	// JS Methods
	
	struct Script {

		public:

			/**
			 * Provide arguments
			*/

			template<typename... Args>
			inline static auto join(
				Args... args
			) -> std::string
			{
				auto result = std::filesystem::path{};
				(result /= ... /= args);
				return result.string();
			}

			inline static auto join(
				const std::vector<std::string> & args
			) -> std::string
			{
				auto result = std::filesystem::path{};
				for(const auto & arg : args) {
					result /= arg;
				}
				auto posix_string = result.string();
				std::replace(posix_string.begin(), posix_string.end(), '\\', '/');
				return posix_string;
			}


			/**
			 * Get basename of a file
			*/

			inline static auto basename(
				const std::string & source
			) -> std::string
			{
				return std::filesystem::path{source}.filename().string();
			}

			/**
			 * Without extension
			*/

			inline static auto base_without_extension(
				const std::string & source
			) -> std::string
			{
				return std::filesystem::path(source).stem().string();
			}

			/**
			 * Without extension
			*/

			inline static auto except_extension(
				const std::string & source
			) -> std::string
			{
				return fmt::format("{}/{}", dirname(source), base_without_extension(source));
			}

			/**
			 * Return current platform delimiter
			*/

			inline static auto delimiter(

			) -> std::string
			{
				#if _WIN32
					return std::string{"\\"};
				#else
					return std::string{"/"};
				#endif
			}

			/**
			 * Return parent directory
			*/

			inline static auto dirname(
				const std::string & source
			) -> std::string
			{
				return std::filesystem::path{source}.parent_path().string();
			}

			/**
			 * Join the format
			*/

			inline static auto format(
				const Format & that
			) -> std::string
			{
				return join(that.dir, that.base);
			}

			/**
			 * Normalize the path
			*/

			inline static auto normalize(
				const std::string & source
			) -> std::string
			{
				return std::filesystem::path{source}.lexically_normal().string();
			}

			/**
			 * Get full path
			*/
			
			inline static auto resolve(
				const std::string & source
			) -> std::string
			{
				return std::filesystem::absolute(source).string();
			}

			/**
			 * Get extension name
			*/
			
			inline static auto extname(
				const std::string & source
			) -> std::string
			{
				return std::filesystem::path(source).extension().string();
			}

			/**
			 * if path is absolute path
			*/
			
			inline static auto is_absolute(
				const std::string & source
			) -> bool
			{
				return source == std::filesystem::path(source).string();
			}

			/**
			 * Get relative path
			*/
			
			inline static auto relative(
				const std::string & from,
				const std::string & to
			) -> std::string
			{
				return std::filesystem::relative(to, from).string();
			}
			
	};
}