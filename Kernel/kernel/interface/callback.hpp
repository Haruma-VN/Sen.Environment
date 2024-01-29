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
					javascript->add_constant<int>(Kernel::version, "Sen"_sv, "Kernel"_sv, "version"_sv);
					javascript->add_constant<int>(static_cast<int>(std::stoi(std::string{shell_version.value, shell_version.size})), "Sen"_sv, "Shell"_sv, "version"_sv);
					javascript->add_constant<bool>(static_cast<bool>(std::stoi(std::string{is_gui.value, is_gui.size})), "Sen"_sv, "Shell"_sv, "is_gui"_sv);
					javascript->add_proxy(Script::callback, "Sen"_sv, "Shell"_sv, "callback"_sv);
					javascript->add_constant(thiz.arguments, "Sen"_sv, "Kernel"_sv, "arguments"_sv);
				}
				// xml
				{
					// deserialize
					javascript->add_proxy(Script::XML::deserialize, "Sen"_sv, "Kernel"_sv, "XML"_sv, "deserialize"_sv);
					// deserialize_fs
					javascript->add_proxy(Script::XML::deserialize_fs, "Sen"_sv, "Kernel"_sv, "XML"_sv, "deserialize_fs"_sv);
					// serialize
					javascript->add_proxy(Script::XML::serialize, "Sen"_sv, "Kernel"_sv, "XML"_sv, "serialize"_sv);
					// serialize_fs
					javascript->add_proxy(Script::XML::serialize_fs, "Sen"_sv, "Kernel"_sv, "XML"_sv, "serialize_fs"_sv);
				}
				// json
				{
					// deserialize
					javascript->add_proxy(Script::JSON::deserialize, "Sen"_sv, "Kernel"_sv, "JSON"_sv, "deserialize"_sv);
					// serialize
					javascript->add_proxy(Script::JSON::serialize, "Sen"_sv, "Kernel"_sv, "JSON"_sv, "serialize"_sv);
					// deserialize fs
					javascript->add_proxy(Script::JSON::deserialize_fs, "Sen"_sv, "Kernel"_sv, "JSON"_sv, "deserialize_fs"_sv);
					// serialize fs
					javascript->add_proxy(Script::JSON::serialize_fs, "Sen"_sv, "Kernel"_sv, "JSON"_sv, "serialize_fs"_sv);
				}
				// home
				{
					// script path
					javascript->add_constant<std::string_view>(String::toPosixStyle(script_path.data()), "Sen"_sv, "Kernel"_sv, "Home"_sv, "script"_sv);
				}
				// vcdiff
				{
					// encode fs
					javascript->add_proxy(Script::Diff::VCDiff::encode_fs, "Sen"_sv, "Kernel"_sv, "Diff"_sv, "VCDiff"_sv, "encode_fs"_sv);
					// decode fs
					javascript->add_proxy(Script::Diff::VCDiff::decode_fs, "Sen"_sv, "Kernel"_sv, "Diff"_sv, "VCDiff"_sv, "decode_fs"_sv);
				}
				// file system
				{
					// read file
					javascript->add_proxy(Script::FileSystem::read_file, "Sen"_sv, "Kernel"_sv, "FileSystem"_sv, "read_file"_sv);
					// read_file_encode_with_utf16le
					javascript->add_proxy(Script::FileSystem::read_file_encode_with_utf16le, "Sen"_sv, "Kernel"_sv, "FileSystem"_sv, "read_file_encode_with_utf16le"_sv);
					// write_file_encode_with_utf16le
					javascript->add_proxy(Script::FileSystem::write_file_encode_with_utf16le, "Sen"_sv, "Kernel"_sv, "FileSystem"_sv, "write_file_encode_with_utf16le"_sv);
					// write file
					javascript->add_proxy(Script::FileSystem::write_file, "Sen"_sv, "Kernel"_sv, "FileSystem"_sv, "write_file"_sv);
					// create directory
					javascript->add_proxy(Script::FileSystem::create_directory, "Sen"_sv, "Kernel"_sv, "FileSystem"_sv, "create_directory"_sv);
					// read_current_directory
					javascript->add_proxy(Script::FileSystem::read_current_directory, "Sen"_sv, "Kernel"_sv, "FileSystem"_sv, "read_current_directory"_sv);
					// read_directory_only_file
					javascript->add_proxy(Script::FileSystem::read_directory_only_file, "Sen"_sv, "Kernel"_sv, "FileSystem"_sv, "read_directory_only_file"_sv);
					// read_directory_only_directory
					javascript->add_proxy(Script::FileSystem::read_directory_only_directory, "Sen"_sv, "Kernel"_sv, "FileSystem"_sv, "read_directory_only_directory"_sv);
					// read_directory
					javascript->add_proxy(Script::FileSystem::read_directory, "Sen"_sv, "Kernel"_sv, "FileSystem"_sv, "read_directory"_sv);
					// is file
					javascript->add_proxy(Script::FileSystem::is_file, "Sen"_sv, "Kernel"_sv, "FileSystem"_sv, "is_file"_sv);
					// is directory
					javascript->add_proxy(Script::FileSystem::is_directory, "Sen"_sv, "Kernel"_sv, "FileSystem"_sv,"is_directory"_sv);
					// operation : rename
					javascript->add_proxy(Script::FileSystem::Operation::rename, "Sen"_sv, "Kernel"_sv, "FileSystem"_sv, "Operation"_sv, "rename"_sv);
					// operation : remove
					javascript->add_proxy(Script::FileSystem::Operation::remove, "Sen"_sv, "Kernel"_sv, "FileSystem"_sv, "Operation"_sv, "remove"_sv);
					// operation : copy
					javascript->add_proxy(Script::FileSystem::Operation::copy, "Sen"_sv, "Kernel"_sv, "FileSystem"_sv, "Operation"_sv, "copy"_sv);
				}
				// path
				{
					// join
					javascript->add_proxy(Script::Path::join, "Sen"_sv, "Kernel"_sv, "Path"_sv, "join"_sv);
					// basename
					javascript->add_proxy(Script::Path::basename, "Sen"_sv, "Kernel"_sv, "Path"_sv, "basename"_sv);
					// delimiter
					javascript->add_proxy(Script::Path::delimiter, "Sen"_sv, "Kernel"_sv, "Path"_sv, "delimiter"_sv);
					// dirname
					javascript->add_proxy(Script::Path::dirname, "Sen"_sv, "Kernel"_sv, "Path"_sv, "dirname"_sv);
					// format
					javascript->add_proxy(Script::Path::format, "Sen"_sv, "Kernel"_sv, "Path"_sv, "format"_sv);
					// normalize
					javascript->add_proxy(Script::Path::normalize, "Sen"_sv, "Kernel"_sv, "Path"_sv, "normalize"_sv);
					// resolve
					javascript->add_proxy(Script::Path::resolve, "Sen"_sv, "Kernel"_sv, "Path"_sv, "resolve"_sv);
					// relative
					javascript->add_proxy(Script::Path::relative, "Sen"_sv, "Kernel"_sv, "Path"_sv, "relative"_sv);
					// extname
					javascript->add_proxy(Script::Path::extname, "Sen"_sv, "Kernel"_sv, "Path"_sv, "extname"_sv);
					// is_absolute
					javascript->add_proxy(Script::Path::relative, "Sen"_sv, "Kernel"_sv, "Path"_sv, "is_absolute"_sv);
					// base_without_extension
					javascript->add_proxy(Script::Path::base_without_extension, "Sen"_sv, "Kernel"_sv, "Path"_sv, "base_without_extension"_sv);
					// except_extension
					javascript->add_proxy(Script::Path::except_extension, "Sen"_sv, "Kernel"_sv, "Path"_sv, "except_extension"_sv);
				}
				// console
				{
					// console print
					javascript->add_proxy(Script::Console::print, "Sen"_sv, "Kernel"_sv, "Console"_sv, "print"_sv);
					// read line
					javascript->add_proxy(Script::Console::readline, "Sen"_sv, "Kernel"_sv, "Console"_sv, "readline"_sv);
				}
				// language
				{
					// load language
					javascript->add_proxy(Script::Language::load_language, "Sen"_sv, "Kernel"_sv, "Language"_sv, "load_language"_sv);
					// get language
					javascript->add_proxy(Script::Language::get, "Sen"_sv, "Kernel"_sv, "Language"_sv, "get"_sv);
				}
				// process
				{
					// run
					javascript->add_proxy(Script::Process::run, "Sen"_sv, "Kernel"_sv, "Process"_sv, "run"_sv);
					// execute
					javascript->add_proxy(Script::Process::execute, "Sen"_sv, "Kernel"_sv, "Process"_sv, "execute"_sv);
					// is_exists_in_path_environment
					javascript->add_proxy(Script::Process::is_exists_in_path_environment, "Sen"_sv, "Kernel"_sv, "Process"_sv, "is_exists_in_path_environment"_sv);
				}
				// thread
				{
					// sleep
					javascript->add_proxy(Script::Thread::sleep, "Sen"_sv, "Kernel"_sv, "Thread"_sv, "sleep"_sv);
					// now
					javascript->add_proxy(Script::Thread::now, "Sen"_sv, "Kernel"_sv, "Thread"_sv, "now"_sv);
				}
				// array buffer
				{
					// open
					javascript->add_proxy(Script::ArrayBuffer::open, "Sen"_sv, "Kernel"_sv, "ArrayBuffer"_sv, "open"_sv);
					// out
					javascript->add_proxy(Script::ArrayBuffer::out, "Sen"_sv, "Kernel"_sv, "ArrayBuffer"_sv, "out"_sv);
				}
				// operating system
				{
					// current
					javascript->add_proxy(Script::OperatingSystem::current, "Sen"_sv, "Kernel"_sv, "OperatingSystem"_sv, "current"_sv);
					// architecture
					javascript->add_proxy(Script::OperatingSystem::architecture, "Sen"_sv, "Kernel"_sv, "OperatingSystem"_sv, "architecture"_sv);
				}
				// image
				{
					// scale_fs
					javascript->add_proxy(Script::Image::scale_fs, "Sen"_sv, "Kernel"_sv, "Image"_sv, "scale_fs"_sv);
					// join_png
					javascript->add_proxy(Script::Image::join_png, "Sen"_sv, "Kernel"_sv, "Image"_sv, "join_png"_sv);
					// transparent_fs
					javascript->add_proxy(Script::Image::transparent_fs, "Sen"_sv, "Kernel"_sv, "Image"_sv, "transparent_fs"_sv);
					// resize_fs
					javascript->add_proxy(Script::Image::resize_fs, "Sen"_sv, "Kernel"_sv, "Image"_sv, "resize_fs"_sv);
					// rotate_fs
					javascript->add_proxy(Script::Image::rotate_fs, "Sen"_sv, "Kernel"_sv, "Image"_sv, "rotate_fs"_sv);
					// composite_fs
					javascript->add_proxy(Script::Image::composite_fs, "Sen"_sv, "Kernel"_sv, "Image"_sv, "composite_fs"_sv);
					// composite_multiple_fs
					javascript->add_proxy(Script::Image::composite_multiple_fs, "Sen"_sv, "Kernel"_sv, "Image"_sv, "composite_multiple_fs"_sv);
					// composite_multiple_fs_asynchronous
					javascript->add_proxy(Script::Image::composite_multiple_fs_asynchronous, "Sen"_sv, "Kernel"_sv, "Image"_sv, "composite_multiple_fs_asynchronous"_sv);
				}
				// javascript
				{
					// evaluate
					javascript->add_proxy(Script::JavaScript::evaluate, "Sen"_sv, "Kernel"_sv, "JavaScript"_sv, "evaluate"_sv);
					// evaluate file
					javascript->add_proxy(Script::JavaScript::evaluate_fs, "Sen"_sv, "Kernel"_sv, "JavaScript"_sv, "evaluate_fs"_sv);
				}
				// md5
				{
					// hash method
					javascript->add_proxy(Script::Encryption::MD5::hash, "Sen"_sv, "Kernel"_sv, "Encryption"_sv, "MD5"_sv, "hash"_sv);
					// hash file method
					javascript->add_proxy(Script::Encryption::MD5::hash_fs, "Sen"_sv, "Kernel"_sv, "Encryption"_sv, "MD5"_sv, "hash_fs"_sv);
				}
				// sha224
				{
					// hash method
					javascript->add_proxy(Script::Encryption::SHA224::hash, "Sen"_sv, "Kernel"_sv, "Encryption"_sv, "SHA224"_sv, "hash"_sv);
					// hash file method
					javascript->add_proxy(Script::Encryption::SHA224::hash_fs, "Sen"_sv, "Kernel"_sv, "Encryption"_sv, "SHA224"_sv, "hash_fs"_sv);
				}
				// sha256
				{
					// hash method
					javascript->add_proxy(Script::Encryption::SHA256::hash, "Sen"_sv, "Kernel"_sv, "Encryption"_sv, "SHA256"_sv, "hash"_sv);
					// hash file method
					javascript->add_proxy(Script::Encryption::SHA224::hash_fs, "Sen"_sv, "Kernel"_sv, "Encryption"_sv, "SHA256"_sv, "hash_fs"_sv);
				}
				// sha384
				{
					// hash method
					javascript->add_proxy(Script::Encryption::SHA384::hash, "Sen"_sv, "Kernel"_sv, "Encryption"_sv, "SHA384"_sv, "hash"_sv);
					// hash file method
					javascript->add_proxy(Script::Encryption::SHA384::hash_fs, "Sen"_sv, "Kernel"_sv, "Encryption"_sv, "SHA384"_sv, "hash_fs"_sv);
				}
				// sha512
				{
					// hash method
					javascript->add_proxy(Script::Encryption::SHA512::hash, "Sen"_sv, "Kernel"_sv, "Encryption"_sv, "SHA512"_sv, "hash"_sv);
					// hash file method
					javascript->add_proxy(Script::Encryption::SHA512::hash_fs, "Sen"_sv, "Kernel"_sv, "Encryption"_sv, "SHA512"_sv, "hash_fs"_sv);
				}
				// xor
				{
					// encrypt method
					javascript->add_proxy(Script::Encryption::XOR::encrypt, "Sen"_sv, "Kernel"_sv, "Encryption"_sv, "X0R"_sv, "encrypt"_sv);
					// encrypt file method
					javascript->add_proxy(Script::Encryption::XOR::encrypt_fs, "Sen"_sv, "Kernel"_sv, "Encryption"_sv, "X0R"_sv, "encrypt_fs"_sv);
				}
				// zip
				{
					// compress directory
					javascript->add_proxy(Script::Compression::Zip::Compress::directory, "Sen"_sv, "Kernel"_sv, "Compression"_sv, "Zip"_sv, "Compress"_sv, "directory"_sv);
					// compress files
					javascript->add_proxy(Script::Compression::Zip::Compress::file, "Sen"_sv, "Kernel"_sv, "Compression"_sv, "Zip"_sv, "Compress"_sv, "file"_sv);
					// uncompress
					javascript->add_proxy(Script::Compression::Zip::Uncompress::process, "Sen"_sv, "Kernel"_sv, "Compression"_sv, "Zip"_sv, "Uncompress"_sv, "process"_sv);
				}
				// zlib
				{
					// compress file method
					javascript->add_proxy(Script::Compression::Zlib::compress_fs, "Sen"_sv, "Kernel"_sv, "Compression"_sv, "Zlib"_sv, "compress_fs"_sv);
					// uncompress file method
					javascript->add_proxy(Script::Compression::Zlib::uncompress_fs, "Sen"_sv, "Kernel"_sv, "Compression"_sv, "Zlib"_sv, "uncompress_fs"_sv);
				}
				// gzip
				{
					// compress file method
					javascript->add_proxy(Script::Compression::Gzip::compress_fs, "Sen"_sv, "Kernel"_sv, "Compression"_sv, "Gzip"_sv, "compress_fs"_sv);
					// uncompress file method
					javascript->add_proxy(Script::Compression::Gzip::uncompress_fs, "Sen"_sv, "Kernel"_sv, "Compression"_sv, "Gzip"_sv, "uncompress_fs"_sv);
				}
				// bzip2
				{
					// compress file method
					javascript->add_proxy(Script::Compression::Bzip2::compress_fs, "Sen"_sv, "Kernel"_sv, "Compression"_sv, "Bzip2"_sv, "compress_fs"_sv);
					// uncompress file method
					javascript->add_proxy(Script::Compression::Bzip2::uncompress_fs, "Sen"_sv, "Kernel"_sv, "Compression"_sv, "Bzip2"_sv, "uncompress_fs"_sv);
				}
				// lzma
				{
					// compress file method
					javascript->add_proxy(Script::Compression::Lzma::compress_fs, "Sen"_sv, "Kernel"_sv, "Compression"_sv, "Lzma"_sv, "compress_fs"_sv);
					// uncompress file method
					javascript->add_proxy(Script::Compression::Lzma::uncompress_fs, "Sen"_sv, "Kernel"_sv, "Compression"_sv, "Lzma"_sv, "uncompress_fs"_sv);
				}
				// base64
				{
					// encode base64
					javascript->add_proxy(Script::Encryption::Base64::encode, "Sen"_sv, "Kernel"_sv, "Encryption"_sv, "Base64"_sv, "encode"_sv);
					// decode base64
					javascript->add_proxy(Script::Encryption::Base64::decode, "Sen"_sv, "Kernel"_sv, "Encryption"_sv, "Base64"_sv, "decode"_sv);
					// encode base64 for file
					javascript->add_proxy(Script::Encryption::Base64::encode_fs, "Sen"_sv, "Kernel"_sv, "Encryption"_sv, "Base64"_sv, "encode_fs"_sv);
					// decode base64 for file
					javascript->add_proxy(Script::Encryption::Base64::decode_fs, "Sen"_sv, "Kernel"_sv, "Encryption"_sv, "Base64"_sv, "decode_fs"_sv);
					// encode base64 for file as thread
					javascript->add_proxy(Script::Encryption::Base64::encode_fs_as_multiple_threads, "Sen"_sv, "Kernel"_sv, "Encryption"_sv, "Base64"_sv, "encode_fs_as_multiple_thread"_sv);
					// decode base64 for file as thread
					javascript->add_proxy(Script::Encryption::Base64::decode_fs_as_multiple_threads, "Sen"_sv, "Kernel"_sv, "Encryption"_sv, "Base64"_sv, "decode_fs_as_multiple_threads"_sv);
				}
				// texture
				{
					// decode fs
					javascript->add_proxy(Script::Support::Texture::decode_fs, "Sen"_sv, "Kernel"_sv, "Support"_sv, "Texture"_sv,"decode_fs"_sv);
					// encode fs
					javascript->add_proxy(Script::Support::Texture::encode_fs, "Sen"_sv, "Kernel"_sv, "Support"_sv, "Texture"_sv, "encode_fs"_sv);
				}
				// newton
				{
					// decode fs
					javascript->add_proxy(Script::Support::PopCap::Newton::decode_fs, "Sen"_sv, "Kernel"_sv, "Support"_sv, "PopCap"_sv, "Newton"_sv, "decode_fs"_sv);
					// encode fs
					javascript->add_proxy(Script::Support::PopCap::Newton::encode_fs, "Sen"_sv, "Kernel"_sv, "Support"_sv, "PopCap"_sv, "Newton"_sv, "encode_fs"_sv);
				}
				// rton
				{
					// decode fs
					javascript->add_proxy(Script::Support::PopCap::RTON::decode_fs, "Sen"_sv, "Kernel"_sv, "Support"_sv, "PopCap"_sv, "RTON"_sv, "decode_fs"_sv);
					// decode_fs_as_multiple_threads
					javascript->add_proxy(Script::Support::PopCap::RTON::decode_fs_as_multiple_threads, "Sen"_sv, "Kernel"_sv, "Support"_sv, "PopCap"_sv, "RTON"_sv, "decode_fs_as_multiple_threads"_sv);
					// decrypt fs
					javascript->add_proxy(Script::Support::PopCap::RTON::decrypt_fs, "Sen"_sv, "Kernel"_sv, "Support"_sv, "PopCap"_sv, "RTON"_sv, "decrypt_fs"_sv);
					// decrypt_and_decode_fs
					javascript->add_proxy(Script::Support::PopCap::RTON::decrypt_and_decode_fs, "Sen"_sv, "Kernel"_sv, "Support"_sv, "PopCap"_sv, "RTON"_sv, "decrypt_and_decode_fs"_sv);
					// encode fs
					javascript->add_proxy(Script::Support::PopCap::RTON::encode_fs, "Sen"_sv, "Kernel"_sv, "Support"_sv, "PopCap"_sv, "RTON"_sv, "encode_fs"_sv);
					// encode_fs_as_multiple_threads
					javascript->add_proxy(Script::Support::PopCap::RTON::encode_fs_as_multiple_threads, "Sen"_sv, "Kernel"_sv, "Support"_sv, "PopCap"_sv, "RTON"_sv, "encode_fs_as_multiple_threads"_sv);
				}
				// zlib
				{
					// uncompress fs
					javascript->add_proxy(Script::Support::PopCap::Zlib::uncompress_fs, "Sen"_sv, "Kernel"_sv, "Support"_sv, "PopCap"_sv, "Zlib"_sv, "uncompress_fs"_sv);
					// compress fs
					javascript->add_proxy(Script::Support::PopCap::Zlib::compress_fs, "Sen"_sv, "Kernel"_sv, "Support"_sv, "PopCap"_sv, "Zlib"_sv, "compress_fs"_sv);
				}
				// compiled text
				{
					// decode fs
					javascript->add_proxy(Script::Support::PopCap::CompiledText::decode_fs, "Sen"_sv, "Kernel"_sv, "Support"_sv, "PopCap"_sv, "CompiledText"_sv, "decode_fs"_sv);
					// encode fs
					javascript->add_proxy(Script::Support::PopCap::CompiledText::encode_fs, "Sen"_sv, "Kernel"_sv, "Support"_sv, "PopCap"_sv, "CompiledText"_sv, "encode_fs"_sv);
				}
				// render-effects
				{
					// decode fs
					javascript->add_proxy(Script::Support::PopCap::RenderEffects::decode_fs, "Sen"_sv, "Kernel"_sv, "Support"_sv, "PopCap"_sv, "RenderEffects"_sv, "decode_fs"_sv);
					// encode fs
					javascript->add_proxy(Script::Support::PopCap::RenderEffects::encode_fs, "Sen"_sv, "Kernel"_sv, "Support"_sv, "PopCap"_sv, "RenderEffects"_sv, "encode_fs"_sv);
				}
				// crypt-data
				{
					// decrypt fs
					javascript->add_proxy(Script::Support::PopCap::CryptData::decrypt_fs, "Sen"_sv, "Kernel"_sv, "Support"_sv, "PopCap"_sv, "CryptData"_sv, "decrypt_fs"_sv);
					// encrypt fs
					javascript->add_proxy(Script::Support::PopCap::CryptData::encrypt_fs, "Sen"_sv, "Kernel"_sv, "Support"_sv, "PopCap"_sv, "CryptData"_sv, "encrypt_fs"_sv);
				}
				// character-font-widget-2
				{
					// decode fs
					javascript->add_proxy(Script::Support::PopCap::CharacterFontWidget2::decode_fs, "Sen"_sv, "Kernel"_sv, "Support"_sv, "PopCap"_sv, "CharacterFontWidget2"_sv, "decode_fs"_sv);
					// encode fs
					javascript->add_proxy(Script::Support::PopCap::CharacterFontWidget2::encode_fs, "Sen"_sv, "Kernel"_sv, "Support"_sv, "PopCap"_sv, "CharacterFontWidget2"_sv, "encode_fs"_sv);
				}
				// rsb
				{
					javascript->add_proxy(Script::Support::PopCap::RSB::unpack_fs, "Sen"_sv, "Kernel"_sv, "Support"_sv, "PopCap"_sv,"RSB"_sv, "unpack_fs"_sv);
					javascript->add_proxy(Script::Support::PopCap::RSB::pack_fs, "Sen"_sv, "Kernel"_sv, "Support"_sv, "PopCap"_sv,"RSB"_sv, "pack_fs"_sv);
				}
				// rsg
				{
					javascript->add_proxy(Script::Support::PopCap::RSG::unpack_fs, "Sen"_sv, "Kernel"_sv, "Support"_sv, "PopCap"_sv, "RSG"_sv, "unpack_fs"_sv);
					javascript->add_proxy(Script::Support::PopCap::RSG::pack_fs, "Sen"_sv, "Kernel"_sv, "Support"_sv, "PopCap"_sv, "RSG"_sv, "pack_fs"_sv);
				}
				// pam
				{
					javascript->add_proxy(Script::Support::PopCap::Animation::decode_fs, "Sen"_sv, "Kernel"_sv, "Support"_sv, "PopCap"_sv, "Animation"_sv, "decode_fs"_sv);
					javascript->add_proxy(Script::Support::PopCap::Animation::encode_fs, "Sen"_sv, "Kernel"_sv, "Support"_sv, "PopCap"_sv, "Animation"_sv, "encode_fs"_sv);
				}
				// bnk
				{
					javascript->add_proxy(Script::Support::WWise::SoundBank::decode_fs, "Sen"_sv, "Kernel"_sv, "Support"_sv, "WWise"_sv, "SoundBank"_sv, "decode_fs"_sv);
					javascript->add_proxy(Script::Support::WWise::SoundBank::encode_fs, "Sen"_sv, "Kernel"_sv, "Support"_sv, "WWise"_sv, "SoundBank"_sv, "encode_fs"_sv);
				}
				// resource group
				{
					javascript->add_proxy(Script::Support::PopCap::ResourceGroup::split_fs, "Sen"_sv, "Kernel"_sv, "Support"_sv, "PopCap"_sv, "ResourceGroup"_sv, "split_fs"_sv);
					javascript->add_proxy(Script::Support::PopCap::ResourceGroup::merge_fs, "Sen"_sv, "Kernel"_sv, "Support"_sv, "PopCap"_sv, "ResourceGroup"_sv, "merge_fs"_sv);
					javascript->add_proxy(Script::Support::PopCap::ResourceGroup::convert_fs, "Sen"_sv, "Kernel"_sv, "Support"_sv, "PopCap"_sv, "ResourceGroup"_sv, "convert_fs"_sv);
				}
				// res info
				{
					javascript->add_proxy(Script::Support::PopCap::ResInfo::split_fs, "Sen"_sv, "Kernel"_sv, "Support"_sv, "PopCap"_sv, "ResInfo"_sv, "split_fs"_sv);
					javascript->add_proxy(Script::Support::PopCap::ResInfo::merge_fs, "Sen"_sv, "Kernel"_sv, "Support"_sv, "PopCap"_sv, "ResInfo"_sv, "merge_fs"_sv);
					javascript->add_proxy(Script::Support::PopCap::ResInfo::convert_fs, "Sen"_sv, "Kernel"_sv, "Support"_sv, "PopCap"_sv, "ResInfo"_sv, "convert_fs"_sv);
				}
				// dimension
				{
					// instance of dimension
					javascript->add_proxy(Script::Dimension::instance, "Sen"_sv, "Kernel"_sv, "Dimension"_sv, "instance"_sv);
					// open the image
					javascript->add_proxy(Script::Dimension::open, "Sen"_sv, "Kernel"_sv, "Image"_sv, "open"_sv);
					// write the image
					javascript->add_proxy(Script::Dimension::write, "Sen"_sv, "Kernel"_sv, "Image"_sv, "write"_sv);
				}
				// DataStreamView
				javascript->register_object(Script::Class::DataStreamView::register_class<false>);
				// DataStreamViewUseBigEndian
				javascript->register_object(Script::Class::DataStreamView::register_class<true>);
				// execute the script
				javascript->evaluate_fs(script_path);
				// call main
				javascript->evaluate("Sen.Script.main()"_sv, std::source_location::current().file_name());
				return;
			}
	};

}