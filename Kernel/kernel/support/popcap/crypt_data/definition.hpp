#pragma once

#include "kernel/definition/utility.hpp"

namespace Sen::Kernel::Support::PopCap::CryptData
{

	#pragma region using

	using Sen::Kernel::Definition::DataStreamView;

	#pragma endregion

	#pragma region definition
	
	struct Definition {

		protected:

			// magic

			inline static constexpr auto magic = std::array<uint8_t, 11> { 0x43, 0x52, 0x59, 0x50, 0x54, 0x5F, 0x52, 0x45, 0x53, 0x0A, 0x00 };

	};

	#pragma endregion
}
