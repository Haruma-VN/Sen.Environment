#pragma once

#if defined MSVC_COMPILER
#pragma warning(push)
#pragma warning(disable:4191)
#pragma warning(disable:4365)
#pragma warning(disable:5039)
#endif
#if defined CLANG_COMPILER
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wold-style-cast"
#pragma clang diagnostic ignored "-Wreserved-macro-identifier"
#pragma clang diagnostic ignored "-Wdouble-promotion"
#pragma clang diagnostic ignored "-Wsign-conversion"
#pragma clang diagnostic ignored "-Wshorten-64-to-32"
#pragma clang diagnostic ignored "-Wreserved-identifier"
#pragma clang diagnostic ignored "-Wzero-as-null-pointer-constant"
#pragma clang diagnostic ignored "-Wcast-function-type"
#pragma clang diagnostic ignored "-Wnewline-eof"
#pragma clang diagnostic ignored "-Wimplicit-int-float-conversion"
#endif

#if defined MSVC_COMPILER
#include "dependencies/quickjs.c_smile/quickjs.h"
#else
#include "dependencies/quickjs/quickjs.h"
#endif

#if defined MSVC_COMPILER
#pragma warning(pop)
#endif
#if defined CLANG_COMPILER
#pragma clang diagnostic pop
#endif

namespace Sen::Kernel::Dependencies::quickjs {

}