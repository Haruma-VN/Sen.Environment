#include "kernel/interface/callback.h"

namespace Sen::Kernel::Interface {

	// constructor
	
	Callback::Callback(
		const Interface::CliCallBack &command, 
		const std::string &argument, 
		const std::vector<std::string> &params,
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

	/**
	 * 
	 * @param index: the index in the parameter need input
	 * @return: the input value
	*/

	auto Callback::parameter_require_input(
		size_t index
	) -> void
	{
		if(index < thiz.params.size()){
			return;
		}
		auto input_data = thiz.input();
		thiz.params.push_back(std::string {input_data.data, input_data.size});
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
		const std::string & title,
		const std::string & message,
		Interface::Color color
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
				thiz.printc(fmt::format("MD5 Hash result:"), Sen::Kernel::Definition::Encryption::MD5::hash(Sen::Kernel::String::convertStringToSpan<unsigned char>(thiz.argument)), Sen::Kernel::Interface::Color::GREEN);
				break;
			}
			case Sen::Kernel::Interface::CliCallBack::SHA224_HASH:{
				thiz.printc(fmt::format("SHA-224 Hash result:"), Sen::Kernel::Definition::Encryption::Sha224::hash(thiz.argument), Sen::Kernel::Interface::Color::GREEN);
				break;
			}
			case Sen::Kernel::Interface::CliCallBack::SHA256_HASH:{
				thiz.printc(fmt::format("SHA-256 Hash result:"),Sen::Kernel::Definition::Encryption::SHA256::hash(thiz.argument), Sen::Kernel::Interface::Color::GREEN);
				break;
			}
			case Sen::Kernel::Interface::CliCallBack::BASE64_ENCODE:{
				thiz.argument_require_input();
				thiz.parameter_require_input(0);
				Sen::Kernel::Definition::Encryption::Base64::encode_fs(thiz.argument, thiz.params.at(0));
				break;
			}
			case Sen::Kernel::Interface::CliCallBack::BASE64_DECODE:{
				thiz.argument_require_input();
				thiz.parameter_require_input(0);
				Sen::Kernel::Definition::Encryption::Base64::decode_fs(thiz.argument, thiz.params.at(0));
				break;
			}
			case Sen::Kernel::Interface::CliCallBack::ZLIB_COMPRESS:{
				thiz.argument_require_input();
				thiz.parameter_require_input(0);
				Sen::Kernel::Definition::Compression::Zlib::compress_fs(thiz.argument, thiz.params.at(0), Sen::Kernel::Definition::Compression::Zlib::Level::LEVEL_9);
				break;
			}
			case Sen::Kernel::Interface::CliCallBack::ZLIB_UNCOMPRESS:{
				thiz.argument_require_input();
				thiz.parameter_require_input(0);
				Sen::Kernel::Definition::Compression::Zlib::uncompress_fs(thiz.argument, thiz.params.at(0));
				break;
			}
			case Sen::Kernel::Interface::CliCallBack::GZIP_COMPRESS:{
				thiz.argument_require_input();
				thiz.parameter_require_input(0);
				Sen::Kernel::Definition::Compression::Zlib::compress_gzip_fs(thiz.argument, thiz.params.at(0));
				break;
			}
			case Sen::Kernel::Interface::CliCallBack::GZIP_UNCOMPRESS:{
				thiz.argument_require_input();
				thiz.parameter_require_input(0);
				Sen::Kernel::Definition::Compression::Zlib::uncompress_gzip_fs(thiz.argument, thiz.params.at(0));
				break;
			}
			case Sen::Kernel::Interface::CliCallBack::LZMA_COMPRESS:{
				thiz.argument_require_input();
				thiz.parameter_require_input(0);
				Sen::Kernel::Definition::Compression::Lzma::compress_fs(thiz.argument, thiz.params.at(0));
				break;
			}
			case Sen::Kernel::Interface::CliCallBack::LZMA_UNCOMPRESS:{
				thiz.argument_require_input();
				thiz.parameter_require_input(0);
				thiz.parameter_require_input(1);
				Sen::Kernel::Definition::Compression::Lzma::uncompress_fs(thiz.argument, thiz.params.at(0), std::stoull(thiz.params.at(1)));
				break;
			}
			case Sen::Kernel::Interface::CliCallBack::BZIP2_COMPRESS:{
				thiz.argument_require_input();
				thiz.parameter_require_input(0);
				Sen::Kernel::Definition::Compression::Bzip2::compress_fs(thiz.argument, thiz.params.at(0));
				break;
			}
			case Sen::Kernel::Interface::CliCallBack::BZIP2_UNCOMPRESS:{
				thiz.argument_require_input();
				thiz.parameter_require_input(0);
				Sen::Kernel::Definition::Compression::Bzip2::uncompress_fs(thiz.argument, thiz.params.at(0));
				break;
			}
			case Sen::Kernel::Interface::CliCallBack::RESOURCE_GROUP_SPLIT:{
				thiz.argument_require_input();
				thiz.parameter_require_input(0);
				Sen::Kernel::Support::PopCap::ResourceGroup::BasicConversion::split(thiz.argument, thiz.params.at(0));
				break;
			}
			case Sen::Kernel::Interface::CliCallBack::RESOURCE_GROUP_MERGE:{
				thiz.argument_require_input();
				thiz.parameter_require_input(0);
				Sen::Kernel::Support::PopCap::ResourceGroup::BasicConversion::merge(thiz.argument, thiz.params.at(0));
				break;
			}
			case Sen::Kernel::Interface::CliCallBack::IMAGE_SCALE: {
				thiz.argument_require_input();
				thiz.parameter_require_input(0);
				thiz.parameter_require_input(1);
				Sen::Kernel::Definition::ImageIO::scale_png(thiz.argument, thiz.params.at(0), std::stof(thiz.params.at(1)));
				break;
			}
			case Sen::Kernel::Interface::CliCallBack::RES_INFO_SPLIT:{
				thiz.argument_require_input();
				thiz.parameter_require_input(0);
				Sen::Kernel::Support::PopCap::ResInfo::BasicConversion::split_fs(thiz.argument, thiz.params.at(0));
				break;
			}
			case Sen::Kernel::Interface::CliCallBack::RES_INFO_MERGE:{
				thiz.argument_require_input();
				thiz.parameter_require_input(0);
				Sen::Kernel::Support::PopCap::ResInfo::BasicConversion::merge_fs(thiz.argument, thiz.params.at(0));
				break;
			}
			case Sen::Kernel::Interface::CliCallBack::RESOURCE_GROUP_TO_RES_INFO:{
				Sen::Kernel::Support::PopCap::ResourceGroup::Convert::convert_fs(thiz.argument, thiz.params.at(0), static_cast<Sen::Kernel::Support::PopCap::ResourceGroup::PathStyle>(std::stoi(thiz.params.at(1))));
				break;
			}
			case Sen::Kernel::Interface::CliCallBack::RES_INFO_TO_RESOURCE_GROUP:{
				Sen::Kernel::Support::PopCap::ResInfo::Convert::convert_fs(thiz.argument, thiz.params.at(0));
				break;
			}
			case Sen::Kernel::Interface::CliCallBack::TEXTURE_DECODE: {
				Sen::Kernel::Support::Texture::InvokeMethod::decode_fs(thiz.argument, thiz.params.at(0), std::stoi(thiz.params.at(1)), std::stoi(thiz.params.at(2)), static_cast<Sen::Kernel::Support::Texture::Format>(std::stoi(thiz.params.at(3))));
				break;
			}
			case Sen::Kernel::Interface::CliCallBack::TEXTURE_ENCODE: {
				Sen::Kernel::Support::Texture::InvokeMethod::encode_fs(thiz.argument, thiz.params.at(0), static_cast<Sen::Kernel::Support::Texture::Format>(std::stoi(thiz.params.at(1))));
				break;
			}
			case Sen::Kernel::Interface::CliCallBack::POPCAP_ZLIB_COMPRESS:{
				thiz.argument_require_input();
				thiz.parameter_require_input(0);
				Sen::Kernel::Support::PopCap::Zlib::Compress::compress_fs(thiz.argument, thiz.params.at(0), static_cast<bool>(std::stoi(thiz.params.at(1))));
				break;
			}
			case Sen::Kernel::Interface::CliCallBack::POPCAP_ZLIB_UNCOMPRESS:{
				thiz.argument_require_input();
				thiz.parameter_require_input(0);
				Sen::Kernel::Support::PopCap::Zlib::Uncompress::uncompress_fs(thiz.argument, thiz.params.at(0), static_cast<bool>(std::stoi(thiz.params.at(1))));
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