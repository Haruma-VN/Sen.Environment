#pragma once

#include "kernel/interface/shell.hpp"

#define M_JS_EXCEPTION_THROW(context, error, source, function_name)                       \
	auto evaluate_context = fmt::format("function {}()", function_name); \
	evaluate_context += "{";\
	evaluate_context += fmt::format("let e = Error(`{}`);", error);\
	evaluate_context += fmt::format("e.source = `{}`;", source);\
	evaluate_context += fmt::format("throw e;");\
	evaluate_context += "}";\
	evaluate_context += fmt::format("\n{}();", function_name);\
	return JS_Eval(context, evaluate_context.c_str(), evaluate_context.size(), source.c_str(), JS_EVAL_FLAG_STRICT | JS_EVAL_TYPE_GLOBAL);

#define M_JS_PROXY_WRAPPER(context, code, func_name)                                         \
	try code catch (...)                                                             \
	{                                                                                \
		auto exception = parse_exception();                                          \
		if(exception.function_name == ""){\
			exception.function_name = func_name;\
		}\
		M_JS_EXCEPTION_THROW(context, exception.message(), exception.source, exception.function_name); \
	}

#define M_JS_UNDEFINED_BEHAVIOR(context, jsval, property, function_name)\
	if (JS_IsException(jsval)){\
		M_JS_EXCEPTION_THROW(context, fmt::format("{} \"{}\" {}", Localization::get("js.cannot_read_property"), property, Localization::get("js.of_current_object")), std::string{std::string{std::source_location::current().file_name()} + std::string{":"} + std::to_string(std::source_location::current().line())}, function_name);\
	}


namespace Sen::Kernel::Interface::Script {

	/**
	 * To make sure Language is ambigious
	*/

	namespace Localization = Sen::Kernel::Language;

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
	 * Shell callback
	*/

	inline static auto callback(
		JSContext *context, 
		JSValueConst this_val, 
		int argc, 
		JSValueConst *argv
	) -> JSValue
	{
		M_JS_PROXY_WRAPPER(context, {
			try_assert(argc == 1, fmt::format("{} 1, {}: {}", Kernel::Language::get("kernel.argument_expected"), Kernel::Language::get("kernel.argument_received"), argc));
			assert_conditional(JS_IsArray(context, argv[0]), fmt::format("{} {} {} {}", Kernel::Language::get("kernel.expected_argument"), 0, Kernel::Language::get("is"), Kernel::Language::get("kernel.tuple.js_array")), "callback");
			auto result = Shell::callback(construct_string_list(JS::Converter::get_vector<std::string>(context, argv[0])));
			return JS::Converter::to_string(context, construct_standard_string(result));
		}, "callback"_sv);
	}

	/**
	 * JavaScript Custom JSON Serializer
	*/

	namespace JSON {

		/**
		 * Convert nlohmann::ordered_json to JSValue for quickjs
		*/

		inline static auto json_to_js_value(
			JSContext* context,
			const nlohmann::ordered_json& json
		) -> JSValue
		{
			switch (json.type()) {
			case nlohmann::ordered_json::value_t::null: {
				return JS::Converter::get_null();
			}
			case nlohmann::ordered_json::value_t::object: {
				auto js_obj = JS_NewObject(context);
				for (auto& [key, value] : json.items()) {
					JS_DefinePropertyValueStr(context, js_obj, key.c_str(), json_to_js_value(context, value), JS_PROP_C_W_E);
				}
				return js_obj;
			}
			case nlohmann::ordered_json::value_t::array: {
				auto js_arr = JS_NewArray(context);
				for (auto i : Range<size_t>(json.size())) {
					JS_DefinePropertyValueUint32(context, js_arr, i, json_to_js_value(context, json[i]), JS_PROP_C_W_E);
				}
				return js_arr;
			}
			case nlohmann::ordered_json::value_t::string: {
				return JS_NewString(context, json.get<std::string>().c_str());
			}
			case nlohmann::ordered_json::value_t::boolean: {
				return JS_NewBool(context, json.get<bool>());
			}
			case nlohmann::ordered_json::value_t::number_integer: {
				return JS_NewBigInt64(context, json.get<int64_t>());
			}
			case nlohmann::ordered_json::value_t::number_unsigned: {
				return JS_NewBigInt64(context, json.get<uint64_t>());
			}
			case nlohmann::ordered_json::value_t::number_float: {
				return JS_NewFloat64(context, json.get<double>());
			}
			default: {
				return JS::Converter::get_undefined();
			}
			}
		}

		/**
		 * ----------------------------------------
		 * JavaScript JSON Deserializer
		 * @param argv[0]: JS String
		 * @returns: Deserialized object
		 * ----------------------------------------
		*/

		inline static auto deserialize(
			JSContext* context,
			JSValueConst this_val,
			int argc,
			JSValueConst* argv
		) -> JSValue
		{
			M_JS_PROXY_WRAPPER(context, {
				try_assert(argc == 1, fmt::format("{} 1, {}: {}", Kernel::Language::get("kernel.argument_expected"), Kernel::Language::get("kernel.argument_received"), argc));
				auto source = JS::Converter::get_string(context, argv[0]);
				auto json = nlohmann::ordered_json::parse(source);
				auto js_obj = json_to_js_value(context, json);
				return js_obj;
				}, "deserialize"_sv);
		}

		/**
		 * ----------------------------------------
		 * JavaScript JSON Deserializer
		 * @param argv[0]: JS String
		 * @returns: Deserialized object
		 * ----------------------------------------
		*/

		inline static auto deserialize_fs(
			JSContext* context,
			JSValueConst this_val,
			int argc,
			JSValueConst* argv
		) -> JSValue
		{
			M_JS_PROXY_WRAPPER(context, {
				try_assert(argc == 1, fmt::format("{} 1, {}: {}", Kernel::Language::get("kernel.argument_expected"), Kernel::Language::get("kernel.argument_received"), argc));
				auto source = JS::Converter::get_c_string(context, argv[0]);
				auto json = Sen::Kernel::FileSystem::read_json(source.get());
				auto js_obj = json_to_js_value(context, json);
				return js_obj;
				}, "deserialize_fs"_sv);
		}

		/**
		 * QuickJS JSON Value to nlohmann::ordered_json and then to string
		*/

		inline static auto js_object_to_json(
			JSContext* context,
			JSValueConst value
		) -> nlohmann::ordered_json
		{
			switch (JS_VALUE_GET_TAG(value)) {
			case JS_TAG_UNDEFINED: {
				return nullptr;
			}
			case JS_TAG_NULL: {
				return nullptr;
			}
			case JS_TAG_OBJECT: {
				if (JS_IsArray(context, value)) {
					auto json = nlohmann::ordered_json::array();
					auto length = uint32_t{};
					JS_ToUint32(context, &length, JS_GetPropertyStr(context, value, "length"));
					for (auto i : Range<uint32_t>(length)) {
						auto val = JS_GetPropertyUint32(context, value, i);
						json.push_back(js_object_to_json(context, val));
						JS_FreeValue(context, val);
					}
					return json;
				}
				else if (JS_IsObject(value)) {
					auto json = nlohmann::ordered_json::object();
					auto* tab = static_cast<JSPropertyEnum*>(nullptr);
					auto tab_size = uint32_t{};
					if (JS_GetOwnPropertyNames(context, &tab, &tab_size, value, JS_GPN_STRING_MASK | JS_GPN_ENUM_ONLY) == 0) {
						for (auto i : Range<uint32_t>(tab_size)) {
							auto key = JS_AtomToCString(context, tab[i].atom);
							auto val = JS_GetProperty(context, value, tab[i].atom);
							if (JS_VALUE_GET_TAG(val) != JS_TAG_UNDEFINED) {
								json[key] = js_object_to_json(context, val);
							}
							JS_FreeAtom(context, tab[i].atom);
							JS_FreeValue(context, val);
						}
						js_free(context, tab);
					}
					return json;
				}
				else {
					throw Exception("Unknown type");
				}
			}
			case JS_TAG_STRING: {
				auto str = JS::Converter::get_string(context, value);
				auto json = nlohmann::ordered_json(str);
				return json;
			}
			case JS_TAG_BOOL: {
				return nlohmann::ordered_json(JS_VALUE_GET_BOOL(value) == 0 ? false : true);
			}
			case JS_TAG_INT: {
				return nlohmann::ordered_json(static_cast<double>(JS_VALUE_GET_INT(value)));
			}
			case JS_TAG_FLOAT64: {
				return nlohmann::ordered_json(JS_VALUE_GET_FLOAT64(value));
			}
			case JS_TAG_BIG_INT: {
				auto val = int64_t{};
				JS_ToBigInt64(context, &val, value);
				return nlohmann::ordered_json(val);
			}
			}
		}

		/**
		 * ----------------------------------------
		 * JavaScript JSON Serializer
		 * @param argv[0]: JSValue
		 * @param argv[1]: indent
		 * @param argv[2]: char
		 * @param argv[3]: ensure ascii
		 * @returns: Serialized object
		 * ----------------------------------------
		*/

		inline static auto serialize(
			JSContext* context,
			JSValueConst this_val,
			int argc,
			JSValueConst* argv
		) -> JSValue
		{
			M_JS_PROXY_WRAPPER(context, {
				try_assert(argc == 3, fmt::format("argument expected {} but received {}", 3, argc));
				auto json = js_object_to_json(context, argv[0]);
				auto indent = JS::Converter::get_int32(context, argv[1]);
				auto ensure_ascii = JS::Converter::get_bool(context, argv[2]);;
				auto source = json.dump(indent, '\t', ensure_ascii);
				auto js_val = JS_NewString(context, source.c_str());
				return js_val;
				}, "serialize"_sv);
		}

		/**
		 * ----------------------------------------
		 * JavaScript JSON Serializer
		 * @param argv[0]: Destination
		 * @param argv[1]: json
		 * @param argv[2]: indent
		 * @param argv[3]: ensure ascii
		 * @returns: Serialized object
		 * ----------------------------------------
		*/

		inline static auto serialize_fs(
			JSContext* context,
			JSValueConst this_val,
			int argc,
			JSValueConst* argv
		) -> JSValue
		{
			M_JS_PROXY_WRAPPER(context, {
				try_assert(argc == 4, fmt::format("argument expected {} but received {}", 4, argc));
				auto destination = JS::Converter::get_string(context, argv[0]);
				auto json = js_object_to_json(context, argv[1]);
				auto indent = JS::Converter::get_int32(context, argv[2]);
				auto ensure_ascii = JS::Converter::get_bool(context, argv[3]);
				;
				auto result = json.dump(indent, '\t', ensure_ascii);
				Sen::Kernel::FileSystem::write_file(destination, result);
				return JS::Converter::get_undefined();
				}, "serialize_fs"_sv);
		}
	}

	/**
	 * Language Support
	*/

	namespace Language {

		/**
		 * Script need to load language before launch whole program
		*/
		
		inline static auto load_language(
			JSContext *context, 
			JSValueConst this_val, 
			int argc, 
			JSValueConst *argv
		) -> JSValue
		{
			M_JS_PROXY_WRAPPER(context, {
				try_assert(argc == 1, fmt::format("{} 1, {}: {}", Kernel::Language::get("kernel.argument_expected"), Kernel::Language::get("kernel.argument_received"), argc));
				assert_conditional(JS_IsString(argv[0]), fmt::format("{} {} {} {}", Kernel::Language::get("kernel.expected_argument"), 0, Kernel::Language::get("is"), Kernel::Language::get("kernel.tuple.js_string")), "load_language");
				Sen::Kernel::Language::read_language(JS::Converter::get_c_string(context, argv[0]).get());
				return JS::Converter::get_undefined();
			}, "load_language"_sv);
		}

		/**
		 * Script or Kernel can get language from this method
		*/

		inline static auto get(
			JSContext *context, 
			JSValueConst this_val, 
			int argc, 
			JSValueConst *argv
		) -> JSValue
		{
			M_JS_PROXY_WRAPPER(context, {
				try_assert(argc == 1, fmt::format("{} 1, {}: {}", Kernel::Language::get("kernel.argument_expected"), Kernel::Language::get("kernel.argument_received"), argc));
				assert_conditional(JS_IsString(argv[0]), fmt::format("{} {} {} {}", Kernel::Language::get("kernel.expected_argument"), 0, Kernel::Language::get("is"), Kernel::Language::get("kernel.tuple.js_string")), "get");
				auto result = Sen::Kernel::Language::get(JS::Converter::get_c_string(context, argv[0]).get());
				return JS::Converter::to_string(context, result.data());
			}, "get"_sv);
		}

	}

	/**
	 * JavaScript Thread
	*/

	namespace Thread {

		/**
		 * ----------------------------------------
		 * JavaScript Sleep
		 * @param argv[0]: time to sleep
		 * @return: undefined
		 * ----------------------------------------
		*/

		inline static auto sleep(
			JSContext *context, 
			JSValueConst this_val, 
			int argc, 
			JSValueConst *argv
		) -> JSValue
		{
			M_JS_PROXY_WRAPPER(context, {
				try_assert(argc == 1, fmt::format("{} 1, {}: {}", Kernel::Language::get("kernel.argument_expected"), Kernel::Language::get("kernel.argument_received"), argc));
				assert_conditional(JS_IsBigInt(context, argv[0]), fmt::format("{} {} {} {}", Kernel::Language::get("kernel.expected_argument"), 0, Kernel::Language::get("is"), Kernel::Language::get("kernel.tuple.js_bigint")), "sleep");
				auto time = JS::Converter::get_bigint64(context, argv[0]);
				Sen::Kernel::Definition::Timer::sleep(time);
				return JS::Converter::get_undefined();
			}, "sleep"_sv);
		}

		/**
		 * ----------------------------------------
		 * JavaScript Date now
		 * @return: undefined
		 * ----------------------------------------
		*/

		inline static auto now(
			JSContext *context, 
			JSValueConst this_val, 
			int argc, 
			JSValueConst *argv
		) -> JSValue
		{
			M_JS_PROXY_WRAPPER(context, {
				try_assert(argc == 0, fmt::format("{} 0, {}: {}", Kernel::Language::get("kernel.argument_expected"), Kernel::Language::get("kernel.argument_received"), argc));
				auto current = std::chrono::duration<double>(std::chrono::system_clock::now().time_since_epoch()).count();
				return JS::Converter::to_number(context, current);
			}, "now"_sv);
		}

	}

	/**
	 * Diff namespace
	*/

	namespace Diff {

		/**
		 * VCDiff interface
		*/

		namespace VCDiff {

			/**
			 * Encode VCDiff
			*/

			inline static auto encode_fs(
				JSContext *context, 
				JSValueConst this_val, 
				int argc, 
				JSValueConst *argv
			) -> JSValue
			{
				M_JS_PROXY_WRAPPER(context, {
					try_assert(argc == 4, fmt::format("{} 4, {}: {}", Kernel::Language::get("kernel.argument_expected"), Kernel::Language::get("kernel.argument_received"), argc));
					assert_conditional(JS_IsString(argv[0]), fmt::format("{} {} {} {}", Kernel::Language::get("kernel.expected_argument"), 0, Kernel::Language::get("is"), Kernel::Language::get("kernel.tuple.js_string")), "encode_fs");
					assert_conditional(JS_IsString(argv[1]), fmt::format("{} {} {} {}", Kernel::Language::get("kernel.expected_argument"), 1, Kernel::Language::get("is"), Kernel::Language::get("kernel.tuple.js_string")), "encode_fs");
					assert_conditional(JS_IsString(argv[2]), fmt::format("{} {} {} {}", Kernel::Language::get("kernel.expected_argument"), 2, Kernel::Language::get("is"), Kernel::Language::get("kernel.tuple.js_string")), "encode_fs");
					assert_conditional(JS_IsNumber(argv[3]), fmt::format("{} {} {} {}", Kernel::Language::get("kernel.expected_argument"), 3, Kernel::Language::get("is"), Kernel::Language::get("kernel.tuple.js_number")), "encode_fs");
					auto before_file = JS::Converter::get_c_string(context, argv[0]);
					auto after_file = JS::Converter::get_c_string(context, argv[1]);
					auto patch_file = JS::Converter::get_c_string(context, argv[2]);
					auto flag = JS::Converter::get_int32(context, argv[3]);
					Sen::Kernel::Definition::Diff::VCDiff::encode_fs(before_file.get(), after_file.get(), patch_file.get(), static_cast<Sen::Kernel::Definition::Diff::VCDiff::Flag>(flag));
					return JS::Converter::get_undefined();
				}, "encode_fs"_sv);
			}

			/**
			 * Decode VCDiff
			*/

			inline static auto decode_fs(
				JSContext *context, 
				JSValueConst this_val, 
				int argc, 
				JSValueConst *argv
			) -> JSValue
			{
				M_JS_PROXY_WRAPPER(context, {
					try_assert(argc == 3, fmt::format("{} 3, {}: {}", Kernel::Language::get("kernel.argument_expected"), Kernel::Language::get("kernel.argument_received"), argc));
					assert_conditional(JS_IsString(argv[0]), fmt::format("{} {} {} {}", Kernel::Language::get("kernel.expected_argument"), 0, Kernel::Language::get("is"), Kernel::Language::get("kernel.tuple.js_string")), "decode_fs");
					assert_conditional(JS_IsString(argv[1]), fmt::format("{} {} {} {}", Kernel::Language::get("kernel.expected_argument"), 1, Kernel::Language::get("is"), Kernel::Language::get("kernel.tuple.js_string")), "decode_fs");
					assert_conditional(JS_IsString(argv[2]), fmt::format("{} {} {} {}", Kernel::Language::get("kernel.expected_argument"), 2, Kernel::Language::get("is"), Kernel::Language::get("kernel.tuple.js_string")), "decode_fs");
					auto before_file = JS::Converter::get_string(context, argv[0]);
					auto patch_file = JS::Converter::get_string(context, argv[1]);
					auto after_file = JS::Converter::get_string(context, argv[2]);
					Sen::Kernel::Definition::Diff::VCDiff::decode_fs(before_file, after_file, patch_file);
					return JS::Converter::get_undefined();
				}, "decode_fs"_sv);
			}

		}

	}

	/**
	 * Operating System
	*/

	namespace OperatingSystem {

		/**
		 * ----------------------------------------
		 * JavaScript Get current OS
		 * @return: current OS
		 * ----------------------------------------
		*/

		inline static auto current(
			JSContext *context, 
			JSValueConst this_val, 
			int argc, 
			JSValueConst *argv
		) -> JSValue
		{
			#if WINDOWS
				return JS::Converter::to_string(context, std::string{"Windows"});
			#elif LINUX
				return JS::Converter::to_string(context, std::string{"Linux"});
			#elif APPLE
				return JS::Converter::to_string(context, std::string{"Macintosh"});
			#elif ANDROID
				return JS::Converter::to_string(context, std::string{"Android"});
			#elif IPHONE
				return JS::Converter::to_string(context, std::string{"iPhone"});
			#else
				return JS::Converter::to_string(context, std::string{"Unknown OS"});
			#endif
		}	

		#if WINDOWS
		struct FileDeleter {
			inline auto operator()(
				FILE* file
			) const -> void
			{
				if (file) {
					std::fclose(file);
					file = nullptr;
				}
				return;
			}
		};
		#else
		struct FileDeleter {
			inline auto operator()(
				FILE* file
			) const -> void
			{
				if (file) {
					pclose(file);
					file = nullptr;
				}
				return;
			}
		};
		#endif

		/**
		 * ----------------------------------------
		 * JavaScript Get current Architecture
		 * @return: current Architecture
		 * ----------------------------------------
		*/

		inline static auto architecture(
			JSContext *context, 
			JSValueConst this_val, 
			int argc, 
			JSValueConst *argv
		) -> JSValue
		{
			#if WINDOWS
					auto si = SYSTEM_INFO{};
					GetNativeSystemInfo(&si);
					switch (si.wProcessorArchitecture)
					{
						case PROCESSOR_ARCHITECTURE_AMD64:
						{
							return JS::Converter::to_string(context, std::string{"x64"});
						}
						case PROCESSOR_ARCHITECTURE_ARM:
						{
							return JS::Converter::to_string(context, std::string{"x86"});
						}
						case PROCESSOR_ARCHITECTURE_IA64:
						{
							return JS::Converter::to_string(context, std::string{"arm64"});
						}
						case PROCESSOR_ARCHITECTURE_INTEL:
						{
							return JS::Converter::to_string(context, std::string{"arm"});
						}
						default:
						{
							return JS::Converter::to_string(context, std::string{"Unknown"});
						}
					}
				#else
					auto buffer = std::array<char, 128>();
					auto result = std::string();
					auto pipe = std::unique_ptr<FILE, FileDeleter>(popen("uname -m", "r"));
					if (!pipe)
					{
						throw Exception("cannot open pipe", std::source_location::current(), "architecture");
					}
					while (fgets(buffer.data(), 128, pipe.get()) != nullptr)
					{
						result += buffer.data();
					}
					result.erase(std::remove(result.begin(), result.end(), '\n'), result.end());
					if (result == "x86_64")
					{
						return JS::Converter::to_string(context, std::string{"x64"});
					}
					else if (result == "i686")
					{
						return JS::Converter::to_string(context, std::string{"x86"});
					}
					else if (result == "aarch64")
					{
						return JS::Converter::to_string(context, std::string{"arm64"});
					}
					else if (result == "armv7l")
					{
						return JS::Converter::to_string(context, std::string{"arm"});
					}
					else
					{
						return JS::Converter::to_string(context, std::string{"Unknown"});
					}
			#endif
		}	

	}

	namespace ArrayBuffer {

		/**
		 * ----------------------------------------
		 * JavaScript Array Buffer open file
		 * @param argv[0]: source file
		 * @return: Array Buffer
		 * ----------------------------------------
		*/

		inline static auto open(
			JSContext *context, 
			JSValueConst this_val, 
			int argc, 
			JSValueConst *argv
		) -> JSValue
		{
			M_JS_PROXY_WRAPPER(context, {
				try_assert(argc == 1, fmt::format("{} 1, {}: {}", Kernel::Language::get("kernel.argument_expected"), Kernel::Language::get("kernel.argument_received"), argc));
				assert_conditional(JS_IsString(argv[0]), fmt::format("{} {} {} {}", Kernel::Language::get("kernel.expected_argument"), 0, Kernel::Language::get("is"), Kernel::Language::get("kernel.tuple.js_string")), "open");
				auto source = JS::Converter::get_c_string(context, argv[0]);
				auto result = JS::Converter::read_file_as_js_arraybuffer(context, source.get());
				return result;
			}, "open"_sv);
		}

		/**
		 * ----------------------------------------
		 * JavaScript Array Buffer open file
		 * @param argv[0]: destination file
		 * @param argv[1]: Array Buffer
		 * @return: undefined
		 * ----------------------------------------
		*/

		inline static auto out(
			JSContext *context, 
			JSValueConst this_val, 
			int argc, 
			JSValueConst *argv
		) -> JSValue
		{
			M_JS_PROXY_WRAPPER(context, {
				try_assert(argc == 2, fmt::format("{} 2, {}: {}", Kernel::Language::get("kernel.argument_expected"), Kernel::Language::get("kernel.argument_received"), argc));
				assert_conditional(JS_IsString(argv[0]), fmt::format("{} {} {} {}", Kernel::Language::get("kernel.expected_argument"), 0, Kernel::Language::get("is"), Kernel::Language::get("kernel.tuple.js_string")), "out");
				auto destination = JS::Converter::get_c_string(context, argv[0]);
				auto array_buffer = argv[1];
				JS::Converter::write_file_as_arraybuffer(context, destination.get(), array_buffer);
				return JS::Converter::get_undefined();
			}, "out"_sv);
		}

	}

	/**
	 * Sub-Process call
	*/

	namespace Process {

		/**
		 * ----------------------------------------
		 * JavaScript Spawn Process
		 * @param argv[0]: process command
		 * @return: undefined
		 * ----------------------------------------
		*/

		inline static auto run(
			JSContext *context, 
			JSValueConst this_val, 
			int argc, 
			JSValueConst *argv
		) -> JSValue
		{
			M_JS_PROXY_WRAPPER(context, {
				try_assert(argc == 1, fmt::format("{} 1, {}: {}", Kernel::Language::get("kernel.argument_expected"), Kernel::Language::get("kernel.argument_received"), argc));
				assert_conditional(JS_IsString(argv[0]), fmt::format("{} {} {} {}", Kernel::Language::get("kernel.expected_argument"), 0, Kernel::Language::get("is"), Kernel::Language::get("kernel.tuple.js_string")), "run");
				auto source = JS::Converter::get_c_string(context, argv[0]);
				Sen::Kernel::Process::run(source.get());
				return JS::Converter::get_undefined();
			}, "run"_sv);
		}

		/**
		 * ----------------------------------------
		 * JavaScript Check Environment
		 * @param argv[0]: process
		 * @return: undefined
		 * ----------------------------------------
		*/

		inline static auto is_exists_in_path_environment(
			JSContext *context, 
			JSValueConst this_val, 
			int argc, 
			JSValueConst *argv
		) -> JSValue
		{
			M_JS_PROXY_WRAPPER(context, {
				try_assert(argc == 1, fmt::format("{} 1, {}: {}", Kernel::Language::get("kernel.argument_expected"), Kernel::Language::get("kernel.argument_received"), argc));
				assert_conditional(JS_IsString(argv[0]), fmt::format("{} {} {} {}", Kernel::Language::get("kernel.expected_argument"), 0, Kernel::Language::get("is"), Kernel::Language::get("kernel.tuple.js_string")), "is_exists_in_path_environment");
				auto source = JS::Converter::get_string(context, argv[0]);
				auto result = Sen::Kernel::Process::is_exists_in_path_environment(source);
				return JS::Converter::to_bool(context, result);
			}, "is_exists_in_path_environment"_sv);
		}

		/**
		 * ----------------------------------------
		 * JavaScript Spawn Process
		 * @param argv[0]: process command
		 * @return: string return
		 * ----------------------------------------
		*/

		inline static auto execute(
			JSContext *context, 
			JSValueConst this_val, 
			int argc, 
			JSValueConst *argv
		) -> JSValue
		{
			M_JS_PROXY_WRAPPER(context, {
				try_assert(argc == 1, fmt::format("{} 1, {}: {}", Kernel::Language::get("kernel.argument_expected"), Kernel::Language::get("kernel.argument_received"), argc));
				assert_conditional(JS_IsString(argv[0]), fmt::format("{} {} {} {}", Kernel::Language::get("kernel.expected_argument"), 0, Kernel::Language::get("is"), Kernel::Language::get("kernel.tuple.js_string")), "execute");
				auto source = JS::Converter::get_string(context, argv[0]);
				auto result = Sen::Kernel::Process::execute(source);
				return JS::Converter::to_string(context, result);
			}, "evaluate"_sv);
		}

	}

	/**
	 * JavaScript Console Interactive
	*/

	namespace Console {

		inline static auto exchange_color(
			Sen::Kernel::Interface::Color color
		) -> std::string
		{
			using Color = Sen::Kernel::Interface::Color;
			if (color == Color::RED) {
				return "red";
			}
			if (color == Color::GREEN) {
				return "green";
			}
			if (color == Color::CYAN) {
				return "cyan";
			}
			if (color == Color::YELLOW) {
				return "yellow";
			}
			return "default";
		}

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
			// todo
			M_JS_PROXY_WRAPPER(context, {
				try_assert(argc >= 1, fmt::format("argument expected greater than {} but received {}", "1", argc));
				switch (argc)
				{
					case 1:
					{
						Shell::callback(construct_string_list(std::vector<std::string>{std::string{"display"}, JS::Converter::get_string(context, argv[0])}));
						break;
					}
					case 2:
					{
						Shell::callback(construct_string_list(std::vector<std::string>{std::string{"display"}, JS::Converter::get_string(context, argv[0]), JS::Converter::get_string(context, argv[1])}));
						break;
					}
					default:
					{
						Shell::callback(construct_string_list(std::vector<std::string>{std::string{"display"}, JS::Converter::get_string(context, argv[0]), JS::Converter::get_string(context, argv[1]), exchange_color(static_cast<Sen::Kernel::Interface::Color>(JS::Converter::get_int32(context, argv[2])))}));
						break;
					}
				}
				return JS::Converter::get_undefined();
			}, "print"_sv);
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
			M_JS_PROXY_WRAPPER(context, {
				try_assert(argc == 0, fmt::format("argument expected {} but received {}", 0, argc));
				Shell::callback(construct_string_list(std::vector<std::string>{std::string{"wait"}}));
				auto result = Shell::callback(construct_string_list(std::vector<std::string>{std::string{"input"}}));
				return JS::Converter::to_string(context, std::string{result.value, result.size});
			}, "readline"_sv);
		}
	}

	namespace Path {

		/**
		 * ----------------------------------------
		 * JS join path method
		 * @return: joined file
		 * ----------------------------------------
		*/

		inline static auto join(
			JSContext *context, 
			JSValueConst this_val, 
			int argc, 
			JSValueConst *argv
		) -> JSValue
		{
			M_JS_PROXY_WRAPPER(context, {
				auto v = std::vector<std::string>{};
				for(auto i : Range<int>(argc))
				{
					assert_conditional(JS_IsString(argv[i]), fmt::format("{} {} {} {}", Kernel::Language::get("kernel.expected_argument"), i, Kernel::Language::get("is"), Kernel::Language::get("kernel.tuple.js_string")), "join");
					auto source = JS::Converter::get_c_string(context, argv[i]);
					v.emplace_back(source.get());
				}
				return JS::Converter::to_string(context, Sen::Kernel::Path::Script::join(v));
			}, "join"_sv);
		}

		/**
		 * ----------------------------------------
		 * JS Basename for file
		 * @param argv[0]: source file
		 * @return: base name
		 * ----------------------------------------
		*/

		inline static auto basename(
			JSContext *context, 
			JSValueConst this_val, 
			int argc, 
			JSValueConst *argv
		) -> JSValue
		{
			M_JS_PROXY_WRAPPER(context, {
				try_assert(argc == 1, fmt::format("{} 1, {}: {}", Kernel::Language::get("kernel.argument_expected"), Kernel::Language::get("kernel.argument_received"), argc));
				assert_conditional(JS_IsString(argv[0]), fmt::format("{} {} {} {}", Kernel::Language::get("kernel.expected_argument"), 0, Kernel::Language::get("is"), Kernel::Language::get("kernel.tuple.js_string")), "basename");
				auto source = JS::Converter::get_string(context, argv[0]);
				auto result = Sen::Kernel::Path::Script::basename(source);
				return JS::Converter::to_string(context, result);
			}, "basename"_sv);
		}

		/**
		 * ----------------------------------------
		 * JS Delimiter of current OS
		 * @param argv[0]: source file
		 * @return: Delimiter of current OS
		 * ----------------------------------------
		*/

		inline static auto delimiter(
			JSContext *context, 
			JSValueConst this_val, 
			int argc, 
			JSValueConst *argv
		) -> JSValue
		{
			M_JS_PROXY_WRAPPER(context, {
				try_assert(argc == 0, fmt::format("{} 0, {}: {}", Kernel::Language::get("kernel.argument_expected"), Kernel::Language::get("kernel.argument_received"), argc));
				auto result = Sen::Kernel::Path::Script::delimiter();
				return JS::Converter::to_string(context, result);
			}, "delimiter"_sv);
		}

		/**
		 * ----------------------------------------
		 * JS Dirname
		 * @param argv[0]: source file
		 * @return: dirname
		 * ----------------------------------------
		*/

		inline static auto dirname(
			JSContext *context, 
			JSValueConst this_val, 
			int argc, 
			JSValueConst *argv
		) -> JSValue
		{
			M_JS_PROXY_WRAPPER(context, {
				try_assert(argc == 1, fmt::format("{} 1, {}: {}", Kernel::Language::get("kernel.argument_expected"), Kernel::Language::get("kernel.argument_received"), argc));
				assert_conditional(JS_IsString(argv[0]), fmt::format("{} {} {} {}", Kernel::Language::get("kernel.expected_argument"), 0, Kernel::Language::get("is"), Kernel::Language::get("kernel.tuple.js_string")), "dirname");
				auto source = JS::Converter::get_string(context, argv[0]);
				auto result = Sen::Kernel::Path::Script::dirname(source);
				return JS::Converter::to_string(context, result);
			}, "dirname"_sv);
		}

		/**
		 * ----------------------------------------
		 * JS Format
		 * @param argv[0]: source
		 * @return: formatted
		 * ----------------------------------------
		*/

		inline static auto format(
			JSContext *context, 
			JSValueConst this_val, 
			int argc, 
			JSValueConst *argv
		) -> JSValue
		{
			M_JS_PROXY_WRAPPER(context, {
				try_assert(argc == 1, fmt::format("{} 1, {}: {}", Kernel::Language::get("kernel.argument_expected"), Kernel::Language::get("kernel.argument_received"), argc));
				assert_conditional(JS_IsObject(argv[0]), fmt::format("{} {} {} {}", Kernel::Language::get("kernel.expected_argument"), 0, Kernel::Language::get("is"), Kernel::Language::get("kernel.tuple.js_object")), "format");
				auto dir = JS_GetPropertyStr(context, argv[0], "dir");
				auto base = JS_GetPropertyStr(context, argv[0], "base");
				auto source_1 = JS::Converter::get_string(context, dir);
				auto source_2 = JS::Converter::get_string(context, base);
				auto result = Sen::Kernel::Path::Script::format(Sen::Kernel::Path::Format{source_1, source_2});
				return JS::Converter::to_string(context, result);
			}, "format"_sv);
		}

		/**
		 * ----------------------------------------
		 * JS Normalize
		 * @param argv[0]: source file
		 * @return: normalize
		 * ----------------------------------------
		*/

		inline static auto normalize(
			JSContext *context, 
			JSValueConst this_val, 
			int argc, 
			JSValueConst *argv
		) -> JSValue
		{
			M_JS_PROXY_WRAPPER(context, {
				try_assert(argc == 1, fmt::format("{} 1, {}: {}", Kernel::Language::get("kernel.argument_expected"), Kernel::Language::get("kernel.argument_received"), argc));
				assert_conditional(JS_IsString(argv[0]), fmt::format("{} {} {} {}", Kernel::Language::get("kernel.expected_argument"), 0, Kernel::Language::get("is"), Kernel::Language::get("kernel.tuple.js_string")), "normalize");
				auto source = JS::Converter::get_string(context, argv[0]);
				auto result = Sen::Kernel::Path::Script::normalize(source);
				return JS::Converter::to_string(context, result);
			}, "normalize"_sv);
		}

		/**
		 * ----------------------------------------
		 * JS Normalize
		 * @param argv[0]: source file
		 * @return: normalize
		 * ----------------------------------------
		*/

		inline static auto base_without_extension(
			JSContext *context, 
			JSValueConst this_val, 
			int argc, 
			JSValueConst *argv
		) -> JSValue
		{
			M_JS_PROXY_WRAPPER(context, {
				try_assert(argc == 1, fmt::format("{} 1, {}: {}", Kernel::Language::get("kernel.argument_expected"), Kernel::Language::get("kernel.argument_received"), argc));
				assert_conditional(JS_IsString(argv[0]), fmt::format("{} {} {} {}", Kernel::Language::get("kernel.expected_argument"), 0, Kernel::Language::get("is"), Kernel::Language::get("kernel.tuple.js_string")), "base_without_extension");
				auto source = JS::Converter::get_string(context, argv[0]);
				auto result = Sen::Kernel::Path::Script::base_without_extension(source);
				return JS::Converter::to_string(context, result);
			}, "base_without_extension"_sv);
		}

		/**
		 * ----------------------------------------
		 * JS Normalize
		 * @param argv[0]: source file
		 * @return: normalize
		 * ----------------------------------------
		*/

		inline static auto except_extension(
			JSContext *context, 
			JSValueConst this_val, 
			int argc, 
			JSValueConst *argv
		) -> JSValue
		{
			M_JS_PROXY_WRAPPER(context, {
				try_assert(argc == 1, fmt::format("{} 1, {}: {}", Kernel::Language::get("kernel.argument_expected"), Kernel::Language::get("kernel.argument_received"), argc));
				assert_conditional(JS_IsString(argv[0]), fmt::format("{} {} {} {}", Kernel::Language::get("kernel.expected_argument"), 0, Kernel::Language::get("is"), Kernel::Language::get("kernel.tuple.js_string")), "except_extension");
				auto source = JS::Converter::get_string(context, argv[0]);
				auto result = Sen::Kernel::Path::Script::except_extension(source);
				return JS::Converter::to_string(context, result);
			}, "except_extension"_sv);
		}

		/**
		 * ----------------------------------------
		 * JS Resolve
		 * @param argv[0]: source file
		 * @return: resolve
		 * ----------------------------------------
		*/

		inline static auto resolve(
			JSContext *context, 
			JSValueConst this_val, 
			int argc, 
			JSValueConst *argv
		) -> JSValue
		{
			M_JS_PROXY_WRAPPER(context, {
				try_assert(argc == 1, fmt::format("{} 1, {}: {}", Kernel::Language::get("kernel.argument_expected"), Kernel::Language::get("kernel.argument_received"), argc));
				assert_conditional(JS_IsString(argv[0]), fmt::format("{} {} {} {}", Kernel::Language::get("kernel.expected_argument"), 0, Kernel::Language::get("is"), Kernel::Language::get("kernel.tuple.js_string")), "resolve");
				auto source = JS::Converter::get_string(context, argv[0]);
				auto result = Sen::Kernel::Path::Script::resolve(source);
				return JS::Converter::to_string(context, result);
			}, "resolve"_sv);
		}

		/**
		 * ----------------------------------------
		 * JS Extension name
		 * @param argv[0]: source file
		 * @return: extname
		 * ----------------------------------------
		*/

		inline static auto extname(
			JSContext *context, 
			JSValueConst this_val, 
			int argc, 
			JSValueConst *argv
		) -> JSValue
		{
			M_JS_PROXY_WRAPPER(context, {
				try_assert(argc == 1, fmt::format("{} 1, {}: {}", Kernel::Language::get("kernel.argument_expected"), Kernel::Language::get("kernel.argument_received"), argc));
				assert_conditional(JS_IsString(argv[0]), fmt::format("{} {} {} {}", Kernel::Language::get("kernel.expected_argument"), 0, Kernel::Language::get("is"), Kernel::Language::get("kernel.tuple.js_string")), "extname");
				auto source = JS::Converter::get_string(context, argv[0]);
				auto result = Sen::Kernel::Path::Script::extname(source);
				return JS::Converter::to_string(context, result);
			}, "extname"_sv);
		}

		/**
		 * ----------------------------------------
		 * JS Is absolute
		 * @param argv[0]: source file
		 * @return: is absolute path
		 * ----------------------------------------
		*/

		inline static auto is_absolute(
			JSContext *context, 
			JSValueConst this_val, 
			int argc, 
			JSValueConst *argv
		) -> JSValue
		{
			M_JS_PROXY_WRAPPER(context, {
				try_assert(argc == 1, fmt::format("{} 1, {}: {}", Kernel::Language::get("kernel.argument_expected"), Kernel::Language::get("kernel.argument_received"), argc));
				assert_conditional(JS_IsString(argv[0]), fmt::format("{} {} {} {}", Kernel::Language::get("kernel.expected_argument"), 0, Kernel::Language::get("is"), Kernel::Language::get("kernel.tuple.js_string")), "is_absolute");
				auto source = JS::Converter::get_string(context, argv[0]);
				auto result = Sen::Kernel::Path::Script::is_absolute(source);
				return JS::Converter::to_bool(context, result);
			}, "is_absolute"_sv);
		}

		/**
		 * ----------------------------------------
		 * JS Relative
		 * @param argv[0]: source file
		 * @param argv[0]: source file
		 * @return: Relative
		 * ----------------------------------------
		*/

		inline static auto relative(
			JSContext *context, 
			JSValueConst this_val, 
			int argc, 
			JSValueConst *argv
		) -> JSValue
		{
			M_JS_PROXY_WRAPPER(context, {
				try_assert(argc == 2, fmt::format("{} 2, {}: {}", Kernel::Language::get("kernel.argument_expected"), Kernel::Language::get("kernel.argument_received"), argc));
				assert_conditional(JS_IsString(argv[0]), fmt::format("{} {} {} {}", Kernel::Language::get("kernel.expected_argument"), 0, Kernel::Language::get("is"), Kernel::Language::get("kernel.tuple.js_string")), "relative");
				assert_conditional(JS_IsString(argv[1]), fmt::format("{} {} {} {}", Kernel::Language::get("kernel.expected_argument"), 1, Kernel::Language::get("is"), Kernel::Language::get("kernel.tuple.js_string")), "relative");
				auto from = JS::Converter::get_string(context, argv[0]);
				auto to = JS::Converter::get_string(context, argv[1]);
				auto result = Sen::Kernel::Path::Script::relative(from, to);
				return JS::Converter::to_string(context, result);
			}, "relative"_sv);
		}

	}

	/**
	 * JavaScript File System
	*/

	namespace FileSystem {

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
			M_JS_PROXY_WRAPPER(context, {
				try_assert(argc == 1, fmt::format("{} 1, {}: {}", Kernel::Language::get("kernel.argument_expected"), Kernel::Language::get("kernel.argument_received"), argc));
				assert_conditional(JS_IsString(argv[0]), fmt::format("{} {} {} {}", Kernel::Language::get("kernel.expected_argument"), 0, Kernel::Language::get("is"), Kernel::Language::get("kernel.tuple.js_string")), "read_file");
				auto source = JS::Converter::get_string(context, argv[0]);
				auto result = Sen::Kernel::FileSystem::read_file(source);
				return JS::Converter::to_string(context, result);
			}, "read_file"_sv);
		}

		/**
		 * ----------------------------------------
		 * JS read file method
		 * @param argv[0]: source file
		 * @return: readed file
		 * ----------------------------------------
		*/

		inline static auto read_file_encode_with_utf16le(
			JSContext *context, 
			JSValueConst this_val, 
			int argc, 
			JSValueConst *argv
		) -> JSValue
		{
			M_JS_PROXY_WRAPPER(context, {
				try_assert(argc == 1, fmt::format("{} 1, {}: {}", Kernel::Language::get("kernel.argument_expected"), Kernel::Language::get("kernel.argument_received"), argc));
				assert_conditional(JS_IsString(argv[0]), fmt::format("{} {} {} {}", Kernel::Language::get("kernel.expected_argument"), 0, Kernel::Language::get("is"), Kernel::Language::get("kernel.tuple.js_string")), "read_file_encode_with_utf16le");
				auto source = JS::Converter::get_c_string(context, argv[0]);
				auto result = Sen::Kernel::FileSystem::read_file_by_utf16(source.get());
				auto converter = std::wstring_convert<std::codecvt_utf8<wchar_t>>{};
				auto utf8_string = std::string{converter.to_bytes(result)};
				return JS::Converter::to_string(context, utf8_string);
			}, "read_file_encode_with_utf16le"_sv);
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
			M_JS_PROXY_WRAPPER(context, {
				try_assert(argc == 2, fmt::format("{} 2, {}: {}", Kernel::Language::get("kernel.argument_expected"), Kernel::Language::get("kernel.argument_received"), argc));
				assert_conditional(JS_IsString(argv[0]), fmt::format("{} {} {} {}", Kernel::Language::get("kernel.expected_argument"), 0, Kernel::Language::get("is"), Kernel::Language::get("kernel.tuple.js_string")), "write_file");
				assert_conditional(JS_IsString(argv[1]), fmt::format("{} {} {} {}", Kernel::Language::get("kernel.expected_argument"), 1, Kernel::Language::get("is"), Kernel::Language::get("kernel.tuple.js_string")), "write_file");
				auto destination = JS::Converter::get_string(context, argv[0]);
				auto data = JS::Converter::get_string(context, argv[1]);
				Sen::Kernel::FileSystem::write_file(destination, data);
				return JS::Converter::get_undefined();
			}, "write_file"_sv);
		}

		/**
		 * ----------------------------------------
		 * JS write file method
		 * @param argv[0]: destination file
		 * @param argv[1]: data to write
		 * @return: written file
		 * ----------------------------------------
		*/

		inline static auto write_file_encode_with_utf16le(
			JSContext *context, 
			JSValueConst this_val, 
			int argc, 
			JSValueConst *argv
		) -> JSValue
		{
			M_JS_PROXY_WRAPPER(context, {
				try_assert(argc == 2, fmt::format("{} 2, {}: {}", Kernel::Language::get("kernel.argument_expected"), Kernel::Language::get("kernel.argument_received"), argc));
				assert_conditional(JS_IsString(argv[0]), fmt::format("{} {} {} {}", Kernel::Language::get("kernel.expected_argument"), 0, Kernel::Language::get("is"), Kernel::Language::get("kernel.tuple.js_string")), "write_file_encode_with_utf16le");
				assert_conditional(JS_IsString(argv[1]), fmt::format("{} {} {} {}", Kernel::Language::get("kernel.expected_argument"), 1, Kernel::Language::get("is"), Kernel::Language::get("kernel.tuple.js_string")), "write_file_encode_with_utf16le");
				auto destination = JS::Converter::get_c_string(context, argv[0]);
				auto data = JS::Converter::get_string(context, argv[1]);
				auto converter = std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>>{};
				auto result = std::wstring{ converter.from_bytes(data) };
				Sen::Kernel::FileSystem::write_file_by_utf16le(destination.get(), result);
				return JS::Converter::get_undefined();
			}, "write_file_encode_with_utf16le"_sv);
		}

		/**
		 * ----------------------------------------
		 * JS read directory method
		 * @param argv[0]: source directory
		 * @return: JS Array of String
		 * ----------------------------------------
		*/

		inline static auto read_current_directory(
			JSContext *context, 
			JSValueConst this_val, 
			int argc, 
			JSValueConst *argv
		) -> JSValue
		{
			M_JS_PROXY_WRAPPER(context, {
				try_assert(argc == 1, fmt::format("{} 1, {}: {}", Kernel::Language::get("kernel.argument_expected"), Kernel::Language::get("kernel.argument_received"), argc));
				assert_conditional(JS_IsString(argv[0]), fmt::format("{} {} {} {}", Kernel::Language::get("kernel.expected_argument"), 0, Kernel::Language::get("is"), Kernel::Language::get("kernel.tuple.js_string")), "read_current_directory");
				auto source = JS::Converter::get_string(context, argv[0]);
				auto result = Sen::Kernel::FileSystem::read_directory(source);
				return JS::Converter::to_array(context, result);
			}, "read_current_directory"_sv);
		}

		/**
		 * ----------------------------------------
		 * JS read directory method
		 * @param argv[0]: source directory
		 * @return: JS Array of String
		 * ----------------------------------------
		*/

		inline static auto read_directory_only_file(
			JSContext *context, 
			JSValueConst this_val, 
			int argc, 
			JSValueConst *argv
		) -> JSValue
		{
			M_JS_PROXY_WRAPPER(context, {
				try_assert(argc == 1, fmt::format("{} 1, {}: {}", Kernel::Language::get("kernel.argument_expected"), Kernel::Language::get("kernel.argument_received"), argc));
				assert_conditional(JS_IsString(argv[0]), fmt::format("{} {} {} {}", Kernel::Language::get("kernel.expected_argument"), 0, Kernel::Language::get("is"), Kernel::Language::get("kernel.tuple.js_string")), "read_directory_only_file");
				auto source = JS::Converter::get_string(context, argv[0]);
				auto result = Sen::Kernel::FileSystem::read_directory_only_file(source);
				return JS::Converter::to_array(context, result);
			}, "read_directory_only_file"_sv);
		}

		/**
		 * ----------------------------------------
		 * JS read directory method
		 * @param argv[0]: source directory
		 * @return: JS Array of String
		 * ----------------------------------------
		*/

		inline static auto read_directory_only_directory(
			JSContext *context, 
			JSValueConst this_val, 
			int argc, 
			JSValueConst *argv
		) -> JSValue
		{
			M_JS_PROXY_WRAPPER(context, {
				try_assert(argc == 1, fmt::format("{} 1, {}: {}", Kernel::Language::get("kernel.argument_expected"), Kernel::Language::get("kernel.argument_received"), argc));
				assert_conditional(JS_IsString(argv[0]), fmt::format("{} {} {} {}", Kernel::Language::get("kernel.expected_argument"), 0, Kernel::Language::get("is"), Kernel::Language::get("kernel.tuple.js_string")), "read_directory_only_directory");
				auto source = JS::Converter::get_string(context, argv[0]);
				auto result = Sen::Kernel::FileSystem::read_directory_only_directory(source);
				return JS::Converter::to_array(context, result);
			}, "read_directory_only_directory"_sv);
		}

		/**
		 * ----------------------------------------
		 * JS read directory method
		 * @param argv[0]: source directory
		 * @return: JS Array of String
		 * ----------------------------------------
		*/

		inline static auto read_directory(
			JSContext *context, 
			JSValueConst this_val, 
			int argc, 
			JSValueConst *argv
		) -> JSValue
		{
			M_JS_PROXY_WRAPPER(context, {
				try_assert(argc == 1, fmt::format("{} 1, {}: {}", Kernel::Language::get("kernel.argument_expected"), Kernel::Language::get("kernel.argument_received"), argc));
				assert_conditional(JS_IsString(argv[0]), fmt::format("{} {} {} {}", Kernel::Language::get("kernel.expected_argument"), 0, Kernel::Language::get("is"), Kernel::Language::get("kernel.tuple.js_string")), "read_directory");
				auto source = JS::Converter::get_string(context, argv[0]);
				auto result = Sen::Kernel::FileSystem::read_whole_directory(source);
				return JS::Converter::to_array(context, result);
			}, "read_directory"_sv);
		}

		/**
		 * ----------------------------------------
		 * JS create directory
		 * @param argv[0]: destination directory
		 * @return: created directory
		 * ----------------------------------------
		*/

		inline static auto create_directory(
			JSContext *context, 
			JSValueConst this_val, 
			int argc, 
			JSValueConst *argv
		) -> JSValue
		{
			M_JS_PROXY_WRAPPER(context, {
				try_assert(argc == 1, fmt::format("{} 1, {}: {}", Kernel::Language::get("kernel.argument_expected"), Kernel::Language::get("kernel.argument_received"), argc));
				assert_conditional(JS_IsString(argv[0]), fmt::format("{} {} {} {}", Kernel::Language::get("kernel.expected_argument"), 0, Kernel::Language::get("is"), Kernel::Language::get("kernel.tuple.js_string")), "create_directory");
				auto destination = JS::Converter::get_string(context, argv[0]);
				Sen::Kernel::FileSystem::create_directory(destination);
				return JS::Converter::get_undefined();
			}, "create_directory"_sv);
		}

		/**
		 * ----------------------------------------
		 * JS check file
		 * @param argv[0]: source file
		 * @return: if its file true else false
		 * ----------------------------------------
		*/

		inline static auto is_file(
			JSContext *context, 
			JSValueConst this_val, 
			int argc, 
			JSValueConst *argv
		) -> JSValue
		{
			M_JS_PROXY_WRAPPER(context, {
				try_assert(argc == 1, fmt::format("{} 1, {}: {}", Kernel::Language::get("kernel.argument_expected"), Kernel::Language::get("kernel.argument_received"), argc));
				assert_conditional(JS_IsString(argv[0]), fmt::format("{} {} {} {}", Kernel::Language::get("kernel.expected_argument"), 0, Kernel::Language::get("is"), Kernel::Language::get("kernel.tuple.js_string")), "is_file");
				auto source = JS::Converter::get_string(context, argv[0]);
				auto result = std::filesystem::is_regular_file(source);
				return JS::Converter::to_bool(context, result);
			}, "is_file"_sv);
		}

		/**
		 * ----------------------------------------
		 * JS check directory
		 * @param argv[0]: source directory
		 * @return: if its directory true
		 * ----------------------------------------
		*/

		inline static auto is_directory(
			JSContext *context, 
			JSValueConst this_val, 
			int argc, 
			JSValueConst *argv
		) -> JSValue
		{
			M_JS_PROXY_WRAPPER(context, {
				try_assert(argc == 1, fmt::format("{} 1, {}: {}", Kernel::Language::get("kernel.argument_expected"), Kernel::Language::get("kernel.argument_received"), argc));
				assert_conditional(JS_IsString(argv[0]), fmt::format("{} {} {} {}", Kernel::Language::get("kernel.expected_argument"), 0, Kernel::Language::get("is"), Kernel::Language::get("kernel.tuple.js_string")), "is_directory");
				auto source = JS::Converter::get_string(context, argv[0]);
				auto result = std::filesystem::is_directory(source);
				return JS::Converter::to_bool(context, result);
			}, "is_directory"_sv);
		}

		// Basic Operation for JS

		namespace Operation {

			/**
			 * ----------------------------------------
			 * JS rename file
			 * @param argv[0]: source file
			 * @param argv[1]: destination file
			 * @return: undefined
			 * ----------------------------------------
			*/

			inline static auto rename(
				JSContext *context, 
				JSValueConst this_val, 
				int argc, 
				JSValueConst *argv
			) -> JSValue
			{
				M_JS_PROXY_WRAPPER(context, {
				try_assert(argc == 2, fmt::format("{} 2, {}: {}", Kernel::Language::get("kernel.argument_expected"), Kernel::Language::get("kernel.argument_received"), argc));
				assert_conditional(JS_IsString(argv[0]), fmt::format("{} {} {} {}", Kernel::Language::get("kernel.expected_argument"), 0, Kernel::Language::get("is"), Kernel::Language::get("kernel.tuple.js_string")), "rename");
				assert_conditional(JS_IsString(argv[0]), fmt::format("{} {} {} {}", Kernel::Language::get("kernel.expected_argument"), 1, Kernel::Language::get("is"), Kernel::Language::get("kernel.tuple.js_string")), "rename");
					auto source = JS::Converter::get_string(context, argv[0]);
					auto destination = JS::Converter::get_string(context, argv[1]);
					std::filesystem::rename(std::filesystem::path{source}, std::filesystem::path{destination});
					return JS::Converter::get_undefined(); 
				}, "rename"_sv);
			}

			/**
			 * ----------------------------------------
			 * JS copy file
			 * @param argv[0]: source file
			 * @param argv[1]: destination file
			 * @return: undefined
			 * ----------------------------------------
			*/

			inline static auto copy(
				JSContext *context, 
				JSValueConst this_val, 
				int argc, 
				JSValueConst *argv
			) -> JSValue
			{
				M_JS_PROXY_WRAPPER(context, {
				try_assert(argc == 2, fmt::format("{} 2, {}: {}", Kernel::Language::get("kernel.argument_expected"), Kernel::Language::get("kernel.argument_received"), argc));
					auto source = JS::Converter::get_string(context, argv[0]);
					auto destination = JS::Converter::get_string(context, argv[1]);
					std::filesystem::copy(std::filesystem::path{source}, std::filesystem::path{destination});
					return JS::Converter::get_undefined();
				}, "copy"_sv);
			}

			/**
			 * ----------------------------------------
			 * JS copy file
			 * @param argv[0]: source file
			 * @return: undefined
			 * ----------------------------------------
			*/

			inline static auto remove(
				JSContext *context, 
				JSValueConst this_val, 
				int argc, 
				JSValueConst *argv
			) -> JSValue
			{
				M_JS_PROXY_WRAPPER(context, {
				try_assert(argc == 1, fmt::format("{} 1, {}: {}", Kernel::Language::get("kernel.argument_expected"), Kernel::Language::get("kernel.argument_received"), argc));
					auto source = JS::Converter::get_string(context, argv[0]);
					std::filesystem::remove(std::filesystem::path{source});
					return JS::Converter::get_undefined();
				}, "remove"_sv);
			}

		}
	}

	namespace Dimension {

		inline static auto area(
			JSContext* context,
			JSValueConst this_val,
			int argc,
			JSValueConst* argv
		) -> JSValue
		{
			M_JS_PROXY_WRAPPER(context, {
				auto width_val = JS_GetPropertyStr(context, this_val, "width");
				auto height_val = JS_GetPropertyStr(context, this_val, "height");
				M_JS_UNDEFINED_BEHAVIOR(context, width_val, "width", "area"_sv);
				M_JS_UNDEFINED_BEHAVIOR(context, width_val, "height", "area"_sv);
				auto width = JS::Converter::get_bigint64(context, width_val);
				auto height = JS::Converter::get_bigint64(context, height_val);
				auto area = width * height;
				JS_FreeValue(context, width_val);
				JS_FreeValue(context, height_val);
				return JS_NewBigInt64(context, area);
				}, "area"_sv);
		}

		inline static auto circumference(
			JSContext* context,
			JSValueConst this_val,
			int argc,
			JSValueConst* argv
		) -> JSValue
		{
			M_JS_PROXY_WRAPPER(context, {
				auto width_val = JS_GetPropertyStr(context, this_val, "width");
				auto height_val = JS_GetPropertyStr(context, this_val, "height");
				M_JS_UNDEFINED_BEHAVIOR(context, width_val, "width", "circumference"_sv);
				M_JS_UNDEFINED_BEHAVIOR(context, width_val, "height", "circumference"_sv);
				auto width = JS::Converter::get_bigint64(context, width_val);
				auto height = JS::Converter::get_bigint64(context, height_val);
				auto area = (width + height) * 2;
				JS_FreeValue(context, width_val);
				JS_FreeValue(context, height_val);
				return JS_NewBigInt64(context, area);
				}, "circumference"_sv);
		}

		/// Create an instance of Dimension object

		inline static auto instance(
			JSContext* context,
			JSValueConst this_val,
			int argc,
			JSValueConst* argv
		) -> JSValue
		{
			M_JS_PROXY_WRAPPER(context, {
				try_assert(argc == 2, fmt::format("{} 2, {}: {}", Kernel::Language::get("kernel.argument_expected"), Kernel::Language::get("kernel.argument_received"), argc));
				auto image_obj = JSValue{};
				auto area_func = JS_NewCFunction(context, area, "area", 0);
				auto circumference_func = JS_NewCFunction(context, circumference, "circumference", 0);
				image_obj = JS_NewObject(context);
				JS_SetPropertyStr(context, image_obj, "width", JS_NewBigInt64(context, JS::Converter::get_bigint64(context, argv[0])));
				JS_SetPropertyStr(context, image_obj, "height", JS_NewBigInt64(context, JS::Converter::get_bigint64(context, argv[1])));
				JS_DefinePropertyValueStr(context, image_obj, "area", area_func, JS_PROP_WRITABLE | JS_PROP_CONFIGURABLE);
				JS_DefinePropertyValueStr(context, image_obj, "circumference", circumference_func, JS_PROP_WRITABLE | JS_PROP_CONFIGURABLE);
				return image_obj;
				}, "instance"_sv);
		}

		inline static auto open(
			JSContext* context,
			JSValueConst this_val,
			int argc,
			JSValueConst* argv
		) -> JSValue
		{
			M_JS_PROXY_WRAPPER(context, {
				try_assert(argc == 1, fmt::format("{} 1, {}: {}", Kernel::Language::get("kernel.argument_expected"), Kernel::Language::get("kernel.argument_received"), argc));
				auto image = Sen::Kernel::Definition::ImageIO::read_png(JS::Converter::get_c_string(context, argv[0]).get());
				auto image_obj = JSValue{};
				auto area_func = JS_NewCFunction(context, area, "area", 0);
				auto circumference_func = JS_NewCFunction(context, circumference, "circumference", 0);
				image_obj = JS_NewObject(context);
				JS_SetPropertyStr(context, image_obj, "width", JS_NewBigInt64(context, image.width));
				JS_SetPropertyStr(context, image_obj, "height", JS_NewBigInt64(context, image.height));
				JS_SetPropertyStr(context, image_obj, "bit_depth", JS_NewBigInt64(context, image.bit_depth));
				JS_SetPropertyStr(context, image_obj, "color_type", JS_NewBigInt64(context, image.color_type));
				JS_SetPropertyStr(context, image_obj, "interlace_type", JS_NewBigInt64(context, image.interlace_type));
				JS_SetPropertyStr(context, image_obj, "channels", JS_NewBigInt64(context, image.channels));
				JS_SetPropertyStr(context, image_obj, "rowbytes", JS_NewBigInt64(context, image.rowbytes));
				JS_SetPropertyStr(context, image_obj, "data", JS_NewArrayBufferCopy(context, image.data().data(), image.data().size()));
				JS_DefinePropertyValueStr(context, image_obj, "area", area_func, JS_PROP_WRITABLE | JS_PROP_CONFIGURABLE);
				JS_DefinePropertyValueStr(context, image_obj, "circumference", circumference_func, JS_PROP_WRITABLE | JS_PROP_CONFIGURABLE);
				return image_obj;
				}, "open"_sv);
		}

		inline static auto write(
			JSContext* context,
			JSValueConst this_val,
			int argc,
			JSValueConst* argv
		) -> JSValue
		{
			M_JS_PROXY_WRAPPER(context, {
				try_assert(argc == 2, fmt::format("{} 2, {}: {}", Kernel::Language::get("kernel.argument_expected"), Kernel::Language::get("kernel.argument_received"), argc));
				auto source_file = JS::Converter::get_c_string(context, argv[0]);
				auto obj = argv[1];
				auto width_val = JS_GetPropertyStr(context, obj, "width");
				auto height_val = JS_GetPropertyStr(context, obj, "height");
				auto bit_depth_val = JS_GetPropertyStr(context, obj, "bit_depth");
				auto color_type_val = JS_GetPropertyStr(context, obj, "color_type");
				auto interlace_type_val = JS_GetPropertyStr(context, obj, "interlace_type");
				auto channels_val = JS_GetPropertyStr(context, obj, "channels");
				auto rowbytes_val = JS_GetPropertyStr(context, obj, "rowbytes");
				auto data_val = JS_GetPropertyStr(context, obj, "data");
				auto width = int64_t{};
				auto height = int64_t{};
				auto bit_depth = int64_t{};
				auto color_type = int64_t{};
				auto interlace_type = int64_t{};
				auto channels = int64_t{};
				auto rowbytes = int64_t{};
				auto data_len = size_t{};
				JS_ToBigInt64(context, &width, width_val);
				JS_ToBigInt64(context, &height, height_val);
				JS_ToBigInt64(context, &bit_depth, bit_depth_val);
				JS_ToBigInt64(context, &color_type, color_type_val);
				JS_ToBigInt64(context, &interlace_type, interlace_type_val);
				JS_ToBigInt64(context, &channels, channels_val);
				JS_ToBigInt64(context, &rowbytes, rowbytes_val);
				auto data = JS_GetArrayBuffer(context, &data_len, data_val);
				auto image = Sen::Kernel::Definition::Image<int>(
					static_cast<int>(width),
					static_cast<int>(height),
					static_cast<int>(bit_depth),
					static_cast<int>(color_type),
					static_cast<int>(interlace_type),
					static_cast<int>(channels),
					static_cast<int>(rowbytes),
					std::move(std::vector<uint8_t>(data, data + data_len))
				);
				Sen::Kernel::Definition::ImageIO::write_png(source_file.get(), image);
				JS_FreeValue(context, width_val);
				JS_FreeValue(context, height_val);
				JS_FreeValue(context, bit_depth_val);
				JS_FreeValue(context, color_type_val);
				JS_FreeValue(context, interlace_type_val);
				JS_FreeValue(context, channels_val);
				JS_FreeValue(context, rowbytes_val);
				JS_FreeValue(context, data_val);
				return JS::Converter::get_undefined();
				}, "write"_sv);
		}

	}

	/**
	 * JavaScript Image Support
	*/

	namespace Image {

		/**
		 * ----------------------------------------
		 * JavaScript scale image
		 * @param argv[0]: source file
		 * @param argv[1]: destination file
		 * @param argv[2]: percentage
		 * @return: UNDEFINED
		 * ----------------------------------------
		*/

		inline static auto scale_fs(
			JSContext *context, 
			JSValueConst this_val, 
			int argc, 
			JSValueConst *argv
		) -> JSValue
		{
			M_JS_PROXY_WRAPPER(context, {
				try_assert(argc == 3, fmt::format("argument expected {} but received {}", 3, argc));
				auto source = JS::Converter::get_string(context, argv[0]);
				auto destination = JS::Converter::get_string(context, argv[1]);
				auto percentage = JS::Converter::get_float32(context, argv[2]);
				Sen::Kernel::Definition::ImageIO::scale_png(source, destination, percentage);
				return JS::Converter::get_undefined();
			}, "scale_fs"_sv);
		}

		/**
		 * ----------------------------------------
		 * JavaScript transparent image
		 * @param argv[0]: destination file
		 * @return: UNDEFINED
		 * ----------------------------------------
		*/

		inline static auto transparent_fs(
			JSContext *context, 
			JSValueConst this_val, 
			int argc, 
			JSValueConst *argv
		) -> JSValue
		{
			M_JS_PROXY_WRAPPER(context, {
				try_assert(argc == 3, fmt::format("argument expected {} but received {}", 3, argc));
				auto destination = JS::Converter::get_string(context, argv[0]);
				auto width = JS::Converter::get_int32(context, argv[1]);
				auto height = JS::Converter::get_int32(context, argv[2]);
				Sen::Kernel::Definition::ImageIO::transparent_png(destination, width, height);
				return JS::Converter::get_undefined();
			}, "transparent_fs"_sv);
		}

		struct Coordinate {
			int x;
			int y;
		};

		inline static auto join_png(
			JSContext* context,
			JSValueConst this_val,
			int argc,
			JSValueConst* argv
		) -> JSValue
		{
			M_JS_PROXY_WRAPPER(context, {
				try_assert(argc == 3, fmt::format("argument expected {} but received {}", 3, argc));
				auto destination = JS::Converter::get_c_string(context, argv[0]);
				auto width = JS_GetPropertyStr(context, argv[1], "width");
				auto height = JS_GetPropertyStr(context, argv[1], "height");
				M_JS_UNDEFINED_BEHAVIOR(context, width, "width", "join_png"_sv);
				M_JS_UNDEFINED_BEHAVIOR(context, height, "height", "join_png"_sv);
				auto length_value = JS_GetPropertyStr(context, argv[2], "length");
				auto length = JS::Converter::get_int32(context, length_value);
				JS_FreeValue(context, length_value);
				auto m_data = std::vector<Sen::Kernel::Definition::Image<int>>{};
				auto x_y = std::vector<Coordinate>{};
				for (auto i : Range<int>(length)) {
					auto current_object = JS_GetPropertyUint32(context, argv[2], i);
					auto m_width = JS_GetPropertyStr(context, current_object, "width");
					auto m_height = JS_GetPropertyStr(context, current_object, "height");
					auto m_x = JS_GetPropertyStr(context, current_object, "x");
					auto m_y = JS_GetPropertyStr(context, current_object, "y");
					auto data_val = JS_GetPropertyStr(context, current_object, "data");
					M_JS_UNDEFINED_BEHAVIOR(context, m_width, "width", "join_png"_sv);
					M_JS_UNDEFINED_BEHAVIOR(context, m_height, "height", "join_png"_sv);
					M_JS_UNDEFINED_BEHAVIOR(context, m_x, "x", "join_png"_sv);
					M_JS_UNDEFINED_BEHAVIOR(context, m_y, "y", "join_png"_sv);
					M_JS_UNDEFINED_BEHAVIOR(context, data_val, "data", "join_png"_sv);
					auto data_len = std::size_t{};
					auto image_x = static_cast<int>(JS::Converter::get_bigint64(context, m_x));
					auto image_y = static_cast<int>(JS::Converter::get_bigint64(context, m_y));
					auto image_width = static_cast<int>(JS::Converter::get_bigint64(context, m_width));
					auto image_height = static_cast<int>(JS::Converter::get_bigint64(context, m_height));
					auto data = JS_GetArrayBuffer(context, &data_len, data_val);
					x_y.emplace_back(Coordinate{.x = image_x, .y = image_y});
					m_data.emplace_back(Sen::Kernel::Definition::Image<int>{
						image_x,
						image_y,
						image_width,
						image_height,
						std::move(std::vector<uint8_t>(data, data + data_len))
					});
					JS_FreeValue(context, m_width);
					JS_FreeValue(context, m_height);
					JS_FreeValue(context, m_x);
					JS_FreeValue(context, m_y);
					JS_FreeValue(context, data_val);
					JS_FreeValue(context, current_object);
				}
				for (auto i : Range<size_t>(x_y.size())) {
					m_data[i].x = x_y[i].x;
					m_data[i].y = x_y[i].y;
				}
				Sen::Kernel::Definition::ImageIO::join_png(
					destination.get(),
					Sen::Kernel::Definition::Dimension<int>{
						static_cast<int>(JS::Converter::get_bigint64(context, width)),
						static_cast<int>(JS::Converter::get_bigint64(context, height))
					},
					m_data
				);
				JS_FreeValue(context, width);
				JS_FreeValue(context, height);
				return JS::Converter::get_undefined();
			}, "join_png"_sv);
		}

		inline static auto join(
			JSContext* context,
			JSValueConst this_val,
			int argc,
			JSValueConst* argv
		) -> JSValue
			{
			M_JS_PROXY_WRAPPER(context, {
				try_assert(argc == 2, fmt::format("{} 2, {}: {}", Kernel::Language::get("kernel.argument_expected"), Kernel::Language::get("kernel.argument_received"), argc));
				auto width = JS_GetPropertyStr(context, argv[0], "width");
				auto height = JS_GetPropertyStr(context, argv[0], "height");
				M_JS_UNDEFINED_BEHAVIOR(context, width, "width", "join_png"_sv);
				M_JS_UNDEFINED_BEHAVIOR(context, height, "height", "join_png"_sv);
				auto length_value = JS_GetPropertyStr(context, argv[1], "length");
				auto length = JS::Converter::get_int32(context, length_value);
				JS_FreeValue(context, length_value);
				auto m_data = std::vector<Sen::Kernel::Definition::Image<int>>{};
				auto x_y = std::vector<Coordinate>{};
				for (auto i : Range<int>(length)) {
					auto current_object = JS_GetPropertyUint32(context, argv[1], i);
					auto m_width = JS_GetPropertyStr(context, current_object, "width");
					auto m_height = JS_GetPropertyStr(context, current_object, "height");
					auto m_x = JS_GetPropertyStr(context, current_object, "x");
					auto m_y = JS_GetPropertyStr(context, current_object, "y");
					auto data_val = JS_GetPropertyStr(context, current_object, "data");
					M_JS_UNDEFINED_BEHAVIOR(context, m_width, "width", "join"_sv);
					M_JS_UNDEFINED_BEHAVIOR(context, m_height, "height", "join"_sv);
					M_JS_UNDEFINED_BEHAVIOR(context, m_x, "x", "join"_sv);
					M_JS_UNDEFINED_BEHAVIOR(context, m_y, "y", "join"_sv);
					M_JS_UNDEFINED_BEHAVIOR(context, data_val, "data", "join"_sv);
					auto data_len = std::size_t{};
					auto image_x = static_cast<int>(JS::Converter::get_bigint64(context, m_x));
					auto image_y = static_cast<int>(JS::Converter::get_bigint64(context, m_y));
					auto image_width = static_cast<int>(JS::Converter::get_bigint64(context, m_width));
					auto image_height = static_cast<int>(JS::Converter::get_bigint64(context, m_height));
					auto data = JS_GetArrayBuffer(context, &data_len, data_val);
					x_y.emplace_back(Coordinate{.x = image_x, .y = image_y});
					m_data.emplace_back(Sen::Kernel::Definition::Image<int>{
						image_x,
						image_y,
						image_width,
						image_height,
						std::move(std::vector<uint8_t>(data, data + data_len))
					});
					JS_FreeValue(context, m_width);
					JS_FreeValue(context, m_height);
					JS_FreeValue(context, m_x);
					JS_FreeValue(context, m_y);
					JS_FreeValue(context, data_val);
					JS_FreeValue(context, current_object);
				}
				for (auto i : Range<size_t>(x_y.size())) {
					m_data[i].x = x_y[i].x;
					m_data[i].y = x_y[i].y;
				}
				auto destination = Kernel::Definition::Image<int>::transparent(
					Kernel::Definition::Dimension<int>(
						static_cast<int>(JS::Converter::get_bigint64(context, width)), 
						static_cast<int>(JS::Converter::get_bigint64(context, height))
					)
				);
				Sen::Kernel::Definition::ImageIO::join(
					destination,
					m_data
				);
				auto image_obj = JS_NewObject(context);
				auto area_func = JS_NewCFunction(context, Dimension::area, "area", 0);
				auto circumference_func = JS_NewCFunction(context, Dimension::circumference, "circumference", 0);
				JS_SetPropertyStr(context, image_obj, "width", JS_NewBigInt64(context, destination.width));
				JS_SetPropertyStr(context, image_obj, "height", JS_NewBigInt64(context, destination.height));
				JS_SetPropertyStr(context, image_obj, "bit_depth", JS_NewBigInt64(context, destination.bit_depth));
				JS_SetPropertyStr(context, image_obj, "color_type", JS_NewBigInt64(context, destination.color_type));
				JS_SetPropertyStr(context, image_obj, "interlace_type", JS_NewBigInt64(context, destination.interlace_type));
				JS_SetPropertyStr(context, image_obj, "channels", JS_NewBigInt64(context, destination.channels));
				JS_SetPropertyStr(context, image_obj, "rowbytes", JS_NewBigInt64(context, destination.rowbytes));
				JS_SetPropertyStr(context, image_obj, "data", JS_NewArrayBufferCopy(context, destination.data().data(), destination.data().size()));
				JS_DefinePropertyValueStr(context, image_obj, "area", area_func, JS_PROP_WRITABLE | JS_PROP_CONFIGURABLE);
				JS_DefinePropertyValueStr(context, image_obj, "circumference", circumference_func, JS_PROP_WRITABLE | JS_PROP_CONFIGURABLE);
				JS_FreeValue(context, width);
				JS_FreeValue(context, height);
				return image_obj;
			}, "join"_sv);
		}

		/**
		 * ----------------------------------------
		 * JavaScript resize image
		 * @param argv[0]: source file
		 * @param argv[1]: destination file
		 * @param argv[2]: percentage
		 * @return: UNDEFINED
		 * ----------------------------------------
		*/

		inline static auto resize_fs(
			JSContext *context, 
			JSValueConst this_val, 
			int argc, 
			JSValueConst *argv
		) -> JSValue
		{
			M_JS_PROXY_WRAPPER(context, {
				try_assert(argc == 3, fmt::format("argument expected {} but received {}", 3, argc));
				auto source = JS::Converter::get_string(context, argv[0]);
				auto destination = JS::Converter::get_string(context, argv[1]);
				auto percentage = JS::Converter::get_float32(context, argv[2]);
				Sen::Kernel::Definition::ImageIO::resize_png(source, destination, percentage);
				return JS::Converter::get_undefined();
			}, "resize_fs"_sv);
		}

		/**
		 * ----------------------------------------
		 * JavaScript rotate image
		 * @param argv[0]: source file
		 * @param argv[1]: destination file
		 * @param argv[2]: percentage
		 * @return: UNDEFINED
		 * ----------------------------------------
		*/

		inline static auto rotate_fs(
			JSContext *context, 
			JSValueConst this_val, 
			int argc, 
			JSValueConst *argv
		) -> JSValue
		{
			M_JS_PROXY_WRAPPER(context, {
				try_assert(argc == 3, fmt::format("argument expected {} but received {}", 3, argc));
				auto source = JS::Converter::get_string(context, argv[0]);
				auto destination = JS::Converter::get_string(context, argv[1]);
				auto percentage = JS::Converter::get_float64(context, argv[2]);
				Sen::Kernel::Definition::ImageIO::rotate_png(source, destination, percentage);
				return JS::Converter::get_undefined();
			}, "rotate_fs"_sv);
		}

		/**
		 * ----------------------------------------
		 * JavaScript rotate image
		 * @param argv[0]: source file
		 * @param argv[1]: destination file
		 * @param argv[2]: percentage
		 * @return: UNDEFINED
		 * ----------------------------------------
		*/

		inline static auto composite_fs(
			JSContext *context, 
			JSValueConst this_val, 
			int argc, 
			JSValueConst *argv
		) -> JSValue
		{
			M_JS_PROXY_WRAPPER(context, {
				try_assert(argc == 3, fmt::format("argument expected {} but received {}", 3, argc));
				auto source = JS::Converter::get_c_string(context, argv[0]);
				auto destination = JS::Converter::get_c_string(context, argv[1]);
				auto rectangle_width = JS_GetPropertyStr(context, argv[2], "width");
				auto rectangle_height = JS_GetPropertyStr(context, argv[2], "height");
				auto rectangle_x = JS_GetPropertyStr(context, argv[2], "x");
				auto rectangle_y = JS_GetPropertyStr(context, argv[2], "y");
				M_JS_UNDEFINED_BEHAVIOR(context, rectangle_width, "width", "composite_fs"_sv);
				M_JS_UNDEFINED_BEHAVIOR(context, rectangle_height, "height", "composite_fs"_sv);
				M_JS_UNDEFINED_BEHAVIOR(context, rectangle_x, "x", "composite_fs"_sv);
				M_JS_UNDEFINED_BEHAVIOR(context, rectangle_y, "y", "composite_fs"_sv);
				Sen::Kernel::Definition::ImageIO::composite_png(
					source.get(), 
					destination.get(), 
					Sen::Kernel::Definition::Rectangle<int>(
						JS::Converter::get_int32(context, rectangle_x), 
						JS::Converter::get_int32(context, rectangle_y), 
						JS::Converter::get_int32(context, rectangle_width), 
						JS::Converter::get_int32(context, rectangle_height)
					)
				);
				JS_FreeValue(context, rectangle_width);
				JS_FreeValue(context, rectangle_height);
				JS_FreeValue(context, rectangle_x);
				JS_FreeValue(context, rectangle_y);
				return JS::Converter::get_undefined();
			}, "composite_fs"_sv);
		}

		/**
		 * ----------------------------------------
		 * JavaScript rotate image
		 * @param argv[0]: source file
		 * @param argv[1]: destination file
		 * @param argv[2]: percentage
		 * @return: UNDEFINED
		 * ----------------------------------------
		*/

		inline static auto composite_multiple_fs(
			JSContext *context, 
			JSValueConst this_val, 
			int argc, 
			JSValueConst *argv
		) -> JSValue
		{
			M_JS_PROXY_WRAPPER(context, {
				try_assert(argc == 2, fmt::format("{} 2, {}: {}", Kernel::Language::get("kernel.argument_expected"), Kernel::Language::get("kernel.argument_received"), argc));
				auto source = JS::Converter::get_c_string(context, argv[0]);
				auto data = std::vector<Sen::Kernel::Definition::RectangleFileIO<int>>{};
				if (JS_IsArray(context, argv[1])) {
					auto length = uint32_t{};
					JS_ToUint32(context, &length, JS_GetPropertyStr(context, argv[1], "length"));
					for (auto i : Range<uint32_t>(length)) {
						auto current_object = JS_GetPropertyUint32(context, argv[1], i);
						auto rectangle_width = JS_GetPropertyStr(context, current_object, "width");
						auto rectangle_height = JS_GetPropertyStr(context, current_object, "height");
						auto rectangle_x = JS_GetPropertyStr(context, current_object, "x");
						auto rectangle_y = JS_GetPropertyStr(context, current_object, "y");
						auto destination = JS_GetPropertyStr(context, current_object, "destination");
						M_JS_UNDEFINED_BEHAVIOR(context, rectangle_width, "width", "composite_multiple_fs"_sv);
						M_JS_UNDEFINED_BEHAVIOR(context, rectangle_height, "height", "composite_multiple_fs"_sv);
						M_JS_UNDEFINED_BEHAVIOR(context, rectangle_x, "x", "composite_multiple_fs"_sv);
						M_JS_UNDEFINED_BEHAVIOR(context, rectangle_y, "y", "composite_multiple_fs"_sv);
						M_JS_UNDEFINED_BEHAVIOR(context, destination, "destination", "composite_multiple_fs"_sv);
						data.emplace_back(
							Sen::Kernel::Definition::RectangleFileIO<int>(
								JS::Converter::get_int32(context, rectangle_x),
								JS::Converter::get_int32(context, rectangle_y),
								JS::Converter::get_int32(context, rectangle_width),
								JS::Converter::get_int32(context, rectangle_height),
								JS::Converter::get_c_string(context, destination).get()
							)
						);
						JS_FreeValue(context, rectangle_width);
						JS_FreeValue(context, rectangle_height);
						JS_FreeValue(context, rectangle_x);
						JS_FreeValue(context, rectangle_y);
						JS_FreeValue(context, destination);
						JS_FreeValue(context, current_object);
					}
				} else {
					throw Exception("Cannot read property \"length\" of undefined");
				}
				Sen::Kernel::Definition::ImageIO::composite_pngs(source.get(), data);
				return JS::Converter::get_undefined();
			}, "composite_multiple_fs"_sv);
		}

		/**
		 * ----------------------------------------
		 * JavaScript rotate image
		 * @param argv[0]: source file
		 * @param argv[1]: destination file
		 * @param argv[2]: percentage
		 * @return: UNDEFINED
		 * ----------------------------------------
		*/

		inline static auto composite_multiple_fs_asynchronous(
			JSContext *context, 
			JSValueConst this_val, 
			int argc, 
			JSValueConst *argv
		) -> JSValue
		{
			M_JS_PROXY_WRAPPER(context, {
				try_assert(argc == 2, fmt::format("{} 2, {}: {}", Kernel::Language::get("kernel.argument_expected"), Kernel::Language::get("kernel.argument_received"), argc));
				auto source = JS::Converter::get_c_string(context, argv[0]);
				auto data = std::vector<Sen::Kernel::Definition::RectangleFileIO<int>>{};
				if (JS_IsArray(context, argv[1])) {
					auto length = uint32_t{};
					JS_ToUint32(context, &length, JS_GetPropertyStr(context, argv[1], "length"));
					for (auto i : Range<uint32_t>(length)) {
						auto current_object = JS_GetPropertyUint32(context, argv[1], i);
						auto rectangle_width = JS_GetPropertyStr(context, current_object, "width");
						auto rectangle_height = JS_GetPropertyStr(context, current_object, "height");
						auto rectangle_x = JS_GetPropertyStr(context, current_object, "x");
						auto rectangle_y = JS_GetPropertyStr(context, current_object, "y");
						auto destination = JS_GetPropertyStr(context, current_object, "destination");
						M_JS_UNDEFINED_BEHAVIOR(context, rectangle_width, "width", "composite_multiple_fs_asynchronous"_sv);
						M_JS_UNDEFINED_BEHAVIOR(context, rectangle_height, "height", "composite_multiple_fs_asynchronous"_sv);
						M_JS_UNDEFINED_BEHAVIOR(context, rectangle_x, "x", "composite_multiple_fs_asynchronous"_sv);
						M_JS_UNDEFINED_BEHAVIOR(context, rectangle_y, "y", "composite_multiple_fs_asynchronous"_sv);
						M_JS_UNDEFINED_BEHAVIOR(context, destination, "destination", "composite_multiple_fs_asynchronous"_sv);
						data.emplace_back(
							Sen::Kernel::Definition::RectangleFileIO<int>(
								JS::Converter::get_int32(context, rectangle_x),
								JS::Converter::get_int32(context, rectangle_y),
								JS::Converter::get_int32(context, rectangle_width),
								JS::Converter::get_int32(context, rectangle_height),
								JS::Converter::get_c_string(context, destination).get()
							)
						);
						JS_FreeValue(context, rectangle_width);
						JS_FreeValue(context, rectangle_height);
						JS_FreeValue(context, rectangle_x);
						JS_FreeValue(context, rectangle_y);
						JS_FreeValue(context, destination);
						JS_FreeValue(context, current_object);
					}
				} else {
					throw Exception("Cannot read property \"length\" of undefined");
				}
				Sen::Kernel::Definition::ImageIO::composite_pngs_asynchronous(source.get(), data);
				return JS::Converter::get_undefined();
			}, "composite_multiple_fs_asynchronous"_sv);
		}


	}

	/**
	 * JavaScript Engine
	*/

	namespace JavaScript {
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
			M_JS_PROXY_WRAPPER(context, {
				try_assert(argc == 1, fmt::format("{} 1, {}: {}", Kernel::Language::get("kernel.argument_expected"), Kernel::Language::get("kernel.argument_received"), argc));
				auto source = JS::Converter::get_string(context, argv[0]);
				auto m_value = JS_Eval(context, source.c_str(), source.size(), "unknown", JS_EVAL_TYPE_GLOBAL);
				return m_value;
			}, "evaluate"_sv);
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
			M_JS_PROXY_WRAPPER(context, {
				try_assert(argc == 1, fmt::format("{} 1, {}: {}", Kernel::Language::get("kernel.argument_expected"), Kernel::Language::get("kernel.argument_received"), argc));
				auto source = JS::Converter::get_string(context, argv[0]);
				auto js_source = Sen::Kernel::FileSystem::read_file(source);
				auto m_value = JS_Eval(context, js_source.c_str(), js_source.size(), source.c_str(), JS_EVAL_TYPE_GLOBAL);
				return m_value;
			}, "evaluate_fs"_sv);
		}
	}

	/**
	 * JavaScript Encryption System
	*/

	namespace Encryption {

		/**
		 * JavaScript Encryption System Supportive of MD5
		*/

		namespace FNV {

			/**
			 * ----------------------------------------
			 * JavaScript MD5 Hash method
			 * @param argv[0]: source hash
			 * @returns: hashed string
			 * ----------------------------------------
			*/

			inline static auto hash(
				JSContext *context, 
				JSValueConst this_val, 
				int argc, 
				JSValueConst *argv
			) -> JSValue
			{
				M_JS_PROXY_WRAPPER(context, {
				try_assert(argc == 1, fmt::format("{} 1, {}: {}", Kernel::Language::get("kernel.argument_expected"), Kernel::Language::get("kernel.argument_received"), argc));
					auto source = JS::Converter::get_c_string(context, argv[0]);
					auto result = Kernel::Definition::Encryption::FNV::Hash<std::uint32_t>{}.make_hash(source.get());
					return JS::Converter::to_bigint(context, result);
				}, "hash"_sv);
			}

			/**
			 * ----------------------------------------
			 * JavaScript MD5 Hash file method
			 * @param argv[0]: source file
			 * @returns: hashed string
			 * ----------------------------------------
			*/

			inline static auto hash_fs(
				JSContext *context, 
				JSValueConst this_val, 
				int argc, 
				JSValueConst *argv
			) -> JSValue
			{
				M_JS_PROXY_WRAPPER(context, {
				try_assert(argc == 1, fmt::format("{} 1, {}: {}", Kernel::Language::get("kernel.argument_expected"), Kernel::Language::get("kernel.argument_received"), argc));
					auto source = JS::Converter::get_c_string(context, argv[0]);
					auto result = Sen::Kernel::Definition::Encryption::FNV::Hash<std::uint32_t>::hash_fs(source.get());
					return JS::Converter::to_bigint(context, result);
				}, "hash_fs"_sv);
			}

		}

		/**
		 * JavaScript Encryption System Supportive of MD5
		*/

		namespace MD5 {
			/**
			 * ----------------------------------------
			 * JavaScript MD5 Hash method
			 * @param argv[0]: source hash
			 * @returns: hashed string
			 * ----------------------------------------
			*/

			inline static auto hash(
				JSContext *context, 
				JSValueConst this_val, 
				int argc, 
				JSValueConst *argv
			) -> JSValue
			{
				M_JS_PROXY_WRAPPER(context, {
				try_assert(argc == 1, fmt::format("{} 1, {}: {}", Kernel::Language::get("kernel.argument_expected"), Kernel::Language::get("kernel.argument_received"), argc));
					auto source = JS::Converter::get_c_string(context, argv[0]);
					auto result = Kernel::Definition::Encryption::MD5::hash(Kernel::FileSystem::read_binary<unsigned char>(source.get()));
					return JS::Converter::to_string(context, result);
				}, "hash"_sv);
			}

			/**
			 * ----------------------------------------
			 * JavaScript MD5 Hash file method
			 * @param argv[0]: source file
			 * @returns: hashed string
			 * ----------------------------------------
			*/

			inline static auto hash_fs(
				JSContext *context, 
				JSValueConst this_val, 
				int argc, 
				JSValueConst *argv
			) -> JSValue
			{
				M_JS_PROXY_WRAPPER(context, {
				try_assert(argc == 1, fmt::format("{} 1, {}: {}", Kernel::Language::get("kernel.argument_expected"), Kernel::Language::get("kernel.argument_received"), argc));
					auto source = JS::Converter::get_string(context, argv[0]);
					auto result = Sen::Kernel::Definition::Encryption::MD5::hash_fs(source);
					return JS::Converter::to_string(context, result);
				}, "hash_fs"_sv);
			}
		}

		/**
		 * JavaScript Base64 Encode/Decode System
		*/

		namespace Base64 {

			/**
			 * ----------------------------------------
			 * JavaScript Base 64 Encode method
			 * @param argv[0]: source to encode
			 * @returns: Encoded string
			 * ----------------------------------------
			*/

			inline static auto encode(
				JSContext *context, 
				JSValueConst this_val, 
				int argc, 
				JSValueConst *argv
			) -> JSValue
			{
				M_JS_PROXY_WRAPPER(context, {
				try_assert(argc == 1, fmt::format("{} 1, {}: {}", Kernel::Language::get("kernel.argument_expected"), Kernel::Language::get("kernel.argument_received"), argc));
					auto source = JS::Converter::get_c_string(context, argv[0]);
					auto result = Sen::Kernel::Definition::Encryption::Base64::encode(source.get());
					return JS::Converter::to_string(context, result);
				}, "encode"_sv);
			}

				/**
				 * ----------------------------------------
				 * JavaScript Base64 Encode File as Threads
				 * @returns: Encoded files
				 * ----------------------------------------
				*/

				inline static auto encode_fs_as_multiple_threads(
					JSContext *context, 
					JSValueConst this_val, 
					int argc, 
					JSValueConst *argv
				) -> JSValue
				{
					M_JS_PROXY_WRAPPER(context, {
						auto paths = std::vector<std::vector<std::string>>{};
						for (const auto &i : Range<int>(argc))
						{
							const auto &data = JS::Converter::get_vector<std::string>(context, argv[i]);
							try_assert(data.size() == 2, fmt::format("argument expected {} but received {}", 2, data.size()));
							paths.emplace_back(data);
						}
						Sen::Kernel::Definition::Encryption::Base64::encode_fs_as_multiple_thread(paths);
						return JS::Converter::get_undefined();
					}, "encode_fs_as_multiple_threads"_sv);
				}

				/**
				 * ----------------------------------------
				 * JavaScript Base64 Decode File as Threads
				 * @returns: Decoded files
				 * ----------------------------------------
				*/

				inline static auto decode_fs_as_multiple_threads(
					JSContext *context, 
					JSValueConst this_val, 
					int argc, 
					JSValueConst *argv
				) -> JSValue
				{
					M_JS_PROXY_WRAPPER(context, {
						auto paths = std::vector<std::vector<std::string>>{};
						for (const auto &i : Range<int>(argc))
						{
							const auto &data = JS::Converter::get_vector<std::string>(context, argv[i]);
							try_assert(data.size() == 2, fmt::format("argument expected {} but received {}", 2, data.size()));
							paths.emplace_back(data);
						}
						Sen::Kernel::Definition::Encryption::Base64::encode_fs_as_multiple_thread(paths);
						return JS::Converter::get_undefined();
					}, "decode_fs_as_multiple_threads"_sv);
				}

			/**
			 * ----------------------------------------
			 * JavaScript Base 64 Decode method
			 * @param argv[0]: source to decode
			 * @returns: Decoded string
			 * ----------------------------------------
			*/

			inline static auto decode(
				JSContext *context, 
				JSValueConst this_val, 
				int argc, 
				JSValueConst *argv
			) -> JSValue
			{
				M_JS_PROXY_WRAPPER(context, {
				try_assert(argc == 1, fmt::format("{} 1, {}: {}", Kernel::Language::get("kernel.argument_expected"), Kernel::Language::get("kernel.argument_received"), argc));
					auto source = JS::Converter::get_string(context, argv[0]);
					auto result = Sen::Kernel::Definition::Encryption::Base64::decode(source);
					return JS::Converter::to_string(context, result);
				}, "decode"_sv);
			}

			/**
			 * ----------------------------------------
			 * JavaScript Base 64 Encode method
			 * @param argv[0]: source to encode
			 * @returns: Encoded file
			 * ----------------------------------------
			*/

			inline static auto encode_fs(
				JSContext *context, 
				JSValueConst this_val, 
				int argc, 
				JSValueConst *argv
			) -> JSValue
			{
				M_JS_PROXY_WRAPPER(context, {
				try_assert(argc == 2, fmt::format("{} 2, {}: {}", Kernel::Language::get("kernel.argument_expected"), Kernel::Language::get("kernel.argument_received"), argc));
					auto source = JS::Converter::get_string(context, argv[0]);
					auto destination = JS::Converter::get_string(context, argv[1]);
					Sen::Kernel::Definition::Encryption::Base64::encode_fs(source, destination);
					return JS::Converter::get_undefined();
				}, "encode_fs"_sv);
			}

			/**
			 * ----------------------------------------
			 * JavaScript Base 64 Encode method
			 * @param argv[0]: source to encode
			 * @returns: Encoded file
			 * ----------------------------------------
			*/

			inline static auto decode_fs(
				JSContext *context, 
				JSValueConst this_val, 
				int argc, 
				JSValueConst *argv
			) -> JSValue
			{
				M_JS_PROXY_WRAPPER(context, {
				try_assert(argc == 2, fmt::format("{} 2, {}: {}", Kernel::Language::get("kernel.argument_expected"), Kernel::Language::get("kernel.argument_received"), argc));
					auto source = JS::Converter::get_string(context, argv[0]);
					auto destination = JS::Converter::get_string(context, argv[1]);
					Sen::Kernel::Definition::Encryption::Base64::decode_fs(source, destination);
					return JS::Converter::get_undefined();
				}, "decode_fs"_sv);
			}
		}

		/**
		 * JavaScript SHA224 Hash 
		*/

		namespace SHA224 {
			/**
			 * ----------------------------------------
			 * JavaScript SHA224 Hash
			 * @param argv[0]: source to encode
			 * @returns: Encoded file
			 * ----------------------------------------
			*/

			inline static auto hash(
				JSContext *context, 
				JSValueConst this_val, 
				int argc, 
				JSValueConst *argv
			) -> JSValue
			{
				M_JS_PROXY_WRAPPER(context, {
				try_assert(argc == 1, fmt::format("{} 1, {}: {}", Kernel::Language::get("kernel.argument_expected"), Kernel::Language::get("kernel.argument_received"), argc));
					auto source = JS::Converter::get_string(context, argv[0]);
					auto result = Sen::Kernel::Definition::Encryption::Sha224::hash(source);
					return JS::Converter::to_string(context, result);
				}, "hash"_sv);
			}

			/**
			 * ----------------------------------------
			 * JavaScript SHA224 Hash File System
			 * @param argv[0]: source to encode
			 * @returns: Encoded file
			 * ----------------------------------------
			*/

			inline static auto hash_fs(
				JSContext *context, 
				JSValueConst this_val, 
				int argc, 
				JSValueConst *argv
			) -> JSValue
			{
				M_JS_PROXY_WRAPPER(context, {
				try_assert(argc == 1, fmt::format("{} 1, {}: {}", Kernel::Language::get("kernel.argument_expected"), Kernel::Language::get("kernel.argument_received"), argc));
					auto source = JS::Converter::get_string(context, argv[0]);
					auto result = Sen::Kernel::Definition::Encryption::Sha224::hash_fs(source);
					return JS::Converter::to_string(context, result);
				}, "hash_fs"_sv);
			}

		}

		/**
		 * JavaScript SHA256 Support
		*/

		namespace SHA256 {
			/**
			 * ----------------------------------------
			 * JavaScript SHA256 Hash
			 * @param argv[0]: source to encode
			 * @returns: Encoded file
			 * ----------------------------------------
			*/

			inline static auto hash(
				JSContext *context, 
				JSValueConst this_val, 
				int argc, 
				JSValueConst *argv
			) -> JSValue
			{
				M_JS_PROXY_WRAPPER(context, {
				try_assert(argc == 1, fmt::format("{} 1, {}: {}", Kernel::Language::get("kernel.argument_expected"), Kernel::Language::get("kernel.argument_received"), argc));
					auto source = JS::Converter::get_string(context, argv[0]);
					auto result = Sen::Kernel::Definition::Encryption::SHA256::hash(source);
					return JS::Converter::to_string(context, result);
				}, "hash"_sv);
			}

			/**
			 * ----------------------------------------
			 * JavaScript SHA256 Hash File System
			 * @param argv[0]: source to encode
			 * @returns: Encoded file
			 * ----------------------------------------
			*/

			inline static auto hash_fs(
				JSContext *context, 
				JSValueConst this_val, 
				int argc, 
				JSValueConst *argv
			) -> JSValue
			{
				M_JS_PROXY_WRAPPER(context, {
				try_assert(argc == 1, fmt::format("{} 1, {}: {}", Kernel::Language::get("kernel.argument_expected"), Kernel::Language::get("kernel.argument_received"), argc));
					auto source = JS::Converter::get_string(context, argv[0]);
					auto result = Sen::Kernel::Definition::Encryption::SHA256::hash_fs(source);
					return JS::Converter::to_string(context, result);
				}, "hash_fs"_sv);
			}
		}

		/**
		 * JavaScript SHA384 hash Support
		*/

		namespace SHA384 {
			/**
			 * ----------------------------------------
			 * JavaScript SHA384 Hash
			 * @param argv[0]: source to encode
			 * @returns: Encoded file
			 * ----------------------------------------
			*/

			inline static auto hash(
				JSContext *context, 
				JSValueConst this_val, 
				int argc, 
				JSValueConst *argv
			) -> JSValue
			{
				M_JS_PROXY_WRAPPER(context, {
				try_assert(argc == 1, fmt::format("{} 1, {}: {}", Kernel::Language::get("kernel.argument_expected"), Kernel::Language::get("kernel.argument_received"), argc));
					auto source = JS::Converter::get_string(context, argv[0]);
					auto result = Sen::Kernel::Definition::Encryption::SHA384::hash(source);
					return JS::Converter::to_string(context, result);
				}, "hash"_sv);
			}

			/**
			 * ----------------------------------------
			 * JavaScript SHA384 Hash File System
			 * @param argv[0]: source to encode
			 * @returns: Encoded file
			 * ----------------------------------------
			*/

			inline static auto hash_fs(
				JSContext *context, 
				JSValueConst this_val, 
				int argc, 
				JSValueConst *argv
			) -> JSValue
			{
				M_JS_PROXY_WRAPPER(context, {
				try_assert(argc == 1, fmt::format("{} 1, {}: {}", Kernel::Language::get("kernel.argument_expected"), Kernel::Language::get("kernel.argument_received"), argc));
					auto source = JS::Converter::get_string(context, argv[0]);
					auto result = Sen::Kernel::Definition::Encryption::SHA384::hash_fs(source);
					return JS::Converter::to_string(context, result);
				}, "hash_fs"_sv);
			}
		}

		/**
		 * JavaScript SHA512 hash Support
		*/

		namespace SHA512 {
			/**
			 * ----------------------------------------
			 * JavaScript SHA512 Hash
			 * @param argv[0]: source to encode
			 * @returns: Encoded file
			 * ----------------------------------------
			*/

			inline static auto hash(
				JSContext *context, 
				JSValueConst this_val, 
				int argc, 
				JSValueConst *argv
			) -> JSValue
			{
				M_JS_PROXY_WRAPPER(context, {
				try_assert(argc == 1, fmt::format("{} 1, {}: {}", Kernel::Language::get("kernel.argument_expected"), Kernel::Language::get("kernel.argument_received"), argc));
					auto source = JS::Converter::get_string(context, argv[0]);
					auto result = Sen::Kernel::Definition::Encryption::SHA512::hash(source);
					return JS::Converter::to_string(context, result);
				}, "hash"_sv);
			}

			/**
			 * ----------------------------------------
			 * JavaScript SHA512 Hash File System
			 * @param argv[0]: source to encode
			 * @returns: Encoded file
			 * ----------------------------------------
			*/

			inline static auto hash_fs(
				JSContext *context, 
				JSValueConst this_val, 
				int argc, 
				JSValueConst *argv
			) -> JSValue
			{
				M_JS_PROXY_WRAPPER(context, {
				try_assert(argc == 1, fmt::format("{} 1, {}: {}", Kernel::Language::get("kernel.argument_expected"), Kernel::Language::get("kernel.argument_received"), argc));
					auto source = JS::Converter::get_string(context, argv[0]);
					auto result = Sen::Kernel::Definition::Encryption::SHA512::hash_fs(source);
					return JS::Converter::to_string(context, result);
				}, "hash_fs"_sv);
			}
		}

		/**
		 * JavaScript XOR Encryption
		*/

		namespace XOR {
			/**
			 * ----------------------------------------
			 * JavaScript XOR Encryption
			 * @param argv[0]: key
			 * @param argv[1]: plain
			 * @returns: Encoded file
			 * ----------------------------------------
			*/

			inline static auto encrypt(
				JSContext *context, 
				JSValueConst this_val, 
				int argc, 
				JSValueConst *argv
			) -> JSValue
			{
				M_JS_PROXY_WRAPPER(context, {
				try_assert(argc == 2, fmt::format("{} 2, {}: {}", Kernel::Language::get("kernel.argument_expected"), Kernel::Language::get("kernel.argument_received"), argc));
					auto plain = JS::Converter::get_string(context, argv[0]);
					auto key = JS::Converter::get_string(context, argv[1]);
					auto result = Sen::Kernel::Definition::Encryption::XOR::encrypt(plain, key.c_str());
					return JS::Converter::to_string(context, result);
				}, "encrypt"_sv);
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

			inline static auto encrypt_fs(
				JSContext *context, 
				JSValueConst this_val, 
				int argc, 
				JSValueConst *argv
			) -> JSValue
			{
				M_JS_PROXY_WRAPPER(context, {
					try_assert(argc == 3, fmt::format("argument expected {} but received {}", 3, argc));
					auto source = JS::Converter::get_string(context, argv[0]);
					auto destination = JS::Converter::get_string(context, argv[1]);
					auto key = JS::Converter::get_string(context, argv[2]);
					Sen::Kernel::Definition::Encryption::XOR::encrypt_fs(source, destination, key.c_str());
					return JS::Converter::get_undefined();
				}, "encrypt_fs"_sv);
			}

		}
	}

	/**
	 * JavaScript Compression Support
	*/

	namespace Compression {
	
		/**
		 * JS Zip Support
		*/

		namespace Zip {

			/**
			 * Zip Compress
			*/

			namespace Compress {
				/**
				 * ----------------------------------------
				 * Zip uncompress to directory
				 * @param argv[0]: source
				 * @param argv[1]: destination
				 * @returns: Compressed file
				 * ----------------------------------------
				*/

				inline static auto directory(
					JSContext *context, 
					JSValueConst this_val, 
					int argc, 
					JSValueConst *argv
				) -> JSValue
				{
					M_JS_PROXY_WRAPPER(context, {
				try_assert(argc == 2, fmt::format("{} 2, {}: {}", Kernel::Language::get("kernel.argument_expected"), Kernel::Language::get("kernel.argument_received"), argc));
						auto source = JS::Converter::get_string(context, argv[0]);
						auto destination = JS::Converter::get_string(context, argv[1]);
						Sen::Kernel::Definition::Compression::Zip::Compress::directory(source, destination);
						return JS::Converter::get_undefined();
					}, "directory"_sv);
				}

				/**
				 * ----------------------------------------
				 * Zip compression for files
				 * @param argv[0]: source array
				 * @param argv[1]: destination
				 * @param argv[2]: Root?
				 * @returns: Compressed file
				 * ----------------------------------------
				*/

				inline static auto file(
					JSContext *context, 
					JSValueConst this_val, 
					int argc, 
					JSValueConst *argv
				) -> JSValue
				{
					M_JS_PROXY_WRAPPER(context, {
						try_assert(argc == 3 || argc == 2, fmt::format("argument expected {} but received {}", "2 or 3", argc));
						auto source = JS::Converter::get_vector<std::string>(context, argv[0]);
						auto destination = JS::Converter::get_string(context, argv[1]);
						if (argc == 3)
						{
							auto root = JS::Converter::get_string(context, argv[2]);
							Sen::Kernel::Definition::Compression::Zip::Compress::file(source, destination, root);
						}
						else
						{
							Sen::Kernel::Definition::Compression::Zip::Compress::file(source, destination);
						}
						return JS::Converter::get_undefined();
					}, "file"_sv);
				}

			}

			/**
			 * Uncompress support
			*/

			namespace Uncompress {

				/**
				 * ----------------------------------------
				 * Zip uncompress a directory
				 * @param argv[0]: source
				 * @param argv[1]: destination
				 * @returns: Compressed file
				 * ----------------------------------------
				*/

				inline static auto process(
					JSContext *context, 
					JSValueConst this_val, 
					int argc, 
					JSValueConst *argv
				) -> JSValue
				{
					M_JS_PROXY_WRAPPER(context, {
				try_assert(argc == 2, fmt::format("{} 2, {}: {}", Kernel::Language::get("kernel.argument_expected"), Kernel::Language::get("kernel.argument_received"), argc));
						auto source = JS::Converter::get_string(context, argv[0]);
						auto destination = JS::Converter::get_string(context, argv[1]);
						Sen::Kernel::Definition::Compression::Zip::Uncompress::process(source, destination);
						return JS::Converter::get_undefined();
					}, "process"_sv);
				}

			}

		}


		/**
		 * JavaScript Zlib Support
		*/

		namespace Zlib {
			/**
			 * ----------------------------------------
			 * JavaScript Zlib Compression File
			 * @param argv[0]: source file
			 * @param argv[1]: destination file
			 * @param argv[2]: level
			 * @returns: Compressed file
			 * ----------------------------------------
			*/

			inline static auto compress_fs(
				JSContext *context, 
				JSValueConst this_val, 
				int argc, 
				JSValueConst *argv
			) -> JSValue
			{
				M_JS_PROXY_WRAPPER(context, {
					try_assert(argc == 3, fmt::format("argument expected {} but received {}", 3, argc));
					auto source = JS::Converter::get_string(context, argv[0]);
					auto destination = JS::Converter::get_string(context, argv[1]);
					auto level = JS::Converter::get_int32(context, argv[2]);
					if (!(static_cast<int>(Sen::Kernel::Definition::Compression::Zlib::Level::DEFAULT) <= level or level <= static_cast<int>(Sen::Kernel::Definition::Compression::Zlib::Level::LEVEL_9)))
					{
						throw std::invalid_argument(fmt::format("{}, {} {}", Kernel::Language::get("zlib.compress.invalid_level"), Kernel::Language::get("but_received"), level));
					}
					Sen::Kernel::Definition::Compression::Zlib::compress_fs(source, destination, static_cast<Sen::Kernel::Definition::Compression::Zlib::Level>(level));
					return JS::Converter::get_undefined();
				}, "compress_fs"_sv);
			}

			/**
			 * ----------------------------------------
			 * JavaScript Zlib Uncompression File
			 * @param argv[0]: source file
			 * @param argv[1]: destination file
			 * @returns: Uncompressed file
			 * ----------------------------------------
			*/

			inline static auto uncompress_fs(
				JSContext *context, 
				JSValueConst this_val, 
				int argc, 
				JSValueConst *argv
			) -> JSValue
			{
				M_JS_PROXY_WRAPPER(context, {
				try_assert(argc == 2, fmt::format("{} 2, {}: {}", Kernel::Language::get("kernel.argument_expected"), Kernel::Language::get("kernel.argument_received"), argc));
					auto source = JS::Converter::get_string(context, argv[0]);
					auto destination = JS::Converter::get_string(context, argv[1]);
					Sen::Kernel::Definition::Compression::Zlib::uncompress_fs(source, destination);
					return JS::Converter::get_undefined();
				}, "uncompress_fs"_sv);
			}
			
		}

		/**
		 * JavaScript Bzip2 Support
		*/

		namespace Bzip2 {
			/**
			 * ----------------------------------------
			 * JavaScript Bzip2 Compression File
			 * @param argv[0]: source file
			 * @param argv[1]: destination file
			 * @param argv[2]: level
			 * @returns: Compressed file
			 * ----------------------------------------
			*/

			inline static auto compress_fs(
				JSContext *context, 
				JSValueConst this_val, 
				int argc, 
				JSValueConst *argv
			) -> JSValue
			{
				M_JS_PROXY_WRAPPER(context, {
				try_assert(argc == 2, fmt::format("{} 2, {}: {}", Kernel::Language::get("kernel.argument_expected"), Kernel::Language::get("kernel.argument_received"), argc));
					auto source = JS::Converter::get_string(context, argv[0]);
					auto destination = JS::Converter::get_string(context, argv[1]);
					Sen::Kernel::Definition::Compression::Bzip2::compress_fs(source, destination);
					return JS::Converter::get_undefined();
				}, "compress_fs"_sv);
			}

			/**
			 * ----------------------------------------
			 * JavaScript Bzip2 Uncompression File
			 * @param argv[0]: source file
			 * @param argv[1]: destination file
			 * @returns: Uncompressed file
			 * ----------------------------------------
			*/

			inline static auto uncompress_fs(
				JSContext *context, 
				JSValueConst this_val, 
				int argc, 
				JSValueConst *argv
			) -> JSValue
			{
				M_JS_PROXY_WRAPPER(context, {
				try_assert(argc == 2, fmt::format("{} 2, {}: {}", Kernel::Language::get("kernel.argument_expected"), Kernel::Language::get("kernel.argument_received"), argc));
					auto source = JS::Converter::get_string(context, argv[0]);
					auto destination = JS::Converter::get_string(context, argv[1]);
					Sen::Kernel::Definition::Compression::Bzip2::uncompress_fs(source, destination);
					return JS::Converter::get_undefined();
				}, "uncompress_fs"_sv);
			}
			
		}

		/**
		 * JavaScript Lzma Support
		*/

		namespace Lzma {
			/**
			 * ----------------------------------------
			 * JavaScript Lzma Compression File
			 * @param argv[0]: source file
			 * @param argv[1]: destination file
			 * @param argv[2]: level
			 * @returns: Compressed file
			 * ----------------------------------------
			*/

			inline static auto compress_fs(
				JSContext *context, 
				JSValueConst this_val, 
				int argc, 
				JSValueConst *argv
			) -> JSValue
			{
				M_JS_PROXY_WRAPPER(context, {
				try_assert(argc == 3, fmt::format("{} 3, {}: {}", Kernel::Language::get("kernel.argument_expected"), Kernel::Language::get("kernel.argument_received"), argc));
					auto source = JS::Converter::get_c_string(context, argv[0]);
					auto destination = JS::Converter::get_c_string(context, argv[1]);
					auto level = JS::Converter::get_bigint64(context, argv[2]);
					Sen::Kernel::Definition::Compression::Lzma::compress_fs(source.get(), destination.get(), static_cast<Sen::Kernel::Definition::Compression::Lzma::Level>(level));
					return JS::Converter::get_undefined();
				}, "compress_fs"_sv);
			}

			/**
			 * ----------------------------------------
			 * JavaScript Lzma Uncompression File
			 * @param argv[0]: source file
			 * @param argv[1]: destination file
			 * @param argv[2]: actual size
			 * @returns: Uncompressed file
			 * ----------------------------------------
			*/

			inline static auto uncompress_fs(
				JSContext *context, 
				JSValueConst this_val, 
				int argc, 
				JSValueConst *argv
			) -> JSValue
			{
				M_JS_PROXY_WRAPPER(context, {
					try_assert(argc == 3, fmt::format("argument expected {} but received {}", 3, argc));
					auto source = JS::Converter::get_string(context, argv[0]);
					auto destination = JS::Converter::get_string(context, argv[1]);
					auto actual_size = JS::Converter::get_uint64(context, argv[2]);
					Sen::Kernel::Definition::Compression::Lzma::uncompress_fs<std::size_t>(source, destination, actual_size);
					return JS::Converter::get_undefined();
				}, "uncompress_fs"_sv);
			}
			
		}

		/**
		 * JavaScript Gzip Compression Support
		*/

		namespace Gzip {
			/**
			 * ----------------------------------------
			 * JavaScript Gzip Compression File
			 * @param argv[0]: source file
			 * @param argv[1]: destination file
			 * @returns: Compressed file
			 * ----------------------------------------
			*/

			inline static auto compress_fs(
				JSContext *context, 
				JSValueConst this_val, 
				int argc, 
				JSValueConst *argv
			) -> JSValue
			{
				M_JS_PROXY_WRAPPER(context, {
				try_assert(argc == 3, fmt::format("{} 3, {}: {}", Kernel::Language::get("kernel.argument_expected"), Kernel::Language::get("kernel.argument_received"), argc));
					auto source = JS::Converter::get_string(context, argv[0]);
					auto destination = JS::Converter::get_string(context, argv[1]);
					auto level = JS::Converter::get_int32(context, argv[2]);
					if (!(static_cast<int>(Sen::Kernel::Definition::Compression::Zlib::Level::DEFAULT) <= level or level <= static_cast<int>(Sen::Kernel::Definition::Compression::Zlib::Level::LEVEL_9)))
					{
						throw std::invalid_argument(fmt::format("{}, {} {}", Kernel::Language::get("zlib.compress.invalid_level"), Kernel::Language::get("but_received"), level));
					}
					Sen::Kernel::Definition::Compression::Zlib::compress_gzip_fs(source, destination, static_cast<Sen::Kernel::Definition::Compression::Zlib::Level>(level));
					return JS::Converter::get_undefined();
				}, "compress_fs"_sv);
			}

			/**
			 * ----------------------------------------
			 * JavaScript Gzip Uncompression File
			 * @param argv[0]: source file
			 * @param argv[1]: destination file
			 * @returns: Uncompressed file
			 * ----------------------------------------
			*/

			inline static auto uncompress_fs(
				JSContext *context, 
				JSValueConst this_val, 
				int argc, 
				JSValueConst *argv
			) -> JSValue
			{
				M_JS_PROXY_WRAPPER(context, {
				try_assert(argc == 2, fmt::format("{} 2, {}: {}", Kernel::Language::get("kernel.argument_expected"), Kernel::Language::get("kernel.argument_received"), argc));
					auto source = JS::Converter::get_string(context, argv[0]);
					auto destination = JS::Converter::get_string(context, argv[1]);
					Sen::Kernel::Definition::Compression::Zlib::uncompress_gzip_fs(source, destination);
					return JS::Converter::get_undefined();
				}, "uncompress_fs"_sv);
			}
		}
	}

	/**
	 * JavaScript Support
	*/

	namespace Support {

		/**
		 * JavaScript Texture Encode & Decode
		*/

		namespace Texture {

				/**
				 * ----------------------------------------
				 * JavaScript Texture Decode File
				 * @param argv[0]: source file
				 * @param argv[1]: destination file
				 * @param argv[2]: width
				 * @param argv[3]: height
				 * @returns: Decoded file
				 * ----------------------------------------
				*/

				inline static auto decode_fs(
					JSContext *context, 
					JSValueConst this_val, 
					int argc, 
					JSValueConst *argv
				) -> JSValue
				{
					M_JS_PROXY_WRAPPER(context, {
						try_assert(argc == 5, fmt::format("argument expected {} but received {}", 5, argc));
						auto source = JS::Converter::get_string(context, argv[0]);
						auto destination = JS::Converter::get_string(context, argv[1]);
						auto width = JS::Converter::get_int32(context, argv[2]);
						auto height = JS::Converter::get_int32(context, argv[3]);
						auto format = JS::Converter::get_int32(context, argv[4]);
						Sen::Kernel::Support::Texture::InvokeMethod::decode_fs(source, destination, width, height, static_cast<Sen::Kernel::Support::Texture::Format>(format));
						return JS::Converter::get_undefined();
					}, "decode_fs"_sv);
				}

				/**
				 * ----------------------------------------
				 * JavaScript Texture Encode File
				 * @param argv[0]: source file
				 * @param argv[1]: destination file
				 * @returns: Encoded file
				 * ----------------------------------------
				*/

				inline static auto encode_fs(
					JSContext *context, 
					JSValueConst this_val, 
					int argc, 
					JSValueConst *argv
				) -> JSValue
				{
					M_JS_PROXY_WRAPPER(context, {
						try_assert(argc == 3, fmt::format("argument expected {} but received {}", 3, argc));
						auto source = JS::Converter::get_string(context, argv[0]);
						auto destination = JS::Converter::get_string(context, argv[1]);
						auto format = JS::Converter::get_int32(context, argv[2]);
						Sen::Kernel::Support::Texture::InvokeMethod::encode_fs(source, destination, static_cast<Sen::Kernel::Support::Texture::Format>(format));
						return JS::Converter::get_undefined();
					}, "encode_fs"_sv);
				}

		}

		/**
		 * JavaScript PopCap Support
		*/

		namespace PopCap {

				/**
			 * JavaScript Zlib Support
			*/

			namespace Zlib {
				/**
				 * ----------------------------------------
				 * JavaScript Zlib Compression File
				 * @param argv[0]: source file
				 * @param argv[1]: destination file
				 * @param argv[2]: use_64_bit_variant
				 * @returns: Compressed file
				 * ----------------------------------------
				*/

				inline static auto compress_fs(
					JSContext *context, 
					JSValueConst this_val, 
					int argc, 
					JSValueConst *argv
				) -> JSValue
				{
					M_JS_PROXY_WRAPPER(context, {
						try_assert(argc == 3, fmt::format("argument expected {} but received {}", 3, argc));
						auto source = JS::Converter::get_string(context, argv[0]);
						auto destination = JS::Converter::get_string(context, argv[1]);
						auto use_64_bit_variant = JS::Converter::get_bool(context, argv[2]);
						Sen::Kernel::Support::PopCap::Zlib::Compress::compress_fs(source, destination, use_64_bit_variant);
						return JS::Converter::get_undefined();
					}, "compress_fs"_sv);
				}

				/**
				 * ----------------------------------------
				 * JavaScript Zlib Uncompression File
				 * @param argv[0]: source file
				 * @param argv[1]: destination file
				 * @returns: Uncompressed file
				 * ----------------------------------------
				*/

				inline static auto uncompress_fs(
					JSContext *context, 
					JSValueConst this_val, 
					int argc, 
					JSValueConst *argv
				) -> JSValue
				{
					M_JS_PROXY_WRAPPER(context, {
						try_assert(argc == 3, fmt::format("argument expected {} but received {}", 3, argc));
						auto source = JS::Converter::get_string(context, argv[0]);
						auto destination = JS::Converter::get_string(context, argv[1]);
						auto use_64_bit_variant = JS::Converter::get_bool(context, argv[2]);
						Sen::Kernel::Support::PopCap::Zlib::Compress::compress_fs(source, destination, use_64_bit_variant);
						return JS::Converter::get_undefined();
					}, "uncompress_fs"_sv);
				}
				
			}

			/**
			 * JavaScript Newton Support
			*/

			namespace CompiledText {

				/**
				 * ----------------------------------------
				 * JavaScript Compiled Text Decode File
				 * @param argv[0]: source file
				 * @param argv[1]: destination file
				 * @param argv[2]: key
				 * @param argv[3]: iv
				 * @param argv[4]: use 64bit variant
				 * @returns: Decoded file
				 * ----------------------------------------
				*/

				inline static auto decode_fs(
					JSContext *context, 
					JSValueConst this_val, 
					int argc, 
					JSValueConst *argv
				) -> JSValue
				{
					M_JS_PROXY_WRAPPER(context, {
						try_assert(argc == 5, fmt::format("argument expected {} but received {}", 5, argc));
						auto source = JS::Converter::get_string(context, argv[0]);
						auto destination = JS::Converter::get_string(context, argv[1]);
						auto key = JS::Converter::get_string(context, argv[2]);
						auto iv = JS::Converter::get_string(context, argv[3]);
						auto use_64_bit_variant = JS::Converter::get_bool(context, argv[4]);
						Sen::Kernel::Support::PopCap::CompiledText::Decode::process_fs(source, destination, key, iv, use_64_bit_variant);
						return JS::Converter::get_undefined();
					}, "decode_fs"_sv);
				}

				/**
				 * ----------------------------------------
				 * JavaScript Compiled Text Encode File
				 * @param argv[0]: source file
				 * @param argv[1]: destination file
				 * @returns: Encoded file
				 * ----------------------------------------
				*/

				inline static auto encode_fs(
					JSContext *context, 
					JSValueConst this_val, 
					int argc, 
					JSValueConst *argv
				) -> JSValue
				{
					M_JS_PROXY_WRAPPER(context, {
						try_assert(argc == 5, fmt::format("argument expected {} but received {}", 5, argc));
						auto source = JS::Converter::get_string(context, argv[0]);
						auto destination = JS::Converter::get_string(context, argv[1]);
						auto key = JS::Converter::get_string(context, argv[2]);
						auto iv = JS::Converter::get_string(context, argv[3]);
						auto use_64_bit_variant = JS::Converter::get_bool(context, argv[4]);
						Sen::Kernel::Support::PopCap::CompiledText::Encode::process_fs(source, destination, key, iv, use_64_bit_variant);
						return JS::Converter::get_undefined();
					}, "encode_fs"_sv);
				}

			}
			

			/**
			 * JavaScript Resource Group Support
			*/

			namespace ResourceGroup {

				/**
				 * ----------------------------------------
				 * JavaScript Resource Group Split File
				 * @param argv[0]: source file
				 * @param argv[1]: destination file
				 * @returns: Splitted file
				 * ----------------------------------------
				*/

				inline static auto split_fs(
					JSContext *context, 
					JSValueConst this_val, 
					int argc, 
					JSValueConst *argv
				) -> JSValue
				{
					M_JS_PROXY_WRAPPER(context, {
				try_assert(argc == 2, fmt::format("{} 2, {}: {}", Kernel::Language::get("kernel.argument_expected"), Kernel::Language::get("kernel.argument_received"), argc));
						auto source = JS::Converter::get_string(context, argv[0]);
						auto destination = JS::Converter::get_string(context, argv[1]);
						Sen::Kernel::Support::PopCap::ResourceGroup::BasicConversion::split(source, destination);
						return JS::Converter::get_undefined();
					}, "split_fs"_sv);
				}

				/**
				 * ----------------------------------------
				 * JavaScript Resource Group Merge File
				 * @param argv[0]: source file
				 * @param argv[1]: destination file
				 * @returns: Merged file
				 * ----------------------------------------
				*/

				inline static auto merge_fs(
					JSContext *context, 
					JSValueConst this_val, 
					int argc, 
					JSValueConst *argv
				) -> JSValue
				{
					M_JS_PROXY_WRAPPER(context, {
				try_assert(argc == 2, fmt::format("{} 2, {}: {}", Kernel::Language::get("kernel.argument_expected"), Kernel::Language::get("kernel.argument_received"), argc));
						auto source = JS::Converter::get_string(context, argv[0]);
						auto destination = JS::Converter::get_string(context, argv[1]);
						Sen::Kernel::Support::PopCap::ResourceGroup::BasicConversion::merge(source, destination);
						return JS::Converter::get_undefined();
					}, "merge_fs"_sv);
				}

				/**
				 * ----------------------------------------
				 * JavaScript Resource Group Convert File
				 * @param argv[0]: source file
				 * @param argv[1]: destination file
				 * @param argv[2]: path style
				 * @returns: Converted file
				 * ----------------------------------------
				*/

				inline static auto convert_fs(
					JSContext *context, 
					JSValueConst this_val, 
					int argc, 
					JSValueConst *argv
				) -> JSValue
				{
					M_JS_PROXY_WRAPPER(context, {
						try_assert(argc == 3, fmt::format("argument expected {} but received {}", 3, argc));
						auto source = JS::Converter::get_string(context, argv[0]);
						auto destination = JS::Converter::get_string(context, argv[1]);
						auto path_style = JS::Converter::get_int32(context, argv[2]);
						Sen::Kernel::Support::PopCap::ResourceGroup::Convert::convert_fs(source, destination, static_cast<Sen::Kernel::Support::PopCap::ResourceGroup::PathStyle>(path_style));
						return JS::Converter::get_undefined();
					}, "convert_fs"_sv);
				}


			}

			/**
			 * JavaScript Res Info Support
			*/

			namespace ResInfo {

				/**
				 * ----------------------------------------
				 * JavaScript ResInfo Split File
				 * @param argv[0]: source file
				 * @param argv[1]: destination file
				 * @returns: Splitted file
				 * ----------------------------------------
				*/

				inline static auto split_fs(
					JSContext *context, 
					JSValueConst this_val, 
					int argc, 
					JSValueConst *argv
				) -> JSValue
				{
					M_JS_PROXY_WRAPPER(context, {
				try_assert(argc == 2, fmt::format("{} 2, {}: {}", Kernel::Language::get("kernel.argument_expected"), Kernel::Language::get("kernel.argument_received"), argc));
						auto source = JS::Converter::get_string(context, argv[0]);
						auto destination = JS::Converter::get_string(context, argv[1]);
						Sen::Kernel::Support::PopCap::ResInfo::BasicConversion::split_fs(source, destination);
						return JS::Converter::get_undefined();
					}, "split_fs"_sv);
				}

				/**
				 * ----------------------------------------
				 * JavaScript ResInfo Merge File
				 * @param argv[0]: source file
				 * @param argv[1]: destination file
				 * @returns: Merged file
				 * ----------------------------------------
				*/

				inline static auto merge_fs(
					JSContext *context, 
					JSValueConst this_val, 
					int argc, 
					JSValueConst *argv
				) -> JSValue
				{
					M_JS_PROXY_WRAPPER(context, {
				try_assert(argc == 2, fmt::format("{} 2, {}: {}", Kernel::Language::get("kernel.argument_expected"), Kernel::Language::get("kernel.argument_received"), argc));
						auto source = JS::Converter::get_string(context, argv[0]);
						auto destination = JS::Converter::get_string(context, argv[1]);
						Sen::Kernel::Support::PopCap::ResInfo::BasicConversion::merge_fs(source, destination);
						return JS::Converter::get_undefined();
					}, "merge_fs"_sv);
				}

				/**
				 * ----------------------------------------
				 * JavaScript ResInfo Convert File
				 * @param argv[0]: source file
				 * @param argv[1]: destination file
				 * @returns: Converted file
				 * ----------------------------------------
				*/

				inline static auto convert_fs(
					JSContext *context, 
					JSValueConst this_val, 
					int argc, 
					JSValueConst *argv
				) -> JSValue
				{
					M_JS_PROXY_WRAPPER(context, {
						try_assert(argc == 2, fmt::format("argument expected {} but received {}", 2, argc));
						auto source = JS::Converter::get_c_string(context, argv[0]);
						auto destination = JS::Converter::get_c_string(context, argv[1]);
						Sen::Kernel::Support::PopCap::ResInfo::Convert::convert_fs(source.get(), destination.get());
						return JS::Converter::get_undefined();
					}, "convert_fs"_sv);
				}


			}

			/**
			 * JavaScript RenderEffects Support
			*/

			namespace RenderEffects {

				/**
				 * ----------------------------------------
				 * JavaScript RenderEffects Decode File
				 * @param argv[0]: source file
				 * @param argv[1]: destination file
				 * @returns: Decoded file
				 * ----------------------------------------
				*/

				inline static auto decode_fs(
					JSContext *context, 
					JSValueConst this_val, 
					int argc, 
					JSValueConst *argv
				) -> JSValue
				{
					M_JS_PROXY_WRAPPER(context, {
				try_assert(argc == 2, fmt::format("{} 2, {}: {}", Kernel::Language::get("kernel.argument_expected"), Kernel::Language::get("kernel.argument_received"), argc));
						auto source = JS::Converter::get_string(context, argv[0]);
						auto destination = JS::Converter::get_string(context, argv[1]);
						Sen::Kernel::Support::PopCap::RenderEffects::Decode::process_fs(source, destination);
						return JS::Converter::get_undefined();
					}, "decode_fs"_sv);
				}

				/**
				 * ----------------------------------------
				 * JavaScript RenderEffects Encode File
				 * @param argv[0]: source file
				 * @param argv[1]: destination file
				 * @returns: Encoded file
				 * ----------------------------------------
				*/

				inline static auto encode_fs(
					JSContext *context, 
					JSValueConst this_val, 
					int argc, 
					JSValueConst *argv
				) -> JSValue
				{
					M_JS_PROXY_WRAPPER(context, {
				try_assert(argc == 2, fmt::format("{} 2, {}: {}", Kernel::Language::get("kernel.argument_expected"), Kernel::Language::get("kernel.argument_received"), argc));
						auto source = JS::Converter::get_string(context, argv[0]);
						auto destination = JS::Converter::get_string(context, argv[1]);
						Sen::Kernel::Support::PopCap::RenderEffects::Encode::process_fs(source, destination);
						return JS::Converter::get_undefined();
					}, "encode_fs"_sv);
				}


			}

			/**
			 * JavaScript CharacterFontWidget2 Support
			*/

			namespace CharacterFontWidget2 {

				/**
				 * ----------------------------------------
				 * CFW2 Decode File
				 * @param argv[0]: source file
				 * @param argv[1]: destination file
				 * @returns: Decoded file
				 * ----------------------------------------
				*/

				inline static auto decode_fs(
					JSContext *context, 
					JSValueConst this_val, 
					int argc, 
					JSValueConst *argv
				) -> JSValue
				{
					M_JS_PROXY_WRAPPER(context, {
						try_assert(argc == 2, fmt::format("{} 2, {}: {}", Kernel::Language::get("kernel.argument_expected"), Kernel::Language::get("kernel.argument_received"), argc));
						auto source = JS::Converter::get_string(context, argv[0]);
						auto destination = JS::Converter::get_string(context, argv[1]);
						Sen::Kernel::Support::PopCap::CFW2::Decode::process_fs(source, destination);
						return JS::Converter::get_undefined();
					}, "decode_fs"_sv);
				}

				/**
				 * ----------------------------------------
				 * CFW2 Encode
				 * @param argv[0]: source file
				 * @param argv[1]: destination file
				 * @returns: Encrypted file
				 * ----------------------------------------
				*/

				inline static auto encode_fs(
					JSContext *context, 
					JSValueConst this_val, 
					int argc, 
					JSValueConst *argv
				) -> JSValue
				{
					M_JS_PROXY_WRAPPER(context, {
				try_assert(argc == 2, fmt::format("{} 2, {}: {}", Kernel::Language::get("kernel.argument_expected"), Kernel::Language::get("kernel.argument_received"), argc));
						auto source = JS::Converter::get_string(context, argv[0]);
						auto destination = JS::Converter::get_string(context, argv[1]);
						Sen::Kernel::Support::PopCap::CFW2::Encode::process_fs(source, destination);
						return JS::Converter::get_undefined();
					}, "encode_fs"_sv);
				}


			}

			/**
			 * JavaScript Crypt-Data Support
			*/

			namespace CryptData {

				/**
				 * ----------------------------------------
				 * JavaScript RenderEffects Decode File
				 * @param argv[0]: source file
				 * @param argv[1]: destination file
				 * @returns: Decoded file
				 * ----------------------------------------
				*/

				inline static auto decrypt_fs(
					JSContext *context, 
					JSValueConst this_val, 
					int argc, 
					JSValueConst *argv
				) -> JSValue
				{
					M_JS_PROXY_WRAPPER(context, {
						try_assert(argc == 3, fmt::format("argument expected {} but received {}", 3, argc));
						auto source = JS::Converter::get_string(context, argv[0]);
						auto destination = JS::Converter::get_string(context, argv[1]);
						auto key = JS::Converter::get_string(context, argv[2]);
						Sen::Kernel::Support::PopCap::CryptData::Decrypt::process_fs(source, destination, key);
						return JS::Converter::get_undefined();
					}, "decrypt_fs"_sv);
				}

				/**
				 * ----------------------------------------
				 * JavaScript Crypt-Data Encrypt
				 * @param argv[0]: source file
				 * @param argv[1]: destination file
				 * @param argv[2]: key
				 * @returns: Encrypted file
				 * ----------------------------------------
				*/

				inline static auto encrypt_fs(
					JSContext *context, 
					JSValueConst this_val, 
					int argc, 
					JSValueConst *argv
				) -> JSValue
				{
					M_JS_PROXY_WRAPPER(context, {
						try_assert(argc == 3, fmt::format("argument expected {} but received {}", 3, argc));
						auto source = JS::Converter::get_string(context, argv[0]);
						auto destination = JS::Converter::get_string(context, argv[1]);
						auto key = JS::Converter::get_string(context, argv[2]);
						Sen::Kernel::Support::PopCap::CryptData::Encrypt::process_fs(source, destination, key);
						return JS::Converter::get_undefined();
					}, "encrypt_fs"_sv);
				}


			}

			/**
			 * JavaScript Newton Support
			*/

			namespace Newton {

				/**
				 * ----------------------------------------
				 * JavaScript Newton Decode File
				 * @param argv[0]: source file
				 * @param argv[1]: destination file
				 * @returns: Decoded file
				 * ----------------------------------------
				*/

				inline static auto decode_fs(
					JSContext *context, 
					JSValueConst this_val, 
					int argc, 
					JSValueConst *argv
				) -> JSValue
				{
					M_JS_PROXY_WRAPPER(context, {
				try_assert(argc == 2, fmt::format("{} 2, {}: {}", Kernel::Language::get("kernel.argument_expected"), Kernel::Language::get("kernel.argument_received"), argc));
						auto source = JS::Converter::get_string(context, argv[0]);
						auto destination = JS::Converter::get_string(context, argv[1]);
						Sen::Kernel::Support::PopCap::Newton::Decode::process_fs(source, destination);
						return JS::Converter::get_undefined();
					}, "decode_fs"_sv);
				}

				/**
				 * ----------------------------------------
				 * JavaScript Newton Decode File
				 * @param argv[0]: source file
				 * @param argv[1]: destination file
				 * @returns: Encoded file
				 * ----------------------------------------
				*/

				inline static auto encode_fs(
					JSContext *context, 
					JSValueConst this_val, 
					int argc, 
					JSValueConst *argv
				) -> JSValue
				{
					M_JS_PROXY_WRAPPER(context, {
				try_assert(argc == 2, fmt::format("{} 2, {}: {}", Kernel::Language::get("kernel.argument_expected"), Kernel::Language::get("kernel.argument_received"), argc));
						auto source = JS::Converter::get_string(context, argv[0]);
						auto destination = JS::Converter::get_string(context, argv[1]);
						Sen::Kernel::Support::PopCap::Newton::Encode::process_fs(source, destination);
						return JS::Converter::get_undefined();
					}, "encode_fs"_sv);
				}

			}

			/**
			 * JavaScript RTON Support
			*/
			
			namespace RTON {
				/**
				 * ----------------------------------------
				 * JavaScript RTON Decode File
				 * @param argv[0]: source file
				 * @param argv[1]: destination file
				 * @returns: Decoded file
				 * ----------------------------------------
				*/

				inline static auto decode_fs(
					JSContext *context, 
					JSValueConst this_val, 
					int argc, 
					JSValueConst *argv
				) -> JSValue
				{
					M_JS_PROXY_WRAPPER(context, {
				try_assert(argc == 2, fmt::format("{} 2, {}: {}", Kernel::Language::get("kernel.argument_expected"), Kernel::Language::get("kernel.argument_received"), argc));
						auto source = JS::Converter::get_c_string(context, argv[0]);
						auto destination = JS::Converter::get_c_string(context, argv[1]);
						Sen::Kernel::Support::PopCap::RTON::Decode::decode_fs(source.get(), destination.get());
						return JS::Converter::get_undefined();
					}, "decode_fs"_sv);
				}
				
				/**
				 * ----------------------------------------
				 * JavaScript RTON Decrypt File
				 * @param argv[0]: source 
				 * @param argv[1]: destination file
				 * @param argv[2]: key
				 * @param argv[3]: iv
				 * @returns: Decoded file
				 * ----------------------------------------
				*/

				inline static auto decrypt_fs(
					JSContext *context, 
					JSValueConst this_val, 
					int argc, 
					JSValueConst *argv
				) -> JSValue
				{
					M_JS_PROXY_WRAPPER(context, {
						try_assert(argc == 4, fmt::format("argument expected {} but received {}", 4, argc));
						auto source = JS::Converter::get_c_string(context, argv[0]);
						auto destination = JS::Converter::get_c_string(context, argv[1]);
						auto key = JS::Converter::get_c_string(context, argv[2]);
						auto iv = JS::Converter::get_c_string(context, argv[3]);
						Sen::Kernel::Support::PopCap::RTON::Decode::decrypt_fs(source.get(), destination.get(), key.get(), iv.get());
						return JS::Converter::get_undefined();
					}, "decrypt_fs"_sv);
				}

				/**
				 * ----------------------------------------
				 * JavaScript RTON Decrypt & Decode File
				 * @param argv[0]: source file
				 * @param argv[1]: destination file
				 * @param argv[2]: key
				 * @param argv[3]: iv
				 * @returns: Decoded file
				 * ----------------------------------------
				*/

				inline static auto decrypt_and_decode_fs(
					JSContext* context,
					JSValueConst this_val,
					int argc,
					JSValueConst* argv
				) -> JSValue
				{
					M_JS_PROXY_WRAPPER(context, {
						try_assert(argc == 4, fmt::format("argument expected {} but received {}", 4, argc));
						auto source = JS::Converter::get_c_string(context, argv[0]);
						auto destination = JS::Converter::get_c_string(context, argv[1]);
						auto key = JS::Converter::get_c_string(context, argv[2]);
						auto iv = JS::Converter::get_c_string(context, argv[3]);
						Sen::Kernel::Support::PopCap::RTON::Decode::decrypt_and_decode_fs(source.get(), destination.get(), key.get(), iv.get());
						return JS::Converter::get_undefined();
					}, "decrypt_and_decode_fs"_sv);
				}

				/**
				 * ----------------------------------------
				 * JavaScript RTON Decode File as Threads
				 * @returns: Decoded file
				 * ----------------------------------------
				*/

				inline static auto decode_fs_as_multiple_threads(
					JSContext *context, 
					JSValueConst this_val, 
					int argc, 
					JSValueConst *argv
				) -> JSValue
				{
					M_JS_PROXY_WRAPPER(context, {
						auto paths = std::vector<std::vector<std::string>>{};
						for (const auto &i : Range<int>(argc))
						{
							const auto &data = JS::Converter::get_vector<std::string>(context, argv[i]);
							try_assert(data.size() == 2, fmt::format("argument expected {} but received {}", 2, data.size()));
							paths.emplace_back(data);
						}
						Sen::Kernel::Support::PopCap::RTON::Decode::decode_fs_as_multiple_threads(paths);
						return JS::Converter::get_undefined();
					}, "decode_fs_as_multiple_threads"_sv);
				}

				/**
				 * ----------------------------------------
				 * JavaScript RTON Encode File as Threads
				 * @returns: Decoded file
				 * ----------------------------------------
				*/

				inline static auto encode_fs_as_multiple_threads(
					JSContext *context, 
					JSValueConst this_val, 
					int argc, 
					JSValueConst *argv
				) -> JSValue
				{
					M_JS_PROXY_WRAPPER(context, {
						auto paths = std::vector<std::vector<std::string>>{};
						for (const auto &i : Range<int>(argc))
						{
							const auto &data = JS::Converter::get_vector<std::string>(context, argv[i]);
							try_assert(data.size() == 2, fmt::format("argument expected {} but received {}", 2, data.size()));
							paths.emplace_back(data);
						}
						Sen::Kernel::Support::PopCap::RTON::Encode::encode_fs_as_multiple_threads(paths);
						return JS::Converter::get_undefined();
					}, "encode_fs_as_multiple_threads"_sv);
				}

				/**
				 * ----------------------------------------
				 * JavaScript RTON Encode File
				 * @param argv[0]: source file
				 * @param argv[1]: destination file
				 * @returns: Encoded file
				 * ----------------------------------------
				*/

				inline static auto encode_fs(
					JSContext *context, 
					JSValueConst this_val, 
					int argc, 
					JSValueConst *argv
				) -> JSValue
				{
					M_JS_PROXY_WRAPPER(context, {
				try_assert(argc == 2, fmt::format("{} 2, {}: {}", Kernel::Language::get("kernel.argument_expected"), Kernel::Language::get("kernel.argument_received"), argc));
						auto source = JS::Converter::get_string(context, argv[0]);
						auto destination = JS::Converter::get_string(context, argv[1]);
						Sen::Kernel::Support::PopCap::RTON::Encode::encode_fs(source, destination);
						return JS::Converter::get_undefined();
					}, "encode_fs"_sv);
				}

				/**
				 * ----------------------------------------
				 * JavaScript RTON Encrypt File
				 * @param argv[0]: source file
				 * @param argv[1]: destination file
				 * @returns: Encrypted file
				 * ----------------------------------------
				*/

				inline static auto encrypt_fs(
					JSContext* context,
					JSValueConst this_val,
					int argc,
					JSValueConst* argv
				) -> JSValue
				{
					M_JS_PROXY_WRAPPER(context, {
						try_assert(argc == 4, fmt::format("argument expected {} but received {}", 4, argc));
						auto source = JS::Converter::get_c_string(context, argv[0]);
						auto destination = JS::Converter::get_c_string(context, argv[1]);
						auto key = JS::Converter::get_c_string(context, argv[2]);
						auto iv = JS::Converter::get_c_string(context, argv[3]);
						Sen::Kernel::Support::PopCap::RTON::Encode::encrypt_fs(source.get(), destination.get(), key.get(), iv.get());
						return JS::Converter::get_undefined();
						}, "encrypt_fs"_sv);
				}
			}

			/**
			 * JavaScript RSB Support
			*/

			namespace RSB {
				/**
				 * ----------------------------------------
				 * JavaScript RSB Unpack File
				 * @param argv[0]: source file
				 * @param argv[1]: destination file
				 * @returns: Unpacked file
				 * ----------------------------------------
				*/

				inline static auto unpack_fs(
					JSContext *context, 
					JSValueConst this_val, 
					int argc, 
					JSValueConst *argv
				) -> JSValue
				{
					M_JS_PROXY_WRAPPER(context, {
					try_assert(argc == 2, fmt::format("{} 2, {}: {}", Kernel::Language::get("kernel.argument_expected"), Kernel::Language::get("kernel.argument_received"), argc));
						auto source = JS::Converter::get_c_string(context, argv[0]);
						auto destination = JS::Converter::get_c_string(context, argv[1]);
						Kernel::Support::PopCap::RSB::Unpack::unpack_fs(source.get(), destination.get());
						return JS::Converter::get_undefined();
					}, "unpack_fs"_sv);
				}

				/**
				 * ----------------------------------------
				 * JavaScript RSB Unpack File
				 * @param argv[0]: source file
				 * @param argv[1]: destination file
				 * @returns: Unpacked file
				 * ----------------------------------------
				*/

				inline static auto unpack_for_modding_fs(
					JSContext* context,
					JSValueConst this_val,
					int argc,
					JSValueConst* argv
				) -> JSValue
				{
					M_JS_PROXY_WRAPPER(context, {
						try_assert(argc == 2, fmt::format("{} 2, {}: {}", Kernel::Language::get("kernel.argument_expected"), Kernel::Language::get("kernel.argument_received"), argc));
						auto source = JS::Converter::get_c_string(context, argv[0]);
						auto destination = JS::Converter::get_c_string(context, argv[1]);
						auto manifest = Sen::Kernel::Support::PopCap::RSB::Manifest <unsigned int>{};
						auto unpack = Kernel::Support::PopCap::RSB::Unpack{ source.get() };
						unpack.process(destination.get(), manifest);
						return JSON::json_to_js_value(context, manifest);
					}, "unpack_fs"_sv);
				}

				/**
				 * ----------------------------------------
				 * JavaScript RSB Pack File
				 * @param argv[0]: source file
				 * @param argv[1]: destination file
				 * @returns: Packed file
				 * ----------------------------------------
				*/

				inline static auto pack_fs(
					JSContext *context, 
					JSValueConst this_val, 
					int argc, 
					JSValueConst *argv
				) -> JSValue
				{
					M_JS_PROXY_WRAPPER(context, {
				try_assert(argc == 2, fmt::format("{} 2, {}: {}", Kernel::Language::get("kernel.argument_expected"), Kernel::Language::get("kernel.argument_received"), argc));
						auto source = JS::Converter::get_string(context, argv[0]);
						auto destination = JS::Converter::get_string(context, argv[1]);
						// encode method
						return JS::Converter::get_undefined();
					}, "pack_fs"_sv);
				}
			}

			/**
			 * JavaScript RSG Support
			*/

			namespace RSG {
				/**
				 * ----------------------------------------
				 * JavaScript RSG Unpack File
				 * @param argv[0]: source file
				 * @param argv[1]: destination file
				 * @returns: Unpacked file
				 * ----------------------------------------
				*/

				inline static auto unpack_fs(
					JSContext *context, 
					JSValueConst this_val, 
					int argc, 
					JSValueConst *argv
				) -> JSValue
				{
					M_JS_PROXY_WRAPPER(context, {
				try_assert(argc == 2, fmt::format("{} 2, {}: {}", Kernel::Language::get("kernel.argument_expected"), Kernel::Language::get("kernel.argument_received"), argc));
						auto source = JS::Converter::get_c_string(context, argv[0]);
						auto destination = JS::Converter::get_c_string(context, argv[1]);
						Kernel::Support::PopCap::RSG::Unpack<false>::unpack_fs(source.get(), destination.get());
						return JS::Converter::get_undefined();
					}, "unpack_fs"_sv);
				}

				/**
				 * ----------------------------------------
				 * JavaScript RSG Pack File
				 * @param argv[0]: source file
				 * @param argv[1]: destination file
				 * @returns: Packed
				 *  file
				 * ----------------------------------------
				*/

				inline static auto pack_fs(
					JSContext *context, 
					JSValueConst this_val, 
					int argc, 
					JSValueConst *argv
				) -> JSValue
				{
					M_JS_PROXY_WRAPPER(context, {
				try_assert(argc == 2, fmt::format("{} 2, {}: {}", Kernel::Language::get("kernel.argument_expected"), Kernel::Language::get("kernel.argument_received"), argc));
						auto source = JS::Converter::get_string(context, argv[0]);
						auto destination = JS::Converter::get_string(context, argv[1]);
						// encode method
						return JS::Converter::get_undefined();
					}, "pack_fs"_sv);
				}
			}

			/**
			 * JavaScript PAM Supportive
			*/

			namespace Animation {
				/**
				 * ----------------------------------------
				 * JavaScript PAM Decode File
				 * @param argv[0]: source file
				 * @param argv[1]: destination file
				 * @returns: Decoded file
				 * ----------------------------------------
				*/

				inline static auto decode_fs(
					JSContext *context, 
					JSValueConst this_val, 
					int argc, 
					JSValueConst *argv
				) -> JSValue
				{
					M_JS_PROXY_WRAPPER(context, {
				try_assert(argc == 2, fmt::format("{} 2, {}: {}", Kernel::Language::get("kernel.argument_expected"), Kernel::Language::get("kernel.argument_received"), argc));
						auto source = JS::Converter::get_c_string(context, argv[0]);
						auto destination = JS::Converter::get_c_string(context, argv[1]);
						Sen::Kernel::Support::PopCap::Animation::Decode::decode_fs(source.get(), destination.get());
						return JS::Converter::get_undefined();
					}, "decode_fs"_sv);
				}

				/**
				 * ----------------------------------------
				 * JavaScript PAM Encode File
				 * @param argv[0]: source file
				 * @param argv[1]: destination file
				 * @returns: Encoded file
				 * ----------------------------------------
				*/

				inline static auto encode_fs(
					JSContext *context, 
					JSValueConst this_val, 
					int argc, 
					JSValueConst *argv
				) -> JSValue
				{
					M_JS_PROXY_WRAPPER(context, {
				try_assert(argc == 2, fmt::format("{} 2, {}: {}", Kernel::Language::get("kernel.argument_expected"), Kernel::Language::get("kernel.argument_received"), argc));
						auto source = JS::Converter::get_c_string(context, argv[0]);
						auto destination = JS::Converter::get_c_string(context, argv[1]);
						Sen::Kernel::Support::PopCap::Animation::Encode::encode_fs(source.get(), destination.get());
						return JS::Converter::get_undefined();
					}, "encode_fs"_sv);
				}
			}
		}

		/**
		 * JavaScript WWise Support
		*/

		namespace WWise {

			/**
			 * JavaScript SoundBank Support
			*/

			namespace SoundBank {
				/**
				 * ----------------------------------------
				 * JavaScript WWise Soundbank Decode File
				 * @param argv[0]: source file
				 * @param argv[1]: destination file
				 * @returns: Decoded file
				 * ----------------------------------------
				*/

				inline static auto decode_fs(
					JSContext *context, 
					JSValueConst this_val, 
					int argc, 
					JSValueConst *argv
				) -> JSValue
				{
					M_JS_PROXY_WRAPPER(context, {
				try_assert(argc == 2, fmt::format("{} 2, {}: {}", Kernel::Language::get("kernel.argument_expected"), Kernel::Language::get("kernel.argument_received"), argc));
						auto source = JS::Converter::get_c_string(context, argv[0]);
						auto destination = JS::Converter::get_c_string(context, argv[1]);
						Kernel::Support::WWise::SoundBank::Decode::process_fs(source.get(), destination.get());
						return JS::Converter::get_undefined();
					}, "decode_fs"_sv);
				}

				/**
				 * ----------------------------------------
				 * JavaScript WWise Soundbank Encode File
				 * @param argv[0]: source file
				 * @param argv[1]: destination file
				 * @returns: Encoded file
				 * ----------------------------------------
				*/

				inline static auto encode_fs(
					JSContext *context, 
					JSValueConst this_val, 
					int argc, 
					JSValueConst *argv
				) -> JSValue
				{
					M_JS_PROXY_WRAPPER(context, {
				try_assert(argc == 2, fmt::format("{} 2, {}: {}", Kernel::Language::get("kernel.argument_expected"), Kernel::Language::get("kernel.argument_received"), argc));
						auto source = JS::Converter::get_c_string(context, argv[0]);
						auto destination = JS::Converter::get_c_string(context, argv[1]);
						Kernel::Support::WWise::SoundBank::Encode::process_fs(source.get(), destination.get());
						return JS::Converter::get_undefined();
					}, "encode_fs"_sv);
				}
			}
		}
	}

	namespace Miscellaneous {

		/**
		 * QuickJS JSON Value deep clone
		*/

		inline static auto deep_clone(
			JSContext* context,
			JSValueConst value
		) -> JSValue
		{
			switch (JS_VALUE_GET_TAG(value)) {
				case JS_TAG_UNDEFINED: {
					return JS_UNDEFINED;
				}
				case JS_TAG_NULL: {
					return JS_NULL;
				}
				case JS_TAG_OBJECT: {
					if (JS_IsArray(context, value)) {
						auto js_array = JS_NewArray(context);
						auto length = uint32_t{};
						auto c_length = JS_GetPropertyStr(context, value, "length");
						JS_ToUint32(context, &length, c_length);
						JS_FreeValue(context, c_length);
						for (auto i : Range<uint32_t>(length)) {
							auto js_value = JS_GetPropertyUint32(context, value, i);
							JS_SetPropertyUint32(context, js_array, i, deep_clone(context, js_value));
						}
						return js_array;
					}
					else if (JS_IsObject(value)) {
						auto json = JS_NewObject(context);
						auto* tab = static_cast<JSPropertyEnum*>(nullptr);
						auto tab_size = uint32_t{};
						if (JS_GetOwnPropertyNames(context, &tab, &tab_size, value, JS_GPN_STRING_MASK | JS_GPN_ENUM_ONLY) == 0) {
							for (auto i : Range<uint32_t>(tab_size)) {
								auto key = JS_AtomToCString(context, tab[i].atom);
								auto val = JS_GetProperty(context, value, tab[i].atom);
								JS_SetPropertyStr(context, json, key, deep_clone(context, val));
								JS_FreeAtom(context, tab[i].atom);
								JS_FreeValue(context, val);
							}
							js_free(context, tab);
						}
						return json;
					}
					else {
						throw Exception("Unknown type");
					}
				}
				case JS_TAG_STRING: {
					auto c_str = JS_ToCString(context, value);
					auto destination = JS_NewString(context, c_str);
					JS_FreeCString(context, c_str);
					return destination;
				}
				case JS_TAG_BOOL: {
					return JS_NewBool(context, JS_VALUE_GET_BOOL(value) != 0);
				}
				case JS_TAG_BIG_INT: {
					auto val = int64_t{};
					JS_ToBigInt64(context, &val, value);
					return JS_NewBigInt64(context, val);
				}
				case JS_TAG_FLOAT64: {
					return JS_NewFloat64(context, JS::Converter::get_float64(context, value));
				}
				case JS_TAG_INT: {
					return JS_NewFloat64(context, JS_VALUE_GET_INT(value));
				}
				default: {
					return value;
				}
			}
		}

		inline static auto make_copy(
			JSContext* context,
			JSValueConst this_val,
			int argc,
			JSValueConst* argv
		) -> JSValue
		{
			M_JS_PROXY_WRAPPER(context, {
				try_assert(argc == 1, fmt::format("{} 1, {}: {}", Kernel::Language::get("kernel.argument_expected"), Kernel::Language::get("kernel.argument_received"), argc));
				return deep_clone(context, argv[0]);
			}, "make_copy"_sv);
		}

		/*
			UTF8 Support
		*/

		inline static auto cast_ArrayBuffer_to_JS_String(
			JSContext* context,
			JSValueConst this_val,
			int argc,
			JSValueConst* argv
		) -> JSValue 
		{
			M_JS_PROXY_WRAPPER(context, {
				try_assert(argc == 1, fmt::format("{} 1, {}: {}", Kernel::Language::get("kernel.argument_expected"), Kernel::Language::get("kernel.argument_received"), argc));
				auto len = size_t{};
				auto buf = JS_GetArrayBuffer(context, &len, argv[0]);
				if (!buf) {
					throw Exception(fmt::format("{}", Kernel::Language::get("kernel.cast_ArrayBuffer_to_JS_String.failed_to_get_array_buffer")));
				}
				auto str = JS_NewStringLen(context, reinterpret_cast<const char*>(buf), len);
				return str;
			}, "cast_ArrayBuffer_to_JS_String"_sv);
		}

		/*
		UTF16 Support
		*/

		inline static auto cast_ArrayBuffer_to_JS_WideString(
			JSContext* context,
			JSValueConst this_val,
			int argc,
			JSValueConst* argv
		) -> JSValue
		{
			M_JS_PROXY_WRAPPER(context, {
				try_assert(argc == 1, fmt::format("{} 1, {}: {}", Kernel::Language::get("kernel.argument_expected"), Kernel::Language::get("kernel.argument_received"), argc));
				auto len = size_t{};
				auto buf = JS_GetArrayBuffer(context, &len, argv[0]);
				if (!buf) {
					throw Exception(fmt::format("{}", Kernel::Language::get("kernel.cast_ArrayBuffer_to_JS_String.failed_to_get_array_buffer")));
				}
				auto utf16 = std::wstring(reinterpret_cast<wchar_t*>(buf), len / sizeof(wchar_t));
				auto converter = std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>>{};
				auto utf8 = converter.to_bytes(utf16);
				auto str = JS_NewStringLen(context, utf8.c_str(), utf8.size());
				return str;
			}, "cast_ArrayBuffer_to_JS_WideString"_sv);
		}


	}


	namespace XML {

		inline static auto xml2json(const tinyxml2::XMLNode* node) -> nlohmann::ordered_json {
			auto j = nlohmann::ordered_json{};
			auto element = node->ToElement();
			if (element) {
				for (auto attr = element->FirstAttribute(); attr; attr = attr->Next()) {
					j["@attributes"][attr->Name()] = attr->Value();
				}
			}
			if (node->ToText()) {
				j["@text"] = {
					{"value", node->Value() },
					{"is_cdata", node->ToText()->CData() },
				};
			}
			else {
				for (auto child = node->FirstChild(); child; child = child->NextSibling()) {
					auto child_json = xml2json(child);
					if (j.contains(child->Value())) {
						if (j[child->Value()].is_array()) {
							j[child->Value()].push_back(child_json);
						}
						else {
							j[child->Value()] = { j[child->Value()], child_json };
						}
					}
					else {
						if (child_json.find("@text") != child_json.end()) {
							if (std::strcmp(child_json["@text"]["value"].get<std::string>().c_str(),
								child->Value()) == 0) {
								j = child_json;
							}
							else {
								j[child->Value()] = child_json;
							}
						}
						else {
							j[child->Value()] = child_json;
						}
					}
				}
			}
			return j;
		}

		inline static auto json2xml(
			const nlohmann::ordered_json& j, 
			tinyxml2::XMLNode* node, 
			tinyxml2::XMLDocument& doc
		) -> void
		{
			if (j.is_object()) {
				for (auto& [key, value] : j.items()) {
					if (key == "@attributes") {
						for (auto& [attribute_key, attribute_value] : value.items()) {
							dynamic_cast<tinyxml2::XMLElement*>(node)->SetAttribute(attribute_key.c_str(), attribute_value.get<std::string>().c_str());
						}
					}
					else if (key == "@text") {
						if(!value["is_cdata"].is_null() and value["is_cdata"]) {
							auto cdata = doc.NewText(value["value"].get<std::string>().c_str());
							cdata->SetCData(true);
							node->InsertEndChild(cdata);
						}
						else {
							dynamic_cast<tinyxml2::XMLElement*>(node)->SetText(value["value"].get<std::string>().c_str());
						}
					}
					else {
						if (value.is_object()) {
							auto child = doc.NewElement(key.c_str());
							node->InsertEndChild(child);
							json2xml(value, child, doc);
						}
						else if (value.is_array()) {
							for (auto& element : value) {
								auto child = doc.NewElement(key.c_str());
								node->InsertEndChild(child);
								json2xml(element, child, doc);
							}
						}
						else {
							auto child = doc.NewElement(node->Value());
							node->Parent()->InsertEndChild(child);
						}
					}
				}
			}
			else if (j.is_array()) {
				for (auto& element : j) {
					auto child = doc.NewElement(node->Value());
					node->Parent()->InsertEndChild(child);
					json2xml(element, child, doc);
				}
			}
			else if(j.is_string()) {
				node->InsertEndChild(doc.NewText(j.get<std::string>().c_str()));
			}
			else {
				auto child = doc.NewElement(node->Value());
				node->Parent()->InsertEndChild(child);
			}
			return;
		}

		inline static auto convert(
			const nlohmann::ordered_json& j, 
			tinyxml2::XMLDocument& doc
		) -> void
		{
			auto & root_name = j.begin().key();
			auto root = doc.NewElement(root_name.c_str());
			doc.InsertEndChild(root);
			json2xml(j.begin().value(), root, doc);
			return;
		}



		inline static auto deserialize(
			JSContext* context,
			JSValueConst this_val,
			int argc,
			JSValueConst* argv
		) -> JSValue
		{
			M_JS_PROXY_WRAPPER(context, {
				try_assert(argc == 1, fmt::format("{} 1, {}: {}", Kernel::Language::get("kernel.argument_expected"), Kernel::Language::get("kernel.argument_received"), argc));
				auto doc = tinyxml2::XMLDocument{};
				auto source = JS::Converter::get_string(context, argv[0]);
				auto eResult = doc.Parse(source.c_str(), source.size());
				if (eResult != tinyxml2::XML_SUCCESS) {
					throw Exception(fmt::format("XML cannot be parsed, data", source));
				}
				auto j = nlohmann::ordered_json{};
				j[doc.RootElement()->Value()] = xml2json(doc.RootElement());
				return JSON::json_to_js_value(context, j);
			}, "deserialize"_sv);
		}

		inline static auto deserialize_fs(
			JSContext* context,
			JSValueConst this_val,
			int argc,
			JSValueConst* argv
		) -> JSValue
		{
			M_JS_PROXY_WRAPPER(context, {
				try_assert(argc == 1, fmt::format("{} 1, {}: {}", Kernel::Language::get("kernel.argument_expected"), Kernel::Language::get("kernel.argument_received"), argc));
				auto doc = tinyxml2::XMLDocument{};
				auto source = JS::Converter::get_c_string(context, argv[0]);
				auto eResult = doc.LoadFile(source.get());
				if (eResult != tinyxml2::XML_SUCCESS) {
					throw Exception(fmt::format("XML cannot be loaded, data: {}", source.get()));
				}
				auto j = nlohmann::ordered_json{};
				j[doc.RootElement()->Value()] = xml2json(doc.RootElement());
				return JSON::json_to_js_value(context, j);
			}, "deserialize_fs"_sv);
		}

		inline static auto serialize(
			JSContext* context,
			JSValueConst this_val,
			int argc,
			JSValueConst* argv
		) -> JSValue
		{
			M_JS_PROXY_WRAPPER(context, {
				try_assert(argc == 1, fmt::format("{} 1, {}: {}", Kernel::Language::get("kernel.argument_expected"), Kernel::Language::get("kernel.argument_received"), argc));
				auto doc = tinyxml2::XMLDocument{};
				auto j = JSON::js_object_to_json(context, argv[0]);
				convert(j, doc);
				auto printer = tinyxml2::XMLPrinter{};
				doc.Print(&printer);
				return JS::Converter::to_string(context, printer.CStr());
			}, "serialize"_sv);
		}

		inline static auto serialize_fs(
			JSContext* context,
			JSValueConst this_val,
			int argc,
			JSValueConst* argv
		) -> JSValue
		{
			M_JS_PROXY_WRAPPER(context, {
				try_assert(argc == 2, fmt::format("{} 2, {}: {}", Kernel::Language::get("kernel.argument_expected"), Kernel::Language::get("kernel.argument_received"), argc));
				auto doc = tinyxml2::XMLDocument{};
				auto source = JSON::js_object_to_json(context, argv[0]);
				convert(source, doc);
				auto printer = tinyxml2::XMLPrinter{};
				doc.Print(&printer);
				auto destination = JS::Converter::get_c_string(context, argv[1]);
				Kernel::FileSystem::write_file(destination.get(), printer.CStr());
				return JS::Converter::get_undefined();
			}, "serialize_fs"_sv);
		}

	}

	namespace Class {

		#define JS_CPPGETSET_MAGIC_DEF(c_name, fgetter, fsetter, _magic) { \
				.name = c_name, .prop_flags = JS_PROP_CONFIGURABLE, .def_type = JS_DEF_CGETSET_MAGIC, .magic = _magic, .u = { .getset = { .get = { .getter_magic = fgetter }, .set = { .setter_magic = fsetter } } } }

		#define JS_CPPFUNC_DEF(c_name, length, func1) { .name = c_name, .prop_flags = JS_PROP_WRITABLE | JS_PROP_CONFIGURABLE, .def_type = JS_DEF_CFUNC, .magic = 0, .u = { .func = { length, JS_CFUNC_generic, { .generic = func1 } } } }

		#define JS_INSTANCE_OF_OBJ(ctx, obj, parent, name) \
					auto obj = JS_GetPropertyStr(ctx, parent, name.data()); \
					if (JS_IsUndefined(obj)) { \
						obj = JS_NewObject(ctx); \
						JS_SetPropertyStr(ctx, parent, name.data(), obj); \
					}

		namespace DataStreamView {

			inline static auto constexpr BooleanConstraint = true;

			template <auto T> requires BooleanConstraint
			using Data = Definition::Buffer::Stream<T>;

			template <auto use_big_endian> requires BooleanConstraint
			struct ClassID {
				inline static JSClassID value = 0;
			};


			template <auto T> requires BooleanConstraint
			inline static auto finalizer(
				JSRuntime* rt, 
				JSValue val
			) -> void
			{
				auto s = (Data<T>*)JS_GetOpaque(val, ClassID<T>::value);
				if (s) {
					delete s;
				}
				return;
			}

			template <auto T> requires BooleanConstraint
			inline static auto constructor(
				JSContext* ctx, 
				JSValueConst new_target, 
				int argc, 
				JSValueConst* argv
			) -> JSValue
			{
				auto s = static_cast<Data<T>*>(nullptr);
				auto obj = JS_UNDEFINED;
				auto proto = JSValue{};
				if (argc == 1) {
					auto path = JS::Converter::get_c_string(ctx, argv[0]);
					s = new Data<T>{ path.get()};
				}
				else if(argc == 0) {
					s = new Data<T>{};
				}
				else {
					return JS_EXCEPTION;
				}
				proto = JS_GetPropertyStr(ctx, new_target, "prototype");
				if (JS_IsException(proto))
					goto fail;
				obj = JS_NewObjectProtoClass(ctx, proto, ClassID<T>::value);
				JS_FreeValue(ctx, proto);
				if (JS_IsException(obj))
					goto fail;
				JS_SetOpaque(obj, s);
				return obj;
			fail:
				js_free(ctx, s);
				JS_FreeValue(ctx, obj);
				return JS_EXCEPTION;
			}

			template <auto T> requires BooleanConstraint
			inline static auto this_class = JSClassDef {
				.class_name = "DataStreamView",
				.finalizer = finalizer<T>,
			};

			template <auto T> requires BooleanConstraint
			inline static auto getter(
				JSContext* ctx, 
				JSValueConst this_val, 
				int magic
			) -> JSValue
			{
				auto s = (Data<T>*)JS_GetOpaque2(ctx, this_val, ClassID<T>::value);
				if (!s) {
					return JS_EXCEPTION;
				}
				if (magic == 0) {
					return JS_NewBigInt64(ctx, s->read_pos);
				}
				else {
					return JS_NewBigInt64(ctx, s->write_pos);
				}
			}

			template <auto T> requires BooleanConstraint
			inline static auto setter(
				JSContext* ctx,
				JSValueConst this_val,
				JSValueConst val,
				int magic
			) -> JSValue
			{
				auto s = (Data<T>*)JS_GetOpaque2(ctx, this_val, ClassID<T>::value);
				auto v = std::int64_t{};
				if (!s){
					return JS_EXCEPTION;
				}
				if (JS_ToBigInt64(ctx, &v, val))
					return JS_EXCEPTION;
				if (magic == 0) {
					s->read_pos = v;
				}
				else {
					s->write_pos = v;
				}
				return JS_UNDEFINED;
			}

			template <auto T> requires BooleanConstraint
			inline static auto size(
				JSContext* ctx,
				JSValueConst this_val,
				int argc,
				JSValueConst* argv
			) -> JSValue
			{
				M_JS_PROXY_WRAPPER(ctx,{
					try_assert(argc == 0, fmt::format("argument expected 0, received: {}", argc));
					auto s = (Data<T>*)JS_GetOpaque2(ctx, this_val, ClassID<T>::value);
					if (!s) {
						return JS_EXCEPTION;
					}
					return JS::Converter::to_bigint<uint64_t>(ctx, s->size());
				}, "size"_sv);
			}

			template <auto T> requires BooleanConstraint
			inline static auto capacity(
				JSContext* ctx,
				JSValueConst this_val,
				int argc,
				JSValueConst* argv
			) -> JSValue
			{
				M_JS_PROXY_WRAPPER(ctx, {
					try_assert(argc == 0, fmt::format("argument expected 0, received: {}", argc));
					auto s = (Data<T>*)JS_GetOpaque2(ctx, this_val, ClassID<T>::value);
					if (!s) {
						return JS_EXCEPTION;
					}
					return JS::Converter::to_bigint<uint64_t>(ctx, s->size());
				}, "capacity"_sv);
			}

			template <auto T> requires BooleanConstraint
			inline static auto fromString(
				JSContext* ctx,
				JSValueConst this_val,
				int argc,
				JSValueConst* argv
			) -> JSValue
			{
				M_JS_PROXY_WRAPPER(ctx, {
				try_assert(argc == 1, fmt::format("{} 1, {}: {}", Kernel::Language::get("kernel.argument_expected"), Kernel::Language::get("kernel.argument_received"), argc));
					auto s = (Data<T>*)JS_GetOpaque2(ctx, this_val, ClassID<T>::value);
					if (!s) {
						return JS_EXCEPTION;
					}
					s->fromString(JS::Converter::get_string(ctx, argv[0]));
					return JS_UNDEFINED;
				}, "fromString"_sv);
			}

			template <auto T> requires BooleanConstraint
			inline static auto reserve(
				JSContext* ctx,
				JSValueConst this_val,
				int argc,
				JSValueConst* argv
			) -> JSValue
			{
				M_JS_PROXY_WRAPPER(ctx, {
				try_assert(argc == 1, fmt::format("{} 1, {}: {}", Kernel::Language::get("kernel.argument_expected"), Kernel::Language::get("kernel.argument_received"), argc));
					auto s = (Data<T>*)JS_GetOpaque2(ctx, this_val, ClassID<T>::value);
					if (!s) {
						return JS_EXCEPTION;
					}
					s->reserve(static_cast<std::uint64_t>(JS::Converter::get_bigint64(ctx, argv[0])));
					return JS_UNDEFINED;
				}, "reserve"_sv);
			}

			#pragma region convert

			inline static auto to_arraybuffer(
				JSContext* ctx, 
				const std::vector<uint8_t>& vec
			) -> JSValue
			{
				auto array_buffer = JS_NewArrayBufferCopy(ctx, vec.data(), vec.size());
				JS_FreeValue(ctx, array_buffer);
				return array_buffer;
			}

			inline static auto to_uint8array(
				JSContext* ctx, 
				const std::vector<uint8_t>& vec
			) -> JSValue
			{
				auto array_buffer = JS_NewArrayBufferCopy(ctx, vec.data(), vec.size());
				auto global_obj = JS_GetGlobalObject(ctx);
				auto uint8array_ctor = JS_GetPropertyStr(ctx, global_obj, "Uint8Array");
				JSValue args[] = { array_buffer };
				auto uint8array = JS_CallConstructor(ctx, uint8array_ctor, 1, args);
				JS_FreeValue(ctx, global_obj);
				JS_FreeValue(ctx, uint8array_ctor);
				JS_FreeValue(ctx, array_buffer);
				return uint8array;
			}

			#pragma endregion

			template <auto T> requires BooleanConstraint
			inline static auto toUint8Array(
				JSContext* ctx,
				JSValueConst this_val,
				int argc,
				JSValueConst* argv
			) -> JSValue
			{
				M_JS_PROXY_WRAPPER(ctx, {
					try_assert(argc == 0, fmt::format("argument expected 0, received: {}", argc));
					auto s = (Data<T>*)JS_GetOpaque2(ctx, this_val, ClassID<T>::value);
					if (!s) {
						return JS_EXCEPTION;
					}
					return to_uint8array(ctx, s->toBytes());
				}, "toUint8Array"_sv);
			}

			template <auto T> requires BooleanConstraint
			inline static auto toArrayBuffer(
				JSContext* ctx,
				JSValueConst this_val,
				int argc,
				JSValueConst* argv
			) -> JSValue
			{
				M_JS_PROXY_WRAPPER(ctx, {
					try_assert(argc == 0, fmt::format("argument expected 0, received: {}", argc));
					auto s = (Data<T>*)JS_GetOpaque2(ctx, this_val, ClassID<T>::value);
					if (!s) {
						return JS_EXCEPTION;
					}
					return to_arraybuffer(ctx, s->toBytes());
				}, "toArrayBuffer"_sv);
			}

			template <auto T> requires BooleanConstraint
			inline static auto getArrayBuffer(
				JSContext* ctx,
				JSValueConst this_val,
				int argc,
				JSValueConst* argv
			) -> JSValue
			{
				M_JS_PROXY_WRAPPER(ctx, {
				try_assert(argc == 2, fmt::format("{} 2, {}: {}", Kernel::Language::get("kernel.argument_expected"), Kernel::Language::get("kernel.argument_received"), argc));
					auto s = (Data<T>*)JS_GetOpaque2(ctx, this_val, ClassID<T>::value);
					if (!s) {
						return JS_EXCEPTION;
					}
					auto from = JS::Converter::get_bigint64(ctx, argv[0]);
					auto to = JS::Converter::get_bigint64(ctx, argv[1]);
					return to_arraybuffer(ctx, s->get(from, to));
				}, "getArrayBuffer"_sv);
			}

			template <auto T> requires BooleanConstraint
			inline static auto getUint8Array(
				JSContext* ctx,
				JSValueConst this_val,
				int argc,
				JSValueConst* argv
			) -> JSValue
			{
				M_JS_PROXY_WRAPPER(ctx, {
				try_assert(argc == 2, fmt::format("{} 2, {}: {}", Kernel::Language::get("kernel.argument_expected"), Kernel::Language::get("kernel.argument_received"), argc));
					auto s = (Data<T>*)JS_GetOpaque2(ctx, this_val, ClassID<T>::value);
					if (!s) {
						return JS_EXCEPTION;
					}
					auto from = JS::Converter::get_bigint64(ctx, argv[0]);
					auto to = JS::Converter::get_bigint64(ctx, argv[1]);
					return to_uint8array(ctx, s->get(from, to));
				}, "getUint8Array"_sv);
			}

			template <auto T> requires BooleanConstraint
			inline static auto toString(
				JSContext* ctx,
				JSValueConst this_val,
				int argc,
				JSValueConst* argv
			) -> JSValue
			{
				M_JS_PROXY_WRAPPER(ctx, {
					try_assert(argc == 0, fmt::format("argument expected 0, received: {}", argc));
					auto s = (Data<T>*)JS_GetOpaque2(ctx, this_val, ClassID<T>::value);
					if (!s) {
						return JS_EXCEPTION;
					}
					return JS::Converter::to_string(ctx, s->toString());
				}, "toString"_sv);
			}

			template <auto T> requires BooleanConstraint
			inline static auto out_file(
				JSContext* ctx,
				JSValueConst this_val,
				int argc,
				JSValueConst* argv
			) -> JSValue
			{
				M_JS_PROXY_WRAPPER(ctx, {
				try_assert(argc == 1, fmt::format("{} 1, {}: {}", Kernel::Language::get("kernel.argument_expected"), Kernel::Language::get("kernel.argument_received"), argc));
					auto s = (Data<T>*)JS_GetOpaque2(ctx, this_val, ClassID<T>::value);
					if (!s) {
						return JS_EXCEPTION;
					}
					s->out_file(JS::Converter::get_c_string(ctx, argv[0]).get());
					return JS_UNDEFINED;
				}, "out_file"_sv);
			}

			template <auto T> requires BooleanConstraint
			inline static auto writeUint8(
				JSContext* ctx,
				JSValueConst this_val,
				int argc,
				JSValueConst* argv
			) -> JSValue
			{
				M_JS_PROXY_WRAPPER(ctx, {
					try_assert(argc == 2 || argc == 1, fmt::format("argument expected 2 or 1, received: {}", argc));
					auto s = (Data<T>*)JS_GetOpaque2(ctx, this_val, ClassID<T>::value);
					if (!s) {
						return JS_EXCEPTION;
					}
					if (argc == 1) {
						s->writeUint8(
							static_cast<uint8_t>(JS::Converter::get_bigint64(ctx, argv[0]))
						);
					}
					else {
						s->writeUint8(
							static_cast<uint8_t>(JS::Converter::get_bigint64(ctx, argv[0])),
							static_cast<uint64_t>(JS::Converter::get_bigint64(ctx, argv[1]))
						);
					}
					return JS_UNDEFINED;
				}, "writeUint8"_sv);
			}

			template <auto T> requires BooleanConstraint
			inline static auto writeUint16(
				JSContext* ctx,
				JSValueConst this_val,
				int argc,
				JSValueConst* argv
			) -> JSValue
			{
				M_JS_PROXY_WRAPPER(ctx, {
					try_assert(argc == 2 || argc == 1, fmt::format("argument expected 2 or 1, received: {}", argc));
					auto s = (Data<T>*)JS_GetOpaque2(ctx, this_val, ClassID<T>::value);
					if (!s) {
						return JS_EXCEPTION;
					}
					if (argc == 1) {
						s->writeUint16(
							static_cast<uint16_t>(JS::Converter::get_bigint64(ctx, argv[0]))
						);
					}
					else {
						s->writeUint16(
							static_cast<uint16_t>(JS::Converter::get_bigint64(ctx, argv[0])),
							static_cast<uint64_t>(JS::Converter::get_bigint64(ctx, argv[1]))
						);
					}
					return JS_UNDEFINED;
				}, "writeUint16"_sv);
			}

			template <auto T> requires BooleanConstraint
			inline static auto writeUint24(
				JSContext* ctx,
				JSValueConst this_val,
				int argc,
				JSValueConst* argv
			) -> JSValue
			{
				M_JS_PROXY_WRAPPER(ctx, {
					try_assert(argc == 2 || argc == 1, fmt::format("argument expected 2 or 1, received: {}", argc));
					auto s = (Data<T>*)JS_GetOpaque2(ctx, this_val, ClassID<T>::value);
					if (!s) {
						return JS_EXCEPTION;
					}
					if (argc == 1) {
						s->writeUint24(
							static_cast<uint32_t>(JS::Converter::get_bigint64(ctx, argv[0]))
						);
					}
					else {
						s->writeUint24(
							static_cast<uint32_t>(JS::Converter::get_bigint64(ctx, argv[0])),
							static_cast<uint64_t>(JS::Converter::get_bigint64(ctx, argv[1]))
						);
					}
					return JS_UNDEFINED;
				}, "writeUint24"_sv);
			}

			template <auto T> requires BooleanConstraint
			inline static auto writeUint32(
				JSContext* ctx,
				JSValueConst this_val,
				int argc,
				JSValueConst* argv
			) -> JSValue
			{
				M_JS_PROXY_WRAPPER(ctx, {
					try_assert(argc == 2 || argc == 1, fmt::format("argument expected 2 or 1, received: {}", argc));
					auto s = (Data<T>*)JS_GetOpaque2(ctx, this_val, ClassID<T>::value);
					if (!s) {
						return JS_EXCEPTION;
					}
					if (argc == 1) {
						s->writeUint32(
							static_cast<uint32_t>(JS::Converter::get_bigint64(ctx, argv[0]))
						);
					}
					else {
						s->writeUint32(
							static_cast<uint32_t>(JS::Converter::get_bigint64(ctx, argv[0])),
							static_cast<uint64_t>(JS::Converter::get_bigint64(ctx, argv[1]))
						);
					}
					return JS_UNDEFINED;
				}, "writeUint32"_sv);
			}

			template <auto T> requires BooleanConstraint
			inline static auto writeUint64(
				JSContext* ctx,
				JSValueConst this_val,
				int argc,
				JSValueConst* argv
			) -> JSValue
			{
				M_JS_PROXY_WRAPPER(ctx, {
					try_assert(argc == 2 || argc == 1, fmt::format("argument expected 2 or 1, received: {}", argc));
					auto s = (Data<T>*)JS_GetOpaque2(ctx, this_val, ClassID<T>::value);
					if (!s) {
						return JS_EXCEPTION;
					}
					if (argc == 1) {
						s->writeUint64(
							static_cast<uint64_t>(JS::Converter::get_bigint64(ctx, argv[0]))
						);
					}
					else {
						s->writeUint64(
							static_cast<uint64_t>(JS::Converter::get_bigint64(ctx, argv[0])),
							static_cast<uint64_t>(JS::Converter::get_bigint64(ctx, argv[1]))
						);
					}
					return JS_UNDEFINED;
				}, "writeUint64"_sv);
			}

			template <auto T> requires BooleanConstraint
			inline static auto writeInt8(
				JSContext* ctx,
				JSValueConst this_val,
				int argc,
				JSValueConst* argv
			) -> JSValue
			{
				M_JS_PROXY_WRAPPER(ctx, {
					try_assert(argc == 2 || argc == 1, fmt::format("argument expected 2 or 1, received: {}", argc));
					auto s = (Data<T>*)JS_GetOpaque2(ctx, this_val, ClassID<T>::value);
					if (!s) {
						return JS_EXCEPTION;
					}
					if (argc == 1) {
						s->writeInt8(
							static_cast<int8_t>(JS::Converter::get_bigint64(ctx, argv[0]))
						);
					}
					else {
						s->writeInt8(
							static_cast<int8_t>(JS::Converter::get_bigint64(ctx, argv[0])),
							static_cast<uint64_t>(JS::Converter::get_bigint64(ctx, argv[1]))
						);
					}
					return JS_UNDEFINED;
				}, "writeInt8"_sv);
			}

			template <auto T> requires BooleanConstraint
			inline static auto writeInt16(
				JSContext* ctx,
				JSValueConst this_val,
				int argc,
				JSValueConst* argv
			) -> JSValue
			{
				M_JS_PROXY_WRAPPER(ctx, {
					try_assert(argc == 2 || argc == 1, fmt::format("argument expected 2 or 1, received: {}", argc));
					auto s = (Data<T>*)JS_GetOpaque2(ctx, this_val, ClassID<T>::value);
					if (!s) {
						return JS_EXCEPTION;
					}
					if (argc == 1) {
						s->writeInt16(
							static_cast<int16_t>(JS::Converter::get_bigint64(ctx, argv[0]))
						);
					}
					else {
						s->writeInt16(
							static_cast<int16_t>(JS::Converter::get_bigint64(ctx, argv[0])),
							static_cast<uint64_t>(JS::Converter::get_bigint64(ctx, argv[1]))
						);
					}
					return JS_UNDEFINED;
				}, "writeInt16"_sv);
			}

			template <auto T> requires BooleanConstraint
			inline static auto writeInt24(
				JSContext* ctx,
				JSValueConst this_val,
				int argc,
				JSValueConst* argv
			) -> JSValue
			{
				M_JS_PROXY_WRAPPER(ctx, {
					try_assert(argc == 2 || argc == 1, fmt::format("argument expected 2 or 1, received: {}", argc));
					auto s = (Data<T>*)JS_GetOpaque2(ctx, this_val, ClassID<T>::value);
					if (!s) {
						return JS_EXCEPTION;
					}
					if (argc == 1) {
						s->writeInt24(
							static_cast<int32_t>(JS::Converter::get_bigint64(ctx, argv[0]))
						);
					}
					else {
						s->writeInt24(
							static_cast<int32_t>(JS::Converter::get_bigint64(ctx, argv[0])),
							static_cast<uint64_t>(JS::Converter::get_bigint64(ctx, argv[1]))
						);
					}
					return JS_UNDEFINED;
				}, "writeInt24"_sv);
			}

			template <auto T> requires BooleanConstraint
			inline static auto writeInt32(
				JSContext* ctx,
				JSValueConst this_val,
				int argc,
				JSValueConst* argv
			) -> JSValue
			{
				M_JS_PROXY_WRAPPER(ctx, {
					try_assert(argc == 2 || argc == 1, fmt::format("argument expected 2 or 1, received: {}", argc));
					auto s = (Data<T>*)JS_GetOpaque2(ctx, this_val, ClassID<T>::value);
					if (!s) {
						return JS_EXCEPTION;
					}
					if (argc == 1) {
						s->writeInt32(
							static_cast<int32_t>(JS::Converter::get_bigint64(ctx, argv[0]))
						);
					}
					else {
						s->writeInt32(
							static_cast<int32_t>(JS::Converter::get_bigint64(ctx, argv[0])),
							static_cast<uint64_t>(JS::Converter::get_bigint64(ctx, argv[1]))
						);
					}
					return JS_UNDEFINED;
				}, "writeInt32"_sv);
			}

			template <auto T> requires BooleanConstraint
			inline static auto writeInt64(
				JSContext* ctx,
				JSValueConst this_val,
				int argc,
				JSValueConst* argv
			) -> JSValue
			{
				M_JS_PROXY_WRAPPER(ctx, {
					try_assert(argc == 2 || argc == 1, fmt::format("argument expected 2 or 1, received: {}", argc));
					auto s = (Data<T>*)JS_GetOpaque2(ctx, this_val, ClassID<T>::value);
					if (!s) {
						return JS_EXCEPTION;
					}
					if (argc == 1) {
						s->writeInt64(
							static_cast<int64_t>(JS::Converter::get_bigint64(ctx, argv[0]))
						);
					}
					else {
						s->writeInt64(
							static_cast<int64_t>(JS::Converter::get_bigint64(ctx, argv[0])),
							static_cast<uint64_t>(JS::Converter::get_bigint64(ctx, argv[1]))
						);
					}
					return JS_UNDEFINED;
				}, "writeInt64"_sv);
			}

			#pragma region convert

			inline static auto from_arraybuffer(
				JSContext* ctx, 
				JSValue array_buffer
			) -> std::vector<uint8_t>
			{
				auto byte_len = size_t{};
				auto data = JS_GetArrayBuffer(ctx, &byte_len, array_buffer);
				return std::vector<uint8_t>(data, data + byte_len);
			}

			inline static auto from_uint8array(
				JSContext* ctx, 
				JSValue uint8array
			) -> std::vector<uint8_t>
			{
				auto array_buffer = JS_GetPropertyStr(ctx, uint8array, "buffer");
				auto vec = from_arraybuffer(ctx, array_buffer);
				JS_FreeValue(ctx, array_buffer);
				return vec;
			}

			#pragma endregion

			template <auto T> requires BooleanConstraint
			inline static auto writeArrayBuffer(
				JSContext* ctx,
				JSValueConst this_val,
				int argc,
				JSValueConst* argv
			) -> JSValue
			{
				M_JS_PROXY_WRAPPER(ctx, {
					try_assert(argc == 1 || argc == 2, fmt::format("argument expected 1 or 2, received: {}", argc));
					auto s = (Data<T>*)JS_GetOpaque2(ctx, this_val, ClassID<T>::value);
					if (!s) {
						return JS_EXCEPTION;
					}
					s->writeBytes(from_arraybuffer(ctx, argv[0]), static_cast<std::uint64_t>(JS::Converter::get_bigint64(ctx, argv[1])));
					return JS_UNDEFINED;
				}, "writeArrayBuffer"_sv);
			}

			template <auto T> requires BooleanConstraint
			inline static auto writeUint8Array(
				JSContext* ctx,
				JSValueConst this_val,
				int argc,
				JSValueConst* argv
			) -> JSValue
			{
				M_JS_PROXY_WRAPPER(ctx, {
					try_assert(argc == 1 || argc == 2, fmt::format("argument expected 1 or 2, received: {}", argc));
					auto s = (Data<T>*)JS_GetOpaque2(ctx, this_val, ClassID<T>::value);
					if (!s) {
						return JS_EXCEPTION;
					}
					if (argc == 1) {
						s->writeBytes(from_uint8array(ctx, argv[0]));
					}
					else {
						s->writeBytes(from_uint8array(ctx, argv[0]), static_cast<std::uint64_t>(JS::Converter::get_bigint64(ctx, argv[1])));
					}
					return JS_UNDEFINED;
				}, "writeUint8Array"_sv);
			}

			template <auto T> requires BooleanConstraint
			inline static auto writeFloat(
				JSContext* ctx,
				JSValueConst this_val,
				int argc,
				JSValueConst* argv
			) -> JSValue
			{
				M_JS_PROXY_WRAPPER(ctx, {
					try_assert(argc == 2 || argc == 1, fmt::format("argument expected 2 or 1, received: {}", argc));
					auto s = (Data<T>*)JS_GetOpaque2(ctx, this_val, ClassID<T>::value);
					if (!s) {
						return JS_EXCEPTION;
					}
					if (argc == 1) {
						s->writeFloat(
							static_cast<float>(JS::Converter::get_float32(ctx, argv[0]))
						);
					}
					else {
						s->writeFloat(
							static_cast<float>(JS::Converter::get_float32(ctx, argv[0])),
							static_cast<uint64_t>(JS::Converter::get_bigint64(ctx, argv[1]))
						);
					}
					return JS_UNDEFINED;
				}, "writeFloat"_sv);
			}

			template <auto T> requires BooleanConstraint
			inline static auto writeDouble(
				JSContext* ctx,
				JSValueConst this_val,
				int argc,
				JSValueConst* argv
			) -> JSValue
			{
				M_JS_PROXY_WRAPPER(ctx, {
					try_assert(argc == 2 || argc == 1, fmt::format("argument expected 2 or 1, received: {}", argc));
					auto s = (Data<T>*)JS_GetOpaque2(ctx, this_val, ClassID<T>::value);
					if (!s) {
						return JS_EXCEPTION;
					}
					if (argc == 1) {
						s->writeDouble(
							static_cast<double>(JS::Converter::get_float64(ctx, argv[0]))
						);
					}
					else {
						s->writeDouble(
							static_cast<double>(JS::Converter::get_float64(ctx, argv[0])),
							static_cast<uint64_t>(JS::Converter::get_bigint64(ctx, argv[1]))
						);
					}
					return JS_UNDEFINED;
				}, "writeDouble"_sv);
			}

			template <auto T> requires BooleanConstraint
			inline static auto writeVarInt32(
				JSContext* ctx,
				JSValueConst this_val,
				int argc,
				JSValueConst* argv
			) -> JSValue
			{
				M_JS_PROXY_WRAPPER(ctx, {
					try_assert(argc == 2 || argc == 1, fmt::format("argument expected 2 or 1, received: {}", argc));
					auto s = (Data<T>*)JS_GetOpaque2(ctx, this_val, ClassID<T>::value);
					if (!s) {
						return JS_EXCEPTION;
					}
					if (argc == 1) {
						s->writeVarInt32(
							static_cast<int32_t>(JS::Converter::get_bigint64(ctx, argv[0]))
						);
					}
					else {
						s->writeVarInt32(
							static_cast<int32_t>(JS::Converter::get_bigint64(ctx, argv[0])),
							static_cast<uint64_t>(JS::Converter::get_bigint64(ctx, argv[1]))
						);
					}
					return JS_UNDEFINED;
				}, "writeVarInt32"_sv);
			}

			template <auto T> requires BooleanConstraint
			inline static auto writeVarInt64(
				JSContext* ctx,
				JSValueConst this_val,
				int argc,
				JSValueConst* argv
			) -> JSValue
			{
				M_JS_PROXY_WRAPPER(ctx, {
					try_assert(argc == 2 || argc == 1, fmt::format("argument expected 2 or 1, received: {}", argc));
					auto s = (Data<T>*)JS_GetOpaque2(ctx, this_val, ClassID<T>::value);
					if (!s) {
						return JS_EXCEPTION;
					}
					if (argc == 1) {
						s->writeVarInt64(
							static_cast<int64_t>(JS::Converter::get_bigint64(ctx, argv[0]))
						);
					}
					else {
						s->writeVarInt64(
							static_cast<int64_t>(JS::Converter::get_bigint64(ctx, argv[0])),
							static_cast<uint64_t>(JS::Converter::get_bigint64(ctx, argv[1]))
						);
					}
					return JS_UNDEFINED;
				}, "writeVarInt64"_sv);
			}

			template <auto T> requires BooleanConstraint
			inline static auto writeZigZag32(
				JSContext* ctx,
				JSValueConst this_val,
				int argc,
				JSValueConst* argv
			) -> JSValue
			{
				M_JS_PROXY_WRAPPER(ctx, {
					try_assert(argc == 2 || argc == 1, fmt::format("argument expected 2 or 1, received: {}", argc));
					auto s = (Data<T>*)JS_GetOpaque2(ctx, this_val, ClassID<T>::value);
					if (!s) {
						return JS_EXCEPTION;
					}
					if (argc == 1) {
						s->writeZigZag32(
							static_cast<int32_t>(JS::Converter::get_bigint64(ctx, argv[0]))
						);
					}
					else {
						s->writeZigZag32(
							static_cast<int32_t>(JS::Converter::get_bigint64(ctx, argv[0])),
							static_cast<uint64_t>(JS::Converter::get_bigint64(ctx, argv[1]))
						);
					}
					return JS_UNDEFINED;
				}, "writeZigZag32"_sv);
			}

			template <auto T> requires BooleanConstraint
			inline static auto writeZigZag64(
				JSContext* ctx,
				JSValueConst this_val,
				int argc,
				JSValueConst* argv
			) -> JSValue
			{
				M_JS_PROXY_WRAPPER(ctx, {
					try_assert(argc == 2 || argc == 1, fmt::format("argument expected 2 or 1, received: {}", argc));
					auto s = (Data<T>*)JS_GetOpaque2(ctx, this_val, ClassID<T>::value);
					if (!s) {
						return JS_EXCEPTION;
					}
					if (argc == 1) {
						s->writeZigZag64(
							static_cast<int64_t>(JS::Converter::get_bigint64(ctx, argv[0]))
						);
					}
					else {
						s->writeZigZag64(
							static_cast<int64_t>(JS::Converter::get_bigint64(ctx, argv[0])),
							static_cast<uint64_t>(JS::Converter::get_bigint64(ctx, argv[1]))
						);
					}
					return JS_UNDEFINED;
				}, "writeZigZag64"_sv);
			}

			template <auto T> requires BooleanConstraint
			inline static auto writeString(
				JSContext* ctx,
				JSValueConst this_val,
				int argc,
				JSValueConst* argv
			) -> JSValue
			{
				M_JS_PROXY_WRAPPER(ctx, {
					try_assert(argc == 2 || argc == 1, fmt::format("argument expected 2 or 1, received: {}", argc));
					auto s = (Data<T>*)JS_GetOpaque2(ctx, this_val, ClassID<T>::value);
					if (!s) {
						return JS_EXCEPTION;
					}
					if (argc == 1) {
						s->writeString(
							JS::Converter::get_string(ctx, argv[0])
						);
					}
					else {
						s->writeString(
							JS::Converter::get_string(ctx, argv[0]),
							static_cast<uint64_t>(JS::Converter::get_bigint64(ctx, argv[1]))
						);
					}
					return JS_UNDEFINED;
				}, "writeString"_sv);
			}

			template <auto T> requires BooleanConstraint
			inline static auto writeStringFourByte(
				JSContext* ctx,
				JSValueConst this_val,
				int argc,
				JSValueConst* argv
			) -> JSValue
			{
				M_JS_PROXY_WRAPPER(ctx, {
					try_assert(argc == 2 || argc == 1, fmt::format("argument expected 2 or 1, received: {}", argc));
					auto s = (Data<T>*)JS_GetOpaque2(ctx, this_val, ClassID<T>::value);
					if (!s) {
						return JS_EXCEPTION;
					}
					if (argc == 1) {
						s->writeStringFourByte(
							JS::Converter::get_string(ctx, argv[0])
						);
					}
					else {
						s->writeStringFourByte(
							JS::Converter::get_string(ctx, argv[0]),
							static_cast<uint64_t>(JS::Converter::get_bigint64(ctx, argv[1]))
						);
					}
					return JS_UNDEFINED;
				}, "writeStringFourByte"_sv);
			}

			template <auto T> requires BooleanConstraint
			inline static auto writeNull(
				JSContext* ctx,
				JSValueConst this_val,
				int argc,
				JSValueConst* argv
			) -> JSValue
			{
				M_JS_PROXY_WRAPPER(ctx, {
					try_assert(argc == 2 || argc == 1, fmt::format("argument expected 2 or 1, received: {}", argc));
					auto s = (Data<T>*)JS_GetOpaque2(ctx, this_val, ClassID<T>::value);
					if (!s) {
						return JS_EXCEPTION;
					}
					if (argc == 1) {
						s->writeNull(
							static_cast<uint64_t>(JS::Converter::get_bigint64(ctx, argv[0]))
						);
					}
					else {
						s->writeNull(
							static_cast<uint64_t>(JS::Converter::get_bigint64(ctx, argv[0])),
							static_cast<uint64_t>(JS::Converter::get_bigint64(ctx, argv[1]))
						);
					}
					return JS_UNDEFINED;
				}, "writeNull"_sv);
			}

			template <auto T> requires BooleanConstraint
			inline static auto writeBoolean(
				JSContext* ctx,
				JSValueConst this_val,
				int argc,
				JSValueConst* argv
			) -> JSValue
			{
				M_JS_PROXY_WRAPPER(ctx, {
					try_assert(argc == 2 || argc == 1, fmt::format("argument expected 2 or 1, received: {}", argc));
					auto s = (Data<T>*)JS_GetOpaque2(ctx, this_val, ClassID<T>::value);
					if (!s) {
						return JS_EXCEPTION;
					}
					if (argc == 1) {
						s->writeBoolean(
							JS::Converter::get_bool(ctx, argv[0])
						);
					}
					else {
						s->writeBoolean(
							JS::Converter::get_bool(ctx, argv[0]),
							static_cast<uint64_t>(JS::Converter::get_bigint64(ctx, argv[1]))
						);
					}
					return JS_UNDEFINED;
				}, "writeBoolean"_sv);
			}

			template <auto T> requires BooleanConstraint
			inline static auto writeStringByUint8(
				JSContext* ctx,
				JSValueConst this_val,
				int argc,
				JSValueConst* argv
			) -> JSValue
			{
				M_JS_PROXY_WRAPPER(ctx, {
					try_assert(argc == 2 || argc == 1, fmt::format("argument expected 2 or 1, received: {}", argc));
					auto s = (Data<T>*)JS_GetOpaque2(ctx, this_val, ClassID<T>::value);
					if (!s) {
						return JS_EXCEPTION;
					}
					if (argc == 1) {
						s->writeStringByUint8(
							JS::Converter::get_string(ctx, argv[0])
						);
					}
					else {
						s->writeStringByUint8(
							JS::Converter::get_string(ctx, argv[0]),
							static_cast<uint64_t>(JS::Converter::get_bigint64(ctx, argv[1]))
						);
					}
					return JS_UNDEFINED;
				}, "writeStringByUint8"_sv);
			}

			template <auto T> requires BooleanConstraint
			inline static auto writeStringByUint16(
				JSContext* ctx,
				JSValueConst this_val,
				int argc,
				JSValueConst* argv
			) -> JSValue
			{
				M_JS_PROXY_WRAPPER(ctx, {
					try_assert(argc == 2 || argc == 1, fmt::format("argument expected 2 or 1, received: {}", argc));
					auto s = (Data<T>*)JS_GetOpaque2(ctx, this_val, ClassID<T>::value);
					if (!s) {
						return JS_EXCEPTION;
					}
					if (argc == 1) {
						s->writeStringByUint16(
							JS::Converter::get_string(ctx, argv[0])
						);
					}
					else {
						s->writeStringByUint16(
							JS::Converter::get_string(ctx, argv[0]),
							static_cast<uint64_t>(JS::Converter::get_bigint64(ctx, argv[1]))
						);
					}
					return JS_UNDEFINED;
				}, "writeStringByUint16"_sv);
			}

			template <auto T> requires BooleanConstraint
			inline static auto writeStringByUint32(
				JSContext* ctx,
				JSValueConst this_val,
				int argc,
				JSValueConst* argv
			) -> JSValue
			{
				M_JS_PROXY_WRAPPER(ctx, {
					try_assert(argc == 2 || argc == 1, fmt::format("argument expected 2 or 1, received: {}", argc));
					auto s = (Data<T>*)JS_GetOpaque2(ctx, this_val, ClassID<T>::value);
					if (!s) {
						return JS_EXCEPTION;
					}
					if (argc == 1) {
						s->writeStringByUint32(
							JS::Converter::get_string(ctx, argv[0])
						);
					}
					else {
						s->writeStringByUint32(
							JS::Converter::get_string(ctx, argv[0]),
							static_cast<uint64_t>(JS::Converter::get_bigint64(ctx, argv[1]))
						);
					}
					return JS_UNDEFINED;
				}, "writeStringByUint32"_sv);
			}

			template <auto T> requires BooleanConstraint
			inline static auto writeStringByInt8(
				JSContext* ctx,
				JSValueConst this_val,
				int argc,
				JSValueConst* argv
			) -> JSValue
			{
				M_JS_PROXY_WRAPPER(ctx, {
					try_assert(argc == 2 || argc == 1, fmt::format("argument expected 2 or 1, received: {}", argc));
					auto s = (Data<T>*)JS_GetOpaque2(ctx, this_val, ClassID<T>::value);
					if (!s) {
						return JS_EXCEPTION;
					}
					if (argc == 1) {
						s->writeStringByInt8(
							JS::Converter::get_string(ctx, argv[0])
						);
					}
					else {
						s->writeStringByInt8(
							JS::Converter::get_string(ctx, argv[0]),
							static_cast<uint64_t>(JS::Converter::get_bigint64(ctx, argv[1]))
						);
					}
					return JS_UNDEFINED;
				}, "writeStringByInt8"_sv);
			}

			template <auto T> requires BooleanConstraint
			inline static auto writeStringByInt16(
				JSContext* ctx,
				JSValueConst this_val,
				int argc,
				JSValueConst* argv
			) -> JSValue
			{
				M_JS_PROXY_WRAPPER(ctx, {
					try_assert(argc == 2 || argc == 1, fmt::format("argument expected 2 or 1, received: {}", argc));
					auto s = (Data<T>*)JS_GetOpaque2(ctx, this_val, ClassID<T>::value);
					if (!s) {
						return JS_EXCEPTION;
					}
					if (argc == 1) {
						s->writeStringByInt16(
							JS::Converter::get_string(ctx, argv[0])
						);
					}
					else {
						s->writeStringByInt16(
							JS::Converter::get_string(ctx, argv[0]),
							static_cast<uint64_t>(JS::Converter::get_bigint64(ctx, argv[1]))
						);
					}
					return JS_UNDEFINED;
				}, "writeStringByInt16"_sv);
			}

			template <auto T> requires BooleanConstraint
			inline static auto writeStringByInt32(
				JSContext* ctx,
				JSValueConst this_val,
				int argc,
				JSValueConst* argv
			) -> JSValue
			{
				M_JS_PROXY_WRAPPER(ctx, {
					try_assert(argc == 2 || argc == 1, fmt::format("argument expected 2 or 1, received: {}", argc));
					auto s = (Data<T>*)JS_GetOpaque2(ctx, this_val, ClassID<T>::value);
					if (!s) {
						return JS_EXCEPTION;
					}
					if (argc == 1) {
						s->writeStringByInt32(
							JS::Converter::get_string(ctx, argv[0])
						);
					}
					else {
						s->writeStringByInt32(
							JS::Converter::get_string(ctx, argv[0]),
							static_cast<uint64_t>(JS::Converter::get_bigint64(ctx, argv[1]))
						);
					}
					return JS_UNDEFINED;
				}, "writeStringByInt32"_sv);
			}

			template <auto T> requires BooleanConstraint
			inline static auto writeStringByEmpty(
				JSContext* ctx,
				JSValueConst this_val,
				int argc,
				JSValueConst* argv
			) -> JSValue
			{
				M_JS_PROXY_WRAPPER(ctx, {
					try_assert(argc == 2 || argc == 1, fmt::format("argument expected 2 or 1, received: {}", argc));
					auto s = (Data<T>*)JS_GetOpaque2(ctx, this_val, ClassID<T>::value);
					if (!s) {
						return JS_EXCEPTION;
					}
					if (argc == 1) {
						s->writeStringByEmpty(
							JS::Converter::get_string(ctx, argv[0])
						);
					}
					else {
						s->writeStringByEmpty(
							JS::Converter::get_string(ctx, argv[0]),
							static_cast<uint64_t>(JS::Converter::get_bigint64(ctx, argv[1]))
						);
					}
					return JS_UNDEFINED;
				}, "writeStringByEmpty"_sv);
			}

			template <auto T> requires BooleanConstraint
			inline static auto readUint8(
				JSContext* ctx,
				JSValueConst this_val,
				int argc,
				JSValueConst* argv
			) -> JSValue
			{
				M_JS_PROXY_WRAPPER(ctx, {
					try_assert(argc == 0 || argc == 1, fmt::format("argument expected 0 or 1, received: {}", argc));
					auto s = (Data<T>*)JS_GetOpaque2(ctx, this_val, ClassID<T>::value);
					auto v = uint8_t{};
					if (!s) {
						return JS_EXCEPTION;
					}
					if (argc == 1) {
						v = s->readUint8(static_cast<uint64_t>(JS::Converter::get_bigint64(ctx, argv[0])));
					}
					else {
						v = s->readUint8();
					}
					return JS::Converter::to_bigint<uint8_t>(ctx, v);
				}, "readUint8"_sv);
			}

			template <auto T> requires BooleanConstraint
			inline static auto readUint16(
				JSContext* ctx,
				JSValueConst this_val,
				int argc,
				JSValueConst* argv
			) -> JSValue
			{
				M_JS_PROXY_WRAPPER(ctx, {
					try_assert(argc == 0 || argc == 1, fmt::format("argument expected 0 or 1, received: {}", argc));
					auto s = (Data<T>*)JS_GetOpaque2(ctx, this_val, ClassID<T>::value);
					auto v = uint16_t{};
					if (!s) {
						return JS_EXCEPTION;
					}
					if (argc == 1) {
						v = s->readUint16(static_cast<uint64_t>(JS::Converter::get_bigint64(ctx, argv[0])));
					}
					else {
						v = s->readUint16();
					}
					return JS::Converter::to_bigint<uint16_t>(ctx, v);
				}, "readUint16"_sv);
			}

			template <auto T> requires BooleanConstraint
			inline static auto readUint24(
				JSContext* ctx,
				JSValueConst this_val,
				int argc,
				JSValueConst* argv
			) -> JSValue
			{
				M_JS_PROXY_WRAPPER(ctx, {
					try_assert(argc == 0 || argc == 1, fmt::format("argument expected 0 or 1, received: {}", argc));
					auto s = (Data<T>*)JS_GetOpaque2(ctx, this_val, ClassID<T>::value);
					auto v = uint32_t{};
					if (!s) {
						return JS_EXCEPTION;
					}
					if (argc == 1) {
						v = s->readUint24(static_cast<uint64_t>(JS::Converter::get_bigint64(ctx, argv[0])));
					}
					else {
						v = s->readUint24();
					}
					return JS::Converter::to_bigint<uint32_t>(ctx, v);
				}, "readUint24"_sv);
			}

			template <auto T> requires BooleanConstraint
			inline static auto readUint32(
				JSContext* ctx,
				JSValueConst this_val,
				int argc,
				JSValueConst* argv
			) -> JSValue
			{
				M_JS_PROXY_WRAPPER(ctx, {
					try_assert(argc == 0 || argc == 1, fmt::format("argument expected 0 or 1, received: {}", argc));
					auto s = (Data<T>*)JS_GetOpaque2(ctx, this_val, ClassID<T>::value);
					auto v = uint32_t{};
					if (!s) {
						return JS_EXCEPTION;
					}
					if (argc == 1) {
						v = s->readUint32(static_cast<uint64_t>(JS::Converter::get_bigint64(ctx, argv[0])));
					}
					else {
						v = s->readUint32();
					}
					return JS::Converter::to_bigint<uint32_t>(ctx, v);
				}, "readUint32"_sv);
			}

			template <auto T> requires BooleanConstraint
			inline static auto readUint64(
				JSContext* ctx,
				JSValueConst this_val,
				int argc,
				JSValueConst* argv
			) -> JSValue
			{
				M_JS_PROXY_WRAPPER(ctx, {
					try_assert(argc == 0 || argc == 1, fmt::format("argument expected 0 or 1, received: {}", argc));
					auto s = (Data<T>*)JS_GetOpaque2(ctx, this_val, ClassID<T>::value);
					auto v = uint64_t{};
					if (!s) {
						return JS_EXCEPTION;
					}
					if (argc == 1) {
						v = s->readUint64(static_cast<uint64_t>(JS::Converter::get_bigint64(ctx, argv[0])));
					}
					else {
						v = s->readUint64();
					}
					return JS::Converter::to_bigint<uint64_t>(ctx, v);
				}, "readUint64"_sv);
			}

			template <auto T> requires BooleanConstraint
			inline static auto readInt8(
				JSContext* ctx,
				JSValueConst this_val,
				int argc,
				JSValueConst* argv
			) -> JSValue
			{
				M_JS_PROXY_WRAPPER(ctx, {
					try_assert(argc == 0 || argc == 1, fmt::format("argument expected 0 or 1, received: {}", argc));
					auto s = (Data<T>*)JS_GetOpaque2(ctx, this_val, ClassID<T>::value);
					auto v = int8_t{};
					if (!s) {
						return JS_EXCEPTION;
					}
					if (argc == 1) {
						v = s->readInt8(static_cast<uint64_t>(JS::Converter::get_bigint64(ctx, argv[0])));
					}
					else {
						v = s->readInt8();
					}
					return JS::Converter::to_bigint<int8_t>(ctx, v);
				}, "readInt8"_sv);
			}

			template <auto T> requires BooleanConstraint
			inline static auto readInt16(
				JSContext* ctx,
				JSValueConst this_val,
				int argc,
				JSValueConst* argv
			) -> JSValue
			{
				M_JS_PROXY_WRAPPER(ctx, {
					try_assert(argc == 0 || argc == 1, fmt::format("argument expected 0 or 1, received: {}", argc));
					auto s = (Data<T>*)JS_GetOpaque2(ctx, this_val, ClassID<T>::value);
					auto v = int16_t{};
					if (!s) {
						return JS_EXCEPTION;
					}
					if (argc == 1) {
						v = s->readInt16(static_cast<uint64_t>(JS::Converter::get_bigint64(ctx, argv[0])));
					}
					else {
						v = s->readInt16();
					}
					return JS::Converter::to_bigint<int16_t>(ctx, v);
				}, "readInt16"_sv);
			}

			template <auto T> requires BooleanConstraint
			inline static auto readInt24(
				JSContext* ctx,
				JSValueConst this_val,
				int argc,
				JSValueConst* argv
			) -> JSValue
			{
				M_JS_PROXY_WRAPPER(ctx, {
					try_assert(argc == 0 || argc == 1, fmt::format("argument expected 0 or 1, received: {}", argc));
					auto s = (Data<T>*)JS_GetOpaque2(ctx, this_val, ClassID<T>::value);
					auto v = int32_t{};
					if (!s) {
						return JS_EXCEPTION;
					}
					if (argc == 1) {
						v = s->readInt24(static_cast<uint64_t>(JS::Converter::get_bigint64(ctx, argv[0])));
					}
					else {
						v = s->readInt24();
					}
					return JS::Converter::to_bigint<int32_t>(ctx, v);
				}, "readInt24"_sv);
			}

			template <auto T> requires BooleanConstraint
			inline static auto readInt32(
				JSContext* ctx,
				JSValueConst this_val,
				int argc,
				JSValueConst* argv
			) -> JSValue
			{
				M_JS_PROXY_WRAPPER(ctx, {
					try_assert(argc == 0 || argc == 1, fmt::format("argument expected 0 or 1, received: {}", argc));
					auto s = (Data<T>*)JS_GetOpaque2(ctx, this_val, ClassID<T>::value);
					auto v = int32_t{};
					if (!s) {
						return JS_EXCEPTION;
					}
					if (argc == 1) {
						v = s->readInt32(static_cast<uint64_t>(JS::Converter::get_bigint64(ctx, argv[0])));
					}
					else {
						v = s->readInt32();
					}
					return JS::Converter::to_bigint<int32_t>(ctx, v);
				}, "readInt32"_sv);
			}

			template <auto T> requires BooleanConstraint
			inline static auto readInt64(
				JSContext* ctx,
				JSValueConst this_val,
				int argc,
				JSValueConst* argv
			) -> JSValue
			{
				M_JS_PROXY_WRAPPER(ctx, {
					try_assert(argc == 0 || argc == 1, fmt::format("argument expected 0 or 1, received: {}", argc));
					auto s = (Data<T>*)JS_GetOpaque2(ctx, this_val, ClassID<T>::value);
					auto v = int64_t{};
					if (!s) {
						return JS_EXCEPTION;
					}
					if (argc == 1) {
						v = s->readInt64(static_cast<uint64_t>(JS::Converter::get_bigint64(ctx, argv[0])));
					}
					else {
						v = s->readInt64();
					}
					return JS::Converter::to_bigint<int64_t>(ctx, v);
				}, "readInt64"_sv);
			}
			
			template <auto T> requires BooleanConstraint
			inline static auto readString(
				JSContext* ctx,
				JSValueConst this_val,
				int argc,
				JSValueConst* argv
			) -> JSValue
			{
				M_JS_PROXY_WRAPPER(ctx, {
					try_assert(argc == 1 || argc == 2, fmt::format("argument expected 1 or 2, received: {}", argc));
					auto s = (Data<T>*)JS_GetOpaque2(ctx, this_val, ClassID<T>::value);
					auto v = std::string{};
					if (!s) {
						return JS_EXCEPTION;
					}
					if (argc == 1) {
						v = s->readString(static_cast<uint64_t>(JS::Converter::get_bigint64(ctx, argv[0])));
					}
					else {
						v = s->readString(
							static_cast<uint64_t>(JS::Converter::get_bigint64(ctx, argv[0])),
							static_cast<uint64_t>(JS::Converter::get_bigint64(ctx, argv[1]))
						);
					}
					return JS::Converter::to_string(ctx, v);
				}, "readString"_sv);
			}

			template <auto T> requires BooleanConstraint
			inline static auto readStringByUint8(
				JSContext* ctx,
				JSValueConst this_val,
				int argc,
				JSValueConst* argv
			) -> JSValue
			{
				M_JS_PROXY_WRAPPER(ctx, {
					try_assert(argc == 0 || argc == 1, fmt::format("argument expected 0 or 1, received: {}", argc));
					auto s = (Data<T>*)JS_GetOpaque2(ctx, this_val, ClassID<T>::value);
					auto v = std::string{};
					if (!s) {
						return JS_EXCEPTION;
					}
					if (argc == 1) {
						v = s->readStringByUint8(static_cast<uint64_t>(JS::Converter::get_bigint64(ctx, argv[0])));
					}
					else {
						v = s->readStringByUint8();
					}
					return JS::Converter::to_string(ctx, v);
				}, "readStringByUint8"_sv);
			}

			template <auto T> requires BooleanConstraint
			inline static auto readStringByUint16(
				JSContext* ctx,
				JSValueConst this_val,
				int argc,
				JSValueConst* argv
			) -> JSValue
			{
				M_JS_PROXY_WRAPPER(ctx, {
					try_assert(argc == 0 || argc == 1, fmt::format("argument expected 0 or 1, received: {}", argc));
					auto s = (Data<T>*)JS_GetOpaque2(ctx, this_val, ClassID<T>::value);
					auto v = std::string{};
					if (!s) {
						return JS_EXCEPTION;
					}
					if (argc == 1) {
						v = s->readStringByUint16(static_cast<uint64_t>(JS::Converter::get_bigint64(ctx, argv[0])));
					}
					else {
						v = s->readStringByUint16();
					}
					return JS::Converter::to_string(ctx, v);
				}, "readStringByUint16"_sv);
			}

			template <auto T> requires BooleanConstraint
			inline static auto readStringByUint32(
				JSContext* ctx,
				JSValueConst this_val,
				int argc,
				JSValueConst* argv
			) -> JSValue
			{
				M_JS_PROXY_WRAPPER(ctx, {
					try_assert(argc == 0 || argc == 1, fmt::format("argument expected 0 or 1, received: {}", argc));
					auto s = (Data<T>*)JS_GetOpaque2(ctx, this_val, ClassID<T>::value);
					auto v = std::string{};
					if (!s) {
						return JS_EXCEPTION;
					}
					if (argc == 1) {
						v = s->readStringByUint32(static_cast<uint64_t>(JS::Converter::get_bigint64(ctx, argv[0])));
					}
					else {
						v = s->readStringByUint32();
					}
					return JS::Converter::to_string(ctx, v);
				}, "readStringByUint32"_sv);
			}

			template <auto T> requires BooleanConstraint
			inline static auto readStringByInt8(
				JSContext* ctx,
				JSValueConst this_val,
				int argc,
				JSValueConst* argv
			) -> JSValue
			{
				M_JS_PROXY_WRAPPER(ctx, {
					try_assert(argc == 0 || argc == 1, fmt::format("argument expected 0 or 1, received: {}", argc));
					auto s = (Data<T>*)JS_GetOpaque2(ctx, this_val, ClassID<T>::value);
					auto v = std::string{};
					if (!s) {
						return JS_EXCEPTION;
					}
					if (argc == 1) {
						v = s->readStringByInt8(static_cast<uint64_t>(JS::Converter::get_bigint64(ctx, argv[0])));
					}
					else {
						v = s->readStringByInt8();
					}
					return JS::Converter::to_string(ctx, v);
				}, "readStringByInt8"_sv);
			}

			template <auto T> requires BooleanConstraint
			inline static auto readStringByInt16(
				JSContext* ctx,
				JSValueConst this_val,
				int argc,
				JSValueConst* argv
			) -> JSValue
			{
				M_JS_PROXY_WRAPPER(ctx, {
					try_assert(argc == 0 || argc == 1, fmt::format("argument expected 0 or 1, received: {}", argc));
					auto s = (Data<T>*)JS_GetOpaque2(ctx, this_val, ClassID<T>::value);
					auto v = std::string{};
					if (!s) {
						return JS_EXCEPTION;
					}
					if (argc == 1) {
						v = s->readStringByInt16(static_cast<uint64_t>(JS::Converter::get_bigint64(ctx, argv[0])));
					}
					else {
						v = s->readStringByInt16();
					}
					return JS::Converter::to_string(ctx, v);
				}, "readStringByInt16"_sv);
			}

			template <auto T> requires BooleanConstraint
			inline static auto readStringByInt32(
				JSContext* ctx,
				JSValueConst this_val,
				int argc,
				JSValueConst* argv
			) -> JSValue
			{
				M_JS_PROXY_WRAPPER(ctx, {
					try_assert(argc == 0 || argc == 1, fmt::format("argument expected 0 or 1, received: {}", argc));
					auto s = (Data<T>*)JS_GetOpaque2(ctx, this_val, ClassID<T>::value);
					auto v = std::string{};
					if (!s) {
						return JS_EXCEPTION;
					}
					if (argc == 1) {
						v = s->readStringByInt32(static_cast<uint64_t>(JS::Converter::get_bigint64(ctx, argv[0])));
					}
					else {
						v = s->readStringByInt32();
					}
					return JS::Converter::to_string(ctx, v);
				}, "readStringByInt32"_sv);
			}

			template <auto T> requires BooleanConstraint
			inline static auto readStringByVarInt32(
				JSContext* ctx,
				JSValueConst this_val,
				int argc,
				JSValueConst* argv
			) -> JSValue
			{
				M_JS_PROXY_WRAPPER(ctx, {
					try_assert(argc == 0 || argc == 1, fmt::format("argument expected 0 or 1, received: {}", argc));
					auto s = (Data<T>*)JS_GetOpaque2(ctx, this_val, ClassID<T>::value);
					auto v = std::string{};
					if (!s) {
						return JS_EXCEPTION;
					}
					if (argc == 1) {
						v = s->readStringByVarInt32(static_cast<uint64_t>(JS::Converter::get_bigint64(ctx, argv[0])));
					}
					else {
						v = s->readStringByVarInt32();
					}
					return JS::Converter::to_string(ctx, v);
				}, "readStringByVarInt32"_sv);
			}

			template <auto T> requires BooleanConstraint
			inline static auto readStringByEmpty(
				JSContext* ctx,
				JSValueConst this_val,
				int argc,
				JSValueConst* argv
			) -> JSValue
			{
				M_JS_PROXY_WRAPPER(ctx, {
					try_assert(argc == 0 || argc == 1, fmt::format("argument expected 0 or 1, received: {}", argc));
					auto s = (Data<T>*)JS_GetOpaque2(ctx, this_val, ClassID<T>::value);
					auto v = std::string{};
					if (!s) {
						return JS_EXCEPTION;
					}
					if (argc == 1) {
						v = s->readStringByEmpty(static_cast<uint64_t>(JS::Converter::get_bigint64(ctx, argv[0])));
					}
					else {
						v = s->readStringByEmpty();
					}
					return JS::Converter::to_string(ctx, v);
				}, "readStringByEmpty"_sv);
			}

			template <auto T> requires BooleanConstraint
			inline static auto readVarInt32(
				JSContext* ctx,
				JSValueConst this_val,
				int argc,
				JSValueConst* argv
			) -> JSValue
			{
				M_JS_PROXY_WRAPPER(ctx, {
					try_assert(argc == 0 || argc == 1, fmt::format("argument expected 0 or 1, received: {}", argc));
					auto s = (Data<T>*)JS_GetOpaque2(ctx, this_val, ClassID<T>::value);
					auto v = int32_t{};
					if (!s) {
						return JS_EXCEPTION;
					}
					if (argc == 1) {
						v = s->readVarInt32(static_cast<uint64_t>(JS::Converter::get_bigint64(ctx, argv[0])));
					}
					else {
						v = s->readVarInt32();
					}
					return JS::Converter::to_bigint<int32_t>(ctx, v);
				}, "readVarInt32"_sv);
			}

			template <auto T> requires BooleanConstraint
			inline static auto readVarInt64(
				JSContext* ctx,
				JSValueConst this_val,
				int argc,
				JSValueConst* argv
			) -> JSValue
			{
				M_JS_PROXY_WRAPPER(ctx, {
					try_assert(argc == 0 || argc == 1, fmt::format("argument expected 0 or 1, received: {}", argc));
					auto s = (Data<T>*)JS_GetOpaque2(ctx, this_val, ClassID<T>::value);
					auto v = int64_t{};
					if (!s) {
						return JS_EXCEPTION;
					}
					if (argc == 1) {
						v = s->readVarInt64(static_cast<uint64_t>(JS::Converter::get_bigint64(ctx, argv[0])));
					}
					else {
						v = s->readVarInt64();
					}
					return JS::Converter::to_bigint<int64_t>(ctx, v);
				}, "readVarInt64"_sv);
			}

			template <auto T> requires BooleanConstraint
			inline static auto readVarUint32(
				JSContext* ctx,
				JSValueConst this_val,
				int argc,
				JSValueConst* argv
			) -> JSValue
			{
				M_JS_PROXY_WRAPPER(ctx, {
					try_assert(argc == 0 || argc == 1, fmt::format("argument expected 0 or 1, received: {}", argc));
					auto s = (Data<T>*)JS_GetOpaque2(ctx, this_val, ClassID<T>::value);
					auto v = uint32_t{};
					if (!s) {
						return JS_EXCEPTION;
					}
					if (argc == 1) {
						v = s->readVarUint32(static_cast<uint64_t>(JS::Converter::get_bigint64(ctx, argv[0])));
					}
					else {
						v = s->readVarUint32();
					}
					return JS::Converter::to_bigint<uint32_t>(ctx, v);
				}, "readVarUint32"_sv);
			}

			template <auto T> requires BooleanConstraint
			inline static auto readVarUint64(
				JSContext* ctx,
				JSValueConst this_val,
				int argc,
				JSValueConst* argv
			) -> JSValue
			{
				M_JS_PROXY_WRAPPER(ctx, {
					try_assert(argc == 0 || argc == 1, fmt::format("argument expected 0 or 1, received: {}", argc));
					auto s = (Data<T>*)JS_GetOpaque2(ctx, this_val, ClassID<T>::value);
					auto v = uint64_t{};
					if (!s) {
						return JS_EXCEPTION;
					}
					if (argc == 1) {
						v = s->readVarUint64(static_cast<uint64_t>(JS::Converter::get_bigint64(ctx, argv[0])));
					}
					else {
						v = s->readVarUint64();
					}
					return JS::Converter::to_bigint<uint64_t>(ctx, v);
				}, "readVarUint64"_sv);
			}

			template <auto T> requires BooleanConstraint
			inline static auto readZigZag32(
				JSContext* ctx,
				JSValueConst this_val,
				int argc,
				JSValueConst* argv
			) -> JSValue
			{
				M_JS_PROXY_WRAPPER(ctx, {
					try_assert(argc == 0 || argc == 1, fmt::format("argument expected 0 or 1, received: {}", argc));
					auto s = (Data<T>*)JS_GetOpaque2(ctx, this_val, ClassID<T>::value);
					auto v = int32_t{};
					if (!s) {
						return JS_EXCEPTION;
					}
					if (argc == 1) {
						v = s->readZigZag32(static_cast<uint64_t>(JS::Converter::get_bigint64(ctx, argv[0])));
					}
					else {
						v = s->readZigZag32();
					}
					return JS::Converter::to_bigint<int32_t>(ctx, v);
				}, "readZigZag32"_sv);
			}

			template <auto T> requires BooleanConstraint
			inline static auto readZigZag64(
				JSContext* ctx,
				JSValueConst this_val,
				int argc,
				JSValueConst* argv
			) -> JSValue
			{
				M_JS_PROXY_WRAPPER(ctx, {
					try_assert(argc == 0 || argc == 1, fmt::format("argument expected 0 or 1, received: {}", argc));
					auto s = (Data<T>*)JS_GetOpaque2(ctx, this_val, ClassID<T>::value);
					auto v = int64_t{};
					if (!s) {
						return JS_EXCEPTION;
					}
					if (argc == 1) {
						v = s->readZigZag64(static_cast<std::uint64_t>(JS::Converter::get_bigint64(ctx, argv[0])));
					}
					else {
						v = s->readZigZag64();
					}
					return JS::Converter::to_bigint<int64_t>(ctx, v);
				}, "readZigZag64"_sv);
			}

			template <auto T> requires BooleanConstraint
			inline static auto readFloat(
				JSContext* ctx,
				JSValueConst this_val,
				int argc,
				JSValueConst* argv
			) -> JSValue
			{
				M_JS_PROXY_WRAPPER(ctx, {
					try_assert(argc == 0 || argc == 1, fmt::format("argument expected 0 or 1, received: {}", argc));
					auto s = (Data<T>*)JS_GetOpaque2(ctx, this_val, ClassID<T>::value);
					auto v = float{};
					if (!s) {
						return JS_EXCEPTION;
					}
					if (argc == 1) {
						v = s->readFloat(static_cast<std::uint64_t>(JS::Converter::get_bigint64(ctx, argv[0])));
					}
					else {
						v = s->readFloat();
					}
					return JS::Converter::to_number(ctx, v);
				}, "readFloat"_sv);
			}

			template <auto T> requires BooleanConstraint
			inline static auto readDouble(
				JSContext* ctx,
				JSValueConst this_val,
				int argc,
				JSValueConst* argv
			) -> JSValue
			{
				M_JS_PROXY_WRAPPER(ctx, {
					try_assert(argc == 0 || argc == 1, fmt::format("argument expected 0 or 1, received: {}", argc));
					auto s = (Data<T>*)JS_GetOpaque2(ctx, this_val, ClassID<T>::value);
					auto v = double{};
					if (!s) {
						return JS_EXCEPTION;
					}
					if (argc == 1) {
						v = s->readDouble(static_cast<std::uint64_t>(JS::Converter::get_bigint64(ctx, argv[0])));
					}
					else {
						v = s->readDouble();
					}
					return JS::Converter::to_number(ctx, v);
				}, "readDouble"_sv);
			}

			template <auto T> requires BooleanConstraint
			inline static auto close(
				JSContext* ctx,
				JSValueConst this_val,
				int argc,
				JSValueConst* argv
			) -> JSValue
			{
				M_JS_PROXY_WRAPPER(ctx, {
					try_assert(argc == 0, fmt::format("argument expected 0, received: {}", argc));
					auto s = (Data<T>*)JS_GetOpaque2(ctx, this_val, ClassID<T>::value);
					if (!s) {
						return JS_EXCEPTION;
					}
					s->close();
					return JS_UNDEFINED;
				}, "close"_sv);
			}

			template <auto T> requires BooleanConstraint
			inline static const JSCFunctionListEntry proto_functions[] = {
				JS_CPPGETSET_MAGIC_DEF("read_position", getter<T>, setter<T>, 0),
				JS_CPPGETSET_MAGIC_DEF("write_position", getter<T>, setter<T>, 1),
				JS_CPPFUNC_DEF("size", 0, size<T>),
				JS_CPPFUNC_DEF("fromString", 1, fromString<T>),
				JS_CPPFUNC_DEF("capacity", 0, capacity<T>),
				JS_CPPFUNC_DEF("reserve", 1, reserve<T>),
				JS_CPPFUNC_DEF("toArrayBuffer", 0, toArrayBuffer<T>),
				JS_CPPFUNC_DEF("toUint8Array", 0, toUint8Array<T>),
				JS_CPPFUNC_DEF("getArrayBuffer", 2, getArrayBuffer<T>),
				JS_CPPFUNC_DEF("getUint8Array", 2, getUint8Array<T>),
				JS_CPPFUNC_DEF("toString", 0, toString<T>),
				JS_CPPFUNC_DEF("out_file", 1, out_file<T>),
				JS_CPPFUNC_DEF("writeUint8", 2, writeUint8<T>),
				JS_CPPFUNC_DEF("writeUint16", 2, writeUint16<T>),
				JS_CPPFUNC_DEF("writeUint24", 2, writeUint24<T>),
				JS_CPPFUNC_DEF("writeUint32", 2, writeUint32<T>),
				JS_CPPFUNC_DEF("writeUint64", 2, writeUint64<T>),
				JS_CPPFUNC_DEF("writeInt8", 2, writeInt8<T>),
				JS_CPPFUNC_DEF("writeInt16", 2, writeInt16<T>),
				JS_CPPFUNC_DEF("writeInt24", 2, writeInt24<T>),
				JS_CPPFUNC_DEF("writeInt32", 2, writeInt32<T>),
				JS_CPPFUNC_DEF("writeInt64", 2, writeInt64<T>),
				JS_CPPFUNC_DEF("writeArrayBuffer", 2, writeArrayBuffer<T>),
				JS_CPPFUNC_DEF("writeUint8Array", 2, writeUint8Array<T>),
				JS_CPPFUNC_DEF("writeFloat", 2, writeFloat<T>),
				JS_CPPFUNC_DEF("writeDouble", 2, writeDouble<T>),
				JS_CPPFUNC_DEF("writeVarInt32", 2, writeVarInt32<T>),
				JS_CPPFUNC_DEF("writeVarInt64", 2, writeVarInt64<T>),
				JS_CPPFUNC_DEF("writeZigZag32", 2, writeZigZag32<T>),
				JS_CPPFUNC_DEF("writeZigZag64", 2, writeZigZag64<T>),
				JS_CPPFUNC_DEF("writeString", 2, writeString<T>),
				JS_CPPFUNC_DEF("writeStringFourByte", 2, writeStringFourByte<T>),
				JS_CPPFUNC_DEF("writeNull", 2, writeNull<T>),
				JS_CPPFUNC_DEF("writeBoolean", 2, writeBoolean<T>),
				JS_CPPFUNC_DEF("writeStringByUint8", 2, writeStringByUint8<T>),
				JS_CPPFUNC_DEF("writeStringByUint16", 2, writeStringByUint16<T>),
				JS_CPPFUNC_DEF("writeStringByUint32", 2, writeStringByUint32<T>),
				JS_CPPFUNC_DEF("writeStringByInt8", 2, writeStringByInt8<T>),
				JS_CPPFUNC_DEF("writeStringByInt16", 2, writeStringByInt16<T>),
				JS_CPPFUNC_DEF("writeStringByInt32", 2, writeStringByInt32<T>),
				JS_CPPFUNC_DEF("writeStringByEmpty", 2, writeStringByEmpty<T>),
				JS_CPPFUNC_DEF("readUint8", 1, readUint8<T>),
				JS_CPPFUNC_DEF("readUint16", 1, readUint16<T>),
				JS_CPPFUNC_DEF("readUint24", 1, readUint24<T>),
				JS_CPPFUNC_DEF("readUint32", 1, readUint32<T>),
				JS_CPPFUNC_DEF("readUint64", 1, readUint64<T>),
				JS_CPPFUNC_DEF("readInt8", 1, readInt8<T>),
				JS_CPPFUNC_DEF("readInt16", 1, readInt16<T>),
				JS_CPPFUNC_DEF("readInt24", 1, readInt24<T>),
				JS_CPPFUNC_DEF("readInt32", 1, readInt32<T>),
				JS_CPPFUNC_DEF("readInt64", 1, readInt64<T>),
				JS_CPPFUNC_DEF("readString", 2, readString<T>),
				JS_CPPFUNC_DEF("readStringByUint8", 1, readStringByUint8<T>),
				JS_CPPFUNC_DEF("readStringByUint16", 1, readStringByUint16<T>),
				JS_CPPFUNC_DEF("readStringByUint32", 1, readStringByUint32<T>),
				JS_CPPFUNC_DEF("readStringByInt8", 1, readStringByInt8<T>),
				JS_CPPFUNC_DEF("readStringByInt16", 1, readStringByInt16<T>),
				JS_CPPFUNC_DEF("readStringByInt32", 1, readStringByInt32<T>),
				JS_CPPFUNC_DEF("readStringByVarInt32", 1, readStringByVarInt32<T>),
				JS_CPPFUNC_DEF("readStringByEmpty", 1, readStringByEmpty<T>),
				JS_CPPFUNC_DEF("readVarInt32", 1, readVarInt32<T>),
				JS_CPPFUNC_DEF("readVarInt64", 1, readVarInt64<T>),
				JS_CPPFUNC_DEF("readVarUint32", 1, readVarUint32<T>),
				JS_CPPFUNC_DEF("readVarUint64", 1, readVarUint64<T>),
				JS_CPPFUNC_DEF("readZigZag32", 1, readZigZag32<T>),
				JS_CPPFUNC_DEF("readZigZag64", 1, readZigZag64<T>),
				JS_CPPFUNC_DEF("readFloat", 1, readFloat<T>),
				JS_CPPFUNC_DEF("readDouble", 1, readDouble<T>),
				JS_CPPFUNC_DEF("close", 0, close<T>),
			};

			
			template <auto use_big_endian> requires BooleanConstraint
			inline static auto register_class(
				JSContext* ctx
			) -> void
			{
				ClassID<use_big_endian>::value = JS_NewClass(JS_GetRuntime(ctx), ClassID<use_big_endian>::value, &this_class<use_big_endian>);
				auto class_name = std::string_view{};
				if constexpr (use_big_endian) {
					class_name = "DataStreamViewUseBigEndian"_sv;
				}
				else {
					class_name = "DataStreamView"_sv;
				}
				auto point_ctor = JS_NewCFunction2(ctx, constructor<use_big_endian>, class_name.data(), 2, JS_CFUNC_constructor, 0);
				auto proto = JS_NewObject(ctx);
				JS_SetPropertyFunctionList(ctx, proto, proto_functions<use_big_endian>, countof(proto_functions<use_big_endian>));
				JS_SetConstructor(ctx, point_ctor, proto);
				auto global_obj = JS_GetGlobalObject(ctx);
				JS_INSTANCE_OF_OBJ(ctx, obj1, global_obj, "Sen"_sv);
				JS_INSTANCE_OF_OBJ(ctx, obj2, obj1, "Kernel"_sv);
				JS_SetPropertyStr(ctx, obj2, class_name.data(), point_ctor);
				JS_FreeValue(ctx, global_obj);
				JS_FreeValue(ctx, obj1);
				JS_FreeValue(ctx, obj2);
				return;
			}

		}

		namespace Boolean {
			
			struct Data {
				bool value;

				Data(
				) = default;

				Data(
					bool value
				) : value(value)
				{

				}

				~Data(
				) = default;
			};

			inline static JSClassID class_id;

			inline static auto finalizer(
				JSRuntime* rt,
				JSValue val
			) -> void
			{
				auto s = (Data*)JS_GetOpaque(val, class_id);
				if (s != nullptr) {
					delete s;
				}
				return;
			}

			inline static auto constructor(
				JSContext* ctx,
				JSValueConst new_target,
				int argc,
				JSValueConst* argv
			) -> JSValue
			{
				auto s = static_cast<Data*>(nullptr);
				auto obj = JS_UNDEFINED;
				auto proto = JSValue{};
				if (argc == 1) {
					s = new Data{JS_VALUE_GET_BOOL(argv[0]) == 0 ? false : true};
				}
				else {
					return JS_EXCEPTION;
				}
				proto = JS_GetPropertyStr(ctx, new_target, "prototype");
				if (JS_IsException(proto)) {
					goto fail;
				}
				obj = JS_NewObjectProtoClass(ctx, proto, class_id);
				JS_FreeValue(ctx, proto);
				if (JS_IsException(obj))
					goto fail;
				JS_SetOpaque(obj, s);
				return obj;
			fail:
				js_free(ctx, s);
				JS_FreeValue(ctx, obj);
				return JS_EXCEPTION;
			}

			inline static auto this_class = JSClassDef{
				.class_name = "Boolean",
				.finalizer = finalizer,
			};

			inline static auto getter(
				JSContext* ctx,
				JSValueConst this_val,
				int magic
			) -> JSValue
			{
				auto s = static_cast<Data*>(JS_GetOpaque2(ctx, this_val, class_id));
				if (!s) {
					return JS_EXCEPTION;
				}
				return JS::Converter::to_bool(ctx, s->value);
			}

			inline static auto setter(
				JSContext* ctx,
				JSValueConst this_val,
				JSValueConst val,
				int magic
			) -> JSValue
			{
				auto s = static_cast<Data*>(JS_GetOpaque2(ctx, this_val, class_id));
				auto v = bool{};
				if (s == nullptr) {
					return JS_EXCEPTION;
				}
				s->value = JS_VALUE_GET_BOOL(val) == 0 ? false : true;
				return JS_UNDEFINED;
			}

			inline static const JSCFunctionListEntry proto_functions[] = {
				JS_CPPGETSET_MAGIC_DEF("value", getter, setter, 0),
			};

			template <auto T>
			inline static auto true_instance(
				JSContext* ctx,
				JSValueConst this_val,
				int argc,
				JSValueConst* argv
			) -> JSValue
			{
				auto s = new Data{ T };
				auto proto = JS_GetPropertyStr(ctx, this_val, "prototype");
				if (JS_IsException(proto)) {
					js_free(ctx, s);
					return JS_EXCEPTION;
				}
				auto obj = JS_NewObjectProtoClass(ctx, proto, class_id);
				JS_FreeValue(ctx, proto);
				if (JS_IsException(obj)) {
					js_free(ctx, s);
					return JS_EXCEPTION;
				}
				JS_SetOpaque(obj, s);
				return obj;
			}


			inline static auto register_class(
				JSContext* ctx
			) -> void
			{
				class_id = JS_NewClass(JS_GetRuntime(ctx), class_id, &this_class);
				auto class_name = "Boolean"_sv;
				auto point_ctor = JS_NewCFunction2(ctx, constructor, class_name.data(), 2, JS_CFUNC_constructor, 0);
				auto proto = JS_NewObject(ctx);
				auto default_true_func_val = JS_NewCFunction(ctx, true_instance<true>, "true", 0);
				auto default_false_func_val = JS_NewCFunction(ctx, true_instance<false>, "false", 0);
				JS_SetPropertyStr(ctx, point_ctor, "true", default_true_func_val);
				JS_SetPropertyStr(ctx, point_ctor, "false", default_false_func_val);
				JS_SetPropertyFunctionList(ctx, proto, proto_functions, countof(proto_functions));
				JS_SetConstructor(ctx, point_ctor, proto);
				auto global_obj = JS_GetGlobalObject(ctx);
				JS_INSTANCE_OF_OBJ(ctx, obj1, global_obj, "Sen"_sv);
				JS_INSTANCE_OF_OBJ(ctx, obj2, obj1, "Kernel"_sv);
				JS_SetPropertyStr(ctx, obj2, class_name.data(), point_ctor);
				JS_FreeValue(ctx, global_obj);
				JS_FreeValue(ctx, obj1);
				JS_FreeValue(ctx, obj2);
				return;
			}

		}
	}

}