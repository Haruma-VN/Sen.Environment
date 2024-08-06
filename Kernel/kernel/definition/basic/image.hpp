#pragma once

#include "kernel/definition/assert.hpp"
#include "kernel/definition/library.hpp"
#include "kernel/definition/macro.hpp"

namespace Sen::Kernel::Definition {

	// Integral typename

	template <typename Type>
	concept Integral = std::is_integral_v<Type>;

	inline static auto constexpr IsCategoryOfInteger = true;
	
	/**
	 * Dimension struct
	*/

	template <typename T> requires IsCategoryOfInteger and Integral<T>

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

			inline auto circumference(

			) const noexcept -> T
			{
				return (thiz.width + thiz.height) * 2;
			}
			
			// get area

			inline auto area(

			) const noexcept -> T
			{
				return thiz.width * thiz.height;
			}
	};

	/**
	 * Rectangle Struct
	*/

	template <typename T> requires IsCategoryOfInteger and Integral<T>

	struct Rectangle : public Dimension<T> {

		public:

			// position in image

			T x;
			T y;

			/**
			 * constructor
			*/

			constexpr Rectangle(

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

			Rectangle(
				Rectangle&& that
			) : x(that.x), y(that.y), Dimension<T>(that.width, that.height)
			{

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

			explicit constexpr Color(

			) = default;
	};

	/**
	 * Image struct
	*/

	template <typename T> requires IsCategoryOfInteger && Integral<T>
	struct Image : public Rectangle<T> {

		private:

			// pixel data should not be accessible
			
			std::vector<unsigned char> mutable _data;

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

			explicit constexpr Image(
				T width, 
				T height, 
				T bit_depth, 
				T color_type, 
				T interlace_type, 
				T channels, 
				T rowbytes, 
				const std::vector<unsigned char> & data
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

			explicit constexpr Image(
				const Image &that
			) : Image<T>(that.width, that.height, that.bit_depth, that.color_type, that.interlace_type, that.channels, that.rowbytes, that.data())
			{

			}

			Image(
				Image&& that
			) noexcept
				: Rectangle<T>(std::move(that)),
				bit_depth(std::move(that.bit_depth)),
				color_type(std::move(that.color_type)),
				interlace_type(std::move(that.interlace_type)),
				channels(std::move(that.channels)),
				rowbytes(std::move(that.rowbytes)),
				_data(std::move(that._data))
			{

			}

			Image& operator=(
				Image&& that
			) noexcept
			{
				if (this != &that) {
					Rectangle<T>::operator=(std::move(that));
					bit_depth = std::move(that.bit_depth);
					color_type = std::move(that.color_type);
					interlace_type = std::move(that.interlace_type);
					channels = std::move(that.channels);
					rowbytes = std::move(that.rowbytes);
					_data = std::move(that._data);
				}
				return *this;
			}

			/**
			 * get pixel data
			*/

			inline auto data(

			) const -> const std::vector<unsigned char> & 
			{
				return thiz._data;
			}

			/**
			 * set pixel data
			*/

			inline auto set_data(
				const std::vector<unsigned char> & data
			) const -> void
			{
				thiz._data = std::move(data);
				return;
			}

			/**
			 *
			 * get color
			 * return: must be RGBA vector of Struct
			 */

			inline auto color(

			) const -> Color
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
				T x,
				T y,
				T width,
				T height,
				const std::vector<unsigned char>& data
			) : Rectangle<T>(x, y, width, height), _data(std::move(data))
			{
			}

			/**
			 * init blank
			*/

			explicit Image(

			) = default;

			/**
			 * destructor
			*/

			~Image(

			) = default;

			using ZList = std::vector<uint8_t>;

			/**
			 * get the image dimension
			*/

			inline auto dimension(

			) const -> Dimension<T>
			{
				return Dimension<T>(thiz.width, thiz.height);
			}

			/**
			 * composite image by copying pixel
			 * image: this struct
			 * rectangle: area to cut
			 * return: newly struct with other data 
			*/

			inline static auto cut(
				const Image<int>& image,
				const Rectangle<int>& rectangle
			) -> Image<int>
			{
				auto data = ZList{};
				data.reserve(static_cast<ZList::size_type>(rectangle.area()) * 4);
				for (auto j : Range<int>(rectangle.y, rectangle.y + rectangle.height, 1)) {
					for (auto i : Range<int>(rectangle.x, rectangle.x + rectangle.width, 1)) {
						auto index = (j * image.width + i) * 4;
						data.insert(data.end(), &image.data()[index], &image.data()[static_cast<std::vector<uint8_t, std::allocator<uint8_t>>::size_type>(index) + 4]);
					}
				}
				return Image<int>(0, 0, rectangle.width, rectangle.height, std::move(data));
			}

			/**
			 * Dimension: dimension (width and height)
			 * return: transparent image
			*/

			inline static auto transparent(
				const Dimension<T>& dimension
			) -> Image<T>
			{
				return Image<T>(0, 0, dimension.width, dimension.height, std::vector<unsigned char>(dimension.area() * 4, 0x00));
			}

			/**
			 * Algorithm to join image - Reverse from split
			 * source: source image
			 * data: data to join
			 * return: the new source
			 */

			inline static auto join(
				Image<T>& source,
				const std::vector<Image<T>>& data
			) -> void
			{
				auto source_data = source.data();
				for (const auto& img : data) {
					if (!(img.width + img.x <= source.width and img.height + img.y <= source.height)) {
						throw Exception(fmt::format("{}", Language::get("image.does_not_fit_current_image")), std::source_location::current(), "join");
					}
					auto& image_data = img.data();
					for (auto j : Range<T>(img.height)) {
						for (auto i : Range<T>(img.width)) {
							auto source_index = ((j + img.y) * source.width + (i + img.x)) * 4;
							auto img_index = (j * img.width + i) * 4;
							source_data[source_index] = image_data[img_index];
							source_data[source_index + 1] = image_data[img_index + 1];
							source_data[source_index + 2] = image_data[img_index + 2];
							source_data[source_index + 3] = image_data[img_index + 3];
						}
					}
				}
				source.set_data(std::move(source_data));
				return;
			}


			/**
			 * Resize image algorithm
			 * source: source image
			 * percent: the new image percent to resize
			 * return: the newly image
			*/

			inline static auto resize(
				const Image<T>& source,
				float percent
			) -> Image<T>
			{
				auto new_width = static_cast<int>(Math::ceil(source.width * percent));
				auto new_height = static_cast<int>(Math::ceil(source.height * percent));
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
				return Image<T>(0, 0, new_width, new_height, std::move(resized_image_data));
			}

			/**
			 * Rotate image algorithm
			 * Image: current image
			 * angle: angle to rotate
			 * return: newly image after rotate
			*/

			inline static auto rotate(
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
				return Image<T>(0, 0, new_width, new_height, std::move(data));
			}

			/**
			 * Scale image using avir.h
			 * source: image source
			 * percentage: percentage to upscale
			 * return: new image 
			*/

			inline static auto scale(
				const Image<int> & source,
				float percentage
			) -> Image<int> const
			{
				auto new_width = static_cast<int>(source.width * percentage);
				auto new_height = static_cast<int>(source.height * percentage);
				const auto area = (new_width * new_height * 4);
				auto data = std::make_unique<uint8_t[]>(area);
				avir::CImageResizer<>{8}.resizeImage(
					source.data().data(), 
					static_cast<int>(source.width), 
					static_cast<int>(source.height), 
					0, 
					data.get(), 
					static_cast<int>(new_width), 
					static_cast<int>(new_height), 
					4, 
					0.0, 
					nullptr
				);
				auto vec = std::vector<uint8_t>(data.get(), data.get() + area);
				return Image<int>(0, 0, new_width, new_height, std::move(vec));
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
				T height
			) : Rectangle<T>(x, y, width, height)
			{
			}

			// constructor

			RectangleFileIO(
				T x,
				T y,
				T width,
				T height,
				const std::string& destination
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
				const std::string& destination
			) : Rectangle<T>(that), destination(destination)
			{
			}

			// destructor

			~RectangleFileIO(

			) = default;
	};

	template <typename T> requires Integral<T>
	struct RectangleFileIOList : RectangleFileIO<T> {
		public:
			std::string source;
			std::vector<RectangleFileIO<T>> data;
			RectangleFileIOList(
				const std::string& source,
				std::vector<RectangleFileIO<T>> data
			) : source(source), data(std::move(data))
			{

			}
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
			
			inline static auto read_png(
				std::string_view source
			) -> Image<int> 
			{
				#if WINDOWS
				auto fp = std::unique_ptr<FILE, decltype(Language::close_file)>(_wfopen(String::utf8view_to_utf16(fmt::format("\\\\?\\{}",
			String::to_windows_style(source.data()))).data(), L"rb"), Language::close_file);
				#else
				auto fp = std::unique_ptr<FILE, decltype(Language::close_file)>(std::fopen(source.data(), "rb"), Language::close_file);
				#endif
				if(!fp){
					throw Exception(fmt::format("{}: {}", Language::get("image.open_png_failed"), source), std::source_location::current(), "read_png");
				}
				auto png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr);
				if(!png_ptr){
					throw Exception(fmt::format("{}: {}", Language::get("image.png_pointer_init_failed"), source), std::source_location::current(), "read_png");
				}
				auto info_ptr = png_create_info_struct(png_ptr);  
				if(!info_ptr){
					png_destroy_read_struct(&png_ptr, nullptr, nullptr);
					throw Exception(fmt::format("{}: {}", Language::get("image.info_pointer_init_failed"), source), std::source_location::current(), "read_png");
				}
				if (setjmp(png_jmpbuf(png_ptr))) {
					png_destroy_read_struct(&png_ptr, &info_ptr, nullptr);
					throw Exception(fmt::format("{}: {}", Language::get("image.unknown_error"), source), std::source_location::current(), "read_png");
				}
				png_init_io(png_ptr, fp.get());
				png_read_png(png_ptr, info_ptr, PNG_TRANSFORM_IDENTITY, nullptr);
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
				};
				auto color_type = static_cast<int>(png_get_color_type(png_ptr, info_ptr));
				auto interlace_type = static_cast<int>(png_get_interlace_type(png_ptr, info_ptr));
				auto rowbytes = static_cast<int>(png_get_rowbytes(png_ptr, info_ptr));
				png_destroy_read_struct(&png_ptr, &info_ptr, nullptr);
				return Image<int>{
					width, 
					height, 
					bit_depth,
					color_type,
					interlace_type,
					channels,
					rowbytes,
					data
				};
			}

			/**
			 * libpng write png adaptation
			 * file path: output path to write
			 * data: the image data
			 * return: written image
			*/

			inline static auto write_png(
				std::string_view filepath, 
				const Image<int> &data
			) -> void
			{
				#if WINDOWS
				auto fp = std::unique_ptr<FILE, decltype(Language::close_file)>(_wfopen(String::utf8_to_utf16(fmt::format("\\\\?\\{}", String::to_windows_style({filepath.data(), filepath.size()}))).data(), L"wb"), Language::close_file);
				#else
				auto fp = std::unique_ptr<FILE, decltype(Language::close_file)>(std::fopen(filepath.data(), "wb"), Language::close_file);
				#endif
				if(fp == nullptr){
					throw Exception(fmt::format("{}: {}", Language::get("image.open_png_failed"), filepath), std::source_location::current(), "write_png");
				}
				auto png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr);
				if(png_ptr == nullptr){
					throw Exception(fmt::format("{}: {}", Language::get("image.png_pointer_init_failed"), filepath), std::source_location::current(), "write_png");
				}
				auto info_ptr = png_create_info_struct(png_ptr);
				if (info_ptr == nullptr) {
					png_destroy_write_struct(&png_ptr, nullptr);
					throw Exception(fmt::format("{}: {}", Language::get("image.info_pointer_init_failed"), filepath), std::source_location::current(), "write_png");
				}
				#define PNG_WRITE_SETJMP(png_ptr, info_ptr, fp) \
				if (setjmp(png_jmpbuf(png_ptr)))  \
				{ \
					png_destroy_write_struct(&png_ptr, &info_ptr);   \
					std::fclose(fp);     \
					throw Exception(fmt::format("{}: {}", Language::get("image.unknown_error"), filepath), std::source_location::current(), "write_png");\
				}
				PNG_WRITE_SETJMP(png_ptr, info_ptr, fp.get());
				png_init_io(png_ptr, fp.get());
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
				png_write_end(png_ptr, nullptr);
				png_destroy_write_struct(&png_ptr, &info_ptr);
				png_write_end(png_ptr, nullptr);
				png_destroy_write_struct(&png_ptr, &info_ptr);
				return;
			}

			/**
			 * Should use to split one image
			 * source: source file
			 * destination: destination file
			 * rectangle: the area to cut
			 * return: the new image
			*/

			inline static auto cut_fs(
				std::string_view source,
				std::string_view destination,
				const Rectangle<int> & rectangle
			) -> void
			{
				ImageIO::write_png(destination, Image<int>::cut(ImageIO::read_png(source), rectangle));
				return;
			}

			/**
			 * Should be used to split images
			 * source: source file
			 * data: list of rectangle file
			 * return: the cut 
			*/

			inline static auto cut_pngs(
				std::string_view source,
				const std::vector<RectangleFileIO<int>> & data
			) -> void
			{
				auto image = ImageIO::read_png(source);
				std::for_each(data.begin(), data.end(), [&](auto c)
				{ 
					ImageIO::write_png(c.destination, Image<int>::cut(image, c));
				});
				return;
			}

			/**
			 * Should be used to split images if id are unique
			 * source: source file
			 * data: list of rectangle file
			 * return: the cut 
			*/

			inline static auto cut_pngs_asynchronous(
				std::string_view source,
				const std::vector<RectangleFileIO<int>> & data
			) -> void
			{
				auto image = ImageIO::read_png(source);
				auto process = std::vector<std::future<void>>{};
				for (auto &c : data) {
					process.push_back(std::async(std::launch::async, [&]{
						ImageIO::write_png(c.destination, Image<int>::cut(image, c));
					}));
				}
				for(auto &f : process) {
					f.get();
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

			inline static auto transparent_png(
				std::string_view destination,
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

			inline static auto join_png(
				std::string_view destination,
				const Dimension<int> & dimension,
				const std::vector<Image<int>> & data
			) -> void
			{
				auto source = Image<int>::transparent(dimension);
				Image<int>::join(source, data);
				ImageIO::write_png(destination, source);
				return;
			}

			/**
			 * Should be used to join image
			 * destination: the file output destination
			 * dimension: output file dimension
			 * data: the image data
			 * return: the file output png
			 */

			inline static auto join(
				Image<int> & destination,
				const std::vector<Image<int>>& data
			) -> void
			{
				Image<int>::join(destination, data);
				return;
			}

			/**
			 * Resize image method
			 * @param source : image source
			 * @param destination : output file destination
			 * @param percent: percentage to resize
			 * @return the newly image
			 */

			inline static auto resize_png(
				std::string_view source,
				std::string_view destination,
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

			inline static auto rotate_png(
				std::string_view source,
				std::string_view destination,
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

			inline static auto scale_png(
				std::string_view source,
				std::string_view destination,
				float percent
			) -> void
			{
				ImageIO::write_png(destination, Image<int>::scale(ImageIO::read_png(source), percent));
				return;
			}

	};
}