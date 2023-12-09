#pragma once

#include "kernel/definition/utility.hpp"

namespace Sen::Kernel::Support::Texture {

	// set pixel

	#define set_pixel(x, y, width) (y * width + x) * 4;

	// pixel area

	#define pixel_area(area) area * 4;

	// pixel area rgba

	#define pixel_area_rgba(width, height) pixel_area(calculate_area(width, height));

	// pixel color 

	class PixelColor {

		public:

			// non alpha

			inline static constexpr auto NON_ALPHA = static_cast<unsigned char>(0xFF);

			// alpha

			inline static constexpr auto ALPHA = static_cast<unsigned char>(0x00);

	};

	// use color

	using namespace Sen::Kernel::Definition;

	/**
	 * Decode class
	*/

	class Decode : public PixelColor {

		private:

			/**
			 * Convert alpha red green blue to red green blue alpha (standard)
			*/

			static auto argb_to_rgba(
				const std::vector<unsigned char> &color
			) -> std::vector<unsigned char>
			{
				auto data = std::vector<unsigned char>();
				data.reserve(color.size());
				for (auto i : Range<size_t>(color.size(), 4)) {
					// red
					data.push_back(color[i + 1]);
					// green
					data.push_back(color[i + 2]);
					// blue
					data.push_back(color[i + 3]);
					// alpha
					data.push_back(color[i]);
				}
				return data;
			}

		public:

			/**
			 * color: stream color
			 * width: image width
			 * height: image height
			 * return: Image struct
			*/
			
			static auto rgba_8888(
				const std::vector<unsigned char> &color,
				int width,
				int height
			) -> Image<int>
			{
				return Image<int>(width, height, color);
			}

			/**
			 * color: stream color
			 * width: image width
			 * height: image height
			 * return: Image struct
			*/

			static auto argb_8888(
				const std::vector<unsigned char> & color,
				int width,
				int height
			) -> Image<int>
			{
				return rgba_8888(argb_to_rgba(color), width, height);
			}

			static auto rgba_4444(
				const std::vector<unsigned char> & color,
				int width,
				int height
			) -> Image<int> 
			{
				auto sen = Buffer::Vector{color};
				auto area = pixel_area_rgba(width, height);
				auto data = std::vector<unsigned char>(area, 0x00);
				for(auto y : Range<int>(height)){
					for(auto x : Range<int>(width)){
						auto temp_pixel = sen.readUint16LE();
						auto red = temp_pixel >> 12;
						auto green = (temp_pixel & 0xF00) >> 8;
						auto blue = (temp_pixel & 0xF0) >> 4;
						auto alpha = temp_pixel & 0xF;
						auto index = set_pixel(x, y, width);
						data[index] = static_cast<unsigned char>((red << 4) | red);
						data[index + 1] = static_cast<unsigned char>((green << 4) | green);
						data[index + 2] = static_cast<unsigned char>((blue << 4) | blue);
						data[index + 3] = static_cast<unsigned char>((alpha << 4) | alpha);
					}
				}
				return Image<int>{width, height, data};
			}

			static auto rgb_565(
				const std::vector<unsigned char> & color,
				int width,
				int height
			) -> Image<int>
			{
				auto sen = Buffer::Vector{color};
				auto area = pixel_area_rgba(width, height);
				auto data = std::vector<unsigned char>(area, 0x00);
				for(auto y : Range<int>(height)){
					for(auto x : Range<int>(width)){
						auto temp_pixel = sen.readUint16LE();
						auto red = temp_pixel >> 12;
						auto green = (temp_pixel & 0x7E0) >> 5;
						auto blue = temp_pixel & 0x1F;
						auto index = set_pixel(x, y, width);
						data[index] = static_cast<unsigned char>((red << 3) | (red >> 2));
						data[index + 1] = static_cast<unsigned char>((green << 2) | (green >> 4));
						data[index + 2] = static_cast<unsigned char>((blue << 3) | (blue >> 2));
						data[index + 3] = PixelColor::NON_ALPHA;
					}
				}
				return Image<int>{width, height, data};
			}

			static auto rgba_5551(
				const std::vector<unsigned char> & color,
				int width,
				int height
			) -> Image<int>
			{
				auto sen = Buffer::Vector{color};
				auto area = pixel_area_rgba(width, height);
				auto data = std::vector<unsigned char>(area, 0x00);
				for(auto y : Range<int>(height)){
					for(auto x : Range<int>(width)){
						auto temp_pixel = sen.readUint16LE();
						auto red = (temp_pixel & 0xF800) >> 11;
						auto green = (temp_pixel & 0x7E0) >> 5;
						auto blue = (temp_pixel & 0x3E) >> 1;
						auto index = set_pixel(x, y, width);
						data[index] = static_cast<unsigned char>((red << 3) | (red >> 2));
						data[index + 1] = static_cast<unsigned char>((green << 3) | (green >> 2));
						data[index + 2] = static_cast<unsigned char>((blue << 3) | (blue >> 2));
						data[index + 3] = static_cast<unsigned char>(-(temp_pixel & 0x1));
					}
				}
				return Image<int>{width, height, data};
			}

			static auto rgba_4444_tiled(
				const std::vector<unsigned char> & color,
				int width,
				int height
			) -> Image<int>
			{
				auto sen = Buffer::Vector{color};
				auto area = pixel_area_rgba(width, height);
				auto data = std::vector<unsigned char>(area, 0x00);
				for(auto i : Range<int>(0, height, 32)){
					for(auto w : Range<int>(0, width, 32)){
						for(auto j : Range<int>(32)){
							for(auto k : Range<int>(32)){
            					auto temp_pixel = sen.readUint16LE();
								if ((i + j) < height and (w + k) < width) {
									auto red = temp_pixel >> 12;
									auto green = (temp_pixel & 0xF00) >> 8;
									auto blue = (temp_pixel & 0xF0) >> 4;
									auto alpha = temp_pixel & 0xF;
									auto index = set_pixel(w + k, i + j, width);
									data[index] = static_cast<unsigned char>((red << 4) | red);
									data[index + 1] = static_cast<unsigned char>((green << 4) | green);
									data[index + 2] = static_cast<unsigned char>((blue << 4) | blue);
									data[index + 3] = static_cast<unsigned char>((alpha << 4) | alpha);
								}
							}
						}
					}
				}
				return Image<int>{width, height, data};
			}

			static auto rgb_565_tiled(
				const std::vector<unsigned char> & color,
				int width,
				int height
			) -> Image<int>
			{
				auto sen = Buffer::Vector{color};
				auto area = pixel_area_rgba(width, height);
				auto data = std::vector<unsigned char>(area, 0x00);
				for(auto i : Range<int>(0, height, 32)){
					for(auto w : Range<int>(0, width, 32)){
						for(auto j : Range<int>(32)){
							for(auto k : Range<int>(32)){
            					auto temp_pixel = sen.readUint16LE();
								if ((i + j) < height and (w + k) < width) {
									auto red = (temp_pixel & 0xF800) >> 8;
									auto green = (temp_pixel & 0x7E0) >> 3;
									auto blue = (temp_pixel & 0x1F) << 3;
									auto index = set_pixel(w + k, i + j, width);
									data[index] = static_cast<unsigned char>((red << 4) | red);
									data[index + 1] = static_cast<unsigned char>((green << 4) | green);
									data[index + 2] = static_cast<unsigned char>((blue << 4) | blue);
									data[index + 3] = PixelColor::NON_ALPHA;
								}
							}
						}
					}
				}
				return Image<int>{width, height, data};
			}

	};
}