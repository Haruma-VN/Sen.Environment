#pragma once

#if defined MSVC_COMPILER
#pragma warning(push)
#pragma warning(disable:4365)
#pragma warning(disable:4582)
#pragma warning(disable:4623)
#pragma warning(disable:4625)
#pragma warning(disable:4626)
#pragma warning(disable:5026)
#pragma warning(disable:5027)
#pragma warning(disable:5045)
#endif
#if defined CLANG_COMPILER
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wdocumentation-unknown-command"
#pragma clang diagnostic ignored "-Wsigned-enum-bitfield"
#pragma clang diagnostic ignored "-Wreserved-identifier"
#pragma clang diagnostic ignored "-Wundefined-func-template"
#endif


#include "dependencies/fmt/include/fmt/format.h"
#include "dependencies/fmt/include/fmt/core.h"

#if defined MSVC_COMPILER
#pragma warning(pop)
#endif
#if defined CLANG_COMPILER
#pragma clang diagnostic pop
#endif

namespace Sen::Kernel::Dependencies::fmt 
{
	
	// the namespace of fmt library is fmt

	using namespace ::fmt;

	
}
