#pragma once

#include "kernel/definition/utility.hpp"
#include "kernel/support/texture/decode.hpp"
#include "kernel/support/texture/compression/pvrtc/pvrtc.hpp"

namespace Sen::Kernel::Support::Texture
{

	using namespace Sen::Kernel::Support::Texture::Compression;
	// use color

	using namespace Sen::Kernel::Definition;

	/**
	 * Encode class
	 */

	class Encode : public PixelColor
	{

	private:
		/**
		 * l_8
		 */

		inline static auto convert_luminance_from_rgb(
			unsigned char red,
			unsigned char green,
			unsigned char blue) -> unsigned char
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
			unsigned char alpha) -> unsigned char
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
			unsigned char alpha) -> unsigned char
		{
			return Encode::round(static_cast<double>((static_cast<int>(red * 0.299f + green * 0.587f + blue * 0.114f) & 0xF0) | ((alpha) >> 4)));
		}

		inline static auto round(
			double a) -> unsigned char
		{
			auto k = (int)a;
			if (k >= 0xFF)
			{
				return 0xFF;
			}
			return (unsigned char)k;
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
			const Image<int> &image) -> std::vector<unsigned char>
		{
			auto color = image.color();
			auto result = std::vector<unsigned char>{};
			result.reserve(color.red.size() + color.green.size() + color.blue.size() + color.alpha.size());
			for (auto i : Range<size_t>(color.red.size()))
			{
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
			const Image<int> &image) -> std::vector<unsigned char>
		{
			auto color = image.color();
			auto result = std::vector<unsigned char>{};
			result.reserve(color.red.size() + color.green.size() + color.blue.size() + color.alpha.size());
			for (auto i : Range<size_t>(color.red.size()))
			{
				result.emplace_back(color.blue.at(i));
				result.emplace_back(color.green.at(i));
				result.emplace_back(color.red.at(i));
				result.emplace_back(color.alpha.at(i));
			}
			return result;
		}

		inline static auto rgba_4444(
			const Image<int> &image) -> std::vector<unsigned char>
		{
			auto stream = DataStreamView{};
			auto data = image.data();
			for (auto y : Range<int>(image.height))
			{
				for (auto x : Range<int>(image.width))
				{
					auto index = set_pixel(x, y, image.width);
					auto color = ((data[index + 3]) >> 4 | ((data[index + 2]) & 0xF0) | (((data[index + 1]) & 0xF0) << 4) |
								  (((data[index]) & 0xF0) << 8));
					stream.writeUint16(color);
				}
			}
			return stream.toBytes();
		}

		inline static auto rgb_565(
			const Image<int> &image) -> std::vector<unsigned char>
		{
			auto stream = DataStreamView{};
			auto data = image.data();
			for (auto y : Range<int>(image.height))
			{
				for (auto x : Range<int>(image.width))
				{
					auto index = set_pixel(x, y, image.width);
					auto color = static_cast<uint16_t>((data[index + 2] >> 3) | (((data[index + 1]) & 0xFC) << 3) | (((data[index]) & 0xF8) << 8));
					stream.writeUint16(color);
				}
			}
			return stream.toBytes();
		}

		inline static auto rgba_5551(
			const Image<int> &image) -> std::vector<unsigned char>
		{
			auto stream = DataStreamView{};
			auto data = image.data();
			for (auto y : Range<int>(image.height))
			{
				for (auto x : Range<int>(image.width))
				{
					auto index = set_pixel(x, y, image.width);
					auto color = ((((data[index + 3]) & 0x80) >> 7) |
								  (((data[index + 2]) & 0xF8) >> 2) |
								  (((data[index + 1]) & 0xF8) << 3) |
								  (((data[index + 0]) & 0xF8) << 8));
					stream.writeUint16(color);
				}
			}
			return stream.toBytes();
		}

		inline static auto rgba_4444_tiled(
			const Image<int> &image) -> std::vector<unsigned char>
		{
			auto stream = DataStreamView{};
			auto data = image.data();
			for (auto y : Range<int>(0, image.height, 32))
			{
				for (auto x : Range<int>(0, image.width, 32))
				{
					for (auto j : Range<int>(32))
					{
						for (auto k : Range<int>(32))
						{
							if ((y + j) < image.height and (x + k) < image.width)
							{
								auto index = set_pixel((x + k), (y + j), image.width);
								auto color = static_cast<uint16_t>((data[index + 3] >> 4) |
																   (data[index + 2] & 0xF0) |
																   (((data[index + 1]) & 0xF0) << 4) |
																   (((data[index + 0]) & 0xF0) << 8));
								stream.writeUint16(color);
							}
							else
							{
								stream.writeUint16(0x00);
							}
						}
					}
				}
			}
			return stream.toBytes();
		}

		inline static auto rgb_565_tiled(
			const Image<int> &image) -> std::vector<unsigned char>
		{
			auto stream = DataStreamView{};
			auto data = image.data();
			for (auto y : Range<int>(0, image.height, 32))
			{
				for (auto x : Range<int>(0, image.width, 32))
				{
					for (auto j : Range<int>(32))
					{
						for (auto k : Range<int>(32))
						{
							if ((y + j) < image.height and (x + k) < image.width)
							{
								auto index = set_pixel((x + k), (y + j), image.width);
								auto color = static_cast<uint16_t>(((data[index + 2] & 0xF8) >> 3) |
																   (((data[index + 1]) & 0xFC) << 3) |
																   (((data[index + 0]) & 0xF8) << 8));
								stream.writeUint16(color);
							}
							else
							{
								stream.writeUint16(0x00);
							}
						}
					}
				}
			}
			return stream.toBytes();
		}

		inline static auto rgba_5551_tiled(
			const Image<int> &image) -> std::vector<unsigned char>
		{
			auto stream = DataStreamView{};
			auto data = image.data();
			for (auto y : Range<int>(0, image.height, 32))
			{
				for (auto x : Range<int>(0, image.width, 32))
				{
					for (auto j : Range<int>(32))
					{
						for (auto k : Range<int>(32))
						{
							if ((y + j) < image.height and (x + k) < image.width)
							{
								auto index = set_pixel((x + k), (y + j), image.width);
								auto color = static_cast<uint16_t>(((data[index + 3] & 0x80) >> 7) |
																   ((data[index + 2] & 0xF8) >> 2) |
																   ((data[index + 1] & 0xF8) << 3) |
																   ((data[index + 0] & 0xF8) << 8));
								stream.writeUint16(color);
							}
							else
							{
								stream.writeUint16(0x00);
							}
						}
					}
				}
			}
			return stream.toBytes();
		}

		inline static auto rgb_etc1(
			const Image<int> &image) -> std::vector<unsigned char>
		{
			auto data = image.data();
			auto view = std::make_unique<uint8_t[]>(static_cast<size_t>(image.area() * 3)); 
			auto index = k_begin_index;
			auto encoded_size = etc1_get_encoded_data_size(static_cast<uint32_t>(image.width), static_cast<uint32_t>(image.height));
			for (auto y : Range<int>(image.height))
			{
				for (auto x : Range<int>(image.width))
				{
					auto pixel = set_pixel(x, y, image.width);
					view[index++] = data[pixel];
					view[index++] = data[pixel + 1];
					view[index++] = data[pixel + 2];
				}
			}
			auto destination = std::vector<uint8_t>(encoded_size);
			etc1_encode_image(view.get(), static_cast<uint32_t>(image.width), static_cast<uint32_t>(image.height), 3_ui, static_cast<uint32_t>(image.width * 3), destination.data());
			auto stream = DataStreamView{};
			stream.writeBytes(destination);
			return stream.toBytes();
		}

		inline static auto rgb_etc1_a_8(
			const Image<int> &image) -> std::vector<unsigned char>
		{
			auto stream = DataStreamView{};
			auto data = image.data();
			auto view = std::make_unique<uint8_t[]>(static_cast<size_t>(image.area() * 3)); 
			auto index = k_begin_index;
			auto encoded_size = etc1_get_encoded_data_size(static_cast<uint32_t>(image.width), static_cast<uint32_t>(image.height));
			stream.writeNull(encoded_size);
			for (auto y : Range<int>(image.height))
			{
				for (auto x : Range<int>(image.width))
				{
					auto pixel = set_pixel(x, y, image.width);
					view[index++] = data[pixel];
					view[index++] = data[pixel + 1];
					view[index++] = data[pixel + 2];
					stream.writeUint8(data[pixel + 3]);
				}
			}
			auto destination = std::vector<uint8_t>(encoded_size);
			etc1_encode_image(view.get(), static_cast<uint32_t>(image.width), static_cast<uint32_t>(image.height), 3_ui, static_cast<uint32_t>(image.width * 3), destination.data());
			stream.writeBytes(destination, 0_size);
			return stream.toBytes();
		}

		inline static auto rgb_etc1_a_palette(
			const Image<int> &image) -> std::vector<unsigned char>
		{
			auto data = image.data();
			auto view = std::make_unique<uint8_t[]>(static_cast<size_t>(image.area() * 3)); 
			auto index = k_begin_index;
			auto encoded_size = etc1_get_encoded_data_size(static_cast<uint32_t>(image.width), static_cast<uint32_t>(image.height));
			for (auto y : Range<int>(image.height))
			{
				for (auto x : Range<int>(image.width))
				{
					auto pixel = set_pixel(x, y, image.width);
					view[index++] = data[pixel];
					view[index++] = data[pixel + 1];
					view[index++] = data[pixel + 2];
				}
			}
			auto destination = std::vector<uint8_t>(encoded_size);
			etc1_encode_image(view.get(), static_cast<uint32_t>(image.width), static_cast<uint32_t>(image.height), 3_ui, static_cast<uint32_t>(image.width * 3), destination.data());
			auto stream = DataStreamView{};
			stream.writeBytes(destination);
			stream.writeUint8(0x10);
			for (auto i : Range<uint8_t>(16))
			{
				stream.writeUint8(i);
			}
			auto half_size = image.area() / 2;
			for (auto i : Range<int>(half_size))
			{
				stream.writeUint8(static_cast<uint8_t>((data[((i << 1) * 4) + 3] & 0b11110000) | (data[(((i << 1) | 1) * 4) + 3] >> 4)));
			}
			if ((half_size & 0b1) == 1)
			{
				stream.writeUint8(static_cast<uint8_t>(data[((half_size << 1) * 4) + 3] & 0b11110000));
			}
			return stream.toBytes();
		}

		inline static auto is_padded_size(
			int size,
			int unit) -> bool
		{
			return size % unit == k_none_size;
		}

		inline static auto rgba_pvrtc_4bpp(
			const Image<int> &image) -> std::vector<unsigned char>
		{
			auto newWidth = image.width;
			auto newHeight = image.height;
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
			auto new_image = Image<int>(0, 0, newWidth, newWidth, std::vector<uint8_t>(pixel_area_rgba(newWidth, newWidth), 0x00));
			auto source_data = new_image.data();
			auto image_data = image.data();
			for (auto j : Range<int>(image.height))
			{
				for (auto i : Range<int>(image.width))
				{
					auto source_index = ((j + image.y) * new_image.width + (i + image.x)) * 4;
					auto img_index = (j * image.width + i) * 4;
					source_data[source_index] = image_data[img_index];
					source_data[source_index + 1] = image_data[img_index + 1];
					source_data[source_index + 2] = image_data[img_index + 2];
					source_data[source_index + 3] = image_data[img_index + 3];
				}
			}
			auto packets = PVRTC::encode_rgba_4bpp<std::uint8_t>(source_data, newWidth);
			auto stream = DataStreamView{};
			for (auto i : Range<std::size_t>(packets.size()))
			{
				stream.writeUint64(packets[i].PvrTcWord);
			}
			return stream.toBytes();
		}

		inline static auto rgb_pvrtc_4bpp(
			const Image<int> &image) -> std::vector<unsigned char>
		{
			auto newWidth = image.width;
			auto newHeight = image.height;
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
			auto new_image = Image<int>(0, 0, newWidth, newWidth, std::vector<uint8_t>(pixel_area_rgba(newWidth, newWidth), 0x00));
			auto source_data = new_image.data();
			auto image_data = image.data();
			for (auto j : Range<int>(image.height))
			{
				for (auto i : Range<int>(image.width))
				{
					auto source_index = ((j + image.y) * new_image.width + (i + image.x)) * 4;
					auto img_index = (j * image.width + i) * 4;
					source_data[source_index] = image_data[img_index];
					source_data[source_index + 1] = image_data[img_index + 1];
					source_data[source_index + 2] = image_data[img_index + 2];
					source_data[source_index + 3] = image_data[img_index + 3];
				}
			}
			auto packets = PVRTC::encode_rgb_4bpp<std::uint8_t>(source_data, newWidth);
			auto stream = DataStreamView{};
			for (auto i : Range<std::uint64_t>(packets.size()))
			{
				stream.writeUint64(packets[i].PvrTcWord);
			}
			return stream.toBytes();
		}

		inline static auto rgb_pvrtc_4bpp_a8(
			const Image<int> &image) -> std::vector<unsigned char>
		{
			auto newWidth = image.width;
			auto newHeight = image.height;
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
			auto new_image = Image<int>(0, 0, newWidth, newWidth, std::vector<uint8_t>(pixel_area_rgba(newWidth, newWidth), 0x00));
			auto source_data = new_image.data();
			auto image_data = image.data();
			for (auto j : Range<int>(image.height))
			{
				for (auto i : Range<int>(image.width))
				{
					auto source_index = ((j + image.y) * new_image.width + (i + image.x)) * 4;
					auto img_index = (j * image.width + i) * 4;
					source_data[source_index] = image_data[img_index];
					source_data[source_index + 1] = image_data[img_index + 1];
					source_data[source_index + 2] = image_data[img_index + 2];
					source_data[source_index + 3] = image_data[img_index + 3];
				}
			}
			auto packets = PVRTC::encode_rgba_4bpp<std::uint8_t>(source_data, newWidth);
			auto stream = DataStreamView{};
			for (auto i : Range<int>(packets.size()))
			{
				stream.writeUint64(packets[i].PvrTcWord);
			}
			for (auto i : Range<int>(image.width * image.height))
			{
				stream.writeUint8(image_data[i * 4 + 3]);
			}
			return stream.toBytes();
		}

		inline static auto a_8(
			const Image<int> &image) -> std::vector<unsigned char>
		{
			auto stream = DataStreamView{};
			auto data = image.data();
			for (auto i = 0; i < image.area() * 4; i += 4)
			{
				stream.writeUint8(data[i + 3]);
			}
			return stream.toBytes();
		}

		inline static auto argb_1555(
			const Image<int> &image) -> std::vector<unsigned char>
		{
			auto stream = DataStreamView{};
			auto data = image.data();
			for (auto i = 0; i < image.area() * 4; i += 4)
			{
				stream.writeUint16(static_cast<unsigned int>(((data[i + 3] & 0x80) << 8) | (data[i + 2] >> 3) | ((data[i + 1] & 0xF8) << 2) | ((data[i] & 0xF8) << 7)));
			}
			return stream.toBytes();
		}

		inline static auto argb_4444(
			const Image<int> &image) -> std::vector<unsigned char>
		{
			auto stream = DataStreamView{};
			auto data = image.data();
			for (auto i = 0; i < image.area() * 4; i += 4)
			{
				stream.writeUint16(static_cast<unsigned int>((data[i + 2] >> 4) | (data[i + 1] & 0xF0) | ((data[i] & 0xF0) << 4) | ((data[i + 3] & 0xF0) << 8)));
			}
			return stream.toBytes();
		}

		inline static auto l_8(
			const Image<int> &image) -> std::vector<unsigned char>
		{
			auto stream = DataStreamView{};
			auto data = image.data();
			for (auto i = 0; i < image.area() * 4; i += 4)
			{
				stream.writeUint8(Encode::convert_luminance_from_rgb(data[i], data[i + 1], data[i + 2]));
			}
			return stream.toBytes();
		}

		inline static auto la_44(
			const Image<int> &image) -> std::vector<unsigned char>
		{
			auto stream = DataStreamView{};
			auto data = image.data();
			for (auto i = 0; i < image.area() * 4; i += 4)
			{
				stream.writeUint8(Encode::convert_luminance_from_rgba(data[i], data[i + 1], data[i + 2], data[i + 3]));
			}
			return stream.toBytes();
		}

		inline static auto la_88(
			const Image<int> &image) -> std::vector<unsigned char>
		{
			auto stream = DataStreamView{};
			auto data = image.data();
			for (auto i = 0; i < image.area() * 4; i += 4)
			{
				stream.writeUint16(static_cast<uint16_t>(Encode::convert_luminance_from_rgb(data[i], data[i + 1], data[i + 2], data[i + 3])));
			}
			return stream.toBytes();
		}
	};

}