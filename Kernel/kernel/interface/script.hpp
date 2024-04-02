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
	return JS_Eval(context, evaluate_context.data(), evaluate_context.size(), source.data(), JS_EVAL_FLAG_STRICT | JS_EVAL_TYPE_GLOBAL);

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

	// JS Type

	namespace JSElement {

		using undefined = JSValue;

		using null = JSValue;

		using bigint = JSValue;

		using number = JSValue;

		using string = JSValue;

		using boolean = JSValue;

		using Array = JSValue;

		using Object = JSValue;

		using ArrayBuffer = JSValue;

		using Uint8Array = JSValue;

		using Prototype = JSValue;

		using any = JSValue;

		using ParameterList = JSValue *;

		using ParameterCount = int;

		using Context = JSContext;

	};

	namespace JSDefine {

		using DataStreamView = JSValue;

		using Boolean = JSValue;

		using Character = JSValue;

		using String = JSValue;

		using Size = JSValue;

		using BinaryView = JSValue;

		using ImageView = JSValue;

		using Canvas = JSValue;

		using JsonWriter = JSValue;

		using APNGMakerSetting = JSValue;
	}

	/**
	 * Shell callback
	*/

	inline static auto callback(
		JSContext *context, 
		JSValueConst this_val, 
		int argc, 
		JSValueConst *argv
	) -> JSElement::undefined
	{
		M_JS_PROXY_WRAPPER(context, {
			try_assert(argc == 1, fmt::format("{} 1, {}: {}", Kernel::Language::get("kernel.argument_expected"), Kernel::Language::get("kernel.argument_received"), argc));
			assert_conditional(JS_IsArray(context, argv[0]), fmt::format("{} {} {} {}", Kernel::Language::get("kernel.expected_argument"), 0, Kernel::Language::get("is"), Kernel::Language::get("kernel.tuple.js_array")), "callback");
			auto destination = std::make_unique<CStringView>();
			Shell::callback(construct_string_list(JS::Converter::get_vector<std::string>(context, argv[0])).get(), destination.get() , nullptr);
			return JS::Converter::to_string(context, construct_standard_string(*destination));
		}, "callback"_sv);
	}

	// Test method

	inline static auto test(
		JSContext* context,
		JSValueConst this_val,
		int argc,
		JSValueConst* argv
	) -> JSElement::undefined
	{
		M_JS_PROXY_WRAPPER(context, {
			return JS_UNDEFINED;
		},
		"test"_sv);
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
		) -> JSElement::Object
		{
			switch (json.type()) {
				case nlohmann::ordered_json::value_t::null: {
					return JS::Converter::get_null();
				}
				case nlohmann::ordered_json::value_t::object: {
					auto js_obj = JS_NewObject(context);
					for (auto& [key, value] : json.items()) {
						JS_DefinePropertyValueStr(context, js_obj, key.data(), json_to_js_value(context, value), JS_PROP_C_W_E);
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
					return JS_NewStringLen(context, json.get<std::string>().data(), json.get<std::string>().size());
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
		) -> JSElement::Object
		{
			M_JS_PROXY_WRAPPER(context, {
				try_assert(argc == 1, fmt::format("{} 1, {}: {}", Kernel::Language::get("kernel.argument_expected"), Kernel::Language::get("kernel.argument_received"), argc));
				auto source = JS::Converter::get_string(context, argv[0]);
				auto json = nlohmann::ordered_json::parse(source);
				auto js_obj = json_to_js_value(context, json);
				return js_obj;
			}, "deserialize"_sv);
		}

		inline static auto deserialize_fs(
			JSContext* context,
			JSValueConst this_val,
			int argc,
			JSValueConst* argv
		) -> JSElement::Object
		{

			M_JS_PROXY_WRAPPER(context, {
				try_assert(argc == 1, fmt::format("{} 1, {}: {}", Kernel::Language::get("kernel.argument_expected"), Kernel::Language::get("kernel.argument_received"), argc));
				auto source = JS::Converter::get_string(context, argv[0]);
				auto json = Sen::Kernel::FileSystem::read_json(source);
				auto js_obj = json_to_js_value(context, *json);


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
		) -> JSElement::string
		{
			M_JS_PROXY_WRAPPER(context, {
				try_assert(argc == 3, fmt::format("{} 3, {}: {}", Kernel::Language::get("kernel.argument_expected"), Kernel::Language::get("kernel.argument_received"), argc));
				auto json = js_object_to_json(context, argv[0]);
				auto indent = JS::Converter::get_int32(context, argv[1]);
				auto ensure_ascii = JS::Converter::get_bool(context, argv[2]);;
				auto source = json.dump(indent, '\t', ensure_ascii);
				return JS_NewStringLen(context, source.data(), source.size());
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
		) -> JSElement::undefined
		{
			M_JS_PROXY_WRAPPER(context, {
				try_assert(argc == 4, fmt::format("{} 4, {}: {}", Kernel::Language::get("kernel.argument_expected"), Kernel::Language::get("kernel.argument_received"), argc));
				auto destination = JS::Converter::get_string(context, argv[0]);
				auto json = js_object_to_json(context, argv[1]);
				auto indent = JS::Converter::get_int32(context, argv[2]);
				auto ensure_ascii = JS::Converter::get_bool(context, argv[3]);
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
		) -> JSElement::undefined
		{
			M_JS_PROXY_WRAPPER(context, {
				try_assert(argc == 1, fmt::format("{} 1, {}: {}", Kernel::Language::get("kernel.argument_expected"), Kernel::Language::get("kernel.argument_received"), argc));
				assert_conditional(JS_IsString(argv[0]), fmt::format("{} {} {} {}", Kernel::Language::get("kernel.expected_argument"), 0, Kernel::Language::get("is"), Kernel::Language::get("kernel.tuple.js_string")), "load_language");
				Sen::Kernel::Language::read_language(JS::Converter::get_string(context, argv[0]));
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
		) -> JSElement::string
		{
			M_JS_PROXY_WRAPPER(context, {
				try_assert(argc == 1, fmt::format("{} 1, {}: {}", Kernel::Language::get("kernel.argument_expected"), Kernel::Language::get("kernel.argument_received"), argc));
				assert_conditional(JS_IsString(argv[0]), fmt::format("{} {} {} {}", Kernel::Language::get("kernel.expected_argument"), 0, Kernel::Language::get("is"), Kernel::Language::get("kernel.tuple.js_string")), "get");
				auto result = Sen::Kernel::Language::get(JS::Converter::get_string(context, argv[0]));
				return JS::Converter::to_string(context, result.data());
			}, "get"_sv);
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
				static_assert(use_big_endian == true or use_big_endian == false, "use_big_endian must be true or false");
				static_assert(sizeof(use_big_endian) == sizeof(bool));
				inline static JSClassID value = 0;
			};


			template <auto T> requires BooleanConstraint
			inline static auto finalizer(
				JSRuntime* rt,
				JSValue val
			) -> void
			{
				static_assert(T == true or T == false, "T must be true or false");
				static_assert(sizeof(T) == sizeof(bool));
				auto s = static_cast<Data<T>*>(JS_GetOpaque(val, ClassID<T>::value));
				if (s != nullptr) {
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
			) -> JSElement::undefined
			{
				static_assert(T == true or T == false, "T must be true or false");
				static_assert(sizeof(T) == sizeof(bool));
				M_JS_PROXY_WRAPPER(ctx, {
					auto s = static_cast<Data<T>*>(nullptr);
					auto obj = JS_UNDEFINED;
					auto proto = JSElement::Prototype{};
					switch (argc) {
						case 1:{
							s = new Data<T>{ JS::Converter::get_string(ctx, argv[0]) };
							break;
						}
						case 0:{
							s = new Data<T>{};
							break;
						}
						default:{
							JS_ThrowInternalError(ctx, fmt::format("{}", Kernel::Language::get("data_stream_view_cannot_be_initialize")).data());
							return JS_EXCEPTION;
						}
					}
					proto = JS_GetPropertyStr(ctx, new_target, "prototype");
					if (JS_IsException(proto)) {
						goto fail;
					}
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
				}, "proxy_constructor");
			}

			template <auto T> requires BooleanConstraint
			inline static auto this_class = JSClassDef{
				.class_name = "DataStreamView",
				.finalizer = finalizer<T>,
			};

			template <auto T> requires BooleanConstraint
			inline static auto getter(
				JSContext* ctx,
				JSValueConst this_val,
				int magic
			) -> JSElement::bigint
			{
				static_assert(T == true or T == false, "T must be true or false");
				static_assert(sizeof(T) == sizeof(bool));
				auto s = static_cast<Data<T>*>(JS_GetOpaque2(ctx, this_val, ClassID<T>::value));
				if (s == nullptr) {
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
			) -> JSElement::bigint
			{
				static_assert(T == true or T == false, "T must be true or false");
				static_assert(sizeof(T) == sizeof(bool));
				auto s = static_cast<Data<T>*>(JS_GetOpaque2(ctx, this_val, ClassID<T>::value));
				auto v = std::int64_t{};
				if (s == nullptr) {
					return JS_EXCEPTION;
				}
				if (JS_ToBigInt64(ctx, &v, val)) {
					return JS_EXCEPTION;
				}
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
			) -> JSElement::bigint
			{
				static_assert(T == true or T == false, "T must be true or false");
				static_assert(sizeof(T) == sizeof(bool));
				M_JS_PROXY_WRAPPER(ctx, {
					try_assert(argc == 0, fmt::format("{} 0, {}: {}", Kernel::Language::get("kernel.argument_expected"), Kernel::Language::get("kernel.argument_received"), argc));
					auto s = static_cast<Data<T>*>(JS_GetOpaque2(ctx, this_val, ClassID<T>::value));
					if (s == nullptr) {
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
			) -> JSElement::bigint
			{
				static_assert(T == true or T == false, "T must be true or false");
				static_assert(sizeof(T) == sizeof(bool));
				M_JS_PROXY_WRAPPER(ctx, {
					try_assert(argc == 0, fmt::format("{} 0, {}: {}", Kernel::Language::get("kernel.argument_expected"), Kernel::Language::get("kernel.argument_received"), argc));
					auto s = static_cast<Data<T>*>(JS_GetOpaque2(ctx, this_val, ClassID<T>::value));
					if (s == nullptr) {
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
			) -> JSElement::undefined
			{
				static_assert(T == true or T == false, "T must be true or false");
				static_assert(sizeof(T) == sizeof(bool));
				M_JS_PROXY_WRAPPER(ctx, {
					try_assert(argc == 1, fmt::format("{} 1, {}: {}", Kernel::Language::get("kernel.argument_expected"), Kernel::Language::get("kernel.argument_received"), argc));
					auto s = static_cast<Data<T>*>(JS_GetOpaque2(ctx, this_val, ClassID<T>::value));
					if (s == nullptr) {
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
			) -> JSElement::undefined
			{
				static_assert(T == true or T == false, "T must be true or false");
				static_assert(sizeof(T) == sizeof(bool));
				M_JS_PROXY_WRAPPER(ctx, {
					try_assert(argc == 1, fmt::format("{} 1, {}: {}", Kernel::Language::get("kernel.argument_expected"), Kernel::Language::get("kernel.argument_received"), argc));
					auto s = static_cast<Data<T>*>(JS_GetOpaque2(ctx, this_val, ClassID<T>::value));
					if (s == nullptr) {
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
			) -> JSElement::ArrayBuffer
			{
				auto array_buffer = JS_NewArrayBufferCopy(ctx, vec.data(), vec.size());
				return array_buffer;
			}

			inline static auto to_uint8array(
				JSContext* ctx,
				const std::vector<uint8_t>& vec
			) -> JSElement::Uint8Array
			{
				auto array_buffer = JS_NewArrayBufferCopy(ctx, vec.data(), vec.size());
				auto global_obj = JS_GetGlobalObject(ctx);
				auto uint8array_ctor = JS_GetPropertyStr(ctx, global_obj, "Uint8Array");
				JSValue args[] = { array_buffer };
				auto uint8array = JS_CallConstructor(ctx, uint8array_ctor, 1, args);
				JS_FreeValue(ctx, global_obj);
				JS_FreeValue(ctx, uint8array_ctor);
				return uint8array;
			}

			#pragma endregion

			template <auto T> requires BooleanConstraint
			inline static auto toUint8Array(
				JSContext* ctx,
				JSValueConst this_val,
				int argc,
				JSValueConst* argv
			) -> JSElement::Uint8Array
			{
				static_assert(T == true or T == false, "T must be true or false");
				static_assert(sizeof(T) == sizeof(bool));
				M_JS_PROXY_WRAPPER(ctx, {
					try_assert(argc == 0, fmt::format("{} 0, {}: {}", Kernel::Language::get("kernel.argument_expected"), Kernel::Language::get("kernel.argument_received"), argc));
					auto s = static_cast<Data<T>*>(JS_GetOpaque2(ctx, this_val, ClassID<T>::value));
					if (s == nullptr) {
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
			) -> JSElement::ArrayBuffer
			{
				static_assert(T == true or T == false, "T must be true or false");
				static_assert(sizeof(T) == sizeof(bool));
				M_JS_PROXY_WRAPPER(ctx, {
					try_assert(argc == 0, fmt::format("{} 0, {}: {}", Kernel::Language::get("kernel.argument_expected"), Kernel::Language::get("kernel.argument_received"), argc));
					auto s = static_cast<Data<T>*>(JS_GetOpaque2(ctx, this_val, ClassID<T>::value));
					if (s == nullptr) {
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
			) -> JSElement::ArrayBuffer
			{
				static_assert(T == true or T == false, "T must be true or false");
				static_assert(sizeof(T) == sizeof(bool));
				M_JS_PROXY_WRAPPER(ctx, {
					try_assert(argc == 2, fmt::format("{} 2, {}: {}", Kernel::Language::get("kernel.argument_expected"), Kernel::Language::get("kernel.argument_received"), argc));
					auto s = static_cast<Data<T>*>(JS_GetOpaque2(ctx, this_val, ClassID<T>::value));
					if (s == nullptr) {
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
			) -> JSElement::Uint8Array
			{
				static_assert(T == true or T == false, "T must be true or false");
				static_assert(sizeof(T) == sizeof(bool));
				M_JS_PROXY_WRAPPER(ctx, {
					try_assert(argc == 2, fmt::format("{} 2, {}: {}", Kernel::Language::get("kernel.argument_expected"), Kernel::Language::get("kernel.argument_received"), argc));
					auto s = static_cast<Data<T>*>(JS_GetOpaque2(ctx, this_val, ClassID<T>::value));
					if (s == nullptr) {
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
			) -> JSElement::string
			{
				static_assert(T == true or T == false, "T must be true or false");
				static_assert(sizeof(T) == sizeof(bool));
				M_JS_PROXY_WRAPPER(ctx, {
				try_assert(argc == 0, fmt::format("{} 2, {}: {}", Kernel::Language::get("kernel.argument_expected"), Kernel::Language::get("kernel.argument_received"), argc));
					auto s = static_cast<Data<T>*>(JS_GetOpaque2(ctx, this_val, ClassID<T>::value));
					if (s == nullptr) {
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
			) -> JSElement::undefined
			{
				static_assert(T == true or T == false, "T must be true or false");
				static_assert(sizeof(T) == sizeof(bool));
				M_JS_PROXY_WRAPPER(ctx, {
					try_assert(argc == 1, fmt::format("{} 2, {}: {}", Kernel::Language::get("kernel.argument_expected"), Kernel::Language::get("kernel.argument_received"), argc));
					auto s = static_cast<Data<T>*>(JS_GetOpaque2(ctx, this_val, ClassID<T>::value));
					if (s == nullptr) {
						return JS_EXCEPTION;
					}
					s->out_file(JS::Converter::get_string(ctx, argv[0]));
					return JS_UNDEFINED;
				}, "out_file"_sv);
			}

			template <auto T> requires BooleanConstraint
			inline static auto allocate(
				JSContext* ctx,
				JSValueConst this_val,
				int argc,
				JSValueConst* argv
			) -> JSElement::undefined
			{
				static_assert(T == true or T == false, "T must be true or false");
				static_assert(sizeof(T) == sizeof(bool));
				M_JS_PROXY_WRAPPER(ctx, {
					try_assert(argc == 1, fmt::format("{} 2, {}: {}", Kernel::Language::get("kernel.argument_expected"), Kernel::Language::get("kernel.argument_received"), argc));
					auto s = static_cast<Data<T>*>(JS_GetOpaque2(ctx, this_val, ClassID<T>::value));
					if (s == nullptr) {
						return JS_EXCEPTION;
					}
					s->allocate(static_cast<std::size_t>(JS::Converter::get_bigint64(ctx, argv[0])));
					return JS_UNDEFINED;
				}, "allocate"_sv);
			}

			template <auto T> requires BooleanConstraint
			inline static auto writeUint8(
				JSContext* ctx,
				JSValueConst this_val,
				int argc,
				JSValueConst* argv
			) -> JSElement::undefined
			{
				static_assert(T == true or T == false, "T must be true or false");
				static_assert(sizeof(T) == sizeof(bool));
				M_JS_PROXY_WRAPPER(ctx, {
					try_assert(argc == 2 || argc == 1, fmt::format("argument expected 2 or 1, received: {}", argc));
					auto s = static_cast<Data<T>*>(JS_GetOpaque2(ctx, this_val, ClassID<T>::value));
					if (s == nullptr) {
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
			) -> JSElement::undefined
			{
				static_assert(T == true or T == false, "T must be true or false");
				static_assert(sizeof(T) == sizeof(bool));
				M_JS_PROXY_WRAPPER(ctx, {
					try_assert(argc == 2 || argc == 1, fmt::format("argument expected 2 or 1, received: {}", argc));
					auto s = static_cast<Data<T> *>(JS_GetOpaque2(ctx, this_val, ClassID<T>::value));
					if (s == nullptr) {
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
			) -> JSElement::undefined
			{
				static_assert(T == true or T == false, "T must be true or false");
				static_assert(sizeof(T) == sizeof(bool));
				M_JS_PROXY_WRAPPER(ctx, {
					try_assert(argc == 2 || argc == 1, fmt::format("argument expected 2 or 1, received: {}", argc));
					auto s = static_cast<Data<T>*>(JS_GetOpaque2(ctx, this_val, ClassID<T>::value));
					if (s == nullptr) {
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
			) -> JSElement::undefined
			{
				static_assert(T == true or T == false, "T must be true or false");
				static_assert(sizeof(T) == sizeof(bool));
				M_JS_PROXY_WRAPPER(ctx, {
					try_assert(argc == 2 || argc == 1, fmt::format("argument expected 2 or 1, received: {}", argc));
					auto s = static_cast<Data<T>*>(JS_GetOpaque2(ctx, this_val, ClassID<T>::value));
					if (s == nullptr) {
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
			) -> JSElement::undefined
			{
				static_assert(T == true or T == false, "T must be true or false");
				static_assert(sizeof(T) == sizeof(bool));
				M_JS_PROXY_WRAPPER(ctx, {
					try_assert(argc == 2 || argc == 1, fmt::format("argument expected 2 or 1, received: {}", argc));
					auto s = static_cast<Data<T>*>(JS_GetOpaque2(ctx, this_val, ClassID<T>::value));
					if (s == nullptr) {
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
			) -> JSElement::undefined
			{
				static_assert(T == true or T == false, "T must be true or false");
				static_assert(sizeof(T) == sizeof(bool));
				M_JS_PROXY_WRAPPER(ctx, {
					try_assert(argc == 2 || argc == 1, fmt::format("argument expected 2 or 1, received: {}", argc));
					auto s = static_cast<Data<T>*>(JS_GetOpaque2(ctx, this_val, ClassID<T>::value));
					if (s == nullptr) {
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
			) -> JSElement::undefined
			{
				static_assert(T == true or T == false, "T must be true or false");
				static_assert(sizeof(T) == sizeof(bool));
				M_JS_PROXY_WRAPPER(ctx, {
					try_assert(argc == 2 || argc == 1, fmt::format("argument expected 2 or 1, received: {}", argc));
					auto s = static_cast<Data<T>*>(JS_GetOpaque2(ctx, this_val, ClassID<T>::value));
					if (s == nullptr) {
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
			) -> JSElement::undefined
			{
				static_assert(T == true or T == false, "T must be true or false");
				static_assert(sizeof(T) == sizeof(bool));
				M_JS_PROXY_WRAPPER(ctx, {
					try_assert(argc == 2 || argc == 1, fmt::format("argument expected 2 or 1, received: {}", argc));
					auto s = static_cast<Data<T>*>(JS_GetOpaque2(ctx, this_val, ClassID<T>::value));
					if (s == nullptr) {
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
			) -> JSElement::undefined
			{
				static_assert(T == true or T == false, "T must be true or false");
				static_assert(sizeof(T) == sizeof(bool));
				M_JS_PROXY_WRAPPER(ctx, {
					try_assert(argc == 2 || argc == 1, fmt::format("argument expected 2 or 1, received: {}", argc));
					auto s = static_cast<Data<T>*>(JS_GetOpaque2(ctx, this_val, ClassID<T>::value));
					if (s == nullptr) {
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
			) -> JSElement::undefined
			{
				static_assert(T == true or T == false, "T must be true or false");
				static_assert(sizeof(T) == sizeof(bool));
				M_JS_PROXY_WRAPPER(ctx, {
					try_assert(argc == 2 || argc == 1, fmt::format("argument expected 2 or 1, received: {}", argc));
					auto s = static_cast<Data<T>*>(JS_GetOpaque2(ctx, this_val, ClassID<T>::value));
					if (s == nullptr) {
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
				assert_conditional(byte_len != 0, "ArrayBuffer is empty", "from_arraybuffer");
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
			) -> JSElement::undefined
			{
				static_assert(T == true or T == false, "T must be true or false");
				static_assert(sizeof(T) == sizeof(bool));
				M_JS_PROXY_WRAPPER(ctx, {
					try_assert(argc == 1 || argc == 2, fmt::format("argument expected 1 or 2, received: {}", argc));
					auto s = static_cast<Data<T>*>(JS_GetOpaque2(ctx, this_val, ClassID<T>::value));
					if (s == nullptr) {
						return JS_EXCEPTION;
					}
					if (argc == 2) {
						s->writeBytes(from_arraybuffer(ctx, argv[0]), static_cast<std::uint64_t>(JS::Converter::get_bigint64(ctx, argv[1])));
					}
					else {
						s->writeBytes(from_arraybuffer(ctx, argv[0]));
					}
					return JS_UNDEFINED;
				}, "writeArrayBuffer"_sv);
			}

			template <auto T> requires BooleanConstraint
			inline static auto writeUint8Array(
				JSContext* ctx,
				JSValueConst this_val,
				int argc,
				JSValueConst* argv
			) -> JSElement::undefined
			{
				static_assert(T == true or T == false, "T must be true or false");
				static_assert(sizeof(T) == sizeof(bool));
				M_JS_PROXY_WRAPPER(ctx, {
					try_assert(argc == 1 || argc == 2, fmt::format("argument expected 1 or 2, received: {}", argc));
					auto s = static_cast<Data<T>*>(JS_GetOpaque2(ctx, this_val, ClassID<T>::value));
					if (s == nullptr) {
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
			) -> JSElement::undefined
			{
				static_assert(T == true or T == false, "T must be true or false");
				static_assert(sizeof(T) == sizeof(bool));
				M_JS_PROXY_WRAPPER(ctx, {
					try_assert(argc == 2 || argc == 1, fmt::format("argument expected 2 or 1, received: {}", argc));
					auto s = static_cast<Data<T>*>(JS_GetOpaque2(ctx, this_val, ClassID<T>::value));
					if (s == nullptr) {
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
			) -> JSElement::undefined
			{
				static_assert(T == true or T == false, "T must be true or false");
				static_assert(sizeof(T) == sizeof(bool));
				M_JS_PROXY_WRAPPER(ctx, {
					try_assert(argc == 2 || argc == 1, fmt::format("argument expected 2 or 1, received: {}", argc));
					auto s = static_cast<Data<T>*>(JS_GetOpaque2(ctx, this_val, ClassID<T>::value));
					if (s == nullptr) {
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
			) -> JSElement::undefined
			{
				static_assert(T == true or T == false, "T must be true or false");
				static_assert(sizeof(T) == sizeof(bool));
				M_JS_PROXY_WRAPPER(ctx, {
					try_assert(argc == 2 || argc == 1, fmt::format("argument expected 2 or 1, received: {}", argc));
					auto s = static_cast<Data<T>*>(JS_GetOpaque2(ctx, this_val, ClassID<T>::value));
					if (s == nullptr) {
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
			) -> JSElement::undefined
			{
				static_assert(T == true or T == false, "T must be true or false");
				static_assert(sizeof(T) == sizeof(bool));
				M_JS_PROXY_WRAPPER(ctx, {
					try_assert(argc == 2 || argc == 1, fmt::format("argument expected 2 or 1, received: {}", argc));
					auto s = static_cast<Data<T>*>(JS_GetOpaque2(ctx, this_val, ClassID<T>::value));
					if (s == nullptr) {
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
			) -> JSElement::undefined
			{
				static_assert(T == true or T == false, "T must be true or false");
				static_assert(sizeof(T) == sizeof(bool));
				M_JS_PROXY_WRAPPER(ctx, {
					try_assert(argc == 2 || argc == 1, fmt::format("argument expected 2 or 1, received: {}", argc));
					auto s = static_cast<Data<T>*>(JS_GetOpaque2(ctx, this_val, ClassID<T>::value));
					if (s == nullptr) {
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
			) -> JSElement::undefined
			{
				static_assert(T == true or T == false, "T must be true or false");
				static_assert(sizeof(T) == sizeof(bool));
				M_JS_PROXY_WRAPPER(ctx, {
					try_assert(argc == 2 || argc == 1, fmt::format("argument expected 2 or 1, received: {}", argc));
					auto s = static_cast<Data<T>*>(JS_GetOpaque2(ctx, this_val, ClassID<T>::value));
					if (s == nullptr) {
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
			) -> JSElement::undefined
			{
				static_assert(T == true or T == false, "T must be true or false");
				static_assert(sizeof(T) == sizeof(bool));
				M_JS_PROXY_WRAPPER(ctx, {
					try_assert(argc == 2 || argc == 1, fmt::format("argument expected 2 or 1, received: {}", argc));
					auto s = static_cast<Data<T>*>(JS_GetOpaque2(ctx, this_val, ClassID<T>::value));
					if (s == nullptr) {
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
			) -> JSElement::undefined
			{
				static_assert(T == true or T == false, "T must be true or false");
				static_assert(sizeof(T) == sizeof(bool));
				M_JS_PROXY_WRAPPER(ctx, {
					try_assert(argc == 2 || argc == 1, fmt::format("argument expected 2 or 1, received: {}", argc));
					auto s = static_cast<Data<T>*>(JS_GetOpaque2(ctx, this_val, ClassID<T>::value));
					if (s == nullptr) {
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
			) -> JSElement::undefined
			{
				static_assert(T == true or T == false, "T must be true or false");
				static_assert(sizeof(T) == sizeof(bool));
				M_JS_PROXY_WRAPPER(ctx, {
					try_assert(argc == 2 || argc == 1, fmt::format("argument expected 2 or 1, received: {}", argc));
					auto s = static_cast<Data<T>*>(JS_GetOpaque2(ctx, this_val, ClassID<T>::value));
					if (s == nullptr) {
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
			) -> JSElement::undefined
			{
				static_assert(T == true or T == false, "T must be true or false");
				static_assert(sizeof(T) == sizeof(bool));
				M_JS_PROXY_WRAPPER(ctx, {
					try_assert(argc == 2 || argc == 1, fmt::format("argument expected 2 or 1, received: {}", argc));
					auto s = static_cast<Data<T>*>(JS_GetOpaque2(ctx, this_val, ClassID<T>::value));
					if (s == nullptr) {
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
			) -> JSElement::undefined
			{
				static_assert(T == true or T == false, "T must be true or false");
				static_assert(sizeof(T) == sizeof(bool));
				M_JS_PROXY_WRAPPER(ctx, {
					try_assert(argc == 2 || argc == 1, fmt::format("argument expected 2 or 1, received: {}", argc));
					auto s = static_cast<Data<T>*>(JS_GetOpaque2(ctx, this_val, ClassID<T>::value));
					if (s == nullptr) {
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
			) -> JSElement::undefined
			{
				static_assert(T == true or T == false, "T must be true or false");
				static_assert(sizeof(T) == sizeof(bool));
				M_JS_PROXY_WRAPPER(ctx, {
					try_assert(argc == 2 || argc == 1, fmt::format("argument expected 2 or 1, received: {}", argc));
					auto s = static_cast<Data<T>*>(JS_GetOpaque2(ctx, this_val, ClassID<T>::value));
					if (s == nullptr) {
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
			) -> JSElement::undefined
			{
				static_assert(T == true or T == false, "T must be true or false");
				static_assert(sizeof(T) == sizeof(bool));
				M_JS_PROXY_WRAPPER(ctx, {
					try_assert(argc == 2 || argc == 1, fmt::format("argument expected 2 or 1, received: {}", argc));
					auto s = static_cast<Data<T>*>(JS_GetOpaque2(ctx, this_val, ClassID<T>::value));
					if (s == nullptr) {
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
			) -> JSElement::undefined
			{
				static_assert(T == true or T == false, "T must be true or false");
				static_assert(sizeof(T) == sizeof(bool));
				M_JS_PROXY_WRAPPER(ctx, {
					try_assert(argc == 2 || argc == 1, fmt::format("argument expected 2 or 1, received: {}", argc));
					auto s = static_cast<Data<T>*>(JS_GetOpaque2(ctx, this_val, ClassID<T>::value));
					if (s == nullptr) {
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
			) -> JSElement::undefined
			{
				static_assert(T == true or T == false, "T must be true or false");
				static_assert(sizeof(T) == sizeof(bool));
				M_JS_PROXY_WRAPPER(ctx, {
					try_assert(argc == 2 || argc == 1, fmt::format("argument expected 2 or 1, received: {}", argc));
					auto s = static_cast<Data<T>*>(JS_GetOpaque2(ctx, this_val, ClassID<T>::value));
					if (s == nullptr) {
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
			) -> JSElement::undefined
			{
				static_assert(T == true or T == false, "T must be true or false");
				static_assert(sizeof(T) == sizeof(bool));
				M_JS_PROXY_WRAPPER(ctx, {
					try_assert(argc == 2 || argc == 1, fmt::format("argument expected 2 or 1, received: {}", argc));
					auto s = static_cast<Data<T>*>(JS_GetOpaque2(ctx, this_val, ClassID<T>::value));
					if (s == nullptr) {
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
			) -> JSElement::undefined
			{
				static_assert(T == true or T == false, "T must be true or false");
				static_assert(sizeof(T) == sizeof(bool));
				M_JS_PROXY_WRAPPER(ctx, {
					try_assert(argc == 2 || argc == 1, fmt::format("argument expected 2 or 1, received: {}", argc));
					auto s = static_cast<Data<T>*>(JS_GetOpaque2(ctx, this_val, ClassID<T>::value));
					if (s == nullptr) {
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
			) -> JSElement::bigint
			{
				static_assert(T == true or T == false, "T must be true or false");
				M_JS_PROXY_WRAPPER(ctx, {
					try_assert(argc == 0 || argc == 1, fmt::format("argument expected 0 or 1, received: {}", argc));
					auto s = static_cast<Data<T> *>(JS_GetOpaque2(ctx, this_val, ClassID<T>::value));
					auto v = uint8_t{};
					if (s != nullptr) {
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
			) -> JSElement::bigint
			{
				static_assert(T == true or T == false, "T must be true or false");
				M_JS_PROXY_WRAPPER(ctx, {
					try_assert(argc == 0 || argc == 1, fmt::format("argument expected 0 or 1, received: {}", argc));
					auto s = static_cast<Data<T>*>(JS_GetOpaque2(ctx, this_val, ClassID<T>::value));
					auto v = uint16_t{};
					if (s != nullptr) {
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
			) -> JSElement::bigint
			{
				static_assert(T == true or T == false, "T must be true or false");
				M_JS_PROXY_WRAPPER(ctx, {
					try_assert(argc == 0 || argc == 1, fmt::format("argument expected 0 or 1, received: {}", argc));
					auto s = static_cast<Data<T>*>(JS_GetOpaque2(ctx, this_val, ClassID<T>::value));
					auto v = uint32_t{};
					if (s != nullptr) {
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
			) -> JSElement::bigint
			{
				static_assert(T == true or T == false, "T must be true or false");
				M_JS_PROXY_WRAPPER(ctx, {
					try_assert(argc == 0 || argc == 1, fmt::format("argument expected 0 or 1, received: {}", argc));
					auto s = static_cast<Data<T>*>(JS_GetOpaque2(ctx, this_val, ClassID<T>::value));
					auto v = uint32_t{};
					if (s != nullptr) {
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
			) -> JSElement::bigint
			{
				static_assert(T == true or T == false, "T must be true or false");
				M_JS_PROXY_WRAPPER(ctx, {
					try_assert(argc == 0 || argc == 1, fmt::format("argument expected 0 or 1, received: {}", argc));
					auto s = static_cast<Data<T>*>(JS_GetOpaque2(ctx, this_val, ClassID<T>::value));
					auto v = uint64_t{};
					if (s != nullptr) {
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
			) -> JSElement::bigint
			{
				static_assert(T == true or T == false, "T must be true or false");
				M_JS_PROXY_WRAPPER(ctx, {
					try_assert(argc == 0 || argc == 1, fmt::format("argument expected 0 or 1, received: {}", argc));
					auto s = static_cast<Data<T>*>(JS_GetOpaque2(ctx, this_val, ClassID<T>::value));
					auto v = int8_t{};
					if (s != nullptr) {
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
			) -> JSElement::bigint
			{
				static_assert(T == true or T == false, "T must be true or false");
				M_JS_PROXY_WRAPPER(ctx, {
					try_assert(argc == 0 || argc == 1, fmt::format("argument expected 0 or 1, received: {}", argc));
					auto s = static_cast<Data<T>*>(JS_GetOpaque2(ctx, this_val, ClassID<T>::value));
					auto v = int16_t{};
					if (s != nullptr) {
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
			) -> JSElement::bigint
			{
				static_assert(T == true or T == false, "T must be true or false");
				M_JS_PROXY_WRAPPER(ctx, {
					try_assert(argc == 0 || argc == 1, fmt::format("argument expected 0 or 1, received: {}", argc));
					auto s = static_cast<Data<T>*>(JS_GetOpaque2(ctx, this_val, ClassID<T>::value));
					auto v = int32_t{};
					if (s != nullptr) {
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
			) -> JSElement::bigint
			{
				static_assert(T == true or T == false, "T must be true or false");
				M_JS_PROXY_WRAPPER(ctx, {
					try_assert(argc == 0 || argc == 1, fmt::format("argument expected 0 or 1, received: {}", argc));
					auto s = static_cast<Data<T>*>(JS_GetOpaque2(ctx, this_val, ClassID<T>::value));
					auto v = int32_t{};
					if (s != nullptr) {
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
			) -> JSElement::bigint
			{
				static_assert(T == true or T == false, "T must be true or false");
				M_JS_PROXY_WRAPPER(ctx, {
					try_assert(argc == 0 || argc == 1, fmt::format("argument expected 0 or 1, received: {}", argc));
					auto s = static_cast<Data<T>*>(JS_GetOpaque2(ctx, this_val, ClassID<T>::value));
					auto v = int64_t{};
					if (s != nullptr) {
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
			) -> JSElement::string
			{
				static_assert(T == true or T == false, "T must be true or false");
				M_JS_PROXY_WRAPPER(ctx, {
					try_assert(argc == 1 || argc == 2, fmt::format("argument expected 1 or 2, received: {}", argc));
					auto s = static_cast<Data<T>*>(JS_GetOpaque2(ctx, this_val, ClassID<T>::value));
					auto v = std::string{};
					if (s != nullptr) {
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
			) -> JSElement::string
			{
				static_assert(T == true or T == false, "T must be true or false");
				M_JS_PROXY_WRAPPER(ctx, {
					try_assert(argc == 0 || argc == 1, fmt::format("argument expected 0 or 1, received: {}", argc));
					auto s = static_cast<Data<T>*>(JS_GetOpaque2(ctx, this_val, ClassID<T>::value));
					auto v = std::string{};
					if (s != nullptr) {
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
			) -> JSElement::string
			{
				static_assert(T == true or T == false, "T must be true or false");
				M_JS_PROXY_WRAPPER(ctx, {
					try_assert(argc == 0 || argc == 1, fmt::format("argument expected 0 or 1, received: {}", argc));
					auto s = static_cast<Data<T>*>(JS_GetOpaque2(ctx, this_val, ClassID<T>::value));
					auto v = std::string{};
					if (s != nullptr) {
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
			) -> JSElement::string
			{
				static_assert(T == true or T == false, "T must be true or false");
				M_JS_PROXY_WRAPPER(ctx, {
					try_assert(argc == 0 || argc == 1, fmt::format("argument expected 0 or 1, received: {}", argc));
					auto s = static_cast<Data<T>*>(JS_GetOpaque2(ctx, this_val, ClassID<T>::value));
					auto v = std::string{};
					if (s != nullptr) {
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
			) -> JSElement::string
			{
				static_assert(T == true or T == false, "T must be true or false");
				M_JS_PROXY_WRAPPER(ctx, {
					try_assert(argc == 0 || argc == 1, fmt::format("argument expected 0 or 1, received: {}", argc));
					auto s = static_cast<Data<T>*>(JS_GetOpaque2(ctx, this_val, ClassID<T>::value));
					auto v = std::string{};
					if (s != nullptr) {
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
			) -> JSElement::string
			{
				static_assert(T == true or T == false, "T must be true or false");
				M_JS_PROXY_WRAPPER(ctx, {
					try_assert(argc == 0 || argc == 1, fmt::format("argument expected 0 or 1, received: {}", argc));
					auto s = static_cast<Data<T>*>(JS_GetOpaque2(ctx, this_val, ClassID<T>::value));
					auto v = std::string{};
					if (s != nullptr) {
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
			) -> JSElement::string
			{
				static_assert(T == true or T == false, "T must be true or false");
				M_JS_PROXY_WRAPPER(ctx, {
					try_assert(argc == 0 || argc == 1, fmt::format("argument expected 0 or 1, received: {}", argc));
					auto s = static_cast<Data<T>*>(JS_GetOpaque2(ctx, this_val, ClassID<T>::value));
					auto v = std::string{};
					if (s != nullptr) {
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
			) -> JSElement::string
			{
				static_assert(T == true or T == false, "T must be true or false");
				M_JS_PROXY_WRAPPER(ctx, {
					try_assert(argc == 0 || argc == 1, fmt::format("argument expected 0 or 1, received: {}", argc));
					auto s = static_cast<Data<T>*>(JS_GetOpaque2(ctx, this_val, ClassID<T>::value));
					auto v = std::string{};
					if (s != nullptr) {
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
			) -> JSElement::string
			{
				static_assert(T == true or T == false, "T must be true or false");
				M_JS_PROXY_WRAPPER(ctx, {
					try_assert(argc == 0 || argc == 1, fmt::format("argument expected 0 or 1, received: {}", argc));
					auto s = static_cast<Data<T>*>(JS_GetOpaque2(ctx, this_val, ClassID<T>::value));
					auto v = std::string{};
					if (s != nullptr) {
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
			) -> JSElement::bigint
			{
				static_assert(T == true or T == false, "T must be true or false");
				M_JS_PROXY_WRAPPER(ctx, {
					try_assert(argc == 0 || argc == 1, fmt::format("argument expected 0 or 1, received: {}", argc));
					auto s = static_cast<Data<T>*>(JS_GetOpaque2(ctx, this_val, ClassID<T>::value));
					auto v = int32_t{};
					if (s != nullptr) {
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
			) -> JSElement::bigint
			{
				static_assert(T == true or T == false, "T must be true or false");
				M_JS_PROXY_WRAPPER(ctx, {
					try_assert(argc == 0 || argc == 1, fmt::format("argument expected 0 or 1, received: {}", argc));
					auto s = static_cast<Data<T>*>(JS_GetOpaque2(ctx, this_val, ClassID<T>::value));
					auto v = int64_t{};
					if (s == nullptr) {
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
			) -> JSElement::bigint
			{
				static_assert(T == true or T == false, "T must be true or false");
				M_JS_PROXY_WRAPPER(ctx, {
					try_assert(argc == 0 || argc == 1, fmt::format("argument expected 0 or 1, received: {}", argc));
					auto s = static_cast<Data<T>*>(JS_GetOpaque2(ctx, this_val, ClassID<T>::value));
					auto v = uint32_t{};
					if (s == nullptr) {
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
			) -> JSElement::bigint
			{
				static_assert(T == true or T == false, "T must be true or false");
				M_JS_PROXY_WRAPPER(ctx, {
					try_assert(argc == 0 || argc == 1, fmt::format("argument expected 0 or 1, received: {}", argc));
					auto s = static_cast<Data<T>*>(JS_GetOpaque2(ctx, this_val, ClassID<T>::value));
					auto v = uint64_t{};
					if (s != nullptr) {
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
			) -> JSElement::bigint
			{
				static_assert(T == true or T == false, "T must be true or false");
				M_JS_PROXY_WRAPPER(ctx, {
					try_assert(argc == 0 || argc == 1, fmt::format("argument expected 0 or 1, received: {}", argc));
					auto s = static_cast<Data<T>*>(JS_GetOpaque2(ctx, this_val, ClassID<T>::value));
					auto v = int32_t{};
					if (s != nullptr) {
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
			) -> JSElement::bigint
			{
				static_assert(T == true or T == false, "T must be true or false");
				M_JS_PROXY_WRAPPER(ctx, {
					try_assert(argc == 0 || argc == 1, fmt::format("argument expected 0 or 1, received: {}", argc));
					auto s = static_cast<Data<T>*>(JS_GetOpaque2(ctx, this_val, ClassID<T>::value));
					auto v = int64_t{};
					if (s != nullptr) {
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
			) -> JSElement::number
			{
				static_assert(T == true or T == false, "T must be true or false");
				M_JS_PROXY_WRAPPER(ctx, {
					try_assert(argc == 0 || argc == 1, fmt::format("argument expected 0 or 1, received: {}", argc));
					auto s = static_cast<Data<T>*>(JS_GetOpaque2(ctx, this_val, ClassID<T>::value));
					auto v = float{};
					if (s != nullptr) {
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
			) -> JSElement::number
			{
				static_assert(T == true or T == false, "T must be true or false");
				M_JS_PROXY_WRAPPER(ctx, {
					try_assert(argc == 0 || argc == 1, fmt::format("argument expected 0 or 1, received: {}", argc));
					auto s = static_cast<Data<T>*>(JS_GetOpaque2(ctx, this_val, ClassID<T>::value));
					auto v = double{};
					if (s != nullptr) {
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
			) -> JSElement::undefined
			{
				static_assert(T == true or T == false, "T must be true or false");
				M_JS_PROXY_WRAPPER(ctx, {
					try_assert(argc == 0, fmt::format("argument expected 0, received: {}", argc));
					auto s = static_cast<Data<T>*>(JS_GetOpaque2(ctx, this_val, ClassID<T>::value));
					if (s != nullptr) {
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
					JS_CPPFUNC_DEF("allocate", 1, allocate<T>),
			};


			template <auto use_big_endian> requires BooleanConstraint
			inline static auto register_class(
				JSContext* ctx
			) -> void
			{
				static_assert(use_big_endian == true || use_big_endian == false, "use_big_endian can only be true or false");
				static_assert(sizeof(use_big_endian) == sizeof(bool));
				JS_NewClassID(&ClassID<use_big_endian>::value);
				assert_conditional(JS_NewClass(JS_GetRuntime(ctx), ClassID<use_big_endian>::value, &this_class<use_big_endian>) == 0, "DataStreamView class register failed", "register_class");
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
			) -> JSElement::undefined
			{
				M_JS_PROXY_WRAPPER(ctx, {
					auto s = static_cast<Data*>(nullptr);
					auto obj = JS_UNDEFINED;
					auto proto = JSElement::Prototype{};
					if (argc == 1) {
						s = new Data(JS_VALUE_GET_BOOL(argv[0]) == 0 ? false : true);
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
				}, "proxy_constructor");
			}

			inline static auto this_class = JSClassDef{
				.class_name = "Boolean",
				.finalizer = finalizer,
			};

			inline static auto getter(
				JSContext* ctx,
				JSValueConst this_val,
				int magic
			) -> JSElement::boolean
			{
				auto s = static_cast<Data*>(JS_GetOpaque2(ctx, this_val, class_id));
				if (s == nullptr) {
					return JS_EXCEPTION;
				}
				return JS::Converter::to_bool(ctx, s->value);
			}

			inline static auto setter(
				JSContext* ctx,
				JSValueConst this_val,
				JSValueConst val,
				int magic
			) -> JSElement::undefined
			{
				auto s = static_cast<Data*>(JS_GetOpaque2(ctx, this_val, class_id));
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
			) -> JSDefine::Boolean
			{
				static_assert(T == true or T == false, "T must be true or false");
				static_assert(sizeof(T) == sizeof(Data));
				auto s = new Data(T);
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
				JS_NewClassID(&class_id);
				assert_conditional(JS_NewClass(JS_GetRuntime(ctx), class_id, &this_class) == 0, "Boolean class register failed", "register_class");
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

		namespace FileInputStream {

			using Data = Kernel::FileSystem::FileHandler;

			inline static JSClassID class_id;

			inline static auto finalizer(
				JSRuntime* rt,
				JSValue val
			) -> void
			{
				auto s = static_cast<Data*>(JS_GetOpaque(val, class_id));
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
			) -> JSElement::undefined
			{
				M_JS_PROXY_WRAPPER(ctx, {
					auto s = static_cast<Data*>(nullptr);
					auto obj = JS_UNDEFINED;
					auto proto = JSElement::Prototype{};
					if (argc == 1) {
						auto source = JS::Converter::get_string(ctx, argv[0]);
						s = new Data(source, "rb");
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
					if (JS_IsException(obj)) {
						goto fail;
					}
					JS_SetOpaque(obj, s);
					return obj;
				fail:
					js_free(ctx, s);
					JS_FreeValue(ctx, obj);
					return JS_EXCEPTION;
				}, "proxy_register");
			}

			inline static auto this_class = JSClassDef{
				.class_name = "FileInputStream",
				.finalizer = finalizer,
			};

			/*
				close
			*/

			inline static auto close (
				JSContext* ctx,
				JSValueConst this_val,
				int argc,
				JSValueConst* argv
			) -> JSElement::undefined
			{
				M_JS_PROXY_WRAPPER(ctx, {
					try_assert(argc == 0, fmt::format("{} 0, {}: {}", Kernel::Language::get("kernel.argument_expected"), Kernel::Language::get("kernel.argument_received"), argc));
					auto s = static_cast<Data*>(JS_GetOpaque2(ctx, this_val, class_id));
					if (s == nullptr) {
						return JS_EXCEPTION;
					}
					s->close();
					return JS_UNDEFINED;
				}, "close"_sv);
			}


			inline static auto size(
				JSContext* ctx,
				JSValueConst this_val,
				int argc,
				JSValueConst* argv
			) -> JSElement::undefined
			{
				M_JS_PROXY_WRAPPER(ctx, {
					try_assert(argc == 0, fmt::format("{} 0, {}: {}", Kernel::Language::get("kernel.argument_expected"), Kernel::Language::get("kernel.argument_received"), argc));
					auto s = static_cast<Data*>(JS_GetOpaque2(ctx, this_val, class_id));
					if (s == nullptr) {
						return JS_EXCEPTION;
					}
					return JS::Converter::to_bigint(ctx, s->size());
				}, "size");
			}

			inline static auto read(
				JSContext* ctx,
				JSValueConst this_val,
				int argc,
				JSValueConst* argv
			) -> JSElement::undefined
			{
				M_JS_PROXY_WRAPPER(ctx, {
					try_assert(argc == 0, fmt::format("{} 0, {}: {}", Kernel::Language::get("kernel.argument_expected"), Kernel::Language::get("kernel.argument_received"), argc));
					auto s = static_cast<Data*>(JS_GetOpaque2(ctx, this_val, class_id));
					if (s == nullptr) {
						return JS_EXCEPTION;
					}
					return JS::Converter::to_bigint(ctx, static_cast<std::int64_t>(s->read()));
				}, "read");
			}

			inline static auto read_all(
				JSContext* ctx,
				JSValueConst this_val,
				int argc,
				JSValueConst* argv
			) -> JSValue
			{
				M_JS_PROXY_WRAPPER(ctx, {
					try_assert(argc == 0, fmt::format("{} 0, {}: {}", Kernel::Language::get("kernel.argument_expected"), Kernel::Language::get("kernel.argument_received"), argc));
					auto s = static_cast<Data*>(JS_GetOpaque2(ctx, this_val, class_id));
					if (s == nullptr) {
						return JS_EXCEPTION;
					}
					auto data = s->read_all();
					return JS_NewArrayBufferCopy(ctx, data.data(), data.size());
				}, "read_all");
			}

			inline static auto getter(
				JSContext* ctx,
				JSValueConst this_val,
				int magic
			) -> JSElement::bigint
			{
				auto s = static_cast<Data*>(JS_GetOpaque2(ctx, this_val, class_id));
				if (s == nullptr) {
					return JS_EXCEPTION;
				}
				return JS::Converter::to_bigint(ctx, s->position());
			}

			inline static auto setter(
				JSContext* ctx,
				JSValueConst this_val,
				JSValueConst val,
				int magic
			) -> JSElement::undefined
			{
				auto s = static_cast<Data*>(JS_GetOpaque2(ctx, this_val, class_id));
				if (s == nullptr) {
					return JS_EXCEPTION;
				}
				s->position(static_cast<std::size_t>(JS::Converter::get_bigint64(ctx, val)));
				return JS_UNDEFINED;
			}

			inline static const JSCFunctionListEntry proto_functions[] = {
				JS_CPPGETSET_MAGIC_DEF("position", getter, setter, 0),
				JS_CPPFUNC_DEF("close", 0, close),
				JS_CPPFUNC_DEF("size", 0, size),
				JS_CPPFUNC_DEF("read", 0, read),
				JS_CPPFUNC_DEF("read_all", 0, read_all),
			};

			inline static auto register_class(
				JSContext* ctx
			) -> void
			{
				JS_NewClassID(&class_id);
				assert_conditional(JS_NewClass(JS_GetRuntime(ctx), class_id, &this_class) == 0, "FileInputStream class register failed", "register_class");
				auto class_name = "FileInputStream"_sv;
				auto point_ctor = JS_NewCFunction2(ctx, constructor, class_name.data(), 2, JS_CFUNC_constructor, 0);
				auto proto = JS_NewObject(ctx);
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

		namespace FileOutputStream {

			using Data = Kernel::FileSystem::FileHandler;

			inline static JSClassID class_id;

			inline static auto finalizer(
				JSRuntime* rt,
				JSValue val
			) -> void
			{
				auto s = static_cast<Data*>(JS_GetOpaque(val, class_id));
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
			) -> JSElement::undefined
			{
				M_JS_PROXY_WRAPPER(ctx, {
					auto s = static_cast<Data*>(nullptr);
					auto obj = JS_UNDEFINED;
					auto proto = JSElement::Prototype{};
					if (argc == 1) {
						auto source = JS::Converter::get_string(ctx, argv[0]);
						s = new Data(source, "wb");
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
					if (JS_IsException(obj)) {
						goto fail;
					}
					JS_SetOpaque(obj, s);
					return obj;
				fail:
					js_free(ctx, s);
					JS_FreeValue(ctx, obj);
					return JS_EXCEPTION;
					}, "proxy_register");
			}

			inline static auto this_class = JSClassDef{
				.class_name = "FileOutputStream",
				.finalizer = finalizer,
			};

			/*
				close
			*/

			inline static auto close(
				JSContext* ctx,
				JSValueConst this_val,
				int argc,
				JSValueConst* argv
			) -> JSElement::undefined
			{
				M_JS_PROXY_WRAPPER(ctx, {
					try_assert(argc == 0, fmt::format("{} 0, {}: {}", Kernel::Language::get("kernel.argument_expected"), Kernel::Language::get("kernel.argument_received"), argc));
					auto s = static_cast<Data*>(JS_GetOpaque2(ctx, this_val, class_id));
					if (s == nullptr) {
						return JS_EXCEPTION;
					}
					s->close();
					return JS_UNDEFINED;
				}, "close"_sv);
			}


			inline static auto size(
				JSContext* ctx,
				JSValueConst this_val,
				int argc,
				JSValueConst* argv
			) -> JSElement::undefined
			{
				M_JS_PROXY_WRAPPER(ctx, {
					try_assert(argc == 0, fmt::format("{} 0, {}: {}", Kernel::Language::get("kernel.argument_expected"), Kernel::Language::get("kernel.argument_received"), argc));
					auto s = static_cast<Data*>(JS_GetOpaque2(ctx, this_val, class_id));
					if (s == nullptr) {
						return JS_EXCEPTION;
					}
					return JS::Converter::to_bigint(ctx, s->size());
				}, "size");
			}

			inline static auto write(
				JSContext* ctx,
				JSValueConst this_val,
				int argc,
				JSValueConst* argv
			) -> JSElement::undefined
			{
				M_JS_PROXY_WRAPPER(ctx, {
					try_assert(argc == 1, fmt::format("{} 1, {}: {}", Kernel::Language::get("kernel.argument_expected"), Kernel::Language::get("kernel.argument_received"), argc));
					auto s = static_cast<Data*>(JS_GetOpaque2(ctx, this_val, class_id));
					if (s == nullptr) {
						return JS_EXCEPTION;
					}
					s->write(static_cast<char>(JS::Converter::get_bigint64(ctx, argv[0])));
					return JS_UNDEFINED;
				}, "write");
			}

			inline static auto write_all(
				JSContext* ctx,
				JSValueConst this_val,
				int argc,
				JSValueConst* argv
			) -> JSValue
			{
				M_JS_PROXY_WRAPPER(ctx, {
					try_assert(argc == 1, fmt::format("{} 1, {}: {}", Kernel::Language::get("kernel.argument_expected"), Kernel::Language::get("kernel.argument_received"), argc));
					auto s = static_cast<Data*>(JS_GetOpaque2(ctx, this_val, class_id));
					if (s == nullptr) {
						return JS_EXCEPTION;
					}
					s->write_all(JS::Converter::to_binary_list(ctx, argv[0]));
					return JS_UNDEFINED;
				}, "write_all");
			}

			inline static auto getter(
				JSContext* ctx,
				JSValueConst this_val,
				int magic
			) -> JSElement::bigint
			{
				auto s = static_cast<Data*>(JS_GetOpaque2(ctx, this_val, class_id));
				if (s == nullptr) {
					return JS_EXCEPTION;
				}
				return JS::Converter::to_bigint(ctx, s->position());
			}

			inline static auto setter(
				JSContext* ctx,
				JSValueConst this_val,
				JSValueConst val,
				int magic
			) -> JSElement::undefined
			{
				auto s = static_cast<Data*>(JS_GetOpaque2(ctx, this_val, class_id));
				if (s == nullptr) {
					return JS_EXCEPTION;
				}
				s->position(static_cast<std::size_t>(JS::Converter::get_bigint64(ctx, val)));
				return JS_UNDEFINED;
			}

			inline static const JSCFunctionListEntry proto_functions[] = {
				JS_CPPGETSET_MAGIC_DEF("position", getter, setter, 0),
				JS_CPPFUNC_DEF("close", 0, close),
				JS_CPPFUNC_DEF("size", 0, size),
				JS_CPPFUNC_DEF("write", 0, write),
				JS_CPPFUNC_DEF("write_all", 0, write_all),
			};

			inline static auto register_class(
				JSContext* ctx
			) -> void
			{
				JS_NewClassID(&class_id);
				assert_conditional(JS_NewClass(JS_GetRuntime(ctx), class_id, &this_class) == 0, "FileOutputStream class register failed", "register_class");
				auto class_name = "FileOutputStream"_sv;
				auto point_ctor = JS_NewCFunction2(ctx, constructor, class_name.data(), 2, JS_CFUNC_constructor, 0);
				auto proto = JS_NewObject(ctx);
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

		namespace FileStream {

			using Data = Kernel::FileSystem::FileHandler;

			inline static JSClassID class_id;

			inline static auto finalizer(
				JSRuntime* rt,
				JSValue val
			) -> void
			{
				auto s = static_cast<Data*>(JS_GetOpaque(val, class_id));
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
			) -> JSElement::undefined
			{
				M_JS_PROXY_WRAPPER(ctx, {
					auto s = static_cast<Data*>(nullptr);
					auto obj = JS_UNDEFINED;
					auto proto = JSElement::Prototype{};
					if (argc == 1) {
						auto source = JS::Converter::get_string(ctx, argv[0]);
						s = new Data(source, "w+b");
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
					if (JS_IsException(obj)) {
						goto fail;
					}
					JS_SetOpaque(obj, s);
					return obj;
				fail:
					js_free(ctx, s);
					JS_FreeValue(ctx, obj);
					return JS_EXCEPTION;
					}, "proxy_register");
			}

			inline static auto this_class = JSClassDef{
				.class_name = "FileStream",
				.finalizer = finalizer,
			};

			/*
				close
			*/

			inline static auto close(
				JSContext* ctx,
				JSValueConst this_val,
				int argc,
				JSValueConst* argv
			) -> JSElement::undefined
			{
				M_JS_PROXY_WRAPPER(ctx, {
					try_assert(argc == 0, fmt::format("{} 0, {}: {}", Kernel::Language::get("kernel.argument_expected"), Kernel::Language::get("kernel.argument_received"), argc));
					auto s = static_cast<Data*>(JS_GetOpaque2(ctx, this_val, class_id));
					if (s == nullptr) {
						return JS_EXCEPTION;
					}
					s->close();
					return JS_UNDEFINED;
				}, "close"_sv);
			}


			inline static auto size(
				JSContext* ctx,
				JSValueConst this_val,
				int argc,
				JSValueConst* argv
			) -> JSElement::undefined
			{
				M_JS_PROXY_WRAPPER(ctx, {
					try_assert(argc == 0, fmt::format("{} 0, {}: {}", Kernel::Language::get("kernel.argument_expected"), Kernel::Language::get("kernel.argument_received"), argc));
					auto s = static_cast<Data*>(JS_GetOpaque2(ctx, this_val, class_id));
					if (s == nullptr) {
						return JS_EXCEPTION;
					}
					return JS::Converter::to_bigint(ctx, s->size());
				}, "size");
			}

			inline static auto write(
				JSContext* ctx,
				JSValueConst this_val,
				int argc,
				JSValueConst* argv
			) -> JSElement::undefined
			{
				M_JS_PROXY_WRAPPER(ctx, {
					try_assert(argc == 1, fmt::format("{} 1, {}: {}", Kernel::Language::get("kernel.argument_expected"), Kernel::Language::get("kernel.argument_received"), argc));
					auto s = static_cast<Data*>(JS_GetOpaque2(ctx, this_val, class_id));
					if (s == nullptr) {
						return JS_EXCEPTION;
					}
					s->write(static_cast<char>(JS::Converter::get_bigint64(ctx, argv[0])));
					return JS_UNDEFINED;
				}, "write");
			}

			inline static auto write_all(
				JSContext* ctx,
				JSValueConst this_val,
				int argc,
				JSValueConst* argv
			) -> JSValue
			{
				M_JS_PROXY_WRAPPER(ctx, {
					try_assert(argc == 1, fmt::format("{} 1, {}: {}", Kernel::Language::get("kernel.argument_expected"), Kernel::Language::get("kernel.argument_received"), argc));
					auto s = static_cast<Data*>(JS_GetOpaque2(ctx, this_val, class_id));
					if (s == nullptr) {
						return JS_EXCEPTION;
					}
					s->write_all(JS::Converter::to_binary_list(ctx, argv[0]));
					return JS_UNDEFINED;
				}, "write_all");
			}

			inline static auto read(
				JSContext* ctx,
				JSValueConst this_val,
				int argc,
				JSValueConst* argv
			) -> JSElement::undefined
			{
				M_JS_PROXY_WRAPPER(ctx, {
					try_assert(argc == 0, fmt::format("{} 0, {}: {}", Kernel::Language::get("kernel.argument_expected"), Kernel::Language::get("kernel.argument_received"), argc));
					auto s = static_cast<Data*>(JS_GetOpaque2(ctx, this_val, class_id));
					if (s == nullptr) {
						return JS_EXCEPTION;
					}
					return JS::Converter::to_bigint(ctx, static_cast<std::int64_t>(s->read()));
				}, "read");
			}

			inline static auto read_all(
				JSContext* ctx,
				JSValueConst this_val,
				int argc,
				JSValueConst* argv
			) -> JSValue
			{
				M_JS_PROXY_WRAPPER(ctx, {
					try_assert(argc == 0, fmt::format("{} 0, {}: {}", Kernel::Language::get("kernel.argument_expected"), Kernel::Language::get("kernel.argument_received"), argc));
					auto s = static_cast<Data*>(JS_GetOpaque2(ctx, this_val, class_id));
					if (s == nullptr) {
						return JS_EXCEPTION;
					}
					auto data = s->read_all();
					return JS_NewArrayBufferCopy(ctx, data.data(), data.size());
				}, "read_all");
			}

			inline static auto getter(
				JSContext* ctx,
				JSValueConst this_val,
				int magic
			) -> JSElement::bigint
			{
				auto s = static_cast<Data*>(JS_GetOpaque2(ctx, this_val, class_id));
				if (s == nullptr) {
					return JS_EXCEPTION;
				}
				return JS::Converter::to_bigint(ctx, s->position());
			}

			inline static auto setter(
				JSContext* ctx,
				JSValueConst this_val,
				JSValueConst val,
				int magic
			) -> JSElement::undefined
			{
				auto s = static_cast<Data*>(JS_GetOpaque2(ctx, this_val, class_id));
				if (s == nullptr) {
					return JS_EXCEPTION;
				}
				s->position(static_cast<std::size_t>(JS::Converter::get_bigint64(ctx, val)));
				return JS_UNDEFINED;
			}

			inline static const JSCFunctionListEntry proto_functions[] = {
				JS_CPPGETSET_MAGIC_DEF("position", getter, setter, 0),
				JS_CPPFUNC_DEF("close", 0, close),
				JS_CPPFUNC_DEF("size", 0, size),
				JS_CPPFUNC_DEF("write", 0, write),
				JS_CPPFUNC_DEF("write_all", 0, write_all),
				JS_CPPFUNC_DEF("read", 0, read),
				JS_CPPFUNC_DEF("read_all", 0, read_all),
			};

			inline static auto register_class(
				JSContext* ctx
			) -> void
			{
				JS_NewClassID(&class_id);
				assert_conditional(JS_NewClass(JS_GetRuntime(ctx), class_id, &this_class) == 0, "FileStream class register failed", "register_class");
				auto class_name = "FileStream"_sv;
				auto point_ctor = JS_NewCFunction2(ctx, constructor, class_name.data(), 2, JS_CFUNC_constructor, 0);
				auto proto = JS_NewObject(ctx);
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

		namespace JsonWriter {

			using Data = Kernel::Definition::JsonWriter;

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
			) -> JSElement::undefined
			{
				M_JS_PROXY_WRAPPER(ctx, {
					auto s = static_cast<Data*>(nullptr);
					auto obj = JS_UNDEFINED;
					auto proto = JSElement::Prototype{};
					if (argc == 0) {
						s = new Data();
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
					if (JS_IsException(obj)) {
						goto fail;
					}
					JS_SetOpaque(obj, s);
					return obj;
				fail:
					js_free(ctx, s);
					JS_FreeValue(ctx, obj);
					return JS_EXCEPTION;
				}, "proxy_constructor");
			}

			inline static auto this_class = JSClassDef{
				.class_name = "JsonWriter",
				.finalizer = finalizer,
			};

			/*
				clear
			*/

			inline static auto clear (
				JSContext* ctx,
				JSValueConst this_val,
				int argc,
				JSValueConst* argv
			) -> JSElement::undefined
			{
				M_JS_PROXY_WRAPPER(ctx, {
					try_assert(argc == 0, fmt::format("{} 0, {}: {}", Kernel::Language::get("kernel.argument_expected"), Kernel::Language::get("kernel.argument_received"), argc));
					auto s = static_cast<Data*>(JS_GetOpaque2(ctx, this_val, class_id));
					if (s == nullptr) {
						return JS_EXCEPTION;
					}
					s->Clear();
					return JS_UNDEFINED;
				}, "clear"_sv);
			}

			/*
				toString
			*/

			inline static auto toString(
				JSContext* ctx,
				JSValueConst this_val,
				int argc,
				JSValueConst* argv
			) -> JSElement::string
			{
				M_JS_PROXY_WRAPPER(ctx, {
					try_assert(argc == 0, fmt::format("{} 0, {}: {}", Kernel::Language::get("kernel.argument_expected"), Kernel::Language::get("kernel.argument_received"), argc));
					auto s = static_cast<Data*>(JS_GetOpaque2(ctx, this_val, class_id));
					if (s == nullptr) {
						return JS_EXCEPTION;
					}
					return JS::Converter::to_string(ctx, s->ToString());
				}, "toString"_sv);
			}

			/*
				writeStartArray
			*/

			inline static auto writeStartArray(
				JSContext* ctx,
				JSValueConst this_val,
				int argc,
				JSValueConst* argv
			) -> JSElement::undefined
			{
				M_JS_PROXY_WRAPPER(ctx, {
					try_assert(argc == 0, fmt::format("{} 0, {}: {}", Kernel::Language::get("kernel.argument_expected"), Kernel::Language::get("kernel.argument_received"), argc));
					auto s = static_cast<Data*>(JS_GetOpaque2(ctx, this_val, class_id));
					if (s == nullptr) {
						return JS_EXCEPTION;
					}
					s->WriteStartArray();
					return JS_UNDEFINED;
				}, "writeStartArray"_sv);
			}

			/*
				writeEndArray
			*/

			inline static auto writeEndArray(
				JSContext* ctx,
				JSValueConst this_val,
				int argc,
				JSValueConst* argv
			) -> JSElement::undefined
			{
				M_JS_PROXY_WRAPPER(ctx, {
					try_assert(argc == 0, fmt::format("{} 0, {}: {}", Kernel::Language::get("kernel.argument_expected"), Kernel::Language::get("kernel.argument_received"), argc));
					auto s = static_cast<Data*>(JS_GetOpaque2(ctx, this_val, class_id));
					if (s == nullptr) {
						return JS_EXCEPTION;
					}
					s->WriteEndArray();
					return JS_UNDEFINED;
				}, "writeEndArray"_sv);
			}

			/*
				writeStartObject
			*/

			inline static auto writeStartObject(
				JSContext* ctx,
				JSValueConst this_val,
				int argc,
				JSValueConst* argv
			) -> JSElement::undefined
			{
				M_JS_PROXY_WRAPPER(ctx, {
					try_assert(argc == 0, fmt::format("{} 0, {}: {}", Kernel::Language::get("kernel.argument_expected"), Kernel::Language::get("kernel.argument_received"), argc));
					auto s = static_cast<Data*>(JS_GetOpaque2(ctx, this_val, class_id));
					if (s == nullptr) {
						return JS_EXCEPTION;
					}
					s->WriteStartObject();
					return JS_UNDEFINED;
				}, "writeStartObject"_sv);
			}

			/*
				writeEndObject
			*/

			inline static auto writeEndObject(
				JSContext* ctx,
				JSValueConst this_val,
				int argc,
				JSValueConst* argv
			) -> JSElement::undefined
			{
				M_JS_PROXY_WRAPPER(ctx, {
					try_assert(argc == 0, fmt::format("{} 0, {}: {}", Kernel::Language::get("kernel.argument_expected"), Kernel::Language::get("kernel.argument_received"), argc));
					auto s = static_cast<Data*>(JS_GetOpaque2(ctx, this_val, class_id));
					if (s == nullptr) {
						return JS_EXCEPTION;
					}
					s->WriteEndObject();
					return JS_UNDEFINED;
				}, "writeEndObject"_sv);
			}

			/*
				writeBoolean
			*/

			inline static auto writeBoolean(
				JSContext* ctx,
				JSValueConst this_val,
				int argc,
				JSValueConst* argv
			) -> JSElement::undefined
			{
				M_JS_PROXY_WRAPPER(ctx, {
					try_assert(argc == 1, fmt::format("{} 1, {}: {}", Kernel::Language::get("kernel.argument_expected"), Kernel::Language::get("kernel.argument_received"), argc));
					auto s = static_cast<Data*>(JS_GetOpaque2(ctx, this_val, class_id));
					if (s == nullptr) {
						return JS_EXCEPTION;
					}
					s->WriteBoolean(JS::Converter::get_bool(ctx, argv[0]));
					return JS_UNDEFINED;
				}, "writeBoolean"_sv);
			}

			/*
				writeNull
			*/

			inline static auto writeNull(
				JSContext* ctx,
				JSValueConst this_val,
				int argc,
				JSValueConst* argv
			) -> JSElement::undefined
			{
				M_JS_PROXY_WRAPPER(ctx, {
					try_assert(argc == 0, fmt::format("{} 0, {}: {}", Kernel::Language::get("kernel.argument_expected"), Kernel::Language::get("kernel.argument_received"), argc));
					auto s = static_cast<Data*>(JS_GetOpaque2(ctx, this_val, class_id));
					if (s == nullptr) {
						return JS_EXCEPTION;
					}
					s->WriteNull();
					return JS_UNDEFINED;
				}, "writeNull"_sv);
			}

			/*
				writePropertyName
			*/

			inline static auto writePropertyName(
				JSContext* ctx,
				JSValueConst this_val,
				int argc,
				JSValueConst* argv
			) -> JSElement::undefined
			{
				M_JS_PROXY_WRAPPER(ctx, {
					try_assert(argc == 1, fmt::format("{} 1, {}: {}", Kernel::Language::get("kernel.argument_expected"), Kernel::Language::get("kernel.argument_received"), argc));
					auto s = static_cast<Data*>(JS_GetOpaque2(ctx, this_val, class_id));
					if (s == nullptr) {
						return JS_EXCEPTION;
					}
					s->WritePropertyName(JS::Converter::get_string(ctx, argv[0]));
					return JS_UNDEFINED;
				}, "writePropertyName"_sv);
			}

			/*
				write value
			*/
			template <typename T> requires std::is_arithmetic<T>::value or std::is_same<T, std::string>::value
			inline static auto writeValue(
				JSContext* ctx,
				JSValueConst this_val,
				int argc,
				JSValueConst* argv
			) -> JSElement::undefined
			{
				auto function_name = std::string_view{};
				if constexpr (std::is_same<T, std::string>::value) {
					function_name = "writeString"_sv;
				}
				if constexpr (std::is_integral<T>::value) {
					function_name = "writeBigInt"_sv;
				}
				if constexpr (std::is_floating_point<T>::value) {
					function_name = "writeNumber"_sv;
				}
				M_JS_PROXY_WRAPPER(ctx, {
					try_assert(argc == 1, fmt::format("{} 1, {}: {}", Kernel::Language::get("kernel.argument_expected"), Kernel::Language::get("kernel.argument_received"), argc));
					auto s = static_cast<Data*>(JS_GetOpaque2(ctx, this_val, class_id));
					if (s == nullptr) {
						return JS_EXCEPTION;
					}
					if constexpr (std::is_same<T, std::string>::value) {
						s->WriteValue(JS::Converter::get_string(ctx, argv[0]));
					}
					if constexpr (std::is_integral<T>::value) {
						s->WriteValue(JS::Converter::get_bigint64(ctx, argv[0]));
					}
					if constexpr (std::is_floating_point<T>::value) {
						s->WriteValue(JS::Converter::get_float64(ctx, argv[0]));
					}
					return JS_UNDEFINED;
				}, function_name);
			}

			inline static auto getter(
				JSContext* ctx,
				JSValueConst this_val,
				int magic
			) -> JSElement::boolean
			{
				auto s = static_cast<Data*>(JS_GetOpaque2(ctx, this_val, class_id));
				if (s == nullptr) {
					return JS_EXCEPTION;
				}
				return JS::Converter::to_bool(ctx, s->WriteIndent);
			}

			inline static auto setter(
				JSContext* ctx,
				JSValueConst this_val,
				JSValueConst val,
				int magic
			) -> JSElement::undefined
			{
				auto s = static_cast<Data*>(JS_GetOpaque2(ctx, this_val, class_id));
				if (s == nullptr) {
					return JS_EXCEPTION;
				}
				s->WriteIndent = JS_VALUE_GET_BOOL(val) == 0 ? false : true;
				return JS_UNDEFINED;
			}

			inline static const JSCFunctionListEntry proto_functions[] = {
				JS_CPPGETSET_MAGIC_DEF("write_indent", getter, setter, 0),
				JS_CPPFUNC_DEF("clear", 0, clear),
				JS_CPPFUNC_DEF("toString", 0, toString),
				JS_CPPFUNC_DEF("writeStartArray", 0, writeStartArray),
				JS_CPPFUNC_DEF("writeEndArray", 0, writeEndArray),
				JS_CPPFUNC_DEF("writeStartObject", 0, writeStartObject),
				JS_CPPFUNC_DEF("writeEndObject", 0, writeEndObject),
				JS_CPPFUNC_DEF("writeBoolean", 1, writeBoolean),
				JS_CPPFUNC_DEF("writeNull", 0, writeNull),
				JS_CPPFUNC_DEF("writePropertyName", 1, writePropertyName),
				JS_CPPFUNC_DEF("writeString", 1, writeValue<std::string>),
				JS_CPPFUNC_DEF("writeNumber", 1, writeValue<double>),
				JS_CPPFUNC_DEF("writeBigInt", 1, writeValue<std::int64_t>),
			};

			inline static auto register_class(
				JSContext* ctx
			) -> void
			{
				JS_NewClassID(&class_id);
				assert_conditional(JS_NewClass(JS_GetRuntime(ctx), class_id, &this_class) == 0, "JsonWriter class register failed", "register_class");
				auto class_name = "JsonWriter"_sv;
				auto point_ctor = JS_NewCFunction2(ctx, constructor, class_name.data(), 2, JS_CFUNC_constructor, 0);
				auto proto = JS_NewObject(ctx);
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

		namespace Number {

			template <typename T> requires std::is_integral<T>::value or std::is_floating_point<T>::value
			struct Data {
				T value{};

				Data(
				) = default;

				Data(
					T value
				) : value(value)
				{

				}

				~Data(
				) = default;
			};

			template <typename T> requires std::is_integral<T>::value or std::is_floating_point<T>::value
			struct NumberID {
				inline static JSClassID class_id = 0;
			};

			template <typename T> requires std::is_integral<T>::value or std::is_floating_point<T>::value
			inline static auto finalizer(
				JSRuntime * rt,
				JSValue val
			) -> void
			{
				auto s = static_cast<Data<T>*>(JS_GetOpaque(val, NumberID<T>::class_id));
				if (s != nullptr) {
					delete s;
				}
				return;
			}

			template <typename T> requires std::is_integral<T>::value or std::is_floating_point<T>::value
			inline static auto constructor(
				JSContext * ctx,
				JSValueConst new_target,
				int argc,
				JSValueConst * argv
			) -> JSElement::undefined
			{
				M_JS_PROXY_WRAPPER(ctx, {
					auto s = static_cast<Data<T>*>(nullptr);
					auto obj = JS_UNDEFINED;
					auto proto = JSElement::Prototype{};
					if (argc == 1) {
						if constexpr (std::is_integral<T>::value) {
							s = new Data<T>(static_cast<T>(JS::Converter::get_bigint64(ctx, argv[0])));
						}
						else {
							s = new Data<T>(static_cast<T>(JS::Converter::get_float64(ctx, argv[0])));
						}
					}
					else {
						s = new Data<T>();
					}
					proto = JS_GetPropertyStr(ctx, new_target, "prototype");
					if (JS_IsException(proto)) {
						goto fail;
					}
					obj = JS_NewObjectProtoClass(ctx, proto, NumberID<T>::class_id);
					JS_FreeValue(ctx, proto);
					if (JS_IsException(obj)) {
						goto fail;
					}
					JS_SetOpaque(obj, s);
					return obj;
				fail:
					js_free(ctx, s);
					JS_FreeValue(ctx, obj);
					return JS_EXCEPTION;
				}, "proxy_constructor");
			}

			template <typename T>
			inline static auto this_class = JSClassDef{
				.class_name = "Number",
				.finalizer = finalizer<T>,
			};

			template <>
			inline static auto this_class<int8_t> = JSClassDef{
				.class_name = "Integer8",
				.finalizer = finalizer<int8_t>,
			};

			template <>
			inline static auto this_class<int16_t> = JSClassDef{
				.class_name = "Integer16",
				.finalizer = finalizer<int16_t>,
			};

			template <>
			inline static auto this_class<int32_t> = JSClassDef{
				.class_name = "Integer32",
				.finalizer = finalizer<int32_t>,
			};

			template <>
			inline static auto this_class<int64_t> = JSClassDef{
				.class_name = "Integer64",
				.finalizer = finalizer<int64_t>,
			};

			template <>
			inline static auto this_class<uint8_t> = JSClassDef{
				.class_name = "UInteger8",
				.finalizer = finalizer<uint8_t>,
			};

			template <>
			inline static auto this_class<uint16_t> = JSClassDef{
				.class_name = "UInteger16",
				.finalizer = finalizer<uint16_t>,
			};

			template <>
			inline static auto this_class<uint32_t> = JSClassDef{
				.class_name = "UInteger32",
				.finalizer = finalizer<uint32_t>,
			};

			template <>
			inline static auto this_class<uint64_t> = JSClassDef{
				.class_name = "UInteger64",
				.finalizer = finalizer<uint64_t>,
			};

			template <>
			inline static auto this_class<float> = JSClassDef{
				.class_name = "Float",
				.finalizer = finalizer<float>,
			};

			template <>
			inline static auto this_class<double> = JSClassDef{
				.class_name = "Double",
				.finalizer = finalizer<double>,
			};

			template <typename T> requires std::is_integral<T>::value or std::is_floating_point<T>::value
			inline static auto getter(
				JSContext * ctx,
				JSValueConst this_val,
				int magic
			) -> JSElement::any
			{
				auto s = static_cast<Data<T>*>(JS_GetOpaque2(ctx, this_val, NumberID<T>::class_id));
				if (s == nullptr) {
					return JS_EXCEPTION;
				}
				if constexpr (std::is_integral<T>::value) {
					return JS::Converter::to_bigint(ctx, s->value);
				}
				else {
					return JS::Converter::to_number(ctx, s->value);
				}
			}

			template <typename T> requires std::is_integral<T>::value or std::is_floating_point<T>::value
			inline static auto setter(
				JSContext * ctx,
				JSValueConst this_val,
				JSValueConst val,
				int magic
			) -> JSElement::undefined
			{
				auto s = static_cast<Data<T>*>(JS_GetOpaque2(ctx, this_val, NumberID<T>::class_id));
				if (s == nullptr) {
					return JS_EXCEPTION;
				}
				if constexpr (std::is_integral<T>::value) {
					s->value = static_cast<T>(JS::Converter::get_bigint64(ctx, val));
				}
				else {
					s->value = static_cast<T>(JS::Converter::get_float64(ctx, val));
				}
				return JS_UNDEFINED;
			}

			template <typename T> requires std::is_integral<T>::value or std::is_floating_point<T>::value
			inline static const JSCFunctionListEntry proto_functions[] = {
				JS_CPPGETSET_MAGIC_DEF("value", getter<T>, setter<T>, 0),
			};

			template <typename T> requires std::is_integral<T>::value or std::is_floating_point<T>::value
			inline static auto register_class(
				JSContext* ctx
			) -> void
			{
				JS_NewClassID(&(NumberID<T>::class_id));
				assert_conditional(JS_NewClass(JS_GetRuntime(ctx), NumberID<T>::class_id, &this_class<T>) == 0, "Number class register failed", "register_class");
				auto class_name = fmt::format("{}", this_class<T>.class_name);
				auto point_ctor = JS_NewCFunction2(ctx, constructor<T>, class_name.data(), 2, JS_CFUNC_constructor, 0);
				auto proto = JS_NewObject(ctx);
				JS_SetPropertyFunctionList(ctx, proto, proto_functions<T>, countof(proto_functions<T>));
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



		// APNGMakerSetting Support

		namespace APNGMakerSetting {

			using Data = Definition::APNGMakerSetting;

			inline static JSClassID class_id;

			inline static auto finalizer(
				JSRuntime* rt,
				JSValue val
			) -> void
			{
				auto s = static_cast<Data*>(JS_GetOpaque(val, class_id));
				if (s != nullptr) {
					delete s;
				}
				return;
			}

			enum GetterSetter {
				delay_frames_list,
				loop,
				width,
				height,
				trim,
			};

			inline static auto constructor(
				JSContext* ctx,
				JSValueConst new_target,
				int argc,
				JSValueConst* argv
			) -> JSElement::undefined
			{
				using Uinteger32 = Number::NumberID<std::uint32_t>;
				using Uinteger32C = Number::Data<std::uint32_t>;
				M_JS_PROXY_WRAPPER(ctx, { 
					auto s = static_cast<Data*>(nullptr);
					auto obj = JS_UNDEFINED;
					auto proto = JSElement::Prototype{};
					if (argc == 5) {
						auto delay_frames_list = JS::Converter::get_array<std::uint32_t>(ctx, argv[0]);
						auto loop = static_cast<Uinteger32C*>(JS_GetOpaque2(ctx, argv[1], Uinteger32::class_id));
						if (loop == nullptr) {
							return JS_EXCEPTION;
						}
						auto width = static_cast<Uinteger32C*>(JS_GetOpaque2(ctx, argv[2], Uinteger32::class_id));
						if (width == nullptr) {
							return JS_EXCEPTION;
						}
						auto height = static_cast<Uinteger32C*>(JS_GetOpaque2(ctx, argv[3], Uinteger32::class_id));
						if (height == nullptr) {
							return JS_EXCEPTION;
						}
						auto trim = static_cast<Boolean::Data*>(JS_GetOpaque2(ctx, argv[4], Boolean::class_id));
						if (trim == nullptr) {
							return JS_EXCEPTION;
						}
						s = new Data(delay_frames_list, loop->value, width->value, height->value, trim->value);
						}
					else {
						s = new Data();
					}
					proto = JS_GetPropertyStr(ctx, new_target, "prototype");
					if (JS_IsException(proto)) {
						goto fail;
					}
					obj = JS_NewObjectProtoClass(ctx, proto, class_id);
					JS_FreeValue(ctx, proto);
					if (JS_IsException(obj)) {
						goto fail;
					}
					JS_SetOpaque(obj, s);
					return obj;
					fail:
						js_free(ctx, s);
						JS_FreeValue(ctx, obj);
						return JS_EXCEPTION;
				}, "proxy_constructor");
				// to
			}

			/*
				Current class
			*/

			inline static auto this_class = JSClassDef{
				.class_name = "APNGMakerSetting",
				.finalizer = finalizer,
			};

			inline static auto getter(
				JSContext* ctx,
				JSValueConst this_val,
				int magic
			) -> JSElement::any
			{
				auto s = static_cast<Data*>(JS_GetOpaque2(ctx, this_val, class_id));
				if (s == nullptr) {
					return JS_EXCEPTION;
				}
				switch (static_cast<GetterSetter>(magic)) {
					case GetterSetter::delay_frames_list: {
						return JS::Converter::to_array(ctx, s->delay_frames_list);
					}
					case GetterSetter::loop: {
						return JS::Converter::to_bigint(ctx, s->loop);
					}
					case GetterSetter::width: {
						return JS::Converter::to_bigint(ctx, s->width);
					}
					case GetterSetter::height: {
						return JS::Converter::to_bigint(ctx, s->height);
					}
					case GetterSetter::trim: {
						return JS::Converter::to_bool(ctx, s->trim);
					}
				}
			}

			inline static auto setter(
				JSContext* ctx,
				JSValueConst this_val,
				JSValueConst val,
				int magic
			) -> JSElement::undefined
			{
				auto s = static_cast<Data*>(JS_GetOpaque2(ctx, this_val, class_id));
				if (s == nullptr) {
					return JS_EXCEPTION;
				}
				switch (static_cast<GetterSetter>(magic)) {
					case GetterSetter::delay_frames_list: {
						s->delay_frames_list.clear();
						auto delay_frames_list = JS::Converter::get_array<uint32_t>(ctx, val);
						s->delay_frames_list.assign(delay_frames_list.begin(), delay_frames_list.end());
						break;
					}
					case GetterSetter::loop: {
						s->loop = static_cast<std::uint32_t>(JS::Converter::get_bigint64(ctx, val));
						break;
					}
					case GetterSetter::width: {
						s->width = static_cast<std::uint32_t>(JS::Converter::get_bigint64(ctx, val));
						break;
					}
					case GetterSetter::height: {
						s->height = static_cast<std::uint32_t>(JS::Converter::get_bigint64(ctx, val));
						break;
					}
					case GetterSetter::trim: {
						s->trim = static_cast<bool>(JS::Converter::get_bool(ctx, val));
						break;
					}
				}
				return JS_UNDEFINED;
			}

			inline static const JSCFunctionListEntry proto_functions[] = {
				JS_CPPGETSET_MAGIC_DEF("delay_frames_list", getter, setter, GetterSetter::delay_frames_list),
				JS_CPPGETSET_MAGIC_DEF("loop", getter, setter, GetterSetter::loop),
				JS_CPPGETSET_MAGIC_DEF("width", getter, setter, GetterSetter::width),
				JS_CPPGETSET_MAGIC_DEF("height", getter, setter, GetterSetter::height),
				JS_CPPGETSET_MAGIC_DEF("trim", getter, setter, GetterSetter::trim),
			};

			inline static auto register_class(
				JSContext* ctx
			) -> void
			{
				JS_NewClassID(&class_id);
				assert_conditional(JS_NewClass(JS_GetRuntime(ctx), class_id, &this_class) == 0, "APNGMakerSetting class failed register", "register_class");
				auto class_name = "APNGMakerSetting"_sv;
				auto point_ctor = JS_NewCFunction2(ctx, constructor, class_name.data(), 2, JS_CFUNC_constructor, 0);
				auto proto = JS_NewObject(ctx);
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

		// Size Support

		namespace Size {

			struct Data {

				std::size_t value;

				Data(
				) = default;

				Data(
					std::size_t value
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
				auto s = static_cast<Data*>(JS_GetOpaque(val, class_id));
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
			) -> JSElement::undefined
			{
				M_JS_PROXY_WRAPPER(ctx, {
					auto s = static_cast<Data*>(nullptr);
					auto obj = JS_UNDEFINED;
					auto proto = JSElement::Prototype{};
					if (argc == 1) {
						s = new Data{ static_cast<std::size_t>(JS::Converter::get_bigint64(ctx, argv[0])) };
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
					if (JS_IsException(obj)) {
						goto fail;
					}
					JS_SetOpaque(obj, s);
					return obj;
				fail:
					js_free(ctx, s);
					JS_FreeValue(ctx, obj);
					return JS_EXCEPTION; 
				}, "proxy_constructor");
			}

			/*
				Current class
			*/

			inline static auto this_class = JSClassDef{
				.class_name = "Size",
				.finalizer = finalizer,
			};

			inline static auto getter(
				JSContext* ctx,
				JSValueConst this_val,
				int magic
			) -> JSElement::bigint
			{
				auto s = static_cast<Data*>(JS_GetOpaque2(ctx, this_val, class_id));
				if (s == nullptr) {
					return JS_EXCEPTION;
				}
				return JS::Converter::to_bigint(ctx, s->value);
			}

			inline static auto setter(
				JSContext* ctx,
				JSValueConst this_val,
				JSValueConst val,
				int magic
			) -> JSElement::undefined
			{
				auto s = static_cast<Data*>(JS_GetOpaque2(ctx, this_val, class_id));
				if (s == nullptr) {
					return JS_EXCEPTION;
				}
				s->value = JS::Converter::get_bigint64(ctx, val);
				return JS_UNDEFINED;
			}

			inline static const JSCFunctionListEntry proto_functions[] = {
				JS_CPPGETSET_MAGIC_DEF("value", getter, setter, 0),
			};

			inline static auto instance(
				JSContext* ctx,
				JSValueConst this_val,
				int argc,
				JSValueConst* argv
			) -> JSDefine::Size
			{
				M_JS_PROXY_WRAPPER(ctx, {
					try_assert(argc == 1, fmt::format("{} 1, {}: {}", Kernel::Language::get("kernel.argument_expected"), Kernel::Language::get("kernel.argument_received"), argc));
					auto s = new Data{ static_cast<std::size_t>(JS::Converter::get_bigint64(ctx, argv[0]))  };
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
				}, "instance");
			}


			inline static auto register_class(
				JSContext* ctx
			) -> void
			{
				JS_NewClassID(&class_id);
				assert_conditional(JS_NewClass(JS_GetRuntime(ctx), class_id, &this_class) == 0, "Size class failed register", "register_class");
				auto class_name = "Size"_sv;
				auto point_ctor = JS_NewCFunction2(ctx, constructor, class_name.data(), 2, JS_CFUNC_constructor, 0);
				auto proto = JS_NewObject(ctx);
				auto instance_c = JS_NewCFunction(ctx, instance, "instance", 0);
				JS_SetPropertyStr(ctx, point_ctor, "instance", instance_c);
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

		// Character Support

		namespace Character {

			template <typename T> requires std::is_same<T, char>::value or std::is_same<T, unsigned char>::value or std::is_same<T, wchar_t>::value
			struct Data {

				T value;

				Data(
				) = default;

				Data(
					T value
				) : value(value)
				{

				}

				~Data(

				) = default;
			};

			template <typename T> requires std::is_same<T, char>::value or std::is_same<T, unsigned char>::value or std::is_same<T, wchar_t>::value
			struct ClassID {
				inline static JSClassID value;
			};

			template <typename T> requires std::is_same<T, char>::value or std::is_same<T, unsigned char>::value or std::is_same<T, wchar_t>::value
			inline static auto finalizer(
				JSRuntime* rt,
				JSValue val
			) -> void
			{
				auto s = static_cast<Data<T>*>(JS_GetOpaque(val, ClassID<T>::value));
				if (s != nullptr) {
					delete s;
				}
				return;
			}

			template <typename T> requires std::is_same<T, char>::value or std::is_same<T, unsigned char>::value or std::is_same<T, wchar_t>::value
			inline static auto constructor(
				JSContext* ctx,
				JSValueConst new_target,
				int argc,
				JSValueConst* argv
			) -> JSElement::undefined
			{
				M_JS_PROXY_WRAPPER(ctx, {
					auto s = static_cast<Data<T>*>(nullptr);
					auto obj = JS_UNDEFINED;
					auto proto = JSDefine::Character{};
					if (argc == 1) {
						s = new Data<T>{ static_cast<T>(JS::Converter::get_bigint64(ctx, argv[0])) };
					}
					else {
						s = new Data<T>();
					}
					proto = JS_GetPropertyStr(ctx, new_target, "prototype");
					if (JS_IsException(proto)) {
						goto fail;
					}
					obj = JS_NewObjectProtoClass(ctx, proto, ClassID<T>::value);
					JS_FreeValue(ctx, proto);
					if (JS_IsException(obj)) {
						goto fail;
					}
					JS_SetOpaque(obj, s);
					return obj;
				fail:
					js_free(ctx, s);
					JS_FreeValue(ctx, obj);
					return JS_EXCEPTION; 
				}, "proxy_constructor");
			}

			/*
				Current class
			*/
			template <typename T> requires std::is_same<T, char>::value or std::is_same<T, unsigned char>::value or std::is_same<T, wchar_t>::value
			inline static auto this_class = JSClassDef{
				.class_name = "Character",
				.finalizer = finalizer<T>,
			};

			template <>
			inline static auto this_class<unsigned char> = JSClassDef{
				.class_name = "UCharacter",
				.finalizer = finalizer<unsigned char>,
			};

			template <>
			inline static auto this_class<wchar_t> = JSClassDef{
				.class_name = "WideCharacter",
				.finalizer = finalizer<wchar_t>,
			};
			// Getter

			template <typename T> requires std::is_same<T, char>::value or std::is_same<T, unsigned char>::value or std::is_same<T, wchar_t>::value
			inline static auto getter(
				JSContext* ctx,
				JSValueConst this_val,
				int magic
			) -> JSElement::bigint
			{
				auto s = static_cast<Data<T>*>(JS_GetOpaque2(ctx, this_val, ClassID<T>::value));
				if (s == nullptr) {
					return JS_EXCEPTION;
				}
				return JS::Converter::to_bigint(ctx, static_cast<std::int64_t>(s->value));
			}

			// Setter
			template <typename T> requires std::is_same<T, char>::value or std::is_same<T, unsigned char>::value or std::is_same<T, wchar_t>::value
			inline static auto setter(
				JSContext* ctx,
				JSValueConst this_val,
				JSValueConst val,
				int magic
			) -> JSElement::undefined
			{
				auto s = static_cast<Data<T>*>(JS_GetOpaque2(ctx, this_val, ClassID<T>::value));
				auto v = bool{};
				if (s == nullptr) {
					return JS_EXCEPTION;
				}
				s->value = static_cast<T>(JS::Converter::get_bigint64(ctx, val));
				return JS_UNDEFINED;
			}

			// Function
			template <typename T> requires std::is_same<T, char>::value or std::is_same<T, unsigned char>::value or std::is_same<T, wchar_t>::value
			inline static const JSCFunctionListEntry proto_functions[] = {
				JS_CPPGETSET_MAGIC_DEF("value", getter<T>, setter<T>, 0),
			};

			// Make instance
			template <typename T> requires std::is_same<T, char>::value or std::is_same<T, unsigned char>::value or std::is_same<T, wchar_t>::value
			inline static auto instance(
				JSContext* ctx,
				JSValueConst this_val,
				int argc,
				JSValueConst* argv
			) -> JSDefine::Character
			{
				M_JS_PROXY_WRAPPER(ctx, {
					try_assert(argc == 1, fmt::format("{} 1, {}: {}", Kernel::Language::get("kernel.argument_expected"), Kernel::Language::get("kernel.argument_received"), argc));
					auto s = new Data{ static_cast<T>(JS::Converter::get_bigint64(ctx, argv[0]))  };
					auto proto = JS_GetPropertyStr(ctx, this_val, "prototype");
					if (JS_IsException(proto)) {
						js_free(ctx, s);
						return JS_EXCEPTION;
					}
					auto obj = JS_NewObjectProtoClass(ctx, proto, ClassID<T>::value);
					JS_FreeValue(ctx, proto);
					if (JS_IsException(obj)) {
						js_free(ctx, s);
						return JS_EXCEPTION;
					}
					JS_SetOpaque(obj, s);
					return obj;
				}, "instance");
			}

			template <typename T> requires std::is_same<T, char>::value or std::is_same<T, unsigned char>::value or std::is_same<T, wchar_t>::value
			inline static auto register_class(
				JSContext* ctx
			) -> void
			{
				JS_NewClassID(&(ClassID<T>::value));
				assert_conditional(JS_NewClass(JS_GetRuntime(ctx), ClassID<T>::value, &this_class<T>) == 0, "Character class register failed", "register_class");
				auto class_name = std::string_view{};
				if constexpr (std::is_same<T, char>::value) {
					class_name = "Character"_sv;
				}
				else if constexpr (std::is_same<T, wchar_t>::value) {
					class_name = "WideCharacter"_sv;
				}
				else {
					class_name = "UCharacter"_sv;
				}
				auto point_ctor = JS_NewCFunction2(ctx, constructor<T>, class_name.data(), 2, JS_CFUNC_constructor, 0);
				auto proto = JS_NewObject(ctx);
				auto instance_c = JS_NewCFunction(ctx, instance<T>, "instance", 0);
				JS_SetPropertyStr(ctx, point_ctor, "instance", instance_c);
				JS_SetPropertyFunctionList(ctx, proto, proto_functions<T>, countof(proto_functions<T>));
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

		// String Support

		namespace String {

			struct CData {
				char* value = nullptr;

				std::size_t size{};

				CData(

				) = default;

				CData(
					std::size_t size
				) : size(size)
				{
				}

				CData(
					char* value,
					std::size_t size
				) : value(value), size(size)
				{

				}

				CData(
					char* value
				) : value(value), size(strlen(value))
				{
				}

				~CData() {
					free(const_cast<char*>(value));
				}

				auto view(

				) -> std::string_view
				{
					return std::string_view{ thiz.value, thiz.size };
				}
			};

			using Data = CData;

			inline static JSClassID class_id;

			inline static auto finalizer(
				JSRuntime* rt,
				JSValue val
			) -> void
			{
				auto s = static_cast<Data*>(JS_GetOpaque(val, class_id));
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
				M_JS_PROXY_WRAPPER(ctx, {
					auto s = static_cast<Data*>(nullptr);
					auto obj = JS_UNDEFINED;
					auto proto = JSDefine::String{};
					if (argc == 1) {
						s = new Data();
						auto str = JS::Converter::get_string(ctx, argv[0]);
						s->value = static_cast<char*>(malloc(sizeof(char) * str.size() + 1));
						if (s->value == nullptr) {
							JS_ThrowReferenceError(ctx, "could not allocate Kernel String");
							return JS_EXCEPTION;
						}
						std::memcpy(s->value, str.data(), str.size());
						s->size = str.size();
						s->value[str.size()] = '\0';
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
					if (JS_IsException(obj)) {
						goto fail;
					}
					JS_SetOpaque(obj, s);
					return obj;
				fail:
					js_free(ctx, s);
					JS_FreeValue(ctx, obj);
					return JS_EXCEPTION; 
				}, "proxy_constructor");
			}

			/*
				Current class
			*/

			inline static auto this_class = JSClassDef{
				.class_name = "String",
				.finalizer = finalizer,
			};

			// Getter

			inline static auto getter(
				JSContext* ctx,
				JSValueConst this_val,
				int magic
			) -> JSElement::string
			{
				auto s = static_cast<Data*>(JS_GetOpaque2(ctx, this_val, class_id));
				if (s == nullptr) {
					return JS_EXCEPTION;
				}
				return JS::Converter::to_string(ctx, s->view());
			}

			// Setter

			inline static auto setter (
				JSContext* ctx,
				JSValueConst this_val,
				JSValueConst val,
				int magic
			) -> JSElement::undefined
			{
				auto s = static_cast<Data*>(JS_GetOpaque2(ctx, this_val, class_id));
				auto v = bool{};
				if (s == nullptr) {
					return JS_EXCEPTION;
				}
				auto str = JS::Converter::get_string(ctx, val);
				s->value = static_cast<char*>(malloc(sizeof(char) * str.size() + 1));
				std::memcpy(s->value, str.data(), str.size());
				s->size = str.size();
				s->value[str.size()] = '\0';
				return JS_UNDEFINED;
			}

			// Function

			inline static const JSCFunctionListEntry proto_functions[] = {
				JS_CPPGETSET_MAGIC_DEF("value", getter, setter, 0),
			};

			// Make instance

			inline static auto instance(
				JSContext* ctx,
				JSValueConst this_val,
				int argc,
				JSValueConst* argv
			) -> JSDefine::String
			{
				M_JS_PROXY_WRAPPER(ctx, {
					try_assert(argc == 1, fmt::format("{} 1, {}: {}", Kernel::Language::get("kernel.argument_expected"), Kernel::Language::get("kernel.argument_received"), argc));
					auto s = new Data{};
					auto str = JS::Converter::get_string(ctx, argv[0]);
					s->value = static_cast<char*>(malloc(sizeof(char) * str.size() + 1));
					std::memcpy(s->value, str.data(), str.size());
					s->size = str.size();
					s->value[str.size()] = '\0';
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
				}, "instance");
			}


			inline static auto register_class(
				JSContext* ctx
			) -> void
			{
				JS_NewClassID(&class_id);
				assert_conditional(JS_NewClass(JS_GetRuntime(ctx), class_id, &this_class) == 0, "String class register failed", "register_class");
				auto class_name = "String"_sv;
				auto point_ctor = JS_NewCFunction2(ctx, constructor, class_name.data(), 2, JS_CFUNC_constructor, 0);
				auto proto = JS_NewObject(ctx);
				auto instance_c = JS_NewCFunction(ctx, instance, "instance", 0);
				JS_SetPropertyStr(ctx, point_ctor, "instance", instance_c);
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

		namespace BinaryView {
		
			struct Data {

				std::vector<std::uint8_t> value;

				using It = decltype(value.begin());

				Data(
				) = default;

				explicit Data(
					const std::vector<std::uint8_t>& value
				) : value(value)
				{

				}

				explicit Data(
					It begin,
					It end
				) : value(begin, end)
				{ 
				}

				explicit Data(
					uint8_t* begin,
					uint8_t* end
				) : value(begin, end)
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
				auto s = static_cast<Data*>(JS_GetOpaque(val, class_id));
				if (s != nullptr) {
					delete s;
				}
				return;
			}

			/*
				Get size
			*/

			inline static auto size(
				JSContext* ctx,
				JSValueConst this_val,
				int argc,
				JSValueConst* argv
			) -> JSElement::bigint
			{
				M_JS_PROXY_WRAPPER(ctx, {
					try_assert(argc == 0, fmt::format("{} 0, {}: {}", Kernel::Language::get("kernel.argument_expected"), Kernel::Language::get("kernel.argument_received"), argc));
					auto s = static_cast<Data*>(JS_GetOpaque2(ctx, this_val, class_id));
					if (s == nullptr) {
						return JS_EXCEPTION;
					}
					return JS::Converter::to_bigint<uint64_t>(ctx, s->value.size());
				}, "size"_sv);
			}

			/*
				Get capacity
			*/

			inline static auto capacity(
				JSContext* ctx,
				JSValueConst this_val,
				int argc,
				JSValueConst* argv
			) -> JSElement::bigint
			{
				M_JS_PROXY_WRAPPER(ctx, {
					try_assert(argc == 0, fmt::format("{} 0, {}: {}", Kernel::Language::get("kernel.argument_expected"), Kernel::Language::get("kernel.argument_received"), argc));
					auto s = static_cast<Data*>(JS_GetOpaque2(ctx, this_val, class_id));
					if (s == nullptr) {
						return JS_EXCEPTION;
					}
					return JS::Converter::to_bigint<uint64_t>(ctx, s->value.capacity());
				}, "capacity"_sv);
			}

			/*
				Allocate
			*/

			inline static auto allocate(
				JSContext* ctx,
				JSValueConst this_val,
				int argc,
				JSValueConst* argv
			) -> JSElement::undefined
			{
				M_JS_PROXY_WRAPPER(ctx, {
					try_assert(argc == 1, fmt::format("{} 1, {}: {}", Kernel::Language::get("kernel.argument_expected"), Kernel::Language::get("kernel.argument_received"), argc));
					auto s = static_cast<Data*>(JS_GetOpaque2(ctx, this_val, class_id));
					if (s == nullptr) {
						return JS_EXCEPTION;
					}
					s->value.reserve(static_cast<std::size_t>(JS::Converter::get_bigint64(ctx, argv[0])));
					return JS::Converter::get_undefined();
				}, "allocate"_sv);
			}

			/*
				Sub
			*/

			inline static auto sub(
				JSContext* ctx,
				JSValueConst this_val,
				int argc,
				JSValueConst* argv
			) -> JSElement::ArrayBuffer
			{
				M_JS_PROXY_WRAPPER(ctx, {
					try_assert(argc == 2, fmt::format("{} 2, {}: {}", Kernel::Language::get("kernel.argument_expected"), Kernel::Language::get("kernel.argument_received"), argc));
					auto s = static_cast<Data*>(JS_GetOpaque2(ctx, this_val, class_id));
					if (s == nullptr) {
						return JS_EXCEPTION;
					}
					auto from = static_cast<std::size_t>(JS::Converter::get_bigint64(ctx, argv[0]));
					auto to = static_cast<std::size_t>(JS::Converter::get_bigint64(ctx, argv[1]));
					assert_conditional(from < to, fmt::format("sub failed because {} >= {}", from, to), "sub");
					assert_conditional(to < s->value.size(), fmt::format("sub failed because trying to reach outside bounds"), "sub");
					assert_conditional(from >= 0, fmt::format("from cannot smaller than zero. Got value: {}", from), "sub");
					return JS_NewArrayBufferCopy(ctx, s->value.data() + from, to);
				}, "sub"_sv);
			}

			// StreamView
			template <auto use_big_endian>
			inline static auto stream (
				JSContext* ctx,
				JSValueConst this_val,
				int argc,
				JSValueConst* argv
			) -> JSDefine::DataStreamView
			{
				static_assert(use_big_endian == true || use_big_endian == false, "use_big_endian can only be true or false");
				auto m_function_name = std::string_view{};
				if constexpr (use_big_endian) {
					m_function_name = "big_stream_view"_sv;
				}
				else {
					m_function_name = "stream_view"_sv;
				}
				M_JS_PROXY_WRAPPER(ctx, {
					try_assert(argc == 0, fmt::format("{} 0, {}: {}", Kernel::Language::get("kernel.argument_expected"), Kernel::Language::get("kernel.argument_received"), argc));
					auto s = static_cast<Data*>(JS_GetOpaque2(ctx, this_val, class_id));
					if (s == nullptr) {
						return JS_EXCEPTION;
					}
					auto sub = new Definition::DataStreamView(s->value);
					auto global_obj = JS_GetGlobalObject(ctx);
					auto sen_obj = JS_GetPropertyStr(ctx, global_obj, "Sen");
					auto kernel_obj = JS_GetPropertyStr(ctx, sen_obj, "Kernel");
					auto constructor_name = std::string_view{};
					if constexpr (use_big_endian) {
						constructor_name = "DataStreamViewUseBigEndian"_sv;
					}
					else {
						constructor_name = "DataStreamView"_sv;
					}
					auto stream_ctor = JS_GetPropertyStr(ctx, kernel_obj, constructor_name.data());
					auto proto = JS_GetPropertyStr(ctx, stream_ctor, "prototype");
					if (JS_IsException(proto)) {
						js_free(ctx, sub);
						throw Exception("not a constructor");
					}
					auto obj = JS_NewObjectProtoClass(ctx, proto, DataStreamView::ClassID<use_big_endian>::value);
					JS_FreeValue(ctx, proto);
					if (JS_IsException(obj)) {
						js_free(ctx, sub);
						throw Exception("can't define class");
					}
					JS_SetOpaque(obj, sub);
					JS_FreeValue(ctx, global_obj);
					JS_FreeValue(ctx, sen_obj);
					JS_FreeValue(ctx, kernel_obj);
					JS_FreeValue(ctx, stream_ctor);
					return obj;
				}, m_function_name);
			}

			// Constructor

			inline static auto constructor(
				JSContext* ctx,
				JSValueConst new_target,
				int argc,
				JSValueConst* argv
			) -> JSElement::undefined
			{
				M_JS_PROXY_WRAPPER(ctx, {
					auto s = static_cast<Data*>(nullptr);
					auto obj = JS_UNDEFINED;
					auto proto = JSElement::Prototype{};
					if (argc == 1) {
						auto byteLength = std::size_t{};
						auto data = JS_GetArrayBuffer(ctx, &byteLength, argv[0]);
						if (data == nullptr) {
							return JS_EXCEPTION;
						}
						s = new Data(data, data + byteLength);
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
					if (JS_IsException(obj)) {
						goto fail;
					}
					JS_SetOpaque(obj, s);
					return obj;
				fail:
					js_free(ctx, s);
					JS_FreeValue(ctx, obj);
					return JS_EXCEPTION; 
				}, "proxy_constructor");
			}

			/*
				Current class
			*/

			inline static auto this_class = JSClassDef{
				.class_name = "BinaryView",
				.finalizer = finalizer,
			};

			// Getter

			inline static auto getter(
				JSContext* ctx,
				JSValueConst this_val,
				int magic
			) -> JSValue
			{
				auto s = static_cast<Data*>(JS_GetOpaque2(ctx, this_val, class_id));
				if (s == nullptr) {
					return JS_EXCEPTION;
				}
				return JS_NewArrayBufferCopy(ctx, s->value.data(), s->value.size());
			}

			// Setter

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
				auto byteLength = std::size_t{};
				auto data = JS_GetArrayBuffer(ctx, &byteLength, val);
				if (data == nullptr) {
					return JS_EXCEPTION;
				}
				s->value.clear();
				s->value.assign(data, data + byteLength);
				return JS_UNDEFINED;
			}

			// Function

			inline static const JSCFunctionListEntry proto_functions[] = {
				JS_CPPGETSET_MAGIC_DEF("value", getter, setter, 0),
				JS_CPPFUNC_DEF("size", 0, size),
				JS_CPPFUNC_DEF("capacity", 0, capacity),
				JS_CPPFUNC_DEF("allocate", 1, allocate),
				JS_CPPFUNC_DEF("sub", 2, sub),
				JS_CPPFUNC_DEF("stream_view", 0, stream<false>),
				JS_CPPFUNC_DEF("big_stream_view", 0, stream<true>),
			};

			// Make instance

			inline static auto instance(
				JSContext* ctx,
				JSValueConst this_val,
				int argc,
				JSValueConst* argv
			) -> JSDefine::BinaryView
			{
				M_JS_PROXY_WRAPPER(ctx, {
					try_assert(argc == 0, fmt::format("{} 0, {}: {}", Kernel::Language::get("kernel.argument_expected"), Kernel::Language::get("kernel.argument_received"), argc));
					auto s = new Data{ };
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
				}, "instance");
			}


			inline static auto register_class(
				JSContext* ctx
			) -> void
			{
				JS_NewClassID(&class_id);
				assert_conditional(JS_NewClass(JS_GetRuntime(ctx), class_id, &this_class) == 0, "BinaryView class register failed", "register_class");
				auto class_name = "BinaryView"_sv;
				auto point_ctor = JS_NewCFunction2(ctx, constructor, class_name.data(), 2, JS_CFUNC_constructor, 0);
				auto proto = JS_NewObject(ctx);
				auto instance_c = JS_NewCFunction(ctx, instance, "instance", 0);
				JS_SetPropertyStr(ctx, point_ctor, "instance", instance_c);
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

		// Canvas Support

		namespace Canvas {

			using Data = canvas_ity::canvas;

			struct Detail {
				inline static JSClassID class_id;
			};

			inline static auto finalizer(
				JSRuntime* rt,
				JSValue val
			) -> void
			{
				auto s = static_cast<Data*>(JS_GetOpaque(val, Detail::class_id));
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
			) -> JSElement::undefined
			{
				M_JS_PROXY_WRAPPER(ctx, {
					auto s = static_cast<Data*>(nullptr);
					auto obj = JS_UNDEFINED;
					auto proto = JSDefine::Canvas{};
					if (argc == 2) {
						s = new Data(static_cast<int>(JS::Converter::get_bigint64(ctx, argv[0])), static_cast<int>(JS::Converter::get_bigint64(ctx, argv[1])));
					}
					else {
						return JS_EXCEPTION;
					}
					proto = JS_GetPropertyStr(ctx, new_target, "prototype");
					if (JS_IsException(proto)) {
						goto fail;
					}
					obj = JS_NewObjectProtoClass(ctx, proto, Detail::class_id);
					JS_FreeValue(ctx, proto);
					if (JS_IsException(obj)) {
						goto fail;
					}
					JS_SetOpaque(obj, s);
					return obj;
				fail:
					js_free(ctx, s);
					JS_FreeValue(ctx, obj);
					return JS_EXCEPTION; 
				}, "proxy_constructor");
			}

			/*
				Current class
			*/

			inline static auto this_class = JSClassDef{
				.class_name = "Canvas",
				.finalizer = finalizer,
			};

			/*
				Scale
			*/

			inline static auto scale(
				JSContext* ctx,
				JSValueConst this_val,
				int argc,
				JSValueConst* argv
			) -> JSElement::undefined
			{
				M_JS_PROXY_WRAPPER(ctx, {
					try_assert(argc == 2, fmt::format("{} 2, {}: {}", Kernel::Language::get("kernel.argument_expected"), Kernel::Language::get("kernel.argument_received"), argc));
					auto s = static_cast<Data*>(JS_GetOpaque2(ctx, this_val, Detail::class_id));
					if (s == nullptr) {
						return JS_EXCEPTION;
					}
					s->scale(JS::Converter::get_float32(ctx, argv[0]), JS::Converter::get_float32(ctx, argv[1]));
					return JS_UNDEFINED;
				}, "scale"_sv);
			}

			/*
				Rotate
			*/

			inline static auto rotate(
				JSContext* ctx,
				JSValueConst this_val,
				int argc,
				JSValueConst* argv
			) -> JSElement::undefined
			{
				M_JS_PROXY_WRAPPER(ctx, {
					try_assert(argc == 1, fmt::format("{} 1, {}: {}", Kernel::Language::get("kernel.argument_expected"), Kernel::Language::get("kernel.argument_received"), argc));
					auto s = static_cast<Data*>(JS_GetOpaque2(ctx, this_val, Detail::class_id));
					if (s == nullptr) {
						return JS_EXCEPTION;
					}
					s->rotate(JS::Converter::get_float32(ctx, argv[0]));
					return JS_UNDEFINED;
				}, "rotate"_sv);
			}

			/*
				Translate
			*/

			inline static auto translate(
				JSContext* ctx,
				JSValueConst this_val,
				int argc,
				JSValueConst* argv
			) -> JSElement::undefined
			{
				M_JS_PROXY_WRAPPER(ctx, {
					try_assert(argc == 2, fmt::format("{} 2, {}: {}", Kernel::Language::get("kernel.argument_expected"), Kernel::Language::get("kernel.argument_received"), argc));
					auto s = static_cast<Data*>(JS_GetOpaque2(ctx, this_val, Detail::class_id));
					if (s == nullptr) {
						return JS_EXCEPTION;
					}
					s->translate(JS::Converter::get_float32(ctx, argv[0]), JS::Converter::get_float32(ctx, argv[1]));
					return JS_UNDEFINED;
				}, "translate"_sv);
			}

			/*
				Transform
			*/

			inline static auto transform(
				JSContext* ctx,
				JSValueConst this_val,
				int argc,
				JSValueConst* argv
			) -> JSElement::undefined
			{
				M_JS_PROXY_WRAPPER(ctx, {
					try_assert(argc == 6, fmt::format("{} 6, {}: {}", Kernel::Language::get("kernel.argument_expected"), Kernel::Language::get("kernel.argument_received"), argc));
					auto s = static_cast<Data*>(JS_GetOpaque2(ctx, this_val, Detail::class_id));
					if (s == nullptr) {
						return JS_EXCEPTION;
					}
					s->transform(JS::Converter::get_float32(ctx, argv[0]), JS::Converter::get_float32(ctx, argv[1]), JS::Converter::get_float32(ctx, argv[2]), JS::Converter::get_float32(ctx, argv[3]), JS::Converter::get_float32(ctx, argv[4]), JS::Converter::get_float32(ctx, argv[5]));
					return JS_UNDEFINED;
				}, "transform"_sv);
			}

			/*
				Set Transform
			*/

			inline static auto set_transform(
				JSContext* ctx,
				JSValueConst this_val,
				int argc,
				JSValueConst* argv
			) -> JSElement::undefined
			{
				M_JS_PROXY_WRAPPER(ctx, {
					try_assert(argc == 6, fmt::format("{} 6, {}: {}", Kernel::Language::get("kernel.argument_expected"), Kernel::Language::get("kernel.argument_received"), argc));
					auto s = static_cast<Data*>(JS_GetOpaque2(ctx, this_val, Detail::class_id));
					if (s == nullptr) {
						return JS_EXCEPTION;
					}
					s->set_transform(JS::Converter::get_float32(ctx, argv[0]), JS::Converter::get_float32(ctx, argv[1]), JS::Converter::get_float32(ctx, argv[2]), JS::Converter::get_float32(ctx, argv[3]), JS::Converter::get_float32(ctx, argv[4]), JS::Converter::get_float32(ctx, argv[5]));
					return JS_UNDEFINED;
				}, "set_transform"_sv);
			}

			/*
				Set global data
			*/

			inline static auto set_global_alpha(
				JSContext* ctx,
				JSValueConst this_val,
				int argc,
				JSValueConst* argv
			) -> JSElement::undefined
			{
				M_JS_PROXY_WRAPPER(ctx, {
					try_assert(argc == 1, fmt::format("{} 1, {}: {}", Kernel::Language::get("kernel.argument_expected"), Kernel::Language::get("kernel.argument_received"), argc));
					auto s = static_cast<Data*>(JS_GetOpaque2(ctx, this_val, Detail::class_id));
					if (s == nullptr) {
						return JS_EXCEPTION;
					}
					s->set_global_alpha(JS::Converter::get_float32(ctx, argv[0]));
					return JS_UNDEFINED;
				}, "set_global_alpha"_sv);
			}

			/*
				Set shadow color
			*/

			inline static auto set_shadow_color(
				JSContext* ctx,
				JSValueConst this_val,
				int argc,
				JSValueConst* argv
			) -> JSElement::undefined
			{
				M_JS_PROXY_WRAPPER(ctx, {
					try_assert(argc == 4, fmt::format("{} 4, {}: {}", Kernel::Language::get("kernel.argument_expected"), Kernel::Language::get("kernel.argument_received"), argc));
					auto s = static_cast<Data*>(JS_GetOpaque2(ctx, this_val, Detail::class_id));
					if (s == nullptr) {
						return JS_EXCEPTION;
					}
					s->set_shadow_color(JS::Converter::get_float32(ctx, argv[0]), JS::Converter::get_float32(ctx, argv[1]), JS::Converter::get_float32(ctx, argv[2]), JS::Converter::get_float32(ctx, argv[3]));
					return JS_UNDEFINED;
				}, "set_shadow_color"_sv);
			}

			/*
				Set line width
			*/

			inline static auto set_line_width(
				JSContext* ctx,
				JSValueConst this_val,
				int argc,
				JSValueConst* argv
			) -> JSElement::undefined
			{
				M_JS_PROXY_WRAPPER(ctx, {
					try_assert(argc == 1, fmt::format("{} 1, {}: {}", Kernel::Language::get("kernel.argument_expected"), Kernel::Language::get("kernel.argument_received"), argc));
					auto s = static_cast<Data*>(JS_GetOpaque2(ctx, this_val, Detail::class_id));
					if (s == nullptr) {
						return JS_EXCEPTION;
					}
					s->set_line_width(JS::Converter::get_float32(ctx, argv[0]));
					return JS_UNDEFINED;
				}, "set_line_width"_sv);
			}

			/*
				Set shadow blur
			*/

			inline static auto set_shadow_blur(
				JSContext* ctx,
				JSValueConst this_val,
				int argc,
				JSValueConst* argv
			) -> JSElement::undefined
			{
				M_JS_PROXY_WRAPPER(ctx, {
					try_assert(argc == 1, fmt::format("{} 1, {}: {}", Kernel::Language::get("kernel.argument_expected"), Kernel::Language::get("kernel.argument_received"), argc));
					auto s = static_cast<Data*>(JS_GetOpaque2(ctx, this_val, Detail::class_id));
					if (s == nullptr) {
						return JS_EXCEPTION;
					}
					s->set_global_alpha(JS::Converter::get_float32(ctx, argv[0]));
					return JS_UNDEFINED;
				}, "set_shadow_blur"_sv);
			}

			/*
				Set miter limit
			*/

			inline static auto set_miter_limit(
				JSContext* ctx,
				JSValueConst this_val,
				int argc,
				JSValueConst* argv
			) -> JSElement::undefined
			{
				M_JS_PROXY_WRAPPER(ctx, {
					try_assert(argc == 1, fmt::format("{} 1, {}: {}", Kernel::Language::get("kernel.argument_expected"), Kernel::Language::get("kernel.argument_received"), argc));
					auto s = static_cast<Data*>(JS_GetOpaque2(ctx, this_val, Detail::class_id));
					if (s == nullptr) {
						return JS_EXCEPTION;
					}
					s->set_miter_limit(JS::Converter::get_float32(ctx, argv[0]));
					return JS_UNDEFINED;
				}, "set_miter_limit"_sv);
			}

			// Perhaps use

			using brush_type = canvas_ity::brush_type;

			/*
				Set color
			*/

			inline static auto set_color(
				JSContext* ctx,
				JSValueConst this_val,
				int argc,
				JSValueConst* argv
			) -> JSElement::undefined
			{
				M_JS_PROXY_WRAPPER(ctx, {
					try_assert(argc == 5, fmt::format("{} 5, {}: {}", Kernel::Language::get("kernel.argument_expected"), Kernel::Language::get("kernel.argument_received"), argc));
					auto s = static_cast<Data*>(JS_GetOpaque2(ctx, this_val, Detail::class_id));
					if (s == nullptr) {
						return JS_EXCEPTION;
					}
					s->set_color(static_cast<brush_type>(JS::Converter::get_bigint64(ctx, argv[0])), JS::Converter::get_float32(ctx, argv[1]), JS::Converter::get_float32(ctx, argv[2]), JS::Converter::get_float32(ctx, argv[3]), JS::Converter::get_float32(ctx, argv[4]));
					return JS_UNDEFINED;
				}, "set_color"_sv);
			}

			/*
				Set linear gradient
			*/

			inline static auto set_linear_gradient(
				JSContext* ctx,
				JSValueConst this_val,
				int argc,
				JSValueConst* argv
			) -> JSElement::undefined
			{
				M_JS_PROXY_WRAPPER(ctx, {
					try_assert(argc == 5, fmt::format("{} 5, {}: {}", Kernel::Language::get("kernel.argument_expected"), Kernel::Language::get("kernel.argument_received"), argc));
					auto s = static_cast<Data*>(JS_GetOpaque2(ctx, this_val, Detail::class_id));
					if (s == nullptr) {
						return JS_EXCEPTION;
					}
					s->set_linear_gradient(static_cast<brush_type>(JS::Converter::get_bigint64(ctx, argv[0])), JS::Converter::get_float32(ctx, argv[1]), JS::Converter::get_float32(ctx, argv[2]), JS::Converter::get_float32(ctx, argv[3]), JS::Converter::get_float32(ctx, argv[4]));
					return JS_UNDEFINED;
				}, "set_linear_gradient"_sv);
			}

			/*
				Set radial gradient
			*/

			inline static auto set_radial_gradient(
				JSContext* ctx,
				JSValueConst this_val,
				int argc,
				JSValueConst* argv
			) -> JSElement::undefined
			{
				M_JS_PROXY_WRAPPER(ctx, {
					try_assert(argc == 7, fmt::format("{} 7, {}: {}", Kernel::Language::get("kernel.argument_expected"), Kernel::Language::get("kernel.argument_received"), argc));
					auto s = static_cast<Data*>(JS_GetOpaque2(ctx, this_val, Detail::class_id));
					if (s == nullptr) {
						return JS_EXCEPTION;
					}
					s->set_radial_gradient(static_cast<brush_type>(JS::Converter::get_bigint64(ctx, argv[0])), JS::Converter::get_float32(ctx, argv[1]), JS::Converter::get_float32(ctx, argv[2]), JS::Converter::get_float32(ctx, argv[3]), JS::Converter::get_float32(ctx, argv[4]), JS::Converter::get_float32(ctx, argv[5]), JS::Converter::get_float32(ctx, argv[6]));
					return JS_UNDEFINED;
				}, "set_radial_gradient"_sv);
			}

			/*
				Add color stop
			*/

			inline static auto add_color_stop(
				JSContext* ctx,
				JSValueConst this_val,
				int argc,
				JSValueConst* argv
			) -> JSElement::undefined
			{
				M_JS_PROXY_WRAPPER(ctx, {
					try_assert(argc == 6, fmt::format("{} 6, {}: {}", Kernel::Language::get("kernel.argument_expected"), Kernel::Language::get("kernel.argument_received"), argc));
					auto s = static_cast<Data*>(JS_GetOpaque2(ctx, this_val, Detail::class_id));
					if (s == nullptr) {
						return JS_EXCEPTION;
					}
					s->add_color_stop(static_cast<brush_type>(JS::Converter::get_bigint64(ctx, argv[0])), JS::Converter::get_float32(ctx, argv[1]), JS::Converter::get_float32(ctx, argv[2]), JS::Converter::get_float32(ctx, argv[3]), JS::Converter::get_float32(ctx, argv[4]), JS::Converter::get_float32(ctx, argv[5]));
					return JS_UNDEFINED;
				}, "add_color_stop"_sv);
			}

			/*
				Begin path
			*/

			inline static auto begin_path(
				JSContext* ctx,
				JSValueConst this_val,
				int argc,
				JSValueConst* argv
			) -> JSElement::undefined
			{
				M_JS_PROXY_WRAPPER(ctx, {
					try_assert(argc == 0, fmt::format("{} 0, {}: {}", Kernel::Language::get("kernel.argument_expected"), Kernel::Language::get("kernel.argument_received"), argc));
					auto s = static_cast<Data*>(JS_GetOpaque2(ctx, this_val, Detail::class_id));
					if (s == nullptr) {
						return JS_EXCEPTION;
					}
					s->begin_path();
					return JS_UNDEFINED;
				}, "begin_path"_sv);
			}

			/*
				Move to
			*/

			inline static auto move_to(
				JSContext* ctx,
				JSValueConst this_val,
				int argc,
				JSValueConst* argv
			) -> JSElement::undefined
			{
				M_JS_PROXY_WRAPPER(ctx, {
					try_assert(argc == 2, fmt::format("{} 2, {}: {}", Kernel::Language::get("kernel.argument_expected"), Kernel::Language::get("kernel.argument_received"), argc));
					auto s = static_cast<Data*>(JS_GetOpaque2(ctx, this_val, Detail::class_id));
					if (s == nullptr) {
						return JS_EXCEPTION;
					}
					s->move_to(JS::Converter::get_float32(ctx, argv[0]), JS::Converter::get_float32(ctx, argv[1]));
					return JS_UNDEFINED;
				}, "move_to"_sv);
			}

			/*
				Close path
			*/

			inline static auto close_path(
				JSContext* ctx,
				JSValueConst this_val,
				int argc,
				JSValueConst* argv
			) -> JSElement::undefined
			{
				M_JS_PROXY_WRAPPER(ctx, {
					try_assert(argc == 0, fmt::format("{} 0, {}: {}", Kernel::Language::get("kernel.argument_expected"), Kernel::Language::get("kernel.argument_received"), argc));
					auto s = static_cast<Data*>(JS_GetOpaque2(ctx, this_val, Detail::class_id));
					if (s == nullptr) {
						return JS_EXCEPTION;
					}
					s->close_path();
					return JS_UNDEFINED;
				}, "close_path"_sv);
			}

			/*
				Line to
			*/

			inline static auto line_to(
				JSContext* ctx,
				JSValueConst this_val,
				int argc,
				JSValueConst* argv
			) -> JSElement::undefined
			{
				M_JS_PROXY_WRAPPER(ctx, {
					try_assert(argc == 2, fmt::format("{} 2, {}: {}", Kernel::Language::get("kernel.argument_expected"), Kernel::Language::get("kernel.argument_received"), argc));
					auto s = static_cast<Data*>(JS_GetOpaque2(ctx, this_val, Detail::class_id));
					if (s == nullptr) {
						return JS_EXCEPTION;
					}
					s->line_to(JS::Converter::get_float32(ctx, argv[0]), JS::Converter::get_float32(ctx, argv[1]));
					return JS_UNDEFINED;
				}, "line_to"_sv);
			}

			/*
				quadratic_curve_to
			*/

			inline static auto quadratic_curve_to(
				JSContext* ctx,
				JSValueConst this_val,
				int argc,
				JSValueConst* argv
			) -> JSElement::undefined
			{
				M_JS_PROXY_WRAPPER(ctx, {
					try_assert(argc == 4, fmt::format("{} 4, {}: {}", Kernel::Language::get("kernel.argument_expected"), Kernel::Language::get("kernel.argument_received"), argc));
					auto s = static_cast<Data*>(JS_GetOpaque2(ctx, this_val, Detail::class_id));
					if (s == nullptr) {
						return JS_EXCEPTION;
					}
					s->quadratic_curve_to(JS::Converter::get_float32(ctx, argv[0]), JS::Converter::get_float32(ctx, argv[1]), JS::Converter::get_float32(ctx, argv[2]), JS::Converter::get_float32(ctx, argv[3]));
					return JS_UNDEFINED;
				}, "quadratic_curve_to"_sv);
			}

			/*
				bezier_curve_to
			*/

			inline static auto bezier_curve_to(
				JSContext* ctx,
				JSValueConst this_val,
				int argc,
				JSValueConst* argv
			) -> JSElement::undefined
			{
				M_JS_PROXY_WRAPPER(ctx, {
					try_assert(argc == 6, fmt::format("{} 6, {}: {}", Kernel::Language::get("kernel.argument_expected"), Kernel::Language::get("kernel.argument_received"), argc));
					auto s = static_cast<Data*>(JS_GetOpaque2(ctx, this_val, Detail::class_id));
					if (s == nullptr) {
						return JS_EXCEPTION;
					}
					s->bezier_curve_to(JS::Converter::get_float32(ctx, argv[0]), JS::Converter::get_float32(ctx, argv[1]), JS::Converter::get_float32(ctx, argv[2]), JS::Converter::get_float32(ctx, argv[3]), JS::Converter::get_float32(ctx, argv[4]), JS::Converter::get_float32(ctx, argv[5]));
					return JS_UNDEFINED;
				}, "bezier_curve_to"_sv);
			}

			/*
				arc_to
			*/

			inline static auto arc_to(
				JSContext* ctx,
				JSValueConst this_val,
				int argc,
				JSValueConst* argv
			) -> JSElement::undefined
			{
				M_JS_PROXY_WRAPPER(ctx, {
					try_assert(argc == 5, fmt::format("{} 5, {}: {}", Kernel::Language::get("kernel.argument_expected"), Kernel::Language::get("kernel.argument_received"), argc));
					auto s = static_cast<Data*>(JS_GetOpaque2(ctx, this_val, Detail::class_id));
					if (s == nullptr) {
						return JS_EXCEPTION;
					}
					s->arc_to(JS::Converter::get_float32(ctx, argv[0]), JS::Converter::get_float32(ctx, argv[1]), JS::Converter::get_float32(ctx, argv[2]), JS::Converter::get_float32(ctx, argv[3]), JS::Converter::get_float32(ctx, argv[4]));
					return JS_UNDEFINED;
				}, "arc_to"_sv);
			}

			/*
				arc
			*/

			inline static auto arc(
				JSContext* ctx,
				JSValueConst this_val,
				int argc,
				JSValueConst* argv
			) -> JSElement::undefined
			{
				M_JS_PROXY_WRAPPER(ctx, {
					try_assert(argc == 6, fmt::format("{} 6, {}: {}", Kernel::Language::get("kernel.argument_expected"), Kernel::Language::get("kernel.argument_received"), argc));
					auto s = static_cast<Data*>(JS_GetOpaque2(ctx, this_val, Detail::class_id));
					if (s == nullptr) {
						return JS_EXCEPTION;
					}
					s->arc(JS::Converter::get_float32(ctx, argv[0]), JS::Converter::get_float32(ctx, argv[1]), JS::Converter::get_float32(ctx, argv[2]), JS::Converter::get_float32(ctx, argv[3]), JS::Converter::get_float32(ctx, argv[4]), JS::Converter::get_bool(ctx, argv[5]));
					return JS_UNDEFINED;
				}, "arc"_sv);
			}

			/*
				rectangle
			*/

			inline static auto rectangle(
				JSContext* ctx,
				JSValueConst this_val,
				int argc,
				JSValueConst* argv
			) -> JSElement::undefined
			{
				M_JS_PROXY_WRAPPER(ctx, {
					try_assert(argc == 4, fmt::format("{} 4, {}: {}", Kernel::Language::get("kernel.argument_expected"), Kernel::Language::get("kernel.argument_received"), argc));
					auto s = static_cast<Data*>(JS_GetOpaque2(ctx, this_val, Detail::class_id));
					if (s == nullptr) {
						return JS_EXCEPTION;
					}
					s->rectangle(JS::Converter::get_float32(ctx, argv[0]), JS::Converter::get_float32(ctx, argv[1]), JS::Converter::get_float32(ctx, argv[2]), JS::Converter::get_float32(ctx, argv[3]));
					return JS_UNDEFINED;
				}, "rectangle"_sv);
			}

			/*
				fill
			*/

			inline static auto fill(
				JSContext* ctx,
				JSValueConst this_val,
				int argc,
				JSValueConst* argv
			) -> JSElement::undefined
			{
				M_JS_PROXY_WRAPPER(ctx, {
					try_assert(argc == 0, fmt::format("{} 0, {}: {}", Kernel::Language::get("kernel.argument_expected"), Kernel::Language::get("kernel.argument_received"), argc));
					auto s = static_cast<Data*>(JS_GetOpaque2(ctx, this_val, Detail::class_id));
					if (s == nullptr) {
						return JS_EXCEPTION;
					}
					s->fill();
					return JS_UNDEFINED;
				}, "fill"_sv);
			}

			/*
				stroke
			*/

			inline static auto stroke(
				JSContext* ctx,
				JSValueConst this_val,
				int argc,
				JSValueConst* argv
			) -> JSElement::undefined
			{
				M_JS_PROXY_WRAPPER(ctx, {
					try_assert(argc == 0, fmt::format("{} 0, {}: {}", Kernel::Language::get("kernel.argument_expected"), Kernel::Language::get("kernel.argument_received"), argc));
					auto s = static_cast<Data*>(JS_GetOpaque2(ctx, this_val, Detail::class_id));
					if (s == nullptr) {
						return JS_EXCEPTION;
					}
					s->stroke();
					return JS_UNDEFINED;
				}, "stroke"_sv);
			}

			/*
				clip
			*/

			inline static auto clip(
				JSContext* ctx,
				JSValueConst this_val,
				int argc,
				JSValueConst* argv
			) -> JSElement::undefined
			{
				M_JS_PROXY_WRAPPER(ctx, {
					try_assert(argc == 0, fmt::format("{} 0, {}: {}", Kernel::Language::get("kernel.argument_expected"), Kernel::Language::get("kernel.argument_received"), argc));
					auto s = static_cast<Data*>(JS_GetOpaque2(ctx, this_val, Detail::class_id));
					if (s == nullptr) {
						return JS_EXCEPTION;
					}
					s->clip();
					return JS_UNDEFINED;
				}, "clip"_sv);
			}

			/*
				is_point_in_path
			*/

			inline static auto is_point_in_path(
				JSContext* ctx,
				JSValueConst this_val,
				int argc,
				JSValueConst* argv
			) -> JSElement::undefined
			{
				M_JS_PROXY_WRAPPER(ctx, {
					try_assert(argc == 2, fmt::format("{} 2, {}: {}", Kernel::Language::get("kernel.argument_expected"), Kernel::Language::get("kernel.argument_received"), argc));
					auto s = static_cast<Data*>(JS_GetOpaque2(ctx, this_val, Detail::class_id));
					if (s == nullptr) {
						return JS_EXCEPTION;
					}
					return JS::Converter::to_bool(ctx, s->is_point_in_path(JS::Converter::get_float32(ctx, argv[0]), JS::Converter::get_float32(ctx, argv[1])));
				}, "is_point_in_path"_sv);
			}

			/*
				clear rectangle
			*/

			inline static auto clear_rectangle(
				JSContext* ctx,
				JSValueConst this_val,
				int argc,
				JSValueConst* argv
			) -> JSElement::undefined
			{
				M_JS_PROXY_WRAPPER(ctx, {
					try_assert(argc == 4, fmt::format("{} 4, {}: {}", Kernel::Language::get("kernel.argument_expected"), Kernel::Language::get("kernel.argument_received"), argc));
					auto s = static_cast<Data*>(JS_GetOpaque2(ctx, this_val, Detail::class_id));
					if (s == nullptr) {
						return JS_EXCEPTION;
					}
					s->clear_rectangle(JS::Converter::get_float32(ctx, argv[0]), JS::Converter::get_float32(ctx, argv[1]), JS::Converter::get_float32(ctx, argv[2]), JS::Converter::get_float32(ctx, argv[3]));
					return JS_UNDEFINED;
				}, "clear_rectangle"_sv);
			}

			/*
				fill rectangle
			*/

			inline static auto fill_rectangle(
				JSContext* ctx,
				JSValueConst this_val,
				int argc,
				JSValueConst* argv
			) -> JSElement::undefined
			{
				M_JS_PROXY_WRAPPER(ctx, {
					try_assert(argc == 4, fmt::format("{} 4, {}: {}", Kernel::Language::get("kernel.argument_expected"), Kernel::Language::get("kernel.argument_received"), argc));
					auto s = static_cast<Data*>(JS_GetOpaque2(ctx, this_val, Detail::class_id));
					if (s == nullptr) {
						return JS_EXCEPTION;
					}
					s->fill_rectangle(JS::Converter::get_float32(ctx, argv[0]), JS::Converter::get_float32(ctx, argv[1]), JS::Converter::get_float32(ctx, argv[2]), JS::Converter::get_float32(ctx, argv[3]));
					return JS_UNDEFINED;
				}, "fill_rectangle"_sv);
			}

			/*
				stroke rectangle
			*/

			inline static auto stroke_rectangle(
				JSContext* ctx,
				JSValueConst this_val,
				int argc,
				JSValueConst* argv
			) -> JSElement::undefined
			{
				M_JS_PROXY_WRAPPER(ctx, {
					try_assert(argc == 4, fmt::format("{} 4, {}: {}", Kernel::Language::get("kernel.argument_expected"), Kernel::Language::get("kernel.argument_received"), argc));
					auto s = static_cast<Data*>(JS_GetOpaque2(ctx, this_val, Detail::class_id));
					if (s == nullptr) {
						return JS_EXCEPTION;
					}
					s->stroke_rectangle(JS::Converter::get_float32(ctx, argv[0]), JS::Converter::get_float32(ctx, argv[1]), JS::Converter::get_float32(ctx, argv[2]), JS::Converter::get_float32(ctx, argv[3]));
					return JS_UNDEFINED;
				}, "stroke_rectangle"_sv);
			}

			/*
				set font
			*/

			inline static auto set_font(
				JSContext* ctx,
				JSValueConst this_val,
				int argc,
				JSValueConst* argv
			) -> JSElement::undefined
			{
				M_JS_PROXY_WRAPPER(ctx, {
					try_assert(argc == 2, fmt::format("{} 2, {}: {}", Kernel::Language::get("kernel.argument_expected"), Kernel::Language::get("kernel.argument_received"), argc));
					auto s = static_cast<Data*>(JS_GetOpaque2(ctx, this_val, Detail::class_id));
					if (s == nullptr) {
						return JS_EXCEPTION;
					}
					auto size = std::size_t{};
					auto data = JS_GetArrayBuffer(ctx, &size, argv[0]);
					return JS::Converter::to_bool(ctx, s->set_font(data, static_cast<int>(JS::Converter::get_bigint64(ctx, argv[1])), size));
				}, "set_font"_sv);
			}

			/*
				draw image
			*/

			inline static auto draw_image(
				JSContext* ctx,
				JSValueConst this_val,
				int argc,
				JSValueConst* argv
			) -> JSElement::undefined
			{
				M_JS_PROXY_WRAPPER(ctx, {
					try_assert(argc == 8, fmt::format("{} 8, {}: {}", Kernel::Language::get("kernel.argument_expected"), Kernel::Language::get("kernel.argument_received"), argc));
					auto s = static_cast<Data*>(JS_GetOpaque2(ctx, this_val, Detail::class_id));
					if (s == nullptr) {
						return JS_EXCEPTION;
					}
					auto size = std::size_t{};
					auto data = JS_GetArrayBuffer(ctx, &size, argv[0]);
					s->draw_image(data, static_cast<int>(JS::Converter::get_bigint64(ctx, argv[1])), 
						static_cast<int>(JS::Converter::get_bigint64(ctx, argv[2])), static_cast<int>(JS::Converter::get_bigint64(ctx, argv[3])),
						static_cast<float>(JS::Converter::get_float32(ctx, argv[4])), static_cast<float>(JS::Converter::get_float32(ctx, argv[5])),
						static_cast<float>(JS::Converter::get_float32(ctx, argv[6])), static_cast<float>(JS::Converter::get_float32(ctx, argv[7])));
					return JS::Converter::get_undefined();
				}, "draw_image"_sv);
			}

			/*
				get_image_data
			*/

			inline static auto get_image_data(
				JSContext* ctx,
				JSValueConst this_val,
				int argc,
				JSValueConst* argv
			) -> JSElement::undefined
			{
				M_JS_PROXY_WRAPPER(ctx, {
					try_assert(argc == 6, fmt::format("{} 6, {}: {}", Kernel::Language::get("kernel.argument_expected"), Kernel::Language::get("kernel.argument_received"), argc));
					auto s = static_cast<Data*>(JS_GetOpaque2(ctx, this_val, Detail::class_id));
					if (s == nullptr) {
						return JS_EXCEPTION;
					}
					auto size = std::size_t{};
					auto data = JS_GetArrayBuffer(ctx, &size, argv[0]);
					s->get_image_data(data, static_cast<int>(JS::Converter::get_bigint64(ctx, argv[1])),
						static_cast<int>(JS::Converter::get_bigint64(ctx, argv[2])), static_cast<int>(JS::Converter::get_bigint64(ctx, argv[3])),
						static_cast<int>(JS::Converter::get_bigint64(ctx, argv[4])), static_cast<int>(JS::Converter::get_bigint64(ctx, argv[5])));
					return JS::Converter::get_undefined();
				}, "get_image_data"_sv);
			}

			/*
				put_image_data
			*/

			inline static auto put_image_data(
				JSContext* ctx,
				JSValueConst this_val,
				int argc,
				JSValueConst* argv
			) -> JSElement::undefined
			{
				M_JS_PROXY_WRAPPER(ctx, {
					try_assert(argc == 6, fmt::format("{} 6, {}: {}", Kernel::Language::get("kernel.argument_expected"), Kernel::Language::get("kernel.argument_received"), argc));
					auto s = static_cast<Data*>(JS_GetOpaque2(ctx, this_val, Detail::class_id));
					if (s == nullptr) {
						return JS_EXCEPTION;
					}
					auto size = std::size_t{};
					auto data = JS_GetArrayBuffer(ctx, &size, argv[0]);
					s->put_image_data(data, static_cast<int>(JS::Converter::get_bigint64(ctx, argv[1])),
						static_cast<int>(JS::Converter::get_bigint64(ctx, argv[2])), static_cast<int>(JS::Converter::get_bigint64(ctx, argv[3])),
						static_cast<int>(JS::Converter::get_bigint64(ctx, argv[4])), static_cast<int>(JS::Converter::get_bigint64(ctx, argv[5])));
					return JS::Converter::get_undefined();
				}, "put_image_data"_sv);
			}

			/*
				save
			*/

			inline static auto save(
				JSContext* ctx,
				JSValueConst this_val,
				int argc,
				JSValueConst* argv
			) -> JSElement::undefined
			{
				M_JS_PROXY_WRAPPER(ctx, {
					try_assert(argc == 0, fmt::format("{} 0, {}: {}", Kernel::Language::get("kernel.argument_expected"), Kernel::Language::get("kernel.argument_received"), argc));
					auto s = static_cast<Data*>(JS_GetOpaque2(ctx, this_val, Detail::class_id));
					if (s == nullptr) {
						return JS_EXCEPTION;
					}
					s->save();
					return JS_UNDEFINED;
				}, "save"_sv);
			}

			/*
				restore
			*/

			inline static auto restore(
				JSContext* ctx,
				JSValueConst this_val,
				int argc,
				JSValueConst* argv
			) -> JSElement::undefined
			{
				M_JS_PROXY_WRAPPER(ctx, {
					try_assert(argc == 0, fmt::format("{} 0, {}: {}", Kernel::Language::get("kernel.argument_expected"), Kernel::Language::get("kernel.argument_received"), argc));
					auto s = static_cast<Data*>(JS_GetOpaque2(ctx, this_val, Detail::class_id));
					if (s == nullptr) {
						return JS_EXCEPTION;
					}
					s->restore();
					return JS_UNDEFINED;
				}, "restore"_sv);
			}

			// Function

			inline static const JSCFunctionListEntry proto_functions[] = {
				JS_CPPFUNC_DEF("scale", 2, scale),
				JS_CPPFUNC_DEF("rotate", 1, rotate),
				JS_CPPFUNC_DEF("translate", 2, translate),
				JS_CPPFUNC_DEF("transform", 2, transform),
				JS_CPPFUNC_DEF("set_transform", 2, set_transform),
				JS_CPPFUNC_DEF("set_global_alpha", 1, set_global_alpha),
				JS_CPPFUNC_DEF("set_shadow_color", 2, set_shadow_color),
				JS_CPPFUNC_DEF("set_shadow_blur", 1, set_shadow_blur),
				JS_CPPFUNC_DEF("set_line_width", 1, set_line_width),
				JS_CPPFUNC_DEF("set_miter_limit", 1, set_miter_limit),
				JS_CPPFUNC_DEF("set_linear_gradient", 2, set_linear_gradient),
				JS_CPPFUNC_DEF("set_color", 2, set_color),
				JS_CPPFUNC_DEF("set_radial_gradient", 2, set_radial_gradient),
				JS_CPPFUNC_DEF("add_color_stop", 2, add_color_stop),
				JS_CPPFUNC_DEF("begin_path", 0, begin_path),
				JS_CPPFUNC_DEF("close_path", 0, close_path),
				JS_CPPFUNC_DEF("move_to", 2, move_to),
				JS_CPPFUNC_DEF("line_to", 2, line_to),
				JS_CPPFUNC_DEF("quadratic_curve_to", 2, quadratic_curve_to),
				JS_CPPFUNC_DEF("bezier_curve_to", 2, bezier_curve_to),
				JS_CPPFUNC_DEF("arc_to", 2, arc_to),
				JS_CPPFUNC_DEF("arc", 2, arc),
				JS_CPPFUNC_DEF("rectangle", 2, rectangle),
				JS_CPPFUNC_DEF("fill", 0, fill),
				JS_CPPFUNC_DEF("stroke", 0, stroke),
				JS_CPPFUNC_DEF("clip", 0, clip),
				JS_CPPFUNC_DEF("is_point_in_path", 1, is_point_in_path),
				JS_CPPFUNC_DEF("clear_rectangle", 2, clear_rectangle),
				JS_CPPFUNC_DEF("fill_rectangle", 2, fill_rectangle),
				JS_CPPFUNC_DEF("stroke_rectangle", 2, stroke_rectangle),
				JS_CPPFUNC_DEF("set_font", 2, set_font),
				JS_CPPFUNC_DEF("draw_image", 2, draw_image),
				JS_CPPFUNC_DEF("get_image_data", 2, get_image_data),
				JS_CPPFUNC_DEF("put_image_data", 2, put_image_data),
				JS_CPPFUNC_DEF("save", 0, save),
				JS_CPPFUNC_DEF("restore", 0, restore),
			};

			// Proxy call

			inline static auto register_class(
				JSContext* ctx
			) -> void
			{
				JS_NewClassID(&(Detail::class_id));
				assert_conditional(JS_NewClass(JS_GetRuntime(ctx), Detail::class_id, &this_class) == 0, "Canvas class register failed", "register_class");
				auto class_name = "Canvas"_sv;
				auto point_ctor = JS_NewCFunction2(ctx, constructor, class_name.data(), 2, JS_CFUNC_constructor, 0);
				auto proto = JS_NewObject(ctx);
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

		// Because Image is duplicated with the namespace

		namespace DimensionView {

			using Data = Kernel::Definition::Dimension<int>;

			inline static JSClassID class_id;

			inline static auto finalizer(
				JSRuntime* rt,
				JSValue val
			) -> void
			{
				auto s = static_cast<Data*>(JS_GetOpaque(val, class_id));
				if (s != nullptr) {
					delete s;
				}
				return;
			}

			enum Magic {
				width,
				height,
			};

			inline static auto constructor(
				JSContext* ctx,
				JSValueConst new_target,
				int argc,
				JSValueConst* argv
			) -> JSElement::undefined
			{
				M_JS_PROXY_WRAPPER(ctx, {
					auto s = static_cast<Data*>(nullptr);
					auto obj = JS_UNDEFINED;
					auto proto = JSDefine::ImageView{};
					if (argc == 1) {
						auto width_val = JS_GetPropertyStr(ctx, argv[0], "width");
						auto height_val = JS_GetPropertyStr(ctx, argv[0], "height");
						auto width = int64_t{};
						auto height = int64_t{};
						JS_ToBigInt64(ctx, &width, width_val);
						JS_ToBigInt64(ctx, &height, height_val);
						s = new Data(
							static_cast<int>(width),
							static_cast<int>(height)
						);
						JS_FreeValue(ctx, width_val);
						JS_FreeValue(ctx, height_val);
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
					if (JS_IsException(obj)) {
						goto fail;
					}
					JS_SetOpaque(obj, s);
					return obj;
				fail:
					js_free(ctx, s);
					JS_FreeValue(ctx, obj);
					return JS_EXCEPTION; 
				}, "proxy_constructor");
			}

			/*
				Current class
			*/

			inline static auto this_class = JSClassDef{
				.class_name = "DimensionView",
				.finalizer = finalizer,
			};


			// Getter

			inline static auto getter(
				JSContext* ctx,
				JSValueConst this_val,
				int magic
			) -> JSElement::any
			{
				auto s = static_cast<Data*>(JS_GetOpaque2(ctx, this_val, class_id));
				if (s == nullptr) {
					return JS_EXCEPTION;
				}
				switch (static_cast<DimensionView::Magic>(magic)) {
					case DimensionView::Magic::width: {
						return JS::Converter::to_bigint(ctx, static_cast<std::int64_t>(s->width));
					}
					case DimensionView::Magic::height: {
						return JS::Converter::to_bigint(ctx, static_cast<std::int64_t>(s->height));
					}
					default: {
						return JS_UNDEFINED;
					}
				}
			}

			// Setter

			inline static auto setter(
				JSContext* ctx,
				JSValueConst this_val,
				JSValueConst val,
				int magic
			) -> JSElement::undefined
			{
				auto s = static_cast<Data*>(JS_GetOpaque2(ctx, this_val, class_id));
				auto v = bool{};
				if (s == nullptr) {
					return JS_EXCEPTION;
				}
				switch (static_cast<DimensionView::Magic>(magic)) {
					case DimensionView::Magic::width: {
						s->width = static_cast<int>(JS::Converter::get_bigint64(ctx, val));
						break;
					}
					case DimensionView::Magic::height: {
						s->height = static_cast<int>(JS::Converter::get_bigint64(ctx, val));
						break;
					}
					default: {
						return JS_UNDEFINED;
					}
				}
				return JS_UNDEFINED;
			}


			// Function

			inline static const JSCFunctionListEntry proto_functions[] = {
				JS_CPPGETSET_MAGIC_DEF("width", getter, setter, DimensionView::Magic::width),
				JS_CPPGETSET_MAGIC_DEF("height", getter, setter, DimensionView::Magic::height),
			};


			inline static auto register_class(
				JSContext* ctx
			) -> void
			{
				JS_NewClassID(&class_id);
				assert_conditional(JS_NewClass(JS_GetRuntime(ctx), class_id, &this_class) == 0, "DimensionView class failed register", "register_class");
				auto class_name = "DimensionView"_sv;
				auto point_ctor = JS_NewCFunction2(ctx, constructor, class_name.data(), 2, JS_CFUNC_constructor, 0);
				auto proto = JS_NewObject(ctx);
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

		// Rectangle class

		namespace Rectangle {

			using Data = Kernel::Definition::Rectangle<int>;

			inline static JSClassID class_id;

			inline static auto finalizer(
				JSRuntime* rt,
				JSValue val
			) -> void
			{
				auto s = static_cast<Data*>(JS_GetOpaque(val, class_id));
				if (s != nullptr) {
					delete s;
				}
				return;
			}

			enum Magic {
				x,
				y,
				width,
				height,
			};

			inline static auto constructor(
				JSContext* ctx,
				JSValueConst new_target,
				int argc,
				JSValueConst* argv
			) -> JSElement::undefined
			{
				M_JS_PROXY_WRAPPER(ctx, {
					auto s = static_cast<Data*>(nullptr);
					auto obj = JS_UNDEFINED;
					auto proto = JSDefine::ImageView{};
					if (argc == 1) {
						auto width_val = JS_GetPropertyStr(ctx, argv[0], "width");
						auto height_val = JS_GetPropertyStr(ctx, argv[0], "height");
						auto x_val = JS_GetPropertyStr(ctx, argv[0], "x");
						auto y_val = JS_GetPropertyStr(ctx, argv[0], "y");
						auto width = int64_t{};
						auto height = int64_t{};
						auto x = int64_t{};
						auto y = int64_t{};
						JS_ToBigInt64(ctx, &width, width_val);
						JS_ToBigInt64(ctx, &height, height_val);
						JS_ToBigInt64(ctx, &x, x_val);
						JS_ToBigInt64(ctx, &y, y_val);
						s = new Data(
							x,
							y,
							static_cast<int>(width),
							static_cast<int>(height)
						);
						JS_FreeValue(ctx, width_val);
						JS_FreeValue(ctx, height_val);
						JS_FreeValue(ctx, x_val);
						JS_FreeValue(ctx, y_val);
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
					if (JS_IsException(obj)) {
						goto fail;
					}
					JS_SetOpaque(obj, s);
					return obj;
				fail:
					js_free(ctx, s);
					JS_FreeValue(ctx, obj);
					return JS_EXCEPTION; 
				}, "proxy_constructor");
			}

			/*
				Current class
			*/

			inline static auto this_class = JSClassDef{
				.class_name = "Rectangle",
				.finalizer = finalizer,
			};


			// Getter

			inline static auto getter(
				JSContext* ctx,
				JSValueConst this_val,
				int magic
			) -> JSElement::any
			{
				auto s = static_cast<Data*>(JS_GetOpaque2(ctx, this_val, class_id));
				if (s == nullptr) {
					return JS_EXCEPTION;
				}
				switch (static_cast<Rectangle::Magic>(magic)) {
					case Rectangle::Magic::x: {
						return JS::Converter::to_bigint(ctx, static_cast<std::int64_t>(s->x));
					}
					case Rectangle::Magic::y: {
						return JS::Converter::to_bigint(ctx, static_cast<std::int64_t>(s->y));
					}
					case Rectangle::Magic::width: {
						return JS::Converter::to_bigint(ctx, static_cast<std::int64_t>(s->width));
					}
					case Rectangle::Magic::height: {
						return JS::Converter::to_bigint(ctx, static_cast<std::int64_t>(s->height));
					}
					default: {
						return JS_UNDEFINED;
					}
				}
			}

			// Setter

			inline static auto setter(
				JSContext* ctx,
				JSValueConst this_val,
				JSValueConst val,
				int magic
			) -> JSElement::undefined
			{
				auto s = static_cast<Data*>(JS_GetOpaque2(ctx, this_val, class_id));
				auto v = bool{};
				if (s == nullptr) {
					return JS_EXCEPTION;
				}
				switch (static_cast<Rectangle::Magic>(magic)) {
					case Rectangle::Magic::x: {
						s->x = static_cast<int>(JS::Converter::get_bigint64(ctx, val));
						break;
					}
					case Rectangle::Magic::y: {
						s->y = static_cast<int>(JS::Converter::get_bigint64(ctx, val));
						break;
					}
					case Rectangle::Magic::width: {
						s->width = static_cast<int>(JS::Converter::get_bigint64(ctx, val));
						break;
					}
					case Rectangle::Magic::height: {
						s->height = static_cast<int>(JS::Converter::get_bigint64(ctx, val));
						break;
					}
					default: {
						return JS_UNDEFINED;
					}
				}
				return JS_UNDEFINED;
			}

			// Function

			inline static const JSCFunctionListEntry proto_functions[] = {
				JS_CPPGETSET_MAGIC_DEF("x", getter, setter, Rectangle::Magic::x),
				JS_CPPGETSET_MAGIC_DEF("y", getter, setter, Rectangle::Magic::y),
				JS_CPPGETSET_MAGIC_DEF("width", getter, setter, Rectangle::Magic::width),
				JS_CPPGETSET_MAGIC_DEF("height", getter, setter, Rectangle::Magic::height),
			};

			// Adapter

			inline static auto register_class(
				JSContext* ctx
			) -> void
			{
				JS_NewClassID(&class_id);
				assert_conditional(JS_NewClass(JS_GetRuntime(ctx), class_id, &this_class) == 0, "Rectangle class register failed", "register_class");
				auto class_name = "Rectangle"_sv;
				auto point_ctor = JS_NewCFunction2(ctx, constructor, class_name.data(), 2, JS_CFUNC_constructor, 0);
				auto proto = JS_NewObject(ctx);
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

		// Because Image is duplicated with the namespace

		namespace ImageView {
		
			using Data = Kernel::Definition::Image<int>;

			inline static JSClassID class_id;

			inline static auto finalizer(
				JSRuntime* rt,
				JSValue val
			) -> void
			{
				auto s = static_cast<Data*>(JS_GetOpaque(val, class_id));
				if (s != nullptr) {
					delete s;
				}
				return;
			}

			enum Magic {
				bit_depth,
				color_type,
				interlace_type,
				channels,
				rowbytes,
				data,
				width,
				height,
			};

			inline static auto constructor(
				JSContext* ctx,
				JSValueConst new_target,
				int argc,
				JSValueConst* argv
			) -> JSElement::undefined
			{
				M_JS_PROXY_WRAPPER(ctx, {
					auto s = static_cast<Data*>(nullptr);
					auto obj = JS_UNDEFINED;
					auto proto = JSDefine::ImageView{};
					if (argc == 1) {
						auto width_val = JS_GetPropertyStr(ctx, argv[0], "width");
						auto height_val = JS_GetPropertyStr(ctx, argv[0], "height");
						auto bit_depth_val = JS_GetPropertyStr(ctx, argv[0], "bit_depth");
						auto color_type_val = JS_GetPropertyStr(ctx, argv[0], "color_type");
						auto interlace_type_val = JS_GetPropertyStr(ctx, argv[0], "interlace_type");
						auto channels_val = JS_GetPropertyStr(ctx, argv[0], "channels");
						auto rowbytes_val = JS_GetPropertyStr(ctx, argv[0], "rowbytes");
						auto data_val = JS_GetPropertyStr(ctx, argv[0], "data");
						auto width = int64_t{};
						auto height = int64_t{};
						auto bit_depth = int64_t{};
						auto color_type = int64_t{};
						auto interlace_type = int64_t{};
						auto channels = int64_t{};
						auto rowbytes = int64_t{};
						auto data_len = size_t{};
						JS_ToBigInt64(ctx, &width, width_val);
						JS_ToBigInt64(ctx, &height, height_val);
						JS_ToBigInt64(ctx, &bit_depth, bit_depth_val);
						JS_ToBigInt64(ctx, &color_type, color_type_val);
						JS_ToBigInt64(ctx, &interlace_type, interlace_type_val);
						JS_ToBigInt64(ctx, &channels, channels_val);
						JS_ToBigInt64(ctx, &rowbytes, rowbytes_val);
						auto data = JS_GetArrayBuffer(ctx, &data_len, data_val);
						s = new Data(
							static_cast<int>(width),
							static_cast<int>(height),
							static_cast<int>(bit_depth),
							static_cast<int>(color_type),
							static_cast<int>(interlace_type),
							static_cast<int>(channels),
							static_cast<int>(rowbytes),
							std::move(std::vector<uint8_t>(data, data + data_len))
						);
						JS_FreeValue(ctx, width_val);
						JS_FreeValue(ctx, height_val);
						JS_FreeValue(ctx, bit_depth_val);
						JS_FreeValue(ctx, color_type_val);
						JS_FreeValue(ctx, interlace_type_val);
						JS_FreeValue(ctx, channels_val);
						JS_FreeValue(ctx, rowbytes_val);
						JS_FreeValue(ctx, data_val);
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
					if (JS_IsException(obj)) {
						goto fail;
					}
					JS_SetOpaque(obj, s);
					return obj;
				fail:
					js_free(ctx, s);
					JS_FreeValue(ctx, obj);
					return JS_EXCEPTION; 
				}, "proxy_constructor");
			}

			/*
				Current class
			*/

			inline static auto this_class = JSClassDef{
				.class_name = "ImageView",
				.finalizer = finalizer,
			};


			// Getter

			inline static auto getter(
				JSContext* ctx,
				JSValueConst this_val,
				int magic
			) -> JSElement::any
			{
				auto s = static_cast<Data*>(JS_GetOpaque2(ctx, this_val, class_id));
				if (s == nullptr) {
					return JS_EXCEPTION;
				}
				switch (static_cast<ImageView::Magic>(magic)) {
					case ImageView::Magic::bit_depth: {
						return JS::Converter::to_bigint(ctx, static_cast<std::int64_t>(s->bit_depth));
					}
					case ImageView::Magic::channels: {
						return JS::Converter::to_bigint(ctx, static_cast<std::int64_t>(s->channels));
					}
					case ImageView::Magic::color_type: {
						return JS::Converter::to_bigint(ctx, static_cast<std::int64_t>(s->color_type));
					}
					case ImageView::Magic::interlace_type: {
						return JS::Converter::to_bigint(ctx, static_cast<std::int64_t>(s->interlace_type));
					}
					case ImageView::Magic::rowbytes: {
						return JS::Converter::to_bigint(ctx, static_cast<std::int64_t>(s->rowbytes));
					}
					case ImageView::Magic::width: {
						return JS::Converter::to_bigint(ctx, static_cast<std::int64_t>(s->width));
					}
					case ImageView::Magic::height: {
						return JS::Converter::to_bigint(ctx, static_cast<std::int64_t>(s->height));
					}
					case ImageView::Magic::data: {
						return JS_NewArrayBufferCopy(ctx, s->data().data(), s->data().size());
					}
					default: {
						return JS_UNDEFINED;
					}
				}
			}

			// Setter

			inline static auto setter(
				JSContext* ctx,
				JSValueConst this_val,
				JSValueConst val,
				int magic
			) -> JSElement::undefined
			{
				auto s = static_cast<Data*>(JS_GetOpaque2(ctx, this_val, class_id));
				if (s == nullptr) {
					return JS_EXCEPTION;
				}
				switch (static_cast<ImageView::Magic>(magic)) {
					case ImageView::Magic::bit_depth: {
						s->bit_depth = static_cast<int>(JS::Converter::get_bigint64(ctx, val));
						break;
					}
					case ImageView::Magic::channels: {
						s->channels = static_cast<int>(JS::Converter::get_bigint64(ctx, val));
						break;
					}
					case ImageView::Magic::color_type: {
						s->color_type = static_cast<int>(JS::Converter::get_bigint64(ctx, val));
						break;
					}
					case ImageView::Magic::interlace_type: {
						s->interlace_type = static_cast<int>(JS::Converter::get_bigint64(ctx, val));
						break;
					}
					case ImageView::Magic::rowbytes: {
						s->rowbytes = static_cast<int>(JS::Converter::get_bigint64(ctx, val));
						break;
					}
					case ImageView::Magic::width: {
						s->width = static_cast<int>(JS::Converter::get_bigint64(ctx, val));
						break;
					}
					case ImageView::Magic::height: {
						s->height = static_cast<int>(JS::Converter::get_bigint64(ctx, val));
						break;
					}
					case ImageView::Magic::data: {
						auto size = std::size_t{};
						auto data = JS_GetArrayBuffer(ctx, &size, val);
						if (!data) {
							throw Exception(fmt::format("{}", Kernel::Language::get("js.converter.failed_to_get_js_array_buffer")), std::source_location::current(), "random");
						}
						s->set_data(std::vector<std::uint8_t>{data, data + size});
						break;
					}
					default: {
						return JS_UNDEFINED;
					}
				}
				return JS_UNDEFINED;
			}

			/*
				Get area
			*/

			inline static auto area(
				JSContext* ctx,
				JSValueConst this_val,
				int argc,
				JSValueConst* argv
			) -> JSElement::bigint
			{
				M_JS_PROXY_WRAPPER(ctx, {
					try_assert(argc == 0, fmt::format("{} 0, {}: {}", Kernel::Language::get("kernel.argument_expected"), Kernel::Language::get("kernel.argument_received"), argc));
					auto s = static_cast<Data*>(JS_GetOpaque2(ctx, this_val, class_id));
					if (s == nullptr) {
						return JS_EXCEPTION;
					}
					return JS::Converter::to_bigint<uint64_t>(ctx, s->area());
				}, "area"_sv);
			}

			/*
				Get circumference
			*/

			inline static auto circumference(
				JSContext* ctx,
				JSValueConst this_val,
				int argc,
				JSValueConst* argv
			) -> JSElement::bigint
			{
				M_JS_PROXY_WRAPPER(ctx, {
					try_assert(argc == 0, fmt::format("{} 0, {}: {}", Kernel::Language::get("kernel.argument_expected"), Kernel::Language::get("kernel.argument_received"), argc));
					auto s = static_cast<Data*>(JS_GetOpaque2(ctx, this_val, class_id));
					if (s == nullptr) {
						return JS_EXCEPTION;
					}
					return JS::Converter::to_bigint<uint64_t>(ctx, s->circumference());
				}, "circumference"_sv);
			}

			template <typename T> requires std::is_integral<T>::value
			using Rectangle = Kernel::Definition::Rectangle<T>;

			// Cut

			inline static auto cut(
				JSContext* ctx,
				JSValueConst this_val,
				int argc,
				JSValueConst* argv
			) -> JSDefine::ImageView
			{
				M_JS_PROXY_WRAPPER(ctx, {
					try_assert(argc == 2, fmt::format("{} 2, {}: {}", Kernel::Language::get("kernel.argument_expected"), Kernel::Language::get("kernel.argument_received"), argc));
					auto s = static_cast<Data*>(JS_GetOpaque2(ctx, argv[0], class_id));
					if (s == nullptr) {
						return JS_EXCEPTION;
					}
					auto x = JS_GetPropertyStr(ctx, argv[1], "x");
					auto y = JS_GetPropertyStr(ctx, argv[1], "y");
					auto width = JS_GetPropertyStr(ctx, argv[1], "width");
					auto height = JS_GetPropertyStr(ctx, argv[1], "height");
					auto data = new Data(std::move(Data::cut(*s, Rectangle<int>(static_cast<int>(JS::Converter::get_bigint64(ctx, x)), static_cast<int>(JS::Converter::get_bigint64(ctx, y)), static_cast<int>(JS::Converter::get_bigint64(ctx, width)), static_cast<int>(JS::Converter::get_bigint64(ctx, height))))));
					JS_FreeValue(ctx, x);
					JS_FreeValue(ctx, y);
					JS_FreeValue(ctx, width);
					JS_FreeValue(ctx, height);
					auto proto = JS_GetPropertyStr(ctx, this_val, "prototype");
					if (JS_IsException(proto)) {
						js_free(ctx, data);
						return JS_EXCEPTION;
					}
					auto obj = JS_NewObjectProtoClass(ctx, proto, class_id);
					JS_FreeValue(ctx, proto);
					if (JS_IsException(obj)) {
						js_free(ctx, data);
						return JS_EXCEPTION;
					}
					JS_SetOpaque(obj, data);
					return obj;
				}, "cut"_sv);
			}

			// Resize

			inline static auto resize(
				JSContext* ctx,
				JSValueConst this_val,
				int argc,
				JSValueConst* argv
			) -> JSDefine::ImageView
			{
				M_JS_PROXY_WRAPPER(ctx, {
					try_assert(argc == 2, fmt::format("{} 2, {}: {}", Kernel::Language::get("kernel.argument_expected"), Kernel::Language::get("kernel.argument_received"), argc));
					auto s = static_cast<Data*>(JS_GetOpaque2(ctx, argv[0], class_id));
					if (s == nullptr) {
						return JS_EXCEPTION;
					}
					auto data = new Data(std::move(Data::resize(*s, JS::Converter::get_float32(ctx, argv[1]))));
					auto proto = JS_GetPropertyStr(ctx, this_val, "prototype");
					if (JS_IsException(proto)) {
						js_free(ctx, data);
						return JS_EXCEPTION;
					}
					auto obj = JS_NewObjectProtoClass(ctx, proto, class_id);
					JS_FreeValue(ctx, proto);
					if (JS_IsException(obj)) {
						js_free(ctx, data);
						return JS_EXCEPTION;
					}
					JS_SetOpaque(obj, data);
					return obj;
				}, "resize"_sv);
			}

			// Scale

			inline static auto scale(
				JSContext* ctx,
				JSValueConst this_val,
				int argc,
				JSValueConst* argv
			) -> JSDefine::ImageView
			{
				M_JS_PROXY_WRAPPER(ctx, {
					try_assert(argc == 2, fmt::format("{} 2, {}: {}", Kernel::Language::get("kernel.argument_expected"), Kernel::Language::get("kernel.argument_received"), argc));
					auto s = static_cast<Data*>(JS_GetOpaque2(ctx, argv[0], class_id));
					if (s == nullptr) {
						return JS_EXCEPTION;
					}
					auto data = new Data(std::move(Data::scale(*s, JS::Converter::get_float64(ctx, argv[1]))));
					auto proto = JS_GetPropertyStr(ctx, this_val, "prototype");
					if (JS_IsException(proto)) {
						js_free(ctx, data);
						return JS_EXCEPTION;
					}
					auto obj = JS_NewObjectProtoClass(ctx, proto, class_id);
					JS_FreeValue(ctx, proto);
					if (JS_IsException(obj)) {
						js_free(ctx, data);
						return JS_EXCEPTION;
					}
					JS_SetOpaque(obj, data);
					return obj;
				}, "scale"_sv);
			}

			// Rotate

			inline static auto rotate(
				JSContext* ctx,
				JSValueConst this_val,
				int argc,
				JSValueConst* argv
			) -> JSDefine::ImageView
			{
				M_JS_PROXY_WRAPPER(ctx, {
					try_assert(argc == 2, fmt::format("{} 2, {}: {}", Kernel::Language::get("kernel.argument_expected"), Kernel::Language::get("kernel.argument_received"), argc));
					auto s = static_cast<Data*>(JS_GetOpaque2(ctx, argv[0], class_id));
					if (s == nullptr) {
						return JS_EXCEPTION;
					}
					auto data = new Data(std::move(Data::rotate(*s, JS::Converter::get_float64(ctx, argv[1]))));
					auto proto = JS_GetPropertyStr(ctx, this_val, "prototype");
					if (JS_IsException(proto)) {
						js_free(ctx, data);
						return JS_EXCEPTION;
					}
					auto obj = JS_NewObjectProtoClass(ctx, proto, class_id);
					JS_FreeValue(ctx, proto);
					if (JS_IsException(obj)) {
						js_free(ctx, data);
						return JS_EXCEPTION;
					}
					JS_SetOpaque(obj, data);
					return obj;
				}, "rotate"_sv);
			}

			// Write

			inline static auto write_fs(
				JSContext* ctx,
				JSValueConst this_val,
				int argc,
				JSValueConst* argv
			) -> JSElement::undefined
			{
				M_JS_PROXY_WRAPPER(ctx, {
					try_assert(argc == 2, fmt::format("{} 2, {}: {}", Kernel::Language::get("kernel.argument_expected"), Kernel::Language::get("kernel.argument_received"), argc));
					auto s = static_cast<Data*>(JS_GetOpaque2(ctx, argv[1], class_id));
					if (s == nullptr) {
						return JS_EXCEPTION;
					}
					Kernel::Definition::ImageIO::write_png(JS::Converter::get_string(ctx, argv[0]), *s);
					return JS_UNDEFINED;
				}, "write_fs"_sv);
			}

			// Read

			inline static auto read_fs(
				JSContext* ctx,
				JSValueConst this_val,
				int argc,
				JSValueConst* argv
			) -> JSDefine::ImageView
			{
				M_JS_PROXY_WRAPPER(ctx, {
					try_assert(argc == 1, fmt::format("{} 1, {}: {}", Kernel::Language::get("kernel.argument_expected"), Kernel::Language::get("kernel.argument_received"), argc));
					auto data = new Data(std::move(Kernel::Definition::ImageIO::read_png(JS::Converter::get_string(ctx, argv[0]))));
					auto proto = JS_GetPropertyStr(ctx, this_val, "prototype");
					if (JS_IsException(proto)) {
						js_free(ctx, data);
						return JS_EXCEPTION;
					}
					auto obj = JS_NewObjectProtoClass(ctx, proto, class_id);
					JS_FreeValue(ctx, proto);
					if (JS_IsException(obj)) {
						js_free(ctx, data);
						return JS_EXCEPTION;
					}
					JS_SetOpaque(obj, data);
					return obj;
				}, "read_fs"_sv);
			}

			inline static auto instance(
				JSContext* ctx,
				JSValueConst this_val,
				int argc,
				JSValueConst* argv
			) -> JSDefine::Boolean
			{
				M_JS_PROXY_WRAPPER(ctx, {
					try_assert(argc == 3, fmt::format("{} 3, {}: {}", Kernel::Language::get("kernel.argument_expected"), Kernel::Language::get("kernel.argument_received"), argc));
					auto s = new Data(
						0,
						0,
						static_cast<int>(JS::Converter::get_bigint64(ctx, argv[0])),
						static_cast<int>(JS::Converter::get_bigint64(ctx, argv[1])),
						JS::Converter::to_binary_list(ctx, argv[2])
					);
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
				}, "instance"_sv);
			}

			// Function

			inline static const JSCFunctionListEntry proto_functions[] = {
				JS_CPPGETSET_MAGIC_DEF("bit_depth", getter, setter, ImageView::Magic::bit_depth),
				JS_CPPGETSET_MAGIC_DEF("channels", getter, setter, ImageView::Magic::channels),
				JS_CPPGETSET_MAGIC_DEF("color_type", getter, setter, ImageView::Magic::color_type),
				JS_CPPGETSET_MAGIC_DEF("interlace_type", getter, setter, ImageView::Magic::interlace_type),
				JS_CPPGETSET_MAGIC_DEF("rowbytes", getter, setter, ImageView::Magic::rowbytes),
				JS_CPPGETSET_MAGIC_DEF("width", getter, setter, ImageView::Magic::width),
				JS_CPPGETSET_MAGIC_DEF("height", getter, setter, ImageView::Magic::height),
				JS_CPPGETSET_MAGIC_DEF("data", getter, setter, ImageView::Magic::data),
				JS_CPPFUNC_DEF("area", 0, area),
				JS_CPPFUNC_DEF("circumference", 0, circumference),
			};

			// Adapter

			inline static auto register_class(
				JSContext* ctx
			) -> void
			{
				class_id = JS_NewClass(JS_GetRuntime(ctx), class_id, &this_class);
				auto class_name = "ImageView"_sv;
				auto point_ctor = JS_NewCFunction2(ctx, constructor, class_name.data(), 2, JS_CFUNC_constructor, 0);
				auto proto = JS_NewObject(ctx);
				auto instance_c = JS_NewCFunction(ctx, instance, "instance", 0);
				JS_SetPropertyStr(ctx, point_ctor, "instance", instance_c);
				auto default_cut = JS_NewCFunction(ctx, cut, "cut", 0);
				auto default_resize = JS_NewCFunction(ctx, resize, "resize", 0);
				auto default_scale = JS_NewCFunction(ctx, scale, "scale", 0);
				auto default_rotate = JS_NewCFunction(ctx, rotate, "rotate", 0);
				auto default_read_fs = JS_NewCFunction(ctx, read_fs, "read_fs", 0);
				auto default_write_fs = JS_NewCFunction(ctx, write_fs, "write_fs", 0);
				JS_SetPropertyStr(ctx, point_ctor, "cut", default_cut);
				JS_SetPropertyStr(ctx, point_ctor, "resize", default_resize);
				JS_SetPropertyStr(ctx, point_ctor, "scale", default_scale);
				JS_SetPropertyStr(ctx, point_ctor, "rotate", default_rotate);
				JS_SetPropertyStr(ctx, point_ctor, "read_fs", default_read_fs);
				JS_SetPropertyStr(ctx, point_ctor, "write_fs", default_write_fs);
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
		) -> JSElement::undefined
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
		) -> JSElement::number
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
			) -> JSElement::undefined
			{
				M_JS_PROXY_WRAPPER(context, {
					try_assert(argc == 4, fmt::format("{} 4, {}: {}", Kernel::Language::get("kernel.argument_expected"), Kernel::Language::get("kernel.argument_received"), argc));
					assert_conditional(JS_IsString(argv[0]), fmt::format("{} {} {} {}", Kernel::Language::get("kernel.expected_argument"), 0, Kernel::Language::get("is"), Kernel::Language::get("kernel.tuple.js_string")), "encode_fs");
					assert_conditional(JS_IsString(argv[1]), fmt::format("{} {} {} {}", Kernel::Language::get("kernel.expected_argument"), 1, Kernel::Language::get("is"), Kernel::Language::get("kernel.tuple.js_string")), "encode_fs");
					assert_conditional(JS_IsString(argv[2]), fmt::format("{} {} {} {}", Kernel::Language::get("kernel.expected_argument"), 2, Kernel::Language::get("is"), Kernel::Language::get("kernel.tuple.js_string")), "encode_fs");
					assert_conditional(JS_IsNumber(argv[3]), fmt::format("{} {} {} {}", Kernel::Language::get("kernel.expected_argument"), 3, Kernel::Language::get("is"), Kernel::Language::get("kernel.tuple.js_number")), "encode_fs");
					auto before_file = JS::Converter::get_string(context, argv[0]);
					auto after_file = JS::Converter::get_string(context, argv[1]);
					auto patch_file = JS::Converter::get_string(context, argv[2]);
					auto flag = JS::Converter::get_int32(context, argv[3]);
					Sen::Kernel::Definition::Diff::VCDiff::encode_fs(before_file, after_file, patch_file, static_cast<Sen::Kernel::Definition::Diff::VCDiff::Flag>(flag));
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
			) -> JSElement::undefined
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
		) -> JSElement::string
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
		) -> JSElement::string
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
					if (pipe == nullptr)
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
		) -> JSElement::string
		{
			M_JS_PROXY_WRAPPER(context, {
				try_assert(argc == 1, fmt::format("{} 1, {}: {}", Kernel::Language::get("kernel.argument_expected"), Kernel::Language::get("kernel.argument_received"), argc));
				assert_conditional(JS_IsString(argv[0]), fmt::format("{} {} {} {}", Kernel::Language::get("kernel.expected_argument"), 0, Kernel::Language::get("is"), Kernel::Language::get("kernel.tuple.js_string")), "open");
				auto source = JS::Converter::get_string(context, argv[0]);
				auto result = JS::Converter::read_file_as_js_arraybuffer(context, source);
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
		) -> JSElement::undefined
		{
			M_JS_PROXY_WRAPPER(context, {
				try_assert(argc == 2, fmt::format("{} 2, {}: {}", Kernel::Language::get("kernel.argument_expected"), Kernel::Language::get("kernel.argument_received"), argc));
				assert_conditional(JS_IsString(argv[0]), fmt::format("{} {} {} {}", Kernel::Language::get("kernel.expected_argument"), 0, Kernel::Language::get("is"), Kernel::Language::get("kernel.tuple.js_string")), "out");
				auto destination = JS::Converter::get_string(context, argv[0]);
				auto array_buffer = argv[1];
				JS::Converter::write_file_as_arraybuffer(context, destination, array_buffer);
				return JS::Converter::get_undefined();
			}, "out"_sv);
		}

		/*
			Random bytes in ArrayBuffer
		*/

		inline static auto random(
			JSContext* ctx, 
			JSValueConst this_val, 
			int argc, 
			JSValueConst* argv
		) -> JSElement::undefined
		{
			M_JS_PROXY_WRAPPER(ctx, {
				try_assert(argc == 1, fmt::format("{} 1, {}: {}", Kernel::Language::get("kernel.argument_expected"), Kernel::Language::get("kernel.argument_received"), argc));
				auto arrayBuffer = argv[0];
				auto byteLength = std::size_t{};
				auto data = JS_GetArrayBuffer(ctx, &byteLength, arrayBuffer);
				if (!data) {
					throw Exception(fmt::format("{}", Kernel::Language::get("js.converter.failed_to_get_js_array_buffer")), std::source_location::current(), "random");
				}
				for (auto i : Range(byteLength)) {
					data[i] = rand() % 256;
				}
				return JS_UNDEFINED;
			}, "random");
		}

		/*
			Random bytes in ArrayBuffer
		*/

		inline static auto fill(
			JSContext* ctx,
			JSValueConst this_val,
			int argc,
			JSValueConst* argv
		) -> JSElement::undefined
		{
			M_JS_PROXY_WRAPPER(ctx, {
				try_assert(argc == 2, fmt::format("{} 2, {}: {}", Kernel::Language::get("kernel.argument_expected"), Kernel::Language::get("kernel.argument_received"), argc));
				auto arrayBuffer = argv[0];
				auto byte_c = static_cast<std::uint8_t>(JS::Converter::get_bigint64(ctx, argv[0]));
				auto byteLength = std::size_t{};
				auto data = JS_GetArrayBuffer(ctx, &byteLength, arrayBuffer);
				if (!data) {
					throw Exception(fmt::format("{}", Kernel::Language::get("js.converter.failed_to_get_js_array_buffer")), std::source_location::current(), "fill");
				}
				for (auto i : Range(byteLength)) {
					data[i] = byte_c;
				}
				return JS_UNDEFINED;
			}, "fill");
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
		) -> JSElement::undefined
		{
			M_JS_PROXY_WRAPPER(context, {
				try_assert(argc == 1, fmt::format("{} 1, {}: {}", Kernel::Language::get("kernel.argument_expected"), Kernel::Language::get("kernel.argument_received"), argc));
				assert_conditional(JS_IsString(argv[0]), fmt::format("{} {} {} {}", Kernel::Language::get("kernel.expected_argument"), 0, Kernel::Language::get("is"), Kernel::Language::get("kernel.tuple.js_string")), "run");
				auto source = JS::Converter::get_string(context, argv[0]);
				Sen::Kernel::Process::run(source);
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
		) -> JSElement::boolean
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
		 * JavaScript Get Environment
		 * @param argv[0]: process
		 * @return: string
		 * ----------------------------------------
		*/

		inline static auto get_path_environment(
			JSContext *context, 
			JSValueConst this_val, 
			int argc, 
			JSValueConst *argv
		) -> JSElement::string
		{
			M_JS_PROXY_WRAPPER(context, {
				try_assert(argc == 1, fmt::format("{} 1, {}: {}", Kernel::Language::get("kernel.argument_expected"), Kernel::Language::get("kernel.argument_received"), argc));
				assert_conditional(JS_IsString(argv[0]), fmt::format("{} {} {} {}", Kernel::Language::get("kernel.expected_argument"), 0, Kernel::Language::get("is"), Kernel::Language::get("kernel.tuple.js_string")), "get_path_environment");
				auto source = JS::Converter::get_string(context, argv[0]);
				auto result = Sen::Kernel::Process::get_path_environment(source);
				return JS::Converter::to_string(context, result);
			}, "get_path_environment"_sv);
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
		) -> JSElement::string
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
		) -> JSElement::undefined
		{
			M_JS_PROXY_WRAPPER(context, {
				try_assert(argc >= 1, fmt::format("argument expected greater than {} but received {}", "1", argc));
				switch (argc)
				{
					case 1:
					{
						Shell::callback(construct_string_list(std::vector<std::string>{std::string{"display"}, JS::Converter::get_string(context, argv[0])}).get(), nullptr, nullptr);
						break;
					}
					case 2:
					{
						Shell::callback(construct_string_list(std::vector<std::string>{std::string{"display"}, JS::Converter::get_string(context, argv[0]), JS::Converter::get_string(context, argv[1])}).get(), nullptr, nullptr);
						break;
					}
					default:
					{
						Shell::callback(construct_string_list(std::vector<std::string>{std::string{"display"}, JS::Converter::get_string(context, argv[0]), JS::Converter::get_string(context, argv[1]), exchange_color(static_cast<Sen::Kernel::Interface::Color>(JS::Converter::get_int32(context, argv[2])))}).get(), nullptr, nullptr);
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
		) -> JSElement::string
		{
			M_JS_PROXY_WRAPPER(context, {
				try_assert(argc == 0, fmt::format("{} 0, {}: {}", Kernel::Language::get("kernel.argument_expected"), Kernel::Language::get("kernel.argument_received"), argc));
				Shell::callback(construct_string_list(std::vector<std::string>{std::string{"wait"}}).get(), nullptr, nullptr);
				auto destination = std::make_unique<CStringView>();
				Shell::callback(construct_string_list(std::vector<std::string>{std::string{"input"}}).get(), destination.get(), nullptr);
				return JS::Converter::to_string(context, std::string{destination->value, static_cast<std::size_t>(destination->size)});
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
		) -> JSElement::string
		{
			M_JS_PROXY_WRAPPER(context, {
				auto v = std::vector<std::string>{};
				for(auto i : Range<int>(argc))
				{
					assert_conditional(JS_IsString(argv[i]), fmt::format("{} {} {} {}", Kernel::Language::get("kernel.expected_argument"), i, Kernel::Language::get("is"), Kernel::Language::get("kernel.tuple.js_string")), "join");
					auto source = JS::Converter::get_string(context, argv[i]);
					v.emplace_back(source);
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
		) -> JSElement::string
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
		) -> JSElement::string
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
		) -> JSElement::string
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
		) -> JSElement::string
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
		) -> JSElement::string
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
		) -> JSElement::string
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
		) -> JSElement::string
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
		) -> JSElement::string
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
		) -> JSElement::string
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
		) -> JSElement::boolean
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
		) -> JSElement::string
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
		) -> JSElement::string
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
		) -> JSElement::string
		{
			M_JS_PROXY_WRAPPER(context, {
				try_assert(argc == 1, fmt::format("{} 1, {}: {}", Kernel::Language::get("kernel.argument_expected"), Kernel::Language::get("kernel.argument_received"), argc));
				assert_conditional(JS_IsString(argv[0]), fmt::format("{} {} {} {}", Kernel::Language::get("kernel.expected_argument"), 0, Kernel::Language::get("is"), Kernel::Language::get("kernel.tuple.js_string")), "read_file_encode_with_utf16le");
				auto source = JS::Converter::get_string(context, argv[0]);
				auto result = Sen::Kernel::FileSystem::read_file_by_utf16(source);
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
		) -> JSElement::undefined
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
		) -> JSElement::undefined
		{
			M_JS_PROXY_WRAPPER(context, {
				try_assert(argc == 2, fmt::format("{} 2, {}: {}", Kernel::Language::get("kernel.argument_expected"), Kernel::Language::get("kernel.argument_received"), argc));
				assert_conditional(JS_IsString(argv[0]), fmt::format("{} {} {} {}", Kernel::Language::get("kernel.expected_argument"), 0, Kernel::Language::get("is"), Kernel::Language::get("kernel.tuple.js_string")), "write_file_encode_with_utf16le");
				assert_conditional(JS_IsString(argv[1]), fmt::format("{} {} {} {}", Kernel::Language::get("kernel.expected_argument"), 1, Kernel::Language::get("is"), Kernel::Language::get("kernel.tuple.js_string")), "write_file_encode_with_utf16le");
				auto destination = JS::Converter::get_string(context, argv[0]);
				auto data = JS::Converter::get_string(context, argv[1]);
				auto converter = std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>>{};
				auto result = std::wstring{ converter.from_bytes(data) };
				Sen::Kernel::FileSystem::write_file_by_utf16le(destination, result);
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
		) -> JSElement::Array
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
		) -> JSElement::Array
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
		) -> JSElement::Array
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
		) -> JSElement::Array
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
		) -> JSElement::undefined
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
		) -> JSElement::boolean
		{
			M_JS_PROXY_WRAPPER(context, {
				try_assert(argc == 1, fmt::format("{} 1, {}: {}", Kernel::Language::get("kernel.argument_expected"), Kernel::Language::get("kernel.argument_received"), argc));
				assert_conditional(JS_IsString(argv[0]), fmt::format("{} {} {} {}", Kernel::Language::get("kernel.expected_argument"), 0, Kernel::Language::get("is"), Kernel::Language::get("kernel.tuple.js_string")), "is_file");
				auto source = JS::Converter::get_string(context, argv[0]);
				return JS::Converter::to_bool(context, Kernel::Path::Script::is_file(source));
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
		) -> JSElement::boolean
		{
			M_JS_PROXY_WRAPPER(context, {
				try_assert(argc == 1, fmt::format("{} 1, {}: {}", Kernel::Language::get("kernel.argument_expected"), Kernel::Language::get("kernel.argument_received"), argc));
				assert_conditional(JS_IsString(argv[0]), fmt::format("{} {} {} {}", Kernel::Language::get("kernel.expected_argument"), 0, Kernel::Language::get("is"), Kernel::Language::get("kernel.tuple.js_string")), "is_directory");
				auto source = JS::Converter::get_string(context, argv[0]);
				return JS::Converter::to_bool(context, Kernel::Path::Script::is_directory(source));
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
			) -> JSElement::undefined
			{
				M_JS_PROXY_WRAPPER(context, {
					try_assert(argc == 2, fmt::format("{} 2, {}: {}", Kernel::Language::get("kernel.argument_expected"), Kernel::Language::get("kernel.argument_received"), argc));
					assert_conditional(JS_IsString(argv[0]), fmt::format("{} {} {} {}", Kernel::Language::get("kernel.expected_argument"), 0, Kernel::Language::get("is"), Kernel::Language::get("kernel.tuple.js_string")), "rename");
					assert_conditional(JS_IsString(argv[0]), fmt::format("{} {} {} {}", Kernel::Language::get("kernel.expected_argument"), 1, Kernel::Language::get("is"), Kernel::Language::get("kernel.tuple.js_string")), "rename");
					auto source = JS::Converter::get_string(context, argv[0]);
					auto destination = JS::Converter::get_string(context, argv[1]);
					Kernel::Path::Script::rename(source, destination);
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
			) -> JSElement::undefined
			{
				M_JS_PROXY_WRAPPER(context, {
					try_assert(argc == 2, fmt::format("{} 2, {}: {}", Kernel::Language::get("kernel.argument_expected"), Kernel::Language::get("kernel.argument_received"), argc));
					auto source = JS::Converter::get_string(context, argv[0]);
					auto destination = JS::Converter::get_string(context, argv[1]);
					Kernel::Path::Script::copy(source, destination);
					return JS::Converter::get_undefined();
				}, "copy"_sv);
			}

			/**
			 * ----------------------------------------
			 * JS remove file
			 * @param argv[0]: source file
			 * @return: undefined
			 * ----------------------------------------
			*/

			inline static auto remove(
				JSContext *context, 
				JSValueConst this_val, 
				int argc, 
				JSValueConst *argv
			) -> JSElement::undefined
			{
				M_JS_PROXY_WRAPPER(context, {
					try_assert(argc == 1, fmt::format("{} 1, {}: {}", Kernel::Language::get("kernel.argument_expected"), Kernel::Language::get("kernel.argument_received"), argc));
					auto source = JS::Converter::get_string(context, argv[0]);
					Kernel::Path::Script::remove(source);
					return JS::Converter::get_undefined();
				}, "remove"_sv);
			}

			/**
			 * ----------------------------------------
			 * JS remove file
			 * @param argv[0]: source file
			 * @return: undefined
			 * ----------------------------------------
			*/

			inline static auto remove_all(
				JSContext* context,
				JSValueConst this_val,
				int argc,
				JSValueConst* argv
			) -> JSElement::undefined
			{
				M_JS_PROXY_WRAPPER(context, {
					try_assert(argc == 1, fmt::format("{} 1, {}: {}", Kernel::Language::get("kernel.argument_expected"), Kernel::Language::get("kernel.argument_received"), argc));
					auto source = JS::Converter::get_string(context, argv[0]);
					Kernel::Path::Script::remove_all(source);
					return JS::Converter::get_undefined();
				}, "remove_all"_sv);
			}

		}
	}

	namespace Dimension {

		inline static auto area(
			JSContext* context,
			JSValueConst this_val,
			int argc,
			JSValueConst* argv
		) -> JSElement::bigint
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
		) -> JSElement::bigint
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
		) -> JSElement::Object
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

		/**
		 * Open file
		*/

		inline static auto open(
			JSContext* context,
			JSValueConst this_val,
			int argc,
			JSValueConst* argv
		) -> JSElement::Object
		{
			M_JS_PROXY_WRAPPER(context, {
				try_assert(argc == 1, fmt::format("{} 1, {}: {}", Kernel::Language::get("kernel.argument_expected"), Kernel::Language::get("kernel.argument_received"), argc));
				auto image = Sen::Kernel::Definition::ImageIO::read_png(JS::Converter::get_string(context, argv[0]));
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

		/**
		 * Write
		*/

		inline static auto write(
			JSContext* context,
			JSValueConst this_val,
			int argc,
			JSValueConst* argv
		) -> JSElement::undefined
		{
			M_JS_PROXY_WRAPPER(context, {
				try_assert(argc == 2, fmt::format("{} 2, {}: {}", Kernel::Language::get("kernel.argument_expected"), Kernel::Language::get("kernel.argument_received"), argc));
				auto source_file = JS::Converter::get_string(context, argv[0]);
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
				Sen::Kernel::Definition::ImageIO::write_png(source_file, image);
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
		) -> JSElement::undefined
		{
			M_JS_PROXY_WRAPPER(context, {
				try_assert(argc == 3, fmt::format("{} 3, {}: {}", Kernel::Language::get("kernel.argument_expected"), Kernel::Language::get("kernel.argument_received"), argc));
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
		) -> JSElement::undefined
		{
			M_JS_PROXY_WRAPPER(context, {
				try_assert(argc == 3, fmt::format("{} 3, {}: {}", Kernel::Language::get("kernel.argument_expected"), Kernel::Language::get("kernel.argument_received"), argc));
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
		) -> JSElement::undefined
		{
			M_JS_PROXY_WRAPPER(context, {
				try_assert(argc == 3, fmt::format("{} 3, {}: {}", Kernel::Language::get("kernel.argument_expected"), Kernel::Language::get("kernel.argument_received"), argc));
				auto destination = JS::Converter::get_string(context, argv[0]);
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
					destination,
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
		) -> JSElement::Object
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
		) -> JSElement::undefined
		{
			M_JS_PROXY_WRAPPER(context, {
				try_assert(argc == 3, fmt::format("{} 3, {}: {}", Kernel::Language::get("kernel.argument_expected"), Kernel::Language::get("kernel.argument_received"), argc));
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
		) -> JSElement::undefined
		{
			M_JS_PROXY_WRAPPER(context, {
				try_assert(argc == 3, fmt::format("{} 3, {}: {}", Kernel::Language::get("kernel.argument_expected"), Kernel::Language::get("kernel.argument_received"), argc));
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

		inline static auto cut_fs(
			JSContext *context, 
			JSValueConst this_val, 
			int argc, 
			JSValueConst *argv
		) -> JSElement::undefined
		{
			M_JS_PROXY_WRAPPER(context, {
				try_assert(argc == 3, fmt::format("{} 3, {}: {}", Kernel::Language::get("kernel.argument_expected"), Kernel::Language::get("kernel.argument_received"), argc));
				auto source = JS::Converter::get_string(context, argv[0]);
				auto destination = JS::Converter::get_string(context, argv[1]);
				auto rectangle_width = JS_GetPropertyStr(context, argv[2], "width");
				auto rectangle_height = JS_GetPropertyStr(context, argv[2], "height");
				auto rectangle_x = JS_GetPropertyStr(context, argv[2], "x");
				auto rectangle_y = JS_GetPropertyStr(context, argv[2], "y");
				M_JS_UNDEFINED_BEHAVIOR(context, rectangle_width, "width", "cut_fs"_sv);
				M_JS_UNDEFINED_BEHAVIOR(context, rectangle_height, "height", "cut_fs"_sv);
				M_JS_UNDEFINED_BEHAVIOR(context, rectangle_x, "x", "cut_fs"_sv);
				M_JS_UNDEFINED_BEHAVIOR(context, rectangle_y, "y", "cut_fs"_sv);
				Sen::Kernel::Definition::ImageIO::cut_fs(
					source, 
					destination, 
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
			}, "cut_fs"_sv);
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

		inline static auto cut_multiple_fs(
			JSContext *context, 
			JSValueConst this_val, 
			int argc, 
			JSValueConst *argv
		) -> JSElement::undefined
		{
			M_JS_PROXY_WRAPPER(context, {
				try_assert(argc == 2, fmt::format("{} 2, {}: {}", Kernel::Language::get("kernel.argument_expected"), Kernel::Language::get("kernel.argument_received"), argc));
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
						M_JS_UNDEFINED_BEHAVIOR(context, rectangle_width, "width", "cut_multiple_fs"_sv);
						M_JS_UNDEFINED_BEHAVIOR(context, rectangle_height, "height", "cut_multiple_fs"_sv);
						M_JS_UNDEFINED_BEHAVIOR(context, rectangle_x, "x", "cut_multiple_fs"_sv);
						M_JS_UNDEFINED_BEHAVIOR(context, rectangle_y, "y", "cut_multiple_fs"_sv);
						M_JS_UNDEFINED_BEHAVIOR(context, destination, "destination", "cut_multiple_fs"_sv);
						data.emplace_back(
							Sen::Kernel::Definition::RectangleFileIO<int>(
								JS::Converter::get_int32(context, rectangle_x),
								JS::Converter::get_int32(context, rectangle_y),
								JS::Converter::get_int32(context, rectangle_width),
								JS::Converter::get_int32(context, rectangle_height),
								JS::Converter::get_string(context, destination)
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
				auto source = JS::Converter::get_string(context, argv[0]);
				Sen::Kernel::Definition::ImageIO::cut_pngs(source, data);
				return JS::Converter::get_undefined();
			}, "cut_multiple_fs"_sv);
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

		inline static auto cut_multiple_fs_asynchronous(
			JSContext *context, 
			JSValueConst this_val, 
			int argc, 
			JSValueConst *argv
		) -> JSElement::undefined
		{
			M_JS_PROXY_WRAPPER(context, {
				try_assert(argc == 2, fmt::format("{} 2, {}: {}", Kernel::Language::get("kernel.argument_expected"), Kernel::Language::get("kernel.argument_received"), argc));
				auto source = JS::Converter::get_string(context, argv[0]);
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
						M_JS_UNDEFINED_BEHAVIOR(context, rectangle_width, "width", "cut_multiple_fs_asynchronous"_sv);
						M_JS_UNDEFINED_BEHAVIOR(context, rectangle_height, "height", "cut_multiple_fs_asynchronous"_sv);
						M_JS_UNDEFINED_BEHAVIOR(context, rectangle_x, "x", "cut_multiple_fs_asynchronous"_sv);
						M_JS_UNDEFINED_BEHAVIOR(context, rectangle_y, "y", "cut_multiple_fs_asynchronous"_sv);
						M_JS_UNDEFINED_BEHAVIOR(context, destination, "destination", "cut_multiple_fs_asynchronous"_sv);
						data.emplace_back(
							Sen::Kernel::Definition::RectangleFileIO<int>(
								JS::Converter::get_int32(context, rectangle_x),
								JS::Converter::get_int32(context, rectangle_y),
								JS::Converter::get_int32(context, rectangle_width),
								JS::Converter::get_int32(context, rectangle_height),
								JS::Converter::get_string(context, destination)
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
				Sen::Kernel::Definition::ImageIO::cut_pngs_asynchronous(source, data);
				return JS::Converter::get_undefined();
			}, "cut_multiple_fs_asynchronous"_sv);
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
				auto m_value = JS_Eval(context, source.data(), source.size(), "unknown", JS_EVAL_TYPE_GLOBAL);
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
				auto m_value = JS_Eval(context, js_source.data(), js_source.size(), source.data(), JS_EVAL_TYPE_GLOBAL);
				return m_value;
			}, "evaluate_fs"_sv);
		}
	}

	/**
	 * JavaScript Encryption System
	*/

	namespace Encryption {

		namespace Rijndael {

			/**
			 * ----------------------------------------
			 * JavaScript Rijndael encrypt
			 * ----------------------------------------
			*/

			inline static auto encrypt(
				JSContext* context,
				JSValueConst this_val,
				int argc,
				JSValueConst* argv
			) -> JSValue
			{
				using Mode = Kernel::Definition::Encryption::Rijndael::Mode;
				static constexpr auto callback = []<auto mode>(
					const std::vector<std::uint8_t>& plain,
					std::string_view key,
					std::string_view iv
				) -> std::vector<std::uint8_t> {
					static_assert(mode == Mode::CBC or mode == Mode::CFB or mode == Mode::ECB, "mode must be cbc or cfb or ecb");
					return Kernel::Definition::Encryption::Rijndael::encrypt<std::size_t, Mode::CBC>(reinterpret_cast<char const*>(plain.data()), key, iv, plain.size());
				};
				M_JS_PROXY_WRAPPER(context, {
					try_assert(argc == 4, fmt::format("{} 1, {}: {}", Kernel::Language::get("kernel.argument_expected"), Kernel::Language::get("kernel.argument_received"), argc));
						auto source = JS::Converter::to_binary_list(context, argv[0]);
						auto key = JS::Converter::get_string(context, argv[1]);
						auto iv = JS::Converter::get_string(context, argv[2]);
						auto mode = JS::Converter::get_bigint64(context, argv[3]);
						switch (static_cast<Mode>(mode)) {
							case Mode::CBC: {
								return JS::Converter::toArrayBuffer(context, callback.operator() <Mode::CBC>(source, key, iv));
							}
							case Mode::CFB: {
								return JS::Converter::toArrayBuffer(context, callback.operator() <Mode::CFB>(source, key, iv));
							}
							case Mode::ECB: {
								return JS::Converter::toArrayBuffer(context, callback.operator() <Mode::ECB>(source, key, iv));
							}
							default: {
								return JS_UNDEFINED;
							}
						}
				}, "encrypt"_sv);
			}

			/**
			 * ----------------------------------------
			 * JavaScript Rijndael decrypt
			 * ----------------------------------------
			*/

			inline static auto decrypt(
				JSContext* context,
				JSValueConst this_val,
				int argc,
				JSValueConst* argv
			) -> JSValue
			{
				using Mode = Kernel::Definition::Encryption::Rijndael::Mode;
				static constexpr auto callback = []<auto mode>(
					const std::vector<std::uint8_t>&plain,
					std::string_view key,
					std::string_view iv
					) -> std::vector<std::uint8_t> {
					static_assert(mode == Mode::CBC or mode == Mode::CFB or mode == Mode::ECB, "mode must be cbc or cfb or ecb");
					return Kernel::Definition::Encryption::Rijndael::decrypt<std::size_t, Mode::CBC>(reinterpret_cast<char const*>(plain.data()), key, iv, plain.size());
				};
				M_JS_PROXY_WRAPPER(context, {
					try_assert(argc == 4, fmt::format("{} 1, {}: {}", Kernel::Language::get("kernel.argument_expected"), Kernel::Language::get("kernel.argument_received"), argc));
						auto source = JS::Converter::to_binary_list(context, argv[0]);
						auto key = JS::Converter::get_string(context, argv[1]);
						auto iv = JS::Converter::get_string(context, argv[2]);
						auto mode = JS::Converter::get_bigint64(context, argv[3]);
						switch (static_cast<Mode>(mode)) {
							case Mode::CBC: {
								return JS::Converter::toArrayBuffer(context, callback.operator() <Mode::CBC> (source, key, iv));
							}
							case Mode::CFB: {
								return JS::Converter::toArrayBuffer(context, callback.operator() <Mode::CFB> (source, key, iv));
							}
							case Mode::ECB: {
								return JS::Converter::toArrayBuffer(context, callback.operator() <Mode::ECB> (source, key, iv));
							}
							default: {
								return JS_UNDEFINED;
							}
						}
				}, "decrypt"_sv);
			}

		}

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
					auto source = JS::Converter::get_string(context, argv[0]);
					auto result = Kernel::Definition::Encryption::FNV::Hash<std::uint32_t>{}.make_hash(source.data());
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
					auto source = JS::Converter::get_string(context, argv[0]);
					auto result = Sen::Kernel::Definition::Encryption::FNV::Hash<std::uint32_t>::hash_fs(source.data());
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
					auto source = JS::Converter::get_string(context, argv[0]);
					auto result = Kernel::Definition::Encryption::MD5::hash(Kernel::FileSystem::read_binary<unsigned char>(source));
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
					auto source = JS::Converter::get_string(context, argv[0]);
					auto result = Sen::Kernel::Definition::Encryption::Base64::encode(source);
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
							try_assert(data.size() == 2, fmt::format("{} 2, {}: {}", Kernel::Language::get("kernel.argument_expected"), Kernel::Language::get("kernel.argument_received"), data.size()));
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
							try_assert(data.size() == 2, fmt::format("{} 2, {}: {}", Kernel::Language::get("kernel.argument_expected"), Kernel::Language::get("kernel.argument_received"), data.size()));
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
					auto plain = JS::Converter::to_binary_list(context, argv[0]);
					auto key = JS::Converter::to_binary_list(context, argv[1]);
					auto result = Sen::Kernel::Definition::Encryption::XOR::encrypt(plain, key);
					return JS::Converter::toArrayBuffer(context, result);
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
					try_assert(argc == 3, fmt::format("{} 3, {}: {}", Kernel::Language::get("kernel.argument_expected"), Kernel::Language::get("kernel.argument_received"), argc));
					auto source = JS::Converter::get_string(context, argv[0]);
					auto destination = JS::Converter::get_string(context, argv[1]);
					auto key = JS::Converter::to_binary_list(context, argv[2]);
					Sen::Kernel::Definition::Encryption::XOR::encrypt_fs(source, destination, key);
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
					try_assert(argc == 3, fmt::format("{} 3, {}: {}", Kernel::Language::get("kernel.argument_expected"), Kernel::Language::get("kernel.argument_received"), argc));
					auto source = JS::Converter::get_string(context, argv[0]);
					auto destination = JS::Converter::get_string(context, argv[1]);
					auto level = JS::Converter::get_int32(context, argv[2]);
					if (!(static_cast<int>(Sen::Kernel::Definition::Compression::Zlib::Level::DEFAULT) <= level or level <= static_cast<int>(Sen::Kernel::Definition::Compression::Zlib::Level::LEVEL_9)))
					{
						throw Exception(fmt::format("{}, {} {}", Kernel::Language::get("zlib.compress.invalid_level"), Kernel::Language::get("but_received"), level), std::source_location::current(), "compress_fs");
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

			/**
			 * ----------------------------------------
			 * JavaScript Zlib Uncompression
			 * @param argv[0]: BinaryView
			 * @returns: Uncompressed file
			 * ----------------------------------------
			*/

			inline static auto uncompress(
				JSContext* context,
				JSValueConst this_val,
				int argc,
				JSValueConst* argv
			) -> JSValue
			{
				using Data = Class::BinaryView::Data;
				M_JS_PROXY_WRAPPER(context, {
					try_assert(argc == 1, fmt::format("{} 1, {}: {}", Kernel::Language::get("kernel.argument_expected"), Kernel::Language::get("kernel.argument_received"), argc));
					auto s = static_cast<Data*>(JS_GetOpaque2(context, argv[0], Class::BinaryView::class_id));
					if (s == nullptr) {
						return JS_EXCEPTION;
					}
					auto sub = new Data(Sen::Kernel::Definition::Compression::Zlib::uncompress(s->value));
					auto global_obj = JS_GetGlobalObject(context);
					auto sen_obj = JS_GetPropertyStr(context, global_obj, "Sen");
					auto kernel_obj = JS_GetPropertyStr(context, sen_obj, "Kernel");
					auto binary_ctor = JS_GetPropertyStr(context, kernel_obj, "BinaryView");
					auto proto = JS_GetPropertyStr(context, binary_ctor, "prototype");
					if (JS_IsException(proto)) {
						js_free(context, sub);
						throw Exception("not a constructor", std::source_location::current(), "uncompress");
					}
					auto obj = JS_NewObjectProtoClass(context, proto, Class::BinaryView::class_id);
					JS_FreeValue(context, proto);
					if (JS_IsException(obj)) {
						js_free(context, sub);
						throw Exception("can't define class", std::source_location::current(), "uncompress");
					}
					JS_SetOpaque(obj, sub);
					JS_FreeValue(context, global_obj);
					JS_FreeValue(context, sen_obj);
					JS_FreeValue(context, kernel_obj);
					JS_FreeValue(context, binary_ctor);
					return obj;
				}, "uncompress"_sv);
			}

			/**
			 * ----------------------------------------
			 * JavaScript Zlib Compress
			 * @param argv[0]: BinaryView
			 * @param argv[1]: level
			 * @returns: Uncompressed file
			 * ----------------------------------------
			*/

			inline static auto compress (
				JSContext* context,
				JSValueConst this_val,
				int argc,
				JSValueConst* argv
			) -> JSValue
			{
				using Data = Class::BinaryView::Data;
				using Level = Definition::Compression::Zlib::Level;
				using Zlib = Definition::Compression::Zlib;
				M_JS_PROXY_WRAPPER(context, {
					try_assert(argc == 2, fmt::format("{} 2, {}: {}", Kernel::Language::get("kernel.argument_expected"), Kernel::Language::get("kernel.argument_received"), argc));
					auto s = static_cast<Data*>(JS_GetOpaque2(context, argv[0], Class::BinaryView::class_id));
					if (s == nullptr) {
						return JS_EXCEPTION;
					}
					auto sub = static_cast<Data*>(nullptr);
					switch (static_cast<Level>(JS::Converter::get_int32(context, argv[1]))) {
						case Level::DEFAULT:{
							sub = new Data(Zlib::compress_deflate<Level::LEVEL_6>(s->value));
							break;
						}
						case Level::LEVEL_0:{
							sub = new Data(Zlib::compress_deflate<Level::LEVEL_0>(s->value));
							break;
						}
						case Level::LEVEL_1:{
							sub = new Data(Zlib::compress_deflate<Level::LEVEL_1>(s->value));
							break;
						}
						case Level::LEVEL_2:{
							sub = new Data(Zlib::compress_deflate<Level::LEVEL_2>(s->value));
							break;
						}
						case Level::LEVEL_3:{
							sub = new Data(Zlib::compress_deflate<Level::LEVEL_3>(s->value));
							break;
						}
						case Level::LEVEL_4:{
							sub = new Data(Zlib::compress_deflate<Level::LEVEL_4>(s->value));
							break;
						}
						case Level::LEVEL_5:{
							sub = new Data(Zlib::compress_deflate<Level::LEVEL_5>(s->value));
							break;
						}
						case Level::LEVEL_6:{
							sub = new Data(Zlib::compress_deflate<Level::LEVEL_6>(s->value));
							break;
						}
						case Level::LEVEL_7:{
							sub = new Data(Zlib::compress_deflate<Level::LEVEL_7>(s->value));
							break;
						}
						case Level::LEVEL_8:{
							sub = new Data(Zlib::compress_deflate<Level::LEVEL_8>(s->value));
							break;
						}
						case Level::LEVEL_9:{
							sub = new Data(Zlib::compress_deflate<Level::LEVEL_9>(s->value));
							break;
						}
						default:{
							throw Exception(fmt::format("{}", Kernel::Language::get("zlib.compress.invalid_level")), std::source_location::current(), "compress");
						}
					}
					auto global_obj = JS_GetGlobalObject(context);
					auto sen_obj = JS_GetPropertyStr(context, global_obj, "Sen");
					auto kernel_obj = JS_GetPropertyStr(context, sen_obj, "Kernel");
					auto binary_ctor = JS_GetPropertyStr(context, kernel_obj, "BinaryView");
					auto proto = JS_GetPropertyStr(context, binary_ctor, "prototype");
					if (JS_IsException(proto)) {
						js_free(context, sub);
						throw Exception("not a constructor");
					}
					auto obj = JS_NewObjectProtoClass(context, proto, Class::BinaryView::class_id);
					JS_FreeValue(context, proto);
					if (JS_IsException(obj)) {
						js_free(context, sub);
						throw Exception("can't define class");
					}
					JS_SetOpaque(obj, sub);
					JS_FreeValue(context, global_obj);
					JS_FreeValue(context, sen_obj);
					JS_FreeValue(context, kernel_obj);
					JS_FreeValue(context, binary_ctor);
					return obj;
				}, "compress"_sv);
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
					auto source = JS::Converter::get_string(context, argv[0]);
					auto destination = JS::Converter::get_string(context, argv[1]);
					auto level = JS::Converter::get_bigint64(context, argv[2]);
					Sen::Kernel::Definition::Compression::Lzma::compress_fs(source, destination, static_cast<Sen::Kernel::Definition::Compression::Lzma::Level>(level));
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
					try_assert(argc == 2, fmt::format("{} 2, {}: {}", Kernel::Language::get("kernel.argument_expected"), Kernel::Language::get("kernel.argument_received"), argc));
					auto source = JS::Converter::get_string(context, argv[0]);
					auto destination = JS::Converter::get_string(context, argv[1]);
					Sen::Kernel::Definition::Compression::Lzma::uncompress_fs(source, destination);
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
						try_assert(argc == 5, fmt::format("{} 5, {}: {}", Kernel::Language::get("kernel.argument_expected"), Kernel::Language::get("kernel.argument_received"), argc));
						auto source = JS::Converter::get_string(context, argv[0]);
						auto destination = JS::Converter::get_string(context, argv[1]);
						auto width = JS::Converter::get_bigint64(context, argv[2]);
						auto height = JS::Converter::get_bigint64(context, argv[3]);
						auto format = JS::Converter::get_int32(context, argv[4]);
						Sen::Kernel::Support::Texture::InvokeMethod::decode_fs(source, destination, static_cast<int>(width), static_cast<int>(height), static_cast<Sen::Kernel::Support::Texture::Format>(format));
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
						try_assert(argc == 3, fmt::format("{} 3, {}: {}", Kernel::Language::get("kernel.argument_expected"), Kernel::Language::get("kernel.argument_received"), argc));
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
						try_assert(argc == 3, fmt::format("{} 3, {}: {}", Kernel::Language::get("kernel.argument_expected"), Kernel::Language::get("kernel.argument_received"), argc));
						auto source = JS::Converter::get_string(context, argv[0]);
						auto destination = JS::Converter::get_string(context, argv[1]);
						auto use_64_bit_variant = JS::Converter::get_bool(context, argv[2]);
						if (use_64_bit_variant) {
							Sen::Kernel::Support::PopCap::Zlib::Compress<true>::compress_fs(source, destination);
						}
						else {
							Sen::Kernel::Support::PopCap::Zlib::Compress<false>::compress_fs(source, destination);
						}
						return JS::Converter::get_undefined();
					}, "compress_fs"_sv);
				}

				/**
				 * ----------------------------------------
				 * JavaScript Zlib Compression File
				 * @param argv[0]: source file
				 * @param argv[1]: use_64_bit_variant
				 * @returns: BinaryView
				 * ----------------------------------------
				*/

				inline static auto compress(
					JSContext* context,
					JSValueConst this_val,
					int argc,
					JSValueConst* argv
				) -> JSValue
				{
					using Data = Class::BinaryView::Data;
					auto constexpr delete_pointer = [](Kernel::Support::PopCap::Zlib::VirtualC *ptr) {
						delete ptr;
					};
					using CompressPointer = std::unique_ptr<Sen::Kernel::Support::PopCap::Zlib::VirtualC, decltype(delete_pointer)>;
					M_JS_PROXY_WRAPPER(context, {
						try_assert(argc == 2, fmt::format("{} 2, {}: {}", Kernel::Language::get("kernel.argument_expected"), Kernel::Language::get("kernel.argument_received"), argc));
						auto source = JS::Converter::to_binary_list(context, argv[0]);
						auto use_64_bit_variant = JS::Converter::get_bool(context, argv[1]);
						auto compressor = CompressPointer(nullptr, delete_pointer);
						if (use_64_bit_variant) {
							compressor.reset(new Sen::Kernel::Support::PopCap::Zlib::Compress<true>());
						}
						else {
							compressor.reset(new Sen::Kernel::Support::PopCap::Zlib::Compress<false>());
						}
						auto sub = new Data(compressor->compress(source));
						auto global_obj = JS_GetGlobalObject(context);
						auto sen_obj = JS_GetPropertyStr(context, global_obj, "Sen");
						auto kernel_obj = JS_GetPropertyStr(context, sen_obj, "Kernel");
						auto binary_ctor = JS_GetPropertyStr(context, kernel_obj, "BinaryView");
						auto proto = JS_GetPropertyStr(context, binary_ctor, "prototype");
						if (JS_IsException(proto)) {
							js_free(context, sub);
							throw Exception("not a constructor");
						}
						auto obj = JS_NewObjectProtoClass(context, proto, Class::BinaryView::class_id);
						JS_FreeValue(context, proto);
						if (JS_IsException(obj)) {
							js_free(context, sub);
							throw Exception("can't define class");
						}
						JS_SetOpaque(obj, sub);
						JS_FreeValue(context, global_obj);
						JS_FreeValue(context, sen_obj);
						JS_FreeValue(context, kernel_obj);
						JS_FreeValue(context, binary_ctor);
						return obj;
					}, "compress"_sv);
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
						try_assert(argc == 3, fmt::format("{} 3, {}: {}", Kernel::Language::get("kernel.argument_expected"), Kernel::Language::get("kernel.argument_received"), argc));
						auto source = JS::Converter::get_string(context, argv[0]);
						auto destination = JS::Converter::get_string(context, argv[1]);
						auto use_64_bit_variant = JS::Converter::get_bool(context, argv[2]);
						if (use_64_bit_variant) {
							Sen::Kernel::Support::PopCap::Zlib::Uncompress<true>::uncompress_fs(source, destination);
						}
						else {
							Sen::Kernel::Support::PopCap::Zlib::Uncompress<false>::uncompress_fs(source, destination);
						}
						return JS::Converter::get_undefined();
					}, "uncompress_fs"_sv);
				}

				/**
				 * ----------------------------------------
				 * JavaScript Zlib Uncompression File
				 * @param argv[0]: source file
				 * @param argv[1]: use_64_bit_variant
				 * @returns: BinaryView
				 * ----------------------------------------
				*/

				inline static auto uncompress(
					JSContext* context,
					JSValueConst this_val,
					int argc,
					JSValueConst* argv
				) -> JSValue
				{
					using Data = Class::BinaryView::Data;
					auto constexpr delete_pointer = [](Kernel::Support::PopCap::Zlib::Virtual *ptr) {
						delete ptr;
					};
					using UncompressPointer = std::unique_ptr<Sen::Kernel::Support::PopCap::Zlib::Virtual, decltype(delete_pointer)>;
					M_JS_PROXY_WRAPPER(context, {
						try_assert(argc == 2, fmt::format("{} 2, {}: {}", Kernel::Language::get("kernel.argument_expected"), Kernel::Language::get("kernel.argument_received"), argc));
						auto source = JS::Converter::to_binary_list(context, argv[0]);
						auto use_64_bit_variant = JS::Converter::get_bool(context, argv[1]);
						auto compressor = UncompressPointer(nullptr, delete_pointer);
						if (use_64_bit_variant) {
							compressor.reset(new Sen::Kernel::Support::PopCap::Zlib::Uncompress<true>());
						}
						else {
							compressor.reset(new Sen::Kernel::Support::PopCap::Zlib::Uncompress<false>());
						}
						auto sub = new Data(compressor->uncompress(source));
						auto global_obj = JS_GetGlobalObject(context);
						auto sen_obj = JS_GetPropertyStr(context, global_obj, "Sen");
						auto kernel_obj = JS_GetPropertyStr(context, sen_obj, "Kernel");
						auto binary_ctor = JS_GetPropertyStr(context, kernel_obj, "BinaryView");
						auto proto = JS_GetPropertyStr(context, binary_ctor, "prototype");
						if (JS_IsException(proto)) {
							js_free(context, sub);
							throw Exception("not a constructor");
						}
						auto obj = JS_NewObjectProtoClass(context, proto, Class::BinaryView::class_id);
						JS_FreeValue(context, proto);
						if (JS_IsException(obj)) {
							js_free(context, sub);
							throw Exception("can't define class");
						}
						JS_SetOpaque(obj, sub);
						JS_FreeValue(context, global_obj);
						JS_FreeValue(context, sen_obj);
						JS_FreeValue(context, kernel_obj);
						JS_FreeValue(context, binary_ctor);
						return obj;
					}, "uncompress"_sv);
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
						try_assert(argc == 5, fmt::format("{} 5, {}: {}", Kernel::Language::get("kernel.argument_expected"), Kernel::Language::get("kernel.argument_received"), argc));
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
						try_assert(argc == 5, fmt::format("{} 5, {}: {}", Kernel::Language::get("kernel.argument_expected"), Kernel::Language::get("kernel.argument_received"), argc));
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
					using PathStyle = Sen::Kernel::Support::PopCap::ResourceGroup::PathStyle;
					M_JS_PROXY_WRAPPER(context, {
						try_assert(argc == 3, fmt::format("{} 3, {}: {}", Kernel::Language::get("kernel.argument_expected"), Kernel::Language::get("kernel.argument_received"), argc));
						auto source = JS::Converter::get_string(context, argv[0]);
						auto destination = JS::Converter::get_string(context, argv[1]);
						auto path_style = JS::Converter::get_int32(context, argv[2]);
						switch (static_cast<PathStyle>(path_style)) {
							case PathStyle::ArrayStyle:{
								Sen::Kernel::Support::PopCap::ResourceGroup::Convert<false>::convert_fs(source, destination);
								break;
							}
							case PathStyle::WindowStyle:{
								Sen::Kernel::Support::PopCap::ResourceGroup::Convert<true>::convert_fs(source, destination);
								break;
							}
						}
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
						try_assert(argc == 2, fmt::format("{} 2, {}: {}", Kernel::Language::get("kernel.argument_expected"), Kernel::Language::get("kernel.argument_received"), argc));
						auto source = JS::Converter::get_string(context, argv[0]);
						auto destination = JS::Converter::get_string(context, argv[1]);
						Sen::Kernel::Support::PopCap::ResInfo::Convert::convert_fs(source, destination);
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
			 * JavaScript CharacterFontWidget2 Support
			*/

			namespace Particles {

				/**
				 * ----------------------------------------
				 * CFW2 Decode File
				 * @param argv[0]: source file
				 * @param argv[1]: destination file
				 * @returns: Decoded file
				 * ----------------------------------------
				*/
				template <auto uncompress_zlib>
				inline static auto decode_fs(
					JSContext* context,
					JSValueConst this_val,
					int argc,
					JSValueConst* argv
				) -> JSValue
				{
					static_assert(uncompress_zlib == false or uncompress_zlib == true);
					static_assert(sizeof(uncompress_zlib) == sizeof(bool));
					auto method_name = std::string_view{};
					if constexpr (uncompress_zlib) {
						method_name = "uncompress_and_decode_fs";
					}
					else {
						method_name = "decode_fs";
					}
					M_JS_PROXY_WRAPPER(context, {
						try_assert(argc == 2, fmt::format("{} 2, {}: {}", Kernel::Language::get("kernel.argument_expected"), Kernel::Language::get("kernel.argument_received"), argc));
						auto source = JS::Converter::get_string(context, argv[0]);
						auto destination = JS::Converter::get_string(context, argv[1]);
						Sen::Kernel::Support::PopCap::Particles::Decode::process_fs<uncompress_zlib>(source, destination);
						return JS::Converter::get_undefined();
					}, method_name);
				}

				/**
				 * ----------------------------------------
				 * CFW2 Encode
				 * @param argv[0]: source file
				 * @param argv[1]: destination file
				 * @returns: Encrypted file
				 * ----------------------------------------
				*/
				template <auto compress_zlib>
				inline static auto encode_fs(
					JSContext* context,
					JSValueConst this_val,
					int argc,
					JSValueConst* argv
				) -> JSValue
				{
					static_assert(sizeof(compress_zlib) == sizeof(bool));
					static_assert(compress_zlib == true or compress_zlib == false);
					auto method_name = std::string_view{};
					if constexpr (compress_zlib) {
						method_name = "encode_and_compress_fs";
					}
					else {
						method_name = "encode_fs";
					}
					M_JS_PROXY_WRAPPER(context, {
					try_assert(argc == 2, fmt::format("{} 2, {}: {}", Kernel::Language::get("kernel.argument_expected"), Kernel::Language::get("kernel.argument_received"), argc));
						auto source = JS::Converter::get_string(context, argv[0]);
						auto destination = JS::Converter::get_string(context, argv[1]);
						Sen::Kernel::Support::PopCap::Particles::Encode::process_fs<compress_zlib>(source, destination);
						return JS::Converter::get_undefined();
					}, method_name);
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
						try_assert(argc == 3, fmt::format("{} 3, {}: {}", Kernel::Language::get("kernel.argument_expected"), Kernel::Language::get("kernel.argument_received"), argc));
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
						try_assert(argc == 3, fmt::format("{} 3, {}: {}", Kernel::Language::get("kernel.argument_expected"), Kernel::Language::get("kernel.argument_received"), argc));
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
						auto source = JS::Converter::get_string(context, argv[0]);
						auto destination = JS::Converter::get_string(context, argv[1]);
						Sen::Kernel::Support::PopCap::RTON::Decode::decode_fs(source, destination);
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
						try_assert(argc == 4, fmt::format("{} 4, {}: {}", Kernel::Language::get("kernel.argument_expected"), Kernel::Language::get("kernel.argument_received"), argc));
						auto source = JS::Converter::get_string(context, argv[0]);
						auto destination = JS::Converter::get_string(context, argv[1]);
						auto key = JS::Converter::get_string(context, argv[2]);
						auto iv = JS::Converter::get_string(context, argv[3]);
						Sen::Kernel::Support::PopCap::RTON::Decode::decrypt_fs(source, destination, key, iv);
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
						try_assert(argc == 4, fmt::format("{} 4, {}: {}", Kernel::Language::get("kernel.argument_expected"), Kernel::Language::get("kernel.argument_received"), argc));
						auto source = JS::Converter::get_string(context, argv[0]);
						auto destination = JS::Converter::get_string(context, argv[1]);
						auto key = JS::Converter::get_string(context, argv[2]);
						auto iv = JS::Converter::get_string(context, argv[3]);
						Sen::Kernel::Support::PopCap::RTON::Decode::decrypt_and_decode_fs(source, destination, key, iv);
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
							try_assert(data.size() == 2, fmt::format("{} 2, {}: {}", Kernel::Language::get("kernel.argument_expected"), Kernel::Language::get("kernel.argument_received"), data.size()));
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
							try_assert(data.size() == 2, fmt::format("{} 2, {}: {}", Kernel::Language::get("kernel.argument_expected"), Kernel::Language::get("kernel.argument_received"), data.size()));
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
						try_assert(argc == 4, fmt::format("{} 4, {}: {}", Kernel::Language::get("kernel.argument_expected"), Kernel::Language::get("kernel.argument_received"), argc));
						auto source = JS::Converter::get_string(context, argv[0]);
						auto destination = JS::Converter::get_string(context, argv[1]);
						auto key = JS::Converter::get_string(context, argv[2]);
						auto iv = JS::Converter::get_string(context, argv[3]);
						Sen::Kernel::Support::PopCap::RTON::Encode::encrypt_fs(source, destination, key, iv);
						return JS::Converter::get_undefined();
					}, "encrypt_fs"_sv);
				}

				/**
				 * ----------------------------------------
				 * JavaScript RTON Encrypt File
				 * @param argv[0]: source file
				 * @param argv[1]: destination file
				 * @returns: Encrypted file
				 * ----------------------------------------
				*/

				inline static auto encode_and_encrypt_fs(
					JSContext* context,
					JSValueConst this_val,
					int argc,
					JSValueConst* argv
				) -> JSValue
				{
					M_JS_PROXY_WRAPPER(context, {
						try_assert(argc == 4, fmt::format("{} 4, {}: {}", Kernel::Language::get("kernel.argument_expected"), Kernel::Language::get("kernel.argument_received"), argc));
						auto source = JS::Converter::get_string(context, argv[0]);
						auto destination = JS::Converter::get_string(context, argv[1]);
						auto key = JS::Converter::get_string(context, argv[2]);
						auto iv = JS::Converter::get_string(context, argv[3]);
						Sen::Kernel::Support::PopCap::RTON::Encode::encode_and_encrypt_fs(source, destination, key, iv);
						return JS::Converter::get_undefined();
					}, "encode_and_encrypt_fs"_sv);
				}
			}

		/**
		 * RSBPatch
		*/

			namespace RSBPatch {

				/**
				 * Encode RSB-Patch
				*/

				inline static auto encode_fs(
					JSContext* context,
					JSValueConst this_val,
					int argc,
					JSValueConst* argv
				) -> JSElement::undefined
				{
					M_JS_PROXY_WRAPPER(context, {
						try_assert(argc == 3, fmt::format("{} 3, {}: {}", Kernel::Language::get("kernel.argument_expected"), Kernel::Language::get("kernel.argument_received"), argc));
						assert_conditional(JS_IsString(argv[0]), fmt::format("{} {} {} {}", Kernel::Language::get("kernel.expected_argument"), 0, Kernel::Language::get("is"), Kernel::Language::get("kernel.tuple.js_string")), "encode_fs");
						assert_conditional(JS_IsString(argv[1]), fmt::format("{} {} {} {}", Kernel::Language::get("kernel.expected_argument"), 1, Kernel::Language::get("is"), Kernel::Language::get("kernel.tuple.js_string")), "encode_fs");
						assert_conditional(JS_IsString(argv[2]), fmt::format("{} {} {} {}", Kernel::Language::get("kernel.expected_argument"), 2, Kernel::Language::get("is"), Kernel::Language::get("kernel.tuple.js_string")), "encode_fs");
						auto before_file = JS::Converter::get_string(context, argv[0]);
						auto after_file = JS::Converter::get_string(context, argv[1]);
						auto patch_file = JS::Converter::get_string(context, argv[2]);
						Kernel::Support::PopCap::RSBPatch::Encode::process_fs(before_file, after_file, patch_file);
						return JS::Converter::get_undefined();
					}, "encode_fs"_sv);
				}

				/**
				 * Decode RSB-Patch
				*/

				inline static auto decode_fs(
					JSContext* context,
					JSValueConst this_val,
					int argc,
					JSValueConst* argv
				) -> JSElement::undefined
				{
					M_JS_PROXY_WRAPPER(context, {
						try_assert(argc == 3, fmt::format("{} 3, {}: {}", Kernel::Language::get("kernel.argument_expected"), Kernel::Language::get("kernel.argument_received"), argc));
						assert_conditional(JS_IsString(argv[0]), fmt::format("{} {} {} {}", Kernel::Language::get("kernel.expected_argument"), 0, Kernel::Language::get("is"), Kernel::Language::get("kernel.tuple.js_string")), "decode_fs");
						assert_conditional(JS_IsString(argv[1]), fmt::format("{} {} {} {}", Kernel::Language::get("kernel.expected_argument"), 1, Kernel::Language::get("is"), Kernel::Language::get("kernel.tuple.js_string")), "decode_fs");
						assert_conditional(JS_IsString(argv[2]), fmt::format("{} {} {} {}", Kernel::Language::get("kernel.expected_argument"), 2, Kernel::Language::get("is"), Kernel::Language::get("kernel.tuple.js_string")), "decode_fs");
						auto before_file = JS::Converter::get_string(context, argv[0]);
						auto patch_file = JS::Converter::get_string(context, argv[1]);
						auto after_file = JS::Converter::get_string(context, argv[2]);
						Kernel::Support::PopCap::RSBPatch::Decode::process_fs(before_file, patch_file, after_file);
						return JS::Converter::get_undefined();
					}, "decode_fs"_sv);
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
						auto source = JS::Converter::get_string(context, argv[0]);
						auto destination = JS::Converter::get_string(context, argv[1]);
						Kernel::Support::PopCap::RSB::Unpack::unpack_fs(source, destination);
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
						auto source = JS::Converter::get_string(context, argv[0]);
						auto destination = JS::Converter::get_string(context, argv[1]);
						auto manifest = Sen::Kernel::Support::PopCap::RSB::Manifest <unsigned int>{};
						auto unpack = Kernel::Support::PopCap::RSB::Unpack{ source };
						unpack.process(destination, manifest);
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
						Kernel::Support::PopCap::RSB::Pack::pack_fs<true>(source, destination);
						return JS::Converter::get_undefined();
					}, "pack_fs"_sv);
				}

				/**
				 * ----------------------------------------
				 * JavaScript RSB Pack
				 * @param argv[0]: source file
				 * @param argv[1]: destination file
				 * @param argv[2]: packet info
				 * @returns: Packed file
				 * ----------------------------------------
				*/

				inline static auto pack(
					JSContext* context,
					JSValueConst this_val,
					int argc,
					JSValueConst* argv
				) -> JSValue
				{
					M_JS_PROXY_WRAPPER(context, {
						try_assert(argc == 3, fmt::format("{} 3, {}: {}", Kernel::Language::get("kernel.argument_expected"), Kernel::Language::get("kernel.argument_received"), argc));
						auto source = JS::Converter::get_string(context, argv[0]);
						auto destination = JS::Converter::get_string(context, argv[1]);
						auto pack = Kernel::Support::PopCap::RSB::Pack{};
						pack.process<true>(source, JSON::js_object_to_json(context, argv[2]));
						pack.sen->out_file(destination);
					}, "pack"_sv);
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
						auto source = JS::Converter::get_string(context, argv[0]);
						auto destination = JS::Converter::get_string(context, argv[1]);
						Kernel::Support::PopCap::RSG::Unpack::regular_unpack(source, destination);
						return JS::Converter::get_undefined();
					}, "unpack_fs"_sv);
				}

				/**
				 * ----------------------------------------
				 * JavaScript RSG Unpack File
				 * @param argv[0]: source file
				 * @param argv[1]: destination file
				 * @returns: Unpacked file
				 * ----------------------------------------
				*/

				inline static auto unpack_modding(
					JSContext* context,
					JSValueConst this_val,
					int argc,
					JSValueConst* argv
				) -> JSValue
				{
					M_JS_PROXY_WRAPPER(context, {
						try_assert(argc == 2, fmt::format("{} 2, {}: {}", Kernel::Language::get("kernel.argument_expected"), Kernel::Language::get("kernel.argument_received"), argc));
						auto source = JS::Converter::get_string(context, argv[0]);
						auto destination = JS::Converter::get_string(context, argv[1]);
						auto obj = Kernel::Support::PopCap::RSG::Unpack::unpack_modding(source, destination);
						return JSON::json_to_js_value(context, *obj);
					}, "unpack_modding"_sv);
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
						Kernel::Support::PopCap::RSG::Pack::pack_fs(source, destination);
						return JS::Converter::get_undefined();
					}, "pack_fs"_sv);
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

				inline static auto pack(
					JSContext* context,
					JSValueConst this_val,
					int argc,
					JSValueConst* argv
				) -> JSValue
				{
					M_JS_PROXY_WRAPPER(context, {
						try_assert(argc == 3, fmt::format("{} 3, {}: {}", Kernel::Language::get("kernel.argument_expected"), Kernel::Language::get("kernel.argument_received"), argc));
						auto source = JS::Converter::get_string(context, argv[0]);
						auto destination = JS::Converter::get_string(context, argv[1]);
						auto pack = Kernel::Support::PopCap::RSG::Pack{};
						pack.process<false>(source, JSON::js_object_to_json(context, argv[2]));
						pack.sen.out_file(destination);
						return JS::Converter::get_undefined();
					}, "pack"_sv);
				}
			}

			/**
			 * JavaScript PAK Support
			*/

			namespace PAK {
				/**
				 * ----------------------------------------
				 * JavaScript PAK Unpack File
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
						auto source = JS::Converter::get_string(context, argv[0]);
						auto destination = JS::Converter::get_string(context, argv[1]);
						Kernel::Support::PopCap::PAK::Unpack::process_fs(source, destination);
						return JS::Converter::get_undefined();
					}, "unpack_fs"_sv);
				}

				/**
				 * ----------------------------------------
				 * JavaScript PAK Pack File
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
						Kernel::Support::PopCap::PAK::Pack::process_fs(source, destination);
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
						auto source = JS::Converter::get_string(context, argv[0]);
						auto destination = JS::Converter::get_string(context, argv[1]);
						Sen::Kernel::Support::PopCap::Animation::Decode<int>::decode_fs(source, destination);
						return JS::Converter::get_undefined();
					}, "decode_fs"_sv);
				}

				/**
				 * ToFlash convert support
				*/

				namespace ToFlash {
					/**
					 * ----------------------------------------
					 * JavaScript PAM Convert File
					 * @param argv[0]: source file
					 * @param argv[1]: destination file
					 * @param argv[2]: scale
					 * @returns: Converted file
					 * ----------------------------------------
					*/

					inline static auto convert_fs(
						JSContext* context,
						JSValueConst this_val,
						int argc,
						JSValueConst* argv
					) -> JSValue
					{
						M_JS_PROXY_WRAPPER(context, {
							try_assert(argc == 3, fmt::format("{} 3, {}: {}", Kernel::Language::get("kernel.argument_expected"), Kernel::Language::get("kernel.argument_received"), argc));
							auto source = JS::Converter::get_string(context, argv[0]);
							auto destination = JS::Converter::get_string(context, argv[1]);
							Sen::Kernel::Support::PopCap::Animation::Convert::ToFlash::process_fs(source, destination, static_cast<int>(JS::Converter::get_bigint64(context, argv[2])));
							return JS::Converter::get_undefined();
						}, "convert_fs"_sv);
					}
				}

				/**
				 * FromFlash convert support
				*/

				namespace FromFlash {
					/**
					 * ----------------------------------------
					 * JavaScript PAM Convert File
					 * @param argv[0]: source file
					 * @param argv[1]: destination file
					 * @returns: Converted file
					 * ----------------------------------------
					*/

					inline static auto convert_fs(
						JSContext* context,
						JSValueConst this_val,
						int argc,
						JSValueConst* argv
					) -> JSValue
					{
						M_JS_PROXY_WRAPPER(context, {
							try_assert(argc == 2, fmt::format("{} 2, {}: {}", Kernel::Language::get("kernel.argument_expected"), Kernel::Language::get("kernel.argument_received"), argc));
							auto source = JS::Converter::get_string(context, argv[0]);
							auto destination = JS::Converter::get_string(context, argv[1]);
							Sen::Kernel::Support::PopCap::Animation::Convert::FromFlash::process_fs(source, destination);
							return JS::Converter::get_undefined();
						}, "convert_fs"_sv);
					}
				}

				/**
				 * Instance convert 
				*/

				namespace Instance {
					/**
					 * ----------------------------------------
					 * JavaScript PAM Convert File
					 * @param argv[0]: source file
					 * @param argv[1]: destination file
					 * @param argv[2]: scale
					 * @returns: Converted file
					 * ----------------------------------------
					*/

					inline static auto to_flash (
						JSContext* context,
						JSValueConst this_val,
						int argc,
						JSValueConst* argv
					) -> JSValue
					{
						M_JS_PROXY_WRAPPER(context, {
							try_assert(argc == 3, fmt::format("{} 3, {}: {}", Kernel::Language::get("kernel.argument_expected"), Kernel::Language::get("kernel.argument_received"), argc));
							auto source = JS::Converter::get_string(context, argv[0]);
							auto destination = JS::Converter::get_string(context, argv[1]);
							Sen::Kernel::Support::PopCap::Animation::Convert::InstanceConvert::to_flash(source, destination, static_cast<int>(JS::Converter::get_bigint64(context, argv[2])));
							return JS::Converter::get_undefined();
						}, "to_flash"_sv);
					}

					/**
					 * ----------------------------------------
					 * JavaScript PAM Convert File
					 * @param argv[0]: source file
					 * @param argv[1]: destination file
					 * @param argv[2]: scale
					 * @returns: Converted file
					 * ----------------------------------------
					*/

					inline static auto from_flash (
						JSContext* context,
						JSValueConst this_val,
						int argc,
						JSValueConst* argv
					) -> JSValue
					{
						M_JS_PROXY_WRAPPER(context, {
							try_assert(argc == 2, fmt::format("{} 2, {}: {}", Kernel::Language::get("kernel.argument_expected"), Kernel::Language::get("kernel.argument_received"), argc));
							auto source = JS::Converter::get_string(context, argv[0]);
							auto destination = JS::Converter::get_string(context, argv[1]);
							Sen::Kernel::Support::PopCap::Animation::Convert::InstanceConvert::from_flash(source, destination);
							return JS::Converter::get_undefined();
						}, "from_flash"_sv);
					}
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
						auto source = JS::Converter::get_string(context, argv[0]);
						auto destination = JS::Converter::get_string(context, argv[1]);
						Sen::Kernel::Support::PopCap::Animation::Encode::encode_fs(source, destination);
						return JS::Converter::get_undefined();
					}, "encode_fs"_sv);
				}
			}

				/**
			 * JavaScript REANIM Supportive
			*/

			namespace Reanim {

				using Platform = Sen::Kernel::Support::PopCap::Reanim::ReanimPlatform;

				// Function to get platform

				inline static auto constexpr get_platform(
					std::string_view platform
				) -> Platform
				{
					if (platform == "pc"_sv) {
						return Platform::PC_Compile;
					}
					if (platform == "game-console"_sv) {
						return Platform::GameConsole_Compile;
					}
					if (platform == "phone-32"_sv) {
						return Platform::Phone32_Compile;
					}
					if (platform == "phone-64"_sv) {
						return Platform::Phone64_Compile;
					}
					if (platform == "raw-xml"_sv) {
						return Platform::RawXML;
					}
					if (platform == "tv"_sv) {
						return Platform::TV_Compile;
					}
					if (platform == "wp"_sv) {
						return Platform::WP_XNB;
					}
				}

				/**
				 * ----------------------------------------
				 * JavaScript Reanim Decode File
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
						try_assert(argc == 3, fmt::format("{} 3, {}: {}", Kernel::Language::get("kernel.argument_expected"), Kernel::Language::get("kernel.argument_received"), argc));
						auto source = JS::Converter::get_string(context, argv[0]);
						auto destination = JS::Converter::get_string(context, argv[1]);
						auto z_platform = JS::Converter::get_string(context, argv[2]);
						auto platform = get_platform(z_platform);
						Sen::Kernel::Support::PopCap::Reanim::Decode::process_fs(source, destination, platform);
						return JS::Converter::get_undefined();
					}, "decode_fs"_sv);
				}

				/**
				 * ToFlash convert support
				*/

				namespace ToFlash {
					/**
					 * ----------------------------------------
					 * JavaScript Reanim Convert File
					 * @param argv[0]: source file
					 * @param argv[1]: destination file
					 * @param argv[2]: scale
					 * @returns: Converted file
					 * ----------------------------------------
					*/

					inline static auto convert_fs(
						JSContext* context,
						JSValueConst this_val,
						int argc,
						JSValueConst* argv
					) -> JSValue
					{
						M_JS_PROXY_WRAPPER(context, {
							try_assert(argc == 2, fmt::format("{} 3, {}: {}", Kernel::Language::get("kernel.argument_expected"), Kernel::Language::get("kernel.argument_received"), argc));
							auto source = JS::Converter::get_string(context, argv[0]);
							auto destination = JS::Converter::get_string(context, argv[1]);
							Sen::Kernel::Support::PopCap::Reanim::Convert::ToFlash::process_fs(source, destination);
							return JS::Converter::get_undefined();
						}, "convert_fs"_sv);
					}
				}

				/**
				 * FromFlash convert support
				*/

				namespace FromFlash {
					/**
					 * ----------------------------------------
					 * JavaScript Reanim Convert File
					 * @param argv[0]: source file
					 * @param argv[1]: destination file
					 * @returns: Converted file
					 * ----------------------------------------
					*/

					inline static auto convert_fs(
						JSContext* context,
						JSValueConst this_val,
						int argc,
						JSValueConst* argv
					) -> JSValue
					{
						M_JS_PROXY_WRAPPER(context, {
							try_assert(argc == 2, fmt::format("{} 2, {}: {}", Kernel::Language::get("kernel.argument_expected"), Kernel::Language::get("kernel.argument_received"), argc));
							auto source = JS::Converter::get_string(context, argv[0]);
							auto destination = JS::Converter::get_string(context, argv[1]);
							Sen::Kernel::Support::PopCap::Reanim::Convert::FromFlash::process_fs(source, destination);
							return JS::Converter::get_undefined();
						}, "convert_fs"_sv);
					}
				}

				/**
				 * ----------------------------------------
				 * JavaScript Reanim Encode File
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
						auto z_platform = JS::Converter::get_string(context, argv[2]);
						auto platform = get_platform(z_platform);
						Sen::Kernel::Support::PopCap::Reanim::Encode::process_fs(source, destination, platform);
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
						auto source = JS::Converter::get_string(context, argv[0]);
						auto destination = JS::Converter::get_string(context, argv[1]);
						Kernel::Support::WWise::SoundBank::Decode::process_fs(source, destination);
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
						auto source = JS::Converter::get_string(context, argv[0]);
						auto destination = JS::Converter::get_string(context, argv[1]);
						Kernel::Support::WWise::SoundBank::Encode::process_fs(source, destination);
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
					auto size = std::size_t{};
					auto c_str = JS_ToCStringLen(context, &size, value);
					auto destination = JS_NewStringLen(context, c_str, size);
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
				if (buf == nullptr) {
					throw Exception(fmt::format("{}", Kernel::Language::get("kernel.cast_ArrayBuffer_to_JS_String.failed_to_get_array_buffer")));
				}
				auto str = JS_NewStringLen(context, reinterpret_cast<const char*>(buf), len);
				return str;
			}, "cast_ArrayBuffer_to_JS_String"_sv);
		}

		/*
			UTF8 Support
		*/

		inline static auto cast_movable_String_to_ArrayBuffer(
			JSContext* context,
			JSValueConst this_val,
			int argc,
			JSValueConst* argv
		) -> JSValue
		{
			M_JS_PROXY_WRAPPER(context, {
				try_assert(argc == 1, fmt::format("{} 1, {}: {}", Kernel::Language::get("kernel.argument_expected"), Kernel::Language::get("kernel.argument_received"), argc));
				auto buf = JS::Converter::get_string(context, argv[0]);
				return JS_NewArrayBufferCopy(context, reinterpret_cast<uint8_t*>(buf.data()), buf.size());
			}, "cast_movable_String_to_ArrayBuffer"_sv);
		}

		/*
			Copy ArrayBuffer
		*/

		inline static auto copyArrayBuffer(
			JSContext* context,
			JSValueConst this_val,
			int argc,
			JSValueConst* argv
		) -> JSValue
		{
			M_JS_PROXY_WRAPPER(context, {
				try_assert(argc == 1, fmt::format("{} 1, {}: {}", Kernel::Language::get("kernel.argument_expected"), Kernel::Language::get("kernel.argument_received"), argc));
				auto byte_len = std::size_t{};
				auto data = JS_GetArrayBuffer(context, &byte_len, argv[0]);
				return JS_NewArrayBufferCopy(context, reinterpret_cast<uint8_t*>(data), byte_len);
			}, "copyArrayBuffer"_sv);
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
				if (buf == nullptr) {
					throw Exception(fmt::format("{}", Kernel::Language::get("kernel.cast_ArrayBuffer_to_JS_String.failed_to_get_array_buffer")));
				}
				auto utf16 = std::wstring(reinterpret_cast<wchar_t*>(buf), len / sizeof(wchar_t));
				auto converter = std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>>{};
				auto utf8 = converter.to_bytes(utf16);
				auto str = JS_NewStringLen(context, utf8.data(), utf8.size());
				return str;
			}, "cast_ArrayBuffer_to_JS_WideString"_sv);
		}

		/*
			to_apng
		*/

		inline static auto to_apng(
			JSContext* context,
			JSValueConst this_val,
			int argc,
			JSValueConst* argv
		) -> JSValue
		{
			M_JS_PROXY_WRAPPER(context, {
				try_assert(argc == 3, fmt::format("{} 3, {}: {}", Kernel::Language::get("kernel.argument_expected"), Kernel::Language::get("kernel.argument_received"), argc));
				auto a = JS::Converter::get_vector<std::string>(context, argv[0]);
				auto b = JS::Converter::get_string(context, argv[1]);
				auto s = static_cast<Class::APNGMakerSetting::Data*>(JS_GetOpaque2(context, argv[2], Class::APNGMakerSetting::class_id));
				if (s == nullptr) {
					return JS_EXCEPTION;
				}
				Kernel::Definition::APNGMaker::process_fs(a, b, s);
				return JS_UNDEFINED;
			}, "to_apng"_sv);
		}


	}


	namespace XML {

		using XMLDocument = tinyxml2::XMLDocument;

		inline static auto xml2json(
			const tinyxml2::XMLNode* node
		) -> nlohmann::ordered_json 
		{
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
							if (std::strcmp(child_json["@text"]["value"].get<std::string>().data(),
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
							dynamic_cast<tinyxml2::XMLElement*>(node)->SetAttribute(attribute_key.data(), attribute_value.get<std::string>().data());
						}
					}
					else if (key == "@text") {
						if(!value["is_cdata"].is_null() and value["is_cdata"]) {
							auto cdata = doc.NewText(value["value"].get<std::string>().data());
							cdata->SetCData(true);
							node->InsertEndChild(cdata);
						}
						else {
							dynamic_cast<tinyxml2::XMLElement*>(node)->SetText(value["value"].get<std::string>().data());
						}
					}
					else {
						if (value.is_object()) {
							auto child = doc.NewElement(key.data());
							node->InsertEndChild(child);
							json2xml(value, child, doc);
						}
						else if (value.is_array()) {
							for (auto& element : value) {
								auto child = doc.NewElement(key.data());
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
				node->InsertEndChild(doc.NewText(j.get<std::string>().data()));
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
			auto root = doc.NewElement(root_name.data());
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
				auto eResult = doc.Parse(source.data(), source.size());
				if (eResult != tinyxml2::XML_SUCCESS) {
					throw Exception(fmt::format("XML cannot be parsed, data: {}", source));
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
				auto source = JS::Converter::get_string(context, argv[0]);
				auto view = Kernel::FileSystem::read_file(source);
				auto eResult = doc.Parse(view.data(), view.size());
				if (eResult != tinyxml2::XML_SUCCESS) {
					throw Exception(fmt::format("XML cannot be loaded, data: {}", source));
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

		/**
		 * JavaScript XML Serializer Adapter
		 * @param argv[0] : Destination file
		 * @param argv[1] : XMLDocument
		 * @returns JS Undefined
		*/

		inline static auto serialize_fs (
			JSElement::Context* context,
			JSElement::any this_value,
			JSElement::ParameterCount argc,
			JSElement::ParameterList argv
		) -> JSElement::undefined
		{
			M_JS_PROXY_WRAPPER(context, {
				try_assert(argc == 2, fmt::format("{} 2, {}: {}", Kernel::Language::get("kernel.argument_expected"), Kernel::Language::get("kernel.argument_received"), argc));
				auto doc = XMLDocument{};
				auto source = JSON::js_object_to_json(context, argv[0]);
				convert(source, doc);
				auto printer = tinyxml2::XMLPrinter{};
				doc.Print(&printer);
				auto destination = JS::Converter::get_string(context, argv[1]);
				Kernel::FileSystem::write_file(destination, printer.CStr());
				return JS::Converter::get_undefined();
			}, "serialize_fs"_sv);
		}

	}

}