#pragma once

#if defined MSVC_COMPILER
#pragma warning(push)
#endif
#if defined CLANG_COMPILER
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wreserved-macro-identifier"
#endif

#include "dependencies/backward-cpp/backward.hpp"

#if defined MSVC_COMPILER
#pragma warning(pop)
#endif
#if defined CLANG_COMPILER
#pragma clang diagnostic pop
#endif

namespace Sen::Kernel::Dependencies::backwardcpp {

	// using third

}