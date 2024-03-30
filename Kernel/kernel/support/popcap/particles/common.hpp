#pragma once

#include "kernel/definition/utility.hpp"

namespace Sen::Kernel::Support::PopCap::Particles {

	struct Common {

		inline static auto constexpr magic = 1092589901;

		inline static auto constexpr default_byte = 0x164;

		inline static auto constexpr zlib_magic = -559022380;

		inline static auto constexpr use_64_bit_variant = false;

	};
}