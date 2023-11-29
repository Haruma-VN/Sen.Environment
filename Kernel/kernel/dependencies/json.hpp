#pragma once

#include "dependencies/json/json.hpp"

#if defined MSVC_COMPILER
#pragma warning(pop)
#endif
#if defined CLANG_COMPILER
#pragma clang diagnostic pop
#endif

namespace Sen::Kernel::Dependencies::json 
{

	// the namespace of json library is nlohmann
	
	using namespace ::nlohmann;

}
