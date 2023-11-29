#pragma once

#if defined MSVC_COMPILER
#pragma warning(push)
#endif
#if defined CLANG_COMPILER
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wreserved-macro-identifier"
#endif

#include "dependencies/PVRTCCompressor/PvrTcDecoder.h"
#include "dependencies/PVRTCCompressor/PvrTcEncoder.h"
#include "dependencies/PVRTCCompressor/RgbBitmap.h"
#include "dependencies/PVRTCCompressor/RgbaBitmap.h"

#if defined MSVC_COMPILER
#pragma warning(pop)
#endif
#if defined CLANG_COMPILER
#pragma clang diagnostic pop
#endif

namespace Sen::Kernel::Dependencies::PVRTCCompressor {


	// using third

	
}