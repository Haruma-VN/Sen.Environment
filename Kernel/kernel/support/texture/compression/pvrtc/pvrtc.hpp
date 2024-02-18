#pragma once

#include "kernel/support/texture/compression/pvrtc/packet.hpp"

namespace Sen::Kernel::Support::Texture::Compression::PVRTC
{

	inline static constexpr auto MORTON_TABLE = std::array<int, 256>{
		0x0000, 0x0001, 0x0004, 0x0005, 0x0010, 0x0011, 0x0014, 0x0015,
		0x0040, 0x0041, 0x0044, 0x0045, 0x0050, 0x0051, 0x0054, 0x0055,
		0x0100, 0x0101, 0x0104, 0x0105, 0x0110, 0x0111, 0x0114, 0x0115,
		0x0140, 0x0141, 0x0144, 0x0145, 0x0150, 0x0151, 0x0154, 0x0155,
		0x0400, 0x0401, 0x0404, 0x0405, 0x0410, 0x0411, 0x0414, 0x0415,
		0x0440, 0x0441, 0x0444, 0x0445, 0x0450, 0x0451, 0x0454, 0x0455,
		0x0500, 0x0501, 0x0504, 0x0505, 0x0510, 0x0511, 0x0514, 0x0515,
		0x0540, 0x0541, 0x0544, 0x0545, 0x0550, 0x0551, 0x0554, 0x0555,
		0x1000, 0x1001, 0x1004, 0x1005, 0x1010, 0x1011, 0x1014, 0x1015,
		0x1040, 0x1041, 0x1044, 0x1045, 0x1050, 0x1051, 0x1054, 0x1055,
		0x1100, 0x1101, 0x1104, 0x1105, 0x1110, 0x1111, 0x1114, 0x1115,
		0x1140, 0x1141, 0x1144, 0x1145, 0x1150, 0x1151, 0x1154, 0x1155,
		0x1400, 0x1401, 0x1404, 0x1405, 0x1410, 0x1411, 0x1414, 0x1415,
		0x1440, 0x1441, 0x1444, 0x1445, 0x1450, 0x1451, 0x1454, 0x1455,
		0x1500, 0x1501, 0x1504, 0x1505, 0x1510, 0x1511, 0x1514, 0x1515,
		0x1540, 0x1541, 0x1544, 0x1545, 0x1550, 0x1551, 0x1554, 0x1555,
		0x4000, 0x4001, 0x4004, 0x4005, 0x4010, 0x4011, 0x4014, 0x4015,
		0x4040, 0x4041, 0x4044, 0x4045, 0x4050, 0x4051, 0x4054, 0x4055,
		0x4100, 0x4101, 0x4104, 0x4105, 0x4110, 0x4111, 0x4114, 0x4115,
		0x4140, 0x4141, 0x4144, 0x4145, 0x4150, 0x4151, 0x4154, 0x4155,
		0x4400, 0x4401, 0x4404, 0x4405, 0x4410, 0x4411, 0x4414, 0x4415,
		0x4440, 0x4441, 0x4444, 0x4445, 0x4450, 0x4451, 0x4454, 0x4455,
		0x4500, 0x4501, 0x4504, 0x4505, 0x4510, 0x4511, 0x4514, 0x4515,
		0x4540, 0x4541, 0x4544, 0x4545, 0x4550, 0x4551, 0x4554, 0x4555,
		0x5000, 0x5001, 0x5004, 0x5005, 0x5010, 0x5011, 0x5014, 0x5015,
		0x5040, 0x5041, 0x5044, 0x5045, 0x5050, 0x5051, 0x5054, 0x5055,
		0x5100, 0x5101, 0x5104, 0x5105, 0x5110, 0x5111, 0x5114, 0x5115,
		0x5140, 0x5141, 0x5144, 0x5145, 0x5150, 0x5151, 0x5154, 0x5155,
		0x5400, 0x5401, 0x5404, 0x5405, 0x5410, 0x5411, 0x5414, 0x5415,
		0x5440, 0x5441, 0x5444, 0x5445, 0x5450, 0x5451, 0x5454, 0x5455,
		0x5500, 0x5501, 0x5504, 0x5505, 0x5510, 0x5511, 0x5514, 0x5515,
		0x5540, 0x5541, 0x5544, 0x5545, 0x5550, 0x5551, 0x5554, 0x5555};

	using Packet = PVRTC::Packet;

	inline static constexpr auto rotate_right(
		uint32_t value,
		int shift) noexcept -> uint32_t
	{
		if ((shift &= 31) == 0)
		{
			return value;
		}
		return static_cast<uint32_t>(value >> shift) | (value << (32 - shift));
	}

	inline static constexpr auto get_morton_number(
		int x,
		int y) noexcept -> int
	{
		return (MORTON_TABLE[x >> 8] << 17) | (MORTON_TABLE[y >> 8] << 16) | (MORTON_TABLE[x & 0xFF] << 1) | MORTON_TABLE[y & 0xFF];
	}

	inline static auto decode_4bpp(
		const std::vector<Packet> &packets,
		int width) -> std::vector<uint8_t>
	{
		auto blocks = width >> 2;
		auto blockMask = blocks - 1;
		auto result = std::vector<uint8_t>((width * width) * 4, 0x00);
		for (auto y : Range<int>(blocks))
		{
			for (auto x : Range<int>(blocks))
			{
				auto packet = packets[get_morton_number(x, y)];
				auto mod = packet.get_modulation_data();
				auto weights = Packet::WEIGHTS;
				auto weightindex = packet.get_use_punch_through_alpha() ? 16 : 0;
				auto factorfather = Packet::BILINEAR_FACTORS;
				auto factorindex = 0;
				for (auto py : Range<int>(4))
				{
					auto yOffset = (py < 2) ? -1 : 0;
					auto y0 = (y + yOffset) & blockMask;
					auto y1 = (y0 + 1) & blockMask;
					for (auto px : Range<int>(4))
					{
						auto factor = factorfather[factorindex];
						auto xOffset = (px < 2) ? -1 : 0;
						auto x0 = (x + xOffset) & blockMask;
						auto x1 = (x0 + 1) & blockMask;
						auto p0 = packets[get_morton_number(x0, y0)];
						auto p1 = packets[get_morton_number(x1, y0)];
						auto p2 = packets[get_morton_number(x0, y1)];
						auto p3 = packets[get_morton_number(x1, y1)];
						auto ca = p0.get_color_alpha_color_rgba() * factor[0] + p1.get_color_alpha_color_rgba() * factor[1] + p2.get_color_alpha_color_rgba() * factor[2] + p3.get_color_alpha_color_rgba() * factor[3];
						auto cb = p0.get_color_blue_color_rgba() * factor[0] + p1.get_color_blue_color_rgba() * factor[1] + p2.get_color_blue_color_rgba() * factor[2] + p3.get_color_blue_color_rgba() * factor[3];
						auto index = weightindex + (((int)mod & 0b11) << 2);
						auto current_index = ((py + (y << 2)) * width + px + (x << 2)) * 4;
						result[current_index] = static_cast<uint8_t>(((ca.red) * weights[index] + cb.red * weights[index + 1]) >> 7);
						result[current_index + 1] = static_cast<uint8_t>(((ca.green * weights[index] + cb.green * weights[index + 1]) >> 7));
						result[current_index + 2] = static_cast<uint8_t>(((ca.blue * weights[index] + cb.blue * weights[index + 1]) >> 7));
						result[current_index + 3] = static_cast<uint8_t>(((ca.alpha * weights[index + 2] + cb.alpha * weights[index + 3]) >> 7));
						mod >>= 2;
						factorindex++;
					}
				}
			}
		}
		return result;
	}

	inline static auto calculate_bounding_box(
		const std::vector<uint8_t> &colors,
		int width,
		int blockX,
		int blockY,
		Rgba32 &min,
		Rgba32 &max) -> void
	{
		uint8_t maxr = 0, maxg = 0, maxb = 0, maxa = 0;
		uint8_t minr = 255, ming = 255, minb = 255, mina = 255;
		auto begin_index = (blockY << 2) * width + (blockX << 2);
		for (auto i : Range<int>(4))
		{
			auto nindex = begin_index + i * width;
			for (auto j : Range<int>(4))
			{
				auto index = nindex + j;
				uint8_t color_byte = 0;
				color_byte = colors[index * 4];
				if (color_byte > maxr)
					maxr = color_byte;
				if (color_byte < minr)
					minr = color_byte;
				color_byte = colors[index * 4 + 1];
				if (color_byte > maxg)
					maxg = color_byte;
				if (color_byte < ming)
					ming = color_byte;
				color_byte = colors[index * 4 + 2];
				if (color_byte > maxb)
					maxb = color_byte;
				if (color_byte < minb)
					minb = color_byte;
				color_byte = colors[index * 4 + 3];
				if (color_byte > maxa)
					maxa = color_byte;
				if (color_byte < mina)
					mina = color_byte;
			}
		}
		min = Rgba32(minr, ming, minb, mina);
		max = Rgba32(maxr, maxg, maxb, maxa);
		return;
	}

	inline static auto encode_rgba_4bpp(
		const std::vector<uint8_t> &color,
		int width) -> std::vector<PVRTC::Packet>
	{
		auto blocks = width >> 2;
		auto blockMask = blocks - 1;
		auto result = std::vector<PVRTC::Packet>{};
		result.resize(((width * width) >> 4));
		Rgba32 min_color;
		Rgba32 max_color;
		int index = 0;
		for (auto y : Range<int>(blocks))
		{
			for (auto x : Range<int>(blocks))
			{
				calculate_bounding_box(color, width, x, y, min_color, max_color);
				
				auto packet = PVRTC::Packet{};
				packet.set_use_punch_through_alpha(false);
				packet.set_color_alpha_color_rgba(min_color);
				packet.set_color_blue_color_rgba(max_color);
				result[get_morton_number(x, y)] = packet;
			}
		}
		for (auto y : Range<int>(blocks))
		{
			for (auto x : Range<int>(blocks))
			{
				auto factorfather = PVRTC::Packet::BILINEAR_FACTORS;
				auto factor_index = 0;
				auto data_index = (y << 2) * width + (x << 2);
				uint32_t modulation_data = 0;
				for (auto py : Range<int>(4))
				{
					auto y_pos = (py < 2) ? -1 : 0;
					auto y0 = (y + y_pos) & blockMask;
					auto y1 = (y0 + 1) & blockMask;
					for (auto px : Range<int>(4))
					{
						auto factor = factorfather[factor_index];
						auto x_pos = (px < 2) ? -1 : 0;
						auto x0 = (x + x_pos) & blockMask;
						auto x1 = (x0 + 1) & blockMask;
						auto p0 = result[get_morton_number(x0, y0)];
						auto p1 = result[get_morton_number(x1, y0)];
						auto p2 = result[get_morton_number(x0, y1)];
						auto p3 = result[get_morton_number(x1, y1)];
						auto ca = p0.get_color_alpha_color_rgba() * factor[0] + p1.get_color_alpha_color_rgba() * factor[1] + p2.get_color_alpha_color_rgba() * factor[2] + p3.get_color_alpha_color_rgba() * factor[3];
						auto cb = p0.get_color_blue_color_rgba() * factor[0] + p1.get_color_blue_color_rgba() * factor[1] + p2.get_color_blue_color_rgba() * factor[2] + p3.get_color_blue_color_rgba() * factor[3];
						auto pixel_index = (data_index + py * width + px) * 4;
						auto d = cb - ca;
						auto p = ColorRGBA(
							color[pixel_index] << 4,
							color[pixel_index + 1] << 4,
							color[pixel_index + 2] << 4,
							color[pixel_index + 3] << 4);
						auto v = p - ca;
						int projection = (v % d) << 4;
						int length_squared = d % d;
						if (projection > 3 * length_squared)
							modulation_data++;
						if (projection > 8 * length_squared)
							modulation_data++;
						if (projection > 13 * length_squared)
							modulation_data++;
						modulation_data = rotate_right(modulation_data, 2);
						factor_index++;
					}
				}
				index++;
				result[get_morton_number(x, y)].set_modulation_data(modulation_data);
			}
		}
		return result;
	}

	inline static auto encode_rgb_4bpp(
		const std::vector<uint8_t> &color,
		int width) -> std::vector<PVRTC::Packet>
	{
		auto blocks = width >> 2;
		auto blockMask = blocks - 1;
		auto result = std::vector<PVRTC::Packet>{};
		result.resize(((width * width) >> 4));
		Rgba32 min_color;
		Rgba32 max_color;
		int index = 0;
		for (auto y : Range<int>(blocks))
		{
			for (auto x : Range<int>(blocks))
			{
				calculate_bounding_box(color, width, x, y, min_color, max_color);
				
				auto packet = PVRTC::Packet{};
				packet.set_use_punch_through_alpha(false);
				packet.set_color_alpha_color_rgb(min_color);
				packet.set_color_blue_color_rgb(max_color);
				result[get_morton_number(x, y)] = packet;
			}
		}
		for (auto y : Range<int>(blocks))
		{
			for (auto x : Range<int>(blocks))
			{
				auto factorfather = PVRTC::Packet::BILINEAR_FACTORS;
				auto factor_index = 0;
				auto data_index = (y << 2) * width + (x << 2);
				uint32_t modulation_data = 0;
				for (auto py : Range<int>(4))
				{
					auto y_pos = (py < 2) ? -1 : 0;
					auto y0 = (y + y_pos) & blockMask;
					auto y1 = (y0 + 1) & blockMask;
					for (auto px : Range<int>(4))
					{
						auto factor = factorfather[factor_index];
						auto x_pos = (px < 2) ? -1 : 0;
						auto x0 = (x + x_pos) & blockMask;
						auto x1 = (x0 + 1) & blockMask;
						auto p0 = result[get_morton_number(x0, y0)];
						auto p1 = result[get_morton_number(x1, y0)];
						auto p2 = result[get_morton_number(x0, y1)];
						auto p3 = result[get_morton_number(x1, y1)];
						auto ca = p0.get_color_alpha_color_rgb() * factor[0] + p1.get_color_alpha_color_rgb() * factor[1] + p2.get_color_alpha_color_rgb() * factor[2] + p3.get_color_alpha_color_rgb() * factor[3];
						auto cb = p0.get_color_blue_color_rgb() * factor[0] + p1.get_color_blue_color_rgb() * factor[1] + p2.get_color_blue_color_rgb() * factor[2] + p3.get_color_blue_color_rgb() * factor[3];
						auto pixel_index = (data_index + py * width + px) * 4;
						auto d = cb - ca;
						auto p = ColorRGB(
							color[pixel_index] << 4,
							color[pixel_index + 1] << 4,
							color[pixel_index + 2] << 4
							);
						
						auto v = p - ca;
						int projection = (v % d) << 4;
						int length_squared = d % d;
						if (projection > 3 * length_squared)
							modulation_data++;
						if (projection > 8 * length_squared)
							modulation_data++;
						if (projection > 13 * length_squared)
							modulation_data++;
						modulation_data = rotate_right(modulation_data, 2);
						factor_index++;
					}
				}
				index++;
				result[get_morton_number(x, y)].set_modulation_data(modulation_data);
			}
		}
		return result;
	}

}