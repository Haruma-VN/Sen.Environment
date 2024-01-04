#pragma once

#include "kernel/definition/utility.hpp"
#include "kernel/support/texture/decode.hpp"

namespace Sen::Kernel::Support::Texture {

	// use color

	using namespace Sen::Kernel::Definition;

	/**
	 * Encode class
	*/

	class Encode : public PixelColor {

		private:

			/**
			 * l_8
			*/
			
			inline static auto convert_luminance_from_rgb(
				unsigned char red,
				unsigned char green,
				unsigned char blue
			) -> unsigned char 
			{
				return Encode::round(static_cast<double>(red * 0.299f + green * 0.587f + blue * 0.114f));
			}

			/**
			 * la_88
			*/

			inline static auto convert_luminance_from_rgb(
				unsigned char red,
				unsigned char green,
				unsigned char blue,
				unsigned char alpha
			) -> unsigned char 
			{
				return Encode::round(static_cast<double>((static_cast<int>(red * 0.299f + green * 0.587f + blue * 0.114f) << 8) | ((alpha))));
			}

			/**
			 * la_44
			*/

			inline static auto convert_luminance_from_rgba(
				unsigned char red,
				unsigned char green,
				unsigned char blue,
				unsigned char alpha
			) -> unsigned char 
			{
				return Encode::round(static_cast<double>((static_cast<int>(red * 0.299f + green * 0.587f + blue * 0.114f) & 0xF0) | ((alpha) >> 4)));
			}

			inline static auto round(
				double a
			) -> unsigned char
			{
				auto k = (int)a;
				if (k >= 0xFF) {
					return 0xFF;
				}
				return (unsigned char) k;
			}
		
		public:

			explicit Encode(

			) = default;

			~Encode(

			) = default;

			/**
			 * color: the extracted color
			 * return: the encoded vector
			*/

			inline static auto rgba(
				const Image<int> & image
			) -> std::vector<unsigned char>
			{
				auto color = image.color();
				auto result = std::vector<unsigned char>{};
				result.reserve(color.red.size() + color.green.size() + color.blue.size() + color.alpha.size());
				for (auto i : Range<size_t>(color.red.size())){
					result.emplace_back(color.red.at(i));
					result.emplace_back(color.green.at(i));
					result.emplace_back(color.blue.at(i));
					result.emplace_back(color.alpha.at(i));
				}
				return result;
			}

			/**
			 * color: the extracted color
			 * return: the encoded vector
			*/

			inline static auto argb(
				const Image<int> & image
			) -> std::vector<unsigned char>
			{
				auto color = image.color();
				auto result = std::vector<unsigned char>{};
				result.reserve(color.red.size() + color.green.size() + color.blue.size() + color.alpha.size());
				for (auto i : Range<size_t>(color.red.size())){
					result.emplace_back(color.alpha.at(i));
					result.emplace_back(color.red.at(i));
					result.emplace_back(color.green.at(i));
					result.emplace_back(color.blue.at(i));
				}
				return result;
			}

			inline static auto rgba_4444(
				const Image<int> & image
			) -> std::vector<unsigned char>
			{
				auto sen = DataStreamView{};
				auto data = image.data();
				for (auto y : Range<int>(image.height)) {
					for (auto x : Range<int>(image.width)) {
						auto index = set_pixel(x, y, image.width);
						auto color = ((data[index + 3]) >> 4 | ((data[index + 2]) & 0xF0) | (((data[index + 1]) & 0xF0) << 4) |
							(((data[index]) & 0xF0) << 8));
						sen.writeUint16LE(color);
					}
				}
				return sen.get();
			}

			inline static auto rgb_565(
				const Image<int> & image
			) -> std::vector<unsigned char>
			{
				auto sen = DataStreamView{};
				auto data = image.data();
				for (auto y : Range<int>(image.height)) {
					for (auto x : Range<int>(image.width)) {
						auto index = set_pixel(x, y, image.width);
						auto color = ((data[index + 2]) >> 3 | (((data[index + 1]) & 0xFC) << 3) | (((data[index]) & 0xF8) << 8));
						sen.writeUint16LE(color);
					}
				}
				return sen.get();
			}

			inline static auto rgba_5551(
				const Image<int> & image
			) -> std::vector<unsigned char>
			{
				auto sen = DataStreamView{};
				auto data = image.data();
				for (auto y : Range<int>(image.height)) {
					for (auto x : Range<int>(image.width)) {
						auto index = set_pixel(x, y, image.width);
						auto color = ((((data[index + 3]) & 0x80) >> 7) |
						(((data[index + 2]) & 0xF8) >> 2) |
						(((data[index + 1]) & 0xF8) << 3) |
						(((data[index + 0]) & 0xF8) << 8));
						sen.writeUint16LE(color);
					}
				}
				return sen.get();
			}

			inline static auto rgba_4444_tiled(
				const Image<int> & image
			) -> std::vector<unsigned char>
			{
				auto sen = DataStreamView{};
				auto data = image.data();
				for (auto y : Range<int>(0, image.height, 32)) {
					for (auto x : Range<int>(0, image.width, 32)) {
						for (auto j : Range<int>(32)) {
							for (auto k : Range<int>(32)) {
								if ((y + j) < image.height and (x + k) < image.width) {
									auto index = set_pixel(x + k, y + j, image.width);
									auto color = ((data[index + 3]) >> 4 |
										(data[index + 2]) & 0xF0 |
										(((data[index + 1]) & 0xF0) << 4) |
										(((data[index + 0]) & 0xF0) << 8));
									sen.writeUint16LE(color);
								} 
								else {
									sen.writeUint16LE(0x00);
								}
							}
						}
					}
				}
				return sen.get();
			}

			inline static auto rgb_565_tiled(
				const Image<int> & image
			) -> std::vector<unsigned char>
			{
				auto sen = DataStreamView{};
				auto data = image.data();
				for (auto y : Range<int>(0, image.height, 32)) {
					for (auto x : Range<int>(0, image.width, 32)) {
						for (auto j : Range<int>(32)) {
							for (auto k : Range<int>(32)) {
								if ((y + j) < image.height and (x + k) < image.width) {
									auto index = set_pixel(x + k, y + j, image.width);
									auto color = (((data[index + 2]) & 0xF8 >> 3) |
												(((data[index + 1]) & 0xFC) << 3) |
												(((data[index + 0]) & 0xF8) << 8));
									sen.writeUint16LE(color);
								} 
								else {
									sen.writeUint16LE(0x00);
								}
							}
						}
					}
				}
				return sen.get();
			}

			inline static auto rgba_5551_tiled(
				const Image<int> & image
			) -> std::vector<unsigned char>
			{
				auto sen = DataStreamView{};
				auto data = image.data();
				for (auto y : Range<int>(0, image.height, 32)) {
					for (auto x : Range<int>(0, image.width, 32)) {
						for (auto j : Range<int>(32)) {
							for (auto k : Range<int>(32)) {
								if ((y + j) < image.height and (x + k) < image.width) {
									auto index = set_pixel(x + k, y + j, image.width);
									auto color = ((((data[index + 3]) & 0x80) >> 7) |
												((data[index + 2]) & 0xF8 >> 2) |
												(((data[index + 1]) & 0xF8) << 3) |
												(((data[index + 0]) & 0xF8) << 8));
									sen.writeUint16LE(color);
								} 
								else {
									sen.writeUint16LE(0x00);
								}
							}
						}
					}
				}
				return sen.get();
			}

			inline static auto rgb_etc1_a_8(
				const Image<int> & image
			) -> std::vector<unsigned char>
			{
				auto size = image.area();
				auto view = std::unique_ptr<unsigned int[]>(new unsigned int[size]);
				auto data = image.data();
				auto index = 0;
				for	(auto y : Range<int>(image.height)){
					for (auto x : Range<int>(image.width)){
						auto pixel = set_pixel(x, y, image.width);
						view[index++] = (data[pixel + 3] << 24 | data[pixel] << 16 | data[pixel + 1] << 8 | data[pixel + 2]);
					}
				}
				auto destination_size = size / 16;
				auto destination = std::unique_ptr<unsigned long long[]>(new unsigned long long[destination_size]);
				CompressEtc1Rgb(view.get(), destination.get(), static_cast<unsigned int>(destination_size), static_cast<size_t>(image.width));
				auto sen = DataStreamView{};
				for (auto i : Range<int>(destination_size)) {
					sen.writeUint64LE(destination[i]);
				}
				for (auto y : Range<int>(image.height)) {
					for (auto x : Range<int>(image.width)) {
						sen.writeUint8(data[set_pixel(x, y, image.width) + 3]);
					}
				}
				return sen.get();
			}

			inline static auto rgb_etc1_a_palette(
				const Image<int> & image
			) -> std::vector<unsigned char>
			{
				auto size = image.area();
				auto view = std::unique_ptr<unsigned int[]>(new unsigned int[size]);
				auto data = image.data();
				auto index = 0;
				for	(auto y : Range<int>(image.height)){
					for (auto x : Range<int>(image.width)){
						auto pixel = set_pixel(x, y, image.width);
						view[index++] = (data[pixel + 3] << 24 | data[pixel] << 16 | data[pixel + 1] << 8 | data[pixel + 2]);
					}
				}
				auto destination_size = size / 16;
				auto destination = std::unique_ptr<unsigned long long[]>(new unsigned long long[destination_size]);
				CompressEtc1Rgb(view.get(), destination.get(), static_cast<unsigned int>(destination_size), static_cast<size_t>(image.width));
				auto sen = DataStreamView{};
				for (auto i : Range<int>(destination_size)) {
					sen.writeUint64LE(destination[i]);
				}
				sen.writeUint8(0x10);
				for (auto i : Range<uint8_t>(16)) {
					sen.writeUint8(i);
				}
				auto half_size = size / 2;
				for (auto i : Range<int>(half_size)) {
					sen.writeUint8(static_cast<uint8_t>(data[(i << 1) * 4] & 0xF0 | data[((i << 1) | 1) * 4] >> 4));
				}
				if((size & 1) == 1){
					sen.writeUint8(static_cast<uint8_t>(data[(half_size * 4 << 1)] & 0xF0));
				}
				return sen.get();
			}

			inline static auto a_8(
				const Image<int> & image
			) -> std::vector<unsigned char>
			{
				auto sen = DataStreamView{};
				auto data = image.data();
				for (auto i = 0; i < image.area() * 4; i += 4) {
					sen.writeUint8(data[i + 3]);
				}
				return sen.get();
			}

			inline static auto argb_1555(
				const Image<int> & image
			) -> std::vector<unsigned char>
			{
				auto sen = DataStreamView{};
				auto data = image.data();
				for (auto i = 0; i < image.area() * 4; i += 4) {
					sen.writeUint16LE(static_cast<unsigned int>(((data[i + 3] & 0x80) << 8) | (data[i + 2] >> 3) | ((data[i + 1] & 0xF8) << 2) | ((data[i] & 0xF8) << 7)));
				}
				return sen.get();
			}

			inline static auto argb_4444(
				const Image<int> & image
			) -> std::vector<unsigned char>
			{
				auto sen = DataStreamView{};
				auto data = image.data();
				for (auto i = 0; i < image.area() * 4; i += 4){
					sen.writeUint16LE(static_cast<unsigned int>((data[i + 2] >> 4) | (data[i + 1] & 0xF0) | ((data[i] & 0xF0) << 4) | ((data[i + 3] & 0xF0) << 8)));
				}
				return sen.get();
			}

			inline static auto l_8(
				const Image<int> & image
			) -> std::vector<unsigned char>
			{
				auto sen = DataStreamView{};
				auto data = image.data();
				for (auto i = 0; i < image.area() * 4; i += 4){
					sen.writeUint8(Encode::convert_luminance_from_rgb(data[i], data[i + 1], data[i + 2]));
				}
				return sen.get();
			}

			inline static auto la_44(
				const Image<int> & image
			) -> std::vector<unsigned char>
			{
				auto sen = DataStreamView{};
				auto data = image.data();
				for (auto i = 0; i < image.area() * 4; i += 4){
					sen.writeUint8(Encode::convert_luminance_from_rgba(data[i], data[i + 1], data[i + 2], data[i + 3]));
				}
				return sen.get();
			}

			inline static auto la_88(
				const Image<int> & image
			) -> std::vector<unsigned char>
			{
				auto sen = DataStreamView{};
				auto data = image.data();
				for (auto i = 0; i < image.area() * 4; i += 4){
					sen.writeUint16LE(static_cast<uint16_t>(Encode::convert_luminance_from_rgb(data[i], data[i + 1], data[i + 2], data[i + 3])));
				}
				return sen.get();
			}
	};
	
}