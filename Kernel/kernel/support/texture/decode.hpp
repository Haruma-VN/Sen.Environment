#pragma once

#include "kernel/definition/utility.hpp"
#include "kernel/support/texture/common.hpp"
#include "kernel/support/texture/compression/pvrtc/pvrtc.hpp"
namespace Sen::Kernel::Support::Texture
{

	using namespace Sen::Kernel::Support::Texture::Compression;
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
				data.emplace_back(color[i + 2]);
				// green
				data.emplace_back(color[i + 1]);
				// blue
				data.emplace_back(color[i]);
				// alpha
				data.emplace_back(color[i + 3]);
			}
			return data;
		}

		template <typename T>
			requires std::is_integral<T>::value
		inline static auto read_one_bit(
			int &bitPostion,
			unsigned char &buffer,
			DataStreamView &image_bytes) -> int
		{
			if (bitPostion == 0)
			{
				buffer = image_bytes.readUint8();
			}
			bitPostion = (bitPostion + 7) & 7;
			return static_cast<int>((buffer >> bitPostion) & 0b1);
		}

		template <typename T>
			requires std::is_integral<T>::value
		inline static auto read_bits(
			int bits,
			int &bitPostion,
			unsigned char &buffer,
			DataStreamView &image_bytes) -> int
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
			int height
		) -> Image<int>
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
			int height
		) -> Image<int>
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
			auto image_block = std::make_unique<uint32_t[]>(width * height);
			auto dst = image_block.get();
			for (auto block_y : Range<int>(height / k_block_width))
			{
				for (auto block_x : Range<int>(width / k_block_width))
				{
					auto d = sen.readUint64();
					BlockData::DecodeRGBBBlock(d, dst, width);
					dst += 4;
				}
				dst += width * 3;
			}
			auto data = DataStreamView{};
			for (auto i : Range<int>(width * height))
			{
				data.writeUint32(image_block[i]);
			}
			return Image<int>{0, 0, width, height, data.toBytes()};
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
			auto image_square = width * height;
			auto image_block = std::make_unique<uint32_t[]>(image_square);
			auto dst = image_block.get();
			for (auto block_y : Range<int>(height / k_block_width))
			{
				for (auto block_x : Range<int>(width / k_block_width))
				{
					auto d = sen.readUint64();
					BlockData::DecodeRGBBBlock(d, dst, width);
					dst += 4;
				}
				dst += width * 3;
			}
			auto data = DataStreamView{};
			for (auto i : Range<int>(image_square))
			{
				data.writeInt24(image_block[i]);
				data.writeUint8(sen.readUint8());
			}
			return Image<int>{0, 0, width, height, data.toBytes()};
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
			auto sen = DataStreamView{color};
			auto image_square = width * height;
			auto image_block = std::make_unique<uint32_t[]>(image_square);
			auto dst = image_block.get();
			for (auto block_y : Range<int>(height / k_block_width))
			{
				for (auto block_x : Range<int>(width / k_block_width))
				{
					auto d = sen.readUint64();
					BlockData::DecodeRGBBBlock(d, dst, width);
					dst += 4;
				}
				dst += width * 3;
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
				bit_depth = num == 1 ? 1 : std::ilogb(num - 1) + 1;
				for (auto i : Range(num))
				{
					auto p_byte = sen.readUint8();
					index_table[i] = static_cast<unsigned char>(((p_byte << 4) | p_byte));
				}
			}
			auto bitPostion = 0;
			auto buffer = static_cast<unsigned char>(0x00);

			auto data = DataStreamView{};
			for (auto i : Range<int>(image_square))
			{
				data.writeInt24(image_block[i]);
				data.writeUint8(index_table[read_bits<int>(bit_depth, bitPostion, buffer, sen)]);
			}
			return Image<int>{0, 0, width, height, data.toBytes()};
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
				newWidth = 0b10 << (static_cast<int>(Math::floor(Math::log2(newWidth))));
			}
			if ((newHeight & (newHeight - 1)) != 0)
			{
				newHeight = 0b10 << (static_cast<int>(Math::floor(Math::log2(newHeight))));
			}
			if (newWidth != newHeight)
			{
				newWidth = newHeight = Math::compare(newWidth, newHeight);
			}
			auto packets = std::vector<PVRTC::Packet>{};
			auto packets_length = ((newWidth * newHeight) >> 4);
			for (auto i : Range<int>(packets_length)) {
				packets.emplace_back(PVRTC::Packet{sen.readUint64()});
			}
			auto image_data = PVRTC::decode_4bpp(packets, newWidth);
			if (newWidth != width || newHeight != height) {
				auto data = std::vector<unsigned char>{};
				data.reserve(static_cast<std::vector<uint8_t, std::allocator<uint8_t>>::size_type>(width * height) * 4);
				for (auto j : Range<int>(height)) {
					for (auto i : Range<int>(width)) {
						auto index = (j * newWidth + i) * 4;
						data.insert(data.end(), &image_data[index], &image_data[static_cast<std::vector<uint8_t, std::allocator<uint8_t>>::size_type>(index) + 4]);
					}
				}
				image_data = std::move(data);
			}
			for (auto i : Range<int>(width * height)) {
				image_data[i * 4 + 3] = sen.readUint8();
			}
			return Image<int>{0, 0, width, height, image_data};;
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
				newWidth = 0b10 << (static_cast<int>(Math::floor(Math::log2(newWidth))));
			}
			if ((newHeight & (newHeight - 1)) != 0)
			{
				newHeight = 0b10 << (static_cast<int>(Math::floor(Math::log2(newHeight))));
			}
			if (newWidth != newHeight)
			{
				newWidth = newHeight = Math::compare(newWidth, newHeight);
			}
			auto packets = std::vector<PVRTC::Packet>{};
			auto packets_length = ((newWidth * newHeight) >> 4);
			for (auto i : Range<int>(packets_length)) {
				packets.emplace_back(PVRTC::Packet{sen.readUint64()});
			}
			auto image_data = PVRTC::decode_4bpp(packets, newWidth);
			auto image = Image<int>{0, 0, newWidth, newHeight, image_data};
			if (newWidth != width || newHeight != height) {
				auto new_image = Image<int>::composite(image, Rectangle{0, 0, width, height});
				return new_image;
			}
			return image;
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
			int height
		) -> Image<int>
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