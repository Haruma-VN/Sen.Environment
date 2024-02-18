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
		A_8,
		ARGB_1555,
		ARGB_4444,
		RGB_ETC1,
		L_8,
		LA_44,
		LA_88,
		RGB_PVRTC_4BPP,
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

			inline static auto decode_fs(
				std::string_view source,
				std::string_view destination,
				int width,
				int height,
				Format format
			) -> void
			{
				auto source_binary = FileSystem::read_binary<unsigned char>(source);
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
					case Format::RGB_ETC1:{
						ImageIO::write_png(destination, Decode::rgb_etc1(source_binary, width, height));
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
					case Format::RGB_PVRTC_4BPP:{
						ImageIO::write_png(destination, Decode::rgba_pvrtc_4bpp(source_binary, width, height));
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
					case Format::A_8:{
						ImageIO::write_png(destination, Decode::a_8(source_binary, width, height));
						break;
					}
					case Format::ARGB_1555:{
						ImageIO::write_png(destination, Decode::argb_1555(source_binary, width, height));
						break;
					}
					case Format::ARGB_4444:{
						ImageIO::write_png(destination, Decode::argb_4444(source_binary, width, height));
						break;
					}
					case Format::L_8:{
						ImageIO::write_png(destination, Decode::l_8(source_binary, width, height));
						break;
					}
					case Format::LA_44:{
						ImageIO::write_png(destination, Decode::la_44(source_binary, width, height));
						break;
					}
					case Format::LA_88:{
						ImageIO::write_png(destination, Decode::la_88(source_binary, width, height));
						break;
					}
					default:{
						throw Exception("Format is not supported", std::source_location::current(), "decode_fs");
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

			inline static auto encode_fs(
				std::string_view source,
				std::string_view destination,
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
					case Format::RGB_ETC1:{
						result = std::move(Encode::rgb_etc1(source_image));
						break;
					}
					case Format::RGB_ETC1_A_8:{
						result = std::move(Encode::rgb_etc1_a_8(source_image));
						break;
					}
					case Format::RGB_ETC1_A_PALETTE:{
						result = std::move(Encode::rgb_etc1_a_palette(source_image));
						break;
					}
					case Format::RGB_PVRTC_4BPP:{
						result = std::move(Encode::rgb_pvrtc_4bpp(source_image));
						break;
					}
					case Format::RGBA_PVRTC_4BPP:{
						result = std::move(Encode::rgba_pvrtc_4bpp(source_image));
						break;
					}
					case Format::RGB_PVRTC_4BPP_A_8:{
						result = std::move(Encode::rgb_pvrtc_4bpp_a8(source_image));
						break;
					}
					case Format::A_8: {
						result = std::move(Encode::a_8(source_image));
						break;
					}
					case Format::ARGB_1555: {
						result = std::move(Encode::argb_1555(source_image));
						break;
					}
					case Format::ARGB_4444: {
						result = std::move(Encode::argb_4444(source_image));
						break;
					}
					case Format::L_8: {
						result = std::move(Encode::l_8(source_image));
						break;
					}
					case Format::LA_44: {
						result = std::move(Encode::la_44(source_image));
						break;
					}
					case Format::LA_88: {
						result = std::move(Encode::la_88(source_image));
						break;
					}
					default:{
						throw Exception("Format is not supported", std::source_location::current(), "encode_fs");
					}
				}
				FileSystem::write_binary<unsigned char>(destination, result);
				return;
			}

			#pragma endregion

	};
}