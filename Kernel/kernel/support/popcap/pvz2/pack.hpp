#pragma once

#include "kernel/definition/utility.hpp"
#include "kernel/support/popcap/pvz2/definition.hpp"
#include "kernel/support/popcap/pvz2/common.hpp"

namespace Sen::Kernel::Support::PopCap::PvZ2 {

	using namespace Definition;
	class Pack {

		protected:
			std::unique_ptr<DataStreamView> stream;
		
		protected:
			inline auto pack_packages() -> void {

			}

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
				std::string_view source
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
				pack.process(source);
				pack.stream->out_file(destination);
				return;
			}


	};


}