#pragma once

#include "kernel/definition/macro.hpp"
#include "kernel/definition/library.hpp"

namespace Sen::Kernel::Math {

	/**
	 * arc cos
	*/

	using ::acos;

	/**
	 * arc sin
	*/

    using ::asin;

	/**
	 * arc tan
	*/

    using ::atan;

	/**
	 * arc tan 2
	*/

    using ::atan2;

	/**
	 * ceil
	*/

    using ::ceil;

	/**
	 * cos
	*/

    using ::cos;

	/**
	 * coshin
	*/

    using ::cosh;

	/**
	 * e^x
	*/

    using ::exp;

	/**
	 * float abs
	*/

    using ::fabs;

	/**
	 * floor
	*/

    using ::floor;

	/**
	 * float mod
	*/

    using ::fmod;

	/**
	 * e^x
	*/

    using ::frexp;

	/**
	 * ?
	*/

    using ::ldexp;

	/**
	 * logarit
	*/

    using ::log;

	/**
	 * log
	*/

    using ::log10;

	/**
	 * mod f
	*/

    using ::modf;

	/**
	 * pow
	*/

    using ::pow;

	/**
	 * sin
	*/

    using ::sin;

	/**
	 * sinh
	*/

    using ::sinh;

	/**
	 * sqrt
	*/

    using ::sqrt;

	/**
	 * tan
	*/

    using ::tan;

	/**
	 * tanh
	*/

    using ::tanh;

	/**
	 * compare between two number
	*/


	template <typename T> requires std::is_integral_v<T>
	inline static auto compare(
		const T & a,
		const T & b
	) -> T
	{
		if(a >= b){
			return a;
		}
		return b;
	}
}