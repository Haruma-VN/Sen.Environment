#pragma once

#include "kernel/definition/utility.hpp"
#include "kernel/definition/javascript/converter.hpp"

namespace Sen::Kernel::Definition::JavaScript
{

	namespace FileSystem = Sen::Kernel::FileSystem;

	/**
	 * Template
	*/

	template <typename Type>
	concept SpaceX = std::is_same_v<Type, int32_t> || std::is_same_v<Type, uint32_t>
	|| std::is_same_v<Type, int64_t> || std::is_same_v<Type, uint64_t> || std::is_same_v<Type, float> || std::is_same_v<Type, double> || std::is_same_v<Type, std::string> || std::is_same_v<Type, bool>;

	// JS Primitive Type are boolean, string, number and bigint

	template <typename Type>
	concept PrimitiveJSValue = std::is_same_v<Type, bool> or std::is_same_v<Type, std::string_view> or
		std::is_integral<Type>::value or std::is_floating_point<Type>::value;

	/**
	 * JS Runtime Native Handler
	*/
	struct Runtime {

		protected:
			using JS = Runtime;

			inline static auto constexpr free_runtime = [](auto rt) {
				JS_FreeRuntime(rt);
				return;
			};

			inline static auto constexpr free_context = [](auto ctx) {
				JS_FreeContext(ctx);
				return;
			};


		private:
			/**
			 * JS Runtime
			*/

			std::unique_ptr<JSRuntime, decltype(free_runtime)>rt = std::unique_ptr<JSRuntime, 
				decltype(free_runtime)>(JS_NewRuntime(), free_runtime);

			/**
			 * JS Context
			*/

			std::unique_ptr<JSContext, decltype(free_context)> ctx = std::unique_ptr<JSContext, decltype(free_context)>
				(JS_NewContext(thiz.rt.get()), free_context);

			/**
			 * Free JS Value
			*/

			inline auto free_value(
				const JSValue & that
			) -> void
			{
				JS_FreeValue(thiz.ctx.get(), that);
				return;
			}

			/**
			 * Free C String
			*/

			inline auto free_string(
				const char* that
			) -> void
			{
				JS_FreeCString(thiz.ctx.get(), that);
				return;
			}

		public:

			#define M_INSTANCE_OF_OBJECT(obj, parent, name) \
			auto obj = JS_GetPropertyStr(ctx.get(), parent, name.data()); \
			if (JS_IsUndefined(obj)) { \
				obj = JS_NewObject(ctx.get()); \
				JS_SetPropertyStr(ctx.get(), parent, name.data(), obj); \
			}

			#define M_GET_GLOBAL_OBJECT()\
			auto global_obj = JS_GetGlobalObject(ctx.get());

			#define M_FREE_GLOBAL_OBJECT()\
			JS_FreeValue(ctx.get(), global_obj);


			/**
			 * JS Handler Constructor
			*/

			explicit Runtime(

			)
			{

			}

			/**
			 * JS Exception
			*/
			inline auto exception(
				const JSValue & val
			) -> std::string
			{
				auto result = std::string{};
				auto exception = JS_GetException(thiz.ctx.get());
				auto exception_stack = JS_ToCString(thiz.ctx.get(), exception);
				result += std::string{exception_stack};
				if(JS_IsError(thiz.ctx.get(), exception)){
					auto js_stack = JS_GetPropertyStr(thiz.ctx.get(), exception, "stack");
					if(JS::not_undefined(js_stack)){
						auto js_exception = JS_ToCString(thiz.ctx.get(), js_stack);
						result += std::string{js_exception};
						thiz.free_string(js_exception);
					}
					thiz.free_value(js_stack);
				}
				thiz.free_string(exception_stack);
				thiz.free_value(exception);
				return result;
			}

			#pragma region register object

			/**
			 * Use this to register an instance of JS Object
			*/

			inline auto register_object(
				std::function<void (JSRuntime*, JSContext*)> register_method
			) -> void
			{
				register_method(thiz.rt.get(), thiz.ctx.get());
				return;
			}

			/**
			 * Use this to register an instance of JS Object
			*/

			inline auto register_object(
				std::function<void (JSContext*)> register_method
			) -> void
			{
				register_method(thiz.ctx.get());
				return;
			}

			#pragma endregion


			#pragma region comment
			/**
			 * --------------------------------
			 * Evaluate JS through file reading
			 * @param source: JS source
			 * @return: JS Value after evaluate
			 * --------------------------------
			*/

			#pragma endregion

			inline auto evaluate_fs(
				std::string_view source
			) -> JSValue
			{
				return thiz.evaluate(FileSystem::read_file(source), source);
			}

			/**
			 * Check if an JSValue is undefined
			*/

			inline static auto constexpr not_undefined(
				const JSValue & that
			) -> bool
			{
				return JS_VALUE_GET_TAG(that) != JS_TAG_UNDEFINED;
			}

			/**
			 * Check if an JSValue is null
			*/

			inline static auto constexpr not_null(
				const JSValue & that
			) -> bool
			{
				return JS_VALUE_GET_TAG(that) != JS_TAG_NULL;
			}

			/**
			 * ------------------------------------------------------------
			 * Evaluate JavaScript
			 * @param source_data: the eval script data
			 * @param source_file: the source file contains the script data
			 * @return: JS eval data
			 * ------------------------------------------------------------
			*/

			inline auto evaluate(
				std::string_view source_data,
				std::string_view source_file
			) -> JSValue
			{
				auto eval_result = JS_Eval(thiz.ctx.get(), source_data.data(), source_data.size(), source_file.data(), JS_EVAL_FLAG_STRICT | JS_EVAL_TYPE_GLOBAL);
				if(JS_IsException(eval_result)){
					throw Exception(thiz.exception(eval_result), std::source_location::current(), "evaluate");
				}
				return eval_result;
			}

			/**
			 * --------------------------------------
			 * Add C method to JS
			 * @param func: the function pointer of C
			 * @param function_name: JS Function name
			 * @return: JS function has been assigned
			 * --------------------------------------
			*/

			inline auto add_proxy(
				JSValue (*func)(JSContext *, JSValueConst, int, JSValueConst *),
				std::string_view function_name
			) const -> void 
			{
				M_GET_GLOBAL_OBJECT();
				JS_SetPropertyStr(ctx.get(), global_obj, function_name.data(), JS_NewCFunction(ctx.get(), func, function_name.data(), 0));
				M_FREE_GLOBAL_OBJECT();
				return;
			}

			/**
			 * --------------------------------------
			 * Add C method to JS
			 * @param func: the function pointer of C
			 * @param function_name: JS Function name
			 * @param object_name: Object name
			 * @return: JS function has been assigned
			 * --------------------------------------
			*/

			inline auto add_proxy(
				JSValue (*func)(JSContext *, JSValueConst, int, JSValueConst *),
				std::string_view object_name,
				std::string_view function_name
			) const -> void 
			{
				M_GET_GLOBAL_OBJECT();
				auto myObject = JS_GetPropertyStr(ctx.get(), global_obj, object_name.data());
				if (JS_IsUndefined(myObject)) {
					myObject = JS_NewObject(ctx.get());
				}
				JS_SetPropertyStr(ctx.get(), myObject, function_name.data(), JS_NewCFunction(ctx.get(), func, function_name.data(), 0));
				JS_SetPropertyStr(ctx.get(), global_obj, object_name.data(), myObject);
				M_FREE_GLOBAL_OBJECT();
				return;
			}


			/**
			 * --------------------------------------
			 * Add C method to JS
			 * @param func: the function pointer of C
			 * @param function_name: JS Function name
			 * @param obj1_name: Object 1 wrapper
			 * @param obj2_name: Object 2 wrapper
			 * @return: JS function has been assigned
			 * --------------------------------------
			*/

			inline auto add_proxy(
				JSValue (*func)(JSContext *, JSValueConst, int, JSValueConst *),
				std::string_view obj1_name,
				std::string_view obj2_name,
				std::string_view function_name
			) const -> void 
			{
				M_GET_GLOBAL_OBJECT();
				auto outerObject = JS_GetPropertyStr(ctx.get(), global_obj, obj1_name.data());
				if (JS_IsUndefined(outerObject)) {
					outerObject = JS_NewObject(ctx.get());
				}
				auto innerObject = JS_GetPropertyStr(ctx.get(), outerObject, obj2_name.data());
				if (JS_IsUndefined(innerObject)) {
					innerObject = JS_NewObject(ctx.get());
				}
				JS_SetPropertyStr(ctx.get(), innerObject, function_name.data(), JS_NewCFunction(ctx.get(), func, function_name.data(), 0));
				JS_SetPropertyStr(ctx.get(), outerObject, obj2_name.data(), innerObject);
				JS_SetPropertyStr(ctx.get(), global_obj, obj1_name.data(), outerObject);
				M_FREE_GLOBAL_OBJECT();
				return;
			}


			/**
			 * --------------------------------------
			 * Add C method to JS
			 * @param func: the function pointer of C
			 * @param function_name: JS Function name
			 * @param obj1_name: Object 1 wrapper
			 * @param obj2_name: Object 2 wrapper
			 * @param obj3_name: Object 3 wrapper
			 * @return: JS function has been assigned
			 * --------------------------------------
			*/

			inline auto add_proxy(
				JSValue (*func)(JSContext *, JSValueConst, int, JSValueConst *),
				std::string_view obj1_name,
				std::string_view obj2_name,
				std::string_view obj3_name,
				std::string_view function_name
			) const -> void 
			{
				M_GET_GLOBAL_OBJECT();
				auto outerObject = JS_GetPropertyStr(ctx.get(), global_obj, obj1_name.data());
				if (JS_IsUndefined(outerObject)) {
					outerObject = JS_NewObject(ctx.get());
				}
				auto middleObject = JS_GetPropertyStr(ctx.get(), outerObject, obj2_name.data());
				if (JS_IsUndefined(middleObject)) {
					middleObject = JS_NewObject(ctx.get());
				}
				auto innerObject = JS_GetPropertyStr(ctx.get(), middleObject, obj3_name.data());
				if (JS_IsUndefined(innerObject)) {
					innerObject = JS_NewObject(ctx.get());
				}
				JS_SetPropertyStr(ctx.get(), innerObject, function_name.data(), JS_NewCFunction(ctx.get(), func, function_name.data(), 0));
				JS_SetPropertyStr(ctx.get(), middleObject, obj3_name.data(), innerObject);
				JS_SetPropertyStr(ctx.get(), outerObject, obj2_name.data(), middleObject);
				JS_SetPropertyStr(ctx.get(), global_obj, obj1_name.data(), outerObject);
				M_FREE_GLOBAL_OBJECT();
				return;
			}

			/**
			 * --------------------------------------
			 * Add C method to JS
			 * @param func: the function pointer of C
			 * @param function_name: JS Function name
			 * @param obj1_name: Object 1 wrapper
			 * @param obj2_name: Object 2 wrapper
			 * @param obj3_name: Object 3 wrapper
			 * @param obj4_name: Object 4 wrapper
			 * @return: JS function has been assigned
			 * --------------------------------------
			*/

			inline auto add_proxy(
				JSValue (*func)(JSContext *, JSValueConst, int, JSValueConst *),
				std::string_view obj1_name,
				std::string_view obj2_name,
				std::string_view obj3_name,
				std::string_view obj4_name,
				std::string_view function_name
			) const -> void 
			{
				M_GET_GLOBAL_OBJECT();
				auto outerObject = JS_GetPropertyStr(ctx.get(), global_obj, obj1_name.data());
				if (JS_IsUndefined(outerObject)) {
					outerObject = JS_NewObject(ctx.get());
				}
				auto middle1Object = JS_GetPropertyStr(ctx.get(), outerObject, obj2_name.data());
				if (JS_IsUndefined(middle1Object)) {
					middle1Object = JS_NewObject(ctx.get());
				}
				auto middle2Object = JS_GetPropertyStr(ctx.get(), middle1Object, obj3_name.data());
				if (JS_IsUndefined(middle2Object)) {
					middle2Object = JS_NewObject(ctx.get());
				}
				auto innerObject = JS_GetPropertyStr(ctx.get(), middle2Object, obj4_name.data());
				if (JS_IsUndefined(innerObject)) {
					innerObject = JS_NewObject(ctx.get());
				}
				JS_SetPropertyStr(ctx.get(), innerObject, function_name.data(), JS_NewCFunction(ctx.get(), func, function_name.data(), 0));
				JS_SetPropertyStr(ctx.get(), middle2Object, obj4_name.data(), innerObject);
				JS_SetPropertyStr(ctx.get(), middle1Object, obj3_name.data(), middle2Object);
				JS_SetPropertyStr(ctx.get(), outerObject, obj2_name.data(), middle1Object);
				JS_SetPropertyStr(ctx.get(), global_obj, obj1_name.data(), outerObject);
				M_FREE_GLOBAL_OBJECT();
				return;
			}

			/**
			 * --------------------------------------
			 * Add C method to JS
			 * @param func: the function pointer of C
			 * @param function_name: JS Function name
			 * @param obj1_name: Object 1 wrapper
			 * @param obj2_name: Object 2 wrapper
			 * @param obj3_name: Object 3 wrapper
			 * @param obj4_name: Object 4 wrapper
			 * @param obj5_name: Object 5 wrapper
			 * @return: JS function has been assigned
			 * --------------------------------------
			*/

			inline auto add_proxy(
				JSValue (*func)(JSContext *, JSValueConst, int, JSValueConst *),
				std::string_view obj1_name,
				std::string_view obj2_name,
				std::string_view obj3_name,
				std::string_view obj4_name,
				std::string_view obj5_name,
				std::string_view function_name
			) const -> void 
			{
				M_GET_GLOBAL_OBJECT();
				auto outerObject = JS_GetPropertyStr(ctx.get(), global_obj, obj1_name.data());
				if (JS_IsUndefined(outerObject)) {
					outerObject = JS_NewObject(ctx.get());
				}
				auto middle1Object = JS_GetPropertyStr(ctx.get(), outerObject, obj2_name.data());
				if (JS_IsUndefined(middle1Object)) {
					middle1Object = JS_NewObject(ctx.get());
				}
				auto middle2Object = JS_GetPropertyStr(ctx.get(), middle1Object, obj3_name.data());
				if (JS_IsUndefined(middle2Object)) {
					middle2Object = JS_NewObject(ctx.get());
				}
				auto middle3Object = JS_GetPropertyStr(ctx.get(), middle2Object, obj4_name.data());
				if (JS_IsUndefined(middle3Object)) {
					middle3Object = JS_NewObject(ctx.get());
				}
				auto innerObject = JS_GetPropertyStr(ctx.get(), middle3Object, obj5_name.data());
				if (JS_IsUndefined(innerObject)) {
					innerObject = JS_NewObject(ctx.get());
				}
				JS_SetPropertyStr(ctx.get(), innerObject, function_name.data(), JS_NewCFunction(ctx.get(), func, function_name.data(), 0));
				JS_SetPropertyStr(ctx.get(), middle3Object, obj5_name.data(), innerObject);
				JS_SetPropertyStr(ctx.get(), middle2Object, obj4_name.data(), middle3Object);
				JS_SetPropertyStr(ctx.get(), middle1Object, obj3_name.data(), middle2Object);
				JS_SetPropertyStr(ctx.get(), outerObject, obj2_name.data(), middle1Object);
				JS_SetPropertyStr(ctx.get(), global_obj, obj1_name.data(), outerObject);
				M_FREE_GLOBAL_OBJECT();
				return;
			}


			/**
			 * --------------------------------------
			 * Add C method to JS
			 * @param func: the function pointer of C
			 * @param function_name: JS Function name
			 * @param obj1_name: Object 1 wrapper
			 * @param obj2_name: Object 2 wrapper
			 * @param obj3_name: Object 3 wrapper
			 * @param obj4_name: Object 4 wrapper
			 * @param obj5_name: Object 5 wrapper
			 * @param obj6_name: Object 6 wrapper
			 * @return: JS function has been assigned
			 * --------------------------------------
			*/

			inline auto add_proxy(
				JSValue (*func)(JSContext *, JSValueConst, int, JSValueConst *),
				std::string_view obj1_name,
				std::string_view obj2_name,
				std::string_view obj3_name,
				std::string_view obj4_name,
				std::string_view obj5_name,
				std::string_view obj6_name,
				std::string_view function_name
			) const -> void 
			{
				M_GET_GLOBAL_OBJECT();
				auto outerObject = JS_GetPropertyStr(ctx.get(), global_obj, obj1_name.data());
				if (JS_IsUndefined(outerObject)) {
					outerObject = JS_NewObject(ctx.get());
				}
				auto middle1Object = JS_GetPropertyStr(ctx.get(), outerObject, obj2_name.data());
				if (JS_IsUndefined(middle1Object)) {
					middle1Object = JS_NewObject(ctx.get());
				}
				auto middle2Object = JS_GetPropertyStr(ctx.get(), middle1Object, obj3_name.data());
				if (JS_IsUndefined(middle2Object)) {
					middle2Object = JS_NewObject(ctx.get());
				}
				auto middle3Object = JS_GetPropertyStr(ctx.get(), middle2Object, obj4_name.data());
				if (JS_IsUndefined(middle3Object)) {
					middle3Object = JS_NewObject(ctx.get());
				}
				auto middle4Object = JS_GetPropertyStr(ctx.get(), middle3Object, obj5_name.data());
				if (JS_IsUndefined(middle4Object)) {
					middle4Object = JS_NewObject(ctx.get());
				}
				auto innerObject = JS_GetPropertyStr(ctx.get(), middle4Object, obj6_name.data());
				if (JS_IsUndefined(innerObject)) {
					innerObject = JS_NewObject(ctx.get());
				}
				JS_SetPropertyStr(ctx.get(), innerObject, function_name.data(), JS_NewCFunction(ctx.get(), func, function_name.data(), 0));
				JS_SetPropertyStr(ctx.get(), middle4Object, obj6_name.data(), innerObject);
				JS_SetPropertyStr(ctx.get(), middle3Object, obj5_name.data(), middle4Object);
				JS_SetPropertyStr(ctx.get(), middle2Object, obj4_name.data(), middle3Object);
				JS_SetPropertyStr(ctx.get(), middle1Object, obj3_name.data(), middle2Object);
				JS_SetPropertyStr(ctx.get(), outerObject, obj2_name.data(), middle1Object);
				JS_SetPropertyStr(ctx.get(), global_obj, obj1_name.data(), outerObject);
				M_FREE_GLOBAL_OBJECT();
				return;
			}

			/**
			 * --------------------------------------
			 * Add C method to JS
			 * @param func: the function pointer of C
			 * @param function_name: JS Function name
			 * @param obj1_name: Object 1 wrapper
			 * @param obj2_name: Object 2 wrapper
			 * @param obj3_name: Object 3 wrapper
			 * @param obj4_name: Object 4 wrapper
			 * @param obj5_name: Object 5 wrapper
			 * @param obj6_name: Object 6 wrapper
			 * @param obj7_name: Object 7 wrapper
			 * @return: JS function has been assigned
			 * --------------------------------------
			*/

			inline auto add_proxy(
				JSValue (*func)(JSContext *, JSValueConst, int, JSValueConst *),
				std::string_view obj1_name,
				std::string_view obj2_name,
				std::string_view obj3_name,
				std::string_view obj4_name,
				std::string_view obj5_name,
				std::string_view obj6_name,
				std::string_view obj7_name,
				std::string_view function_name
			) -> void 
			{
				M_GET_GLOBAL_OBJECT();
				auto outerObject = JS_GetPropertyStr(ctx.get(), global_obj, obj1_name.data());
				if (JS_IsUndefined(outerObject)) {
					outerObject = JS_NewObject(ctx.get());
				}
				auto middle1Object = JS_GetPropertyStr(ctx.get(), outerObject, obj2_name.data());
				if (JS_IsUndefined(middle1Object)) {
					middle1Object = JS_NewObject(ctx.get());
				}
				auto middle2Object = JS_GetPropertyStr(ctx.get(), middle1Object, obj3_name.data());
				if (JS_IsUndefined(middle2Object)) {
					middle2Object = JS_NewObject(ctx.get());
				}
				auto middle3Object = JS_GetPropertyStr(ctx.get(), middle2Object, obj4_name.data());
				if (JS_IsUndefined(middle3Object)) {
					middle3Object = JS_NewObject(ctx.get());
				}
				auto middle4Object = JS_GetPropertyStr(ctx.get(), middle3Object, obj5_name.data());
				if (JS_IsUndefined(middle4Object)) {
					middle4Object = JS_NewObject(ctx.get());
				}
				auto middle5Object = JS_GetPropertyStr(ctx.get(), middle4Object, obj6_name.data());
				if (JS_IsUndefined(middle5Object)) {
					middle5Object = JS_NewObject(ctx.get());
				}
				auto innerObject = JS_GetPropertyStr(ctx.get(), middle5Object, obj7_name.data());
				if (JS_IsUndefined(innerObject)) {
					innerObject = JS_NewObject(ctx.get());
				}
				JS_SetPropertyStr(ctx.get(), innerObject, function_name.data(), JS_NewCFunction(ctx.get(), func, function_name.data(), 0));
				JS_SetPropertyStr(ctx.get(), middle5Object, obj7_name.data(), innerObject);
				JS_SetPropertyStr(ctx.get(), middle4Object, obj6_name.data(), middle5Object);
				JS_SetPropertyStr(ctx.get(), middle3Object, obj5_name.data(), middle4Object);
				JS_SetPropertyStr(ctx.get(), middle2Object, obj4_name.data(), middle3Object);
				JS_SetPropertyStr(ctx.get(), middle1Object, obj3_name.data(), middle2Object);
				JS_SetPropertyStr(ctx.get(), outerObject, obj2_name.data(), middle1Object);
				JS_SetPropertyStr(ctx.get(), global_obj, obj1_name.data(), outerObject);
				M_FREE_GLOBAL_OBJECT();
				return;
			}


			/**
			 * --------------------------------------
			 * Add C method to JS
			 * @param func: the function pointer of C
			 * @param function_name: JS Function name
			 * @param obj1_name: Object 1 wrapper
			 * @param obj2_name: Object 2 wrapper
			 * @param obj3_name: Object 3 wrapper
			 * @param obj4_name: Object 4 wrapper
			 * @param obj5_name: Object 5 wrapper
			 * @param obj6_name: Object 6 wrapper
			 * @param obj7_name: Object 7 wrapper
			 * @param obj8_name: Object 8 wrapper
			 * @return: JS function has been assigned
			 * --------------------------------------
			*/

			inline auto add_proxy(
				JSValue (*func)(JSContext *, JSValueConst, int, JSValueConst *),
				std::string_view obj1_name,
				std::string_view obj2_name,
				std::string_view obj3_name,
				std::string_view obj4_name,
				std::string_view obj5_name,
				std::string_view obj6_name,
				std::string_view obj7_name,
				std::string_view obj8_name,
				std::string_view function_name
			) -> void 
			{
				M_GET_GLOBAL_OBJECT();
				auto outerObject = JS_GetPropertyStr(ctx.get(), global_obj, obj1_name.data());
				if (JS_IsUndefined(outerObject)) {
					outerObject = JS_NewObject(ctx.get());
				}
				auto middle1Object = JS_GetPropertyStr(ctx.get(), outerObject, obj2_name.data());
				if (JS_IsUndefined(middle1Object)) {
					middle1Object = JS_NewObject(ctx.get());
				}
				auto middle2Object = JS_GetPropertyStr(ctx.get(), middle1Object, obj3_name.data());
				if (JS_IsUndefined(middle2Object)) {
					middle2Object = JS_NewObject(ctx.get());
				}
				auto middle3Object = JS_GetPropertyStr(ctx.get(), middle2Object, obj4_name.data());
				if (JS_IsUndefined(middle3Object)) {
					middle3Object = JS_NewObject(ctx.get());
				}
				auto middle4Object = JS_GetPropertyStr(ctx.get(), middle3Object, obj5_name.data());
				if (JS_IsUndefined(middle4Object)) {
					middle4Object = JS_NewObject(ctx.get());
				}
				auto middle5Object = JS_GetPropertyStr(ctx.get(), middle4Object, obj6_name.data());
				if (JS_IsUndefined(middle5Object)) {
					middle5Object = JS_NewObject(ctx.get());
				}
				auto middle6Object = JS_GetPropertyStr(ctx.get(), middle5Object, obj7_name.data());
				if (JS_IsUndefined(middle6Object)) {
					middle6Object = JS_NewObject(ctx.get());
				}
				auto innerObject = JS_GetPropertyStr(ctx.get(), middle6Object, obj8_name.data());
				if (JS_IsUndefined(innerObject)) {
					innerObject = JS_NewObject(ctx.get());
				}
				JS_SetPropertyStr(ctx.get(), innerObject, function_name.data(), JS_NewCFunction(ctx.get(), func, function_name.data(), 0));
				JS_SetPropertyStr(ctx.get(), middle6Object, obj8_name.data(), innerObject);
				JS_SetPropertyStr(ctx.get(), middle5Object, obj7_name.data(), middle6Object);
				JS_SetPropertyStr(ctx.get(), middle4Object, obj6_name.data(), middle5Object);
				JS_SetPropertyStr(ctx.get(), middle3Object, obj5_name.data(), middle4Object);
				JS_SetPropertyStr(ctx.get(), middle2Object, obj4_name.data(), middle3Object);
				JS_SetPropertyStr(ctx.get(), middle1Object, obj3_name.data(), middle2Object);
				JS_SetPropertyStr(ctx.get(), outerObject, obj2_name.data(), middle1Object);
				JS_SetPropertyStr(ctx.get(), global_obj, obj1_name.data(), outerObject);
				M_FREE_GLOBAL_OBJECT();
				return;
			}

			#define M_WRAP_JSVALUE_AS_PRIMITIVE(obj, value_adapter, variable_name)\
			if constexpr (std::is_same<T, bool>::value) {\
				JS_SetPropertyStr(ctx.get(), obj, variable_name.data(), JS_NewBool(ctx.get(), value_adapter));\
			}\
			if constexpr (std::is_same<T, std::string_view>::value) {\
				JS_SetPropertyStr(ctx.get(), obj, variable_name.data(), JS_NewString(ctx.get(), value_adapter.data()));\
			}\
			if constexpr (std::is_integral<T>::value) {\
				JS_SetPropertyStr(ctx.get(), obj, variable_name.data(), JS_NewBigUint64(ctx.get(), value_adapter));\
			}\
			if constexpr (std::is_floating_point<T>::value) {\
				JS_SetPropertyStr(ctx.get(), obj, variable_name.data(), JS_NewFloat64(ctx.get(), value_adapter));\
			}

			#define M_WRAP_JSVALUE_AS_ARRAY(object, variable_name)\
			auto jsarray = JS_NewArray(ctx.get());\
			for (auto index : Range<size_t>(value.size())) {\
				auto item = value[index];\
				auto val = JSValue{};\
				if constexpr (std::is_same_v<T, bool>) {\
					val = JS_NewBool(ctx.get(), item);\
				}\
				if constexpr (std::is_same_v<T, std::string_view>) {\
					val = JS_NewString(ctx.get(), item.data());\
				}\
				if constexpr (std::is_integral<T>::value) {\
					val = JS_NewBigUint64(ctx.get(), item);\
				}\
				if constexpr (std::is_floating_point<T>::value) {\
					val = JS_NewFloat64(ctx.get(), item);\
				}\
				JS_SetPropertyUint32(ctx.get(), jsarray, index, val);\
			}\
			JS_SetPropertyStr(ctx.get(), object, variable_name.data(), jsarray);


			/**
			 * --------------------------------------
			 * Add a constant JS value from C
			 * @param value: the C string value
			 * @param var_name: JS variable name
			 * --------------------------------------
			*/
			template <typename T> requires PrimitiveJSValue<T>
			inline auto add_constant(
				T value,
				std::string_view var_name
			) -> void
			{
				M_GET_GLOBAL_OBJECT();
				M_WRAP_JSVALUE_AS_PRIMITIVE(global_obj, value, var_name);
				M_FREE_GLOBAL_OBJECT();
				return;
			}

			/**
			 * --------------------------------------
			 * Add a constant JS value from C
			 * @param value: the C string value
			 * @param var_name: JS variable name
			 * --------------------------------------
			*/
			template <typename T> requires PrimitiveJSValue<T>
			inline auto add_constant(
				const std::vector<T> & value,
				std::string_view var_name
			) -> void
			{
				M_GET_GLOBAL_OBJECT();
				M_WRAP_JSVALUE_AS_ARRAY(global_obj, var_name);
				M_FREE_GLOBAL_OBJECT();
				return;
			}


			/**
			 * --------------------------------------
			 * Add a constant JS value to an object from C
			 * @param value: the C string value
			 * @param object_name: JS object name
			 * @param property_name: JS property name
			 * --------------------------------------
			*/

			template <typename T> requires PrimitiveJSValue<T>
			inline auto add_constant(
				T value,
				std::string_view object_name,
				std::string_view property_name
			) -> void 
			{
				M_GET_GLOBAL_OBJECT();
				M_INSTANCE_OF_OBJECT(obj1, global_obj, object_name);
				M_WRAP_JSVALUE_AS_PRIMITIVE(obj1, value, property_name);
				M_FREE_GLOBAL_OBJECT();
				return;
			}

			/**
			 * --------------------------------------
			 * Add a constant JS value from C
			 * @param value: the C string value
			 * @param var_name: JS variable name
			 * --------------------------------------
			*/
			template <typename T> requires PrimitiveJSValue<T>
			inline auto add_constant(
				const std::vector<T>& value,
				std::string_view object_name,
				std::string_view var_name
			) -> void
			{
				M_GET_GLOBAL_OBJECT();
				M_INSTANCE_OF_OBJECT(my_object, global_obj, object_name);
				M_WRAP_JSVALUE_AS_ARRAY(my_object, var_name);
				M_FREE_GLOBAL_OBJECT();
				return;
			}

			/**
			 * --------------------------------------
			 * Add a constant JS value to an object from C
			 * @param value: the C string value
			 * @param obj1_name: JS object name
			 * @param obj2_name: JS object name
			 * @param property_name: JS property name
			 * --------------------------------------
			*/
			template <typename T> requires PrimitiveJSValue<T>
			inline auto add_constant(
				T value,
				std::string_view obj1_name,
				std::string_view obj2_name,
				std::string_view property_name
			) -> void 
			{
				M_GET_GLOBAL_OBJECT();
				M_INSTANCE_OF_OBJECT(obj1, global_obj, obj1_name);
				M_INSTANCE_OF_OBJECT(obj2, obj1, obj2_name);
				M_WRAP_JSVALUE_AS_PRIMITIVE(obj2, value, property_name);
				M_FREE_GLOBAL_OBJECT();
				return;
			}

			/**
			 * --------------------------------------
			 * Add a constant JS value to an object from C
			 * @param value: the C string value
			 * @param obj1_name: JS object name
			 * @param obj2_name: JS object name
			 * @param property_name: JS property name
			 * --------------------------------------
			*/
			template <typename T> requires PrimitiveJSValue<T>
			inline auto add_constant(
				const std::vector<T>& value,
				std::string_view obj1_name,
				std::string_view obj2_name,
				std::string_view property_name
			) -> void
			{
				M_GET_GLOBAL_OBJECT();
				M_INSTANCE_OF_OBJECT(obj1, global_obj, obj1_name);
				M_INSTANCE_OF_OBJECT(obj2, obj1, obj2_name);
				M_WRAP_JSVALUE_AS_ARRAY(obj2, property_name);
				M_FREE_GLOBAL_OBJECT();
				return;
			}

			/**
			 * --------------------------------------
			 * Add a constant JS value to an object from C
			 * @param value: the C string value
			 * @param obj1_name: JS object name
			 * @param obj2_name: JS object name
			 * @param property_name: JS property name
			 * --------------------------------------
			*/
			inline auto add_constant(
				const std::vector<std::string> & value,
				std::string_view obj1_name,
				std::string_view obj2_name,
				std::string_view property_name
			) -> void 
			{
				M_GET_GLOBAL_OBJECT();
				M_INSTANCE_OF_OBJECT(obj1, global_obj, obj1_name);
				M_INSTANCE_OF_OBJECT(obj2, obj1, obj2_name);
				auto js_array = JS_NewArray(ctx.get());
				for (auto i : Range<size_t>(value.size())) {
					JS_SetPropertyUint32(ctx.get(), js_array, i, JS_NewString(ctx.get(), value[i].c_str()));
				}
				JS_SetPropertyStr(ctx.get(), obj2, property_name.data(), js_array);
				M_FREE_GLOBAL_OBJECT();
				return;
			}

			#pragma region comment

			/**
			 * --------------------------------------
			 * Add a constant JS value to an object from C
			 * @param value: the C string value
			 * @param obj1_name: JS object name
			 * @param obj2_name: JS object name
			 * @param property_name: JS property name
			 * --------------------------------------
			*/

			#pragma endregion

			/**
			 * --------------------------------------
			 * Add a constant JS value to an object from C
			 * @param value: the C string value
			 * @param obj1_name: JS object name
			 * @param obj2_name: JS object name
			 * @param obj3_name: JS object name
			 * @param property_name: JS property name
			 * --------------------------------------
			*/

			template <typename T> requires PrimitiveJSValue<T>
			inline auto add_constant(
				T value,
				std::string_view obj1_name,
				std::string_view obj2_name,
				std::string_view obj3_name,
				std::string_view property_name
			) -> void 
			{
				M_GET_GLOBAL_OBJECT();
				M_INSTANCE_OF_OBJECT(obj1, global_obj, obj1_name);
				M_INSTANCE_OF_OBJECT(obj2, obj1, obj2_name);
				M_INSTANCE_OF_OBJECT(obj3, obj2, obj3_name);
				M_WRAP_JSVALUE_AS_PRIMITIVE(obj3, value, property_name);
				M_FREE_GLOBAL_OBJECT();
				return;
			}

			/**
			 * --------------------------------------
			 * Add a constant JS value to an object from C
			 * @param value: the C string value
			 * @param obj1_name: JS object name
			 * @param obj2_name: JS object name
			 * @param obj3_name: JS object name
			 * @param property_name: JS property name
			 * --------------------------------------
			*/

			template <typename T> requires PrimitiveJSValue<T>
			inline auto add_constant(
				const std::vector<T> & value,
				std::string_view obj1_name,
				std::string_view obj2_name,
				std::string_view obj3_name,
				std::string_view property_name
			) -> void
			{
				M_GET_GLOBAL_OBJECT();
				M_INSTANCE_OF_OBJECT(obj1, global_obj, obj1_name);
				M_INSTANCE_OF_OBJECT(obj2, obj1, obj2_name);
				M_INSTANCE_OF_OBJECT(obj3, obj2, obj3_name);
				M_WRAP_JSVALUE_AS_ARRAY(obj3, property_name);
				M_FREE_GLOBAL_OBJECT();
				return;
			}

			/**
			 * --------------------------------------
			 * Add a constant JS value to an object from C
			 * @param value: the C string value
			 * @param obj1_name: JS object name
			 * @param obj2_name: JS object name
			 * @param obj3_name: JS object name
			 * @param obj4_name: JS object name
			 * @param property_name: JS property name
			 * --------------------------------------
			*/
			template <typename T> requires PrimitiveJSValue<T>
			inline auto add_constant(
				T value,
				std::string_view obj1_name,
				std::string_view obj2_name,
				std::string_view obj3_name,
				std::string_view obj4_name,
				std::string_view property_name
			) -> void 
			{
				M_GET_GLOBAL_OBJECT();
				M_INSTANCE_OF_OBJECT(obj1, global_obj, obj1_name);
				M_INSTANCE_OF_OBJECT(obj2, obj1, obj2_name);
				M_INSTANCE_OF_OBJECT(obj3, obj2, obj3_name);
				M_INSTANCE_OF_OBJECT(obj4, obj3, obj4_name);
				M_WRAP_JSVALUE_AS_PRIMITIVE(obj4, value, property_name);
				M_FREE_GLOBAL_OBJECT();
				return;
			}

			/**
			 * --------------------------------------
			 * Add a constant JS value to an object from C
			 * @param value: the C string value
			 * @param obj1_name: JS object name
			 * @param obj2_name: JS object name
			 * @param obj3_name: JS object name
			 * @param obj4_name: JS object name
			 * @param property_name: JS property name
			 * --------------------------------------
			*/
			template <typename T> requires PrimitiveJSValue<T>
			inline auto add_constant(
				const std::vector<T> & value,
				std::string_view obj1_name,
				std::string_view obj2_name,
				std::string_view obj3_name,
				std::string_view obj4_name,
				std::string_view property_name
			) -> void
			{
				M_GET_GLOBAL_OBJECT();
				M_INSTANCE_OF_OBJECT(obj1, global_obj, obj1_name);
				M_INSTANCE_OF_OBJECT(obj2, obj1, obj2_name);
				M_INSTANCE_OF_OBJECT(obj3, obj2, obj3_name);
				M_INSTANCE_OF_OBJECT(obj4, obj3, obj4_name);
				M_WRAP_JSVALUE_AS_ARRAY(obj4, value);
				M_FREE_GLOBAL_OBJECT();
				return;
			}

			/**
			 * --------------------------------------
			 * Add a constant JS value to an object from C
			 * @param value: the C string value
			 * @param obj1_name: JS object name
			 * @param obj2_name: JS object name
			 * @param obj3_name: JS object name
			 * @param obj4_name: JS object name
			 * @param obj5_name: JS object name
			 * @param property_name: JS property name
			 * --------------------------------------
			*/
			template <typename T> requires PrimitiveJSValue<T>
			inline auto add_constant(
				T value,
				std::string_view obj1_name,
				std::string_view obj2_name,
				std::string_view obj3_name,
				std::string_view obj4_name,
				std::string_view obj5_name,
				std::string_view property_name
			) -> void 
			{
				M_GET_GLOBAL_OBJECT();
				M_INSTANCE_OF_OBJECT(obj1, global_obj, obj1_name);
				M_INSTANCE_OF_OBJECT(obj2, obj1, obj2_name);
				M_INSTANCE_OF_OBJECT(obj3, obj2, obj3_name);
				M_INSTANCE_OF_OBJECT(obj4, obj3, obj4_name);
				M_INSTANCE_OF_OBJECT(obj5, obj4, obj5_name);
				M_WRAP_JSVALUE_AS_PRIMITIVE(obj5, value, property_name);
				M_FREE_GLOBAL_OBJECT();
				return;
			}

			/**
			 * --------------------------------------
			 * Add a constant JS value to an object from C
			 * @param value: the C string value
			 * @param obj1_name: JS object name
			 * @param obj2_name: JS object name
			 * @param obj3_name: JS object name
			 * @param obj4_name: JS object name
			 * @param obj5_name: JS object name
			 * @param property_name: JS property name
			 * --------------------------------------
			*/
			template <typename T> requires PrimitiveJSValue<T>
			inline auto add_constant(
				const std::vector<T> & value,
				std::string_view obj1_name,
				std::string_view obj2_name,
				std::string_view obj3_name,
				std::string_view obj4_name,
				std::string_view obj5_name,
				std::string_view property_name
			) -> void
			{
				M_GET_GLOBAL_OBJECT();
				M_INSTANCE_OF_OBJECT(obj1, global_obj, obj1_name);
				M_INSTANCE_OF_OBJECT(obj2, obj1, obj2_name);
				M_INSTANCE_OF_OBJECT(obj3, obj2, obj3_name);
				M_INSTANCE_OF_OBJECT(obj4, obj3, obj4_name);
				M_INSTANCE_OF_OBJECT(obj5, obj4, obj5_name);
				M_WRAP_JSVALUE_AS_ARRAY(obj5, property_name);
				M_FREE_GLOBAL_OBJECT();
				return;
			}

			/**
			 * --------------------------------------
			 * Add a constant JS value to an object from C
			 * @param value: the C string value
			 * @param obj1_name: JS object name
			 * @param obj2_name: JS object name
			 * @param obj3_name: JS object name
			 * @param obj4_name: JS object name
			 * @param obj5_name: JS object name
			 * @param obj6_name: JS object name
			 * @param property_name: JS property name
			 * --------------------------------------
			*/
			template <typename T> requires PrimitiveJSValue<T>
			inline auto add_constant(
				T value,
				std::string_view obj1_name,
				std::string_view obj2_name,
				std::string_view obj3_name,
				std::string_view obj4_name,
				std::string_view obj5_name,
				std::string_view obj6_name,
				std::string_view property_name
			) -> void 
			{
				M_GET_GLOBAL_OBJECT();
				M_INSTANCE_OF_OBJECT(obj1, global_obj, obj1_name);
				M_INSTANCE_OF_OBJECT(obj2, obj1, obj2_name);
				M_INSTANCE_OF_OBJECT(obj3, obj2, obj3_name);
				M_INSTANCE_OF_OBJECT(obj4, obj3, obj4_name);
				M_INSTANCE_OF_OBJECT(obj5, obj4, obj5_name);
				M_INSTANCE_OF_OBJECT(obj6, obj5, obj6_name);
				M_WRAP_JSVALUE_AS_PRIMITIVE(obj6, value, property_name);
				M_FREE_GLOBAL_OBJECT();
				return;
			}

			/**
			 * --------------------------------------
			 * Add a constant JS value to an object from C
			 * @param value: the C string value
			 * @param obj1_name: JS object name
			 * @param obj2_name: JS object name
			 * @param obj3_name: JS object name
			 * @param obj4_name: JS object name
			 * @param obj5_name: JS object name
			 * @param obj6_name: JS object name
			 * @param property_name: JS property name
			 * --------------------------------------
			*/
			template <typename T> requires PrimitiveJSValue<T>
			inline auto add_constant(
				const std::vector<T> & value,
				std::string_view obj1_name,
				std::string_view obj2_name,
				std::string_view obj3_name,
				std::string_view obj4_name,
				std::string_view obj5_name,
				std::string_view obj6_name,
				std::string_view property_name
			) -> void
			{
				M_GET_GLOBAL_OBJECT();
				M_INSTANCE_OF_OBJECT(obj1, global_obj, obj1_name);
				M_INSTANCE_OF_OBJECT(obj2, obj1, obj2_name);
				M_INSTANCE_OF_OBJECT(obj3, obj2, obj3_name);
				M_INSTANCE_OF_OBJECT(obj4, obj3, obj4_name);
				M_INSTANCE_OF_OBJECT(obj5, obj4, obj5_name);
				M_INSTANCE_OF_OBJECT(obj6, obj5, obj6_name);
				M_WRAP_JSVALUE_AS_ARRAY(obj6, property_name);
				M_FREE_GLOBAL_OBJECT();
				return;
			}

			/**
			 * --------------------------------------
			 * Add a constant JS value to an object from C
			 * @param value: the C string value
			 * @param obj1_name: JS object name
			 * @param obj2_name: JS object name
			 * @param obj3_name: JS object name
			 * @param obj4_name: JS object name
			 * @param obj5_name: JS object name
			 * @param obj6_name: JS object name
			 * @param obj7_name: JS object name
			 * @param property_name: JS property name
			 * --------------------------------------
			*/
			template <typename T> requires PrimitiveJSValue<T>
			inline auto add_constant(
				T value,
				std::string_view obj1_name,
				std::string_view obj2_name,
				std::string_view obj3_name,
				std::string_view obj4_name,
				std::string_view obj5_name,
				std::string_view obj6_name,
				std::string_view obj7_name,
				std::string_view property_name
			) -> void 
			{
				M_GET_GLOBAL_OBJECT();
				M_INSTANCE_OF_OBJECT(obj1, global_obj, obj1_name);
				M_INSTANCE_OF_OBJECT(obj2, obj1, obj2_name);
				M_INSTANCE_OF_OBJECT(obj3, obj2, obj3_name);
				M_INSTANCE_OF_OBJECT(obj4, obj3, obj4_name);
				M_INSTANCE_OF_OBJECT(obj5, obj4, obj5_name);
				M_INSTANCE_OF_OBJECT(obj6, obj5, obj6_name);
				M_INSTANCE_OF_OBJECT(obj7, obj6, obj7_name);
				M_WRAP_JSVALUE_AS_PRIMITIVE(obj7, value, property_name);
				M_FREE_GLOBAL_OBJECT();
				return;
			}

			/**
			 * --------------------------------------
			 * Add a constant JS value to an object from C
			 * @param value: the C string value
			 * @param obj1_name: JS object name
			 * @param obj2_name: JS object name
			 * @param obj3_name: JS object name
			 * @param obj4_name: JS object name
			 * @param obj5_name: JS object name
			 * @param obj6_name: JS object name
			 * @param obj7_name: JS object name
			 * @param property_name: JS property name
			 * --------------------------------------
			*/
			template <typename T> requires PrimitiveJSValue<T>
			inline auto add_constant(
				const std::vector<T> & value,
				std::string_view obj1_name,
				std::string_view obj2_name,
				std::string_view obj3_name,
				std::string_view obj4_name,
				std::string_view obj5_name,
				std::string_view obj6_name,
				std::string_view obj7_name,
				std::string_view property_name
			) -> void
			{
				M_GET_GLOBAL_OBJECT();
				M_INSTANCE_OF_OBJECT(obj1, global_obj, obj1_name);
				M_INSTANCE_OF_OBJECT(obj2, obj1, obj2_name);
				M_INSTANCE_OF_OBJECT(obj3, obj2, obj3_name);
				M_INSTANCE_OF_OBJECT(obj4, obj3, obj4_name);
				M_INSTANCE_OF_OBJECT(obj5, obj4, obj5_name);
				M_INSTANCE_OF_OBJECT(obj6, obj5, obj6_name);
				M_INSTANCE_OF_OBJECT(obj7, obj6, obj7_name);
				M_WRAP_JSVALUE_AS_ARRAY(obj7, property_name);
				M_FREE_GLOBAL_OBJECT();
				return;
			}


			/**
			 * --------------------------------------
			 * Add a constant JS value to an object from C
			 * @param value: the C string value
			 * @param obj1_name: JS object name
			 * @param obj2_name: JS object name
			 * @param obj3_name: JS object name
			 * @param obj4_name: JS object name
			 * @param obj5_name: JS object name
			 * @param obj6_name: JS object name
			 * @param obj7_name: JS object name
			 * @param obj8_name: JS object name
			 * @param property_name: JS property name
			 * --------------------------------------
			*/
			template <typename T> requires PrimitiveJSValue<T>
			inline auto add_constant(
				T value,
				std::string_view obj1_name,
				std::string_view obj2_name,
				std::string_view obj3_name,
				std::string_view obj4_name,
				std::string_view obj5_name,
				std::string_view obj6_name,
				std::string_view obj7_name,
				std::string_view obj8_name,
				std::string_view property_name
			) -> void
			{
				M_GET_GLOBAL_OBJECT();
				M_INSTANCE_OF_OBJECT(obj1, global_obj, obj1_name);
				M_INSTANCE_OF_OBJECT(obj2, obj1, obj2_name);
				M_INSTANCE_OF_OBJECT(obj3, obj2, obj3_name);
				M_INSTANCE_OF_OBJECT(obj4, obj3, obj4_name);
				M_INSTANCE_OF_OBJECT(obj5, obj4, obj5_name);
				M_INSTANCE_OF_OBJECT(obj6, obj5, obj6_name);
				M_INSTANCE_OF_OBJECT(obj7, obj6, obj7_name);
				M_INSTANCE_OF_OBJECT(obj8, obj7, obj8_name);
				M_WRAP_JSVALUE_AS_PRIMITIVE(obj8, value, property_name);
				M_FREE_GLOBAL_OBJECT();
				return;
			}

			/**
			 * --------------------------------------
			 * Add a constant JS value to an object from C
			 * @param value: the C float value
			 * @param obj1_name: JS object name
			 * @param obj2_name: JS object name
			 * @param obj3_name: JS object name
			 * @param obj4_name: JS object name
			 * @param obj5_name: JS object name
			 * @param obj6_name: JS object name
			 * @param obj7_name: JS object name
			 * @param obj8_name: JS object name
			 * @param obj9_name: JS object name
			 * @param property_name: JS property name
			 * --------------------------------------
			*/
			template <typename T> requires PrimitiveJSValue<T>
			inline auto add_constant(
				T value,
				std::string_view obj1_name,
				std::string_view obj2_name,
				std::string_view obj3_name,
				std::string_view obj4_name,
				std::string_view obj5_name,
				std::string_view obj6_name,
				std::string_view obj7_name,
				std::string_view obj8_name,
				std::string_view obj9_name,
				std::string_view property_name
			) -> void 
			{
				M_GET_GLOBAL_OBJECT();
				M_INSTANCE_OF_OBJECT(obj1, global_obj, obj1_name);
				M_INSTANCE_OF_OBJECT(obj2, obj1, obj2_name);
				M_INSTANCE_OF_OBJECT(obj3, obj2, obj3_name);
				M_INSTANCE_OF_OBJECT(obj4, obj3, obj4_name);
				M_INSTANCE_OF_OBJECT(obj5, obj4, obj5_name);
				M_INSTANCE_OF_OBJECT(obj6, obj5, obj6_name);
				M_INSTANCE_OF_OBJECT(obj7, obj6, obj7_name);
				M_INSTANCE_OF_OBJECT(obj8, obj7, obj8_name);
				M_INSTANCE_OF_OBJECT(obj9, obj8, obj9_name);
				M_WRAP_JSVALUE_AS_PRIMITIVE(obj9, value, property_name);
				M_FREE_GLOBAL_OBJECT();
				return;
			}

			/**
			 * Destructor
			*/

			~Runtime(

			) 
			{
			}
	};
}