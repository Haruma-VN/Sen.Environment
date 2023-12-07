#pragma once

#include "kernel/definition/library.hpp"
#include "kernel/definition/macro.hpp"

namespace Sen::Kernel {

	template <typename T> 
	struct Temporary {
		public:
		
			T value;

			Temporary(
				T value
			) 
			{
				thiz.value = value;
			}

			~Temporary(

			) = default;
	};

	/**
	 * Pass value and generic type for casting pointer
	*/

	template <typename T> 
	inline auto static create_pointer_for_reference(
		const T &value
	) -> Temporary<T> const
	{
		return Temporary<T>(value);
	}


}