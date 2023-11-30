#pragma once

#include "kernel/definition/macro.hpp"
#include "kernel/definition/library.hpp"

namespace Sen::Kernel {

	// using without namespace

	using std::runtime_error;

	// string is used

	using std::string;

	// assert: condition & message
	// if not condition, will throw message

	inline auto try_assert(
		bool conditional, 
		string message
	) -> void
	{
		if (!conditional) {
			throw runtime_error(message);
		}
		return;
	}

	inline auto parse_exception(
		
	) -> std::string
	{
		auto destination = std::string{};
		auto st = backward::StackTrace{};
		st.load_here(32);
		auto p = backward::Printer{};
		auto ss = std::stringstream{};
		p.print(st, ss);
		destination += ss.str();
		return destination;
	}

}