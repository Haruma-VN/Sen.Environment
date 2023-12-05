#pragma once

#include "kernel/interface/cli.hpp"
#include "kernel/interface/data.hpp"
#include "kernel/definition/library.hpp"
#include "kernel/definition/macro.hpp"
#include "kernel/support/utility.hpp"

namespace Sen::Kernel::Interface {

	// use string

	using std::string;

	// use vector

	using std::vector;

	// callback

	class Callback {

		private:

			Interface::CliCallBack command;

			std::string argument;

			std::vector<std::string> params;

			Interface::input input;

			Interface::callback print;

			auto parameter_require_input(
				size_t index
			) -> void;

			auto argument_require_input(

			) -> void;

			auto printc(
				const std::string &title,
				const std::string &message,
				Interface::Color color
			) -> void;

			static auto constexpr emptyString = "";

		public:

			Callback(
				const Interface::CliCallBack &command, 
				const std::string &argument, 
				const std::vector<std::string> &params,
				const Interface::input &input,
				const Interface::callback &print
			);

			~Callback(

			) = default;

			auto execute(

			) -> void;
	};

}