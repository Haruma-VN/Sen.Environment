#pragma once

#include "kernel/definition/utility.hpp"
#include "kernel/support/popcap/pvz2/definition.hpp"
#include "kernel/support/popcap/pvz2/common.hpp"

namespace Sen::Kernel::Support::PopCap::PvZ2 {

	using namespace Definition;

	
	class Encode {

		public:

			explicit Encode(

			) = default;

			~Encode(

			) = default;

			auto operator =(
				Encode&& that
			) -> Encode& = delete;

			Encode(
				Encode&& that
			) = delete;

			inline auto process(
				
			) -> void
			{
				return;
			}

			inline static auto process_fs (
				std::string_view source,
				std::string_view destination
			) -> void
			{
				using Encode = Encode;
				auto encode = Encode{};
				encode.process();
				return;
			}


	};


}