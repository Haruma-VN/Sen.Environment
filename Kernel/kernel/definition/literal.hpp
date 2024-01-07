#pragma once

namespace Sen::Kernel {


	inline constexpr auto operator "" _c(
		char c
	) -> char
	{
		return c;
	}

	inline constexpr auto operator "" _byte(
		unsigned long long int c
	) -> unsigned char
	{
		return static_cast<unsigned char>(c);
	}


	inline constexpr auto operator "" _ui(
		unsigned long long int n
	) -> unsigned int
	{
    	return static_cast<unsigned int>(n);
	}

	inline constexpr auto operator "" _ul(
		unsigned long long int n
	) -> unsigned long
	{
    	return static_cast<unsigned long>(n);
	}

	// inline constexpr auto operator "" _s(
	// 	const std::string & n
	// ) -> std::string
	// {
    // 	return static_cast<std::string>(n);
	// }

}