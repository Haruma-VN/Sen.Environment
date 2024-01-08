#pragma once

#if defined MSVC_COMPILER
#pragma warning(push)
#endif
#if defined CLANG_COMPILER
#pragma clang diagnostic push
#endif

// Because MSVC have no official support for dirent
#if defined MSVC_COMPILER
#include "dependencies/dirent/dirent.h"
#else
#include <dirent.h>
#endif

#if defined MSVC_COMPILER
#pragma warning(pop)
#endif
#if defined CLANG_COMPILER
#pragma clang diagnostic pop
#endif

namespace Sen::Kernel::Dependencies::dirent {
	
}

