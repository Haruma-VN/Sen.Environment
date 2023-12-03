#include "kernel/interface/callback.h"

namespace Sen::Kernel::Interface 
{
	
	Callback::Callback(
		const Interface::CliCallBack &command, 
		const string &argument, 
		const vector<string> &params,
		const Interface::input &input,
		const Interface::callback &print
	)
	{
		thiz.command = command;
		thiz.argument = argument;
		thiz.params = params;
		thiz.input = input;
		thiz.print = print;
	}

	auto Callback::parameter_require_input(
		const size_t &index
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

	auto Callback::printc(
		const string & title,
		const string & message,
		const Interface::Color &color
	) -> void
	{
		thiz.print(fmt::format("{}\n  ", title).c_str(), message.c_str(), color);
		return;
		return;
	}

	auto Callback::execute(

	) -> void
	{
		auto const before = Sen::Kernel::Definition::Timer::start();
		switch(thiz.command){
			case Sen::Kernel::Interface::CliCallBack::MD5_HASH:{
				thiz.print(fmt::format("Method loaded: {}", "MD5 Hash").c_str(), Sen::Kernel::Interface::Callback::emptyString, Sen::Kernel::Interface::Color::CYAN);
				thiz.printc(fmt::format("MD5 Hash result:"), Sen::Kernel::Definition::Encryption::MD5::hash(Sen::Kernel::String::convertStringToSpan<unsigned char>(thiz.argument)), Sen::Kernel::Interface::Color::GREEN);
				break;
			}
			case Sen::Kernel::Interface::CliCallBack::SHA224_HASH:{
				thiz.print(fmt::format("Method loaded: {}", "SHA-224 Hash").c_str(), Sen::Kernel::Interface::Callback::emptyString, Sen::Kernel::Interface::Color::CYAN);
				thiz.printc(fmt::format("SHA-224 Hash result:"), Sen::Kernel::Definition::Encryption::Sha224::hash(thiz.argument), Sen::Kernel::Interface::Color::GREEN);
				break;
			}
			case Sen::Kernel::Interface::CliCallBack::SHA256_HASH:{
				thiz.print(fmt::format("Method loaded: {}", "SHA-256 Hash").c_str(), Sen::Kernel::Interface::Callback::emptyString, Sen::Kernel::Interface::Color::CYAN);
				thiz.printc(fmt::format("SHA-256 Hash result:"),Sen::Kernel::Definition::Encryption::SHA256::hash(thiz.argument), Sen::Kernel::Interface::Color::GREEN);
				break;
			}
			case Sen::Kernel::Interface::CliCallBack::BASE64_ENCODE:{
				thiz.print(fmt::format("Method loaded: {}", "Base64 Encode").c_str(), Sen::Kernel::Interface::Callback::emptyString, Sen::Kernel::Interface::Color::CYAN);
				thiz.argument_require_input();
				thiz.parameter_require_input(0);
				Sen::Kernel::Definition::Encryption::Base64::encode_fs(thiz.argument, thiz.params.at(0));
				break;
			}
			case Sen::Kernel::Interface::CliCallBack::BASE64_DECODE:{
				thiz.print(fmt::format("Method loaded: {}", "Base64 Decode").c_str(), Sen::Kernel::Interface::Callback::emptyString, Sen::Kernel::Interface::Color::CYAN);
				thiz.argument_require_input();
				thiz.parameter_require_input(0);
				Sen::Kernel::Definition::Encryption::Base64::decode_fs(thiz.argument, thiz.params.at(0));
				break;
			}
			case Sen::Kernel::Interface::CliCallBack::ZLIB_COMPRESS:{
				thiz.print(fmt::format("Method loaded: {}", "Zlib Compress").c_str(), Sen::Kernel::Interface::Callback::emptyString, Sen::Kernel::Interface::Color::CYAN);
				thiz.argument_require_input();
				thiz.parameter_require_input(0);
				Sen::Kernel::Definition::Compression::Zlib::compress_fs(thiz.argument, thiz.params.at(0), Sen::Kernel::Definition::Compression::ZlibLevel::LEVEL_9);
				break;
			}
			case Sen::Kernel::Interface::CliCallBack::ZLIB_UNCOMPRESS:{
				thiz.print(fmt::format("Method loaded: {}", "Zlib Uncompress").c_str(), Sen::Kernel::Interface::Callback::emptyString, Sen::Kernel::Interface::Color::CYAN);
				thiz.argument_require_input();
				thiz.parameter_require_input(0);
				Sen::Kernel::Definition::Compression::Zlib::uncompress_fs(thiz.argument, thiz.params.at(0));
				break;
			}
			case Sen::Kernel::Interface::CliCallBack::GZIP_COMPRESS:{
				thiz.print(fmt::format("Method loaded: {}", "Gzip Compress").c_str(), Sen::Kernel::Interface::Callback::emptyString, Sen::Kernel::Interface::Color::CYAN);
				thiz.argument_require_input();
				thiz.parameter_require_input(0);
				Sen::Kernel::Definition::Compression::Zlib::compress_gzip_fs(thiz.argument, thiz.params.at(0));
				break;
			}
			case Sen::Kernel::Interface::CliCallBack::GZIP_UNCOMPRESS:{
				thiz.print(fmt::format("Method loaded: {}", "Gzip Uncompress").c_str(), Sen::Kernel::Interface::Callback::emptyString, Sen::Kernel::Interface::Color::CYAN);
				thiz.argument_require_input();
				thiz.parameter_require_input(0);
				Sen::Kernel::Definition::Compression::Zlib::uncompress_gzip_fs(thiz.argument, thiz.params.at(0));
				break;
			}
			case Sen::Kernel::Interface::CliCallBack::RESOURCE_GROUP_SPLIT:{
				thiz.argument_require_input();
				thiz.parameter_require_input(0);
				Sen::Kernel::Support::PopCap::ResourceGroup::split(thiz.argument, thiz.params.at(0));
				break;
			}
			case Interface::RESOURCE_GROUP_MERGE:{
				thiz.argument_require_input();
				thiz.parameter_require_input(0);
				Sen::Kernel::Support::PopCap::ResourceGroup::merge(thiz.argument, thiz.params.at(0));
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
		auto const after = Sen::Kernel::Definition::Timer::start();
		auto const time_spent = Sen::Kernel::Definition::Timer::calculate(before, after);
		thiz.print(fmt::format("Time spent: {}s", time_spent).c_str(), Sen::Kernel::Interface::Callback::emptyString,Sen::Kernel::Interface::Color::GREEN);
		return;
	}
}