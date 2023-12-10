#pragma once

#include "kernel/support/texture/decode.hpp"
#include "kernel/support/texture/encode.hpp"

namespace Sen::Kernel::Support::Texture {

	/**
	 * Texture format supported by Kernel
	*/

	enum Format
	{
		RGBA_8888 = 1,
		ARGB_8888,
		RGBA_4444,
		RGB_565,
		RGBA_5551,
		RGBA_4444_TILED,
		RGB_565_TILED,
		RGBA_5551_TILED,
		RGB_ETC1_A_8,
		RGB_ETC1_A_PALETTE,
		RGBA_PVRTC_4BPP,
		RGB_PVRTC_4BPP_A_8,
	};

	/**
	 * Invoke method class
	*/

	class InvokeMethod {

		public:

			#pragma region decode

			/**
			 * source: the input file path
			 * destination: the output file path
			 * width: image width
			 * height: image height
			 * format: texture format
			 * return: decoded image
			*/

			static auto decode_fs(
				const std::string & source,
				const std::string & destination,
				int width,
				int height,
				Format format
			) -> void
			{
				auto source_binary = FileSystem::readBinary<unsigned char>(source);
				switch(format){
					case Format::RGBA_8888: {
						ImageIO::write_png(destination, Decode::rgba_8888(source_binary, width, height));
						break;
					}
					case Format::ARGB_8888: {
						ImageIO::write_png(destination, Decode::argb_8888(source_binary, width, height));
						break;
					}
					case Format::RGBA_4444:{
						ImageIO::write_png(destination, Decode::rgba_4444(source_binary, width, height));
						break;
					}
					case Format::RGB_565:{
						ImageIO::write_png(destination, Decode::rgb_565(source_binary, width, height));
						break;
					}
					case Format::RGBA_5551:{
						ImageIO::write_png(destination, Decode::rgba_5551(source_binary, width, height));
						break;
					}
					case Format::RGBA_4444_TILED:{
						ImageIO::write_png(destination, Decode::rgba_4444_tiled(source_binary, width, height));
						break;
					}
					case Format::RGB_565_TILED:{
						ImageIO::write_png(destination, Decode::rgb_565_tiled(source_binary, width, height));
						break;
					}
					case Format::RGBA_5551_TILED:{
						ImageIO::write_png(destination, Decode::rgba_5551_tiled(source_binary, width, height));
						break;
					}
					case Format::RGB_ETC1_A_8:{
						ImageIO::write_png(destination, Decode::rgb_etc1_a_8(source_binary, width, height));
						break;
					}
					case Format::RGB_ETC1_A_PALETTE:{
						ImageIO::write_png(destination, Decode::rgb_etc1_a_palette(source_binary, width, height));
						break;
					}
					case Format::RGBA_PVRTC_4BPP:{
						ImageIO::write_png(destination, Decode::rgba_pvrtc_4bpp(source_binary, width, height));
						break;
					}
					case Format::RGB_PVRTC_4BPP_A_8:{
						ImageIO::write_png(destination, Decode::rgb_pvrtc_4bpp_a_8(source_binary, width, height));
						break;
					}
					default:{
						throw std::runtime_error("Format is not supported");
					}
				}
				return;
			}

			#pragma endregion


			#pragma region encode

			/**
			 * source: the input file
			 * destination: the output file
			 * format: texture format
			 * return: encoded file
			*/

			static auto encode_fs(
				const std::string & source,
				const std::string & destination,
				Format format
			) -> void
			{
				auto source_image = ImageIO::read_png(source);
				auto result = std::vector<unsigned char>{};
				switch(format){
					case Format::RGBA_8888:{
						result = std::move(Encode::rgba(source_image));
						break;
					}
					case Format::ARGB_8888:{
						result = std::move(Encode::argb(source_image));
						break;
					}
					case Format::RGBA_4444:{
						result = std::move(Encode::rgba_4444(source_image));
						break;
					}
					case Format::RGB_565:{
						result = std::move(Encode::rgb_565(source_image));
						break;
					}
					case Format::RGBA_5551:{
						result = std::move(Encode::rgba_5551(source_image));
						break;
					}
					case Format::RGBA_4444_TILED:{
						result = std::move(Encode::rgba_4444_tiled(source_image));
						break;
					}
					case Format::RGB_565_TILED:{
						result = std::move(Encode::rgb_565_tiled(source_image));
						break;
					}
					case Format::RGBA_5551_TILED:{
						result = std::move(Encode::rgba_5551_tiled(source_image));
						break;
					}
					case Format::RGB_ETC1_A_8:{
						result = std::move(Encode::rgba(source_image));
						break;
					}
					case Format::RGB_ETC1_A_PALETTE:{
						result = std::move(Encode::rgba(source_image));
						break;
					}
					case Format::RGBA_PVRTC_4BPP:{
						result = std::move(Encode::rgba(source_image));
						break;
					}
					case Format::RGB_PVRTC_4BPP_A_8:{
						result = std::move(Encode::rgba(source_image));
						break;
					}
					default:{
						throw std::runtime_error("Format is not supported");
					}
				}
				FileSystem::writeBinary<unsigned char>(destination, result);
				return;
			}

			#pragma endregion

	};
}