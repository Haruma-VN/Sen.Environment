#pragma once

#include "kernel/interface/script.hpp"
#include "kernel/interface/version.hpp"

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
				shell.print(fmt::format("{}\n  ", title).c_str(), color);
				shell.print(fmt::format("{}\n  ", message).c_str(), Interface::Color::DEFAULT);
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
			 * Execute method
			*/

			auto execute(

			) -> void
			{
				auto javascript = std::make_shared<JS::Runtime>();
				auto script_path = thiz.argument;
				// kernel version
				{
					javascript->add_constant(Kernel::version, std::string{"Sen"}, std::string{"Kernel"}, std::string{"version"});
				}
				// home
				{
					javascript->add_constant(Path::getParents(script_path), std::string{"Sen"}, std::string{"Kernel"} , std::string{"Home"}, std::string{"script_parent"});
				}
				// file system
				{
					// read file
					javascript->add_proxy(Script::read_file, std::string{"Sen"}, std::string{"Kernel"}, std::string{"FileSystem"} ,std::string{"read_file"});
					// write file
					javascript->add_proxy(Script::write_file, std::string{"Sen"}, std::string{"Kernel"}, std::string{"FileSystem"} ,std::string{"write_file"});
				}
				// console
				{
					// console log with color
					javascript->add_proxy(Script::print, std::string{"Sen"}, std::string{"Kernel"}, std::string{"Console"} ,std::string{"print"});
					// console log
					javascript->add_proxy(Script::readline, std::string{"Sen"}, std::string{"Kernel"}, std::string{"Console"} ,std::string{"readline"});
				}
				// javascript
				{
					// evaluate
					javascript->add_proxy(Script::evaluate, std::string{"Sen"}, std::string{"Kernel"}, std::string{"JavaScript"} ,std::string{"evaluate"});
					// evaluate file
					javascript->add_proxy(Script::evaluate_fs, std::string{"Sen"}, std::string{"Kernel"}, std::string{"JavaScript"} ,std::string{"evaluate_fs"});
				}
				// md5
				{
					// hash method
					javascript->add_proxy(Script::md5_hash, std::string{"Sen"}, std::string{"Kernel"}, std::string{"Encryption"}, std::string{"MD5"}, std::string{"hash"});
					// hash file method
					javascript->add_proxy(Script::md5_hash_fs, std::string{"Sen"}, std::string{"Kernel"}, std::string{"Encryption"}, std::string{"MD5"}, std::string{"hash_fs"});
				}
				// sha224
				{
					// hash method
					javascript->add_proxy(Script::sha224_hash, std::string{"Sen"}, std::string{"Kernel"}, std::string{"Encryption"}, std::string{"SHA224"}, std::string{"hash"});
					// hash file method
					javascript->add_proxy(Script::sha224_hash_fs, std::string{"Sen"}, std::string{"Kernel"}, std::string{"Encryption"}, std::string{"SHA224"}, std::string{"hash_fs"});
				}
				// sha256
				{
					// hash method
					javascript->add_proxy(Script::sha256_hash, std::string{"Sen"}, std::string{"Kernel"}, std::string{"Encryption"}, std::string{"SHA256"}, std::string{"hash"});
					// hash file method
					javascript->add_proxy(Script::sha256_hash_fs, std::string{"Sen"}, std::string{"Kernel"}, std::string{"Encryption"}, std::string{"SHA256"}, std::string{"hash_fs"});
				}
				// sha384
				{
					// hash method
					javascript->add_proxy(Script::sha384_hash, std::string{"Sen"}, std::string{"Kernel"}, std::string{"Encryption"}, std::string{"SHA384"}, std::string{"hash"});
					// hash file method
					javascript->add_proxy(Script::sha384_hash_fs, std::string{"Sen"}, std::string{"Kernel"}, std::string{"Encryption"}, std::string{"SHA384"}, std::string{"hash_fs"});
				}
				// sha512
				{
					// hash method
					javascript->add_proxy(Script::sha512_hash, std::string{"Sen"}, std::string{"Kernel"}, std::string{"Encryption"}, std::string{"SHA512"}, std::string{"hash"});
					// hash file method
					javascript->add_proxy(Script::sha512_hash_fs, std::string{"Sen"}, std::string{"Kernel"}, std::string{"Encryption"}, std::string{"SHA512"}, std::string{"hash_fs"});
				}
				// xor
				{
					// encrypt method
					javascript->add_proxy(Script::xor_encrypt, std::string{"Sen"}, std::string{"Kernel"}, std::string{"Encryption"}, std::string{"X0R"}, std::string{"encrypt"});
					// encrypt file method
					javascript->add_proxy(Script::xor_encrypt_fs, std::string{"Sen"}, std::string{"Kernel"}, std::string{"Encryption"}, std::string{"X0R"}, std::string{"encrypt_fs"});
				}
				// zlib
				{
					// compress file method
					javascript->add_proxy(Script::zlib_compress_fs, std::string{"Sen"}, std::string{"Kernel"}, std::string{"Compression"}, std::string{"Zlib"}, std::string{"compress_fs"});
					// uncompress file method
					javascript->add_proxy(Script::zlib_uncompress_fs, std::string{"Sen"}, std::string{"Kernel"}, std::string{"Compression"}, std::string{"Zlib"}, std::string{"uncompress_fs"});
				}
				// gzip
				{
					// compress file method
					javascript->add_proxy(Script::gzip_compress_fs, std::string{"Sen"}, std::string{"Kernel"}, std::string{"Compression"}, std::string{"Gzip"}, std::string{"compress_fs"});
					// uncompress file method
					javascript->add_proxy(Script::gzip_uncompress_fs, std::string{"Sen"}, std::string{"Kernel"}, std::string{"Compression"}, std::string{"Gzip"}, std::string{"uncompress_fs"});
				}
				// base64
				{
					// encode base64
					javascript->add_proxy(Script::base64_encode, std::string{"Sen"}, std::string{"Kernel"}, std::string{"Encryption"}, std::string{"Base64"}, std::string{"encode"});
					// decode base64
					javascript->add_proxy(Script::base64_decode, std::string{"Sen"}, std::string{"Kernel"}, std::string{"Encryption"}, std::string{"Base64"}, std::string{"decode"});
					// encode base64 for file
					javascript->add_proxy(Script::base64_encode_fs, std::string{"Sen"}, std::string{"Kernel"}, std::string{"Encryption"}, std::string{"Base64"}, std::string{"encode_fs"});
					// decode base64 for file
					javascript->add_proxy(Script::base64_decode_fs, std::string{"Sen"}, std::string{"Kernel"}, std::string{"Encryption"}, std::string{"Base64"}, std::string{"decode_fs"});
				}
				{
					javascript->add_proxy(Script::rton_decode_fs, std::string{"Sen"}, std::string{"Kernel"}, std::string{"Support"}, std::string{"PopCap"}, std::string{"RTON"}, std::string{"decode_fs"});
				}
				javascript->evaluate_fs(script_path);
				javascript->evaluate(std::string{"Sen.Script.main()"}, "<script>");
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
				return;
			}
	};

}