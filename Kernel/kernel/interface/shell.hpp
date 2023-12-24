#pragma once

#include "kernel/interface/cli.hpp"
#include "kernel/interface/data.hpp"
#include "kernel/definition/library.hpp"
#include "kernel/definition/macro.hpp"
#include "kernel/support/utility.hpp"

namespace Sen::Kernel::Interface {

	struct Shell {
		private:
			inline static auto input_m(

			) -> BasicStringView
			{
				return BasicStringView{"", 0};
			}

			inline static auto print_m(
				const char* title,
				const Sen::Kernel::Interface::Color color
			) -> void
			{

			}

		public:
			inline static Interface::input input = input_m;

			inline static Interface::callback print = print_m;
	};

	struct MShellAPI {

		public:
			unsigned int version;

			bool is_gui;
	};
}