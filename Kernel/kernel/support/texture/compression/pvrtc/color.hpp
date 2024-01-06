#pragma once

#include "kernel/definition/utility.hpp"

namespace Sen::Kernel::Support::Texture::Compression::PVRTC
{
	
	struct ColorRGBA {

		public:

			int red;
			int green;
			int blue;
			int alpha;

			explicit ColorRGBA(
				int red = 0xFF_byte,
				int green = 0xFF_byte,
				int blue = 0xFF_byte,
				int alpha = 0xFF_byte
			) : red(red), green(green), blue(blue), alpha(alpha)
			{

			}

			~ColorRGBA(

			) = default;

			inline auto operator *(
				int x
			) -> ColorRGBA
			{
				return ColorRGBA{(thiz.red * x), thiz.green * x, thiz.blue * x, thiz.alpha * x};
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
	
	inline static auto to_json(
		nlohmann::ordered_json & json, 
		const ColorRGBA & that
	) -> void
	{
		json = nlohmann::ordered_json{
			{"red", that.red},
			{"green", that.green},
			{"blue", that.blue},
			{"alpha", that.alpha}
		};
		return;
	}

	inline static auto from_json(
		const nlohmann::ordered_json & json, 
		ColorRGBA & that
	) -> void
	{
		json.at("red").get_to(that.red);
		json.at("green").get_to(that.green);
		json.at("blue").get_to(that.blue);
		json.at("alpha").get_to(that.alpha);
		return;
	}

	struct ColorRGB {

		public:
		
			int red;
			int green;
			int blue;

			explicit ColorRGB(
				int red = 0xFF_byte,
				int green = 0xFF_byte,
				int blue = 0xFF_byte
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

	inline static auto to_json(
		nlohmann::ordered_json & json, 
		const ColorRGB & that
	) -> void
	{
		json = nlohmann::ordered_json{
			{"red", that.red},
			{"green", that.green},
			{"blue", that.blue}
		};
		return;
	}

	inline static auto from_json(
		const nlohmann::ordered_json & json, 
		ColorRGB & that
	) -> void
	{
		json.at("red").get_to(that.red);
		json.at("green").get_to(that.green);
		json.at("blue").get_to(that.blue);
		return;
	}

}
