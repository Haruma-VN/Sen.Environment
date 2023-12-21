#pragma once

#include "kernel/interface/shell.hpp"

namespace Sen::Kernel::Interface::Script {

	/**
	 * JS namespace is used
	*/

	namespace JS = Sen::Kernel::Definition::JavaScript;

	/**
	 * JS Declaration
	*/

	typedef JSValue (*JavaScriptNativeMethod)(JSContext *, JSValueConst, int, JSValueConst *);

	/**
	 * JS Print method
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
	 * JS read file method
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
		JS_FreeCString(context, source);
		return JS::Converter::to_string(context, result);
	}

	/**
	 * JS write file method
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
	 * JavaScript evaluate method
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
	 * JavaScript evaluate as file
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
}