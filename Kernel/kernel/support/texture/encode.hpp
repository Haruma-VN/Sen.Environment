#pragma once

#include "kernel/definition/utility.hpp"
#include "kernel/support/texture/common.hpp"

namespace Sen::Kernel::Support::Texture {

	// use color

	using namespace Sen::Kernel::Definition;

	/**
	 * Encode class
	*/

	class Encode {
		
		public:

			/**
			 * color: the extracted color
			 * return: the encoded vector
			*/

			static auto rgba(
				Image<int> & image
			) -> std::vector<unsigned char>
			{
				auto color = image.color();
				auto result = std::vector<unsigned char>{};
				result.reserve(color.red.size() + color.green.size() + color.blue.size() + color.alpha.size());
				for (auto i : Range<size_t>(color.red.size())){
					result.push_back(color.red.at(i));
					result.push_back(color.green.at(i));
					result.push_back(color.blue.at(i));
					result.push_back(color.alpha.at(i));
				}
				return result;
			}

			/**
			 * color: the extracted color
			 * return: the encoded vector
			*/

			static auto argb(
				Image<int> & image
			) -> std::vector<unsigned char>
			{
				auto color = image.color();
				auto result = std::vector<unsigned char>{};
				result.reserve(color.red.size() + color.green.size() + color.blue.size() + color.alpha.size());
				for (auto i : Range<size_t>(color.red.size())){
					result.push_back(color.alpha.at(i));
					result.push_back(color.red.at(i));
					result.push_back(color.green.at(i));
					result.push_back(color.blue.at(i));
				}
				return result;
			}

			static auto rgba_4444(
				Image<int> & image
			) -> std::vector<unsigned char>
			{
				auto sen = Buffer::Vector{};
				auto data = image.data();
				for (auto y : Range<int>(image.height)) {
					for (auto x : Range<int>(image.width)) {
						auto index = set_pixel(x, y, image.width);
						auto color = (static_cast<int>(data[index + 3]) >> 4 | (static_cast<int>(data[index + 2]) & 0xF0) | ((static_cast<int>(data[index + 1]) & 0xF0) << 4) |
							((static_cast<int>(data[index]) & 0xF0) << 8));
						sen.writeUint16LE(color);
					}
				}
				return sen.get();
			}

			static auto rgb_565(
				Image<int> & image
			) -> std::vector<unsigned char>
			{
				auto sen = Buffer::Vector{};
				auto data = image.data();
				for (auto y : Range<int>(image.height)) {
					for (auto x : Range<int>(image.width)) {
						auto index = set_pixel(x, y, image.width);
						auto color = (static_cast<int>(data[index + 2]) >> 3 | ((static_cast<int>(data[index + 1]) & 0xFC) << 3) | ((static_cast<int>(data[index]) & 0xF8) << 8));
						sen.writeUint16LE(color);
					}
				}
				return sen.get();
			}

			static auto rgba_5551(
				Image<int> & image
			) -> std::vector<unsigned char>
			{
				auto sen = Buffer::Vector{};
				auto data = image.data();
				for (auto y : Range<int>(image.height)) {
					for (auto x : Range<int>(image.width)) {
						auto index = set_pixel(x, y, image.width);
						auto color = (((static_cast<int>(data[index + 3]) & 0x80) >> 7) |
						((static_cast<int>(data[index + 2]) & 0xF8) >> 2) |
						((static_cast<int>(data[index + 1]) & 0xF8) << 3) |
						((static_cast<int>(data[index + 0]) & 0xF8) << 8));
						sen.writeUint16LE(color);
					}
				}
				return sen.get();
			}

			static auto rgba_4444_tiled(
				Image<int> & image
			) -> std::vector<unsigned char>
			{
				auto sen = Buffer::Vector{};
				auto data = image.data();
				for (auto y : Range<int>(0, image.height, 32)) {
					for (auto x : Range<int>(0, image.width, 32)) {
						for (auto j : Range<int>(32)) {
							for (auto k : Range<int>(32)) {
								if ((y + j) < image.height and (x + k) < image.width) {
									auto index = set_pixel(x + k, y + j, image.width);
									auto color = (static_cast<int>(data[index + 3]) >> 4 |
										static_cast<int>(data[index + 2]) & 0xF0 |
										((static_cast<int>(data[index + 1]) & 0xF0) << 4) |
										((static_cast<int>(data[index + 0]) & 0xF0) << 8));
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

			static auto rgb_565_tiled(
				Image<int> & image
			) -> std::vector<unsigned char>
			{
				auto sen = Buffer::Vector{};
				auto data = image.data();
				for (auto y : Range<int>(0, image.height, 32)) {
					for (auto x : Range<int>(0, image.width, 32)) {
						for (auto j : Range<int>(32)) {
							for (auto k : Range<int>(32)) {
								if ((y + j) < image.height and (x + k) < image.width) {
									auto index = set_pixel(x + k, y + j, image.width);
									auto color = ((static_cast<int>(data[index + 2]) & 0xF8 >> 3) |
												((static_cast<int>(data[index + 1]) & 0xFC) << 3) |
												((static_cast<int>(data[index + 0]) & 0xF8) << 8));
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

			static auto rgba_5551_tiled(
				Image<int> & image
			) -> std::vector<unsigned char>
			{
				auto sen = Buffer::Vector{};
				auto data = image.data();
				for (auto y : Range<int>(0, image.height, 32)) {
					for (auto x : Range<int>(0, image.width, 32)) {
						for (auto j : Range<int>(32)) {
							for (auto k : Range<int>(32)) {
								if ((y + j) < image.height and (x + k) < image.width) {
									auto index = set_pixel(x + k, y + j, image.width);
									auto color = (((static_cast<int>(data[index + 3]) & 0x80) >> 7) |
												(static_cast<int>(data[index + 2]) & 0xF8 >> 2) |
												((static_cast<int>(data[index + 1]) & 0xF8) << 3) |
												((static_cast<int>(data[index + 0]) & 0xF8) << 8));
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
	};
	
}