#pragma once

#include "kernel/definition/macro.hpp"
#include "kernel/definition/library.hpp"
#include "kernel/definition/string/common.hpp"

namespace Sen::Kernel::Path 
{

	// file system C++

	namespace fs = std::filesystem;

	/**
	 * Convert to: "a\\b\\c" if it's "a/b/c"
	*/
	
	inline auto toWindowsStyle(
		const string &mPath
	) -> string const
	{
		return String::toWindowsStyle(mPath);
	}

	
	/**
	 * Convert to: "a/b/c" if it's "a\\b\\c"
	*/

	inline auto toPosixStyle(
		const string &mPath
	) -> string const
	{
		return String::toPosixStyle(mPath);
	}

	/**
	 * Return the absolute path
	 * Example: "./test.js" -> "{DISK CONTAIN}/what/test.js"
	*/

	inline auto absolute(
		const string &mPath
	) -> string const
	{
		return fs::absolute(mPath).string();
	}

	/**
	 * Get all the parent directory: 
	 * Example: "{DISK_CONTAIN}/what/hello/main.js" -> "{DISK_CONTAIN}/what/hello"
	*/

	inline auto getParents(
		const string &mPath
	) -> string const
	{
		return fs::path(mPath).parent_path().string();
	}

	/**
	 * Get the file name of the path
	 * Example: "{DISK_CONTAIN}/what/hello/main.js" -> "main.js"
	*/


	inline auto getFileName(
		const string &mPath
	) -> string const
	{
		return fs::path(mPath).filename().string();
	}

	/**
	 * Get the file name of the path
	 * Example: "{DISK_CONTAIN}/what/hello/main.js" -> "hello"
	*/

	inline auto getNearestParent(
		const string &mPath
	) -> string const
	{
		return fs::path(mPath).parent_path().filename().string();
	}

	/**
	 * Get extension only
	 * Example: "{DISK_CONTAIN}/what/hello/main.js" -> "js"
	*/

	inline auto getExtension(
		const string &mPath
	) -> string const
	{
		return fs::path(mPath).extension().string();
	}

	/**
	 * Get file name only
	 * Example: "{DISK_CONTAIN}/what/hello/main.js" -> "main"
	*/

	inline auto getFileNameWithoutExtension(
		const string &mPath
	) -> string const
	{
		return fs::path(mPath).stem().string();
	}


	/**
	 * Convert to posix type
	*/

	inline auto normalize(
		const string &mPath
	) -> string const
	{
		return Path::absolute(Path::toPosixStyle(mPath));
	}

}