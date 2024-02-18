#pragma once

#include "kernel/support/texture/compression/pvrtc/color.hpp"

namespace Sen::Kernel::Support::Texture::Compression::PVRTC
{

	struct Packet {

		protected:

			/**
			 * Represent the 64-bit Integer
			*/

			using ulong = uint64_t;

			/**
			 * Represent the boolean type
			*/

			using boolean = bool;

			/**
			 * Represent the integer type
			*/

			using integer = int;
			
			/**
			 * Represent color RGBA
			*/

			using ColorRGBA = ColorRGBA;

			/**
			 * Represent color RGB
			*/

			using ColorRGB = ColorRGB;

		public:

			ulong PvrTcWord;

			inline static constexpr auto BILINEAR_FACTORS = std::array<std::array<uint8_t, 4>, 16>{{
				{{4, 4, 4, 4}},
				{{2, 6, 2, 6}},
				{{8, 0, 8, 0}},
				{{6, 2, 6, 2}},
				{{2, 2, 6, 6}},
				{{1, 3, 3, 9}},
				{{4, 0, 12, 0}},
				{{3, 1, 9, 3}},
				{{8, 8, 0, 0}},
				{{4, 12, 0, 0}},
				{{16, 0, 0, 0}},
				{{12, 4, 0, 0}},
				{{6, 6, 2, 2}},
				{{3, 9, 1, 3}},
				{{12, 0, 4, 0}},
				{{9, 3, 3, 1}},
			}};

			inline static constexpr auto WEIGHTS = std::array<uint8_t, 32>{8, 0, 8, 0, 5, 3, 5, 3, 3, 5, 3, 5, 0, 8, 0, 8, 8, 0, 8, 0, 4, 4, 4, 4, 4, 4, 0, 0, 0, 8, 0, 8};

			explicit Packet(

			) : PvrTcWord(0)
			{

			}

			explicit Packet(
				ulong PvrTcWord
			) : PvrTcWord(PvrTcWord)
			{

			}

			~Packet(

			) = default;

			inline auto get_modulation_data(

			) -> uint32_t
			{
				return static_cast<uint32_t>(thiz.PvrTcWord & 0xffffffff);
			}

			inline auto set_modulation_data(
				uint32_t PvrTcWord
			) -> void
			{
				thiz.PvrTcWord |= PvrTcWord;
				return;
			}

			inline auto get_use_punch_through_alpha(

			) -> boolean
			{
				return ((thiz.PvrTcWord >> 32) & 0b1) == 1;
			}

			inline auto set_use_punch_through_alpha(
				boolean value
			) -> void
			{
				thiz.PvrTcWord |= (value ? uint64_t{1} : uint64_t{0}) << 32;
				return;
			}

			inline auto get_alpha(

			) -> integer
			{
				return static_cast<integer>((thiz.PvrTcWord >> 33) & 0b11111111111111);
			}

			inline auto set_alpha(
				integer value
			) -> void
			{
				thiz.PvrTcWord |= static_cast<ulong>(value & 0b11111111111111) << 33;
				return;
			}

			inline auto get_color_alpha_is_opaque(

			) -> boolean
			{
        		return ((thiz.PvrTcWord >> 47) & 0b1) == 1;
			}

			inline auto set_color_alpha_is_opaque(
				boolean value
			) -> void
			{
        		thiz.PvrTcWord |= (value ? uint64_t{1} : uint64_t{0}) << 47;
				return;
			}

			inline auto get_blue(

			) -> integer
			{
				return static_cast<integer>((thiz.PvrTcWord >> 48) & 0b111111111111111);
			}

			inline auto set_blue(
				integer value
			) -> void
			{
        		thiz.PvrTcWord |= static_cast<ulong>(value & 0b111111111111111) << 48;
				return;
			}

			inline auto get_color_blue_is_opaque(

			) -> boolean
			{
        		return (thiz.PvrTcWord >> 63) == 1;
			}

			inline auto set_color_blue_is_opaque(
				boolean value
			) -> void
			{
        		thiz.PvrTcWord |= (value ? uint64_t{1} : uint64_t{0}) << 63;
				return;
			}

			inline auto get_color_alpha_color_rgba(

			) -> ColorRGBA
			{
				auto color_alpha = thiz.get_alpha();
				if(thiz.get_color_alpha_is_opaque()){
					auto r = color_alpha >> 9;
					auto g = (color_alpha >> 4) & 0x1f;
					auto b = color_alpha & 0xf;
					return ColorRGBA(static_cast<uint8_t>((r << 3) | (r >> 2)), static_cast<uint8_t>((g << 3) | (g >> 2)), static_cast<uint8_t>((b << 4) | b));
				}
				auto a = (color_alpha >> 11) & 0x7;
				auto r = (color_alpha >> 7) & 0xf;
				auto g = (color_alpha >> 3) & 0xf;
				auto b = color_alpha & 0x7;
				return ColorRGBA(static_cast<uint8_t>((r << 4) | r), static_cast<uint8_t>((g << 4) | g), static_cast<uint8_t>((b << 5) | (b << 2) | (b >> 1)), static_cast<uint8_t>((a << 5) | (a << 2) | (a >> 1)));
			}

			inline auto get_color_blue_color_rgba(

			) -> ColorRGBA
			{
				auto color_blue = thiz.get_blue();
				if(thiz.get_color_blue_is_opaque()){
					auto r = color_blue >> 10;
					auto g = (color_blue >> 5) & 0x1f;
					auto b = color_blue & 0x1f;
					return ColorRGBA(static_cast<uint8_t>((r << 3) | (r >> 2)), static_cast<uint8_t>((g << 3) | (g >> 2)), static_cast<uint8_t>((b << 3) | (b >> 2)));
				}
				auto a = (color_blue >> 12) & 0x7;
				auto r = (color_blue >> 8) & 0xf;
				auto g = (color_blue >> 4) & 0xf;
				auto b = color_blue & 0xf;
				return ColorRGBA(static_cast<uint8_t>((r << 4) | r), static_cast<uint8_t>((g << 4) | g), static_cast<uint8_t>((b << 4) | b), static_cast<uint8_t>((a << 5) | (a << 2) | (a >> 1)));
			}

			inline auto get_color_alpha_color_rgb(

			) -> ColorRGB
			{
				auto color_alpha = thiz.get_alpha();
				if(thiz.get_color_alpha_is_opaque()){
					auto r = color_alpha >> 9;
					auto g = (color_alpha >> 4) & 0x1f;
					auto b = color_alpha & 0xf;
					return ColorRGB(static_cast<uint8_t>((r << 3) | (r >> 2)), static_cast<uint8_t>((g << 3) | (g >> 2)), static_cast<uint8_t>((b << 4) | b));
				}
				auto r = (color_alpha >> 7) & 0xf;
				auto g = (color_alpha >> 3) & 0xf;
				auto b = color_alpha & 0x7;
				return ColorRGB(static_cast<uint8_t>((r << 4) | r), static_cast<uint8_t>((g << 4) | g), static_cast<uint8_t>((b << 5) | (b << 2) | (b >> 1)));
			}

			inline auto get_color_blue_color_rgb(

			) -> ColorRGB
			{
				auto color_blue = thiz.get_blue();
				if(thiz.get_color_blue_is_opaque()){
					auto r = color_blue >> 10;
					auto g = (color_blue >> 5) & 0x1f;
					auto b = color_blue & 0x1f;
					return ColorRGB(static_cast<uint8_t>((r << 3) | (r >> 2), (g << 3)) | static_cast<uint8_t>((g >> 2)), static_cast<uint8_t>((b << 3) | (b >> 2)));
				}
				auto r = (color_blue >> 8) & 0xf;
				auto g = (color_blue >> 4) & 0xf;
				auto b = color_blue & 0xf;
            	return ColorRGB(static_cast<uint8_t>((r << 4) | r), static_cast<uint8_t>((g << 4) | g), static_cast<uint8_t>((b << 4) | b));
			}

			inline auto set_color_alpha_color_rgba(
				const Rgba32 & color
			) -> void 
			{
				auto a = color.alpha >> 5;
				if (a == 0x7) {
					auto r = color.red >> 3;
					auto g = color.green >> 3;
					auto b = color.blue >> 4;
					thiz.set_alpha(((r << 9) | (g << 4) | b));
					thiz.set_color_alpha_is_opaque(true);
					return;
				} 
				auto r = color.red >> 4;
				auto g = color.green >> 4;
				auto b = color.blue >> 5;
				thiz.set_alpha(((a << 11) | (r << 7) | (g << 3) | b));
				thiz.set_color_alpha_is_opaque(false);
				return;
    		}

			inline auto set_color_blue_color_rgba(
				const Rgba32 & color
			) -> void
			{
				auto a = color.alpha >> 5;
				if (a == 0x7) {
					auto r = color.red >> 3;
					auto g = color.green >> 3;
					auto b = color.blue >> 3;
					thiz.set_blue(((r << 10) | (g << 5) | b));
					thiz.set_color_blue_is_opaque(true);
					return;
				}
				auto r = color.red >> 4;
				auto g = color.green >> 4;
				auto b = color.blue >> 4;
				thiz.set_blue(((a << 12) | (r << 8) | (g << 4) | b));
				thiz.set_color_blue_is_opaque(false);
				return;
			}

			inline auto set_color_alpha_color_rgb(
				const Rgba32 & color 
			) -> void 
			{
				auto r = color.red >> 3;
				auto g = color.green >> 3;
				auto b = color.blue >> 4;
				thiz.set_alpha(((r << 9) | (g << 4) | b));
				thiz.set_color_alpha_is_opaque(true);
				return;
			}

			inline auto set_color_blue_color_rgb(
				const Rgba32 & color
			) -> void 
			{
				auto r = color.red >> 3;
				auto g = color.green >> 3;
				auto b = color.blue >> 3;
				thiz.set_blue(((r << 10) | (g << 5) | b));
				thiz.set_color_blue_is_opaque(true);
				return;
			}

			
	};
}