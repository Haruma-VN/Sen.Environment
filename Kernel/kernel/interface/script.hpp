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
	 * @param argv[1]: message
	 * @param argv[2]: color
	 * ----------------------------------------
	*/

	inline static auto print(
		JSContext *context, 
		JSValueConst this_val, 
		int argc, 
		JSValueConst *argv
	) -> JSValue
	{
		try_assert(argc == 3, fmt::format("argument expected {} but received {}", 3, argc));
		auto str = JS_ToCString(context, argv[0]);
		Shell{}.print(JS::Converter::get_string(context, argv[0]).c_str(), JS::Converter::get_string(context, argv[1]).c_str(), static_cast<Sen::Kernel::Interface::Color>(JS::Converter::get_int32(context, argv[2])));
		JS_FreeCString(context, str);
		return JS_UNDEFINED;
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
		return JS_UNDEFINED;
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
		auto str = FileSystem::readFile(source);
		auto result = Kernel::Definition::Encryption::MD5::hash(static_cast<std::span<unsigned char>>(String::convertStringToSpan<unsigned char>(str)));
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
		return JS_UNDEFINED;
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
		return JS_UNDEFINED;
	}
}