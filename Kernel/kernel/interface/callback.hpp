#pragma once

#include "kernel/interface/script.hpp"
#include "kernel/interface/version.hpp"
#include "kernel/interface/shell.hpp"

namespace Sen::Kernel::Interface {

	/**
	 * Use JS instead of Sen::Kernel::Definition::JavaScript
	*/

	namespace JS = Sen::Kernel::Definition::JavaScript;

	// callback

	class Callback {

		private:

			std::string_view argument;

			ShellCallback callback;

			std::vector<std::string> arguments;

		public:

			explicit Callback(
				std::string_view argument,
				ShellCallback callback,
				const std::vector<std::string> & arguments
			) : argument(argument), callback(callback), arguments(std::move(arguments))
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

			inline auto execute(

			) -> void
			{
				auto javascript = std::unique_ptr<JS::Runtime>(new JS::Runtime());
				auto script_path = thiz.argument;
				// shell callback
				{
					auto is_gui = thiz.callback(construct_string_list(std::vector<std::string>{std::string{"is_gui"}}));
					auto shell_version = thiz.callback(construct_string_list(std::vector<std::string>{std::string{"version"}}));
					javascript->add_constant<int>(Kernel::version, "Sen", "Kernel", "version");
					javascript->add_constant<int>(static_cast<int>(std::stoi(std::string{shell_version.value, shell_version.size})), "Sen", "Shell", "version");
					javascript->add_constant<bool>(static_cast<bool>(std::stoi(std::string{is_gui.value, is_gui.size})), "Sen", "Shell", "is_gui");
					javascript->add_proxy(Script::callback, "Sen", "Shell", "callback");
					javascript->add_constant(thiz.arguments, "Sen", "Kernel", "arguments");
					// script path
					javascript->add_constant<int>(std::vector<int>{1,2,3,4,5}, "Sen", "Kernel", "Array", "Test", "X", "Y", "Z");
				}
				// xml
				{
					// deserialize
					javascript->add_proxy(Script::XML::deserialize, "Sen", "Kernel", "XML", "deserialize");
					// deserialize_fs
					javascript->add_proxy(Script::XML::deserialize_fs, "Sen", "Kernel", "XML", "deserialize_fs");
					// serialize
					javascript->add_proxy(Script::XML::serialize, "Sen", "Kernel", "XML", "serialize");
					// serialize_fs
					javascript->add_proxy(Script::XML::serialize_fs, "Sen", "Kernel", "XML", "serialize_fs");
				}
				// json
				{
					// deserialize
					javascript->add_proxy(Script::JSON::deserialize, "Sen", "Kernel", "JSON", "deserialize");
					// serialize
					javascript->add_proxy(Script::JSON::serialize, "Sen", "Kernel", "JSON", "serialize");
					// deserialize fs
					javascript->add_proxy(Script::JSON::deserialize_fs, "Sen", "Kernel", "JSON", "deserialize_fs");
					// serialize fs
					javascript->add_proxy(Script::JSON::serialize_fs, "Sen", "Kernel", "JSON", "serialize_fs");
				}
				// home
				{
					// script path
					javascript->add_constant<std::string_view>(String::toPosixStyle(script_path.data()), "Sen", "Kernel", "Home", "script");
				}
				// vcdiff
				{
					// encode fs
					javascript->add_proxy(Script::Diff::VCDiff::encode_fs, "Sen", "Kernel", "Diff", "VCDiff", "encode_fs");
					// decode fs
					javascript->add_proxy(Script::Diff::VCDiff::decode_fs, "Sen", "Kernel", "Diff", "VCDiff", "decode_fs");
				}
				// file system
				{
					// read file
					javascript->add_proxy(Script::FileSystem::read_file, "Sen", "Kernel", "FileSystem", "read_file");
					// read_file_encode_with_utf16le
					javascript->add_proxy(Script::FileSystem::read_file_encode_with_utf16le, "Sen", "Kernel", "FileSystem", "read_file_encode_with_utf16le");
					// write_file_encode_with_utf16le
					javascript->add_proxy(Script::FileSystem::write_file_encode_with_utf16le, "Sen", "Kernel", "FileSystem", "write_file_encode_with_utf16le");
					// write file
					javascript->add_proxy(Script::FileSystem::write_file, "Sen", "Kernel", "FileSystem", "write_file");
					// create directory
					javascript->add_proxy(Script::FileSystem::create_directory, "Sen", "Kernel", "FileSystem", "create_directory");
					// read_current_directory
					javascript->add_proxy(Script::FileSystem::read_current_directory, "Sen", "Kernel", "FileSystem", "read_current_directory");
					// read_directory_only_file
					javascript->add_proxy(Script::FileSystem::read_directory_only_file, "Sen", "Kernel", "FileSystem", "read_directory_only_file");
					// read_directory_only_directory
					javascript->add_proxy(Script::FileSystem::read_directory_only_directory, "Sen", "Kernel", "FileSystem", "read_directory_only_directory");
					// read_directory
					javascript->add_proxy(Script::FileSystem::read_directory, "Sen", "Kernel", "FileSystem", "read_directory");
					// is file
					javascript->add_proxy(Script::FileSystem::is_file, "Sen", "Kernel", "FileSystem", "is_file");
					// is directory
					javascript->add_proxy(Script::FileSystem::is_directory, "Sen", "Kernel", "FileSystem","is_directory");
					// operation : rename
					javascript->add_proxy(Script::FileSystem::Operation::rename, "Sen", "Kernel", "FileSystem", "Operation", "rename");
					// operation : remove
					javascript->add_proxy(Script::FileSystem::Operation::remove, "Sen", "Kernel", "FileSystem", "Operation", "remove");
					// operation : copy
					javascript->add_proxy(Script::FileSystem::Operation::copy, "Sen", "Kernel", "FileSystem", "Operation", "copy");
				}
				// path
				{
					// join
					javascript->add_proxy(Script::Path::join, "Sen", "Kernel", "Path", "join");
					// basename
					javascript->add_proxy(Script::Path::basename, "Sen", "Kernel", "Path", "basename");
					// delimiter
					javascript->add_proxy(Script::Path::delimiter, "Sen", "Kernel", "Path", "delimiter");
					// dirname
					javascript->add_proxy(Script::Path::dirname, "Sen", "Kernel", "Path", "dirname");
					// format
					javascript->add_proxy(Script::Path::format, "Sen", "Kernel", "Path", "format");
					// normalize
					javascript->add_proxy(Script::Path::normalize, "Sen", "Kernel", "Path", "normalize");
					// resolve
					javascript->add_proxy(Script::Path::resolve, "Sen", "Kernel", "Path", "resolve");
					// relative
					javascript->add_proxy(Script::Path::relative, "Sen", "Kernel", "Path", "relative");
					// extname
					javascript->add_proxy(Script::Path::extname, "Sen", "Kernel", "Path", "extname");
					// is_absolute
					javascript->add_proxy(Script::Path::relative, "Sen", "Kernel", "Path", "is_absolute");
					// base_without_extension
					javascript->add_proxy(Script::Path::base_without_extension, "Sen", "Kernel", "Path", "base_without_extension");
					// except_extension
					javascript->add_proxy(Script::Path::except_extension, "Sen", "Kernel", "Path", "except_extension");
				}
				// console
				{
					// console print
					javascript->add_proxy(Script::Console::print, "Sen", "Kernel", "Console" ,"print");
					// read line
					javascript->add_proxy(Script::Console::readline, "Sen", "Kernel", "Console" ,"readline");
				}
				// language
				{
					// load language
					javascript->add_proxy(Script::Language::load_language, "Sen", "Kernel", "Language" ,"load_language");
					// get language
					javascript->add_proxy(Script::Language::get, "Sen", "Kernel", "Language" ,"get");
				}
				// process
				{
					// run
					javascript->add_proxy(Script::Process::run, "Sen", "Kernel", "Process" ,"run");
					// execute
					javascript->add_proxy(Script::Process::execute, "Sen", "Kernel", "Process" ,"execute");
					// is_exists_in_path_environment
					javascript->add_proxy(Script::Process::is_exists_in_path_environment, "Sen", "Kernel", "Process" ,"is_exists_in_path_environment");
				}
				// thread
				{
					// sleep
					javascript->add_proxy(Script::Thread::sleep, "Sen", "Kernel", "Thread", "sleep");
					// now
					javascript->add_proxy(Script::Thread::now, "Sen", "Kernel", "Thread", "now");
				}
				// array buffer
				{
					// open
					javascript->add_proxy(Script::ArrayBuffer::open, "Sen", "Kernel", "ArrayBuffer","open");
					// out
					javascript->add_proxy(Script::ArrayBuffer::out, "Sen", "Kernel", "ArrayBuffer","out");
				}
				// operating system
				{
					// current
					javascript->add_proxy(Script::OperatingSystem::current, "Sen", "Kernel", "OperatingSystem", "current");
					// architecture
					javascript->add_proxy(Script::OperatingSystem::architecture, "Sen", "Kernel", "OperatingSystem", "architecture");
				}
				// image
				{
					// scale_fs
					javascript->add_proxy(Script::Image::scale_fs, "Sen", "Kernel", "Image", "scale_fs");
					// transparent_fs
					javascript->add_proxy(Script::Image::transparent_fs, "Sen", "Kernel", "Image", "transparent_fs");
					// resize_fs
					javascript->add_proxy(Script::Image::resize_fs, "Sen", "Kernel", "Image", "resize_fs");
					// rotate_fs
					javascript->add_proxy(Script::Image::rotate_fs, "Sen", "Kernel", "Image", "rotate_fs");
					// composite_fs
					javascript->add_proxy(Script::Image::composite_fs, "Sen", "Kernel", "Image", "composite_fs");
					// composite_multiple_fs
					javascript->add_proxy(Script::Image::composite_multiple_fs, "Sen", "Kernel", "Image", "composite_multiple_fs");
					// composite_multiple_fs_asynchronous
					javascript->add_proxy(Script::Image::composite_multiple_fs_asynchronous, "Sen", "Kernel", "Image", "composite_multiple_fs_asynchronous");
				}
				// javascript
				{
					// evaluate
					javascript->add_proxy(Script::JavaScript::evaluate, "Sen", "Kernel", "JavaScript", "evaluate");
					// evaluate file
					javascript->add_proxy(Script::JavaScript::evaluate_fs, "Sen", "Kernel", "JavaScript", "evaluate_fs");
				}
				// md5
				{
					// hash method
					javascript->add_proxy(Script::Encryption::MD5::hash, "Sen", "Kernel", "Encryption", "MD5", "hash");
					// hash file method
					javascript->add_proxy(Script::Encryption::MD5::hash_fs, "Sen", "Kernel", "Encryption", "MD5", "hash_fs");
				}
				// sha224
				{
					// hash method
					javascript->add_proxy(Script::Encryption::SHA224::hash, "Sen", "Kernel", "Encryption", "SHA224", "hash");
					// hash file method
					javascript->add_proxy(Script::Encryption::SHA224::hash_fs, "Sen", "Kernel", "Encryption", "SHA224", "hash_fs");
				}
				// sha256
				{
					// hash method
					javascript->add_proxy(Script::Encryption::SHA256::hash, "Sen", "Kernel", "Encryption", "SHA256", "hash");
					// hash file method
					javascript->add_proxy(Script::Encryption::SHA224::hash_fs, "Sen", "Kernel", "Encryption", "SHA256", "hash_fs");
				}
				// sha384
				{
					// hash method
					javascript->add_proxy(Script::Encryption::SHA384::hash, "Sen", "Kernel", "Encryption", "SHA384", "hash");
					// hash file method
					javascript->add_proxy(Script::Encryption::SHA384::hash_fs, "Sen", "Kernel", "Encryption", "SHA384", "hash_fs");
				}
				// sha512
				{
					// hash method
					javascript->add_proxy(Script::Encryption::SHA512::hash, "Sen", "Kernel", "Encryption", "SHA512", "hash");
					// hash file method
					javascript->add_proxy(Script::Encryption::SHA512::hash_fs, "Sen", "Kernel", "Encryption", "SHA512", "hash_fs");
				}
				// xor
				{
					// encrypt method
					javascript->add_proxy(Script::Encryption::XOR::encrypt, "Sen", "Kernel", "Encryption", "X0R", "encrypt");
					// encrypt file method
					javascript->add_proxy(Script::Encryption::XOR::encrypt_fs, "Sen", "Kernel", "Encryption", "X0R", "encrypt_fs");
				}
				// zip
				{
					// compress directory
					javascript->add_proxy(Script::Compression::Zip::Compress::directory, "Sen", "Kernel", "Compression", "Zip", "Compress", "directory");
					// compress files
					javascript->add_proxy(Script::Compression::Zip::Compress::file, "Sen", "Kernel", "Compression", "Zip", "Compress", "file");
					// uncompress
					javascript->add_proxy(Script::Compression::Zip::Uncompress::process, "Sen", "Kernel", "Compression", "Zip", "Uncompress", "process");
				}
				// zlib
				{
					// compress file method
					javascript->add_proxy(Script::Compression::Zlib::compress_fs, "Sen", "Kernel", "Compression", "Zlib", "compress_fs");
					// uncompress file method
					javascript->add_proxy(Script::Compression::Zlib::uncompress_fs, "Sen", "Kernel", "Compression", "Zlib", "uncompress_fs");
				}
				// gzip
				{
					// compress file method
					javascript->add_proxy(Script::Compression::Gzip::compress_fs, "Sen", "Kernel", "Compression", "Gzip", "compress_fs");
					// uncompress file method
					javascript->add_proxy(Script::Compression::Gzip::uncompress_fs, "Sen", "Kernel", "Compression", "Gzip", "uncompress_fs");
				}
				// bzip2
				{
					// compress file method
					javascript->add_proxy(Script::Compression::Bzip2::compress_fs, "Sen", "Kernel", "Compression", "Bzip2", "compress_fs");
					// uncompress file method
					javascript->add_proxy(Script::Compression::Bzip2::uncompress_fs, "Sen", "Kernel", "Compression", "Bzip2", "uncompress_fs");
				}
				// lzma
				{
					// compress file method
					javascript->add_proxy(Script::Compression::Lzma::compress_fs, "Sen", "Kernel", "Compression", "Lzma", "compress_fs");
					// uncompress file method
					javascript->add_proxy(Script::Compression::Lzma::uncompress_fs, "Sen", "Kernel", "Compression", "Lzma", "uncompress_fs");
				}
				// base64
				{
					// encode base64
					javascript->add_proxy(Script::Encryption::Base64::encode, "Sen", "Kernel", "Encryption", "Base64", "encode");
					// decode base64
					javascript->add_proxy(Script::Encryption::Base64::decode, "Sen", "Kernel", "Encryption", "Base64", "decode");
					// encode base64 for file
					javascript->add_proxy(Script::Encryption::Base64::encode_fs, "Sen", "Kernel", "Encryption", "Base64", "encode_fs");
					// decode base64 for file
					javascript->add_proxy(Script::Encryption::Base64::decode_fs, "Sen", "Kernel", "Encryption", "Base64", "decode_fs");
					// encode base64 for file as thread
					javascript->add_proxy(Script::Encryption::Base64::encode_fs_as_multiple_threads, "Sen", "Kernel", "Encryption", "Base64", "encode_fs_as_multiple_thread");
					// decode base64 for file as thread
					javascript->add_proxy(Script::Encryption::Base64::decode_fs_as_multiple_threads, "Sen", "Kernel", "Encryption", "Base64", "decode_fs_as_multiple_threads");
				}
				// texture
				{
					javascript->add_proxy(Script::Support::Texture::decode_fs, "Sen", "Kernel", "Support", "Texture","decode_fs");
					javascript->add_proxy(Script::Support::Texture::encode_fs, "Sen", "Kernel", "Support", "Texture", "encode_fs");
				}
				// newton
				{
					javascript->add_proxy(Script::Support::PopCap::Newton::decode_fs, "Sen", "Kernel", "Support", "PopCap", "Newton", "decode_fs");
					javascript->add_proxy(Script::Support::PopCap::Newton::encode_fs, "Sen", "Kernel", "Support", "PopCap", "Newton", "encode_fs");
				}
				// rton
				{
					// decode fs
					javascript->add_proxy(Script::Support::PopCap::RTON::decode_fs, "Sen", "Kernel", "Support", "PopCap", "RTON", "decode_fs");
					// decode_fs_as_multiple_threads
					javascript->add_proxy(Script::Support::PopCap::RTON::decode_fs_as_multiple_threads, "Sen", "Kernel", "Support", "PopCap", "RTON", "decode_fs_as_multiple_threads");
					// decrypt fs
					javascript->add_proxy(Script::Support::PopCap::RTON::decrypt_fs, "Sen", "Kernel", "Support", "PopCap", "RTON", "decrypt_fs");
					// decrypt_and_decode_fs
					javascript->add_proxy(Script::Support::PopCap::RTON::decrypt_and_decode_fs, "Sen", "Kernel", "Support", "PopCap", "RTON", "decrypt_and_decode_fs");
					// encode fs
					javascript->add_proxy(Script::Support::PopCap::RTON::encode_fs, "Sen", "Kernel", "Support", "PopCap", "RTON", "encode_fs");
					// encode_fs_as_multiple_threads
					javascript->add_proxy(Script::Support::PopCap::RTON::encode_fs_as_multiple_threads, "Sen", "Kernel", "Support", "PopCap", "RTON", "encode_fs_as_multiple_threads");
				}
				// zlib
				{
					javascript->add_proxy(Script::Support::PopCap::Zlib::uncompress_fs, "Sen", "Kernel", "Support", "PopCap", "Zlib", "uncompress_fs");
					javascript->add_proxy(Script::Support::PopCap::Zlib::compress_fs, "Sen", "Kernel", "Support", "PopCap", "Zlib", "compress_fs");
				}
				// compiled text
				{
					javascript->add_proxy(Script::Support::PopCap::CompiledText::decode_fs, "Sen", "Kernel", "Support", "PopCap", "CompiledText", "decode_fs");
					javascript->add_proxy(Script::Support::PopCap::CompiledText::encode_fs, "Sen", "Kernel", "Support", "PopCap", "CompiledText", "encode_fs");
				}
				// render-effects
				{
					// decode fs
					javascript->add_proxy(Script::Support::PopCap::RenderEffects::decode_fs, "Sen", "Kernel", "Support", "PopCap", "RenderEffects", "decode_fs");
					// encode fs
					javascript->add_proxy(Script::Support::PopCap::RenderEffects::encode_fs, "Sen", "Kernel", "Support", "PopCap", "RenderEffects", "encode_fs");
				}
				// crypt-data
				{
					// decrypt fs
					javascript->add_proxy(Script::Support::PopCap::CryptData::decrypt_fs, "Sen", "Kernel", "Support", "PopCap", "CryptData", "decrypt_fs");
					// encrypt fs
					javascript->add_proxy(Script::Support::PopCap::CryptData::encrypt_fs, "Sen", "Kernel", "Support", "PopCap", "CryptData", "encrypt_fs");
				}
				// character-font-widget-2
				{
					// decode fs
					javascript->add_proxy(Script::Support::PopCap::CharacterFontWidget2::decode_fs, "Sen", "Kernel", "Support", "PopCap", "CharacterFontWidget2", "decode_fs");
					// encode fs
					javascript->add_proxy(Script::Support::PopCap::CharacterFontWidget2::encode_fs, "Sen", "Kernel", "Support", "PopCap", "CharacterFontWidget2", "encode_fs");
				}
				// rsb
				{
					javascript->add_proxy(Script::Support::PopCap::RSB::unpack_fs, "Sen", "Kernel", "Support", "PopCap", 
					"RSB", "unpack_fs");
					javascript->add_proxy(Script::Support::PopCap::RSB::pack_fs, "Sen", "Kernel", "Support", "PopCap", 
					"RSB", "pack_fs");
				}
				// rsg
				{
					javascript->add_proxy(Script::Support::PopCap::RSG::unpack_fs, "Sen", "Kernel", "Support", "PopCap", "RSG", "unpack_fs");
					javascript->add_proxy(Script::Support::PopCap::RSG::pack_fs, "Sen", "Kernel", "Support", "PopCap", "RSG", "pack_fs");
				}
				// pam
				{
					javascript->add_proxy(Script::Support::PopCap::Animation::decode_fs, "Sen", "Kernel", "Support", "PopCap", "Animation", "decode_fs");
					javascript->add_proxy(Script::Support::PopCap::Animation::encode_fs, "Sen", "Kernel", "Support", "PopCap", "Animation", "encode_fs");
				}
				// bnk
				{
					javascript->add_proxy(Script::Support::WWise::SoundBank::decode_fs, "Sen", "Kernel", "Support", "WWise", "SoundBank", "decode_fs");
					javascript->add_proxy(Script::Support::WWise::SoundBank::encode_fs, "Sen", "Kernel", "Support", "WWise", "SoundBank", "encode_fs");
				}
				// resource group
				{
					javascript->add_proxy(Script::Support::PopCap::ResourceGroup::split_fs, "Sen", "Kernel", "Support", "PopCap", "ResourceGroup", "split_fs");
					javascript->add_proxy(Script::Support::PopCap::ResourceGroup::merge_fs, "Sen", "Kernel", "Support", "PopCap", "ResourceGroup", "merge_fs");
					javascript->add_proxy(Script::Support::PopCap::ResourceGroup::convert_fs, "Sen", "Kernel", "Support", "PopCap", "ResourceGroup", "convert_fs");
				}
				// res info
				{
					javascript->add_proxy(Script::Support::PopCap::ResInfo::split_fs, "Sen", "Kernel", "Support", "PopCap", "ResInfo", "split_fs");
					javascript->add_proxy(Script::Support::PopCap::ResInfo::merge_fs, "Sen", "Kernel", "Support", "PopCap", "ResInfo", "merge_fs");
					javascript->add_proxy(Script::Support::PopCap::ResInfo::convert_fs, "Sen", "Kernel", "Support", "PopCap", "ResInfo", "convert_fs");
				}
				// dimension
				{
					// instance of dimension
					javascript->add_proxy(Script::Dimension::instance, "Sen", "Kernel", "Dimension", "instance");
					// open the image
					javascript->add_proxy(Script::Dimension::open, "Sen", "Kernel", "Image", "open");
					// write the image
					javascript->add_proxy(Script::Dimension::write, "Sen", "Kernel", "Image", "write");
				}
				javascript->evaluate_fs(script_path);
				javascript->evaluate("Sen.Script.main()", "<script>");
				return;
			}
	};

}