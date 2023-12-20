#pragma once

#include "kernel/dependencies/quickjs.hpp"

namespace Sen::Kernel::JavaScript {

	/**
	 * JS Runtime Native Handler
	*/

	struct Runtime {
		private:
			JSContext* ctx;
			JSRuntime *rt;

			auto free_value(
				const JSValue & that
			) -> void
			{
				JS_FreeValue(thiz.ctx, that);
				return;
			}

			auto free_string(
				const char* that
			) -> void
			{
				JS_FreeCString(thiz.ctx, that);
				return;
			}

		public:
			explicit Runtime(

			) : rt(JS_NewRuntime()), ctx(JS_NewContext(thiz.rt)) 
			{

			}

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

			auto execute_fs(
				const std::string & source
			) -> void
			{
				auto js_data = FileSystem::readFile(source);
				auto eval_result = JS_Eval(thiz.ctx, js_data.c_str(), js_data.size(), Path::getFileName(source).c_str(), JS_EVAL_TYPE_GLOBAL);
				if(JS_IsException(eval_result)){
					thiz.exception(eval_result);
					return;
				}
				return;
			}

			~Runtime(

			) 
			{
				JS_FreeContext(thiz.ctx);
				JS_FreeRuntime(thiz.rt);
			}
	};
}