#pragma once

#include "kernel/interface/script.hpp"
#include "kernel/interface/version.hpp"

namespace Sen::Kernel::Interface {

	/**
	 * Use JS instead of Sen::Kernel::Definition::JavaScript
	*/

	namespace JS = Sen::Kernel::Definition::JavaScript;

	// callback

	class Callback {

		private:

			std::string argument;

			std::vector<std::string> params;

			Interface::Shell shell;

			Interface::MShellAPI shell_api;

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
				const std::string &argument, 
				const std::vector<std::string> &params,
				const Interface::MShellAPI & shell_api
			) : argument(argument), params(params), shell_api(shell_api)
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
					javascript->add_constant(thiz.shell_api.version, std::string{"Sen"}, std::string{"Shell"}, std::string{"version"});
					javascript->add_constant(thiz.shell_api.is_gui, std::string{"Sen"}, std::string{"Shell"}, std::string{"is_gui"});
				}
				// json
				{
					javascript->add_proxy(Script::JSON::deserialize, std::string{"Sen"}, std::string{"Kernel"}, std::string{"JSON"}, std::string{"deserialize"});
					javascript->add_proxy(Script::JSON::serialize, std::string{"Sen"}, std::string{"Kernel"}, std::string{"JSON"}, std::string{"serialize"});
					javascript->add_proxy(Script::JSON::deserialize_fs, std::string{"Sen"}, std::string{"Kernel"}, std::string{"JSON"}, std::string{"deserialize_fs"});
					javascript->add_proxy(Script::JSON::serialize_fs, std::string{"Sen"}, std::string{"Kernel"}, std::string{"JSON"}, std::string{"serialize_fs"});
				}
				// home
				{
					// script path
					javascript->add_constant(Path::toPosixStyle(script_path), std::string{"Sen"}, std::string{"Kernel"} , std::string{"Home"}, std::string{"script"});
				}
				// vcdiff
				{
					// encode fs
					javascript->add_constant(Script::Diff::VCDiff::encode_fs, std::string{"Sen"}, std::string{"Kernel"} , std::string{"Diff"}, std::string{"VCDiff"}, std::string{"encode_fs"});
					// decode fs
					javascript->add_constant(Script::Diff::VCDiff::decode_fs, std::string{"Sen"}, std::string{"Kernel"} , std::string{"Diff"}, std::string{"VCDiff"}, std::string{"decode_fs"});
				}
				// file system
				{
					// read file
					javascript->add_proxy(Script::FileSystem::read_file, std::string{"Sen"}, std::string{"Kernel"}, std::string{"FileSystem"} ,std::string{"read_file"});
					// read_file_encode_with_utf16le
					javascript->add_proxy(Script::FileSystem::read_file_encode_with_utf16le, std::string{"Sen"}, std::string{"Kernel"}, std::string{"FileSystem"} ,std::string{"read_file_encode_with_utf16le"});
					// write_file_encode_with_utf16le
					javascript->add_proxy(Script::FileSystem::write_file_encode_with_utf16le, std::string{"Sen"}, std::string{"Kernel"}, std::string{"FileSystem"} ,std::string{"write_file_encode_with_utf16le"});
					// write file
					javascript->add_proxy(Script::FileSystem::write_file, std::string{"Sen"}, std::string{"Kernel"}, std::string{"FileSystem"} ,std::string{"write_file"});
					// create directory
					javascript->add_proxy(Script::FileSystem::create_directory, std::string{"Sen"}, std::string{"Kernel"}, std::string{"FileSystem"} ,std::string{"create_directory"});
					// read_current_directory
					javascript->add_proxy(Script::FileSystem::read_current_directory, std::string{"Sen"}, std::string{"Kernel"}, std::string{"FileSystem"} ,std::string{"read_current_directory"});
					// read_directory_only_file
					javascript->add_proxy(Script::FileSystem::read_directory_only_file, std::string{"Sen"}, std::string{"Kernel"}, std::string{"FileSystem"} ,std::string{"read_directory_only_file"});
					// read_directory_only_directory
					javascript->add_proxy(Script::FileSystem::read_directory_only_directory, std::string{"Sen"}, std::string{"Kernel"}, std::string{"FileSystem"} ,std::string{"read_directory_only_directory"});
					// read_directory
					javascript->add_proxy(Script::FileSystem::read_directory, std::string{"Sen"}, std::string{"Kernel"}, std::string{"FileSystem"} ,std::string{"read_directory"});
					// is file
					javascript->add_proxy(Script::FileSystem::is_file, std::string{"Sen"}, std::string{"Kernel"}, std::string{"FileSystem"} ,std::string{"is_file"});
					// is directory
					javascript->add_proxy(Script::FileSystem::is_directory, std::string{"Sen"}, std::string{"Kernel"}, std::string{"FileSystem"} ,std::string{"is_directory"});
					// operation : rename
					javascript->add_proxy(Script::FileSystem::Operation::rename, std::string{"Sen"}, std::string{"Kernel"}, std::string{"FileSystem"}, std::string{"Operation"}, std::string{"rename"});
					// operation : remove
					javascript->add_proxy(Script::FileSystem::Operation::remove, std::string{"Sen"}, std::string{"Kernel"}, std::string{"FileSystem"}, std::string{"Operation"}, std::string{"remove"});
					// operation : copy
					javascript->add_proxy(Script::FileSystem::Operation::copy, std::string{"Sen"}, std::string{"Kernel"}, std::string{"FileSystem"}, std::string{"Operation"}, std::string{"copy"});
				}
				// path
				{
					// join
					javascript->add_proxy(Script::Path::join, std::string{"Sen"}, std::string{"Kernel"}, std::string{"Path"} ,std::string{"join"});
					// basename
					javascript->add_proxy(Script::Path::basename, std::string{"Sen"}, std::string{"Kernel"}, std::string{"Path"} ,std::string{"basename"});
					// delimiter
					javascript->add_proxy(Script::Path::delimiter, std::string{"Sen"}, std::string{"Kernel"}, std::string{"Path"} ,std::string{"delimiter"});
					// dirname
					javascript->add_proxy(Script::Path::dirname, std::string{"Sen"}, std::string{"Kernel"}, std::string{"Path"} ,std::string{"dirname"});
					// format
					javascript->add_proxy(Script::Path::format, std::string{"Sen"}, std::string{"Kernel"}, std::string{"Path"} ,std::string{"format"});
					// normalize
					javascript->add_proxy(Script::Path::normalize, std::string{"Sen"}, std::string{"Kernel"}, std::string{"Path"} ,std::string{"normalize"});
					// resolve
					javascript->add_proxy(Script::Path::resolve, std::string{"Sen"}, std::string{"Kernel"}, std::string{"Path"} ,std::string{"resolve"});
					// relative
					javascript->add_proxy(Script::Path::relative, std::string{"Sen"}, std::string{"Kernel"}, std::string{"Path"} ,std::string{"relative"});
					// extname
					javascript->add_proxy(Script::Path::extname, std::string{"Sen"}, std::string{"Kernel"}, std::string{"Path"} ,std::string{"extname"});
					// is_absolute
					javascript->add_proxy(Script::Path::relative, std::string{"Sen"}, std::string{"Kernel"}, std::string{"Path"} ,std::string{"is_absolute"});
				}
				// console
				{
					// console print
					javascript->add_proxy(Script::Console::print, std::string{"Sen"}, std::string{"Kernel"}, std::string{"Console"} ,std::string{"print"});
					// read line
					javascript->add_proxy(Script::Console::readline, std::string{"Sen"}, std::string{"Kernel"}, std::string{"Console"} ,std::string{"readline"});
				}
				// process
				{
					// run
					javascript->add_proxy(Script::Process::run, std::string{"Sen"}, std::string{"Kernel"}, std::string{"Process"} ,std::string{"run"});
					// execute
					javascript->add_proxy(Script::Process::execute, std::string{"Sen"}, std::string{"Kernel"}, std::string{"Process"} ,std::string{"execute"});
					// is_exists_in_path_environment
					javascript->add_proxy(Script::Process::is_exists_in_path_environment, std::string{"Sen"}, std::string{"Kernel"}, std::string{"Process"} ,std::string{"is_exists_in_path_environment"});
				}
				// thread
				{
					// sleep
					javascript->add_proxy(Script::Thread::sleep, std::string{"Sen"}, std::string{"Kernel"}, std::string{"Thread"} ,std::string{"sleep"});
					// now
					javascript->add_proxy(Script::Thread::now, std::string{"Sen"}, std::string{"Kernel"}, std::string{"Thread"} ,std::string{"now"});
				}
				// array buffer
				{
					// open
					javascript->add_proxy(Script::ArrayBuffer::open, std::string{"Sen"}, std::string{"Kernel"}, std::string{"ArrayBuffer"} ,std::string{"open"});
					// out
					javascript->add_proxy(Script::ArrayBuffer::out, std::string{"Sen"}, std::string{"Kernel"}, std::string{"ArrayBuffer"} ,std::string{"out"});
				}
				// operating system
				{
					// current
					javascript->add_proxy(Script::OperatingSystem::current, std::string{"Sen"}, std::string{"Kernel"}, std::string{"OperatingSystem"}, std::string{"current"});
					// architecture
					javascript->add_proxy(Script::OperatingSystem::architecture, std::string{"Sen"}, std::string{"Kernel"}, std::string{"OperatingSystem"}, std::string{"architecture"});
				}
				// image
				{
					// scale_fs
					javascript->add_proxy(Script::Image::scale_fs, std::string{"Sen"}, std::string{"Kernel"}, std::string{"Image"} ,std::string{"scale_fs"});
					// transparent_fs
					javascript->add_proxy(Script::Image::transparent_fs, std::string{"Sen"}, std::string{"Kernel"}, std::string{"Image"} ,std::string{"transparent_fs"});
					// resize_fs
					javascript->add_proxy(Script::Image::resize_fs, std::string{"Sen"}, std::string{"Kernel"}, std::string{"Image"} ,std::string{"resize_fs"});
					// rotate_fs
					javascript->add_proxy(Script::Image::rotate_fs, std::string{"Sen"}, std::string{"Kernel"}, std::string{"Image"} ,std::string{"rotate_fs"});
				}
				// javascript
				{
					// evaluate
					javascript->add_proxy(Script::JavaScript::evaluate, std::string{"Sen"}, std::string{"Kernel"}, std::string{"JavaScript"} ,std::string{"evaluate"});
					// evaluate file
					javascript->add_proxy(Script::JavaScript::evaluate_fs, std::string{"Sen"}, std::string{"Kernel"}, std::string{"JavaScript"} ,std::string{"evaluate_fs"});
				}
				// md5
				{
					// hash method
					javascript->add_proxy(Script::Encryption::MD5::hash, std::string{"Sen"}, std::string{"Kernel"}, std::string{"Encryption"}, std::string{"MD5"}, std::string{"hash"});
					// hash file method
					javascript->add_proxy(Script::Encryption::MD5::hash_fs, std::string{"Sen"}, std::string{"Kernel"}, std::string{"Encryption"}, std::string{"MD5"}, std::string{"hash_fs"});
				}
				// sha224
				{
					// hash method
					javascript->add_proxy(Script::Encryption::SHA224::hash, std::string{"Sen"}, std::string{"Kernel"}, std::string{"Encryption"}, std::string{"SHA224"}, std::string{"hash"});
					// hash file method
					javascript->add_proxy(Script::Encryption::SHA224::hash_fs, std::string{"Sen"}, std::string{"Kernel"}, std::string{"Encryption"}, std::string{"SHA224"}, std::string{"hash_fs"});
				}
				// sha256
				{
					// hash method
					javascript->add_proxy(Script::Encryption::SHA256::hash, std::string{"Sen"}, std::string{"Kernel"}, std::string{"Encryption"}, std::string{"SHA256"}, std::string{"hash"});
					// hash file method
					javascript->add_proxy(Script::Encryption::SHA224::hash_fs, std::string{"Sen"}, std::string{"Kernel"}, std::string{"Encryption"}, std::string{"SHA256"}, std::string{"hash_fs"});
				}
				// sha384
				{
					// hash method
					javascript->add_proxy(Script::Encryption::SHA384::hash, std::string{"Sen"}, std::string{"Kernel"}, std::string{"Encryption"}, std::string{"SHA384"}, std::string{"hash"});
					// hash file method
					javascript->add_proxy(Script::Encryption::SHA384::hash_fs, std::string{"Sen"}, std::string{"Kernel"}, std::string{"Encryption"}, std::string{"SHA384"}, std::string{"hash_fs"});
				}
				// sha512
				{
					// hash method
					javascript->add_proxy(Script::Encryption::SHA512::hash, std::string{"Sen"}, std::string{"Kernel"}, std::string{"Encryption"}, std::string{"SHA512"}, std::string{"hash"});
					// hash file method
					javascript->add_proxy(Script::Encryption::SHA512::hash_fs, std::string{"Sen"}, std::string{"Kernel"}, std::string{"Encryption"}, std::string{"SHA512"}, std::string{"hash_fs"});
				}
				// xor
				{
					// encrypt method
					javascript->add_proxy(Script::Encryption::XOR::encrypt, std::string{"Sen"}, std::string{"Kernel"}, std::string{"Encryption"}, std::string{"X0R"}, std::string{"encrypt"});
					// encrypt file method
					javascript->add_proxy(Script::Encryption::XOR::encrypt_fs, std::string{"Sen"}, std::string{"Kernel"}, std::string{"Encryption"}, std::string{"X0R"}, std::string{"encrypt_fs"});
				}
				// zlib
				{
					// compress file method
					javascript->add_proxy(Script::Compression::Zlib::compress_fs, std::string{"Sen"}, std::string{"Kernel"}, std::string{"Compression"}, std::string{"Zlib"}, std::string{"compress_fs"});
					// uncompress file method
					javascript->add_proxy(Script::Compression::Zlib::uncompress_fs, std::string{"Sen"}, std::string{"Kernel"}, std::string{"Compression"}, std::string{"Zlib"}, std::string{"uncompress_fs"});
				}
				// gzip
				{
					// compress file method
					javascript->add_proxy(Script::Compression::Gzip::compress_fs, std::string{"Sen"}, std::string{"Kernel"}, std::string{"Compression"}, std::string{"Gzip"}, std::string{"compress_fs"});
					// uncompress file method
					javascript->add_proxy(Script::Compression::Gzip::uncompress_fs, std::string{"Sen"}, std::string{"Kernel"}, std::string{"Compression"}, std::string{"Gzip"}, std::string{"uncompress_fs"});
				}
				// bzip2
				{
					// compress file method
					javascript->add_proxy(Script::Compression::Bzip2::compress_fs, std::string{"Sen"}, std::string{"Kernel"}, std::string{"Compression"}, std::string{"Bzip2"}, std::string{"compress_fs"});
					// uncompress file method
					javascript->add_proxy(Script::Compression::Bzip2::uncompress_fs, std::string{"Sen"}, std::string{"Kernel"}, std::string{"Compression"}, std::string{"Bzip2"}, std::string{"uncompress_fs"});
				}
				// lzma
				{
					// compress file method
					javascript->add_proxy(Script::Compression::Lzma::compress_fs, std::string{"Sen"}, std::string{"Kernel"}, std::string{"Compression"}, std::string{"Lzma"}, std::string{"compress_fs"});
					// uncompress file method
					javascript->add_proxy(Script::Compression::Lzma::uncompress_fs, std::string{"Sen"}, std::string{"Kernel"}, std::string{"Compression"}, std::string{"Lzma"}, std::string{"uncompress_fs"});
				}
				// base64
				{
					// encode base64
					javascript->add_proxy(Script::Encryption::Base64::encode, std::string{"Sen"}, std::string{"Kernel"}, std::string{"Encryption"}, std::string{"Base64"}, std::string{"encode"});
					// decode base64
					javascript->add_proxy(Script::Encryption::Base64::decode, std::string{"Sen"}, std::string{"Kernel"}, std::string{"Encryption"}, std::string{"Base64"}, std::string{"decode"});
					// encode base64 for file
					javascript->add_proxy(Script::Encryption::Base64::encode_fs, std::string{"Sen"}, std::string{"Kernel"}, std::string{"Encryption"}, std::string{"Base64"}, std::string{"encode_fs"});
					// decode base64 for file
					javascript->add_proxy(Script::Encryption::Base64::decode_fs, std::string{"Sen"}, std::string{"Kernel"}, std::string{"Encryption"}, std::string{"Base64"}, std::string{"decode_fs"});
				}
				// texture
				{
					javascript->add_proxy(Script::Support::Texture::decode_fs, std::string{"Sen"}, std::string{"Kernel"}, std::string{"Support"}, std::string{"Texture"},std::string{"decode_fs"});
					javascript->add_proxy(Script::Support::Texture::encode_fs, std::string{"Sen"}, std::string{"Kernel"}, std::string{"Support"}, std::string{"Texture"}, std::string{"encode_fs"});
				}
				// newton
				{
					javascript->add_proxy(Script::Support::PopCap::Newton::decode_fs, std::string{"Sen"}, std::string{"Kernel"}, std::string{"Support"}, std::string{"PopCap"}, std::string{"Newton"}, std::string{"decode_fs"});
					javascript->add_proxy(Script::Support::PopCap::Newton::encode_fs, std::string{"Sen"}, std::string{"Kernel"}, std::string{"Support"}, std::string{"PopCap"}, std::string{"Newton"}, std::string{"encode_fs"});
				}
				// rton
				{
					javascript->add_proxy(Script::Support::PopCap::RTON::decode_fs, std::string{"Sen"}, std::string{"Kernel"}, std::string{"Support"}, std::string{"PopCap"}, std::string{"RTON"}, std::string{"decode_fs"});
					javascript->add_proxy(Script::Support::PopCap::RTON::decode_fs_as_multiple_threads, std::string{"Sen"}, std::string{"Kernel"}, std::string{"Support"}, std::string{"PopCap"}, std::string{"RTON"}, std::string{"decode_fs_as_multiple_threads"});
					javascript->add_proxy(Script::Support::PopCap::RTON::encode_fs, std::string{"Sen"}, std::string{"Kernel"}, std::string{"Support"}, std::string{"PopCap"}, std::string{"RTON"}, std::string{"encode_fs"});
				}
				// zlib
				{
					javascript->add_proxy(Script::Support::PopCap::Zlib::uncompress_fs, std::string{"Sen"}, std::string{"Kernel"}, std::string{"Support"}, std::string{"PopCap"}, std::string{"Zlib"}, std::string{"uncompress_fs"});
					javascript->add_proxy(Script::Support::PopCap::Zlib::compress_fs, std::string{"Sen"}, std::string{"Kernel"}, std::string{"Support"}, std::string{"PopCap"}, std::string{"Zlib"}, std::string{"compress_fs"});
				}
				// compiled text
				{
					javascript->add_proxy(Script::Support::PopCap::CompiledText::decode_fs, std::string{"Sen"}, std::string{"Kernel"}, std::string{"Support"}, std::string{"PopCap"}, std::string{"CompiledText"}, std::string{"decode_fs"});
					javascript->add_proxy(Script::Support::PopCap::CompiledText::encode_fs, std::string{"Sen"}, std::string{"Kernel"}, std::string{"Support"}, std::string{"PopCap"}, std::string{"CompiledText"}, std::string{"encode_fs"});
				}
				// render-effects
				{
					// decode fs
					javascript->add_proxy(Script::Support::PopCap::RenderEffects::decode_fs, std::string{"Sen"}, std::string{"Kernel"}, std::string{"Support"}, std::string{"PopCap"}, std::string{"RenderEffects"}, std::string{"decode_fs"});
					// encode fs
					javascript->add_proxy(Script::Support::PopCap::RenderEffects::encode_fs, std::string{"Sen"}, std::string{"Kernel"}, std::string{"Support"}, std::string{"PopCap"}, std::string{"RenderEffects"}, std::string{"encode_fs"});
				}
				// crypt-data
				{
					// decrypt fs
					javascript->add_proxy(Script::Support::PopCap::CryptData::decrypt_fs, std::string{"Sen"}, std::string{"Kernel"}, std::string{"Support"}, std::string{"PopCap"}, std::string{"CryptData"}, std::string{"decrypt_fs"});
					// encrypt fs
					javascript->add_proxy(Script::Support::PopCap::CryptData::encrypt_fs, std::string{"Sen"}, std::string{"Kernel"}, std::string{"Support"}, std::string{"PopCap"}, std::string{"CryptData"}, std::string{"encrypt_fs"});
				}
				// character-font-widget-2
				{
					// decode fs
					javascript->add_proxy(Script::Support::PopCap::CharacterFontWidget2::decode_fs, std::string{"Sen"}, std::string{"Kernel"}, std::string{"Support"}, std::string{"PopCap"}, std::string{"CharacterFontWidget2"}, std::string{"decode_fs"});
					// encode fs
					javascript->add_proxy(Script::Support::PopCap::CharacterFontWidget2::encode_fs, std::string{"Sen"}, std::string{"Kernel"}, std::string{"Support"}, std::string{"PopCap"}, std::string{"CharacterFontWidget2"}, std::string{"encode_fs"});
				}
				// rsb
				{
					javascript->add_proxy(Script::Support::PopCap::RSB::unpack_fs, std::string{"Sen"}, std::string{"Kernel"}, std::string{"Support"}, std::string{"PopCap"}, std::string{"RSB"}, std::string{"unpack_fs"});
					javascript->add_proxy(Script::Support::PopCap::RSB::pack_fs, std::string{"Sen"}, std::string{"Kernel"}, std::string{"Support"}, std::string{"PopCap"}, std::string{"RSB"}, std::string{"pack_fs"});
				}
				// rsg
				{
					javascript->add_proxy(Script::Support::PopCap::RSG::unpack_fs, std::string{"Sen"}, std::string{"Kernel"}, std::string{"Support"}, std::string{"PopCap"}, std::string{"RSG"}, std::string{"unpack_fs"});
					javascript->add_proxy(Script::Support::PopCap::RSG::pack_fs, std::string{"Sen"}, std::string{"Kernel"}, std::string{"Support"}, std::string{"PopCap"}, std::string{"RSG"}, std::string{"pack_fs"});
				}
				// pam
				{
					javascript->add_proxy(Script::Support::PopCap::Animation::decode_fs, std::string{"Sen"}, std::string{"Kernel"}, std::string{"Support"}, std::string{"PopCap"}, std::string{"Animation"}, std::string{"decode_fs"});
					javascript->add_proxy(Script::Support::PopCap::Animation::encode_fs, std::string{"Sen"}, std::string{"Kernel"}, std::string{"Support"}, std::string{"PopCap"}, std::string{"Animation"}, std::string{"encode_fs"});
				}
				// bnk
				{
					javascript->add_proxy(Script::Support::WWise::SoundBank::decode_fs, std::string{"Sen"}, std::string{"Kernel"}, std::string{"Support"}, std::string{"WWise"}, std::string{"SoundBank"}, std::string{"decode_fs"});
					javascript->add_proxy(Script::Support::WWise::SoundBank::encode_fs, std::string{"Sen"}, std::string{"Kernel"}, std::string{"Support"}, std::string{"WWise"}, std::string{"SoundBank"}, std::string{"encode_fs"});
				}
				// resource group
				{
					javascript->add_proxy(Script::Support::PopCap::ResourceGroup::split_fs, std::string{"Sen"}, std::string{"Kernel"}, std::string{"Support"}, std::string{"PopCap"}, std::string{"ResourceGroup"}, std::string{"split_fs"});
					javascript->add_proxy(Script::Support::PopCap::ResourceGroup::merge_fs, std::string{"Sen"}, std::string{"Kernel"}, std::string{"Support"}, std::string{"PopCap"}, std::string{"ResourceGroup"}, std::string{"merge_fs"});
					javascript->add_proxy(Script::Support::PopCap::ResourceGroup::convert_fs, std::string{"Sen"}, std::string{"Kernel"}, std::string{"Support"}, std::string{"PopCap"}, std::string{"ResourceGroup"}, std::string{"convert_fs"});
				}
				// res info
				{
					javascript->add_proxy(Script::Support::PopCap::ResInfo::split_fs, std::string{"Sen"}, std::string{"Kernel"}, std::string{"Support"}, std::string{"PopCap"}, std::string{"ResInfo"}, std::string{"split_fs"});
					javascript->add_proxy(Script::Support::PopCap::ResInfo::merge_fs, std::string{"Sen"}, std::string{"Kernel"}, std::string{"Support"}, std::string{"PopCap"}, std::string{"ResInfo"}, std::string{"merge_fs"});
					javascript->add_proxy(Script::Support::PopCap::ResInfo::convert_fs, std::string{"Sen"}, std::string{"Kernel"}, std::string{"Support"}, std::string{"PopCap"}, std::string{"ResInfo"}, std::string{"convert_fs"});
				}
				javascript->evaluate_fs(script_path);
				javascript->evaluate(std::string{"Sen.Script.main()"}, "<script>");
				return;
			}
	};

}