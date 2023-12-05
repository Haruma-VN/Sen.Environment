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

			static auto argb_to_rgba(
				const std::vector<unsigned char> &color
			) -> std::vector<unsigned char>
			{
				auto data = std::vector<unsigned char>();
				data.reserve(color.size());
				for (auto i = static_cast<size_t>(0); i < color.size(); i += 4){
					data.push_back(color[i + 1]);
					data.push_back(color[i + 2]);
					data.push_back(color[i + 3]);
					data.push_back(color[i]);
				}
				return data;
			}

		public:
			
			static auto rgba(
				const std::vector<unsigned char> &color,
				int width,
				int height
			) -> Image<int>
			{
				auto img = Image<int>{};
				img.width = width;
				img.height = height;
				img.set_data(color);
				return img;
			}

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