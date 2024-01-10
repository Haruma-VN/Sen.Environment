#pragma once

#include "kernel/interface/cli.hpp"
#include "kernel/interface/data.hpp"
#include "kernel/definition/library.hpp"
#include "kernel/definition/macro.hpp"
#include "kernel/support/utility.hpp"

namespace Sen::Kernel::Interface {

	struct StringView {
		const char* value;
		size_t size;
	};

	struct StringList {
		StringView* value;
		size_t size;
	};

	using CStringView = StringView;
	
	using CStringList = StringList;

	typedef CStringView (*shell_callback)(CStringList list);

	struct Shell {
		private:
			inline static auto input_cb(

			) -> BasicStringView
			{
				return BasicStringView{"", 0};
			}

			inline static auto print_cb(
				const char* title,
				Interface::Color color
			) -> void
			{
				return;
			}

			inline static auto shell_cb(
				CStringList list
			) -> CStringView
			{
				return CStringView{"", 0};
			}

		public:
			inline static Interface::input input = input_cb;

			inline static Interface::callback print = print_cb;

			inline static Interface::shell_callback callback = shell_cb;
	};

	struct MShellAPI {

		public:
			unsigned int version;

			bool is_gui;
	};
}