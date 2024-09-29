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
	 * Unsigned short
	*/

	inline constexpr auto operator "" _us(
		unsigned long long int n
	) -> unsigned short
	{
    	return static_cast<unsigned short>(n);
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
	 * Size
	*/

	inline constexpr auto operator "" _size(
		unsigned long long int n
	) -> size_t
	{
    	return static_cast<size_t>(n);
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

	inline static auto constexpr operator "" _sv(
		const char* str, std::size_t len
	) noexcept -> std::string_view
	{
		return { str, len };
	}

	inline static constexpr auto hash_sv(
		std::string_view str
	) -> unsigned int
	{
		auto hash = 5381_byte;
		for (auto c : str) {
			hash = (hash * 33) ^ c;
		}
		return hash;
	}
}