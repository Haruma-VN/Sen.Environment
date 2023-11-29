#include "kernel/interface/callback.h"

namespace Sen::Kernel::Interface 
{
	
	Callback::Callback(
		Interface::CliCallBack &command, 
		string &argument, 
		vector<string> &params,
		Interface::input &input,
		Interface::callback &printline,
		Interface::callback &print
	)
	{
		thiz.command = command;
		thiz.argument = argument;
		thiz.params = params;
		thiz.input = input;
		thiz.printline = printline;
		thiz.print = print;
	}

	auto Callback::parameter_require_input(
		size_t index
	) -> void
	{
		if(index < thiz.params.size()){
			return;
		}
		auto input_data = thiz.input();
		thiz.params.push_back(string {input_data.data, input_data.size});
		return;
	}

	auto Callback::argument_require_input(

	) -> void
	{
		if(std::filesystem::exists(Path::normalize(thiz.argument)))
		{
			return;
		}
		auto input_data = thiz.input();
		thiz.argument = string{input_data.data, input_data.size};
		return;
	}

	auto Callback::execute(

	) -> void
	{
		switch(thiz.command){
			case Interface::MD5_HASH:{
				thiz.printline(Sen::Kernel::Definition::Encryption::MD5::hash(Sen::Kernel::String::convertStringToSpan<unsigned char>(thiz.argument)).c_str());
				break;
			}
			case Interface::SHA224_HASH:{
				thiz.printline(Sen::Kernel::Definition::Encryption::Sha224::hash(thiz.argument).c_str());
				break;
			}
			case Interface::SHA256_HASH:{
				thiz.printline(Sen::Kernel::Definition::Encryption::SHA256::hash(thiz.argument).c_str());
				break;
			}
			case Interface::BASE64_ENCODE:{
				thiz.argument_require_input();
				thiz.parameter_require_input(0);
				Sen::Kernel::Definition::Encryption::Base64::encode_fs(thiz.argument, thiz.params.at(0));
				break;
			}
			case Interface::BASE64_DECODE:{
				thiz.argument_require_input();
				thiz.parameter_require_input(0);
				Sen::Kernel::Definition::Encryption::Base64::decode_fs(thiz.argument, thiz.params.at(0));
				break;
			}
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