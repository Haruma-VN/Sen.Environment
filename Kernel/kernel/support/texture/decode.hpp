#pragma once

#include "kernel/definition/utility.hpp"

namespace Sen::Kernel::Support::Texture {

	// use color

	using namespace Sen::Kernel::Definition;

	/**
	 * Decode class
	*/

	class Decode {

		private:

			/**
			 * Convert alpha red green blue to red green blue alpha (standard)
			*/

			static auto argb_to_rgba(
				const std::vector<unsigned char> &color
			) -> std::vector<unsigned char>
			{
				auto data = std::vector<unsigned char>();
				data.reserve(color.size());
				for (auto i : Range<size_t>(color.size(), 4)) {
					// red
					data.push_back(color[i + 1]);
					// green
					data.push_back(color[i + 2]);
					// blue
					data.push_back(color[i + 3]);
					// alpha
					data.push_back(color[i]);
				}
				return data;
			}

		public:

			/**
			 * color: stream color
			 * width: image width
			 * height: image height
			 * return: Image struct
			*/
			
			static auto rgba(
				const std::vector<unsigned char> &color,
				int width,
				int height
			) -> Image<int>
			{
				return Image<int>(width, height, color);
			}

			/**
			 * color: stream color
			 * width: image width
			 * height: image height
			 * return: Image struct
			*/

			static auto argb(
				const std::vector<unsigned char> &color,
				int width,
				int height
			) -> Image<int>
			{
				return rgba(argb_to_rgba(color), width, height);
			}

	};
}