#pragma once

#include "kernel/definition/utility.hpp"
#include "kernel/support/popcap/pvz2/definition.hpp"
#include "kernel/support/popcap/pvz2/common.hpp"

namespace Sen::Kernel::Support::PopCap::PvZ2 {

	using namespace Definition;

	
	class Pack {

		public:

			explicit Pack(

			) = default;

			~Pack(

			) = default;

			auto operator =(
				Pack&& that
			) -> Pack& = delete;

			Pack(
				Pack&& that
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
				using Pack = Pack;
				auto pack = Pack{};
				pack.process();
				return;
			}


	};


}