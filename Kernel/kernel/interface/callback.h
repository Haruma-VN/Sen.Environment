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

			Interface::callback printline;

			Interface::callback print;

			void parameter_require_input(
				size_t index
			);

			void argument_require_input(

			);

		public:

			Callback(
				Interface::CliCallBack &command, 
				string &argument, 
				vector<string> &params,
				Interface::input &input,
				Interface::callback &printline,
				Interface::callback &print
			);

			~Callback(

			) = default;

			void execute(

			);
	};

}