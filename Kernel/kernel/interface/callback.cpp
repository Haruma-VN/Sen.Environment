#include "kernel/interface/callback.h"

namespace Sen::Kernel::Interface 
{
	
	Callback::Callback(
		Interface::CliCallBack command, 
		string argument, 
		vector<string> params,
		Interface::input input,
		Interface::callback printline,
		Interface::callback print
	)
	{
		thiz.command = command;
		thiz.argument = argument;
		thiz.params = params;
		thiz.input = input;
		thiz.printline = printline;
		thiz.print = print;
	}

	void Callback::parameter_require_input(
		size_t index
	)
	{
		if(index < thiz.params.size()){
			return;
		}
		auto input_data = thiz.input();
		thiz.params.push_back(string {input_data.data, input_data.size});
		return;
	}

	void Callback::argument_require_input(

	)
	{
		if(std::filesystem::exists(Path::toPosixStyle(thiz.argument)))
		{
			return;
		}
		auto input_data = thiz.input();
		thiz.argument = string{input_data.data, input_data.size};
		return;
	}

	void Callback::execute(

	)
	{
		switch(thiz.command){
			case Interface::RESOURCE_GROUP_SPLIT:{
				thiz.argument_require_input();
				thiz.parameter_require_input(0);
				Sen::Kernel::Support::PopCap::ResourceGroup::split(thiz.argument, thiz.params.at(0));
				break;
			}
			case Interface::RESOURCE_GROUP_MERGE:{
				break;
			}
			case Interface::RES_INFO_SPLIT: {
				break;
			}
			case Interface::RES_INFO_MERGE:{
				break;
			}
			case Interface::RESOURCE_GROUP_TO_RES_INFO:{
				break;
			}
			case Interface::RES_INFO_TO_RESOURCE_GROUP:{
				break;
			}
			default:{
				throw std::runtime_error(fmt::format("Method not found: {}", thiz.command));
			}
		}
		return;
	}
}