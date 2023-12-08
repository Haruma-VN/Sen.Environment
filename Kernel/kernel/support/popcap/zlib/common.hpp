#pragma once

#include "kernel/definition/compression/zlib.hpp"

namespace Sen::Kernel::Support::PopCap::Zlib {

	// common class

	struct Common {

		// public
		
		public:

			/**
			 * PopCap Zlib magic
			*/

			inline static auto constexpr magic = 0xDEADFED4;

			// Zlib compression level

			inline static auto const level = Kernel::Definition::Compression::Zlib::Level::LEVEL_9;

	};

}
