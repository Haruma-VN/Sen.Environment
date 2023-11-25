#pragma once

#include "kernel/definition/macro.hpp"
#include "kernel/definition/library.hpp"
#include "kernel/definition/string/common.hpp"

namespace Sen::Kernel::Path 
{
	
	inline auto toWindowsStyle(
		string mPath
	) -> string
	{
		return String::toWindowsStyle(mPath);
	}

	inline auto toPosixStyle(
		string mPath
	) -> string
	{
		return String::toPosixStyle(mPath);
	}

	inline auto normalize(
		string mPath
	) -> string
	{
		return Path::toPosixStyle(mPath);
	}
}