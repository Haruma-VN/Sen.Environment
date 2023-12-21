#pragma once

#include "kernel/interface/shell.hpp"

namespace Sen::Kernel::Interface {

	// use string

	using std::string;

	// use vector

	using std::vector;

	/**
	 * Use JS instead of Sen::Kernel::Definition::JavaScript
	*/

	namespace JS = Sen::Kernel::Definition::JavaScript;

	// callback

	class Callback {

		private:

			Interface::CliCallBack command;

			std::string argument;

			std::vector<std::string> params;

			Interface::Shell shell;

			/**
			 * 
			 * @param index: the index in the parameter need input
			 * @return: the input value
			*/

			auto parameter_require_input(
				size_t index
			) -> void
			{
				if(index < thiz.params.size()){
					return;
				}
				auto input_data = shell.input();
				thiz.params.push_back(std::string {input_data.data, input_data.size});
				return;
			}

			/**
			 * Require input the argument [argc]
			*/

			auto argument_require_input(

			) -> void
			{
				if(std::filesystem::exists(Path::normalize(thiz.argument)))
				{
					return;
				}
				auto input_data = shell.input();
				thiz.argument = string{input_data.data, input_data.size};
				return;
			}

			/**
			 * Invoke caller in Shell
			*/

			auto printc(
				const std::string & title,
				const std::string & message,
				Interface::Color color
			) -> void
			{
				shell.print(fmt::format("{}\n  ", title).c_str(), message.c_str(), color);
				return;
			}

			/**
			 * empty string
			*/

			inline static auto const emptyString = std::string{""};

		public:

			explicit Callback(
				const Interface::CliCallBack &command, 
				const std::string &argument, 
				const std::vector<std::string> &params
			) : command(command), argument(argument), params(params)
			{

			}
			
			/**
			 * Destructor
			*/

			~Callback(

			) = default;

			/**
			 * JS Print method
			*/

			static auto print_js(
				JSContext *ctx, 
				JSValueConst this_val, 
				int argc, 
				JSValueConst *argv
			) 
			{
				try_assert(argc > 2, "argument must be greater than 2");
				const char* str = JS_ToCString(ctx, argv[0]);
				if (str) {
					Shell{}.print(JS::Converter::to_string(ctx, argv[0]).c_str(), JS::Converter::to_string(ctx, argv[1]).c_str(), static_cast<Sen::Kernel::Interface::Color>(JS::Converter::to_int32(ctx, argv[2])));
					JS_FreeCString(ctx, str);
				}
				return JS_UNDEFINED;
			}

			/**
			 * Execute method
			*/

			auto execute(

			) -> void
			{
				auto const before = Sen::Kernel::Definition::Timer::start();
				auto js_runtime = std::make_shared<JS::Runtime>();
				js_runtime->add_proxy(print_js, std::string{"Sen"}, std::string{"Kernel"}, std::string{"Console"} ,std::string{"print"});
				js_runtime->eval(
					std::string{
						"let a = 5; let b = 2; Sen.Kernel.Console.print(\"\", a + b, 12);"},
					std::string{"./test.js"});
				// switch(thiz.command){
				// 	case Sen::Kernel::Interface::CliCallBack::MD5_HASH:{
				// 		thiz.printc(fmt::format("MD5 Hash result:"), Sen::Kernel::Definition::Encryption::MD5::hash(Sen::Kernel::String::convertStringToSpan<unsigned char>(thiz.argument)), Sen::Kernel::Interface::Color::GREEN);
				// 		break;
				// 	}
				// 	case Sen::Kernel::Interface::CliCallBack::SHA224_HASH:{
				// 		thiz.printc(fmt::format("SHA-224 Hash result:"), Sen::Kernel::Definition::Encryption::Sha224::hash(thiz.argument), Sen::Kernel::Interface::Color::GREEN);
				// 		break;
				// 	}
				// 	case Sen::Kernel::Interface::CliCallBack::SHA256_HASH:{
				// 		thiz.printc(fmt::format("SHA-256 Hash result:"),Sen::Kernel::Definition::Encryption::SHA256::hash(thiz.argument), Sen::Kernel::Interface::Color::GREEN);
				// 		break;
				// 	}
				// 	case Sen::Kernel::Interface::CliCallBack::BASE64_ENCODE:{
				// 		thiz.argument_require_input();
				// 		thiz.parameter_require_input(0);
				// 		Sen::Kernel::Definition::Encryption::Base64::encode_fs(thiz.argument, thiz.params.at(0));
				// 		break;
				// 	}
				// 	case Sen::Kernel::Interface::CliCallBack::BASE64_DECODE:{
				// 		thiz.argument_require_input();
				// 		thiz.parameter_require_input(0);
				// 		Sen::Kernel::Definition::Encryption::Base64::decode_fs(thiz.argument, thiz.params.at(0));
				// 		break;
				// 	}
				// 	case Sen::Kernel::Interface::CliCallBack::ZLIB_COMPRESS:{
				// 		thiz.argument_require_input();
				// 		thiz.parameter_require_input(0);
				// 		Sen::Kernel::Definition::Compression::Zlib::compress_fs(thiz.argument, thiz.params.at(0), Sen::Kernel::Definition::Compression::Zlib::Level::LEVEL_9);
				// 		break;
				// 	}
				// 	case Sen::Kernel::Interface::CliCallBack::ZLIB_UNCOMPRESS:{
				// 		thiz.argument_require_input();
				// 		thiz.parameter_require_input(0);
				// 		Sen::Kernel::Definition::Compression::Zlib::uncompress_fs(thiz.argument, thiz.params.at(0));
				// 		break;
				// 	}
				// 	case Sen::Kernel::Interface::CliCallBack::GZIP_COMPRESS:{
				// 		thiz.argument_require_input();
				// 		thiz.parameter_require_input(0);
				// 		Sen::Kernel::Definition::Compression::Zlib::compress_gzip_fs(thiz.argument, thiz.params.at(0));
				// 		break;
				// 	}
				// 	case Sen::Kernel::Interface::CliCallBack::GZIP_UNCOMPRESS:{
				// 		thiz.argument_require_input();
				// 		thiz.parameter_require_input(0);
				// 		Sen::Kernel::Definition::Compression::Zlib::uncompress_gzip_fs(thiz.argument, thiz.params.at(0));
				// 		break;
				// 	}
				// 	case Sen::Kernel::Interface::CliCallBack::LZMA_COMPRESS:{
				// 		thiz.argument_require_input();
				// 		thiz.parameter_require_input(0);
				// 		Sen::Kernel::Definition::Compression::Lzma::compress_fs(thiz.argument, thiz.params.at(0));
				// 		break;
				// 	}
				// 	case Sen::Kernel::Interface::CliCallBack::LZMA_UNCOMPRESS:{
				// 		thiz.argument_require_input();
				// 		thiz.parameter_require_input(0);
				// 		thiz.parameter_require_input(1);
				// 		Sen::Kernel::Definition::Compression::Lzma::uncompress_fs(thiz.argument, thiz.params.at(0), std::stoull(thiz.params.at(1)));
				// 		break;
				// 	}
				// 	case Sen::Kernel::Interface::CliCallBack::BZIP2_COMPRESS:{
				// 		thiz.argument_require_input();
				// 		thiz.parameter_require_input(0);
				// 		Sen::Kernel::Definition::Compression::Bzip2::compress_fs(thiz.argument, thiz.params.at(0));
				// 		break;
				// 	}
				// 	case Sen::Kernel::Interface::CliCallBack::BZIP2_UNCOMPRESS:{
				// 		thiz.argument_require_input();
				// 		thiz.parameter_require_input(0);
				// 		Sen::Kernel::Definition::Compression::Bzip2::uncompress_fs(thiz.argument, thiz.params.at(0));
				// 		break;
				// 	}
				// 	case Sen::Kernel::Interface::CliCallBack::RESOURCE_GROUP_SPLIT:{
				// 		thiz.argument_require_input();
				// 		thiz.parameter_require_input(0);
				// 		Sen::Kernel::Support::PopCap::ResourceGroup::BasicConversion::split(thiz.argument, thiz.params.at(0));
				// 		break;
				// 	}
				// 	case Sen::Kernel::Interface::CliCallBack::RESOURCE_GROUP_MERGE:{
				// 		thiz.argument_require_input();
				// 		thiz.parameter_require_input(0);
				// 		Sen::Kernel::Support::PopCap::ResourceGroup::BasicConversion::merge(thiz.argument, thiz.params.at(0));
				// 		break;
				// 	}
				// 	case Sen::Kernel::Interface::CliCallBack::IMAGE_SCALE: {
				// 		thiz.argument_require_input();
				// 		thiz.parameter_require_input(0);
				// 		thiz.parameter_require_input(1);
				// 		Sen::Kernel::Definition::ImageIO::scale_png(thiz.argument, thiz.params.at(0), std::stof(thiz.params.at(1)));
				// 		break;
				// 	}
				// 	case Sen::Kernel::Interface::CliCallBack::RES_INFO_SPLIT:{
				// 		thiz.argument_require_input();
				// 		thiz.parameter_require_input(0);
				// 		Sen::Kernel::Support::PopCap::ResInfo::BasicConversion::split_fs(thiz.argument, thiz.params.at(0));
				// 		break;
				// 	}
				// 	case Sen::Kernel::Interface::CliCallBack::RES_INFO_MERGE:{
				// 		thiz.argument_require_input();
				// 		thiz.parameter_require_input(0);
				// 		Sen::Kernel::Support::PopCap::ResInfo::BasicConversion::merge_fs(thiz.argument, thiz.params.at(0));
				// 		break;
				// 	}
				// 	case Sen::Kernel::Interface::CliCallBack::RESOURCE_GROUP_TO_RES_INFO:{
				// 		Sen::Kernel::Support::PopCap::ResourceGroup::Convert::convert_fs(thiz.argument, thiz.params.at(0), static_cast<Sen::Kernel::Support::PopCap::ResourceGroup::PathStyle>(std::stoi(thiz.params.at(1))));
				// 		break;
				// 	}
				// 	case Sen::Kernel::Interface::CliCallBack::RES_INFO_TO_RESOURCE_GROUP:{
				// 		Sen::Kernel::Support::PopCap::ResInfo::Convert::convert_fs(thiz.argument, thiz.params.at(0));
				// 		break;
				// 	}
				// 	case Sen::Kernel::Interface::CliCallBack::TEXTURE_DECODE: {
				// 		Sen::Kernel::Support::Texture::InvokeMethod::decode_fs(thiz.argument, thiz.params.at(0), std::stoi(thiz.params.at(1)), std::stoi(thiz.params.at(2)), static_cast<Sen::Kernel::Support::Texture::Format>(std::stoi(thiz.params.at(3))));
				// 		break;
				// 	}
				// 	case Sen::Kernel::Interface::CliCallBack::TEXTURE_ENCODE: {
				// 		Sen::Kernel::Support::Texture::InvokeMethod::encode_fs(thiz.argument, thiz.params.at(0), static_cast<Sen::Kernel::Support::Texture::Format>(std::stoi(thiz.params.at(1))));
				// 		break;
				// 	}
				// 	case Sen::Kernel::Interface::CliCallBack::POPCAP_ZLIB_COMPRESS:{
				// 		thiz.argument_require_input();
				// 		thiz.parameter_require_input(0);
				// 		Sen::Kernel::Support::PopCap::Zlib::Compress::compress_fs(thiz.argument, thiz.params.at(0), static_cast<bool>(std::stoi(thiz.params.at(1))));
				// 		break;
				// 	}
				// 	case Sen::Kernel::Interface::CliCallBack::POPCAP_ZLIB_UNCOMPRESS:{
				// 		thiz.argument_require_input();
				// 		thiz.parameter_require_input(0);
				// 		Sen::Kernel::Support::PopCap::Zlib::Uncompress::uncompress_fs(thiz.argument, thiz.params.at(0), static_cast<bool>(std::stoi(thiz.params.at(1))));
				// 		break;
				// 	}
				// 	case Sen::Kernel::Interface::CliCallBack::ATLAS_UNPACK: {
				// 		auto m_list = List<std::string>{thiz.params};
				// 		m_list.unshift(thiz.argument);
				// 		try_assert(m_list.size() >= 5, fmt::format("argument too few for atlas unpack, expected: m_list.size() >= {}", 5));
				// 		Sen::Kernel::Support::PopCap::Atlas::Unpack::process_fs(m_list, thiz.params.at(thiz.params.size() - 3), thiz.params.at(thiz.params.size() - 2), thiz.params.at(thiz.params.size() - 1));
				// 		break;
				// 	}
				// 	case Sen::Kernel::Interface::CliCallBack::RTON_DECODE:{
				// 		thiz.argument_require_input();
				// 		thiz.parameter_require_input(0);
				// 		Sen::Kernel::Support::PopCap::RTON::Decode::decode_fs(thiz.argument, thiz.params.at(0));
				// 		break;
				// 	}
				// 	case Sen::Kernel::Interface::CliCallBack::RTON_DECRYPT:{
				// 		thiz.argument_require_input();
				// 		thiz.parameter_require_input(0);
				// 		thiz.parameter_require_input(1);
				// 		thiz.parameter_require_input(2);
				// 		Sen::Kernel::Support::PopCap::RTON::Decode::decrypt_fs(thiz.argument, thiz.params.at(0), thiz.params.at(1), thiz.params.at(2));
				// 		break;
				// 	}
				// 	case Sen::Kernel::Interface::CliCallBack::RTON_DECRYPT_AND_DECODE:{
				// 		thiz.argument_require_input();
				// 		thiz.parameter_require_input(0);
				// 		thiz.parameter_require_input(1);
				// 		thiz.parameter_require_input(2);
				// 		Sen::Kernel::Support::PopCap::RTON::Decode::decrypt_and_decode_fs(thiz.argument, thiz.params.at(0), thiz.params.at(1), thiz.params.at(2));
				// 		break;
				// 	}
				// 	case Sen::Kernel::Interface::CliCallBack::NEWTON_DECODE:{
				// 		Sen::Kernel::Support::PopCap::Newton::Decode::process_fs(thiz.argument, thiz.params.at(0));
				// 		break;
				// 	}
				// 	case Sen::Kernel::Interface::CliCallBack::NEWTON_ENCODE:{
				// 		Sen::Kernel::Support::PopCap::Newton::Encode::process_fs(thiz.argument, thiz.params.at(0));
				// 		break;
				// 	}
				// 	case Sen::Kernel::Interface::CliCallBack::COMPILED_TEXT_DECODE:{
				// 		Sen::Kernel::Support::PopCap::CompiledText::Decode::process_fs(thiz.argument, thiz.params.at(0), thiz.params.at(1), thiz.params.at(2), static_cast<bool>(std::stoi(thiz.params.at(3))));
				// 		break;
				// 	}
				// 	default:{
						
				// 		throw std::runtime_error(fmt::format("Method not found: {}", thiz.command));
				// 	}
				// }
				auto const after = Sen::Kernel::Definition::Timer::start();
				auto const time_spent = Sen::Kernel::Definition::Timer::calculate(before, after);
				shell.print(fmt::format("Time spent: {}s", time_spent).c_str(), Sen::Kernel::Interface::Callback::emptyString.c_str(), Sen::Kernel::Interface::Color::GREEN);
				return;
			}
	};

}