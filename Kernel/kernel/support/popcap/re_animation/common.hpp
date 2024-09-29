#pragma once

#include "kernel/definition/utility.hpp"

namespace Sen::Kernel::Support::PopCap::ReAnimation
{

    struct Common
    {
        inline static auto constexpr zlib_magic = -559022380;

        inline static auto constexpr zlib_magic_big = -721506850;

        inline static constexpr auto transform_tfloat_end = -10000.0f;

        // inline static constexpr auto transform_wp_tfloat_end = -99999.0f;

        inline static constexpr auto reanim_compress_head = -559022380;

        inline static constexpr auto reanim_pc_head = -1282165568;

        inline static constexpr auto reanim_xmb_head = 1833061976;

        inline static constexpr auto reanim_xmb_version = 5;

        inline static constexpr auto reanim_xmb_info = std::array<uint8_t, 0x26>{0x01, 0x1E, 0x53, 0x65, 0x78, 0x79, 0x2E, 0x54, 0x6F, 0x64, 0x4C, 0x69, 0x62, 0x2E, 0x52, 0x65, 0x61, 0x6E, 0x69, 0x6D, 0x52, 0x65, 0x61, 0x64, 0x65, 0x72, 0x2C, 0x20, 0x4C, 0x41, 0x57, 0x4E, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01};
    };

}