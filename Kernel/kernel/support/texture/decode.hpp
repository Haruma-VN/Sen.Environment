#pragma once

#include "kernel/definition/utility.hpp"
#include "kernel/support/texture/common.hpp"
#include "kernel/support/texture/compression/pvrtc/pvrtc.hpp"

namespace Sen::Kernel::Support::Texture
{

	// pixel color

	class PixelColor
	{

	public:
		// non alpha

		inline static constexpr auto NON_ALPHA = static_cast<unsigned char>(0xFF);

		// alpha

		inline static constexpr auto ALPHA = static_cast<unsigned char>(0x00);

		// k_block_width

		inline static constexpr auto k_block_width = 4;

		// k_none_size

		inline static constexpr auto k_none_size = 0;

		// k_begin_index

		inline static constexpr auto k_begin_index = 0;

		// k_bpp

		inline static constexpr auto k_bpp = 4;
	};

	// use color

	using namespace Sen::Kernel::Definition;

	/**
	 * Decode class
	 */

	class Decode : public PixelColor
	{

	private:
		/**
		 * Convert alpha red green blue to red green blue alpha (standard)
		 */

		inline static auto argb_to_rgba(
			const std::vector<unsigned char> &color) -> std::vector<unsigned char>
		{
			auto data = std::vector<unsigned char>();
			data.reserve(color.size());
			for (auto i : Range<size_t>(color.size(), 4))
			{
				// red
				data.emplace_back(color[i + 1]);
				// green
				data.emplace_back(color[i + 2]);
				// blue
				data.emplace_back(color[i + 3]);
				// alpha
				data.emplace_back(color[i]);
			}
			return data;
		}

		template <typename T>
			requires std::is_integral<T>::value
		inline static auto read_one_bit(
			int &bitPostion,
			unsigned char &buffer,
			DataStreamViewBigEndian &image_bytes) -> int
		{
			if (bitPostion == 0)
			{
				buffer = image_bytes.readUint8();
			}
			bitPostion = (bitPostion + 7) & 7;
			return static_cast<int>((buffer >> bitPostion) & 1);
		}

		template <typename T>
			requires std::is_integral<T>::value
		inline static auto read_bits(
			int bits,
			int &bitPostion,
			unsigned char &buffer,
			DataStreamViewBigEndian &image_bytes) -> int
		{
			auto ans = 0;
			for (auto i = bits - 1; i >= 0; i--)
			{
				ans |= read_one_bit<T>(bitPostion, buffer, image_bytes) << i;
			}
			return ans;
		}

	public:
		/**
		 * color: stream color
		 * width: image width
		 * height: image height
		 * return: Image struct
		 */

		inline static auto rgba_8888(
			const std::vector<unsigned char> &color,
			int width,
			int height) -> Image<int>
		{
			return Image<int>(0, 0, width, height, color);
		}

		/**
		 * color: stream color
		 * width: image width
		 * height: image height
		 * return: Image struct
		 */

		inline static auto argb_8888(
			const std::vector<unsigned char> &color,
			int width,
			int height) -> Image<int>
		{
			return rgba_8888(argb_to_rgba(color), width, height);
		}

		/**
		 * color: stream color
		 * width: image width
		 * height: image height
		 * return: Image struct
		 */

		inline static auto rgba_4444(
			const std::vector<unsigned char> &color,
			int width,
			int height) -> Image<int>
		{
			auto sen = DataStreamView{color};
			auto area = pixel_area_rgba(width, height);
			auto data = std::vector<unsigned char>(area, 0x00);
			for (auto y : Range<int>(height))
			{
				for (auto x : Range<int>(width))
				{
					auto temp_pixel = sen.readUint16();
					auto red = temp_pixel >> 12;
					auto green = (temp_pixel & 0xF00) >> 8;
					auto blue = (temp_pixel & 0xF0) >> 4;
					auto alpha = temp_pixel & 0xF;
					auto index = static_cast<std::vector<uint8_t, std::allocator<uint8_t>>::size_type>(set_pixel(x, y, width));
					data[index] = static_cast<unsigned char>((red << 4) | red);
					data[index + 1] = static_cast<unsigned char>((green << 4) | green);
					data[index + 2] = static_cast<unsigned char>((blue << 4) | blue);
					data[index + 3] = static_cast<unsigned char>((alpha << 4) | alpha);
				}
			}
			return Image<int>{0, 0, width, height, data};
		}

		/**
		 * color: stream color
		 * width: image width
		 * height: image height
		 * return: Image struct
		 */

		inline static auto rgb_565(
			const std::vector<unsigned char> &color,
			int width,
			int height) -> Image<int>
		{
			auto sen = DataStreamView{color};
			auto area = pixel_area_rgba(width, height);
			auto data = std::vector<unsigned char>(area, 0x00);
			for (auto y : Range<int>(height))
			{
				for (auto x : Range<int>(width))
				{
					auto temp_pixel = sen.readUint16();
					auto red = temp_pixel >> 11;
					auto green = (temp_pixel & 0x7E0) >> 5;
					auto blue = temp_pixel & 0x1F;
					auto index = static_cast<std::vector<uint8_t, std::allocator<uint8_t>>::size_type>(set_pixel(x, y, width));
					data[index] = static_cast<unsigned char>((red << 3) | (red >> 2));
					data[index + 1] = static_cast<unsigned char>((green << 2) | (green >> 4));
					data[index + 2] = static_cast<unsigned char>((blue << 3) | (blue >> 2));
					data[index + 3] = PixelColor::NON_ALPHA;
				}
			}
			return Image<int>{0, 0, width, height, data};
		}

		/**
		 * color: stream color
		 * width: image width
		 * height: image height
		 * return: Image struct
		 */

		inline static auto rgba_5551(
			const std::vector<unsigned char> &color,
			int width,
			int height) -> Image<int>
		{
			auto sen = DataStreamView{color};
			auto area = pixel_area_rgba(width, height);
			auto data = std::vector<unsigned char>(area, 0x00);
			for (auto y : Range<int>(height))
			{
				for (auto x : Range<int>(width))
				{
					auto temp_pixel = sen.readUint16();
					auto red = (temp_pixel & 0xF800) >> 11;
					auto green = (temp_pixel & 0x7E0) >> 6;
					auto blue = (temp_pixel & 0x3E) >> 1;
					auto index = static_cast<std::vector<uint8_t, std::allocator<uint8_t>>::size_type>(set_pixel(x, y, width));
					data[index] = static_cast<unsigned char>((red << 3) | (red >> 2));
					data[index + 1] = static_cast<unsigned char>((green << 3) | (green >> 2));
					data[index + 2] = static_cast<unsigned char>((blue << 3) | (blue >> 2));
					data[index + 3] = static_cast<unsigned char>(-(temp_pixel & 0x1));
				}
			}
			return Image<int>{0, 0, width, height, data};
		}

		/**
		 * color: stream color
		 * width: image width
		 * height: image height
		 * return: Image struct
		 */

		inline static auto rgba_4444_tiled(
			const std::vector<unsigned char> &color,
			int width,
			int height) -> Image<int>
		{
			auto sen = DataStreamView{color};
			auto area = pixel_area_rgba(width, height);
			auto data = std::vector<unsigned char>(area, 0x00);
			for (auto i : Range<int>(0, height, 32))
			{
				for (auto w : Range<int>(0, width, 32))
				{
					for (auto j : Range<int>(32))
					{
						for (auto k : Range<int>(32))
						{
							auto temp_pixel = sen.readUint16();
							if ((i + j) < height and (w + k) < width)
							{
								auto red = temp_pixel >> 12;
								auto green = (temp_pixel & 0xF00) >> 8;
								auto blue = (temp_pixel & 0xF0) >> 4;
								auto alpha = temp_pixel & 0xF;
								auto index = static_cast<std::vector<uint8_t, std::allocator<uint8_t>>::size_type>(set_pixel((w + k), (i + j), width));
								data[index] = static_cast<unsigned char>((red << 4) | red);
								data[index + 1] = static_cast<unsigned char>((green << 4) | green);
								data[index + 2] = static_cast<unsigned char>((blue << 4) | blue);
								data[index + 3] = static_cast<unsigned char>((alpha << 4) | alpha);
							}
						}
					}
				}
			}
			return Image<int>{0, 0, width, height, data};
		}

		/**
		 * color: stream color
		 * width: image width
		 * height: image height
		 * return: Image struct
		 */

		inline static auto rgb_565_tiled(
			const std::vector<unsigned char> &color,
			int width,
			int height) -> Image<int>
		{
			auto sen = DataStreamView{color};
			auto area = pixel_area_rgba(width, height);
			auto data = std::vector<unsigned char>(area, 0x00);
			for (auto i : Range<int>(0, height, 32))
			{
				for (auto w : Range<int>(0, width, 32))
				{
					for (auto j : Range<int>(32))
					{
						for (auto k : Range<int>(32))
						{
							auto temp_pixel = sen.readUint16();
							if ((i + j) < height and (w + k) < width)
							{
								auto red = (temp_pixel & 0xF800) >> 8;
								auto green = (temp_pixel & 0x7E0) >> 3;
								auto blue = (temp_pixel & 0x1F) << 3;
								auto index = static_cast<std::vector<uint8_t, std::allocator<uint8_t>>::size_type>(set_pixel((w + k), (i + j), width));
								data[index] = static_cast<unsigned char>(red);
								data[index + 1] = static_cast<unsigned char>(green);
								data[index + 2] = static_cast<unsigned char>(blue);
								data[index + 3] = PixelColor::NON_ALPHA;
							}
						}
					}
				}
			}
			return Image<int>{0, 0, width, height, data};
		}

		/**
		 * color: stream color
		 * width: image width
		 * height: image height
		 * return: Image struct
		 */

		inline static auto rgba_5551_tiled(
			const std::vector<unsigned char> &color,
			int width,
			int height) -> Image<int>
		{
			auto sen = DataStreamView{color};
			auto area = pixel_area_rgba(width, height);
			auto data = std::vector<unsigned char>(area, 0x00);
			for (auto i : Range<int>(0, height, 32))
			{
				for (auto w : Range<int>(0, width, 32))
				{
					for (auto j : Range<int>(32))
					{
						for (auto k : Range<int>(32))
						{
							auto temp_pixel = sen.readUint16();
							if ((i + j) < height and (w + k) < width)
							{
								auto red = temp_pixel >> 11;
								auto green = (temp_pixel & 0x7C0) >> 6;
								auto blue = (temp_pixel & 0x3E) >> 1;
								auto index = static_cast<std::vector<uint8_t, std::allocator<uint8_t>>::size_type>(set_pixel((w + k), (i + j), width));
								data[index] = static_cast<unsigned char>((red << 3) | (red >> 2));
								data[index + 1] = static_cast<unsigned char>((green << 3) | (green >> 2));
								data[index + 2] = static_cast<unsigned char>((blue << 3) | (blue >> 2));
								data[index + 3] = static_cast<unsigned char>(-(temp_pixel & 0x1));
							}
						}
					}
				}
			}
			return Image<int>{0, 0, width, height, data};
		}

		/**
		 * color: stream color
		 * width: image width
		 * height: image height
		 * return: Image struct
		 */

		inline static auto rgb_etc1(
			const std::vector<unsigned char> &color,
			int width,
			int height) -> Image<int>
		{
			auto sen = DataStreamView{color};
			auto area = pixel_area_rgba(width, height);
			auto data = std::vector<unsigned char>(area, 0x00);
			auto image_block = std::make_unique<uint32_t[]>(pixel_area_rgba(k_block_width, k_block_width));
			/*
			for (auto block_y : Range<int>(height / k_block_width))
			{
				for (auto block_x : Range<int>(width / k_block_width))
				{
					auto d = sen.readUint64();
					// DecodeRGBPart(d, image_block.get(), width);
				}
			}*/
			return Image<int>{0, 0, width, height, data};
		}

		/**
		 * color: stream color
		 * width: image width
		 * height: image height
		 * return: Image struct
		 */

		inline static auto rgb_etc1_a_8(
			const std::vector<unsigned char> &color,
			int width,
			int height) -> Image<int>
		{
			auto sen = DataStreamView{color};
			auto area = pixel_area_rgba(width, height);
			auto data = std::vector<unsigned char>(area, 0x00);
			auto image_block = std::make_unique<uint8_t[]>(pixel_area_rgba(k_block_width, k_block_width));
			for (auto block_y : Range<int>(height / k_block_width))
			{
				for (auto block_x : Range<int>(width / k_block_width))
				{
					auto block_part_1 = sen.readUint32();
					auto block_part_2 = sen.readUint32();
					decompressBlockETC2c(
						static_cast<unsigned int>(block_part_1),
						static_cast<unsigned int>(block_part_2),
						image_block.get(),
						k_block_width,
						k_block_width,
						k_begin_index,
						k_begin_index,
						4);
					for (auto pixel_y : Range<int>(k_block_width))
					{
						for (auto pixel_x : Range<int>(k_block_width))
						{
							auto index = static_cast<std::vector<uint8_t, std::allocator<uint8_t>>::size_type>(set_pixel((block_x * k_block_width + pixel_x), (block_y * k_block_width + pixel_y), width));
							auto block_index = static_cast<std::vector<uint8_t, std::allocator<uint8_t>>::size_type>(set_pixel(pixel_x, pixel_y, k_block_width));
							data[index] = image_block[block_index];
							data[index + 1] = image_block[block_index + 1];
							data[index + 2] = image_block[block_index + 2];
							data[index + 3] = image_block[block_index + 3];
						}
					}
				}
			}
			return Image<int>{0, 0, width, height, data};
		}

		/**
		 * color: stream color
		 * width: image width
		 * height: image height
		 * return: Image struct
		 */

		inline static auto rgb_etc1_a_palette(
			const std::vector<unsigned char> &color,
			int width,
			int height) -> Image<int>
		{
			auto sen = DataStreamViewBigEndian{color};
			auto area = pixel_area_rgba(width, height);
			auto data = std::vector<unsigned char>(area, 0x00);
			auto image_block = std::make_unique<uint8_t[]>(pixel_area_rgba(k_block_width, k_block_width));
			for (auto block_y : Range<int>(height / k_block_width))
			{
				for (auto block_x : Range<int>(width / k_block_width))
				{
					auto block_part_1 = sen.readUint32();
					auto block_part_2 = sen.readUint32();
					decompressBlockETC2c(
						static_cast<unsigned int>(block_part_1),
						static_cast<unsigned int>(block_part_2),
						image_block.get(),
						k_block_width,
						k_block_width,
						k_begin_index,
						k_begin_index,
						4);
					for (auto pixel_y : Range<int>(k_block_width))
					{
						for (auto pixel_x : Range<int>(k_block_width))
						{
							auto index = static_cast<std::vector<uint8_t, std::allocator<uint8_t>>::size_type>(set_pixel((block_x * k_block_width + pixel_x), (block_y * k_block_width + pixel_y), width));
							auto block_index = static_cast<std::vector<uint8_t, std::allocator<uint8_t>>::size_type>(set_pixel(pixel_x, pixel_y, k_block_width));
							data[index] = image_block[block_index];
							data[index + 1] = image_block[block_index + 1];
							data[index + 2] = image_block[block_index + 2];
						}
					}
				}
			}
			auto num = sen.readUint8();
			auto index_table = std::make_unique<uint8_t[]>(num == 0 ? 2 : num);
			auto bit_depth = int{};
			if (num == 0)
			{
				index_table[0] = 0x0;
				index_table[1] = 0xFF;
				bit_depth = 1;
			}
			else
			{
				for (auto i : Range(num))
				{
					auto p_byte = sen.readUint8();
					index_table[i] = static_cast<unsigned char>((p_byte << 4) | p_byte);
				}
				auto tableSize = 2;
				for (bit_depth = 1; num > tableSize; bit_depth++)
				{
					tableSize *= 2;
				}
			}
			auto bitPostion = 0;
			auto buffer = static_cast<unsigned char>(0x00);
			for (auto y : Range<int>(height))
			{
				for (auto x : Range<int>(width))
				{
					data[static_cast<std::vector<uint8_t, std::allocator<uint8_t>>::size_type>(set_pixel(x, y, width)) + 3] = index_table[read_bits<int>(bit_depth, bitPostion, buffer, sen)];
				}
			}
			return Image<int>{0, 0, width, height, data};
		}

		/**
		 * color: stream color
		 * width: image width
		 * height: image height
		 * return: Image struct
		 */

		inline static auto rgba_pvrtc_4bpp(
			const std::vector<unsigned char> &color,
			int width,
			int height) -> Image<int>
		{
			auto sen = DataStreamView{color};
			auto newWidth = width;
			auto newHeight = height;
			if (newWidth < 8)
			{
				newWidth = 8;
			}
			if (newHeight < 8)
			{
				newHeight = 8;
			}
			if ((newWidth & (newWidth - 1)) != 0)
			{
				newWidth = 0b10 << ((int)Math::floor(Math::log2(newWidth)));
			}
			if ((newHeight & (newHeight - 1)) != 0)
			{
				newHeight = 0b10 << ((int)Math::floor(Math::log2(newHeight)));
			}
			if (newWidth != newHeight)
			{
				newWidth = newHeight = Math::compare(newWidth, newHeight);
			}
			auto result = std::vector<Javelin::ColorRgba<unsigned char>>(static_cast<size_t>(newWidth * newHeight));
			Javelin::PvrTcDecoder::DecodeRgba4Bpp(result.data(), Javelin::Point2<int>(width, height), sen.get().data());
			auto data = std::vector<uint8_t>{};
			for (auto &c : result)
			{
				data.emplace_back(c.r);
				data.emplace_back(c.g);
				data.emplace_back(c.b);
				data.emplace_back(c.a);
			}
			return Image<int>{0, 0, width, height, data};

			// auto sen = DataStreamView{color};
			// auto area = pixel_area_rgba(width, height);
			// auto data = std::vector<unsigned char>(area, 0x00);
			// auto actual_data = std::unique_ptr<Javelin::ColorRgba<unsigned char>[]>(new Javelin::ColorRgba<unsigned char>[calculate_area(width, height)]);
			// Javelin::PvrTcDecoder::DecodeRgba4Bpp(actual_data.get(), Javelin::Point2<int>(width, height), sen.current_pointer()._Ptr);
			// for (auto y : Range<int>(height)) {
			// 	for (auto x : Range<int>(width)) {
			// 		auto index = set_pixel(x, y, width);
			// 		auto block_index = y * width + x;
			// 		data[index] = actual_data[block_index].r;
			// 		data[index + 1] = actual_data[block_index].g;
			// 		data[index + 2] = actual_data[block_index].b;
			// 		data[index + 3] = actual_data[block_index].a;
			// 	}
			// }
		}

		/**
		 * color: stream color
		 * width: image width
		 * height: image height
		 * return: Image struct
		 */

		inline static auto rgb_pvrtc_4bpp_a_8(
			const std::vector<unsigned char> &color,
			int width,
			int height) -> Image<int>
		{
			auto sen = DataStreamView{color};
			auto area = pixel_area_rgba(width, height);
			auto data = std::vector<unsigned char>(area, 0x00);
			auto actual_data = std::make_unique<Javelin::ColorRgba<unsigned char>[]>(calculate_area(width, height));
			Javelin::PvrTcDecoder::DecodeRgba4Bpp(&actual_data[0], Javelin::Point2<int>(width, height), sen.getBytes(0, sen.size()).data());
			for (auto y : Range<int>(height))
			{
				for (auto x : Range<int>(width))
				{
					auto index = static_cast<std::vector<uint8_t, std::allocator<uint8_t>>::size_type>(set_pixel(x, y, width));
					auto block_index = static_cast<std::vector<uint8_t, std::allocator<uint8_t>>::size_type>(y * width + x);
					data[index] = actual_data[block_index].r;
					data[index + 1] = actual_data[block_index].g;
					data[index + 2] = actual_data[block_index].b;
					data[index + 3] = actual_data[block_index].a;
				}
			}

			// todo
			return Image<int>{0, 0, width, height, data};
		}

		inline static auto a_8(
			const std::vector<unsigned char> &color,
			int width,
			int height) -> Image<int>
		{
			auto sen = DataStreamView{color};
			auto area = pixel_area_rgba(width, height);
			auto data = std::vector<unsigned char>(area, 0x00);
			for (auto i = static_cast<std::size_t>(0); i < area; i += 4)
			{
				data[i] = Decode::NON_ALPHA;
				data[i + 1] = Decode::NON_ALPHA;
				data[i + 2] = Decode::NON_ALPHA;
				data[i + 3] = sen.readUint8();
			}
			return Image<int>{0, 0, width, height, data};
		}

		inline static auto argb_1555(
			const std::vector<unsigned char> &color,
			int width,
			int height) -> Image<int>
		{
			auto sen = DataStreamView{color};
			auto area = pixel_area_rgba(width, height);
			auto data = std::vector<unsigned char>(area, 0x00);
			for (auto i = static_cast<std::size_t>(0); i < area; i += 4)
			{
				auto pixel_color = sen.readUint16();
				auto red = (pixel_color & 0x7C00) >> 10;
				auto green = (pixel_color & 0x3E0) >> 5;
				auto blue = pixel_color & 0x1F;
				data[i] = ((red << 3) | (red >> 2));
				data[i + 1] = ((green << 3) | (green >> 2));
				data[i + 2] = ((blue << 3) | (blue >> 2));
				data[i + 3] = static_cast<unsigned char>(-(pixel_color >> 15));
			}
			return Image<int>{0, 0, width, height, data};
		}

		inline static auto argb_4444(
			const std::vector<unsigned char> &color,
			int width,
			int height) -> Image<int>
		{
			auto sen = DataStreamView{color};
			auto area = pixel_area_rgba(width, height);
			auto data = std::vector<unsigned char>(area, 0x00);
			for (auto i = static_cast<std::size_t>(0); i < area; i += 4)
			{
				auto pixel_color = sen.readUint16();
				auto alpha = pixel_color >> 12;
				auto red = (pixel_color & 0xF00) >> 8;
				auto green = (pixel_color & 0xF0) >> 4;
				auto blue = pixel_color & 0xF;
				data[i] = (red << 4) | red;
				data[i + 1] = (green << 4) | green;
				data[i + 2] = (blue << 4) | blue;
				data[i + 3] = (alpha << 4) | alpha;
			}
			return Image<int>{0, 0, width, height, data};
		}

		inline static auto l_8(
			const std::vector<unsigned char> &color,
			int width,
			int height) -> Image<int>
		{
			auto sen = DataStreamView{color};
			auto area = pixel_area_rgba(width, height);
			auto data = std::vector<unsigned char>(area, 0x00);
			for (auto i = static_cast<std::size_t>(0); i < area; i += 4)
			{
				auto color = sen.readUint8();
				data[i] = color;
				data[i + 1] = color;
				data[i + 2] = color;
				data[i + 3] = Decode::NON_ALPHA;
			}
			return Image<int>{0, 0, width, height, data};
		}

		inline static auto la_44(
			const std::vector<unsigned char> &color,
			int width,
			int height) -> Image<int>
		{
			auto sen = DataStreamView{color};
			auto area = pixel_area_rgba(width, height);
			auto data = std::vector<unsigned char>(area, 0x00);
			for (auto i = static_cast<std::size_t>(0); i < area; i += 4)
			{
				auto pixel_color = sen.readUint8();
				auto alpha = pixel_color & 0xF;
				pixel_color >>= 4;
				auto color = (pixel_color | (pixel_color << 4));
				data[i] = color;
				data[i + 1] = color;
				data[i + 2] = color;
				data[i + 3] = (alpha | (alpha << 4));
			}
			return Image<int>{0, 0, width, height, data};
		}

		inline static auto la_88(
			const std::vector<unsigned char> &color,
			int width,
			int height) -> Image<int>
		{
			auto sen = DataStreamView{color};
			auto area = pixel_area_rgba(width, height);
			auto data = std::vector<unsigned char>(area, 0x00);
			for (auto i = static_cast<std::size_t>(0); i < area; i += 4)
			{
				auto pixel_color = sen.readUint16();
				auto color = (pixel_color >> 8);
				data[i] = color;
				data[i + 1] = color;
				data[i + 2] = color;
				data[i + 3] = (pixel_color & 0xFF);
			}
			return Image<int>{0, 0, width, height, data};
		}
	};
}