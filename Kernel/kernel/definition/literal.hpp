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