#pragma once

#include "dependencies/SHA224CryptoServiceProvider/SHA224CryptoServiceProvider.h"

#if defined MSVC_COMPILER
#pragma warning(pop)
#endif
#if defined CLANG_COMPILER
#pragma clang diagnostic pop
#endif

namespace Sen::Kernel::Dependencies::SHA224CryptoServiceProvider {

	// using sha224 instead

	using sha224 = ::SHA224CryptoServiceProvider;

}