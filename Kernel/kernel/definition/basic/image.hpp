#pragma once

#include "kernel/definition/assert.hpp"
#include "kernel/definition/library.hpp"
#include "kernel/definition/macro.hpp"

namespace Sen::Kernel::Definition {

	// Integral typename

	template <typename Type>
	concept Integral = std::is_integral_v<Type>;
	
	/**
	 * Dimension struct
	*/

	template <Integral T>

	struct Dimension {

		public:

			// width and height

			T width;

			T height;

			// constructor

			explicit Dimension(

			) = default;

			// constructor

			Dimension(
				T width,
				T height
			) : width(width), height(height)
			{

			}

			// destructor

			~Dimension(

			) = default;

			// get circumference

			auto circumference(

			) -> T
			{
				return thiz.width * 2 + thiz.height * 2;
			}
			
			// get area

			auto area(

			) -> T
			{
				return thiz.width * thiz.height;
			}
	};

	/**
	 * Rectangle Struct
	*/

	template <typename T> requires Integral<T>

	struct Rectangle : Dimension<T> {

		public:

			// position in image

			T x;
			T y;

			/**
			 * constructor
			*/

			Rectangle(

			) = default;

			/**
			 * destructor
			*/

			~Rectangle(

			) = default;

			/**
			 * constructor
			*/

			Rectangle(
				T x, 
				T y,
				T width,
				T height
			) : x(x), y(y), Dimension<T>(width, height)
			{

			}

			/**
			 * constructor
			*/

			Rectangle(
				const Dimension<T> &that,
				T x, 
				T y
			) : x(x), y(y), Dimension<T>(that.width, that.height)
			{

			}

			/**
			 * constructor
			*/

			Rectangle(
				const Rectangle &that
			) : x(that.x), y(that.y), Dimension<T>(that.width, that.height)
			{

			}

			/**
			 * get area
			*/

			auto area(

			) const -> T
			{
				return thiz.width * thiz.height;
			}

			/**
			 * get circumference
			*/

			auto circumference(

			) const -> T
			{
				return thiz.width * 2 + thiz.height * 2;
			}
	};

	/**
	 * Image color
	*/

	struct Color {

		public:

			// RGBA 
			
			// 0 - 1 - 2 - 3

			// red (i = 0)

			std::vector<unsigned char> red;

			// green (i = 1)

			std::vector<unsigned char> green;

			// blue (i = 2)

			std::vector<unsigned char> blue;

			// alpha (i = 3)

			std::vector<unsigned char> alpha;
			
			// destructor

			~Color(

			) = default;

			// constructor

			Color(

			) 
			{
				thiz.red = std::vector<unsigned char>();
				thiz.green = std::vector<unsigned char>();
				thiz.blue = std::vector<unsigned char>();
				thiz.alpha = std::vector<unsigned char>();
			}
	};

	/**
	 * Image struct
	*/

	template <typename T> requires Integral<T>
	struct Image : Rectangle<T> {

		private:

			// pixel data should not be accessible
			
			std::vector<unsigned char> _data;

		public:

			// easy accessible data

			T bit_depth;
			T color_type;
			T interlace_type;
			T channels;
			T rowbytes;

			/**
			 * constructor
			*/

			Image(
				T width, 
				T height, 
				T bit_depth, 
				T color_type, 
				T interlace_type, 
				T channels, 
				T rowbytes, 
				std::vector<unsigned char> data
			) : 
			Rectangle<T>(0, 0, width, height), 
			bit_depth(bit_depth),
			color_type(color_type), 
			interlace_type(interlace_type), 
			channels(channels), 
			rowbytes(rowbytes), 
			_data(std::move(data))
			{

			}

			/**
			 * constructor
			*/

			Image(
				const Image &that
			) : Image<T>(that.width, that.height, that.bit_depth, that.color_type, that.interlace_type, that.channels, that.rowbytes, that.data())
			{

			}

			/**
			 * get pixel data
			*/

			auto data(

			) const -> const std::vector<unsigned char> & 
			{
				return thiz._data;
			}

			/**
			 * set pixel data
			*/

			auto set_data(
				const std::vector<unsigned char> &data
			) -> void
			{
				thiz._data = std::move(data);
				return;
			}

			/**
			 *
			 * get color
			 * return: must be RGBA vector of Struct
			 */

			auto color(

			) -> Color
			{
				auto c = Color{};
				for (auto i = static_cast<size_t>(0); i < thiz._data.size(); i += 4)
				{
					c.red.push_back(thiz._data.at(i));
					c.green.push_back(thiz._data.at(i + 1));
					c.blue.push_back(thiz._data.at(i + 2));
					c.alpha.push_back(thiz._data.at(i + 3));
				}
				return c;
			}

			/**
			 * default constructor
			*/

			Image(
				T width, 
				T height, 
				std::vector<unsigned char> data
			) : Rectangle<T>(0, 0, width, height), _data(std::move(data)) 
			{
			}

			/**
			 * init blank
			*/

			Image(

			) = default;

			/**
			 * destructor
			*/

			~Image(

			) = default;

			/**
			 * get the image dimension
			*/

			auto dimension(

			) -> Dimension<T>
			{
				auto dz = Dimension<T>{};
				dz.width = thiz.width;
				dz.height = thiz.height;
				return dz;
			}

			/**
			 * composite image by copying pixel
			 * image: this struct
			 * rectangle: area to cut
			 * return: newly struct with other data 
			*/

			static auto composite(
				const Image<int>& image, 
				Rectangle<int> rectangle
			) -> Image<int> const
			{
				auto data = std::vector<unsigned char>{};
				data.reserve(rectangle.area() * 4);
				for (auto j : Range<int>(rectangle.y, rectangle.y + rectangle.height, 1)) {
					for (auto i : Range<int>(rectangle.x, rectangle.x + rectangle.width, 1)) {
						auto index = (j * image.width + i) * 4;
						data.insert(data.end(), &image.data()[index], &image.data()[index + 4]);
					}
				}
				auto result = Image<int>(rectangle.width, rectangle.height, data);
				return result;
			}

			/**
			 * Dimension: dimension (width and height)
			 * return: transparent image
			*/

			static auto transparent(
				Dimension<T> dimension
			) -> Image<T>
			{
				return Image<T>(dimension.width, dimension.height, std::vector<unsigned char>(dimension.area() * 4, 0x00));
			}

			/**
			 * Algorithm to join image - Reverse from split
			 * source: source image
			 * data: data to join
			 * return: the new source
			 */

			static auto join(
				Image<T> &source,
				const std::vector<Image<T>> &data
			) -> void
			{
				auto source_data = source.data();
				for (const auto& img : data) {
					if (!(img.width + img.x <= source.width && img.height + img.y <= source.height)) {
						throw std::runtime_error("Image does not fit within the source image");
					}
					for (auto j : Range<T>(img.height)) {
						for (auto i : Range<T>(img.width)) {
							auto source_index = ((j + img.y) * source.width + (i + img.x)) * 4;
							auto img_index = (j * img.width + i) * 4; 
							source_data[source_index] = img.data()[img_index];
							source_data[source_index + 1] = img.data()[img_index + 1];
							source_data[source_index + 2] = img.data()[img_index + 2];
							source_data[source_index + 3] = img.data()[img_index + 3];
						}
					}
				}
				source.set_data(source_data);
				return;
			}

			/**
			 * Resize image algorithm
			 * source: source image
			 * percent: the new image percent to resize
			 * return: the newly image
			*/

			static auto resize(
				const Image<T>& source,
				float percent
			) -> Image<T> const
			{
				auto new_width = static_cast<int>(ceil(source.width * percent));
				auto new_height = static_cast<int>(ceil(source.height * percent));
				auto resized_image_data = std::vector<unsigned char>(new_width * new_height * 4);
				for (auto j : Range<int>(new_height)) {
					for (auto i : Range<int>(new_width)) {
						auto old_i = static_cast<int>(i / percent);
						auto old_j = static_cast<int>(j / percent);
						auto old_index = (old_j * source.width + old_i) * 4;
						auto new_index = (j * new_width + i) * 4;
						std::copy(&source.data()[old_index], &source.data()[old_index + 4], &resized_image_data[new_index]);
					}
				}
				return Image<T>(new_width, new_height, resized_image_data);
			}

			/**
			 * Rotate image algorithm
			 * Image: current image
			 * angle: angle to rotate
			 * return: newly image after rotate
			*/

			static auto rotate(
				const Image<T>& image, 
				double angle
			) -> Image<T> const
			{
				auto new_width = static_cast<int>(std::abs(image.width * std::cos(angle)) + std::abs(image.height * std::sin(angle)));
				auto new_height = static_cast<int>(std::abs(image.height * std::cos(angle)) + std::abs(image.width * std::sin(angle)));
				auto data = std::vector<unsigned char>(new_width * new_height * 4, 0);
				auto center_x = static_cast<double>(image.width) / 2.0;
				auto center_y = static_cast<double>(image.height) / 2.0;
				for (auto j : Range<int>(new_height)) {
					for (auto i : Range<int>(new_width)) {
						auto old_i = static_cast<int>((i - new_width / 2.0) * std::cos(angle) + (j - new_height / 2.0) * std::sin(angle) + center_x);
						auto old_j = static_cast<int>((j - new_height / 2.0) * std::cos(angle) - (i - new_width / 2.0) * std::sin(angle) + center_y);
						if (old_i >= 0 && old_i < image.width && old_j >= 0 && old_j < image.height) {
							auto old_index = (old_j * image.width + old_i) * 4;
							auto new_index = (j * new_width + i) * 4; 
							std::copy(&image.data()[old_index], &image.data()[old_index + 4], &data[new_index]);
						}
					}
				}
				return Image<T>(new_width, new_height, data);
			}

			/**
			 * Scale image using avir.h
			 * source: image source
			 * percentage: percentage to upscale
			 * return: new image 
			*/

			static auto scale(
				const Image<int> & source,
				float percentage
			) -> Image<int> const
			{
				auto new_width = static_cast<int>(source.width * percentage);
				auto new_height = static_cast<int>(source.height * percentage);
				const auto area = (new_width * new_height * 4);
				auto data = new uint8_t[area];
				avir::CImageResizer<>{8}.resizeImage(
					source.data().data(), 
					static_cast<int>(source.width), 
					static_cast<int>(source.height), 
					0, 
					data, 
					static_cast<int>(new_width), 
					static_cast<int>(new_height), 
					4, 
					0.0, 
					nullptr
				);
				auto vec = std::vector<uint8_t>(data, data + area);
				delete[] data;
				data = nullptr;
				return Image<int>(new_width, new_height, vec);
			}
	};

	/**
	 * Rectangle with destination path
	*/

	template <typename T> requires Integral<T>

	struct RectangleFileIO : Rectangle<T> {
		
		public:

			// destination file
			std::string destination;

			// constructor

			RectangleFileIO(

			) = default;

			// constructor

			RectangleFileIO(
				T x,
				T y,
				T width,
				T height,
				const std::string &destination
			) : Rectangle<T>(x, y, width, height), destination(destination)
			{
			}

			/**
			 * constructor
			*/

			RectangleFileIO(
				const RectangleFileIO& that
			) : Rectangle<T>(that.x, that.y, that.width, that.height), destination(that.destination)
			{
			}

			// constructor

			RectangleFileIO(
				const Rectangle<T> &that,
				const std::string &destination
			) : Rectangle<T>(that), destination(destination)
			{
			}

			// destructor

			~RectangleFileIO(

			) = default;
	};

	/**
	 * File System Image
	 * In/Out Image struct
	*/


	struct ImageIO {

		public:

			/**
			 * libpng readpng adapation -> C++ implementation
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
				for (auto y : Range<int>(height)) {
					for (auto x : Range<int>(width * bit_depth * channels / 8)) {
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
			 * libpng write png adaptation
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
				for (auto y : Range<int>(data.height)) {
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

			/**
			 * Should use to split one image
			 * source: source file
			 * destination: destination file
			 * rectangle: the area to composite
			 * return: the new image
			*/

			static auto composite_png(
				const std::string & source,
				const std::string & destination,
				Rectangle<int> rectangle
			) -> void
			{
				ImageIO::write_png(destination, Image<int>::composite(ImageIO::read_png(source), rectangle));
				return;
			}

			/**
			 * Should be used to split images
			 * source: source file
			 * data: list of rectangle file
			 * return: the composite 
			*/

			static auto composite_pngs(
				const std::string & source,
				const std::vector<RectangleFileIO<int>> & data
			) -> void
			{
				for(auto &c : data) {
					ImageIO::composite_png(source, c.destination, c);
				}
				return;
			}

			/**
			 * TEST FUNCTION : THIS IS A TEST METHOD THAT SHOULD NOT BE USED
			 * destination: the destination of file to write
			 * width: image width
			 * height: image height
			 * pixel data: all filled with 0x00
			 * return: the written transparent png
			*/

			static auto transparent_png(
				const std::string & destination,
				int width,
				int height
			) -> void
			{
				ImageIO::write_png(destination, Image<int>::transparent(Dimension<int>(width, height)));
				return;
			}

			/**
			 * Should be used to join image
			 * destination: the file output destination
			 * dimension: output file dimension
			 * data: the image data
			 * return: the file output png
			 */

			static auto join_png(
				const std::string & destination,
				const Dimension<int> & dimension,
				const std::vector<Image<int>> &data
			) -> void
			{
				// create new transparent source
				auto source = Image<int>::transparent(dimension);
				// join to source
				Image<int>::join(source, data);
				// write png
				ImageIO::write_png(destination, source);
				return;
			}

			/**
			 * Resize image method
			 * @param source : image source
			 * @param destination : output file destination
			 * @param percent: percentage to resize
			 * @return the newly image
			 */

			static auto resize_png(
				const std::string & source,
				const std::string & destination,
				float percent
			) -> void
			{
				ImageIO::write_png(destination, Image<int>::resize(ImageIO::read_png(source), percent));
				return;
			}

			/**
			 * Rotate image
			 * @param source: image input source
			 * @param destination: image output destination
			 * @param angle: angle to rotate
			 * return: the newly image after rotate
			*/

			static auto rotate_png(
				const std::string & source,
				const std::string & destination,
				double angle
			) -> void
			{
				ImageIO::write_png(destination, Image<int>::rotate(ImageIO::read_png(source), angle));
				return;
			}

			/**
			 * Scale image
			 * @param source: source file
			 * @param destination: destination file
			 * @param percent: upscale percentage
			 * @return: written file to destination
			*/

			static auto scale_png(
				const std::string & source,
				const std::string & destination,
				float percent
			) -> void
			{
				ImageIO::write_png(destination, Image<int>::scale(ImageIO::read_png(source), percent));
				return;
			}

	};
}