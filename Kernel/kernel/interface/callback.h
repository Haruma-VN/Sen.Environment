#pragma once

#include "kernel/interface/cli.hpp"
#include "kernel/interface/data.hpp"
#include "kernel/definition/library.hpp"
#include "kernel/definition/macro.hpp"
#include "kernel/support/popcap/resource_group/common.hpp"

namespace Sen::Kernel::Interface 
{

	// use string

	using std::string;

	// use vector

	using std::vector;

	// callback

	class Callback {

		private:

			Interface::CliCallBack command;

			string argument;

			vector<string> params;

			Interface::input input;

			Interface::callback print;

			auto parameter_require_input(
				const size_t &index
			) -> void;

			auto argument_require_input(

			) -> void;

			auto printc(
				const string &title,
				const string &message,
				const Interface::Color &color
			) -> void;

			static auto constexpr emptyString = "";

		public:

			Callback(
				const Interface::CliCallBack &command, 
				const string &argument, 
				const vector<string> &params,
				const Interface::input &input,
				const Interface::callback &print
			);

			~Callback(

			) = default;

			auto execute(

			) -> void;
	};

}