#pragma once

#include "kernel/definition/utility.hpp"

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
				const Color &color
			) -> std::vector<unsigned char>
			{
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
				const Color &color
			) -> std::vector<unsigned char>
			{
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

	};
	
}