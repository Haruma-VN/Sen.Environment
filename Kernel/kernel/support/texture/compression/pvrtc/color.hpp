#pragma once

#include "kernel/definition/utility.hpp"

namespace Sen::Kernel::Support::Texture::Compression::PVRTC
{

	struct Rgba32 {

		public:

			uint8_t red;
			uint8_t green;
			uint8_t blue;
			uint8_t alpha;

			explicit Rgba32(
				uint8_t red = 0xFF,
				uint8_t green = 0xFF,
				uint8_t blue = 0xFF,
				uint8_t alpha = 0xFF
			) : red(red), green(green), blue(blue), alpha(alpha)
			{

			}

			~Rgba32(

			) = default;

	};
	
	struct ColorRGBA {

		public:

			int red;
			int green;
			int blue;
			int alpha;

			explicit ColorRGBA(
				int red = 0xFF,
				int green = 0xFF,
				int blue = 0xFF,
				int alpha = 0xFF
			) : red(red), green(green), blue(blue), alpha(alpha)
			{

			}

			~ColorRGBA(

			) = default;

			inline auto operator *(
				int x
			) -> ColorRGBA
			{
				return ColorRGBA{thiz.red * x, thiz.green * x, thiz.blue * x, thiz.alpha * x};
			}

			inline auto operator +(
				const ColorRGBA & that
			) -> ColorRGBA
			{
				return ColorRGBA{thiz.red + that.red, thiz.green + that.green, thiz.blue + that.blue, thiz.alpha + that.alpha};
			}

			inline auto operator -(
				const ColorRGBA & that
			) -> ColorRGBA
			{
				return ColorRGBA{thiz.red - that.red, thiz.green - that.green, thiz.blue - that.blue, thiz.alpha - that.alpha};
			} 

			inline auto operator %(
				const ColorRGBA & that
			) -> int
			{
				return thiz.red * that.red + thiz.green * that.green + thiz.blue * that.blue + thiz.alpha * that.alpha;
			}
	};

	struct ColorRGB {

		public:
		
			int red;
			int green;
			int blue;

			explicit ColorRGB(
				int red = 0xFF,
				int green = 0xFF,
				int blue = 0xFF
			) : red(red), green(green), blue(blue)
			{

			}

			~ColorRGB(

			) = default;

			inline auto operator *(
				int x
			) -> ColorRGB
			{
				return ColorRGB{thiz.red * x, thiz.green * x, thiz.blue * x};
			}

			inline auto operator +(
				const ColorRGB & that
			) -> ColorRGB
			{
				return ColorRGB{thiz.red + that.red, thiz.green + that.green, thiz.blue + that.blue};
			}

			inline auto operator -(
				const ColorRGB & that
			) -> ColorRGB
			{
				return ColorRGB{thiz.red - that.red, thiz.green - that.green, thiz.blue - that.blue};
			} 

			inline auto operator %(
				const ColorRGB & that
			) -> int
			{
				return thiz.red * that.red + thiz.green * that.green + thiz.blue * that.blue;
			}
	};

}