#pragma once

#include "kernel/interface/shell.hpp"

namespace Sen::Kernel::Interface::Script {

	/**
	 * ----------------------------------------
	 * JS namespace is used
	 * ----------------------------------------
	*/

	namespace JS = Sen::Kernel::Definition::JavaScript;

	/**
	 * ----------------------------------------
	 * JS Method Declaration
	 * ----------------------------------------
	*/

	typedef JSValue (*JavaScriptNativeMethod)(JSContext *, JSValueConst, int, JSValueConst *);

	/**
	 * ----------------------------------------
	 * JS Print method
	 * @param argv[0]: title
	 * @param argv[1]: color
	 * ----------------------------------------
	*/

	inline static auto print(
		JSContext *context, 
		JSValueConst this_val, 
		int argc, 
		JSValueConst *argv
	) -> JSValue
	{
		try_assert(argc == 2 or argc == 1, fmt::format("argument expected {} but received {}", "2 or 1", argc));
		auto str = JS_ToCString(context, argv[0]);
		if(argc == 2){
			Shell{}.print(JS::Converter::get_string(context, argv[0]).c_str(),static_cast<Sen::Kernel::Interface::Color>(JS::Converter::get_int32(context, argv[1])));
		}
		else{
			Shell{}.print(JS::Converter::get_string(context, argv[0]).c_str(), Sen::Kernel::Interface::Color::DEFAULT);
		}
		JS_FreeCString(context, str);
		return JS::Converter::get_undefined();
	}

	/**
	 * ----------------------------------------
	 * JS Readline method
	 * @returns: input data
	 * ----------------------------------------
	*/

	inline static auto readline(
		JSContext *context, 
		JSValueConst this_val, 
		int argc, 
		JSValueConst *argv
	) -> JSValue
	{
		try_assert(argc == 0, fmt::format("argument expected {} but received {}", 0, argc));
		auto result = Shell{}.input();
		auto m_result = std::string{result.data, result.size};
		return JS::Converter::to_string(context, m_result);
	}

	/**
	 * ----------------------------------------
	 * JS read file method
	 * @param argv[0]: source file
	 * @return: readed file
	 * ----------------------------------------
	*/

	inline static auto read_file(
		JSContext *context, 
		JSValueConst this_val, 
		int argc, 
		JSValueConst *argv
	) -> JSValue
	{
		try_assert(argc == 1, fmt::format("argument expected {} but received {}", 1, argc));
		auto source = JS_ToCString(context, argv[0]);
		auto result = FileSystem::readFile(source);
		bool c = JS::Converter::get_bool(context, argv[1]);
		JS_FreeCString(context, source);
		return JS::Converter::to_string(context, result);
	}

	/**
	 * ----------------------------------------
	 * JS write file method
	 * @param argv[0]: destination file
	 * @param argv[1]: data to write
	 * @return: written file
	 * ----------------------------------------
	*/

	inline static auto write_file(
		JSContext *context, 
		JSValueConst this_val, 
		int argc, 
		JSValueConst *argv
	) -> JSValue
	{
		try_assert(argc == 2, fmt::format("argument expected {} but received {}", 2, argc));
		auto destination = JS_ToCString(context, argv[0]);
		auto data = JS_ToCString(context, argv[1]);
		FileSystem::writeFile(destination, data);
		JS_FreeCString(context, destination);
		JS_FreeCString(context, data);
		return JS::Converter::get_undefined();
	}

	/**
	 * ----------------------------------------
	 * JavaScript evaluate method
	 * @param argv[0]: JS code to execute
	 * @return: JS value after evaluate
	 * ----------------------------------------
	*/

	inline static auto evaluate(
		JSContext *context, 
		JSValueConst this_val, 
		int argc, 
		JSValueConst *argv
	) -> JSValue
	{
		try_assert(argc == 1, fmt::format("argument expected {} but received {}", 1, argc));
		auto source = JS_ToCString(context, argv[0]);
		auto m_value = JS_Eval(context, source, strlen(source), "unknown", JS_EVAL_TYPE_GLOBAL);
		JS_FreeCString(context, source);
		return m_value;
	}

	/**
	 * ----------------------------------------
	 * JavaScript evaluate as file
	 * @param argv[0]: source file
	 * @return: JS Value after evaluate
	 * ----------------------------------------
	*/

	inline static auto evaluate_fs(
		JSContext *context, 
		JSValueConst this_val, 
		int argc, 
		JSValueConst *argv
	) -> JSValue
	{
		try_assert(argc == 1, fmt::format("argument expected {} but received {}", 1, argc));
		auto source = JS_ToCString(context, argv[0]);
		auto js_source = FileSystem::readFile(source);
		auto m_value = JS_Eval(context, js_source.c_str(), js_source.size(), source, JS_EVAL_TYPE_GLOBAL);
		JS_FreeCString(context, source);
		return m_value;
	}

	/**
	 * ----------------------------------------
	 * JavaScript MD5 Hash method
	 * @param argv[0]: source hash
	 * @returns: hashed string
	 * ----------------------------------------
	*/

	inline static auto md5_hash(
		JSContext *context, 
		JSValueConst this_val, 
		int argc, 
		JSValueConst *argv
	) -> JSValue
	{
		try_assert(argc == 1, fmt::format("argument expected {} but received {}", 1, argc));
		auto source = JS_ToCString(context, argv[0]);
		auto str = std::string{source};
		auto result = Kernel::Definition::Encryption::MD5::hash(static_cast<std::span<unsigned char>>(String::convertStringToSpan<unsigned char>(str)));
		JS_FreeCString(context, source);
		return JS::Converter::to_string(context, result);
	}

	/**
	 * ----------------------------------------
	 * JavaScript MD5 Hash file method
	 * @param argv[0]: source file
	 * @returns: hashed string
	 * ----------------------------------------
	*/

	inline static auto md5_hash_fs(
		JSContext *context, 
		JSValueConst this_val, 
		int argc, 
		JSValueConst *argv
	) -> JSValue
	{
		try_assert(argc == 1, fmt::format("argument expected {} but received {}", 1, argc));
		auto source = JS_ToCString(context, argv[0]);
		auto result = Sen::Kernel::Definition::Encryption::MD5::hash_fs(source);
		JS_FreeCString(context, source);
		return JS::Converter::to_string(context, result);
	}

	/**
	 * ----------------------------------------
	 * JavaScript Base 64 Encode method
	 * @param argv[0]: source to encode
	 * @returns: Encoded string
	 * ----------------------------------------
	*/

	inline static auto base64_encode(
		JSContext *context, 
		JSValueConst this_val, 
		int argc, 
		JSValueConst *argv
	) -> JSValue
	{
		try_assert(argc == 1, fmt::format("argument expected {} but received {}", 1, argc));
		auto source = JS_ToCString(context, argv[0]);
		auto result = Sen::Kernel::Definition::Encryption::Base64::encode(source);
		JS_FreeCString(context, source);
		return JS::Converter::to_string(context, result);
	}

	/**
	 * ----------------------------------------
	 * JavaScript Base 64 Decode method
	 * @param argv[0]: source to decode
	 * @returns: Decoded string
	 * ----------------------------------------
	*/

	inline static auto base64_decode(
		JSContext *context, 
		JSValueConst this_val, 
		int argc, 
		JSValueConst *argv
	) -> JSValue
	{
		try_assert(argc == 1, fmt::format("argument expected {} but received {}", 1, argc));
		auto source = JS_ToCString(context, argv[0]);
		auto result = Sen::Kernel::Definition::Encryption::Base64::decode(source);
		JS_FreeCString(context, source);
		return JS::Converter::to_string(context, result);
	}

	/**
	 * ----------------------------------------
	 * JavaScript Base 64 Encode method
	 * @param argv[0]: source to encode
	 * @returns: Encoded file
	 * ----------------------------------------
	*/

	inline static auto base64_encode_fs(
		JSContext *context, 
		JSValueConst this_val, 
		int argc, 
		JSValueConst *argv
	) -> JSValue
	{
		try_assert(argc == 2, fmt::format("argument expected {} but received {}", 2, argc));
		auto source = JS_ToCString(context, argv[0]);
		auto destination = JS_ToCString(context, argv[1]);
		Sen::Kernel::Definition::Encryption::Base64::encode_fs(source, destination);
		JS_FreeCString(context, source);
		JS_FreeCString(context, destination);
		return JS::Converter::get_undefined();
	}

	/**
	 * ----------------------------------------
	 * JavaScript Base 64 Encode method
	 * @param argv[0]: source to encode
	 * @returns: Encoded file
	 * ----------------------------------------
	*/

	inline static auto base64_decode_fs(
		JSContext *context, 
		JSValueConst this_val, 
		int argc, 
		JSValueConst *argv
	) -> JSValue
	{
		try_assert(argc == 2, fmt::format("argument expected {} but received {}", 2, argc));
		auto source = JS_ToCString(context, argv[0]);
		auto destination = JS_ToCString(context, argv[1]);
		Sen::Kernel::Definition::Encryption::Base64::decode_fs(source, destination);
		JS_FreeCString(context, source);
		JS_FreeCString(context, destination);
		return JS::Converter::get_undefined();
	}

	/**
	 * ----------------------------------------
	 * JavaScript SHA224 Hash
	 * @param argv[0]: source to encode
	 * @returns: Encoded file
	 * ----------------------------------------
	*/

	inline static auto sha224_hash(
		JSContext *context, 
		JSValueConst this_val, 
		int argc, 
		JSValueConst *argv
	) -> JSValue
	{
		try_assert(argc == 1, fmt::format("argument expected {} but received {}", 1, argc));
		auto source = JS_ToCString(context, argv[0]);
		auto result = Sen::Kernel::Definition::Encryption::Sha224::hash(source);
		JS_FreeCString(context, source);
		return JS::Converter::to_string(context, result);
	}

	/**
	 * ----------------------------------------
	 * JavaScript SHA224 Hash File System
	 * @param argv[0]: source to encode
	 * @returns: Encoded file
	 * ----------------------------------------
	*/

	inline static auto sha224_hash_fs(
		JSContext *context, 
		JSValueConst this_val, 
		int argc, 
		JSValueConst *argv
	) -> JSValue
	{
		try_assert(argc == 1, fmt::format("argument expected {} but received {}", 1, argc));
		auto source = JS_ToCString(context, argv[0]);
		auto result = Sen::Kernel::Definition::Encryption::Sha224::hash_fs(source);
		JS_FreeCString(context, source);
		return JS::Converter::to_string(context, result);
	}

	/**
	 * ----------------------------------------
	 * JavaScript SHA256 Hash
	 * @param argv[0]: source to encode
	 * @returns: Encoded file
	 * ----------------------------------------
	*/

	inline static auto sha256_hash(
		JSContext *context, 
		JSValueConst this_val, 
		int argc, 
		JSValueConst *argv
	) -> JSValue
	{
		try_assert(argc == 1, fmt::format("argument expected {} but received {}", 1, argc));
		auto source = JS_ToCString(context, argv[0]);
		auto result = Sen::Kernel::Definition::Encryption::SHA256::hash(source);
		JS_FreeCString(context, source);
		return JS::Converter::to_string(context, result);
	}

	/**
	 * ----------------------------------------
	 * JavaScript SHA256 Hash File System
	 * @param argv[0]: source to encode
	 * @returns: Encoded file
	 * ----------------------------------------
	*/

	inline static auto sha256_hash_fs(
		JSContext *context, 
		JSValueConst this_val, 
		int argc, 
		JSValueConst *argv
	) -> JSValue
	{
		try_assert(argc == 1, fmt::format("argument expected {} but received {}", 1, argc));
		auto source = JS_ToCString(context, argv[0]);
		auto result = Sen::Kernel::Definition::Encryption::SHA256::hash_fs(source);
		JS_FreeCString(context, source);
		return JS::Converter::to_string(context, result);
	}

	/**
	 * ----------------------------------------
	 * JavaScript SHA384 Hash
	 * @param argv[0]: source to encode
	 * @returns: Encoded file
	 * ----------------------------------------
	*/

	inline static auto sha384_hash(
		JSContext *context, 
		JSValueConst this_val, 
		int argc, 
		JSValueConst *argv
	) -> JSValue
	{
		try_assert(argc == 1, fmt::format("argument expected {} but received {}", 1, argc));
		auto source = JS_ToCString(context, argv[0]);
		auto result = Sen::Kernel::Definition::Encryption::SHA384::hash(source);
		JS_FreeCString(context, source);
		return JS::Converter::to_string(context, result);
	}

	/**
	 * ----------------------------------------
	 * JavaScript SHA384 Hash File System
	 * @param argv[0]: source to encode
	 * @returns: Encoded file
	 * ----------------------------------------
	*/

	inline static auto sha384_hash_fs(
		JSContext *context, 
		JSValueConst this_val, 
		int argc, 
		JSValueConst *argv
	) -> JSValue
	{
		try_assert(argc == 1, fmt::format("argument expected {} but received {}", 1, argc));
		auto source = JS_ToCString(context, argv[0]);
		auto result = Sen::Kernel::Definition::Encryption::SHA384::hash_fs(source);
		JS_FreeCString(context, source);
		return JS::Converter::to_string(context, result);
	}

	/**
	 * ----------------------------------------
	 * JavaScript SHA512 Hash
	 * @param argv[0]: source to encode
	 * @returns: Encoded file
	 * ----------------------------------------
	*/

	inline static auto sha512_hash(
		JSContext *context, 
		JSValueConst this_val, 
		int argc, 
		JSValueConst *argv
	) -> JSValue
	{
		try_assert(argc == 1, fmt::format("argument expected {} but received {}", 1, argc));
		auto source = JS_ToCString(context, argv[0]);
		auto result = Sen::Kernel::Definition::Encryption::SHA512::hash(source);
		JS_FreeCString(context, source);
		return JS::Converter::to_string(context, result);
	}

	/**
	 * ----------------------------------------
	 * JavaScript SHA512 Hash File System
	 * @param argv[0]: source to encode
	 * @returns: Encoded file
	 * ----------------------------------------
	*/

	inline static auto sha512_hash_fs(
		JSContext *context, 
		JSValueConst this_val, 
		int argc, 
		JSValueConst *argv
	) -> JSValue
	{
		try_assert(argc == 1, fmt::format("argument expected {} but received {}", 1, argc));
		auto source = JS_ToCString(context, argv[0]);
		auto result = Sen::Kernel::Definition::Encryption::SHA512::hash_fs(source);
		JS_FreeCString(context, source);
		return JS::Converter::to_string(context, result);
	}

	/**
	 * ----------------------------------------
	 * JavaScript XOR Encryption
	 * @param argv[0]: key
	 * @param argv[1]: plain
	 * @returns: Encoded file
	 * ----------------------------------------
	*/

	inline static auto xor_encrypt(
		JSContext *context, 
		JSValueConst this_val, 
		int argc, 
		JSValueConst *argv
	) -> JSValue
	{
		try_assert(argc == 2, fmt::format("argument expected {} but received {}", 2, argc));
		auto plain = JS_ToCString(context, argv[0]);
		auto key = JS_ToCString(context, argv[1]);
		auto result = Sen::Kernel::Definition::Encryption::XOR::encrypt(plain, key);
		JS_FreeCString(context, plain);
		JS_FreeCString(context, key);
		return JS::Converter::to_string(context, result);
	}

	/**
	 * ----------------------------------------
	 * JavaScript XOR Encryption File System
	 * @param argv[0]: source file
	 * @param argv[1]: destination file
	 * @param argv[2]: key
	 * @returns: Encoded file
	 * ----------------------------------------
	*/

	inline static auto xor_encrypt_fs(
		JSContext *context, 
		JSValueConst this_val, 
		int argc, 
		JSValueConst *argv
	) -> JSValue
	{
		try_assert(argc == 3, fmt::format("argument expected {} but received {}", 3, argc));
		auto source = JS_ToCString(context, argv[0]);
		auto destination = JS_ToCString(context, argv[1]);
		auto key = JS_ToCString(context, argv[2]);
		Sen::Kernel::Definition::Encryption::XOR::encrypt_fs(source, destination, key);
		JS_FreeCString(context, source);
		JS_FreeCString(context, destination);
		JS_FreeCString(context, key);
		return JS::Converter::get_undefined();
	}

	/**
	 * ----------------------------------------
	 * JavaScript Zlib Compression File
	 * @param argv[0]: source file
	 * @param argv[1]: destination file
	 * @param argv[2]: level
	 * @returns: Compressed file
	 * ----------------------------------------
	*/

	inline static auto zlib_compress_fs(
		JSContext *context, 
		JSValueConst this_val, 
		int argc, 
		JSValueConst *argv
	) -> JSValue
	{
		try_assert(argc == 3, fmt::format("argument expected {} but received {}", 3, argc));
		auto source = JS_ToCString(context, argv[0]);
		auto destination = JS_ToCString(context, argv[1]);
		auto level = JS::Converter::get_int32(context, argv[2]);
		if(!(Sen::Kernel::Definition::Compression::Zlib::Level::DEFAULT <= level or level <= Sen::Kernel::Definition::Compression::Zlib::Level::LEVEL_9)){
			JS_FreeCString(context, source);
			JS_FreeCString(context, destination);
			throw std::invalid_argument(fmt::format("Invalid zlib level, expected level from 0 to 9, received {}", level));
		}
		Sen::Kernel::Definition::Compression::Zlib::compress_fs(source, destination, static_cast<Sen::Kernel::Definition::Compression::Zlib::Level>(level));
		JS_FreeCString(context, source);
		JS_FreeCString(context, destination);
		return JS::Converter::get_undefined();
	}

	/**
	 * ----------------------------------------
	 * JavaScript Zlib Uncompression File
	 * @param argv[0]: source file
	 * @param argv[1]: destination file
	 * @returns: Uncompressed file
	 * ----------------------------------------
	*/

	inline static auto zlib_uncompress_fs(
		JSContext *context, 
		JSValueConst this_val, 
		int argc, 
		JSValueConst *argv
	) -> JSValue
	{
		try_assert(argc == 2, fmt::format("argument expected {} but received {}", 2, argc));
		auto source = JS_ToCString(context, argv[0]);
		auto destination = JS_ToCString(context, argv[1]);
		Sen::Kernel::Definition::Compression::Zlib::uncompress_fs(source, destination);
		JS_FreeCString(context, source);
		JS_FreeCString(context, destination);
		return JS::Converter::get_undefined();
	}

	/**
	 * ----------------------------------------
	 * JavaScript Gzip Compression File
	 * @param argv[0]: source file
	 * @param argv[1]: destination file
	 * @returns: Compressed file
	 * ----------------------------------------
	*/

	inline static auto gzip_compress_fs(
		JSContext *context, 
		JSValueConst this_val, 
		int argc, 
		JSValueConst *argv
	) -> JSValue
	{
		try_assert(argc == 2, fmt::format("argument expected {} but received {}", 2, argc));
		auto source = JS_ToCString(context, argv[0]);
		auto destination = JS_ToCString(context, argv[1]);
		Sen::Kernel::Definition::Compression::Zlib::compress_gzip_fs(source, destination);
		JS_FreeCString(context, source);
		JS_FreeCString(context, destination);
		return JS::Converter::get_undefined();
	}

	/**
	 * ----------------------------------------
	 * JavaScript Gzip Uncompression File
	 * @param argv[0]: source file
	 * @param argv[1]: destination file
	 * @returns: Uncompressed file
	 * ----------------------------------------
	*/

	inline static auto gzip_uncompress_fs(
		JSContext *context, 
		JSValueConst this_val, 
		int argc, 
		JSValueConst *argv
	) -> JSValue
	{
		try_assert(argc == 2, fmt::format("argument expected {} but received {}", 2, argc));
		auto source = JS_ToCString(context, argv[0]);
		auto destination = JS_ToCString(context, argv[1]);
		Sen::Kernel::Definition::Compression::Zlib::uncompress_gzip_fs(source, destination);
		JS_FreeCString(context, source);
		JS_FreeCString(context, destination);
		return JS::Converter::get_undefined();
	}

	inline static auto rton_decode_fs(
		JSContext *context, 
		JSValueConst this_val, 
		int argc, 
		JSValueConst *argv
	) -> JSValue
	{
		try_assert(argc == 2, fmt::format("argument expected {} but received {}", 2, argc));
		auto source = JS_ToCString(context, argv[0]);
		auto destination = JS_ToCString(context, argv[1]);
		Sen::Kernel::Support::PopCap::RTON::Decode::decode_fs(source, destination);
		JS_FreeCString(context, source);
		JS_FreeCString(context, destination);
		return JS::Converter::get_undefined();
	}
}