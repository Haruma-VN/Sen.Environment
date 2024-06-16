#pragma once

#include "kernel/definition/utility.hpp"

namespace Sen::Kernel::Support::PopCap::Animation
{
    using namespace Definition;
    struct Common
    {
        inline static constexpr auto k_magic_identifier = 0xBAF01954_ui;

        inline static constexpr auto version_list = std::array<int, 6>{1, 2, 3, 4, 5, 6};

        inline static constexpr auto vertical_bar = "|"_sv;

        inline static auto k_version = 0_ui;

        inline static auto k_frame_rate = static_cast<double>(0);

        struct LayerRemoveFlag {
            inline static constexpr auto k_count = 5_byte;

            // ----------------

            inline static constexpr auto unused_1 = 0_byte;

            inline static constexpr auto unused_2 = 1_byte;

            inline static constexpr auto unused_3 = 2_byte;

            inline static constexpr auto unused_4 = 3_byte;

            inline static constexpr auto unused_5 = 4_byte;
        };

        struct LayerAppendFlag {
            inline static constexpr auto k_count = 5_byte;

            // ----------------

            inline static constexpr auto time_scale = 0_byte;

            inline static constexpr auto name = 1_byte;

            inline static constexpr auto preload_frame = 2_byte;

            inline static constexpr auto additive = 3_byte;

            inline static constexpr auto sprite = 4_byte;
        };

        struct LayerChangeFlag {
            inline static constexpr auto k_count = 6_byte;

            // ----------------

            inline static constexpr auto sprite_frame_number = 0_byte;

            inline static constexpr auto long_coord = 1_byte;

            inline static constexpr auto matrix = 2_byte;

            inline static constexpr auto color = 3_byte;

            inline static constexpr auto rotate = 4_byte;

            inline static constexpr auto source_rectangle = 5_byte;
        };

        struct FrameFlag {
            inline static constexpr auto k_count = 8_byte;

            // ----------------

            inline static constexpr auto remove = 0_byte;

            inline static constexpr auto append = 1_byte;

            inline static constexpr auto change = 2_byte;

            inline static constexpr auto label = 3_byte;

            inline static constexpr auto stop = 4_byte;

            inline static constexpr auto command = 5_byte;

            inline static constexpr auto unused_7 = 6_byte;

            inline static constexpr auto unused_8 = 7_byte;
        };

        
        struct ValueRate {

			inline static constexpr auto time = 65536.0f;

			inline static constexpr auto size = 20.0f;

			inline static constexpr auto angle = 1000.0f;

			inline static constexpr auto matrix = 65536.0f;

			inline static constexpr auto matrix_exact = 20.0f * 65536.0f;

			inline static constexpr auto color = 255.0f;

		};
    };
}