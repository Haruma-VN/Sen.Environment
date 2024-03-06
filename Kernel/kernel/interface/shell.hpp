#pragma once

#include "kernel/interface/cli.hpp"
#include "kernel/interface/data.hpp"
#include "kernel/definition/library.hpp"
#include "kernel/definition/macro.hpp"
#include "kernel/support/utility.hpp"

namespace Sen::Kernel::Interface {

	struct StringView {
		public:
			int size;
			const char* value;
	};

	struct StringList {
		public:
			StringView* value;
			int size;

			~StringList(

			) 
			{
				delete[] thiz.value;
			}
	};

	using CStringView = StringView;
	
	using CStringList = StringList;

	typedef void (*ShellCallback)(CStringList* list, StringView* destination, void* proxy_call);

	// Construct CStringView from standard String

	inline static auto construct_string(
		const std::string & that
	) -> CStringView
	{
		return CStringView {
			.size = static_cast<int>(that.size()),
			.value = that.c_str(),
		};
	}

	// Construct std::string from CStringView

	inline static auto make_standard_string(
		CStringView* that
	) -> std::string
	{
		return std::string{that->value, static_cast<std::size_t>(that->size)};
	}

	inline static auto construct_standard_string(
		const CStringView & that
	) -> std::string
	{
		return std::string{that.value, static_cast<std::size_t>(that.size)};
	}

	inline static auto construct_string_list(
		const std::vector<std::string> & that
	) -> std::shared_ptr<CStringList>
	{
		auto destination = std::make_shared<CStringList>(new StringView[that.size()], static_cast<int>(that.size()));
		for	(auto i : Range<std::size_t>(that.size())){
			destination->value[i] = construct_string(that.at(i));
		}
		return destination;
	}

	inline static auto destruct_string_list(
		CStringList* that
	) -> std::vector<std::string>
	{
		auto destination = std::vector<std::string>{};
		for (auto i : Range(static_cast<std::size_t>(that->size)))
		{
			destination.emplace_back(std::string{ that->value[i].value, static_cast<std::size_t>(that->value[i].size) });
		}
		return destination;
	}

	struct Shell {
		private:

			inline static auto shell_cb(
				CStringList* list,
				CStringView* destination,
				void* proxy
			) -> void
			{
				return;
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