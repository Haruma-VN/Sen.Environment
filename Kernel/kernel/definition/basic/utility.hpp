
#pragma once

#include "kernel/definition/macro.hpp"
#include "kernel/definition/library.hpp"

namespace Sen::Kernel::Definition
{
    inline constexpr auto is_padded_size(
        size_t const &size,
        size_t const &unit) -> bool
    {
        return size % unit == 0;
    }

    inline constexpr auto compute_padded_size(
        size_t const &size,
        size_t const &unit) -> size_t
    {
        return is_padded_size(size, unit) ? size : ((size / unit + size_t{1}) * unit);
    }

    inline constexpr auto compute_padding_size(
        size_t const &size,
        size_t const &unit) -> size_t
    {
        return compute_padded_size(size, unit) - size;
    }

    template <typename T>
        requires std::is_arithmetic_v<T>
    inline auto clip_bit(
        T const &it,
        size_t const &begin,
        size_t const &size) -> T
    {
        return (it >> begin) & ~(static_cast<T>((std::numeric_limits<T>::max)()) << size);
    }

    inline constexpr auto compute_utf8_character_length(
            std::string_view value) -> size_t
        {
            auto string_length = size_t{0};
            for (auto &c : value)
            {
                if (c <= 0b01111111)
                {
                    ++string_length;
                }
                else if (c >= 0b11000010 && c <= 0b11011111)
                {
                    ++string_length;
                }
                else if (c >= 0b11100001 && c <= 0b11101111)
                {
                    ++string_length;
                }
                else if (c >= 0b11110000 && c <= 0b11110111)
                {
                    ++string_length;
                }
            }
            return string_length;
        }
}