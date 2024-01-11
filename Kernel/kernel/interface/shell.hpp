#pragma once

#include "kernel/interface/cli.hpp"
#include "kernel/interface/data.hpp"
#include "kernel/definition/library.hpp"
#include "kernel/definition/macro.hpp"
#include "kernel/support/utility.hpp"

namespace Sen::Kernel::Interface {

	struct StringView {
		public:
			size_t size;
			const char* value;
	};

	struct StringList {
		public:
			StringView* value;
			size_t size;

			~StringList(

			) 
			{
				delete[] thiz.value;
			}
	};

	using CStringView = StringView;
	
	using CStringList = StringList;

	typedef CStringView (*ShellCallback)(CStringList list);

	// Construct CStringView from standard String

	inline static auto construct_string(
		const std::string & that
	) -> CStringView
	{
		return CStringView{
			.size = that.size(),
			.value = that.c_str()
		};
	}

	// Construct std::string from CStringView

	inline static auto make_standard_string(
		CStringView* that
	) -> std::string
	{
		return std::string{that->value, that->size};
	}

	inline static auto construct_string_list(
		const std::vector<std::string> & that
	) -> CStringList
	{
		auto destination = CStringList{
			.value = new StringView[that.size()],
			.size = that.size()
		};
		for	(auto i : Range<size_t>(that.size())){
			destination.value[i] = construct_string(that.at(i));
		}
		return destination;
	}

	struct Shell {
		private:

			inline static auto shell_cb(
				CStringList list
			) -> CStringView
			{
				return CStringView{0, ""};
			}

		public:

			inline static Interface::ShellCallback callback = shell_cb;
	};

	struct MShellAPI {

		public:
			unsigned int version;

			bool is_gui;
	};
}