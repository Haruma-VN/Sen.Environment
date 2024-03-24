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
	
	inline auto to_windows_style(
		const std::string & source
	) -> std::string const
	{
		return String::to_windows_style(source);
	}

	
	/**
	 * Convert to: "a/b/c" if it's "a\\b\\c"
	*/

	inline auto to_posix_style(
		const std::string & source
	) -> std::string const
	{
		return String::to_posix_style(source);
	}

	/**
	 * Return the absolute path
	 * Example: "./test.js" -> "{DISK CONTAIN}/what/test.js"
	*/

	inline auto absolute(
		const std::string & source
	) -> std::string const
	{
		#if WINDOWS
		return String::utf16_to_utf8(fs::absolute(source).wstring());
		#else
		return fs::absolute(source).string();
		#endif
	}

	/**
	 * Get all the parent directory: 
	 * Example: "{DISK_CONTAIN}/what/hello/main.js" -> "{DISK_CONTAIN}/what/hello"
	*/

	inline auto getParents(
		const std::string &source
	) -> std::string const
	{
		#if WINDOWS
		return String::utf16_to_utf8(fs::path(source).parent_path().wstring());
		#else
		return fs::path(source).parent_path().string();
		#endif
	}

	/**
	 * Get the file name of the path
	 * Example: "{DISK_CONTAIN}/what/hello/main.js" -> "main.js"
	*/


	inline auto getFileName(
		const std::string &source
	) -> std::string const
	{
		#if WINDOWS
		return String::utf16_to_utf8(fs::path(source).filename().wstring());
		#else
		return fs::path(source).filename().string();
		#endif
	}

	/**
	 * Get the file name of the path
	 * Example: "{DISK_CONTAIN}/what/hello/main.js" -> "hello"
	*/

	inline auto getNearestParent(
		const std::string &source
	) -> std::string const
	{
		#if WINDOWS
		return String::utf16_to_utf8(fs::path(source).parent_path().filename().wstring());
		#else
		return fs::path(source).parent_path().filename().string();
		#endif
	}

	/**
	 * Get extension only
	 * Example: "{DISK_CONTAIN}/what/hello/main.js" -> "js"
	*/

	inline auto getExtension(
		const std::string &source
	) -> std::string const
	{
		#if WINDOWS
		return String::utf16_to_utf8(fs::path(source).extension().wstring());
		#else
		return fs::path(source).extension().string();
		#endif
	}

	/**
	 * Get file name only
	 * Example: "{DISK_CONTAIN}/what/hello/main.js" -> "main"
	*/

	inline auto getFileNameWithoutExtension(
		const std::string &source
	) -> std::string const
	{
		#if WINDOWS
		return String::utf16_to_utf8(fs::path(source).stem().wstring());
		#else
		return fs::path(source).stem().string();
		#endif
	}


	/**
	 * Convert to posix type
	*/

	inline auto normalize(
		const std::string &source
	) -> std::string const
	{
		#if WINDOWS
			return Path::to_windows_style(Path::absolute(source));
		#else
			return Path::absolute(Path::to_posix_style(source));
		#endif
	}

	/**
	 * Convert list of path to path
	*/

	inline auto join(
		const std::string & source
	) -> std::string
	{
		#if WINDOWS
		return String::utf16_to_utf8(fs::path(source).wstring());
		#else
		return fs::path(source).string();
		#endif
	}

	/**
	 * Convert list of path to path
	*/

	inline auto join(
		std::initializer_list<std::string> list
	) -> std::string
	{
		#if WINDOWS
		auto path = fs::path(std::wstring{});
		for(auto & element : list){
			path.append(String::utf8_to_utf16(element));
		}
		return String::utf16_to_utf8(path.wstring());
		#else
		auto path = fs::path(std::string{});
		for(auto & element : list){
			path.append(element);
		}
		return path.string();
		#endif
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
				const Args&... args
			) -> std::string
			{
				#if WINDOWS && !defined MSVC_COMPILER
                    static_assert(false, "msvc compiler is required on windows");
				#endif
				auto result = std::filesystem::path{};
				(result /= ... /= args);
				#if WINDOWS
					return String::utf16_to_utf8(result.wstring());
				#else
					return result.string();
				#endif
			}

			inline static auto join(
				const std::vector<std::string> & args
			) -> std::string
			{
				auto result = std::filesystem::path{};
				for(auto & arg : args) {
					result /= String::utf8_to_utf16(arg);
				}
				#if WINDOWS
					auto posix_string = result.wstring();
					std::replace(posix_string.begin(), posix_string.end(), L'\\', L'/');
				#else
					auto posix_string = result.string();
					std::replace(posix_string.begin(), posix_string.end(), '\\', '/');
				#endif
				#if WINDOWS
					return String::utf16_to_utf8(result.wstring());
				#else
					return result.string();
				#endif
			}


			/**
			 * Get basename of a file
			*/

			inline static auto basename(
				const std::string & source
			) -> std::string
			{
				#if WINDOWS
					return String::utf16_to_utf8(std::filesystem::path{String::utf8_to_utf16(source)}.filename().wstring());
				#else
					return std::filesystem::path{source}.filename().string();
				#endif
			}

			/**
			 * Without extension
			*/

			inline static auto base_without_extension(
				const std::string & source
			) -> std::string
			{
				#if WINDOWS
					return String::utf16_to_utf8(std::filesystem::path{String::utf8_to_utf16(source)}.stem().wstring());
				#else
					return std::filesystem::path{source}.stem().string();
				#endif
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
				#if WINDOWS
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
				#if WINDOWS
					return String::utf16_to_utf8(std::filesystem::path{String::utf8_to_utf16(source)}.parent_path().wstring());
				#else
					return std::filesystem::path{source}.parent_path().string();
				#endif
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
				#if WINDOWS
					return String::utf16_to_utf8(std::filesystem::path{String::utf8_to_utf16(source)}.lexically_normal().wstring());
				#else
					return std::filesystem::path{source}.lexically_normal().string();
				#endif
			}

			/**
			 * Get full path
			*/
			
			inline static auto resolve(
				const std::string & source
			) -> std::string
			{
				#if WINDOWS
					return String::utf16_to_utf8(std::filesystem::absolute(String::utf8_to_utf16(source)).wstring());
				#else
					return std::filesystem::absolute(source).string();
				#endif
			}

			/**
			 * Get extension name
			*/
			
			inline static auto extname(
				const std::string & source
			) -> std::string
			{
				#if WINDOWS
					return String::utf16_to_utf8(std::filesystem::path{String::utf8_to_utf16(source)}.extension().wstring());
				#else
					return std::filesystem::path{source}.extension().string();
				#endif
			}

			/**
			 * if path is absolute path
			*/
			
			inline static auto is_absolute(
				const std::string & source
			) -> bool
			{
				#if WINDOWS
					return source == String::utf16_to_utf8(std::filesystem::path(String::utf8_to_utf16(source)).extension().wstring());
				#else
					return source == std::filesystem::path(source).string();
				#endif
			}

			/**
			 * Get relative path
			*/
			
			inline static auto relative(
				const std::string & from,
				const std::string & to
			) -> std::string
			{
				#if WINDOWS
					return String::utf16_to_utf8(std::filesystem::relative(String::utf8_to_utf16(to), String::utf8_to_utf16(from)).wstring());
				#else
					return std::filesystem::relative(to, from).string();
				#endif
			}

			inline static auto is_file(
				std::string_view source
			) -> bool
			{
				#if WINDOWS
					auto result = std::filesystem::is_regular_file(String::utf8_to_utf16(source.data()));
				#else
					auto result = std::filesystem::is_regular_file(source.data());
				#endif
				return result;
			}

			inline static auto is_directory (
				std::string_view source
			) -> bool
			{
				#if WINDOWS
					auto result = std::filesystem::is_directory(String::utf8_to_utf16(source.data()));
				#else
					auto result = std::filesystem::is_directory(source.data());
				#endif
				return result;
			}

			inline static auto rename (
				std::string_view source,
				std::string_view destination
			) -> void
			{
				#if WINDOWS
					std::filesystem::rename(std::filesystem::path{String::utf8_to_utf16(source.data())}, std::filesystem::path{String::utf8_to_utf16(destination.data())});
				#else
					std::filesystem::rename(std::filesystem::path{source.data()}, std::filesystem::path{destination.data()});
				#endif
				return;
			}

			inline static auto copy (
				std::string_view source,
				std::string_view destination
			) -> void
			{
				#if WINDOWS
					std::filesystem::copy(std::filesystem::path{String::utf8_to_utf16(source.data())}, std::filesystem::path{String::utf8_to_utf16(destination.data())});
				#else
					std::filesystem::copy(std::filesystem::path{source.data()}, std::filesystem::path{destination.data()});
				#endif
				return;
			}

			inline static auto remove (
				std::string_view source
			) -> void
			{
				#if WINDOWS
					std::filesystem::remove(std::filesystem::path{String::utf8_to_utf16(source.data())});
				#else
					std::filesystem::remove(std::filesystem::path{source.data()});
				#endif
				return;
			}

			inline static auto remove_all (
				std::string_view source
			) -> void
			{
				#if WINDOWS
					std::filesystem::remove_all(std::filesystem::path{String::utf8_to_utf16(source.data())});
				#else
					std::filesystem::remove_all(std::filesystem::path{source.data()});
				#endif
				return;
			}
	};
}