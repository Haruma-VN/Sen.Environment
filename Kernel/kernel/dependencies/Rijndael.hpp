#pragma once

#include "dependencies/Rijndael/Rijndael.h"

#if defined MSVC_COMPILER
#pragma warning(pop)
#endif
#if defined CLANG_COMPILER
#pragma clang diagnostic pop
#endif

namespace Sen::Kernel::Dependencies::Rijndael 
{

	// class Rijndael 
	using CRijndael = ::CRijndael;

	
}
