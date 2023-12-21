#pragma once

#include "kernel/definition/javascript/converter.hpp"

namespace Sen::Kernel::Definition::JavaScript
{

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

			auto free_value(
				const JSValue & that
			) -> void
			{
				JS_FreeValue(thiz.ctx, that);
				return;
			}

			/**
			 * Free C String
			*/

			auto free_string(
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

			auto exception(
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

			auto eval_fs(
				const std::string & source
			) -> JSValue
			{
				return thiz.eval(FileSystem::readFile(source), source);
			}

			/**
			 * ------------------------------------------------------------
			 * Evaluate JS
			 * @param source_data: the eval script data
			 * @param source_file: the source file contains the script data
			 * @return: JS eval data
			 * ------------------------------------------------------------
			*/

			auto eval(
				const std::string & source_data,
				const std::string & source_file
			) -> JSValue
			{
				auto eval_result = JS_Eval(thiz.ctx, source_data.c_str(), source_data.size(), Path::getFileName(source_file).c_str(), JS_EVAL_TYPE_GLOBAL);
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

			auto add_proxy(
				JSValue (*func)(JSContext *, JSValueConst, int, JSValueConst *),
				const std::string & function_name
			) -> void 
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

			auto add_proxy(
				JSValue (*func)(JSContext *, JSValueConst, int, JSValueConst *),
				const std::string & object_name,
				const std::string & function_name
			) -> void 
			{
				auto myObject = JS_NewObject(ctx);
				JS_SetPropertyStr(ctx, myObject, function_name.c_str(), JS_NewCFunction(ctx, func, function_name.c_str(), 0));
				auto global_obj = JS_GetGlobalObject(ctx);
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

			auto add_proxy(
				JSValue (*func)(JSContext *, JSValueConst, int, JSValueConst *),
				const std::string & obj1_name,
				const std::string & obj2_name,
				const std::string & function_name
			) -> void 
			{
				auto innerObject = JS_NewObject(ctx);
				JS_SetPropertyStr(ctx, innerObject, function_name.c_str(), JS_NewCFunction(ctx, func, function_name.c_str(), 0));
				auto outerObject = JS_NewObject(ctx);
				JS_SetPropertyStr(ctx, outerObject, obj2_name.c_str(), innerObject);
				auto global_obj = JS_GetGlobalObject(ctx);
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

			auto add_proxy(
				JSValue (*func)(JSContext *, JSValueConst, int, JSValueConst *),
				const std::string & obj1_name,
				const std::string & obj2_name,
				const std::string & obj3_name,
				const std::string & function_name
			) -> void 
			{
				auto innerObject = JS_NewObject(ctx);
				JS_SetPropertyStr(ctx, innerObject, function_name.c_str(), JS_NewCFunction(ctx, func, function_name.c_str(), 0));
				auto middleObject = JS_NewObject(ctx);
				JS_SetPropertyStr(ctx, middleObject, obj3_name.c_str(), innerObject);
				auto outerObject = JS_NewObject(ctx);
				JS_SetPropertyStr(ctx, outerObject, obj2_name.c_str(), middleObject);
				auto global_obj = JS_GetGlobalObject(ctx);
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

			auto add_proxy(
				JSValue (*func)(JSContext *, JSValueConst, int, JSValueConst *),
				const std::string & obj1_name,
				const std::string & obj2_name,
				const std::string & obj3_name,
				const std::string & obj4_name,
				const std::string & function_name
			) -> void 
			{
				auto innerObject = JS_NewObject(ctx);
				JS_SetPropertyStr(ctx, innerObject, function_name.c_str(), JS_NewCFunction(ctx, func, function_name.c_str(), 0));
				auto middle2Object = JS_NewObject(ctx);
				JS_SetPropertyStr(ctx, middle2Object, obj4_name.c_str(), innerObject);
				auto middle1Object = JS_NewObject(ctx);
				JS_SetPropertyStr(ctx, middle1Object, obj3_name.c_str(), middle2Object);
				auto outerObject = JS_NewObject(ctx);
				JS_SetPropertyStr(ctx, outerObject, obj2_name.c_str(), middle1Object);
				auto global_obj = JS_GetGlobalObject(ctx);
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

			auto add_proxy(
				JSValue (*func)(JSContext *, JSValueConst, int, JSValueConst *),
				const std::string & obj1_name,
				const std::string & obj2_name,
				const std::string & obj3_name,
				const std::string & obj4_name,
				const std::string & obj5_name,
				const std::string & function_name
			) -> void 
			{
				auto innerObject = JS_NewObject(ctx);
				JS_SetPropertyStr(ctx, innerObject, function_name.c_str(), JS_NewCFunction(ctx, func, function_name.c_str(), 0));
				auto middle3Object = JS_NewObject(ctx);
				JS_SetPropertyStr(ctx, middle3Object, obj5_name.c_str(), innerObject);
				auto middle2Object = JS_NewObject(ctx);
				JS_SetPropertyStr(ctx, middle2Object, obj4_name.c_str(), middle3Object);
				auto middle1Object = JS_NewObject(ctx);
				JS_SetPropertyStr(ctx, middle1Object, obj3_name.c_str(), middle2Object);
				auto outerObject = JS_NewObject(ctx);
				JS_SetPropertyStr(ctx, outerObject, obj2_name.c_str(), middle1Object);
				auto global_obj = JS_GetGlobalObject(ctx);
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

			auto add_proxy(
				JSValue (*func)(JSContext *, JSValueConst, int, JSValueConst *),
				const std::string & obj1_name,
				const std::string & obj2_name,
				const std::string & obj3_name,
				const std::string & obj4_name,
				const std::string & obj5_name,
				const std::string & obj6_name,
				const std::string & function_name
			) -> void 
			{
				auto innerObject = JS_NewObject(ctx);
				JS_SetPropertyStr(ctx, innerObject, function_name.c_str(), JS_NewCFunction(ctx, func, function_name.c_str(), 0));
				auto middle4Object = JS_NewObject(ctx);
				JS_SetPropertyStr(ctx, middle4Object, obj6_name.c_str(), innerObject);
				auto middle3Object = JS_NewObject(ctx);
				JS_SetPropertyStr(ctx, middle3Object, obj5_name.c_str(), middle4Object);
				auto middle2Object = JS_NewObject(ctx);
				JS_SetPropertyStr(ctx, middle2Object, obj4_name.c_str(), middle3Object);
				auto middle1Object = JS_NewObject(ctx);
				JS_SetPropertyStr(ctx, middle1Object, obj3_name.c_str(), middle2Object);
				auto outerObject = JS_NewObject(ctx);
				JS_SetPropertyStr(ctx, outerObject, obj2_name.c_str(), middle1Object);
				auto global_obj = JS_GetGlobalObject(ctx);
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

			auto add_proxy(
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
				auto innerObject = JS_NewObject(ctx);
				JS_SetPropertyStr(ctx, innerObject, function_name.c_str(), JS_NewCFunction(ctx, func, function_name.c_str(), 0));
				auto middle5Object = JS_NewObject(ctx);
				JS_SetPropertyStr(ctx, middle5Object, obj7_name.c_str(), innerObject);
				auto middle4Object = JS_NewObject(ctx);
				JS_SetPropertyStr(ctx, middle4Object, obj6_name.c_str(), middle5Object);
				auto middle3Object = JS_NewObject(ctx);
				JS_SetPropertyStr(ctx, middle3Object, obj5_name.c_str(), middle4Object);
				auto middle2Object = JS_NewObject(ctx);
				JS_SetPropertyStr(ctx, middle2Object, obj4_name.c_str(), middle3Object);
				auto middle1Object = JS_NewObject(ctx);
				JS_SetPropertyStr(ctx, middle1Object, obj3_name.c_str(), middle2Object);
				auto outerObject = JS_NewObject(ctx);
				JS_SetPropertyStr(ctx, outerObject, obj2_name.c_str(), middle1Object);
				auto global_obj = JS_GetGlobalObject(ctx);
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

			auto add_proxy(
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
				auto innerObject = JS_NewObject(ctx);
				JS_SetPropertyStr(ctx, innerObject, function_name.c_str(), JS_NewCFunction(ctx, func, function_name.c_str(), 0));
				auto middle6Object = JS_NewObject(ctx);
				JS_SetPropertyStr(ctx, middle6Object, obj8_name.c_str(), innerObject);
				auto middle5Object = JS_NewObject(ctx);
				JS_SetPropertyStr(ctx, middle5Object, obj7_name.c_str(), middle6Object);
				auto middle4Object = JS_NewObject(ctx);
				JS_SetPropertyStr(ctx, middle4Object, obj6_name.c_str(), middle5Object);
				auto middle3Object = JS_NewObject(ctx);
				JS_SetPropertyStr(ctx, middle3Object, obj5_name.c_str(), middle4Object);
				auto middle2Object = JS_NewObject(ctx);
				JS_SetPropertyStr(ctx, middle2Object, obj4_name.c_str(), middle3Object);
				auto middle1Object = JS_NewObject(ctx);
				JS_SetPropertyStr(ctx, middle1Object, obj3_name.c_str(), middle2Object);
				auto outerObject = JS_NewObject(ctx);
				JS_SetPropertyStr(ctx, outerObject, obj2_name.c_str(), middle1Object);
				auto global_obj = JS_GetGlobalObject(ctx);
				JS_SetPropertyStr(ctx, global_obj, obj1_name.c_str(), outerObject);
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