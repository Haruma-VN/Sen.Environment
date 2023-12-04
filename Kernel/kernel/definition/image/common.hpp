#pragma once

#include "kernel/definition/assert.hpp"
#include "kernel/definition/library.hpp"
#include "kernel/definition/macro.hpp"

namespace Sen::Kernel::Definition
{
	
	/**
	 * Dimension struct
	*/

	template <typename T>
	struct Dimension{
		public:
			T width;
			T height;
			Dimension(

			)
			{

			}

			Dimension(
				T width,
				T height
			) : width(width), height(height)
			{

			}

			~Dimension(

				) = default;
	};

	/**
	 * Image struct
	*/
	
	template <typename T>
	struct Image {

		private:
			
			std::vector<unsigned char> _data;

		public:
			T width;
			T height;
			T bit_depth;
			T color_type;
			T interlace_type;
			T channels;
			T rowbytes;

			Image(
				T width, 
				T height, 
				T bit_depth, 
				T color_type, 
				T interlace_type, 
				T channels, 
				T rowbytes, 
				std::vector<unsigned char>
			) : 
			width(width), 
			height(height), 
			bit_depth(bit_depth),
			color_type(color_type), 
			interlace_type(interlace_type), 
			channels(channels), 
			rowbytes(rowbytes), 
			_data(std::move(data))
			{

			}

			auto data(

			) const -> const std::vector<unsigned char> & 
			{
				return thiz._data;
			}

			auto set_data(
				const std::vector<unsigned char> &data
			) -> void
			{
				thiz._data = std::move(data);
				return;
			}

			Image(

			) = default;

			~Image(

			) = default;

			auto dimension(

			) -> Dimension<T>
			{
				auto dz = Dimension<T>{};
				dz.width = thiz.width;
				dz.height = thiz.height;
				return dz;
			}
	};

	struct ImageIO {

		public:

			/**
			 * file path: provide file path to read
			 * return: image data
			*/
			
			static auto read_png(
				const string &filePath
			) -> Image<int>
			{
				auto *fp = fopen(filePath.c_str(), "rb");
				if(!fp){
					throw std::runtime_error(fmt::format("Open png failed: {}", filePath));
				}
				auto png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
				if(!png_ptr){
					fclose(fp);
					throw std::runtime_error(fmt::format("PNG Pointer initialize failed: {}", filePath));
				}
				auto info_ptr = png_create_info_struct(png_ptr);  
				if(!info_ptr){
					png_destroy_read_struct(&png_ptr, NULL, NULL);
					fclose(fp);
					throw std::runtime_error(fmt::format("Info Pointer initialize failed: {}", filePath));
				}
				if (setjmp(png_jmpbuf(png_ptr))) {
					png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
					fclose(fp);
					throw std::runtime_error("unknown error");
				}
				png_init_io(png_ptr, fp);
				png_read_png(png_ptr, info_ptr, PNG_TRANSFORM_IDENTITY, NULL);
				auto row_pointers = png_get_rows(png_ptr, info_ptr);
				auto data = std::vector<unsigned char>();
				auto width = static_cast<int>(png_get_image_width(png_ptr, info_ptr));
				auto height = static_cast<int>(png_get_image_height(png_ptr, info_ptr));
				auto bit_depth = static_cast<int>(png_get_bit_depth(png_ptr, info_ptr));
				auto channels = static_cast<int>(png_get_channels(png_ptr, info_ptr));
				for (auto y = 0; y < height; ++y) {
					for (auto x = 0; x < width * bit_depth * channels / 8; ++x) {
						data.push_back(row_pointers[y][x]);
					}
				}
				auto image = Image<int>{};
				image.width = width,
				image.height = height,
				image.bit_depth = bit_depth,
				image.color_type = static_cast<int>(png_get_color_type(png_ptr, info_ptr)),
				image.interlace_type = static_cast<int>(png_get_interlace_type(png_ptr, info_ptr)),
				image.channels = channels,
				image.rowbytes = static_cast<int>(png_get_rowbytes(png_ptr, info_ptr)),
				image.set_data(data);
				png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
				fclose(fp);
				return image;
			}

			/**
			 * file path: output path to write
			 * data: the image data
			 * return: written image
			*/

			static auto write_png(
				const std::string &filepath, 
				const Image<int> &data
			) -> void
			{
				auto *fp = fopen(filepath.c_str(), "wb");
				if(!fp){
					throw std::runtime_error(fmt::format("Open png failed: {}", filepath));
				}
				auto png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
				if(!png_ptr){
					fclose(fp);
					throw std::runtime_error(fmt::format("PNG Pointer initialize failed: {}", filepath));
				}
				auto info_ptr = png_create_info_struct(png_ptr);
				if (!info_ptr) {
					png_destroy_write_struct(&png_ptr, NULL);
					fclose(fp);
					throw std::runtime_error(fmt::format("Info Pointer initialize failed: {}", filepath));
				}
				#define PNG_WRITE_SETJMP(png_ptr, info_ptr, fp) \
				if (setjmp(png_jmpbuf(png_ptr)))  \
				{ \
					png_destroy_write_struct(&png_ptr, &info_ptr);   \
					fclose(fp);     \
					throw std::runtime_error("unknown error"); \
				}
				PNG_WRITE_SETJMP(png_ptr, info_ptr, fp);
				png_init_io(png_ptr, fp);
				png_set_IHDR(
					png_ptr,
					info_ptr,
					data.width,
					data.height,
					8,
					PNG_COLOR_TYPE_RGB_ALPHA,
					PNG_INTERLACE_NONE,
					PNG_COMPRESSION_TYPE_DEFAULT, 
					PNG_FILTER_TYPE_DEFAULT
				);
				auto row_pointers = std::vector<png_bytep>(data.height);
				for (auto y = 0; y < data.height; ++y) {
					row_pointers[y] = const_cast<unsigned char*>(&data.data()[y * data.width * 4]);
				}
				png_write_info(png_ptr, info_ptr);
				png_write_image(png_ptr, row_pointers.data());
				png_write_end(png_ptr, NULL);
				png_destroy_write_struct(&png_ptr, &info_ptr);
				fclose(fp);
				png_write_end(png_ptr, NULL);
				png_destroy_write_struct(&png_ptr, &info_ptr);
				fclose(fp);
				return;
			}

	};
}