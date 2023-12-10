#pragma once

#if defined MSVC_COMPILER
#pragma warning(push)
#endif
#if defined CLANG_COMPILER
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wreserved-macro-identifier"
#endif


#include "dependencies/open_vcdiff/google/vcdecoder.h"
#include "dependencies/open_vcdiff/google/vcencoder.h"
#include "dependencies/open_vcdiff/google/output_string.h"

#if defined MSVC_COMPILER
#pragma warning(pop)
#endif
#if defined CLANG_COMPILER
#pragma clang diagnostic pop
#endif

namespace Sen::Kernel::Dependencies::open_vcdiff {
	
}