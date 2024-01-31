#pragma once

#include "kernel/support/wwise/sound_bank/definition.hpp"

namespace Sen::Kernel::Support::WWise::SoundBank {

	#pragma region Category

	inline static auto constexpr IntegerCategory = true;

	#pragma endregion
	
	#pragma region encode

	struct Encode {

		protected:

			template <typename T> requires std::is_integral<T>::value and IntegerCategory
			constexpr auto beautify_length(
				T size
			) -> T
			{
				if (size % 16 == 0) {
					return 0;
				}
				else
				{
					return 16 - (size % 16);
				}
			}

	};

	#pragma endregion

}