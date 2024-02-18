#pragma once

#include "kernel/definition/utility.hpp"

namespace Sen::Kernel::Support::Texture::Compression::PVRTC
{

	template <typename T> requires std::is_integral<T>::value && std::is_unsigned<T>::value
	struct Rgba32 {

		public:

			T red;
			T green;
			T blue;
			T alpha;

			explicit Rgba32(
				T red = 0xFF,
				T green = 0xFF,
				T blue = 0xFF,
				T alpha = 0xFF
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