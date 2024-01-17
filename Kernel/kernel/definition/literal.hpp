#pragma once

namespace Sen::Kernel {
	/**
	 * Char literal
	*/

	inline constexpr auto operator "" _c(
		char c
	) -> char
	{
		return c;
	}

	/**
	 * Byte literal
	*/

	inline constexpr auto operator "" _byte(
		unsigned long long int c
	) -> unsigned char
	{
		return static_cast<unsigned char>(c);
	}

	/**
	 * Unsigned int
	*/

	inline constexpr auto operator "" _ui(
		unsigned long long int n
	) -> unsigned int
	{
    	return static_cast<unsigned int>(n);
	}

	/**
	 * Unsigned long
	*/

	inline constexpr auto operator "" _ul(
		unsigned long long int n
	) -> unsigned long
	{
    	return static_cast<unsigned long>(n);
	}

	/**
	 * String
	*/

	inline constexpr auto operator "" _s(
		const char* n
	) -> std::string
	{
    	return std::string{n, strlen(n)};
	}

	/**
	 * String view literal
	*/

	inline constexpr auto operator "" _sv(
		const char* n
	) -> std::string_view
	{
    	return std::string_view{n};
	}
}