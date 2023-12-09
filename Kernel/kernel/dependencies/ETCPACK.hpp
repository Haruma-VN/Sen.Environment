#pragma once

#if defined MSVC_COMPILER
#pragma warning(push)
#endif
#if defined CLANG_COMPILER
#pragma clang diagnostic push
#endif

#include "dependencies/ETCPACK/_etcpack.hpp"

#if defined MSVC_COMPILER
#pragma warning(pop)
#endif
#if defined CLANG_COMPILER
#pragma clang diagnostic pop
#endif

namespace Sen::Kernel::Dependencies::ETCPACK {

	// using third
	
	inline constexpr auto decompressBlockETC2c = ::decompressBlockETC2c;

	inline constexpr auto decompressBlockAlphaC = ::decompressBlockAlphaC;

}
