#pragma once

#include "kernel/definition/utility.hpp"

namespace Sen::Kernel::Support::PopCap::Animation::Miscellaneous {
	

	using String = std::string;

	template <typename T>
	using List = std::vector<T>;

	template <typename T, size_t n>
	using Array = std::array<T, n>;

	using Matrix = Array<double, 6>;

	using Color = Array<double, 4>;

	using XMLDocument = tinyxml2::XMLDocument;

	using XMLElement = tinyxml2::XMLElement;

	struct BasicDocument;

	struct Sprite;

	struct Image;

	struct BasicDocument {
		List<String> media{};
		List<String> image{};
		List<String> sprite{};
		List<String> action{};

		BasicDocument(

		) = default;

		~BasicDocument(

		) = default;

		auto operator=(
			BasicDocument &&that
		) -> BasicDocument & = delete;

		BasicDocument(
			BasicDocument &&that
		) = delete;
	};

	struct Sprite {
		String name{};
		String link{};
		Matrix transform{};
		Color color{};

		explicit Sprite(
			String& name,
			String& link,
			Matrix& transform,
			Color& color
		) : name(name), link(link), transform(transform), color(color)
		{

		}

		Sprite(
		 	
		) = default;

		~Sprite(

		) = default;

		auto operator =(
			Sprite&& that
		) -> Sprite& = delete;

		Sprite(
			Sprite &&that
		) = delete;
	};

	struct Image {
		String name{};
		String id{};
		Matrix transform{};

		Image(
			String& name,
			String& id,
			Matrix& transform
		) : name(name), id(name), transform(transform)
		{

		}

		Image(

		) = default;

		~Image(

		) = default;

		auto operator=(
			Image &&that
		) -> Image & = delete;

		Image(
			Image &&that
		) = delete;
	};

	template <auto point, typename T> 
			requires std::is_integral<T>::value or std::is_floating_point<T>::value
	inline static auto to_fixed(
		const T& number
	) -> std::string
	{
		static_assert(sizeof(point) == sizeof(int));
		auto stream = std::ostringstream{};
		stream << std::fixed << std::setprecision(static_cast<std::streamsize>(point)) << number;
		return stream.str();
	}
}