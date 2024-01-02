#pragma once

#include "kernel/definition/javascript/converter.hpp"

namespace Sen::Kernel::Definition::JavaScript
{

	/**
	 * Template
	*/

	template <typename Type>
	concept SpaceX = std::is_same_v<Type, int32_t> || std::is_same_v<Type, uint32_t>
	|| std::is_same_v<Type, int64_t> || std::is_same_v<Type, uint64_t> || std::is_same_v<Type, float> || std::is_same_v<Type, double> || std::is_same_v<Type, std::string> || std::is_same_v<Type, bool>;

	/**
	 * JS Runtime Native Handler
	*/

	struct Runtime {

		private:
			/**
			 * JS Runtime
			*/

			JSRuntime *rt = JS_NewRuntime();

			/**
			 * JS Context
			*/

			JSContext* ctx = JS_NewContext(thiz.rt);

			/**
			 * Free JS Value
			*/

			inline auto free_value(
				const JSValue & that
			) -> void
			{
				JS_FreeValue(thiz.ctx, that);
				return;
			}

			/**
			 * Free C String
			*/

			inline auto free_string(
				const char* that
			) -> void
			{
				JS_FreeCString(thiz.ctx, that);
				return;
			}

		public:

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
				auto exception = JS_GetException(thiz.ctx);
				auto exceptionStack = JS_ToCString(thiz.ctx, exception);
				result += std::string{exceptionStack};
				if(JS_IsError(thiz.ctx, exception)){
					auto stackTrace = JS_GetPropertyStr(thiz.ctx, exception, "stack");
					if(JS_IsUndefined(stackTrace)){
						auto js_exception = JS_ToCString(thiz.ctx, stackTrace);
						result += std::string{js_exception};
						thiz.free_string(js_exception);
					}
					thiz.free_value(stackTrace);
				}
				thiz.free_string(exceptionStack);
				thiz.free_value(exception);
				return result;
			}

			/**
			 * --------------------------------
			 * Evaluate JS through file reading
			 * @param source: JS source
			 * @return: JS Value after evaluate
			 * --------------------------------
			*/

			inline auto evaluate_fs(
				const std::string & source
			) -> JSValue
			{
				return thiz.evaluate(FileSystem::read_file(source), source);
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
				const std::string & source_data,
				const std::string & source_file
			) -> JSValue
			{
				auto eval_result = JS_Eval(thiz.ctx, source_data.c_str(), source_data.size(), source_file.c_str(), JS_EVAL_TYPE_GLOBAL);
				if(JS_IsException(eval_result)){
					throw std::runtime_error(thiz.exception(eval_result));
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
				const std::string & function_name
			) const -> void 
			{
				auto global_obj = JS_GetGlobalObject(ctx);
				JS_SetPropertyStr(ctx, global_obj, function_name.c_str(), JS_NewCFunction(ctx, func, function_name.c_str(), 0));
				JS_FreeValue(ctx, global_obj);
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
				const std::string & object_name,
				const std::string & function_name
			) const -> void 
			{
				auto global_obj = JS_GetGlobalObject(ctx);
				auto myObject = JS_GetPropertyStr(ctx, global_obj, object_name.c_str());
				if (JS_IsUndefined(myObject)) {
					myObject = JS_NewObject(ctx);
				}
				JS_SetPropertyStr(ctx, myObject, function_name.c_str(), JS_NewCFunction(ctx, func, function_name.c_str(), 0));
				JS_SetPropertyStr(ctx, global_obj, object_name.c_str(), myObject);
				JS_FreeValue(ctx, global_obj);
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
				const std::string & obj1_name,
				const std::string & obj2_name,
				const std::string & function_name
			) const -> void 
			{
				auto global_obj = JS_GetGlobalObject(ctx);
				auto outerObject = JS_GetPropertyStr(ctx, global_obj, obj1_name.c_str());
				if (JS_IsUndefined(outerObject)) {
					outerObject = JS_NewObject(ctx);
				}
				auto innerObject = JS_GetPropertyStr(ctx, outerObject, obj2_name.c_str());
				if (JS_IsUndefined(innerObject)) {
					innerObject = JS_NewObject(ctx);
				}
				JS_SetPropertyStr(ctx, innerObject, function_name.c_str(), JS_NewCFunction(ctx, func, function_name.c_str(), 0));
				JS_SetPropertyStr(ctx, outerObject, obj2_name.c_str(), innerObject);
				JS_SetPropertyStr(ctx, global_obj, obj1_name.c_str(), outerObject);
				JS_FreeValue(ctx, global_obj);
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
				const std::string & obj1_name,
				const std::string & obj2_name,
				const std::string & obj3_name,
				const std::string & function_name
			) const -> void 
			{
				auto global_obj = JS_GetGlobalObject(ctx);
				auto outerObject = JS_GetPropertyStr(ctx, global_obj, obj1_name.c_str());
				if (JS_IsUndefined(outerObject)) {
					outerObject = JS_NewObject(ctx);
				}
				auto middleObject = JS_GetPropertyStr(ctx, outerObject, obj2_name.c_str());
				if (JS_IsUndefined(middleObject)) {
					middleObject = JS_NewObject(ctx);
				}
				auto innerObject = JS_GetPropertyStr(ctx, middleObject, obj3_name.c_str());
				if (JS_IsUndefined(innerObject)) {
					innerObject = JS_NewObject(ctx);
				}
				JS_SetPropertyStr(ctx, innerObject, function_name.c_str(), JS_NewCFunction(ctx, func, function_name.c_str(), 0));
				JS_SetPropertyStr(ctx, middleObject, obj3_name.c_str(), innerObject);
				JS_SetPropertyStr(ctx, outerObject, obj2_name.c_str(), middleObject);
				JS_SetPropertyStr(ctx, global_obj, obj1_name.c_str(), outerObject);
				JS_FreeValue(ctx, global_obj);
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
				const std::string & obj1_name,
				const std::string & obj2_name,
				const std::string & obj3_name,
				const std::string & obj4_name,
				const std::string & function_name
			) const -> void 
			{
				auto global_obj = JS_GetGlobalObject(ctx);
				auto outerObject = JS_GetPropertyStr(ctx, global_obj, obj1_name.c_str());
				if (JS_IsUndefined(outerObject)) {
					outerObject = JS_NewObject(ctx);
				}
				auto middle1Object = JS_GetPropertyStr(ctx, outerObject, obj2_name.c_str());
				if (JS_IsUndefined(middle1Object)) {
					middle1Object = JS_NewObject(ctx);
				}
				auto middle2Object = JS_GetPropertyStr(ctx, middle1Object, obj3_name.c_str());
				if (JS_IsUndefined(middle2Object)) {
					middle2Object = JS_NewObject(ctx);
				}
				auto innerObject = JS_GetPropertyStr(ctx, middle2Object, obj4_name.c_str());
				if (JS_IsUndefined(innerObject)) {
					innerObject = JS_NewObject(ctx);
				}
				JS_SetPropertyStr(ctx, innerObject, function_name.c_str(), JS_NewCFunction(ctx, func, function_name.c_str(), 0));
				JS_SetPropertyStr(ctx, middle2Object, obj4_name.c_str(), innerObject);
				JS_SetPropertyStr(ctx, middle1Object, obj3_name.c_str(), middle2Object);
				JS_SetPropertyStr(ctx, outerObject, obj2_name.c_str(), middle1Object);
				JS_SetPropertyStr(ctx, global_obj, obj1_name.c_str(), outerObject);
				JS_FreeValue(ctx, global_obj);
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
				const std::string & obj1_name,
				const std::string & obj2_name,
				const std::string & obj3_name,
				const std::string & obj4_name,
				const std::string & obj5_name,
				const std::string & function_name
			) const -> void 
			{
				auto global_obj = JS_GetGlobalObject(ctx);
				auto outerObject = JS_GetPropertyStr(ctx, global_obj, obj1_name.c_str());
				if (JS_IsUndefined(outerObject)) {
					outerObject = JS_NewObject(ctx);
				}
				auto middle1Object = JS_GetPropertyStr(ctx, outerObject, obj2_name.c_str());
				if (JS_IsUndefined(middle1Object)) {
					middle1Object = JS_NewObject(ctx);
				}
				auto middle2Object = JS_GetPropertyStr(ctx, middle1Object, obj3_name.c_str());
				if (JS_IsUndefined(middle2Object)) {
					middle2Object = JS_NewObject(ctx);
				}
				auto middle3Object = JS_GetPropertyStr(ctx, middle2Object, obj4_name.c_str());
				if (JS_IsUndefined(middle3Object)) {
					middle3Object = JS_NewObject(ctx);
				}
				auto innerObject = JS_GetPropertyStr(ctx, middle3Object, obj5_name.c_str());
				if (JS_IsUndefined(innerObject)) {
					innerObject = JS_NewObject(ctx);
				}
				JS_SetPropertyStr(ctx, innerObject, function_name.c_str(), JS_NewCFunction(ctx, func, function_name.c_str(), 0));
				JS_SetPropertyStr(ctx, middle3Object, obj5_name.c_str(), innerObject);
				JS_SetPropertyStr(ctx, middle2Object, obj4_name.c_str(), middle3Object);
				JS_SetPropertyStr(ctx, middle1Object, obj3_name.c_str(), middle2Object);
				JS_SetPropertyStr(ctx, outerObject, obj2_name.c_str(), middle1Object);
				JS_SetPropertyStr(ctx, global_obj, obj1_name.c_str(), outerObject);
				JS_FreeValue(ctx, global_obj);
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
				const std::string & obj1_name,
				const std::string & obj2_name,
				const std::string & obj3_name,
				const std::string & obj4_name,
				const std::string & obj5_name,
				const std::string & obj6_name,
				const std::string & function_name
			) const -> void 
			{
				auto global_obj = JS_GetGlobalObject(ctx);
				auto outerObject = JS_GetPropertyStr(ctx, global_obj, obj1_name.c_str());
				if (JS_IsUndefined(outerObject)) {
					outerObject = JS_NewObject(ctx);
				}
				auto middle1Object = JS_GetPropertyStr(ctx, outerObject, obj2_name.c_str());
				if (JS_IsUndefined(middle1Object)) {
					middle1Object = JS_NewObject(ctx);
				}
				auto middle2Object = JS_GetPropertyStr(ctx, middle1Object, obj3_name.c_str());
				if (JS_IsUndefined(middle2Object)) {
					middle2Object = JS_NewObject(ctx);
				}
				auto middle3Object = JS_GetPropertyStr(ctx, middle2Object, obj4_name.c_str());
				if (JS_IsUndefined(middle3Object)) {
					middle3Object = JS_NewObject(ctx);
				}
				auto middle4Object = JS_GetPropertyStr(ctx, middle3Object, obj5_name.c_str());
				if (JS_IsUndefined(middle4Object)) {
					middle4Object = JS_NewObject(ctx);
				}
				auto innerObject = JS_GetPropertyStr(ctx, middle4Object, obj6_name.c_str());
				if (JS_IsUndefined(innerObject)) {
					innerObject = JS_NewObject(ctx);
				}
				JS_SetPropertyStr(ctx, innerObject, function_name.c_str(), JS_NewCFunction(ctx, func, function_name.c_str(), 0));
				JS_SetPropertyStr(ctx, middle4Object, obj6_name.c_str(), innerObject);
				JS_SetPropertyStr(ctx, middle3Object, obj5_name.c_str(), middle4Object);
				JS_SetPropertyStr(ctx, middle2Object, obj4_name.c_str(), middle3Object);
				JS_SetPropertyStr(ctx, middle1Object, obj3_name.c_str(), middle2Object);
				JS_SetPropertyStr(ctx, outerObject, obj2_name.c_str(), middle1Object);
				JS_SetPropertyStr(ctx, global_obj, obj1_name.c_str(), outerObject);
				JS_FreeValue(ctx, global_obj);
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
				const std::string & obj1_name,
				const std::string & obj2_name,
				const std::string & obj3_name,
				const std::string & obj4_name,
				const std::string & obj5_name,
				const std::string & obj6_name,
				const std::string & obj7_name,
				const std::string & function_name
			) -> void 
			{
				auto global_obj = JS_GetGlobalObject(ctx);
				auto outerObject = JS_GetPropertyStr(ctx, global_obj, obj1_name.c_str());
				if (JS_IsUndefined(outerObject)) {
					outerObject = JS_NewObject(ctx);
				}
				auto middle1Object = JS_GetPropertyStr(ctx, outerObject, obj2_name.c_str());
				if (JS_IsUndefined(middle1Object)) {
					middle1Object = JS_NewObject(ctx);
				}
				auto middle2Object = JS_GetPropertyStr(ctx, middle1Object, obj3_name.c_str());
				if (JS_IsUndefined(middle2Object)) {
					middle2Object = JS_NewObject(ctx);
				}
				auto middle3Object = JS_GetPropertyStr(ctx, middle2Object, obj4_name.c_str());
				if (JS_IsUndefined(middle3Object)) {
					middle3Object = JS_NewObject(ctx);
				}
				auto middle4Object = JS_GetPropertyStr(ctx, middle3Object, obj5_name.c_str());
				if (JS_IsUndefined(middle4Object)) {
					middle4Object = JS_NewObject(ctx);
				}
				auto middle5Object = JS_GetPropertyStr(ctx, middle4Object, obj6_name.c_str());
				if (JS_IsUndefined(middle5Object)) {
					middle5Object = JS_NewObject(ctx);
				}
				auto innerObject = JS_GetPropertyStr(ctx, middle5Object, obj7_name.c_str());
				if (JS_IsUndefined(innerObject)) {
					innerObject = JS_NewObject(ctx);
				}
				JS_SetPropertyStr(ctx, innerObject, function_name.c_str(), JS_NewCFunction(ctx, func, function_name.c_str(), 0));
				JS_SetPropertyStr(ctx, middle5Object, obj7_name.c_str(), innerObject);
				JS_SetPropertyStr(ctx, middle4Object, obj6_name.c_str(), middle5Object);
				JS_SetPropertyStr(ctx, middle3Object, obj5_name.c_str(), middle4Object);
				JS_SetPropertyStr(ctx, middle2Object, obj4_name.c_str(), middle3Object);
				JS_SetPropertyStr(ctx, middle1Object, obj3_name.c_str(), middle2Object);
				JS_SetPropertyStr(ctx, outerObject, obj2_name.c_str(), middle1Object);
				JS_SetPropertyStr(ctx, global_obj, obj1_name.c_str(), outerObject);
				JS_FreeValue(ctx, global_obj);
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
				const std::string & obj1_name,
				const std::string & obj2_name,
				const std::string & obj3_name,
				const std::string & obj4_name,
				const std::string & obj5_name,
				const std::string & obj6_name,
				const std::string & obj7_name,
				const std::string & obj8_name,
				const std::string & function_name
			) -> void 
			{
				auto global_obj = JS_GetGlobalObject(ctx);
				auto outerObject = JS_GetPropertyStr(ctx, global_obj, obj1_name.c_str());
				if (JS_IsUndefined(outerObject)) {
					outerObject = JS_NewObject(ctx);
				}
				auto middle1Object = JS_GetPropertyStr(ctx, outerObject, obj2_name.c_str());
				if (JS_IsUndefined(middle1Object)) {
					middle1Object = JS_NewObject(ctx);
				}
				auto middle2Object = JS_GetPropertyStr(ctx, middle1Object, obj3_name.c_str());
				if (JS_IsUndefined(middle2Object)) {
					middle2Object = JS_NewObject(ctx);
				}
				auto middle3Object = JS_GetPropertyStr(ctx, middle2Object, obj4_name.c_str());
				if (JS_IsUndefined(middle3Object)) {
					middle3Object = JS_NewObject(ctx);
				}
				auto middle4Object = JS_GetPropertyStr(ctx, middle3Object, obj5_name.c_str());
				if (JS_IsUndefined(middle4Object)) {
					middle4Object = JS_NewObject(ctx);
				}
				auto middle5Object = JS_GetPropertyStr(ctx, middle4Object, obj6_name.c_str());
				if (JS_IsUndefined(middle5Object)) {
					middle5Object = JS_NewObject(ctx);
				}
				auto middle6Object = JS_GetPropertyStr(ctx, middle5Object, obj7_name.c_str());
				if (JS_IsUndefined(middle6Object)) {
					middle6Object = JS_NewObject(ctx);
				}
				auto innerObject = JS_GetPropertyStr(ctx, middle6Object, obj8_name.c_str());
				if (JS_IsUndefined(innerObject)) {
					innerObject = JS_NewObject(ctx);
				}
				JS_SetPropertyStr(ctx, innerObject, function_name.c_str(), JS_NewCFunction(ctx, func, function_name.c_str(), 0));
				JS_SetPropertyStr(ctx, middle6Object, obj8_name.c_str(), innerObject);
				JS_SetPropertyStr(ctx, middle5Object, obj7_name.c_str(), middle6Object);
				JS_SetPropertyStr(ctx, middle4Object, obj6_name.c_str(), middle5Object);
				JS_SetPropertyStr(ctx, middle3Object, obj5_name.c_str(), middle4Object);
				JS_SetPropertyStr(ctx, middle2Object, obj4_name.c_str(), middle3Object);
				JS_SetPropertyStr(ctx, middle1Object, obj3_name.c_str(), middle2Object);
				JS_SetPropertyStr(ctx, outerObject, obj2_name.c_str(), middle1Object);
				JS_SetPropertyStr(ctx, global_obj, obj1_name.c_str(), outerObject);
				JS_FreeValue(ctx, global_obj);
				return;
			}

			/**
			 * --------------------------------------
			 * Add a constant JS value from C
			 * @param value: the C string value
			 * @param var_name: JS variable name
			 * --------------------------------------
			*/

			inline auto add_constant(
				const std::string & value,
				const std::string & var_name
			) -> void 
			{
				auto global_obj = JS_GetGlobalObject(ctx);
				JS_SetPropertyStr(ctx, global_obj, var_name.c_str(), JS_NewString(ctx, value.c_str()));
				JS_FreeValue(ctx, global_obj);
				return;
			}

			/**
			 * --------------------------------------
			 * Add a constant JS value from C
			 * @param value: the integer 32 value
			 * @param var_name: JS variable name
			 * --------------------------------------
			*/

			inline auto add_constant(
				int32_t value,
				const std::string & var_name
			) -> void 
			{
				auto global_obj = JS_GetGlobalObject(ctx);
				JS_SetPropertyStr(ctx, global_obj, var_name.c_str(), JS_NewInt32(ctx, value));
				JS_FreeValue(ctx, global_obj);
				return;
			}

			/**
			 * --------------------------------------
			 * Add a constant JS value from C
			 * @param value: the unsigned integer 32 value
			 * @param var_name: JS variable name
			 * --------------------------------------
			*/

			inline auto add_constant(
				uint32_t value,
				const std::string & var_name
			) -> void 
			{
				auto global_obj = JS_GetGlobalObject(ctx);
				JS_SetPropertyStr(ctx, global_obj, var_name.c_str(), JS_NewUint32(ctx, value));
				JS_FreeValue(ctx, global_obj);
				return;
			}

			/**
			 * --------------------------------------
			 * Add a constant JS value from C
			 * @param value: the integer 64 value
			 * @param var_name: JS variable name
			 * --------------------------------------
			*/

			inline auto add_constant(
				int64_t value,
				const std::string & var_name
			) -> void 
			{
				auto global_obj = JS_GetGlobalObject(ctx);
				JS_SetPropertyStr(ctx, global_obj, var_name.c_str(), JS_NewInt64(ctx, value));
				JS_FreeValue(ctx, global_obj);
				return;
			}

			/**
			 * --------------------------------------
			 * Add a constant JS value from C
			 * @param value: the unsigned integer 64 value
			 * @param var_name: JS variable name
			 * --------------------------------------
			*/

			inline auto add_constant(
				uint64_t value,
				const std::string & var_name
			) -> void 
			{
				auto global_obj = JS_GetGlobalObject(ctx);
				JS_SetPropertyStr(ctx, global_obj, var_name.c_str(), JS_NewBigUint64(ctx, value));
				JS_FreeValue(ctx, global_obj);
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

			inline auto add_constant(
				const std::string & value,
				const std::string & object_name,
				const std::string & property_name
			) -> void 
			{
				auto global_obj = JS_GetGlobalObject(ctx);
				auto myObject = JS_GetPropertyStr(ctx, global_obj, object_name.c_str());
				if (JS_IsUndefined(myObject)) {
					myObject = JS_NewObject(ctx);
				}
				JS_SetPropertyStr(ctx, myObject, property_name.c_str(), JS_NewString(ctx, value.c_str()));
				JS_SetPropertyStr(ctx, global_obj, object_name.c_str(), myObject);
				JS_FreeValue(ctx, global_obj);
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

			inline auto add_constant(
				int32_t value,
				const std::string & object_name,
				const std::string & property_name
			) -> void 
			{
				auto global_obj = JS_GetGlobalObject(ctx);
				auto myObject = JS_GetPropertyStr(ctx, global_obj, object_name.c_str());
				if (JS_IsUndefined(myObject)) {
					myObject = JS_NewObject(ctx);
				}
				JS_SetPropertyStr(ctx, myObject, property_name.c_str(), JS_NewInt32(ctx, value));
				JS_SetPropertyStr(ctx, global_obj, object_name.c_str(), myObject);
				JS_FreeValue(ctx, global_obj);
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

			inline auto add_constant(
				uint32_t value,
				const std::string & object_name,
				const std::string & property_name
			) -> void 
			{
				auto global_obj = JS_GetGlobalObject(ctx);
				auto myObject = JS_GetPropertyStr(ctx, global_obj, object_name.c_str());
				if (JS_IsUndefined(myObject)) {
					myObject = JS_NewObject(ctx);
				}
				JS_SetPropertyStr(ctx, myObject, property_name.c_str(), JS_NewUint32(ctx, value));
				JS_SetPropertyStr(ctx, global_obj, object_name.c_str(), myObject);
				JS_FreeValue(ctx, global_obj);
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

			inline auto add_constant(
				int64_t value,
				const std::string & object_name,
				const std::string & property_name
			) -> void 
			{
				auto global_obj = JS_GetGlobalObject(ctx);
				auto myObject = JS_GetPropertyStr(ctx, global_obj, object_name.c_str());
				if (JS_IsUndefined(myObject)) {
					myObject = JS_NewObject(ctx);
				}
				JS_SetPropertyStr(ctx, myObject, property_name.c_str(), JS_NewInt64(ctx, value));
				JS_SetPropertyStr(ctx, global_obj, object_name.c_str(), myObject);
				JS_FreeValue(ctx, global_obj);
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

			inline auto add_constant(
				uint64_t value,
				const std::string & object_name,
				const std::string & property_name
			) -> void 
			{
				auto global_obj = JS_GetGlobalObject(ctx);
				auto myObject = JS_GetPropertyStr(ctx, global_obj, object_name.c_str());
				if (JS_IsUndefined(myObject)) {
					myObject = JS_NewObject(ctx);
				}
				JS_SetPropertyStr(ctx, myObject, property_name.c_str(), JS_NewBigUint64(ctx, value));
				JS_SetPropertyStr(ctx, global_obj, object_name.c_str(), myObject);
				JS_FreeValue(ctx, global_obj);
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

			inline auto add_constant(
				double value,
				const std::string & object_name,
				const std::string & property_name
			) -> void 
			{
				auto global_obj = JS_GetGlobalObject(ctx);
				auto myObject = JS_GetPropertyStr(ctx, global_obj, object_name.c_str());
				if (JS_IsUndefined(myObject)) {
					myObject = JS_NewObject(ctx);
				}
				JS_SetPropertyStr(ctx, myObject, property_name.c_str(), JS_NewFloat64(ctx, value));
				JS_SetPropertyStr(ctx, global_obj, object_name.c_str(), myObject);
				JS_FreeValue(ctx, global_obj);
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

			inline auto add_constant(
				float value,
				const std::string & object_name,
				const std::string & property_name
			) -> void 
			{
				auto global_obj = JS_GetGlobalObject(ctx);
				auto myObject = JS_GetPropertyStr(ctx, global_obj, object_name.c_str());
				if (JS_IsUndefined(myObject)) {
					myObject = JS_NewObject(ctx);
				}
				JS_SetPropertyStr(ctx, myObject, property_name.c_str(), JS_NewFloat64(ctx, static_cast<double>(value)));
				JS_SetPropertyStr(ctx, global_obj, object_name.c_str(), myObject);
				JS_FreeValue(ctx, global_obj);
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

			inline auto add_constant(
				bool value,
				const std::string & object_name,
				const std::string & property_name
			) -> void 
			{
				auto global_obj = JS_GetGlobalObject(ctx);
				auto myObject = JS_GetPropertyStr(ctx, global_obj, object_name.c_str());
				if (JS_IsUndefined(myObject)) {
					myObject = JS_NewObject(ctx);
				}
				JS_SetPropertyStr(ctx, myObject, property_name.c_str(), JS_NewBool(ctx, value ? 1 : 0));
				JS_SetPropertyStr(ctx, global_obj, object_name.c_str(), myObject);
				JS_FreeValue(ctx, global_obj);
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
				const std::string & value,
				const std::string & obj1_name,
				const std::string & obj2_name,
				const std::string & property_name
			) -> void 
			{
				auto global_obj = JS_GetGlobalObject(ctx);
				auto obj1 = JS_GetPropertyStr(ctx, global_obj, obj1_name.c_str());
				if (JS_IsUndefined(obj1)) {
					obj1 = JS_NewObject(ctx);
					JS_SetPropertyStr(ctx, global_obj, obj1_name.c_str(), obj1);
				}
				auto obj2 = JS_GetPropertyStr(ctx, obj1, obj2_name.c_str());
				if (JS_IsUndefined(obj2)) {
					obj2 = JS_NewObject(ctx);
					JS_SetPropertyStr(ctx, obj1, obj2_name.c_str(), obj2);
				}
				JS_SetPropertyStr(ctx, obj2, property_name.c_str(), JS_NewString(ctx, value.c_str()));
				JS_FreeValue(ctx, global_obj);
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
				int32_t value,
				const std::string & obj1_name,
				const std::string & obj2_name,
				const std::string & property_name
			) -> void 
			{
				auto global_obj = JS_GetGlobalObject(ctx);
				auto obj1 = JS_GetPropertyStr(ctx, global_obj, obj1_name.c_str());
				if (JS_IsUndefined(obj1)) {
					obj1 = JS_NewObject(ctx);
					JS_SetPropertyStr(ctx, global_obj, obj1_name.c_str(), obj1);
				}
				auto obj2 = JS_GetPropertyStr(ctx, obj1, obj2_name.c_str());
				if (JS_IsUndefined(obj2)) {
					obj2 = JS_NewObject(ctx);
					JS_SetPropertyStr(ctx, obj1, obj2_name.c_str(), obj2);
				}
				JS_SetPropertyStr(ctx, obj2, property_name.c_str(), JS_NewInt32(ctx, value));
				JS_FreeValue(ctx, global_obj);
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
				uint32_t value,
				const std::string & obj1_name,
				const std::string & obj2_name,
				const std::string & property_name
			) -> void 
			{
				auto global_obj = JS_GetGlobalObject(ctx);
				auto obj1 = JS_GetPropertyStr(ctx, global_obj, obj1_name.c_str());
				if (JS_IsUndefined(obj1)) {
					obj1 = JS_NewObject(ctx);
					JS_SetPropertyStr(ctx, global_obj, obj1_name.c_str(), obj1);
				}
				auto obj2 = JS_GetPropertyStr(ctx, obj1, obj2_name.c_str());
				if (JS_IsUndefined(obj2)) {
					obj2 = JS_NewObject(ctx);
					JS_SetPropertyStr(ctx, obj1, obj2_name.c_str(), obj2);
				}
				JS_SetPropertyStr(ctx, obj2, property_name.c_str(), JS_NewUint32(ctx, value));
				JS_FreeValue(ctx, global_obj);
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
				int64_t value,
				const std::string & obj1_name,
				const std::string & obj2_name,
				const std::string & property_name
			) -> void 
			{
				auto global_obj = JS_GetGlobalObject(ctx);
				auto obj1 = JS_GetPropertyStr(ctx, global_obj, obj1_name.c_str());
				if (JS_IsUndefined(obj1)) {
					obj1 = JS_NewObject(ctx);
					JS_SetPropertyStr(ctx, global_obj, obj1_name.c_str(), obj1);
				}
				auto obj2 = JS_GetPropertyStr(ctx, obj1, obj2_name.c_str());
				if (JS_IsUndefined(obj2)) {
					obj2 = JS_NewObject(ctx);
					JS_SetPropertyStr(ctx, obj1, obj2_name.c_str(), obj2);
				}
				JS_SetPropertyStr(ctx, obj2, property_name.c_str(), JS_NewInt64(ctx, value));
				JS_FreeValue(ctx, global_obj);
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
				uint64_t value,
				const std::string & obj1_name,
				const std::string & obj2_name,
				const std::string & property_name
			) -> void 
			{
				auto global_obj = JS_GetGlobalObject(ctx);
				auto obj1 = JS_GetPropertyStr(ctx, global_obj, obj1_name.c_str());
				if (JS_IsUndefined(obj1)) {
					obj1 = JS_NewObject(ctx);
					JS_SetPropertyStr(ctx, global_obj, obj1_name.c_str(), obj1);
				}
				auto obj2 = JS_GetPropertyStr(ctx, obj1, obj2_name.c_str());
				if (JS_IsUndefined(obj2)) {
					obj2 = JS_NewObject(ctx);
					JS_SetPropertyStr(ctx, obj1, obj2_name.c_str(), obj2);
				}
				JS_SetPropertyStr(ctx, obj2, property_name.c_str(), JS_NewBigUint64(ctx, value));
				JS_FreeValue(ctx, global_obj);
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
				double value,
				const std::string & obj1_name,
				const std::string & obj2_name,
				const std::string & property_name
			) -> void 
			{
				auto global_obj = JS_GetGlobalObject(ctx);
				auto obj1 = JS_GetPropertyStr(ctx, global_obj, obj1_name.c_str());
				if (JS_IsUndefined(obj1)) {
					obj1 = JS_NewObject(ctx);
					JS_SetPropertyStr(ctx, global_obj, obj1_name.c_str(), obj1);
				}
				auto obj2 = JS_GetPropertyStr(ctx, obj1, obj2_name.c_str());
				if (JS_IsUndefined(obj2)) {
					obj2 = JS_NewObject(ctx);
					JS_SetPropertyStr(ctx, obj1, obj2_name.c_str(), obj2);
				}
				JS_SetPropertyStr(ctx, obj2, property_name.c_str(), JS_NewFloat64(ctx, value));
				JS_FreeValue(ctx, global_obj);
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
				float value,
				const std::string & obj1_name,
				const std::string & obj2_name,
				const std::string & property_name
			) -> void 
			{
				auto global_obj = JS_GetGlobalObject(ctx);
				auto obj1 = JS_GetPropertyStr(ctx, global_obj, obj1_name.c_str());
				if (JS_IsUndefined(obj1)) {
					obj1 = JS_NewObject(ctx);
					JS_SetPropertyStr(ctx, global_obj, obj1_name.c_str(), obj1);
				}
				auto obj2 = JS_GetPropertyStr(ctx, obj1, obj2_name.c_str());
				if (JS_IsUndefined(obj2)) {
					obj2 = JS_NewObject(ctx);
					JS_SetPropertyStr(ctx, obj1, obj2_name.c_str(), obj2);
				}
				JS_SetPropertyStr(ctx, obj2, property_name.c_str(), JS_NewFloat64(ctx, static_cast<double>(value)));
				JS_FreeValue(ctx, global_obj);
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
				bool value,
				const std::string & obj1_name,
				const std::string & obj2_name,
				const std::string & property_name
			) -> void 
			{
				auto global_obj = JS_GetGlobalObject(ctx);
				auto obj1 = JS_GetPropertyStr(ctx, global_obj, obj1_name.c_str());
				if (JS_IsUndefined(obj1)) {
					obj1 = JS_NewObject(ctx);
					JS_SetPropertyStr(ctx, global_obj, obj1_name.c_str(), obj1);
				}
				auto obj2 = JS_GetPropertyStr(ctx, obj1, obj2_name.c_str());
				if (JS_IsUndefined(obj2)) {
					obj2 = JS_NewObject(ctx);
					JS_SetPropertyStr(ctx, obj1, obj2_name.c_str(), obj2);
				}
				JS_SetPropertyStr(ctx, obj2, property_name.c_str(), JS_NewBool(ctx, value ? 1 : 0));
				JS_FreeValue(ctx, global_obj);
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

			inline auto add_constant(
				const std::string & value,
				const std::string & obj1_name,
				const std::string & obj2_name,
				const std::string & obj3_name,
				const std::string & property_name
			) -> void 
			{
				auto global_obj = JS_GetGlobalObject(ctx);
				auto obj1 = JS_GetPropertyStr(ctx, global_obj, obj1_name.c_str());
				if (JS_IsUndefined(obj1)) {
					obj1 = JS_NewObject(ctx);
					JS_SetPropertyStr(ctx, global_obj, obj1_name.c_str(), obj1);
				}
				auto obj2 = JS_GetPropertyStr(ctx, obj1, obj2_name.c_str());
				if (JS_IsUndefined(obj2)) {
					obj2 = JS_NewObject(ctx);
					JS_SetPropertyStr(ctx, obj1, obj2_name.c_str(), obj2);
				}
				auto obj3 = JS_GetPropertyStr(ctx, obj2, obj3_name.c_str());
				if (JS_IsUndefined(obj3)) {
					obj3 = JS_NewObject(ctx);
					JS_SetPropertyStr(ctx, obj2, obj3_name.c_str(), obj3);
				}
				JS_SetPropertyStr(ctx, obj3, property_name.c_str(), JS_NewString(ctx, value.c_str()));
				JS_FreeValue(ctx, global_obj);
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

			inline auto add_constant(
				int32_t value,
				const std::string & obj1_name,
				const std::string & obj2_name,
				const std::string & obj3_name,
				const std::string & property_name
			) -> void 
			{
				auto global_obj = JS_GetGlobalObject(ctx);
				auto obj1 = JS_GetPropertyStr(ctx, global_obj, obj1_name.c_str());
				if (JS_IsUndefined(obj1)) {
					obj1 = JS_NewObject(ctx);
					JS_SetPropertyStr(ctx, global_obj, obj1_name.c_str(), obj1);
				}
				auto obj2 = JS_GetPropertyStr(ctx, obj1, obj2_name.c_str());
				if (JS_IsUndefined(obj2)) {
					obj2 = JS_NewObject(ctx);
					JS_SetPropertyStr(ctx, obj1, obj2_name.c_str(), obj2);
				}
				auto obj3 = JS_GetPropertyStr(ctx, obj2, obj3_name.c_str());
				if (JS_IsUndefined(obj3)) {
					obj3 = JS_NewObject(ctx);
					JS_SetPropertyStr(ctx, obj2, obj3_name.c_str(), obj3);
				}
				JS_SetPropertyStr(ctx, obj3, property_name.c_str(), JS_NewInt32(ctx, value));
				JS_FreeValue(ctx, global_obj);
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

			inline auto add_constant(
				uint32_t value,
				const std::string & obj1_name,
				const std::string & obj2_name,
				const std::string & obj3_name,
				const std::string & property_name
			) -> void 
			{
				auto global_obj = JS_GetGlobalObject(ctx);
				auto obj1 = JS_GetPropertyStr(ctx, global_obj, obj1_name.c_str());
				if (JS_IsUndefined(obj1)) {
					obj1 = JS_NewObject(ctx);
					JS_SetPropertyStr(ctx, global_obj, obj1_name.c_str(), obj1);
				}
				auto obj2 = JS_GetPropertyStr(ctx, obj1, obj2_name.c_str());
				if (JS_IsUndefined(obj2)) {
					obj2 = JS_NewObject(ctx);
					JS_SetPropertyStr(ctx, obj1, obj2_name.c_str(), obj2);
				}
				auto obj3 = JS_GetPropertyStr(ctx, obj2, obj3_name.c_str());
				if (JS_IsUndefined(obj3)) {
					obj3 = JS_NewObject(ctx);
					JS_SetPropertyStr(ctx, obj2, obj3_name.c_str(), obj3);
				}
				JS_SetPropertyStr(ctx, obj3, property_name.c_str(), JS_NewUint32(ctx, value));
				JS_FreeValue(ctx, global_obj);
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

			inline auto add_constant(
				int64_t value,
				const std::string & obj1_name,
				const std::string & obj2_name,
				const std::string & obj3_name,
				const std::string & property_name
			) -> void 
			{
				auto global_obj = JS_GetGlobalObject(ctx);
				auto obj1 = JS_GetPropertyStr(ctx, global_obj, obj1_name.c_str());
				if (JS_IsUndefined(obj1)) {
					obj1 = JS_NewObject(ctx);
					JS_SetPropertyStr(ctx, global_obj, obj1_name.c_str(), obj1);
				}
				auto obj2 = JS_GetPropertyStr(ctx, obj1, obj2_name.c_str());
				if (JS_IsUndefined(obj2)) {
					obj2 = JS_NewObject(ctx);
					JS_SetPropertyStr(ctx, obj1, obj2_name.c_str(), obj2);
				}
				auto obj3 = JS_GetPropertyStr(ctx, obj2, obj3_name.c_str());
				if (JS_IsUndefined(obj3)) {
					obj3 = JS_NewObject(ctx);
					JS_SetPropertyStr(ctx, obj2, obj3_name.c_str(), obj3);
				}
				JS_SetPropertyStr(ctx, obj3, property_name.c_str(), JS_NewInt64(ctx, value));
				JS_FreeValue(ctx, global_obj);
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

			inline auto add_constant(
				uint64_t value,
				const std::string & obj1_name,
				const std::string & obj2_name,
				const std::string & obj3_name,
				const std::string & property_name
			) -> void 
			{
				auto global_obj = JS_GetGlobalObject(ctx);
				auto obj1 = JS_GetPropertyStr(ctx, global_obj, obj1_name.c_str());
				if (JS_IsUndefined(obj1)) {
					obj1 = JS_NewObject(ctx);
					JS_SetPropertyStr(ctx, global_obj, obj1_name.c_str(), obj1);
				}
				auto obj2 = JS_GetPropertyStr(ctx, obj1, obj2_name.c_str());
				if (JS_IsUndefined(obj2)) {
					obj2 = JS_NewObject(ctx);
					JS_SetPropertyStr(ctx, obj1, obj2_name.c_str(), obj2);
				}
				auto obj3 = JS_GetPropertyStr(ctx, obj2, obj3_name.c_str());
				if (JS_IsUndefined(obj3)) {
					obj3 = JS_NewObject(ctx);
					JS_SetPropertyStr(ctx, obj2, obj3_name.c_str(), obj3);
				}
				JS_SetPropertyStr(ctx, obj3, property_name.c_str(), JS_NewBigUint64(ctx, value));
				JS_FreeValue(ctx, global_obj);
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

			inline auto add_constant(
				double value,
				const std::string & obj1_name,
				const std::string & obj2_name,
				const std::string & obj3_name,
				const std::string & property_name
			) -> void 
			{
				auto global_obj = JS_GetGlobalObject(ctx);
				auto obj1 = JS_GetPropertyStr(ctx, global_obj, obj1_name.c_str());
				if (JS_IsUndefined(obj1)) {
					obj1 = JS_NewObject(ctx);
					JS_SetPropertyStr(ctx, global_obj, obj1_name.c_str(), obj1);
				}
				auto obj2 = JS_GetPropertyStr(ctx, obj1, obj2_name.c_str());
				if (JS_IsUndefined(obj2)) {
					obj2 = JS_NewObject(ctx);
					JS_SetPropertyStr(ctx, obj1, obj2_name.c_str(), obj2);
				}
				auto obj3 = JS_GetPropertyStr(ctx, obj2, obj3_name.c_str());
				if (JS_IsUndefined(obj3)) {
					obj3 = JS_NewObject(ctx);
					JS_SetPropertyStr(ctx, obj2, obj3_name.c_str(), obj3);
				}
				JS_SetPropertyStr(ctx, obj3, property_name.c_str(), JS_NewFloat64(ctx, value));
				JS_FreeValue(ctx, global_obj);
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

			inline auto add_constant(
				float value,
				const std::string & obj1_name,
				const std::string & obj2_name,
				const std::string & obj3_name,
				const std::string & property_name
			) -> void 
			{
				auto global_obj = JS_GetGlobalObject(ctx);
				auto obj1 = JS_GetPropertyStr(ctx, global_obj, obj1_name.c_str());
				if (JS_IsUndefined(obj1)) {
					obj1 = JS_NewObject(ctx);
					JS_SetPropertyStr(ctx, global_obj, obj1_name.c_str(), obj1);
				}
				auto obj2 = JS_GetPropertyStr(ctx, obj1, obj2_name.c_str());
				if (JS_IsUndefined(obj2)) {
					obj2 = JS_NewObject(ctx);
					JS_SetPropertyStr(ctx, obj1, obj2_name.c_str(), obj2);
				}
				auto obj3 = JS_GetPropertyStr(ctx, obj2, obj3_name.c_str());
				if (JS_IsUndefined(obj3)) {
					obj3 = JS_NewObject(ctx);
					JS_SetPropertyStr(ctx, obj2, obj3_name.c_str(), obj3);
				}
				JS_SetPropertyStr(ctx, obj3, property_name.c_str(), JS_NewFloat64(ctx, value));
				JS_FreeValue(ctx, global_obj);
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

			inline auto add_constant(
				bool value,
				const std::string & obj1_name,
				const std::string & obj2_name,
				const std::string & obj3_name,
				const std::string & property_name
			) -> void 
			{
				auto global_obj = JS_GetGlobalObject(ctx);
				auto obj1 = JS_GetPropertyStr(ctx, global_obj, obj1_name.c_str());
				if (JS_IsUndefined(obj1)) {
					obj1 = JS_NewObject(ctx);
					JS_SetPropertyStr(ctx, global_obj, obj1_name.c_str(), obj1);
				}
				auto obj2 = JS_GetPropertyStr(ctx, obj1, obj2_name.c_str());
				if (JS_IsUndefined(obj2)) {
					obj2 = JS_NewObject(ctx);
					JS_SetPropertyStr(ctx, obj1, obj2_name.c_str(), obj2);
				}
				auto obj3 = JS_GetPropertyStr(ctx, obj2, obj3_name.c_str());
				if (JS_IsUndefined(obj3)) {
					obj3 = JS_NewObject(ctx);
					JS_SetPropertyStr(ctx, obj2, obj3_name.c_str(), obj3);
				}
				JS_SetPropertyStr(ctx, obj3, property_name.c_str(), JS_NewBool(ctx, value ? 1 : 0));
				JS_FreeValue(ctx, global_obj);
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

			inline auto add_constant(
				const std::string & value,
				const std::string & obj1_name,
				const std::string & obj2_name,
				const std::string & obj3_name,
				const std::string & obj4_name,
				const std::string & property_name
			) -> void 
			{
				auto global_obj = JS_GetGlobalObject(ctx);
				auto obj1 = JS_GetPropertyStr(ctx, global_obj, obj1_name.c_str());
				if (JS_IsUndefined(obj1)) {
					obj1 = JS_NewObject(ctx);
					JS_SetPropertyStr(ctx, global_obj, obj1_name.c_str(), obj1);
				}
				auto obj2 = JS_GetPropertyStr(ctx, obj1, obj2_name.c_str());
				if (JS_IsUndefined(obj2)) {
					obj2 = JS_NewObject(ctx);
					JS_SetPropertyStr(ctx, obj1, obj2_name.c_str(), obj2);
				}
				auto obj3 = JS_GetPropertyStr(ctx, obj2, obj3_name.c_str());
				if (JS_IsUndefined(obj3)) {
					obj3 = JS_NewObject(ctx);
					JS_SetPropertyStr(ctx, obj2, obj3_name.c_str(), obj3);
				}
				auto obj4 = JS_GetPropertyStr(ctx, obj3, obj4_name.c_str());
				if (JS_IsUndefined(obj4)) {
					obj4 = JS_NewObject(ctx);
					JS_SetPropertyStr(ctx, obj3, obj4_name.c_str(), obj4);
				}
				JS_SetPropertyStr(ctx, obj4, property_name.c_str(), JS_NewString(ctx, value.c_str()));
				JS_FreeValue(ctx, global_obj);
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

			inline auto add_constant(
				int32_t value,
				const std::string & obj1_name,
				const std::string & obj2_name,
				const std::string & obj3_name,
				const std::string & obj4_name,
				const std::string & property_name
			) -> void 
			{
				auto global_obj = JS_GetGlobalObject(ctx);
				auto obj1 = JS_GetPropertyStr(ctx, global_obj, obj1_name.c_str());
				if (JS_IsUndefined(obj1)) {
					obj1 = JS_NewObject(ctx);
					JS_SetPropertyStr(ctx, global_obj, obj1_name.c_str(), obj1);
				}
				auto obj2 = JS_GetPropertyStr(ctx, obj1, obj2_name.c_str());
				if (JS_IsUndefined(obj2)) {
					obj2 = JS_NewObject(ctx);
					JS_SetPropertyStr(ctx, obj1, obj2_name.c_str(), obj2);
				}
				auto obj3 = JS_GetPropertyStr(ctx, obj2, obj3_name.c_str());
				if (JS_IsUndefined(obj3)) {
					obj3 = JS_NewObject(ctx);
					JS_SetPropertyStr(ctx, obj2, obj3_name.c_str(), obj3);
				}
				auto obj4 = JS_GetPropertyStr(ctx, obj3, obj4_name.c_str());
				if (JS_IsUndefined(obj4)) {
					obj4 = JS_NewObject(ctx);
					JS_SetPropertyStr(ctx, obj3, obj4_name.c_str(), obj4);
				}
				JS_SetPropertyStr(ctx, obj4, property_name.c_str(), JS_NewInt32(ctx, value));
				JS_FreeValue(ctx, global_obj);
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

			inline auto add_constant(
				uint32_t value,
				const std::string & obj1_name,
				const std::string & obj2_name,
				const std::string & obj3_name,
				const std::string & obj4_name,
				const std::string & property_name
			) -> void 
			{
				auto global_obj = JS_GetGlobalObject(ctx);
				auto obj1 = JS_GetPropertyStr(ctx, global_obj, obj1_name.c_str());
				if (JS_IsUndefined(obj1)) {
					obj1 = JS_NewObject(ctx);
					JS_SetPropertyStr(ctx, global_obj, obj1_name.c_str(), obj1);
				}
				auto obj2 = JS_GetPropertyStr(ctx, obj1, obj2_name.c_str());
				if (JS_IsUndefined(obj2)) {
					obj2 = JS_NewObject(ctx);
					JS_SetPropertyStr(ctx, obj1, obj2_name.c_str(), obj2);
				}
				auto obj3 = JS_GetPropertyStr(ctx, obj2, obj3_name.c_str());
				if (JS_IsUndefined(obj3)) {
					obj3 = JS_NewObject(ctx);
					JS_SetPropertyStr(ctx, obj2, obj3_name.c_str(), obj3);
				}
				auto obj4 = JS_GetPropertyStr(ctx, obj3, obj4_name.c_str());
				if (JS_IsUndefined(obj4)) {
					obj4 = JS_NewObject(ctx);
					JS_SetPropertyStr(ctx, obj3, obj4_name.c_str(), obj4);
				}
				JS_SetPropertyStr(ctx, obj4, property_name.c_str(), JS_NewUint32(ctx, value));
				JS_FreeValue(ctx, global_obj);
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

			inline auto add_constant(
				int64_t value,
				const std::string & obj1_name,
				const std::string & obj2_name,
				const std::string & obj3_name,
				const std::string & obj4_name,
				const std::string & property_name
			) -> void 
			{
				auto global_obj = JS_GetGlobalObject(ctx);
				auto obj1 = JS_GetPropertyStr(ctx, global_obj, obj1_name.c_str());
				if (JS_IsUndefined(obj1)) {
					obj1 = JS_NewObject(ctx);
					JS_SetPropertyStr(ctx, global_obj, obj1_name.c_str(), obj1);
				}
				auto obj2 = JS_GetPropertyStr(ctx, obj1, obj2_name.c_str());
				if (JS_IsUndefined(obj2)) {
					obj2 = JS_NewObject(ctx);
					JS_SetPropertyStr(ctx, obj1, obj2_name.c_str(), obj2);
				}
				auto obj3 = JS_GetPropertyStr(ctx, obj2, obj3_name.c_str());
				if (JS_IsUndefined(obj3)) {
					obj3 = JS_NewObject(ctx);
					JS_SetPropertyStr(ctx, obj2, obj3_name.c_str(), obj3);
				}
				auto obj4 = JS_GetPropertyStr(ctx, obj3, obj4_name.c_str());
				if (JS_IsUndefined(obj4)) {
					obj4 = JS_NewObject(ctx);
					JS_SetPropertyStr(ctx, obj3, obj4_name.c_str(), obj4);
				}
				JS_SetPropertyStr(ctx, obj4, property_name.c_str(), JS_NewInt64(ctx, value));
				JS_FreeValue(ctx, global_obj);
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

			inline auto add_constant(
				uint64_t value,
				const std::string & obj1_name,
				const std::string & obj2_name,
				const std::string & obj3_name,
				const std::string & obj4_name,
				const std::string & property_name
			) -> void 
			{
				auto global_obj = JS_GetGlobalObject(ctx);
				auto obj1 = JS_GetPropertyStr(ctx, global_obj, obj1_name.c_str());
				if (JS_IsUndefined(obj1)) {
					obj1 = JS_NewObject(ctx);
					JS_SetPropertyStr(ctx, global_obj, obj1_name.c_str(), obj1);
				}
				auto obj2 = JS_GetPropertyStr(ctx, obj1, obj2_name.c_str());
				if (JS_IsUndefined(obj2)) {
					obj2 = JS_NewObject(ctx);
					JS_SetPropertyStr(ctx, obj1, obj2_name.c_str(), obj2);
				}
				auto obj3 = JS_GetPropertyStr(ctx, obj2, obj3_name.c_str());
				if (JS_IsUndefined(obj3)) {
					obj3 = JS_NewObject(ctx);
					JS_SetPropertyStr(ctx, obj2, obj3_name.c_str(), obj3);
				}
				auto obj4 = JS_GetPropertyStr(ctx, obj3, obj4_name.c_str());
				if (JS_IsUndefined(obj4)) {
					obj4 = JS_NewObject(ctx);
					JS_SetPropertyStr(ctx, obj3, obj4_name.c_str(), obj4);
				}
				 JS_SetPropertyStr(ctx, obj4, property_name.c_str(), JS_NewBigUint64(ctx, value));
				JS_FreeValue(ctx, global_obj);
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

			inline auto add_constant(
				double value,
				const std::string & obj1_name,
				const std::string & obj2_name,
				const std::string & obj3_name,
				const std::string & obj4_name,
				const std::string & property_name
			) -> void 
			{
				auto global_obj = JS_GetGlobalObject(ctx);
				auto obj1 = JS_GetPropertyStr(ctx, global_obj, obj1_name.c_str());
				if (JS_IsUndefined(obj1)) {
					obj1 = JS_NewObject(ctx);
					JS_SetPropertyStr(ctx, global_obj, obj1_name.c_str(), obj1);
				}
				auto obj2 = JS_GetPropertyStr(ctx, obj1, obj2_name.c_str());
				if (JS_IsUndefined(obj2)) {
					obj2 = JS_NewObject(ctx);
					JS_SetPropertyStr(ctx, obj1, obj2_name.c_str(), obj2);
				}
				auto obj3 = JS_GetPropertyStr(ctx, obj2, obj3_name.c_str());
				if (JS_IsUndefined(obj3)) {
					obj3 = JS_NewObject(ctx);
					JS_SetPropertyStr(ctx, obj2, obj3_name.c_str(), obj3);
				}
				auto obj4 = JS_GetPropertyStr(ctx, obj3, obj4_name.c_str());
				if (JS_IsUndefined(obj4)) {
					obj4 = JS_NewObject(ctx);
					JS_SetPropertyStr(ctx, obj3, obj4_name.c_str(), obj4);
				}
				JS_SetPropertyStr(ctx, obj4, property_name.c_str(), JS_NewFloat64(ctx, value));
				JS_FreeValue(ctx, global_obj);
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

			inline auto add_constant(
				float value,
				const std::string & obj1_name,
				const std::string & obj2_name,
				const std::string & obj3_name,
				const std::string & obj4_name,
				const std::string & property_name
			) -> void 
			{
				auto global_obj = JS_GetGlobalObject(ctx);
				auto obj1 = JS_GetPropertyStr(ctx, global_obj, obj1_name.c_str());
				if (JS_IsUndefined(obj1)) {
					obj1 = JS_NewObject(ctx);
					JS_SetPropertyStr(ctx, global_obj, obj1_name.c_str(), obj1);
				}
				auto obj2 = JS_GetPropertyStr(ctx, obj1, obj2_name.c_str());
				if (JS_IsUndefined(obj2)) {
					obj2 = JS_NewObject(ctx);
					JS_SetPropertyStr(ctx, obj1, obj2_name.c_str(), obj2);
				}
				auto obj3 = JS_GetPropertyStr(ctx, obj2, obj3_name.c_str());
				if (JS_IsUndefined(obj3)) {
					obj3 = JS_NewObject(ctx);
					JS_SetPropertyStr(ctx, obj2, obj3_name.c_str(), obj3);
				}
				auto obj4 = JS_GetPropertyStr(ctx, obj3, obj4_name.c_str());
				if (JS_IsUndefined(obj4)) {
					obj4 = JS_NewObject(ctx);
					JS_SetPropertyStr(ctx, obj3, obj4_name.c_str(), obj4);
				}
				JS_SetPropertyStr(ctx, obj4, property_name.c_str(), JS_NewFloat64(ctx, static_cast<double>(value)));
				JS_FreeValue(ctx, global_obj);
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

			inline auto add_constant(
				bool value,
				const std::string & obj1_name,
				const std::string & obj2_name,
				const std::string & obj3_name,
				const std::string & obj4_name,
				const std::string & property_name
			) -> void 
			{
				auto global_obj = JS_GetGlobalObject(ctx);
				auto obj1 = JS_GetPropertyStr(ctx, global_obj, obj1_name.c_str());
				if (JS_IsUndefined(obj1)) {
					obj1 = JS_NewObject(ctx);
					JS_SetPropertyStr(ctx, global_obj, obj1_name.c_str(), obj1);
				}
				auto obj2 = JS_GetPropertyStr(ctx, obj1, obj2_name.c_str());
				if (JS_IsUndefined(obj2)) {
					obj2 = JS_NewObject(ctx);
					JS_SetPropertyStr(ctx, obj1, obj2_name.c_str(), obj2);
				}
				auto obj3 = JS_GetPropertyStr(ctx, obj2, obj3_name.c_str());
				if (JS_IsUndefined(obj3)) {
					obj3 = JS_NewObject(ctx);
					JS_SetPropertyStr(ctx, obj2, obj3_name.c_str(), obj3);
				}
				auto obj4 = JS_GetPropertyStr(ctx, obj3, obj4_name.c_str());
				if (JS_IsUndefined(obj4)) {
					obj4 = JS_NewObject(ctx);
					JS_SetPropertyStr(ctx, obj3, obj4_name.c_str(), obj4);
				}
				JS_SetPropertyStr(ctx, obj4, property_name.c_str(), JS_NewBool(ctx, value ? 1 : 0));
				JS_FreeValue(ctx, global_obj);
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

			inline auto add_constant(
				const std::string & value,
				const std::string & obj1_name,
				const std::string & obj2_name,
				const std::string & obj3_name,
				const std::string & obj4_name,
				const std::string & obj5_name,
				const std::string & property_name
			) -> void 
			{
				auto global_obj = JS_GetGlobalObject(ctx);
				auto obj1 = JS_GetPropertyStr(ctx, global_obj, obj1_name.c_str());
				if (JS_IsUndefined(obj1)) {
					obj1 = JS_NewObject(ctx);
					JS_SetPropertyStr(ctx, global_obj, obj1_name.c_str(), obj1);
				}
				auto obj2 = JS_GetPropertyStr(ctx, obj1, obj2_name.c_str());
				if (JS_IsUndefined(obj2)) {
					obj2 = JS_NewObject(ctx);
					JS_SetPropertyStr(ctx, obj1, obj2_name.c_str(), obj2);
				}
				auto obj3 = JS_GetPropertyStr(ctx, obj2, obj3_name.c_str());
				if (JS_IsUndefined(obj3)) {
					obj3 = JS_NewObject(ctx);
					JS_SetPropertyStr(ctx, obj2, obj3_name.c_str(), obj3);
				}
				auto obj4 = JS_GetPropertyStr(ctx, obj3, obj4_name.c_str());
				if (JS_IsUndefined(obj4)) {
					obj4 = JS_NewObject(ctx);
					JS_SetPropertyStr(ctx, obj3, obj4_name.c_str(), obj4);
				}
				auto obj5 = JS_GetPropertyStr(ctx, obj4, obj5_name.c_str());
				if (JS_IsUndefined(obj5)) {
					obj5 = JS_NewObject(ctx);
					JS_SetPropertyStr(ctx, obj4, obj5_name.c_str(), obj5);
				}
				JS_SetPropertyStr(ctx, obj5, property_name.c_str(), JS_NewString(ctx, value.c_str()));
				JS_FreeValue(ctx, global_obj);
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

			inline auto add_constant(
				int32_t value,
				const std::string & obj1_name,
				const std::string & obj2_name,
				const std::string & obj3_name,
				const std::string & obj4_name,
				const std::string & obj5_name,
				const std::string & property_name
			) -> void 
			{
				auto global_obj = JS_GetGlobalObject(ctx);
				auto obj1 = JS_GetPropertyStr(ctx, global_obj, obj1_name.c_str());
				if (JS_IsUndefined(obj1)) {
					obj1 = JS_NewObject(ctx);
					JS_SetPropertyStr(ctx, global_obj, obj1_name.c_str(), obj1);
				}
				auto obj2 = JS_GetPropertyStr(ctx, obj1, obj2_name.c_str());
				if (JS_IsUndefined(obj2)) {
					obj2 = JS_NewObject(ctx);
					JS_SetPropertyStr(ctx, obj1, obj2_name.c_str(), obj2);
				}
				auto obj3 = JS_GetPropertyStr(ctx, obj2, obj3_name.c_str());
				if (JS_IsUndefined(obj3)) {
					obj3 = JS_NewObject(ctx);
					JS_SetPropertyStr(ctx, obj2, obj3_name.c_str(), obj3);
				}
				auto obj4 = JS_GetPropertyStr(ctx, obj3, obj4_name.c_str());
				if (JS_IsUndefined(obj4)) {
					obj4 = JS_NewObject(ctx);
					JS_SetPropertyStr(ctx, obj3, obj4_name.c_str(), obj4);
				}
				auto obj5 = JS_GetPropertyStr(ctx, obj4, obj5_name.c_str());
				if (JS_IsUndefined(obj5)) {
					obj5 = JS_NewObject(ctx);
					JS_SetPropertyStr(ctx, obj4, obj5_name.c_str(), obj5);
				}
				JS_SetPropertyStr(ctx, obj5, property_name.c_str(), JS_NewInt32(ctx, value));
				JS_FreeValue(ctx, global_obj);
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

			inline auto add_constant(
				uint32_t value,
				const std::string & obj1_name,
				const std::string & obj2_name,
				const std::string & obj3_name,
				const std::string & obj4_name,
				const std::string & obj5_name,
				const std::string & property_name
			) -> void 
			{
				auto global_obj = JS_GetGlobalObject(ctx);
				auto obj1 = JS_GetPropertyStr(ctx, global_obj, obj1_name.c_str());
				if (JS_IsUndefined(obj1)) {
					obj1 = JS_NewObject(ctx);
					JS_SetPropertyStr(ctx, global_obj, obj1_name.c_str(), obj1);
				}
				auto obj2 = JS_GetPropertyStr(ctx, obj1, obj2_name.c_str());
				if (JS_IsUndefined(obj2)) {
					obj2 = JS_NewObject(ctx);
					JS_SetPropertyStr(ctx, obj1, obj2_name.c_str(), obj2);
				}
				auto obj3 = JS_GetPropertyStr(ctx, obj2, obj3_name.c_str());
				if (JS_IsUndefined(obj3)) {
					obj3 = JS_NewObject(ctx);
					JS_SetPropertyStr(ctx, obj2, obj3_name.c_str(), obj3);
				}
				auto obj4 = JS_GetPropertyStr(ctx, obj3, obj4_name.c_str());
				if (JS_IsUndefined(obj4)) {
					obj4 = JS_NewObject(ctx);
					JS_SetPropertyStr(ctx, obj3, obj4_name.c_str(), obj4);
				}
				auto obj5 = JS_GetPropertyStr(ctx, obj4, obj5_name.c_str());
				if (JS_IsUndefined(obj5)) {
					obj5 = JS_NewObject(ctx);
					JS_SetPropertyStr(ctx, obj4, obj5_name.c_str(), obj5);
				}
				JS_SetPropertyStr(ctx, obj5, property_name.c_str(), JS_NewUint32(ctx, value));
				JS_FreeValue(ctx, global_obj);
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

			inline auto add_constant(
				int64_t value,
				const std::string & obj1_name,
				const std::string & obj2_name,
				const std::string & obj3_name,
				const std::string & obj4_name,
				const std::string & obj5_name,
				const std::string & property_name
			) -> void 
			{
				auto global_obj = JS_GetGlobalObject(ctx);
				auto obj1 = JS_GetPropertyStr(ctx, global_obj, obj1_name.c_str());
				if (JS_IsUndefined(obj1)) {
					obj1 = JS_NewObject(ctx);
					JS_SetPropertyStr(ctx, global_obj, obj1_name.c_str(), obj1);
				}
				auto obj2 = JS_GetPropertyStr(ctx, obj1, obj2_name.c_str());
				if (JS_IsUndefined(obj2)) {
					obj2 = JS_NewObject(ctx);
					JS_SetPropertyStr(ctx, obj1, obj2_name.c_str(), obj2);
				}
				auto obj3 = JS_GetPropertyStr(ctx, obj2, obj3_name.c_str());
				if (JS_IsUndefined(obj3)) {
					obj3 = JS_NewObject(ctx);
					JS_SetPropertyStr(ctx, obj2, obj3_name.c_str(), obj3);
				}
				auto obj4 = JS_GetPropertyStr(ctx, obj3, obj4_name.c_str());
				if (JS_IsUndefined(obj4)) {
					obj4 = JS_NewObject(ctx);
					JS_SetPropertyStr(ctx, obj3, obj4_name.c_str(), obj4);
				}
				auto obj5 = JS_GetPropertyStr(ctx, obj4, obj5_name.c_str());
				if (JS_IsUndefined(obj5)) {
					obj5 = JS_NewObject(ctx);
					JS_SetPropertyStr(ctx, obj4, obj5_name.c_str(), obj5);
				}
				JS_SetPropertyStr(ctx, obj5, property_name.c_str(), JS_NewInt64(ctx, value));
				JS_FreeValue(ctx, global_obj);
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

			inline auto add_constant(
				uint64_t value,
				const std::string & obj1_name,
				const std::string & obj2_name,
				const std::string & obj3_name,
				const std::string & obj4_name,
				const std::string & obj5_name,
				const std::string & property_name
			) -> void 
			{
				auto global_obj = JS_GetGlobalObject(ctx);
				auto obj1 = JS_GetPropertyStr(ctx, global_obj, obj1_name.c_str());
				if (JS_IsUndefined(obj1)) {
					obj1 = JS_NewObject(ctx);
					JS_SetPropertyStr(ctx, global_obj, obj1_name.c_str(), obj1);
				}
				auto obj2 = JS_GetPropertyStr(ctx, obj1, obj2_name.c_str());
				if (JS_IsUndefined(obj2)) {
					obj2 = JS_NewObject(ctx);
					JS_SetPropertyStr(ctx, obj1, obj2_name.c_str(), obj2);
				}
				auto obj3 = JS_GetPropertyStr(ctx, obj2, obj3_name.c_str());
				if (JS_IsUndefined(obj3)) {
					obj3 = JS_NewObject(ctx);
					JS_SetPropertyStr(ctx, obj2, obj3_name.c_str(), obj3);
				}
				auto obj4 = JS_GetPropertyStr(ctx, obj3, obj4_name.c_str());
				if (JS_IsUndefined(obj4)) {
					obj4 = JS_NewObject(ctx);
					JS_SetPropertyStr(ctx, obj3, obj4_name.c_str(), obj4);
				}
				auto obj5 = JS_GetPropertyStr(ctx, obj4, obj5_name.c_str());
				if (JS_IsUndefined(obj5)) {
					obj5 = JS_NewObject(ctx);
					JS_SetPropertyStr(ctx, obj4, obj5_name.c_str(), obj5);
				}
				JS_SetPropertyStr(ctx, obj5, property_name.c_str(), JS_NewBigUint64(ctx, value));
				JS_FreeValue(ctx, global_obj);
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

			inline auto add_constant(
				double value,
				const std::string & obj1_name,
				const std::string & obj2_name,
				const std::string & obj3_name,
				const std::string & obj4_name,
				const std::string & obj5_name,
				const std::string & property_name
			) -> void 
			{
				auto global_obj = JS_GetGlobalObject(ctx);
				auto obj1 = JS_GetPropertyStr(ctx, global_obj, obj1_name.c_str());
				if (JS_IsUndefined(obj1)) {
					obj1 = JS_NewObject(ctx);
					JS_SetPropertyStr(ctx, global_obj, obj1_name.c_str(), obj1);
				}
				auto obj2 = JS_GetPropertyStr(ctx, obj1, obj2_name.c_str());
				if (JS_IsUndefined(obj2)) {
					obj2 = JS_NewObject(ctx);
					JS_SetPropertyStr(ctx, obj1, obj2_name.c_str(), obj2);
				}
				auto obj3 = JS_GetPropertyStr(ctx, obj2, obj3_name.c_str());
				if (JS_IsUndefined(obj3)) {
					obj3 = JS_NewObject(ctx);
					JS_SetPropertyStr(ctx, obj2, obj3_name.c_str(), obj3);
				}
				auto obj4 = JS_GetPropertyStr(ctx, obj3, obj4_name.c_str());
				if (JS_IsUndefined(obj4)) {
					obj4 = JS_NewObject(ctx);
					JS_SetPropertyStr(ctx, obj3, obj4_name.c_str(), obj4);
				}
				auto obj5 = JS_GetPropertyStr(ctx, obj4, obj5_name.c_str());
				if (JS_IsUndefined(obj5)) {
					obj5 = JS_NewObject(ctx);
					JS_SetPropertyStr(ctx, obj4, obj5_name.c_str(), obj5);
				}
				JS_SetPropertyStr(ctx, obj5, property_name.c_str(), JS_NewFloat64(ctx, value));
				JS_FreeValue(ctx, global_obj);
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

			inline auto add_constant(
				float value,
				const std::string & obj1_name,
				const std::string & obj2_name,
				const std::string & obj3_name,
				const std::string & obj4_name,
				const std::string & obj5_name,
				const std::string & property_name
			) -> void 
			{
				auto global_obj = JS_GetGlobalObject(ctx);
				auto obj1 = JS_GetPropertyStr(ctx, global_obj, obj1_name.c_str());
				if (JS_IsUndefined(obj1)) {
					obj1 = JS_NewObject(ctx);
					JS_SetPropertyStr(ctx, global_obj, obj1_name.c_str(), obj1);
				}
				auto obj2 = JS_GetPropertyStr(ctx, obj1, obj2_name.c_str());
				if (JS_IsUndefined(obj2)) {
					obj2 = JS_NewObject(ctx);
					JS_SetPropertyStr(ctx, obj1, obj2_name.c_str(), obj2);
				}
				auto obj3 = JS_GetPropertyStr(ctx, obj2, obj3_name.c_str());
				if (JS_IsUndefined(obj3)) {
					obj3 = JS_NewObject(ctx);
					JS_SetPropertyStr(ctx, obj2, obj3_name.c_str(), obj3);
				}
				auto obj4 = JS_GetPropertyStr(ctx, obj3, obj4_name.c_str());
				if (JS_IsUndefined(obj4)) {
					obj4 = JS_NewObject(ctx);
					JS_SetPropertyStr(ctx, obj3, obj4_name.c_str(), obj4);
				}
				auto obj5 = JS_GetPropertyStr(ctx, obj4, obj5_name.c_str());
				if (JS_IsUndefined(obj5)) {
					obj5 = JS_NewObject(ctx);
					JS_SetPropertyStr(ctx, obj4, obj5_name.c_str(), obj5);
				}
				JS_SetPropertyStr(ctx, obj5, property_name.c_str(), JS_NewFloat64(ctx, static_cast<double>(value)));
				JS_FreeValue(ctx, global_obj);
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

			inline auto add_constant(
				bool value,
				const std::string & obj1_name,
				const std::string & obj2_name,
				const std::string & obj3_name,
				const std::string & obj4_name,
				const std::string & obj5_name,
				const std::string & property_name
			) -> void 
			{
				auto global_obj = JS_GetGlobalObject(ctx);
				auto obj1 = JS_GetPropertyStr(ctx, global_obj, obj1_name.c_str());
				if (JS_IsUndefined(obj1)) {
					obj1 = JS_NewObject(ctx);
					JS_SetPropertyStr(ctx, global_obj, obj1_name.c_str(), obj1);
				}
				auto obj2 = JS_GetPropertyStr(ctx, obj1, obj2_name.c_str());
				if (JS_IsUndefined(obj2)) {
					obj2 = JS_NewObject(ctx);
					JS_SetPropertyStr(ctx, obj1, obj2_name.c_str(), obj2);
				}
				auto obj3 = JS_GetPropertyStr(ctx, obj2, obj3_name.c_str());
				if (JS_IsUndefined(obj3)) {
					obj3 = JS_NewObject(ctx);
					JS_SetPropertyStr(ctx, obj2, obj3_name.c_str(), obj3);
				}
				auto obj4 = JS_GetPropertyStr(ctx, obj3, obj4_name.c_str());
				if (JS_IsUndefined(obj4)) {
					obj4 = JS_NewObject(ctx);
					JS_SetPropertyStr(ctx, obj3, obj4_name.c_str(), obj4);
				}
				auto obj5 = JS_GetPropertyStr(ctx, obj4, obj5_name.c_str());
				if (JS_IsUndefined(obj5)) {
					obj5 = JS_NewObject(ctx);
					JS_SetPropertyStr(ctx, obj4, obj5_name.c_str(), obj5);
				}
				JS_SetPropertyStr(ctx, obj5, property_name.c_str(), JS_NewBool(ctx, value ? 1 : 0));
				JS_FreeValue(ctx, global_obj);
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

			inline auto add_constant(
				const std::string & value,
				const std::string & obj1_name,
				const std::string & obj2_name,
				const std::string & obj3_name,
				const std::string & obj4_name,
				const std::string & obj5_name,
				const std::string & obj6_name,
				const std::string & property_name
			) -> void 
			{
				auto global_obj = JS_GetGlobalObject(ctx);
				auto obj1 = JS_GetPropertyStr(ctx, global_obj, obj1_name.c_str());
				if (JS_IsUndefined(obj1)) {
					obj1 = JS_NewObject(ctx);
					JS_SetPropertyStr(ctx, global_obj, obj1_name.c_str(), obj1);
				}
				auto obj2 = JS_GetPropertyStr(ctx, obj1, obj2_name.c_str());
				if (JS_IsUndefined(obj2)) {
					obj2 = JS_NewObject(ctx);
					JS_SetPropertyStr(ctx, obj1, obj2_name.c_str(), obj2);
				}
				auto obj3 = JS_GetPropertyStr(ctx, obj2, obj3_name.c_str());
				if (JS_IsUndefined(obj3)) {
					obj3 = JS_NewObject(ctx);
					JS_SetPropertyStr(ctx, obj2, obj3_name.c_str(), obj3);
				}
				auto obj4 = JS_GetPropertyStr(ctx, obj3, obj4_name.c_str());
				if (JS_IsUndefined(obj4)) {
					obj4 = JS_NewObject(ctx);
					JS_SetPropertyStr(ctx, obj3, obj4_name.c_str(), obj4);
				}
				auto obj5 = JS_GetPropertyStr(ctx, obj4, obj5_name.c_str());
				if (JS_IsUndefined(obj5)) {
					obj5 = JS_NewObject(ctx);
					JS_SetPropertyStr(ctx, obj4, obj5_name.c_str(), obj5);
				}
				auto obj6 = JS_GetPropertyStr(ctx, obj5, obj6_name.c_str());
				if (JS_IsUndefined(obj6)) {
					obj6 = JS_NewObject(ctx);
					JS_SetPropertyStr(ctx, obj5, obj6_name.c_str(), obj6);
				}
				JS_SetPropertyStr(ctx, obj6, property_name.c_str(), JS_NewString(ctx, value.c_str()));
				JS_FreeValue(ctx, global_obj);
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

			inline auto add_constant(
				int32_t value,
				const std::string & obj1_name,
				const std::string & obj2_name,
				const std::string & obj3_name,
				const std::string & obj4_name,
				const std::string & obj5_name,
				const std::string & obj6_name,
				const std::string & property_name
			) -> void 
			{
				auto global_obj = JS_GetGlobalObject(ctx);
				auto obj1 = JS_GetPropertyStr(ctx, global_obj, obj1_name.c_str());
				if (JS_IsUndefined(obj1)) {
					obj1 = JS_NewObject(ctx);
					JS_SetPropertyStr(ctx, global_obj, obj1_name.c_str(), obj1);
				}
				auto obj2 = JS_GetPropertyStr(ctx, obj1, obj2_name.c_str());
				if (JS_IsUndefined(obj2)) {
					obj2 = JS_NewObject(ctx);
					JS_SetPropertyStr(ctx, obj1, obj2_name.c_str(), obj2);
				}
				auto obj3 = JS_GetPropertyStr(ctx, obj2, obj3_name.c_str());
				if (JS_IsUndefined(obj3)) {
					obj3 = JS_NewObject(ctx);
					JS_SetPropertyStr(ctx, obj2, obj3_name.c_str(), obj3);
				}
				auto obj4 = JS_GetPropertyStr(ctx, obj3, obj4_name.c_str());
				if (JS_IsUndefined(obj4)) {
					obj4 = JS_NewObject(ctx);
					JS_SetPropertyStr(ctx, obj3, obj4_name.c_str(), obj4);
				}
				auto obj5 = JS_GetPropertyStr(ctx, obj4, obj5_name.c_str());
				if (JS_IsUndefined(obj5)) {
					obj5 = JS_NewObject(ctx);
					JS_SetPropertyStr(ctx, obj4, obj5_name.c_str(), obj5);
				}
				auto obj6 = JS_GetPropertyStr(ctx, obj5, obj6_name.c_str());
				if (JS_IsUndefined(obj6)) {
					obj6 = JS_NewObject(ctx);
					JS_SetPropertyStr(ctx, obj5, obj6_name.c_str(), obj6);
				}
				JS_SetPropertyStr(ctx, obj6, property_name.c_str(), JS_NewInt32(ctx, value));
				JS_FreeValue(ctx, global_obj);
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

			inline auto add_constant(
				uint32_t value,
				const std::string & obj1_name,
				const std::string & obj2_name,
				const std::string & obj3_name,
				const std::string & obj4_name,
				const std::string & obj5_name,
				const std::string & obj6_name,
				const std::string & property_name
			) -> void 
			{
				auto global_obj = JS_GetGlobalObject(ctx);
				auto obj1 = JS_GetPropertyStr(ctx, global_obj, obj1_name.c_str());
				if (JS_IsUndefined(obj1)) {
					obj1 = JS_NewObject(ctx);
					JS_SetPropertyStr(ctx, global_obj, obj1_name.c_str(), obj1);
				}
				auto obj2 = JS_GetPropertyStr(ctx, obj1, obj2_name.c_str());
				if (JS_IsUndefined(obj2)) {
					obj2 = JS_NewObject(ctx);
					JS_SetPropertyStr(ctx, obj1, obj2_name.c_str(), obj2);
				}
				auto obj3 = JS_GetPropertyStr(ctx, obj2, obj3_name.c_str());
				if (JS_IsUndefined(obj3)) {
					obj3 = JS_NewObject(ctx);
					JS_SetPropertyStr(ctx, obj2, obj3_name.c_str(), obj3);
				}
				auto obj4 = JS_GetPropertyStr(ctx, obj3, obj4_name.c_str());
				if (JS_IsUndefined(obj4)) {
					obj4 = JS_NewObject(ctx);
					JS_SetPropertyStr(ctx, obj3, obj4_name.c_str(), obj4);
				}
				auto obj5 = JS_GetPropertyStr(ctx, obj4, obj5_name.c_str());
				if (JS_IsUndefined(obj5)) {
					obj5 = JS_NewObject(ctx);
					JS_SetPropertyStr(ctx, obj4, obj5_name.c_str(), obj5);
				}
				auto obj6 = JS_GetPropertyStr(ctx, obj5, obj6_name.c_str());
				if (JS_IsUndefined(obj6)) {
					obj6 = JS_NewObject(ctx);
					JS_SetPropertyStr(ctx, obj5, obj6_name.c_str(), obj6);
				}
				JS_SetPropertyStr(ctx, obj6, property_name.c_str(), JS_NewUint32(ctx, value));
				JS_FreeValue(ctx, global_obj);
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

			inline auto add_constant(
				int64_t value,
				const std::string & obj1_name,
				const std::string & obj2_name,
				const std::string & obj3_name,
				const std::string & obj4_name,
				const std::string & obj5_name,
				const std::string & obj6_name,
				const std::string & property_name
			) -> void 
			{
				auto global_obj = JS_GetGlobalObject(ctx);
				auto obj1 = JS_GetPropertyStr(ctx, global_obj, obj1_name.c_str());
				if (JS_IsUndefined(obj1)) {
					obj1 = JS_NewObject(ctx);
					JS_SetPropertyStr(ctx, global_obj, obj1_name.c_str(), obj1);
				}
				auto obj2 = JS_GetPropertyStr(ctx, obj1, obj2_name.c_str());
				if (JS_IsUndefined(obj2)) {
					obj2 = JS_NewObject(ctx);
					JS_SetPropertyStr(ctx, obj1, obj2_name.c_str(), obj2);
				}
				auto obj3 = JS_GetPropertyStr(ctx, obj2, obj3_name.c_str());
				if (JS_IsUndefined(obj3)) {
					obj3 = JS_NewObject(ctx);
					JS_SetPropertyStr(ctx, obj2, obj3_name.c_str(), obj3);
				}
				auto obj4 = JS_GetPropertyStr(ctx, obj3, obj4_name.c_str());
				if (JS_IsUndefined(obj4)) {
					obj4 = JS_NewObject(ctx);
					JS_SetPropertyStr(ctx, obj3, obj4_name.c_str(), obj4);
				}
				auto obj5 = JS_GetPropertyStr(ctx, obj4, obj5_name.c_str());
				if (JS_IsUndefined(obj5)) {
					obj5 = JS_NewObject(ctx);
					JS_SetPropertyStr(ctx, obj4, obj5_name.c_str(), obj5);
				}
				auto obj6 = JS_GetPropertyStr(ctx, obj5, obj6_name.c_str());
				if (JS_IsUndefined(obj6)) {
					obj6 = JS_NewObject(ctx);
					JS_SetPropertyStr(ctx, obj5, obj6_name.c_str(), obj6);
				}
				JS_SetPropertyStr(ctx, obj6, property_name.c_str(), JS_NewInt64(ctx, value));
				JS_FreeValue(ctx, global_obj);
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

			inline auto add_constant(
				uint64_t value,
				const std::string & obj1_name,
				const std::string & obj2_name,
				const std::string & obj3_name,
				const std::string & obj4_name,
				const std::string & obj5_name,
				const std::string & obj6_name,
				const std::string & property_name
			) -> void 
			{
				auto global_obj = JS_GetGlobalObject(ctx);
				auto obj1 = JS_GetPropertyStr(ctx, global_obj, obj1_name.c_str());
				if (JS_IsUndefined(obj1)) {
					obj1 = JS_NewObject(ctx);
					JS_SetPropertyStr(ctx, global_obj, obj1_name.c_str(), obj1);
				}
				auto obj2 = JS_GetPropertyStr(ctx, obj1, obj2_name.c_str());
				if (JS_IsUndefined(obj2)) {
					obj2 = JS_NewObject(ctx);
					JS_SetPropertyStr(ctx, obj1, obj2_name.c_str(), obj2);
				}
				auto obj3 = JS_GetPropertyStr(ctx, obj2, obj3_name.c_str());
				if (JS_IsUndefined(obj3)) {
					obj3 = JS_NewObject(ctx);
					JS_SetPropertyStr(ctx, obj2, obj3_name.c_str(), obj3);
				}
				auto obj4 = JS_GetPropertyStr(ctx, obj3, obj4_name.c_str());
				if (JS_IsUndefined(obj4)) {
					obj4 = JS_NewObject(ctx);
					JS_SetPropertyStr(ctx, obj3, obj4_name.c_str(), obj4);
				}
				auto obj5 = JS_GetPropertyStr(ctx, obj4, obj5_name.c_str());
				if (JS_IsUndefined(obj5)) {
					obj5 = JS_NewObject(ctx);
					JS_SetPropertyStr(ctx, obj4, obj5_name.c_str(), obj5);
				}
				auto obj6 = JS_GetPropertyStr(ctx, obj5, obj6_name.c_str());
				if (JS_IsUndefined(obj6)) {
					obj6 = JS_NewObject(ctx);
					JS_SetPropertyStr(ctx, obj5, obj6_name.c_str(), obj6);
				}
				JS_SetPropertyStr(ctx, obj6, property_name.c_str(), JS_NewBigUint64(ctx, value));
				JS_FreeValue(ctx, global_obj);
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

			inline auto add_constant(
				double value,
				const std::string & obj1_name,
				const std::string & obj2_name,
				const std::string & obj3_name,
				const std::string & obj4_name,
				const std::string & obj5_name,
				const std::string & obj6_name,
				const std::string & property_name
			) -> void 
			{
				auto global_obj = JS_GetGlobalObject(ctx);
				auto obj1 = JS_GetPropertyStr(ctx, global_obj, obj1_name.c_str());
				if (JS_IsUndefined(obj1)) {
					obj1 = JS_NewObject(ctx);
					JS_SetPropertyStr(ctx, global_obj, obj1_name.c_str(), obj1);
				}
				auto obj2 = JS_GetPropertyStr(ctx, obj1, obj2_name.c_str());
				if (JS_IsUndefined(obj2)) {
					obj2 = JS_NewObject(ctx);
					JS_SetPropertyStr(ctx, obj1, obj2_name.c_str(), obj2);
				}
				auto obj3 = JS_GetPropertyStr(ctx, obj2, obj3_name.c_str());
				if (JS_IsUndefined(obj3)) {
					obj3 = JS_NewObject(ctx);
					JS_SetPropertyStr(ctx, obj2, obj3_name.c_str(), obj3);
				}
				auto obj4 = JS_GetPropertyStr(ctx, obj3, obj4_name.c_str());
				if (JS_IsUndefined(obj4)) {
					obj4 = JS_NewObject(ctx);
					JS_SetPropertyStr(ctx, obj3, obj4_name.c_str(), obj4);
				}
				auto obj5 = JS_GetPropertyStr(ctx, obj4, obj5_name.c_str());
				if (JS_IsUndefined(obj5)) {
					obj5 = JS_NewObject(ctx);
					JS_SetPropertyStr(ctx, obj4, obj5_name.c_str(), obj5);
				}
				auto obj6 = JS_GetPropertyStr(ctx, obj5, obj6_name.c_str());
				if (JS_IsUndefined(obj6)) {
					obj6 = JS_NewObject(ctx);
					JS_SetPropertyStr(ctx, obj5, obj6_name.c_str(), obj6);
				}
				JS_SetPropertyStr(ctx, obj6, property_name.c_str(), JS_NewFloat64(ctx, value));
				JS_FreeValue(ctx, global_obj);
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

			inline auto add_constant(
				float value,
				const std::string & obj1_name,
				const std::string & obj2_name,
				const std::string & obj3_name,
				const std::string & obj4_name,
				const std::string & obj5_name,
				const std::string & obj6_name,
				const std::string & property_name
			) -> void 
			{
				auto global_obj = JS_GetGlobalObject(ctx);
				auto obj1 = JS_GetPropertyStr(ctx, global_obj, obj1_name.c_str());
				if (JS_IsUndefined(obj1)) {
					obj1 = JS_NewObject(ctx);
					JS_SetPropertyStr(ctx, global_obj, obj1_name.c_str(), obj1);
				}
				auto obj2 = JS_GetPropertyStr(ctx, obj1, obj2_name.c_str());
				if (JS_IsUndefined(obj2)) {
					obj2 = JS_NewObject(ctx);
					JS_SetPropertyStr(ctx, obj1, obj2_name.c_str(), obj2);
				}
				auto obj3 = JS_GetPropertyStr(ctx, obj2, obj3_name.c_str());
				if (JS_IsUndefined(obj3)) {
					obj3 = JS_NewObject(ctx);
					JS_SetPropertyStr(ctx, obj2, obj3_name.c_str(), obj3);
				}
				auto obj4 = JS_GetPropertyStr(ctx, obj3, obj4_name.c_str());
				if (JS_IsUndefined(obj4)) {
					obj4 = JS_NewObject(ctx);
					JS_SetPropertyStr(ctx, obj3, obj4_name.c_str(), obj4);
				}
				auto obj5 = JS_GetPropertyStr(ctx, obj4, obj5_name.c_str());
				if (JS_IsUndefined(obj5)) {
					obj5 = JS_NewObject(ctx);
					JS_SetPropertyStr(ctx, obj4, obj5_name.c_str(), obj5);
				}
				auto obj6 = JS_GetPropertyStr(ctx, obj5, obj6_name.c_str());
				if (JS_IsUndefined(obj6)) {
					obj6 = JS_NewObject(ctx);
					JS_SetPropertyStr(ctx, obj5, obj6_name.c_str(), obj6);
				}
				JS_SetPropertyStr(ctx, obj6, property_name.c_str(), JS_NewFloat64(ctx, static_cast<double>(value)));
				JS_FreeValue(ctx, global_obj);
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

			inline auto add_constant(
				bool value,
				const std::string & obj1_name,
				const std::string & obj2_name,
				const std::string & obj3_name,
				const std::string & obj4_name,
				const std::string & obj5_name,
				const std::string & obj6_name,
				const std::string & property_name
			) -> void 
			{
				auto global_obj = JS_GetGlobalObject(ctx);
				auto obj1 = JS_GetPropertyStr(ctx, global_obj, obj1_name.c_str());
				if (JS_IsUndefined(obj1)) {
					obj1 = JS_NewObject(ctx);
					JS_SetPropertyStr(ctx, global_obj, obj1_name.c_str(), obj1);
				}
				auto obj2 = JS_GetPropertyStr(ctx, obj1, obj2_name.c_str());
				if (JS_IsUndefined(obj2)) {
					obj2 = JS_NewObject(ctx);
					JS_SetPropertyStr(ctx, obj1, obj2_name.c_str(), obj2);
				}
				auto obj3 = JS_GetPropertyStr(ctx, obj2, obj3_name.c_str());
				if (JS_IsUndefined(obj3)) {
					obj3 = JS_NewObject(ctx);
					JS_SetPropertyStr(ctx, obj2, obj3_name.c_str(), obj3);
				}
				auto obj4 = JS_GetPropertyStr(ctx, obj3, obj4_name.c_str());
				if (JS_IsUndefined(obj4)) {
					obj4 = JS_NewObject(ctx);
					JS_SetPropertyStr(ctx, obj3, obj4_name.c_str(), obj4);
				}
				auto obj5 = JS_GetPropertyStr(ctx, obj4, obj5_name.c_str());
				if (JS_IsUndefined(obj5)) {
					obj5 = JS_NewObject(ctx);
					JS_SetPropertyStr(ctx, obj4, obj5_name.c_str(), obj5);
				}
				auto obj6 = JS_GetPropertyStr(ctx, obj5, obj6_name.c_str());
				if (JS_IsUndefined(obj6)) {
					obj6 = JS_NewObject(ctx);
					JS_SetPropertyStr(ctx, obj5, obj6_name.c_str(), obj6);
				}
				JS_SetPropertyStr(ctx, obj6, property_name.c_str(), JS_NewBool(ctx, value ? 1 : 0));
				JS_FreeValue(ctx, global_obj);
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

			inline auto add_constant(
				const std::string & value,
				const std::string & obj1_name,
				const std::string & obj2_name,
				const std::string & obj3_name,
				const std::string & obj4_name,
				const std::string & obj5_name,
				const std::string & obj6_name,
				const std::string & obj7_name,
				const std::string & property_name
			) -> void 
			{
				auto global_obj = JS_GetGlobalObject(ctx);
				auto obj1 = JS_GetPropertyStr(ctx, global_obj, obj1_name.c_str());
				if (JS_IsUndefined(obj1)) {
					obj1 = JS_NewObject(ctx);
					JS_SetPropertyStr(ctx, global_obj, obj1_name.c_str(), obj1);
				}
				auto obj2 = JS_GetPropertyStr(ctx, obj1, obj2_name.c_str());
				if (JS_IsUndefined(obj2)) {
					obj2 = JS_NewObject(ctx);
					JS_SetPropertyStr(ctx, obj1, obj2_name.c_str(), obj2);
				}
				auto obj3 = JS_GetPropertyStr(ctx, obj2, obj3_name.c_str());
				if (JS_IsUndefined(obj3)) {
					obj3 = JS_NewObject(ctx);
					JS_SetPropertyStr(ctx, obj2, obj3_name.c_str(), obj3);
				}
				auto obj4 = JS_GetPropertyStr(ctx, obj3, obj4_name.c_str());
				if (JS_IsUndefined(obj4)) {
					obj4 = JS_NewObject(ctx);
					JS_SetPropertyStr(ctx, obj3, obj4_name.c_str(), obj4);
				}
				auto obj5 = JS_GetPropertyStr(ctx, obj4, obj5_name.c_str());
				if (JS_IsUndefined(obj5)) {
					obj5 = JS_NewObject(ctx);
					JS_SetPropertyStr(ctx, obj4, obj5_name.c_str(), obj5);
				}
				auto obj6 = JS_GetPropertyStr(ctx, obj5, obj6_name.c_str());
				if (JS_IsUndefined(obj6)) {
					obj6 = JS_NewObject(ctx);
					JS_SetPropertyStr(ctx, obj5, obj6_name.c_str(), obj6);
				}
				auto obj7 = JS_GetPropertyStr(ctx, obj6, obj7_name.c_str());
				if (JS_IsUndefined(obj7)) {
					obj7 = JS_NewObject(ctx);
					JS_SetPropertyStr(ctx, obj6, obj7_name.c_str(), obj7);
				}
				JS_SetPropertyStr(ctx, obj7, property_name.c_str(), JS_NewString(ctx, value.c_str()));
				JS_FreeValue(ctx, global_obj);
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

			inline auto add_constant(
				int32_t value,
				const std::string & obj1_name,
				const std::string & obj2_name,
				const std::string & obj3_name,
				const std::string & obj4_name,
				const std::string & obj5_name,
				const std::string & obj6_name,
				const std::string & obj7_name,
				const std::string & property_name
			) -> void 
			{
				auto global_obj = JS_GetGlobalObject(ctx);
				auto obj1 = JS_GetPropertyStr(ctx, global_obj, obj1_name.c_str());
				if (JS_IsUndefined(obj1)) {
					obj1 = JS_NewObject(ctx);
					JS_SetPropertyStr(ctx, global_obj, obj1_name.c_str(), obj1);
				}
				auto obj2 = JS_GetPropertyStr(ctx, obj1, obj2_name.c_str());
				if (JS_IsUndefined(obj2)) {
					obj2 = JS_NewObject(ctx);
					JS_SetPropertyStr(ctx, obj1, obj2_name.c_str(), obj2);
				}
				auto obj3 = JS_GetPropertyStr(ctx, obj2, obj3_name.c_str());
				if (JS_IsUndefined(obj3)) {
					obj3 = JS_NewObject(ctx);
					JS_SetPropertyStr(ctx, obj2, obj3_name.c_str(), obj3);
				}
				auto obj4 = JS_GetPropertyStr(ctx, obj3, obj4_name.c_str());
				if (JS_IsUndefined(obj4)) {
					obj4 = JS_NewObject(ctx);
					JS_SetPropertyStr(ctx, obj3, obj4_name.c_str(), obj4);
				}
				auto obj5 = JS_GetPropertyStr(ctx, obj4, obj5_name.c_str());
				if (JS_IsUndefined(obj5)) {
					obj5 = JS_NewObject(ctx);
					JS_SetPropertyStr(ctx, obj4, obj5_name.c_str(), obj5);
				}
				auto obj6 = JS_GetPropertyStr(ctx, obj5, obj6_name.c_str());
				if (JS_IsUndefined(obj6)) {
					obj6 = JS_NewObject(ctx);
					JS_SetPropertyStr(ctx, obj5, obj6_name.c_str(), obj6);
				}
				auto obj7 = JS_GetPropertyStr(ctx, obj6, obj7_name.c_str());
				if (JS_IsUndefined(obj7)) {
					obj7 = JS_NewObject(ctx);
					JS_SetPropertyStr(ctx, obj6, obj7_name.c_str(), obj7);
				}
				JS_SetPropertyStr(ctx, obj7, property_name.c_str(), JS_NewInt32(ctx, value));
				JS_FreeValue(ctx, global_obj);
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

			inline auto add_constant(
				int32_t value,
				const std::string & obj1_name,
				const std::string & obj2_name,
				const std::string & obj3_name,
				const std::string & obj4_name,
				const std::string & obj5_name,
				const std::string & obj6_name,
				const std::string & obj7_name,
				const std::string & obj8_name,
				const std::string & obj9_name,
				const std::string & property_name
			) -> void 
			{
				auto global_obj = JS_GetGlobalObject(ctx);
				auto obj1 = JS_GetPropertyStr(ctx, global_obj, obj1_name.c_str());
				if (JS_IsUndefined(obj1)) {
					obj1 = JS_NewObject(ctx);
					JS_SetPropertyStr(ctx, global_obj, obj1_name.c_str(), obj1);
				}
				auto obj2 = JS_GetPropertyStr(ctx, obj1, obj2_name.c_str());
				if (JS_IsUndefined(obj2)) {
					obj2 = JS_NewObject(ctx);
					JS_SetPropertyStr(ctx, obj1, obj2_name.c_str(), obj2);
				}
				auto obj3 = JS_GetPropertyStr(ctx, obj2, obj3_name.c_str());
				if (JS_IsUndefined(obj3)) {
					obj3 = JS_NewObject(ctx);
					JS_SetPropertyStr(ctx, obj2, obj3_name.c_str(), obj3);
				}
				auto obj4 = JS_GetPropertyStr(ctx, obj3, obj4_name.c_str());
				if (JS_IsUndefined(obj4)) {
					obj4 = JS_NewObject(ctx);
					JS_SetPropertyStr(ctx, obj3, obj4_name.c_str(), obj4);
				}
				auto obj5 = JS_GetPropertyStr(ctx, obj4, obj5_name.c_str());
				if (JS_IsUndefined(obj5)) {
					obj5 = JS_NewObject(ctx);
					JS_SetPropertyStr(ctx, obj4, obj5_name.c_str(), obj5);
				}
				auto obj6 = JS_GetPropertyStr(ctx, obj5, obj6_name.c_str());
				if (JS_IsUndefined(obj6)) {
					obj6 = JS_NewObject(ctx);
					JS_SetPropertyStr(ctx, obj5, obj6_name.c_str(), obj6);
				}
				auto obj7 = JS_GetPropertyStr(ctx, obj6, obj7_name.c_str());
				if (JS_IsUndefined(obj7)) {
					obj7 = JS_NewObject(ctx);
					JS_SetPropertyStr(ctx, obj6, obj7_name.c_str(), obj7);
				}
				auto obj8 = JS_GetPropertyStr(ctx, obj7, obj8_name.c_str());
				if (JS_IsUndefined(obj8)) {
					obj8 = JS_NewObject(ctx);
					JS_SetPropertyStr(ctx, obj7, obj8_name.c_str(), obj8);
				}
				auto obj9 = JS_GetPropertyStr(ctx, obj8, obj9_name.c_str());
				if (JS_IsUndefined(obj9)) {
					obj9 = JS_NewObject(ctx);
					JS_SetPropertyStr(ctx, obj8, obj9_name.c_str(), obj9);
				}
				JS_SetPropertyStr(ctx, obj8, property_name.c_str(), JS_NewInt32(ctx, value));
				JS_FreeValue(ctx, global_obj);
				return;
			}

			/**
			 * --------------------------------------
			 * Add a constant JS value to an object from C
			 * @param value: the C String value
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

			inline auto add_constant(
				const std::string & value,
				const std::string & obj1_name,
				const std::string & obj2_name,
				const std::string & obj3_name,
				const std::string & obj4_name,
				const std::string & obj5_name,
				const std::string & obj6_name,
				const std::string & obj7_name,
				const std::string & obj8_name,
				const std::string & obj9_name,
				const std::string & property_name
			) -> void 
			{
				auto global_obj = JS_GetGlobalObject(ctx);
				auto obj1 = JS_GetPropertyStr(ctx, global_obj, obj1_name.c_str());
				if (JS_IsUndefined(obj1)) {
					obj1 = JS_NewObject(ctx);
					JS_SetPropertyStr(ctx, global_obj, obj1_name.c_str(), obj1);
				}
				auto obj2 = JS_GetPropertyStr(ctx, obj1, obj2_name.c_str());
				if (JS_IsUndefined(obj2)) {
					obj2 = JS_NewObject(ctx);
					JS_SetPropertyStr(ctx, obj1, obj2_name.c_str(), obj2);
				}
				auto obj3 = JS_GetPropertyStr(ctx, obj2, obj3_name.c_str());
				if (JS_IsUndefined(obj3)) {
					obj3 = JS_NewObject(ctx);
					JS_SetPropertyStr(ctx, obj2, obj3_name.c_str(), obj3);
				}
				auto obj4 = JS_GetPropertyStr(ctx, obj3, obj4_name.c_str());
				if (JS_IsUndefined(obj4)) {
					obj4 = JS_NewObject(ctx);
					JS_SetPropertyStr(ctx, obj3, obj4_name.c_str(), obj4);
				}
				auto obj5 = JS_GetPropertyStr(ctx, obj4, obj5_name.c_str());
				if (JS_IsUndefined(obj5)) {
					obj5 = JS_NewObject(ctx);
					JS_SetPropertyStr(ctx, obj4, obj5_name.c_str(), obj5);
				}
				auto obj6 = JS_GetPropertyStr(ctx, obj5, obj6_name.c_str());
				if (JS_IsUndefined(obj6)) {
					obj6 = JS_NewObject(ctx);
					JS_SetPropertyStr(ctx, obj5, obj6_name.c_str(), obj6);
				}
				auto obj7 = JS_GetPropertyStr(ctx, obj6, obj7_name.c_str());
				if (JS_IsUndefined(obj7)) {
					obj7 = JS_NewObject(ctx);
					JS_SetPropertyStr(ctx, obj6, obj7_name.c_str(), obj7);
				}
				auto obj8 = JS_GetPropertyStr(ctx, obj7, obj8_name.c_str());
				if (JS_IsUndefined(obj8)) {
					obj8 = JS_NewObject(ctx);
					JS_SetPropertyStr(ctx, obj7, obj8_name.c_str(), obj8);
				}
				auto obj9 = JS_GetPropertyStr(ctx, obj8, obj9_name.c_str());
				if (JS_IsUndefined(obj9)) {
					obj9 = JS_NewObject(ctx);
					JS_SetPropertyStr(ctx, obj8, obj9_name.c_str(), obj9);
				}
				JS_SetPropertyStr(ctx, obj8, property_name.c_str(), JS_NewString(ctx, value.c_str()));
				JS_FreeValue(ctx, global_obj);
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

			inline auto add_constant(
				uint32_t value,
				const std::string & obj1_name,
				const std::string & obj2_name,
				const std::string & obj3_name,
				const std::string & obj4_name,
				const std::string & obj5_name,
				const std::string & obj6_name,
				const std::string & obj7_name,
				const std::string & property_name
			) -> void 
			{
				auto global_obj = JS_GetGlobalObject(ctx);
				auto obj1 = JS_GetPropertyStr(ctx, global_obj, obj1_name.c_str());
				if (JS_IsUndefined(obj1)) {
					obj1 = JS_NewObject(ctx);
					JS_SetPropertyStr(ctx, global_obj, obj1_name.c_str(), obj1);
				}
				auto obj2 = JS_GetPropertyStr(ctx, obj1, obj2_name.c_str());
				if (JS_IsUndefined(obj2)) {
					obj2 = JS_NewObject(ctx);
					JS_SetPropertyStr(ctx, obj1, obj2_name.c_str(), obj2);
				}
				auto obj3 = JS_GetPropertyStr(ctx, obj2, obj3_name.c_str());
				if (JS_IsUndefined(obj3)) {
					obj3 = JS_NewObject(ctx);
					JS_SetPropertyStr(ctx, obj2, obj3_name.c_str(), obj3);
				}
				auto obj4 = JS_GetPropertyStr(ctx, obj3, obj4_name.c_str());
				if (JS_IsUndefined(obj4)) {
					obj4 = JS_NewObject(ctx);
					JS_SetPropertyStr(ctx, obj3, obj4_name.c_str(), obj4);
				}
				auto obj5 = JS_GetPropertyStr(ctx, obj4, obj5_name.c_str());
				if (JS_IsUndefined(obj5)) {
					obj5 = JS_NewObject(ctx);
					JS_SetPropertyStr(ctx, obj4, obj5_name.c_str(), obj5);
				}
				auto obj6 = JS_GetPropertyStr(ctx, obj5, obj6_name.c_str());
				if (JS_IsUndefined(obj6)) {
					obj6 = JS_NewObject(ctx);
					JS_SetPropertyStr(ctx, obj5, obj6_name.c_str(), obj6);
				}
				auto obj7 = JS_GetPropertyStr(ctx, obj6, obj7_name.c_str());
				if (JS_IsUndefined(obj7)) {
					obj7 = JS_NewObject(ctx);
					JS_SetPropertyStr(ctx, obj6, obj7_name.c_str(), obj7);
				}
				JS_SetPropertyStr(ctx, obj7, property_name.c_str(), JS_NewUint32(ctx, value));
				JS_FreeValue(ctx, global_obj);
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

			inline auto add_constant(
				int64_t value,
				const std::string & obj1_name,
				const std::string & obj2_name,
				const std::string & obj3_name,
				const std::string & obj4_name,
				const std::string & obj5_name,
				const std::string & obj6_name,
				const std::string & obj7_name,
				const std::string & property_name
			) -> void 
			{
				auto global_obj = JS_GetGlobalObject(ctx);
				auto obj1 = JS_GetPropertyStr(ctx, global_obj, obj1_name.c_str());
				if (JS_IsUndefined(obj1)) {
					obj1 = JS_NewObject(ctx);
					JS_SetPropertyStr(ctx, global_obj, obj1_name.c_str(), obj1);
				}
				auto obj2 = JS_GetPropertyStr(ctx, obj1, obj2_name.c_str());
				if (JS_IsUndefined(obj2)) {
					obj2 = JS_NewObject(ctx);
					JS_SetPropertyStr(ctx, obj1, obj2_name.c_str(), obj2);
				}
				auto obj3 = JS_GetPropertyStr(ctx, obj2, obj3_name.c_str());
				if (JS_IsUndefined(obj3)) {
					obj3 = JS_NewObject(ctx);
					JS_SetPropertyStr(ctx, obj2, obj3_name.c_str(), obj3);
				}
				auto obj4 = JS_GetPropertyStr(ctx, obj3, obj4_name.c_str());
				if (JS_IsUndefined(obj4)) {
					obj4 = JS_NewObject(ctx);
					JS_SetPropertyStr(ctx, obj3, obj4_name.c_str(), obj4);
				}
				auto obj5 = JS_GetPropertyStr(ctx, obj4, obj5_name.c_str());
				if (JS_IsUndefined(obj5)) {
					obj5 = JS_NewObject(ctx);
					JS_SetPropertyStr(ctx, obj4, obj5_name.c_str(), obj5);
				}
				auto obj6 = JS_GetPropertyStr(ctx, obj5, obj6_name.c_str());
				if (JS_IsUndefined(obj6)) {
					obj6 = JS_NewObject(ctx);
					JS_SetPropertyStr(ctx, obj5, obj6_name.c_str(), obj6);
				}
				auto obj7 = JS_GetPropertyStr(ctx, obj6, obj7_name.c_str());
				if (JS_IsUndefined(obj7)) {
					obj7 = JS_NewObject(ctx);
					JS_SetPropertyStr(ctx, obj6, obj7_name.c_str(), obj7);
				}
				JS_SetPropertyStr(ctx, obj7, property_name.c_str(), JS_NewInt64(ctx, value));
				JS_FreeValue(ctx, global_obj);
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

			inline auto add_constant(
				uint64_t value,
				const std::string & obj1_name,
				const std::string & obj2_name,
				const std::string & obj3_name,
				const std::string & obj4_name,
				const std::string & obj5_name,
				const std::string & obj6_name,
				const std::string & obj7_name,
				const std::string & property_name
			) -> void 
			{
				auto global_obj = JS_GetGlobalObject(ctx);
				auto obj1 = JS_GetPropertyStr(ctx, global_obj, obj1_name.c_str());
				if (JS_IsUndefined(obj1)) {
					obj1 = JS_NewObject(ctx);
					JS_SetPropertyStr(ctx, global_obj, obj1_name.c_str(), obj1);
				}
				auto obj2 = JS_GetPropertyStr(ctx, obj1, obj2_name.c_str());
				if (JS_IsUndefined(obj2)) {
					obj2 = JS_NewObject(ctx);
					JS_SetPropertyStr(ctx, obj1, obj2_name.c_str(), obj2);
				}
				auto obj3 = JS_GetPropertyStr(ctx, obj2, obj3_name.c_str());
				if (JS_IsUndefined(obj3)) {
					obj3 = JS_NewObject(ctx);
					JS_SetPropertyStr(ctx, obj2, obj3_name.c_str(), obj3);
				}
				auto obj4 = JS_GetPropertyStr(ctx, obj3, obj4_name.c_str());
				if (JS_IsUndefined(obj4)) {
					obj4 = JS_NewObject(ctx);
					JS_SetPropertyStr(ctx, obj3, obj4_name.c_str(), obj4);
				}
				auto obj5 = JS_GetPropertyStr(ctx, obj4, obj5_name.c_str());
				if (JS_IsUndefined(obj5)) {
					obj5 = JS_NewObject(ctx);
					JS_SetPropertyStr(ctx, obj4, obj5_name.c_str(), obj5);
				}
				auto obj6 = JS_GetPropertyStr(ctx, obj5, obj6_name.c_str());
				if (JS_IsUndefined(obj6)) {
					obj6 = JS_NewObject(ctx);
					JS_SetPropertyStr(ctx, obj5, obj6_name.c_str(), obj6);
				}
				auto obj7 = JS_GetPropertyStr(ctx, obj6, obj7_name.c_str());
				if (JS_IsUndefined(obj7)) {
					obj7 = JS_NewObject(ctx);
					JS_SetPropertyStr(ctx, obj6, obj7_name.c_str(), obj7);
				}
				JS_SetPropertyStr(ctx, obj7, property_name.c_str(), JS_NewBigUint64(ctx, value));
				JS_FreeValue(ctx, global_obj);
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

			inline auto add_constant(
				double value,
				const std::string & obj1_name,
				const std::string & obj2_name,
				const std::string & obj3_name,
				const std::string & obj4_name,
				const std::string & obj5_name,
				const std::string & obj6_name,
				const std::string & obj7_name,
				const std::string & property_name
			) -> void 
			{
				auto global_obj = JS_GetGlobalObject(ctx);
				auto obj1 = JS_GetPropertyStr(ctx, global_obj, obj1_name.c_str());
				if (JS_IsUndefined(obj1)) {
					obj1 = JS_NewObject(ctx);
					JS_SetPropertyStr(ctx, global_obj, obj1_name.c_str(), obj1);
				}
				auto obj2 = JS_GetPropertyStr(ctx, obj1, obj2_name.c_str());
				if (JS_IsUndefined(obj2)) {
					obj2 = JS_NewObject(ctx);
					JS_SetPropertyStr(ctx, obj1, obj2_name.c_str(), obj2);
				}
				auto obj3 = JS_GetPropertyStr(ctx, obj2, obj3_name.c_str());
				if (JS_IsUndefined(obj3)) {
					obj3 = JS_NewObject(ctx);
					JS_SetPropertyStr(ctx, obj2, obj3_name.c_str(), obj3);
				}
				auto obj4 = JS_GetPropertyStr(ctx, obj3, obj4_name.c_str());
				if (JS_IsUndefined(obj4)) {
					obj4 = JS_NewObject(ctx);
					JS_SetPropertyStr(ctx, obj3, obj4_name.c_str(), obj4);
				}
				auto obj5 = JS_GetPropertyStr(ctx, obj4, obj5_name.c_str());
				if (JS_IsUndefined(obj5)) {
					obj5 = JS_NewObject(ctx);
					JS_SetPropertyStr(ctx, obj4, obj5_name.c_str(), obj5);
				}
				auto obj6 = JS_GetPropertyStr(ctx, obj5, obj6_name.c_str());
				if (JS_IsUndefined(obj6)) {
					obj6 = JS_NewObject(ctx);
					JS_SetPropertyStr(ctx, obj5, obj6_name.c_str(), obj6);
				}
				auto obj7 = JS_GetPropertyStr(ctx, obj6, obj7_name.c_str());
				if (JS_IsUndefined(obj7)) {
					obj7 = JS_NewObject(ctx);
					JS_SetPropertyStr(ctx, obj6, obj7_name.c_str(), obj7);
				}
				JS_SetPropertyStr(ctx, obj7, property_name.c_str(), JS_NewFloat64(ctx, value));
				JS_FreeValue(ctx, global_obj);
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

			inline auto add_constant(
				float value,
				const std::string & obj1_name,
				const std::string & obj2_name,
				const std::string & obj3_name,
				const std::string & obj4_name,
				const std::string & obj5_name,
				const std::string & obj6_name,
				const std::string & obj7_name,
				const std::string & property_name
			) -> void 
			{
				auto global_obj = JS_GetGlobalObject(ctx);
				auto obj1 = JS_GetPropertyStr(ctx, global_obj, obj1_name.c_str());
				if (JS_IsUndefined(obj1)) {
					obj1 = JS_NewObject(ctx);
					JS_SetPropertyStr(ctx, global_obj, obj1_name.c_str(), obj1);
				}
				auto obj2 = JS_GetPropertyStr(ctx, obj1, obj2_name.c_str());
				if (JS_IsUndefined(obj2)) {
					obj2 = JS_NewObject(ctx);
					JS_SetPropertyStr(ctx, obj1, obj2_name.c_str(), obj2);
				}
				auto obj3 = JS_GetPropertyStr(ctx, obj2, obj3_name.c_str());
				if (JS_IsUndefined(obj3)) {
					obj3 = JS_NewObject(ctx);
					JS_SetPropertyStr(ctx, obj2, obj3_name.c_str(), obj3);
				}
				auto obj4 = JS_GetPropertyStr(ctx, obj3, obj4_name.c_str());
				if (JS_IsUndefined(obj4)) {
					obj4 = JS_NewObject(ctx);
					JS_SetPropertyStr(ctx, obj3, obj4_name.c_str(), obj4);
				}
				auto obj5 = JS_GetPropertyStr(ctx, obj4, obj5_name.c_str());
				if (JS_IsUndefined(obj5)) {
					obj5 = JS_NewObject(ctx);
					JS_SetPropertyStr(ctx, obj4, obj5_name.c_str(), obj5);
				}
				auto obj6 = JS_GetPropertyStr(ctx, obj5, obj6_name.c_str());
				if (JS_IsUndefined(obj6)) {
					obj6 = JS_NewObject(ctx);
					JS_SetPropertyStr(ctx, obj5, obj6_name.c_str(), obj6);
				}
				auto obj7 = JS_GetPropertyStr(ctx, obj6, obj7_name.c_str());
				if (JS_IsUndefined(obj7)) {
					obj7 = JS_NewObject(ctx);
					JS_SetPropertyStr(ctx, obj6, obj7_name.c_str(), obj7);
				}
				JS_SetPropertyStr(ctx, obj7, property_name.c_str(), JS_NewFloat64(ctx, static_cast<double>(value)));
				JS_FreeValue(ctx, global_obj);
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

			inline auto add_constant(
				bool value,
				const std::string & obj1_name,
				const std::string & obj2_name,
				const std::string & obj3_name,
				const std::string & obj4_name,
				const std::string & obj5_name,
				const std::string & obj6_name,
				const std::string & obj7_name,
				const std::string & property_name
			) -> void 
			{
				auto global_obj = JS_GetGlobalObject(ctx);
				auto obj1 = JS_GetPropertyStr(ctx, global_obj, obj1_name.c_str());
				if (JS_IsUndefined(obj1)) {
					obj1 = JS_NewObject(ctx);
					JS_SetPropertyStr(ctx, global_obj, obj1_name.c_str(), obj1);
				}
				auto obj2 = JS_GetPropertyStr(ctx, obj1, obj2_name.c_str());
				if (JS_IsUndefined(obj2)) {
					obj2 = JS_NewObject(ctx);
					JS_SetPropertyStr(ctx, obj1, obj2_name.c_str(), obj2);
				}
				auto obj3 = JS_GetPropertyStr(ctx, obj2, obj3_name.c_str());
				if (JS_IsUndefined(obj3)) {
					obj3 = JS_NewObject(ctx);
					JS_SetPropertyStr(ctx, obj2, obj3_name.c_str(), obj3);
				}
				auto obj4 = JS_GetPropertyStr(ctx, obj3, obj4_name.c_str());
				if (JS_IsUndefined(obj4)) {
					obj4 = JS_NewObject(ctx);
					JS_SetPropertyStr(ctx, obj3, obj4_name.c_str(), obj4);
				}
				auto obj5 = JS_GetPropertyStr(ctx, obj4, obj5_name.c_str());
				if (JS_IsUndefined(obj5)) {
					obj5 = JS_NewObject(ctx);
					JS_SetPropertyStr(ctx, obj4, obj5_name.c_str(), obj5);
				}
				auto obj6 = JS_GetPropertyStr(ctx, obj5, obj6_name.c_str());
				if (JS_IsUndefined(obj6)) {
					obj6 = JS_NewObject(ctx);
					JS_SetPropertyStr(ctx, obj5, obj6_name.c_str(), obj6);
				}
				auto obj7 = JS_GetPropertyStr(ctx, obj6, obj7_name.c_str());
				if (JS_IsUndefined(obj7)) {
					obj7 = JS_NewObject(ctx);
					JS_SetPropertyStr(ctx, obj6, obj7_name.c_str(), obj7);
				}
				JS_SetPropertyStr(ctx, obj7, property_name.c_str(), JS_NewBool(ctx, value ? 1 : 0));
				JS_FreeValue(ctx, global_obj);
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

			inline auto add_constant(
				const std::string & value,
				const std::string & obj1_name,
				const std::string & obj2_name,
				const std::string & obj3_name,
				const std::string & obj4_name,
				const std::string & obj5_name,
				const std::string & obj6_name,
				const std::string & obj7_name,
				const std::string & obj8_name,
				const std::string & property_name
			) -> void 
			{
				auto global_obj = JS_GetGlobalObject(ctx);
				auto obj1 = JS_GetPropertyStr(ctx, global_obj, obj1_name.c_str());
				if (JS_IsUndefined(obj1)) {
					obj1 = JS_NewObject(ctx);
					JS_SetPropertyStr(ctx, global_obj, obj1_name.c_str(), obj1);
				}
				auto obj2 = JS_GetPropertyStr(ctx, obj1, obj2_name.c_str());
				if (JS_IsUndefined(obj2)) {
					obj2 = JS_NewObject(ctx);
					JS_SetPropertyStr(ctx, obj1, obj2_name.c_str(), obj2);
				}
				auto obj3 = JS_GetPropertyStr(ctx, obj2, obj3_name.c_str());
				if (JS_IsUndefined(obj3)) {
					obj3 = JS_NewObject(ctx);
					JS_SetPropertyStr(ctx, obj2, obj3_name.c_str(), obj3);
				}
				auto obj4 = JS_GetPropertyStr(ctx, obj3, obj4_name.c_str());
				if (JS_IsUndefined(obj4)) {
					obj4 = JS_NewObject(ctx);
					JS_SetPropertyStr(ctx, obj3, obj4_name.c_str(), obj4);
				}
				auto obj5 = JS_GetPropertyStr(ctx, obj4, obj5_name.c_str());
				if (JS_IsUndefined(obj5)) {
					obj5 = JS_NewObject(ctx);
					JS_SetPropertyStr(ctx, obj4, obj5_name.c_str(), obj5);
				}
				auto obj6 = JS_GetPropertyStr(ctx, obj5, obj6_name.c_str());
				if (JS_IsUndefined(obj6)) {
					obj6 = JS_NewObject(ctx);
					JS_SetPropertyStr(ctx, obj5, obj6_name.c_str(), obj6);
				}
				auto obj7 = JS_GetPropertyStr(ctx, obj6, obj7_name.c_str());
				if (JS_IsUndefined(obj7)) {
					obj7 = JS_NewObject(ctx);
					JS_SetPropertyStr(ctx, obj6, obj7_name.c_str(), obj7);
				}
				auto obj8 = JS_GetPropertyStr(ctx, obj7, obj8_name.c_str());
				if (JS_IsUndefined(obj8)) {
					obj8 = JS_NewObject(ctx);
					JS_SetPropertyStr(ctx, obj7, obj8_name.c_str(), obj8);
				}
				JS_SetPropertyStr(ctx, obj8, property_name.c_str(), JS_NewString(ctx, value.c_str()));
				JS_FreeValue(ctx, global_obj);
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

			inline auto add_constant(
				int32_t value,
				const std::string & obj1_name,
				const std::string & obj2_name,
				const std::string & obj3_name,
				const std::string & obj4_name,
				const std::string & obj5_name,
				const std::string & obj6_name,
				const std::string & obj7_name,
				const std::string & obj8_name,
				const std::string & property_name
			) -> void 
			{
				auto global_obj = JS_GetGlobalObject(ctx);
				auto obj1 = JS_GetPropertyStr(ctx, global_obj, obj1_name.c_str());
				if (JS_IsUndefined(obj1)) {
					obj1 = JS_NewObject(ctx);
					JS_SetPropertyStr(ctx, global_obj, obj1_name.c_str(), obj1);
				}
				auto obj2 = JS_GetPropertyStr(ctx, obj1, obj2_name.c_str());
				if (JS_IsUndefined(obj2)) {
					obj2 = JS_NewObject(ctx);
					JS_SetPropertyStr(ctx, obj1, obj2_name.c_str(), obj2);
				}
				auto obj3 = JS_GetPropertyStr(ctx, obj2, obj3_name.c_str());
				if (JS_IsUndefined(obj3)) {
					obj3 = JS_NewObject(ctx);
					JS_SetPropertyStr(ctx, obj2, obj3_name.c_str(), obj3);
				}
				auto obj4 = JS_GetPropertyStr(ctx, obj3, obj4_name.c_str());
				if (JS_IsUndefined(obj4)) {
					obj4 = JS_NewObject(ctx);
					JS_SetPropertyStr(ctx, obj3, obj4_name.c_str(), obj4);
				}
				auto obj5 = JS_GetPropertyStr(ctx, obj4, obj5_name.c_str());
				if (JS_IsUndefined(obj5)) {
					obj5 = JS_NewObject(ctx);
					JS_SetPropertyStr(ctx, obj4, obj5_name.c_str(), obj5);
				}
				auto obj6 = JS_GetPropertyStr(ctx, obj5, obj6_name.c_str());
				if (JS_IsUndefined(obj6)) {
					obj6 = JS_NewObject(ctx);
					JS_SetPropertyStr(ctx, obj5, obj6_name.c_str(), obj6);
				}
				auto obj7 = JS_GetPropertyStr(ctx, obj6, obj7_name.c_str());
				if (JS_IsUndefined(obj7)) {
					obj7 = JS_NewObject(ctx);
					JS_SetPropertyStr(ctx, obj6, obj7_name.c_str(), obj7);
				}
				auto obj8 = JS_GetPropertyStr(ctx, obj7, obj8_name.c_str());
				if (JS_IsUndefined(obj8)) {
					obj8 = JS_NewObject(ctx);
					JS_SetPropertyStr(ctx, obj7, obj8_name.c_str(), obj8);
				}
				JS_SetPropertyStr(ctx, obj8, property_name.c_str(), JS_NewInt32(ctx, value));
				JS_FreeValue(ctx, global_obj);
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

			inline auto add_constant(
				uint32_t value,
				const std::string & obj1_name,
				const std::string & obj2_name,
				const std::string & obj3_name,
				const std::string & obj4_name,
				const std::string & obj5_name,
				const std::string & obj6_name,
				const std::string & obj7_name,
				const std::string & obj8_name,
				const std::string & property_name
			) -> void 
			{
				auto global_obj = JS_GetGlobalObject(ctx);
				auto obj1 = JS_GetPropertyStr(ctx, global_obj, obj1_name.c_str());
				if (JS_IsUndefined(obj1)) {
					obj1 = JS_NewObject(ctx);
					JS_SetPropertyStr(ctx, global_obj, obj1_name.c_str(), obj1);
				}
				auto obj2 = JS_GetPropertyStr(ctx, obj1, obj2_name.c_str());
				if (JS_IsUndefined(obj2)) {
					obj2 = JS_NewObject(ctx);
					JS_SetPropertyStr(ctx, obj1, obj2_name.c_str(), obj2);
				}
				auto obj3 = JS_GetPropertyStr(ctx, obj2, obj3_name.c_str());
				if (JS_IsUndefined(obj3)) {
					obj3 = JS_NewObject(ctx);
					JS_SetPropertyStr(ctx, obj2, obj3_name.c_str(), obj3);
				}
				auto obj4 = JS_GetPropertyStr(ctx, obj3, obj4_name.c_str());
				if (JS_IsUndefined(obj4)) {
					obj4 = JS_NewObject(ctx);
					JS_SetPropertyStr(ctx, obj3, obj4_name.c_str(), obj4);
				}
				auto obj5 = JS_GetPropertyStr(ctx, obj4, obj5_name.c_str());
				if (JS_IsUndefined(obj5)) {
					obj5 = JS_NewObject(ctx);
					JS_SetPropertyStr(ctx, obj4, obj5_name.c_str(), obj5);
				}
				auto obj6 = JS_GetPropertyStr(ctx, obj5, obj6_name.c_str());
				if (JS_IsUndefined(obj6)) {
					obj6 = JS_NewObject(ctx);
					JS_SetPropertyStr(ctx, obj5, obj6_name.c_str(), obj6);
				}
				auto obj7 = JS_GetPropertyStr(ctx, obj6, obj7_name.c_str());
				if (JS_IsUndefined(obj7)) {
					obj7 = JS_NewObject(ctx);
					JS_SetPropertyStr(ctx, obj6, obj7_name.c_str(), obj7);
				}
				auto obj8 = JS_GetPropertyStr(ctx, obj7, obj8_name.c_str());
				if (JS_IsUndefined(obj8)) {
					obj8 = JS_NewObject(ctx);
					JS_SetPropertyStr(ctx, obj7, obj8_name.c_str(), obj8);
				}
				JS_SetPropertyStr(ctx, obj8, property_name.c_str(), JS_NewUint32(ctx, value));
				JS_FreeValue(ctx, global_obj);
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

			inline auto add_constant(
				int64_t value,
				const std::string & obj1_name,
				const std::string & obj2_name,
				const std::string & obj3_name,
				const std::string & obj4_name,
				const std::string & obj5_name,
				const std::string & obj6_name,
				const std::string & obj7_name,
				const std::string & obj8_name,
				const std::string & property_name
			) -> void 
			{
				auto global_obj = JS_GetGlobalObject(ctx);
				auto obj1 = JS_GetPropertyStr(ctx, global_obj, obj1_name.c_str());
				if (JS_IsUndefined(obj1)) {
					obj1 = JS_NewObject(ctx);
					JS_SetPropertyStr(ctx, global_obj, obj1_name.c_str(), obj1);
				}
				auto obj2 = JS_GetPropertyStr(ctx, obj1, obj2_name.c_str());
				if (JS_IsUndefined(obj2)) {
					obj2 = JS_NewObject(ctx);
					JS_SetPropertyStr(ctx, obj1, obj2_name.c_str(), obj2);
				}
				auto obj3 = JS_GetPropertyStr(ctx, obj2, obj3_name.c_str());
				if (JS_IsUndefined(obj3)) {
					obj3 = JS_NewObject(ctx);
					JS_SetPropertyStr(ctx, obj2, obj3_name.c_str(), obj3);
				}
				auto obj4 = JS_GetPropertyStr(ctx, obj3, obj4_name.c_str());
				if (JS_IsUndefined(obj4)) {
					obj4 = JS_NewObject(ctx);
					JS_SetPropertyStr(ctx, obj3, obj4_name.c_str(), obj4);
				}
				auto obj5 = JS_GetPropertyStr(ctx, obj4, obj5_name.c_str());
				if (JS_IsUndefined(obj5)) {
					obj5 = JS_NewObject(ctx);
					JS_SetPropertyStr(ctx, obj4, obj5_name.c_str(), obj5);
				}
				auto obj6 = JS_GetPropertyStr(ctx, obj5, obj6_name.c_str());
				if (JS_IsUndefined(obj6)) {
					obj6 = JS_NewObject(ctx);
					JS_SetPropertyStr(ctx, obj5, obj6_name.c_str(), obj6);
				}
				auto obj7 = JS_GetPropertyStr(ctx, obj6, obj7_name.c_str());
				if (JS_IsUndefined(obj7)) {
					obj7 = JS_NewObject(ctx);
					JS_SetPropertyStr(ctx, obj6, obj7_name.c_str(), obj7);
				}
				auto obj8 = JS_GetPropertyStr(ctx, obj7, obj8_name.c_str());
				if (JS_IsUndefined(obj8)) {
					obj8 = JS_NewObject(ctx);
					JS_SetPropertyStr(ctx, obj7, obj8_name.c_str(), obj8);
				}
				JS_SetPropertyStr(ctx, obj8, property_name.c_str(), JS_NewInt64(ctx, value));
				JS_FreeValue(ctx, global_obj);
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

			inline auto add_constant(
				uint64_t value,
				const std::string & obj1_name,
				const std::string & obj2_name,
				const std::string & obj3_name,
				const std::string & obj4_name,
				const std::string & obj5_name,
				const std::string & obj6_name,
				const std::string & obj7_name,
				const std::string & obj8_name,
				const std::string & property_name
			) -> void 
			{
				auto global_obj = JS_GetGlobalObject(ctx);
				auto obj1 = JS_GetPropertyStr(ctx, global_obj, obj1_name.c_str());
				if (JS_IsUndefined(obj1)) {
					obj1 = JS_NewObject(ctx);
					JS_SetPropertyStr(ctx, global_obj, obj1_name.c_str(), obj1);
				}
				auto obj2 = JS_GetPropertyStr(ctx, obj1, obj2_name.c_str());
				if (JS_IsUndefined(obj2)) {
					obj2 = JS_NewObject(ctx);
					JS_SetPropertyStr(ctx, obj1, obj2_name.c_str(), obj2);
				}
				auto obj3 = JS_GetPropertyStr(ctx, obj2, obj3_name.c_str());
				if (JS_IsUndefined(obj3)) {
					obj3 = JS_NewObject(ctx);
					JS_SetPropertyStr(ctx, obj2, obj3_name.c_str(), obj3);
				}
				auto obj4 = JS_GetPropertyStr(ctx, obj3, obj4_name.c_str());
				if (JS_IsUndefined(obj4)) {
					obj4 = JS_NewObject(ctx);
					JS_SetPropertyStr(ctx, obj3, obj4_name.c_str(), obj4);
				}
				auto obj5 = JS_GetPropertyStr(ctx, obj4, obj5_name.c_str());
				if (JS_IsUndefined(obj5)) {
					obj5 = JS_NewObject(ctx);
					JS_SetPropertyStr(ctx, obj4, obj5_name.c_str(), obj5);
				}
				auto obj6 = JS_GetPropertyStr(ctx, obj5, obj6_name.c_str());
				if (JS_IsUndefined(obj6)) {
					obj6 = JS_NewObject(ctx);
					JS_SetPropertyStr(ctx, obj5, obj6_name.c_str(), obj6);
				}
				auto obj7 = JS_GetPropertyStr(ctx, obj6, obj7_name.c_str());
				if (JS_IsUndefined(obj7)) {
					obj7 = JS_NewObject(ctx);
					JS_SetPropertyStr(ctx, obj6, obj7_name.c_str(), obj7);
				}
				auto obj8 = JS_GetPropertyStr(ctx, obj7, obj8_name.c_str());
				if (JS_IsUndefined(obj8)) {
					obj8 = JS_NewObject(ctx);
					JS_SetPropertyStr(ctx, obj7, obj8_name.c_str(), obj8);
				}
				JS_SetPropertyStr(ctx, obj8, property_name.c_str(), JS_NewBigUint64(ctx, value));
				JS_FreeValue(ctx, global_obj);
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

			inline auto add_constant(
				double value,
				const std::string & obj1_name,
				const std::string & obj2_name,
				const std::string & obj3_name,
				const std::string & obj4_name,
				const std::string & obj5_name,
				const std::string & obj6_name,
				const std::string & obj7_name,
				const std::string & obj8_name,
				const std::string & property_name
			) -> void 
			{
				auto global_obj = JS_GetGlobalObject(ctx);
				auto obj1 = JS_GetPropertyStr(ctx, global_obj, obj1_name.c_str());
				if (JS_IsUndefined(obj1)) {
					obj1 = JS_NewObject(ctx);
					JS_SetPropertyStr(ctx, global_obj, obj1_name.c_str(), obj1);
				}
				auto obj2 = JS_GetPropertyStr(ctx, obj1, obj2_name.c_str());
				if (JS_IsUndefined(obj2)) {
					obj2 = JS_NewObject(ctx);
					JS_SetPropertyStr(ctx, obj1, obj2_name.c_str(), obj2);
				}
				auto obj3 = JS_GetPropertyStr(ctx, obj2, obj3_name.c_str());
				if (JS_IsUndefined(obj3)) {
					obj3 = JS_NewObject(ctx);
					JS_SetPropertyStr(ctx, obj2, obj3_name.c_str(), obj3);
				}
				auto obj4 = JS_GetPropertyStr(ctx, obj3, obj4_name.c_str());
				if (JS_IsUndefined(obj4)) {
					obj4 = JS_NewObject(ctx);
					JS_SetPropertyStr(ctx, obj3, obj4_name.c_str(), obj4);
				}
				auto obj5 = JS_GetPropertyStr(ctx, obj4, obj5_name.c_str());
				if (JS_IsUndefined(obj5)) {
					obj5 = JS_NewObject(ctx);
					JS_SetPropertyStr(ctx, obj4, obj5_name.c_str(), obj5);
				}
				auto obj6 = JS_GetPropertyStr(ctx, obj5, obj6_name.c_str());
				if (JS_IsUndefined(obj6)) {
					obj6 = JS_NewObject(ctx);
					JS_SetPropertyStr(ctx, obj5, obj6_name.c_str(), obj6);
				}
				auto obj7 = JS_GetPropertyStr(ctx, obj6, obj7_name.c_str());
				if (JS_IsUndefined(obj7)) {
					obj7 = JS_NewObject(ctx);
					JS_SetPropertyStr(ctx, obj6, obj7_name.c_str(), obj7);
				}
				auto obj8 = JS_GetPropertyStr(ctx, obj7, obj8_name.c_str());
				if (JS_IsUndefined(obj8)) {
					obj8 = JS_NewObject(ctx);
					JS_SetPropertyStr(ctx, obj7, obj8_name.c_str(), obj8);
				}
				JS_SetPropertyStr(ctx, obj8, property_name.c_str(), JS_NewFloat64(ctx, value));
				JS_FreeValue(ctx, global_obj);
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

			inline auto add_constant(
				float value,
				const std::string & obj1_name,
				const std::string & obj2_name,
				const std::string & obj3_name,
				const std::string & obj4_name,
				const std::string & obj5_name,
				const std::string & obj6_name,
				const std::string & obj7_name,
				const std::string & obj8_name,
				const std::string & property_name
			) -> void 
			{
				auto global_obj = JS_GetGlobalObject(ctx);
				auto obj1 = JS_GetPropertyStr(ctx, global_obj, obj1_name.c_str());
				if (JS_IsUndefined(obj1)) {
					obj1 = JS_NewObject(ctx);
					JS_SetPropertyStr(ctx, global_obj, obj1_name.c_str(), obj1);
				}
				auto obj2 = JS_GetPropertyStr(ctx, obj1, obj2_name.c_str());
				if (JS_IsUndefined(obj2)) {
					obj2 = JS_NewObject(ctx);
					JS_SetPropertyStr(ctx, obj1, obj2_name.c_str(), obj2);
				}
				auto obj3 = JS_GetPropertyStr(ctx, obj2, obj3_name.c_str());
				if (JS_IsUndefined(obj3)) {
					obj3 = JS_NewObject(ctx);
					JS_SetPropertyStr(ctx, obj2, obj3_name.c_str(), obj3);
				}
				auto obj4 = JS_GetPropertyStr(ctx, obj3, obj4_name.c_str());
				if (JS_IsUndefined(obj4)) {
					obj4 = JS_NewObject(ctx);
					JS_SetPropertyStr(ctx, obj3, obj4_name.c_str(), obj4);
				}
				auto obj5 = JS_GetPropertyStr(ctx, obj4, obj5_name.c_str());
				if (JS_IsUndefined(obj5)) {
					obj5 = JS_NewObject(ctx);
					JS_SetPropertyStr(ctx, obj4, obj5_name.c_str(), obj5);
				}
				auto obj6 = JS_GetPropertyStr(ctx, obj5, obj6_name.c_str());
				if (JS_IsUndefined(obj6)) {
					obj6 = JS_NewObject(ctx);
					JS_SetPropertyStr(ctx, obj5, obj6_name.c_str(), obj6);
				}
				auto obj7 = JS_GetPropertyStr(ctx, obj6, obj7_name.c_str());
				if (JS_IsUndefined(obj7)) {
					obj7 = JS_NewObject(ctx);
					JS_SetPropertyStr(ctx, obj6, obj7_name.c_str(), obj7);
				}
				auto obj8 = JS_GetPropertyStr(ctx, obj7, obj8_name.c_str());
				if (JS_IsUndefined(obj8)) {
					obj8 = JS_NewObject(ctx);
					JS_SetPropertyStr(ctx, obj7, obj8_name.c_str(), obj8);
				}
				JS_SetPropertyStr(ctx, obj8, property_name.c_str(), JS_NewFloat64(ctx, static_cast<double>(value)));
				JS_FreeValue(ctx, global_obj);
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

			inline auto add_constant(
				bool value,
				const std::string & obj1_name,
				const std::string & obj2_name,
				const std::string & obj3_name,
				const std::string & obj4_name,
				const std::string & obj5_name,
				const std::string & obj6_name,
				const std::string & obj7_name,
				const std::string & obj8_name,
				const std::string & property_name
			) -> void 
			{
				auto global_obj = JS_GetGlobalObject(ctx);
				auto obj1 = JS_GetPropertyStr(ctx, global_obj, obj1_name.c_str());
				if (JS_IsUndefined(obj1)) {
					obj1 = JS_NewObject(ctx);
					JS_SetPropertyStr(ctx, global_obj, obj1_name.c_str(), obj1);
				}
				auto obj2 = JS_GetPropertyStr(ctx, obj1, obj2_name.c_str());
				if (JS_IsUndefined(obj2)) {
					obj2 = JS_NewObject(ctx);
					JS_SetPropertyStr(ctx, obj1, obj2_name.c_str(), obj2);
				}
				auto obj3 = JS_GetPropertyStr(ctx, obj2, obj3_name.c_str());
				if (JS_IsUndefined(obj3)) {
					obj3 = JS_NewObject(ctx);
					JS_SetPropertyStr(ctx, obj2, obj3_name.c_str(), obj3);
				}
				auto obj4 = JS_GetPropertyStr(ctx, obj3, obj4_name.c_str());
				if (JS_IsUndefined(obj4)) {
					obj4 = JS_NewObject(ctx);
					JS_SetPropertyStr(ctx, obj3, obj4_name.c_str(), obj4);
				}
				auto obj5 = JS_GetPropertyStr(ctx, obj4, obj5_name.c_str());
				if (JS_IsUndefined(obj5)) {
					obj5 = JS_NewObject(ctx);
					JS_SetPropertyStr(ctx, obj4, obj5_name.c_str(), obj5);
				}
				auto obj6 = JS_GetPropertyStr(ctx, obj5, obj6_name.c_str());
				if (JS_IsUndefined(obj6)) {
					obj6 = JS_NewObject(ctx);
					JS_SetPropertyStr(ctx, obj5, obj6_name.c_str(), obj6);
				}
				auto obj7 = JS_GetPropertyStr(ctx, obj6, obj7_name.c_str());
				if (JS_IsUndefined(obj7)) {
					obj7 = JS_NewObject(ctx);
					JS_SetPropertyStr(ctx, obj6, obj7_name.c_str(), obj7);
				}
				auto obj8 = JS_GetPropertyStr(ctx, obj7, obj8_name.c_str());
				if (JS_IsUndefined(obj8)) {
					obj8 = JS_NewObject(ctx);
					JS_SetPropertyStr(ctx, obj7, obj8_name.c_str(), obj8);
				}
				JS_SetPropertyStr(ctx, obj7, property_name.c_str(), JS_NewBool(ctx, value ? 1 : 0));
				JS_FreeValue(ctx, global_obj);
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

			inline auto add_constant(
				float value,
				const std::string & obj1_name,
				const std::string & obj2_name,
				const std::string & obj3_name,
				const std::string & obj4_name,
				const std::string & obj5_name,
				const std::string & obj6_name,
				const std::string & obj7_name,
				const std::string & obj8_name,
				const std::string & obj9_name,
				const std::string & property_name
			) -> void 
			{
				auto global_obj = JS_GetGlobalObject(ctx);
				auto obj1 = JS_GetPropertyStr(ctx, global_obj, obj1_name.c_str());
				if (JS_IsUndefined(obj1)) {
					obj1 = JS_NewObject(ctx);
					JS_SetPropertyStr(ctx, global_obj, obj1_name.c_str(), obj1);
				}
				auto obj2 = JS_GetPropertyStr(ctx, obj1, obj2_name.c_str());
				if (JS_IsUndefined(obj2)) {
					obj2 = JS_NewObject(ctx);
					JS_SetPropertyStr(ctx, obj1, obj2_name.c_str(), obj2);
				}
				auto obj3 = JS_GetPropertyStr(ctx, obj2, obj3_name.c_str());
				if (JS_IsUndefined(obj3)) {
					obj3 = JS_NewObject(ctx);
					JS_SetPropertyStr(ctx, obj2, obj3_name.c_str(), obj3);
				}
				auto obj4 = JS_GetPropertyStr(ctx, obj3, obj4_name.c_str());
				if (JS_IsUndefined(obj4)) {
					obj4 = JS_NewObject(ctx);
					JS_SetPropertyStr(ctx, obj3, obj4_name.c_str(), obj4);
				}
				auto obj5 = JS_GetPropertyStr(ctx, obj4, obj5_name.c_str());
				if (JS_IsUndefined(obj5)) {
					obj5 = JS_NewObject(ctx);
					JS_SetPropertyStr(ctx, obj4, obj5_name.c_str(), obj5);
				}
				auto obj6 = JS_GetPropertyStr(ctx, obj5, obj6_name.c_str());
				if (JS_IsUndefined(obj6)) {
					obj6 = JS_NewObject(ctx);
					JS_SetPropertyStr(ctx, obj5, obj6_name.c_str(), obj6);
				}
				auto obj7 = JS_GetPropertyStr(ctx, obj6, obj7_name.c_str());
				if (JS_IsUndefined(obj7)) {
					obj7 = JS_NewObject(ctx);
					JS_SetPropertyStr(ctx, obj6, obj7_name.c_str(), obj7);
				}
				auto obj8 = JS_GetPropertyStr(ctx, obj7, obj8_name.c_str());
				if (JS_IsUndefined(obj8)) {
					obj8 = JS_NewObject(ctx);
					JS_SetPropertyStr(ctx, obj7, obj8_name.c_str(), obj8);
				}
				auto obj9 = JS_GetPropertyStr(ctx, obj8, obj9_name.c_str());
				if (JS_IsUndefined(obj9)) {
					obj9 = JS_NewObject(ctx);
					JS_SetPropertyStr(ctx, obj8, obj9_name.c_str(), obj9);
				}
				JS_SetPropertyStr(ctx, obj8, property_name.c_str(), JS_NewFloat64(ctx, static_cast<double>(value)));
				JS_FreeValue(ctx, global_obj);
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
			 * @param obj9_name: JS object name
			 * @param property_name: JS property name
			 * --------------------------------------
			*/

			inline auto add_constant(
				bool value,
				const std::string & obj1_name,
				const std::string & obj2_name,
				const std::string & obj3_name,
				const std::string & obj4_name,
				const std::string & obj5_name,
				const std::string & obj6_name,
				const std::string & obj7_name,
				const std::string & obj8_name,
				const std::string & obj9_name,
				const std::string & property_name
			) -> void 
			{
				auto global_obj = JS_GetGlobalObject(ctx);
				auto obj1 = JS_GetPropertyStr(ctx, global_obj, obj1_name.c_str());
				if (JS_IsUndefined(obj1)) {
					obj1 = JS_NewObject(ctx);
					JS_SetPropertyStr(ctx, global_obj, obj1_name.c_str(), obj1);
				}
				auto obj2 = JS_GetPropertyStr(ctx, obj1, obj2_name.c_str());
				if (JS_IsUndefined(obj2)) {
					obj2 = JS_NewObject(ctx);
					JS_SetPropertyStr(ctx, obj1, obj2_name.c_str(), obj2);
				}
				auto obj3 = JS_GetPropertyStr(ctx, obj2, obj3_name.c_str());
				if (JS_IsUndefined(obj3)) {
					obj3 = JS_NewObject(ctx);
					JS_SetPropertyStr(ctx, obj2, obj3_name.c_str(), obj3);
				}
				auto obj4 = JS_GetPropertyStr(ctx, obj3, obj4_name.c_str());
				if (JS_IsUndefined(obj4)) {
					obj4 = JS_NewObject(ctx);
					JS_SetPropertyStr(ctx, obj3, obj4_name.c_str(), obj4);
				}
				auto obj5 = JS_GetPropertyStr(ctx, obj4, obj5_name.c_str());
				if (JS_IsUndefined(obj5)) {
					obj5 = JS_NewObject(ctx);
					JS_SetPropertyStr(ctx, obj4, obj5_name.c_str(), obj5);
				}
				auto obj6 = JS_GetPropertyStr(ctx, obj5, obj6_name.c_str());
				if (JS_IsUndefined(obj6)) {
					obj6 = JS_NewObject(ctx);
					JS_SetPropertyStr(ctx, obj5, obj6_name.c_str(), obj6);
				}
				auto obj7 = JS_GetPropertyStr(ctx, obj6, obj7_name.c_str());
				if (JS_IsUndefined(obj7)) {
					obj7 = JS_NewObject(ctx);
					JS_SetPropertyStr(ctx, obj6, obj7_name.c_str(), obj7);
				}
				auto obj8 = JS_GetPropertyStr(ctx, obj7, obj8_name.c_str());
				if (JS_IsUndefined(obj8)) {
					obj8 = JS_NewObject(ctx);
					JS_SetPropertyStr(ctx, obj7, obj8_name.c_str(), obj8);
				}
				auto obj9 = JS_GetPropertyStr(ctx, obj8, obj9_name.c_str());
				if (JS_IsUndefined(obj9)) {
					obj9 = JS_NewObject(ctx);
					JS_SetPropertyStr(ctx, obj8, obj9_name.c_str(), obj9);
				}
				JS_SetPropertyStr(ctx, obj8, property_name.c_str(), JS_NewBool(ctx, value ? 1 : 0));
				JS_FreeValue(ctx, global_obj);
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

			inline auto add_constant(
				uint32_t value,
				const std::string & obj1_name,
				const std::string & obj2_name,
				const std::string & obj3_name,
				const std::string & obj4_name,
				const std::string & obj5_name,
				const std::string & obj6_name,
				const std::string & obj7_name,
				const std::string & obj8_name,
				const std::string & obj9_name,
				const std::string & property_name
			) -> void 
			{
				auto global_obj = JS_GetGlobalObject(ctx);
				auto obj1 = JS_GetPropertyStr(ctx, global_obj, obj1_name.c_str());
				if (JS_IsUndefined(obj1)) {
					obj1 = JS_NewObject(ctx);
					JS_SetPropertyStr(ctx, global_obj, obj1_name.c_str(), obj1);
				}
				auto obj2 = JS_GetPropertyStr(ctx, obj1, obj2_name.c_str());
				if (JS_IsUndefined(obj2)) {
					obj2 = JS_NewObject(ctx);
					JS_SetPropertyStr(ctx, obj1, obj2_name.c_str(), obj2);
				}
				auto obj3 = JS_GetPropertyStr(ctx, obj2, obj3_name.c_str());
				if (JS_IsUndefined(obj3)) {
					obj3 = JS_NewObject(ctx);
					JS_SetPropertyStr(ctx, obj2, obj3_name.c_str(), obj3);
				}
				auto obj4 = JS_GetPropertyStr(ctx, obj3, obj4_name.c_str());
				if (JS_IsUndefined(obj4)) {
					obj4 = JS_NewObject(ctx);
					JS_SetPropertyStr(ctx, obj3, obj4_name.c_str(), obj4);
				}
				auto obj5 = JS_GetPropertyStr(ctx, obj4, obj5_name.c_str());
				if (JS_IsUndefined(obj5)) {
					obj5 = JS_NewObject(ctx);
					JS_SetPropertyStr(ctx, obj4, obj5_name.c_str(), obj5);
				}
				auto obj6 = JS_GetPropertyStr(ctx, obj5, obj6_name.c_str());
				if (JS_IsUndefined(obj6)) {
					obj6 = JS_NewObject(ctx);
					JS_SetPropertyStr(ctx, obj5, obj6_name.c_str(), obj6);
				}
				auto obj7 = JS_GetPropertyStr(ctx, obj6, obj7_name.c_str());
				if (JS_IsUndefined(obj7)) {
					obj7 = JS_NewObject(ctx);
					JS_SetPropertyStr(ctx, obj6, obj7_name.c_str(), obj7);
				}
				auto obj8 = JS_GetPropertyStr(ctx, obj7, obj8_name.c_str());
				if (JS_IsUndefined(obj8)) {
					obj8 = JS_NewObject(ctx);
					JS_SetPropertyStr(ctx, obj7, obj8_name.c_str(), obj8);
				}
				auto obj9 = JS_GetPropertyStr(ctx, obj8, obj9_name.c_str());
				if (JS_IsUndefined(obj9)) {
					obj9 = JS_NewObject(ctx);
					JS_SetPropertyStr(ctx, obj8, obj9_name.c_str(), obj9);
				}
				JS_SetPropertyStr(ctx, obj8, property_name.c_str(), JS_NewUint32(ctx, value));
				JS_FreeValue(ctx, global_obj);
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

			inline auto add_constant(
				int64_t value,
				const std::string & obj1_name,
				const std::string & obj2_name,
				const std::string & obj3_name,
				const std::string & obj4_name,
				const std::string & obj5_name,
				const std::string & obj6_name,
				const std::string & obj7_name,
				const std::string & obj8_name,
				const std::string & obj9_name,
				const std::string & property_name
			) -> void 
			{
				auto global_obj = JS_GetGlobalObject(ctx);
				auto obj1 = JS_GetPropertyStr(ctx, global_obj, obj1_name.c_str());
				if (JS_IsUndefined(obj1)) {
					obj1 = JS_NewObject(ctx);
					JS_SetPropertyStr(ctx, global_obj, obj1_name.c_str(), obj1);
				}
				auto obj2 = JS_GetPropertyStr(ctx, obj1, obj2_name.c_str());
				if (JS_IsUndefined(obj2)) {
					obj2 = JS_NewObject(ctx);
					JS_SetPropertyStr(ctx, obj1, obj2_name.c_str(), obj2);
				}
				auto obj3 = JS_GetPropertyStr(ctx, obj2, obj3_name.c_str());
				if (JS_IsUndefined(obj3)) {
					obj3 = JS_NewObject(ctx);
					JS_SetPropertyStr(ctx, obj2, obj3_name.c_str(), obj3);
				}
				auto obj4 = JS_GetPropertyStr(ctx, obj3, obj4_name.c_str());
				if (JS_IsUndefined(obj4)) {
					obj4 = JS_NewObject(ctx);
					JS_SetPropertyStr(ctx, obj3, obj4_name.c_str(), obj4);
				}
				auto obj5 = JS_GetPropertyStr(ctx, obj4, obj5_name.c_str());
				if (JS_IsUndefined(obj5)) {
					obj5 = JS_NewObject(ctx);
					JS_SetPropertyStr(ctx, obj4, obj5_name.c_str(), obj5);
				}
				auto obj6 = JS_GetPropertyStr(ctx, obj5, obj6_name.c_str());
				if (JS_IsUndefined(obj6)) {
					obj6 = JS_NewObject(ctx);
					JS_SetPropertyStr(ctx, obj5, obj6_name.c_str(), obj6);
				}
				auto obj7 = JS_GetPropertyStr(ctx, obj6, obj7_name.c_str());
				if (JS_IsUndefined(obj7)) {
					obj7 = JS_NewObject(ctx);
					JS_SetPropertyStr(ctx, obj6, obj7_name.c_str(), obj7);
				}
				auto obj8 = JS_GetPropertyStr(ctx, obj7, obj8_name.c_str());
				if (JS_IsUndefined(obj8)) {
					obj8 = JS_NewObject(ctx);
					JS_SetPropertyStr(ctx, obj7, obj8_name.c_str(), obj8);
				}
				auto obj9 = JS_GetPropertyStr(ctx, obj8, obj9_name.c_str());
				if (JS_IsUndefined(obj9)) {
					obj9 = JS_NewObject(ctx);
					JS_SetPropertyStr(ctx, obj8, obj9_name.c_str(), obj9);
				}
				JS_SetPropertyStr(ctx, obj8, property_name.c_str(), JS_NewInt64(ctx, value));
				JS_FreeValue(ctx, global_obj);
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

			inline auto add_constant(
				uint64_t value,
				const std::string & obj1_name,
				const std::string & obj2_name,
				const std::string & obj3_name,
				const std::string & obj4_name,
				const std::string & obj5_name,
				const std::string & obj6_name,
				const std::string & obj7_name,
				const std::string & obj8_name,
				const std::string & obj9_name,
				const std::string & property_name
			) -> void 
			{
				auto global_obj = JS_GetGlobalObject(ctx);
				auto obj1 = JS_GetPropertyStr(ctx, global_obj, obj1_name.c_str());
				if (JS_IsUndefined(obj1)) {
					obj1 = JS_NewObject(ctx);
					JS_SetPropertyStr(ctx, global_obj, obj1_name.c_str(), obj1);
				}
				auto obj2 = JS_GetPropertyStr(ctx, obj1, obj2_name.c_str());
				if (JS_IsUndefined(obj2)) {
					obj2 = JS_NewObject(ctx);
					JS_SetPropertyStr(ctx, obj1, obj2_name.c_str(), obj2);
				}
				auto obj3 = JS_GetPropertyStr(ctx, obj2, obj3_name.c_str());
				if (JS_IsUndefined(obj3)) {
					obj3 = JS_NewObject(ctx);
					JS_SetPropertyStr(ctx, obj2, obj3_name.c_str(), obj3);
				}
				auto obj4 = JS_GetPropertyStr(ctx, obj3, obj4_name.c_str());
				if (JS_IsUndefined(obj4)) {
					obj4 = JS_NewObject(ctx);
					JS_SetPropertyStr(ctx, obj3, obj4_name.c_str(), obj4);
				}
				auto obj5 = JS_GetPropertyStr(ctx, obj4, obj5_name.c_str());
				if (JS_IsUndefined(obj5)) {
					obj5 = JS_NewObject(ctx);
					JS_SetPropertyStr(ctx, obj4, obj5_name.c_str(), obj5);
				}
				auto obj6 = JS_GetPropertyStr(ctx, obj5, obj6_name.c_str());
				if (JS_IsUndefined(obj6)) {
					obj6 = JS_NewObject(ctx);
					JS_SetPropertyStr(ctx, obj5, obj6_name.c_str(), obj6);
				}
				auto obj7 = JS_GetPropertyStr(ctx, obj6, obj7_name.c_str());
				if (JS_IsUndefined(obj7)) {
					obj7 = JS_NewObject(ctx);
					JS_SetPropertyStr(ctx, obj6, obj7_name.c_str(), obj7);
				}
				auto obj8 = JS_GetPropertyStr(ctx, obj7, obj8_name.c_str());
				if (JS_IsUndefined(obj8)) {
					obj8 = JS_NewObject(ctx);
					JS_SetPropertyStr(ctx, obj7, obj8_name.c_str(), obj8);
				}
				auto obj9 = JS_GetPropertyStr(ctx, obj8, obj9_name.c_str());
				if (JS_IsUndefined(obj9)) {
					obj9 = JS_NewObject(ctx);
					JS_SetPropertyStr(ctx, obj8, obj9_name.c_str(), obj9);
				}
				JS_SetPropertyStr(ctx, obj8, property_name.c_str(), JS_NewBigUint64(ctx, value));
				JS_FreeValue(ctx, global_obj);
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

			inline auto add_constant(
				double value,
				const std::string & obj1_name,
				const std::string & obj2_name,
				const std::string & obj3_name,
				const std::string & obj4_name,
				const std::string & obj5_name,
				const std::string & obj6_name,
				const std::string & obj7_name,
				const std::string & obj8_name,
				const std::string & obj9_name,
				const std::string & property_name
			) -> void 
			{
				auto global_obj = JS_GetGlobalObject(ctx);
				auto obj1 = JS_GetPropertyStr(ctx, global_obj, obj1_name.c_str());
				if (JS_IsUndefined(obj1)) {
					obj1 = JS_NewObject(ctx);
					JS_SetPropertyStr(ctx, global_obj, obj1_name.c_str(), obj1);
				}
				auto obj2 = JS_GetPropertyStr(ctx, obj1, obj2_name.c_str());
				if (JS_IsUndefined(obj2)) {
					obj2 = JS_NewObject(ctx);
					JS_SetPropertyStr(ctx, obj1, obj2_name.c_str(), obj2);
				}
				auto obj3 = JS_GetPropertyStr(ctx, obj2, obj3_name.c_str());
				if (JS_IsUndefined(obj3)) {
					obj3 = JS_NewObject(ctx);
					JS_SetPropertyStr(ctx, obj2, obj3_name.c_str(), obj3);
				}
				auto obj4 = JS_GetPropertyStr(ctx, obj3, obj4_name.c_str());
				if (JS_IsUndefined(obj4)) {
					obj4 = JS_NewObject(ctx);
					JS_SetPropertyStr(ctx, obj3, obj4_name.c_str(), obj4);
				}
				auto obj5 = JS_GetPropertyStr(ctx, obj4, obj5_name.c_str());
				if (JS_IsUndefined(obj5)) {
					obj5 = JS_NewObject(ctx);
					JS_SetPropertyStr(ctx, obj4, obj5_name.c_str(), obj5);
				}
				auto obj6 = JS_GetPropertyStr(ctx, obj5, obj6_name.c_str());
				if (JS_IsUndefined(obj6)) {
					obj6 = JS_NewObject(ctx);
					JS_SetPropertyStr(ctx, obj5, obj6_name.c_str(), obj6);
				}
				auto obj7 = JS_GetPropertyStr(ctx, obj6, obj7_name.c_str());
				if (JS_IsUndefined(obj7)) {
					obj7 = JS_NewObject(ctx);
					JS_SetPropertyStr(ctx, obj6, obj7_name.c_str(), obj7);
				}
				auto obj8 = JS_GetPropertyStr(ctx, obj7, obj8_name.c_str());
				if (JS_IsUndefined(obj8)) {
					obj8 = JS_NewObject(ctx);
					JS_SetPropertyStr(ctx, obj7, obj8_name.c_str(), obj8);
				}
				auto obj9 = JS_GetPropertyStr(ctx, obj8, obj9_name.c_str());
				if (JS_IsUndefined(obj9)) {
					obj9 = JS_NewObject(ctx);
					JS_SetPropertyStr(ctx, obj8, obj9_name.c_str(), obj9);
				}
				JS_SetPropertyStr(ctx, obj8, property_name.c_str(), JS_NewFloat64(ctx, value));
				JS_FreeValue(ctx, global_obj);
				return;
			}



			/**
			 * Destructor
			*/

			~Runtime(

			) 
			{
				JS_FreeContext(thiz.ctx);
				JS_FreeRuntime(thiz.rt);
			}
	};
}