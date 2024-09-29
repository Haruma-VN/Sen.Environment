#pragma once

#include "kernel/definition/utility.hpp"

namespace Sen::Kernel::Support::PopCap::Particles
{

	struct Common
	{
		inline static auto constexpr k_32bit_magic = 1092589901;

		inline static auto constexpr k_64bit_magic = -527264279;

		inline static auto constexpr k_32bit_block_byte_1 = 0x164;

		inline static auto constexpr k_32bit_block_byte_2 = 0x14;

		inline static auto constexpr k_64bit_block_byte_1 = 0x2B0;

		inline static auto constexpr k_64bit_block_byte_2 = 0x28;

		inline static auto constexpr zlib_magic = -559022380;

		inline static auto constexpr zlib_magic_big = -721506850;

		inline static auto constexpr use_64_bit_variant = false;

		inline static auto constexpr trail_enum = std::array<std::string_view, 14>{"Constant"_sv, "Linear"_sv, "EaseIn"_sv, "EaseOut"_sv, "EaseInOut"_sv, "EaseInOutWeak"_sv, "FastInOut"_sv, "FastInOutWeak"_sv, "WeakFastInOut"_sv, "Bounce"_sv, "BounceFastMiddle"_sv, "BounceSlowMiddle"_sv, "SinWave"_sv, "EaseSinWave"_sv};

		inline static auto constexpr emitter_type = std::array<std::string_view, 5>{"Circle"_sv, "Box"_sv, "BoxPath"_sv, "CirclePath"_sv, "CircleEvenSpacing"_sv};

		inline static auto constexpr field_type = std::array<std::string_view, 12>{"Invalid"_sv, "Friction"_sv, "Acceleration"_sv, "Attractor"_sv, "MaxVelocity"_sv, "Velocity"_sv, "Position"_sv, "SystemPosition"_sv, "GroundConstraint"_sv, "Shake"_sv, "Circle"_sv, "Away"_sv};

		// trim from start (in place)
        inline static auto ltrim(std::string &s) -> void
        {
            s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](unsigned char ch)
                                            { return !std::isspace(ch); }));
            return;
        }

        // trim from end (in place)
        inline static auto rtrim(std::string &s) -> void
        {
            s.erase(std::find_if(s.rbegin(), s.rend(), [](unsigned char ch)
                                 { return !std::isspace(ch); })
                        .base(),
                    s.end());
            return;
        }

        inline static auto trim(
            std::string &s) -> void
        {
            ltrim(s);
            rtrim(s);
            return;
        }
	};
}