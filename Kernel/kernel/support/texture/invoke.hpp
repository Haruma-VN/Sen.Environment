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
				const std::string &source,
				const std::string &destination,
				int width,
				int height,
				Format format
			) -> void
			{
				auto source_binary = FileSystem::readBinary<unsigned char>(source);
				switch(format){
					case Format::RGBA_8888: {
						ImageIO::write_png(destination, Decode::rgba(source_binary, width, height));
						break;
					}
					case Format::ARGB_8888: {
						ImageIO::write_png(destination, Decode::argb(source_binary, width, height));
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
				const std::string &source,
				const std::string &destination,
				Format format
			) -> void
			{
				auto source_image = ImageIO::read_png(source);
				auto result = std::vector<unsigned char>{};
				switch(format){
					case Format::RGBA_8888:{
						result = std::move(Encode::rgba(source_image.color()));
						break;
					}
					case Format::ARGB_8888:{
						result = std::move(Encode::argb(source_image.color()));
						break;
					}
				}
				FileSystem::writeBinary<unsigned char>(destination, result);
				return;
			}

			#pragma endregion

	};
}