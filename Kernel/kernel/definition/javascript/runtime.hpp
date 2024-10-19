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

			inline static auto constexpr free_runtime = [](JSRuntime *rt)
			{
				if (rt != nullptr) {
            		JS_RunGC(rt);
					JS_FreeRuntime(rt);
				}
				return;
			};

			inline static auto constexpr free_context = [](JSContext* ctx) {
				if (ctx != nullptr) {
					JS_FreeContext(ctx);
				}
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
			 * Mutex to protect from local thread
			 */

			std::mutex mutex;

			/**
			 * Check if current JS is module to load
			 */


			bool is_module;

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
				auto atom = JS_NewAtomLen(ctx.get(), name.data(), name.size());\
				JS_DefinePropertyValue(ctx.get(), parent, atom, obj, int{JS_PROP_C_W_E}); \
				JS_FreeAtom(ctx.get(), atom);\
			}

			#define M_GET_GLOBAL_OBJECT()\
			auto global_obj = JS_GetGlobalObject(ctx.get());

			#define M_FREE_GLOBAL_OBJECT()\
			JS_FreeValue(ctx.get(), global_obj);


			/**
			 * JS Handler Constructor
			*/

			explicit Runtime(

			) : mutex{}, is_module{false}
			{

			}

			/**
			 * JS Exception
			*/
			inline auto exception(
			) -> std::string
			{
				auto result = std::string{};
				auto exception = JS_GetException(thiz.ctx.get());
				auto size = std::size_t{};
				auto exception_stack = JS_ToCStringLen(thiz.ctx.get(), &size, exception);
				result += std::string{exception_stack, size};
				if(JS_IsError(thiz.ctx.get(), exception)){
					auto atom = JS_NewAtomLen(thiz.ctx.get(), "stack", 5);
					auto js_stack = JS_GetProperty(thiz.ctx.get(), exception, atom);
					JS_FreeAtom(thiz.ctx.get(), atom);
					if (JS::not_undefined(js_stack))
					{
						auto res_size = std::size_t{};
						auto js_exception = JS_ToCStringLen(thiz.ctx.get(), &res_size, js_stack);
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
			 * Use this to clean up an instance of JS Object
			*/


			inline auto unregister_object(
				std::function<void (JSContext*)> callback
			) -> void
			{
				callback(thiz.ctx.get());
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

			inline auto get_property(
				std::string_view object_name
			) -> JSValue
			{
				M_GET_GLOBAL_OBJECT();
				auto atom = JS_NewAtomLen(ctx.get(), object_name.data(), object_name.size());
				auto val = JS_GetProperty(ctx.get(), global_obj, atom);
				JS_FreeAtom(ctx.get(), atom);
				M_FREE_GLOBAL_OBJECT();
				return val;
			}

			/**
			 * Check if a Promise still remain
			*/

			inline auto has_promise(
			) -> bool
			{
				return static_cast<bool>(JS_IsJobPending(thiz.rt.get()));
			}

			inline static auto custom_module_loader(
				JSContext* ctx,
				char const* module_name,
				void* opaque
			) -> JSModuleDef*
			{
				auto source = std::string{module_name, std::strlen(module_name)};
				auto file = FileSystem::read_file(source);
				auto module_val = JS_Eval(ctx, file.data(), file.size(), module_name, JS_EVAL_TYPE_MODULE | JS_EVAL_FLAG_COMPILE_ONLY | JS_EVAL_FLAG_STRICT);
				if (JS_IsException(module_val)) {
					JS_FreeValue(ctx, module_val);
					JS_ThrowInternalError(ctx, "Cannot read module: %s", module_name);
					return nullptr;
				}
				auto module_def = static_cast<JSModuleDef *>(JS_VALUE_GET_PTR(module_val));
				JS_FreeValue(ctx, module_val);
				return module_def;
			}

			inline auto enable_module_load (
			) -> void
			{
				thiz.is_module = true;
				JS_SetModuleLoaderFunc(thiz.rt.get(), nullptr, &custom_module_loader, nullptr);
				return;
			}

			inline auto disable_module_load (
			) -> void
			{
				thiz.is_module = false;
				JS_SetModuleLoaderFunc(thiz.rt.get(), nullptr, nullptr, nullptr);
				return;
			}

			/**
			 * Check if a Promise still remain
			*/

			inline auto execute_pending_job(
			) -> void
			{
				auto job_ctx = static_cast<JSContext *>(nullptr);
				auto count = JS_ExecutePendingJob(thiz.rt.get(), &job_ctx);
				assert_conditional(count == 0, "There is no pending job on the list", "execute_pending_job");
				if (count < 0) {
					throw Exception{thiz.exception()};
				}
				return;
			}

			inline auto constexpr evaluate_flag(

			) -> int
			{
				return thiz.is_module ? JS_EVAL_TYPE_MODULE : JS_EVAL_TYPE_GLOBAL;
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
				thiz.mutex.lock();
				auto eval_result = JS_Eval(thiz.ctx.get(), source_data.data(), source_data.size(), source_file.data(), JS_EVAL_FLAG_STRICT | thiz.evaluate_flag());
				if(JS_IsException(eval_result)){
					throw Exception(thiz.exception(), std::source_location::current(), "evaluate");
				}
				thiz.mutex.unlock();
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
				auto atom = JS_NewAtomLen(ctx.get(), function_name.data(), function_name.size());
				JS_DefinePropertyValue(ctx.get(), global_obj, atom, JS_NewCFunction(ctx.get(), func, function_name.data(), 0), int{JS_PROP_C_W_E});
				JS_FreeAtom(ctx.get(), atom);
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
				auto obj_atom = JS_NewAtomLen(ctx.get(), object_name.data(), object_name.size());
				auto myObject = JS_GetProperty(ctx.get(), global_obj, obj_atom);
				JS_FreeAtom(ctx.get(), obj_atom);
				if (JS_IsUndefined(myObject)) {
					myObject = JS_NewObject(ctx.get());
				}
				auto func_atom = JS_NewAtomLen(ctx.get(), function_name.data(), function_name.size());
				JS_DefinePropertyValue(ctx.get(), myObject, func_atom, 
					JS_NewCFunction(ctx.get(), func, function_name.data(), 0), int{JS_PROP_C_W_E});
				JS_FreeAtom(ctx.get(), func_atom);
				obj_atom = JS_NewAtomLen(ctx.get(), object_name.data(), object_name.size());
				JS_DefinePropertyValue(ctx.get(), global_obj, obj_atom, myObject, int{JS_PROP_C_W_E});
				JS_FreeAtom(ctx.get(), obj_atom);
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
				auto obj1_atom = JS_NewAtomLen(ctx.get(), obj1_name.data(), obj1_name.size());
				auto outerObject = JS_GetProperty(ctx.get(), global_obj, obj1_atom);
				JS_FreeAtom(ctx.get(), obj1_atom); 
				if (JS_IsUndefined(outerObject)) {
					outerObject = JS_NewObject(ctx.get());
				}
				auto obj2_atom = JS_NewAtomLen(ctx.get(), obj2_name.data(), obj2_name.size());
				auto innerObject = JS_GetProperty(ctx.get(), outerObject, obj2_atom);
				JS_FreeAtom(ctx.get(), obj2_atom); 
				if (JS_IsUndefined(innerObject)) {
					innerObject = JS_NewObject(ctx.get());
				}
				auto func_atom = JS_NewAtomLen(ctx.get(), function_name.data(), function_name.size());
				JS_DefinePropertyValue(ctx.get(), innerObject, func_atom, 
					JS_NewCFunction(ctx.get(), func, function_name.data(), 0), int{JS_PROP_C_W_E});
				JS_FreeAtom(ctx.get(), func_atom);
				obj2_atom = JS_NewAtomLen(ctx.get(), obj2_name.data(), obj2_name.size());
				JS_DefinePropertyValue(ctx.get(), outerObject, obj2_atom, innerObject, int{JS_PROP_C_W_E});
				JS_FreeAtom(ctx.get(), obj2_atom); 
				obj1_atom = JS_NewAtomLen(ctx.get(), obj1_name.data(), obj1_name.size());
				JS_DefinePropertyValue(ctx.get(), global_obj, obj1_atom, outerObject, int{JS_PROP_C_W_E});
				JS_FreeAtom(ctx.get(), obj1_atom);
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
				auto obj1_atom = JS_NewAtomLen(ctx.get(), obj1_name.data(), obj1_name.size());
				auto outerObject = JS_GetProperty(ctx.get(), global_obj, obj1_atom);
				JS_FreeAtom(ctx.get(), obj1_atom); 
				if (JS_IsUndefined(outerObject)) {
					outerObject = JS_NewObject(ctx.get());
				}
				auto obj2_atom = JS_NewAtomLen(ctx.get(), obj2_name.data(), obj2_name.size());
				auto middleObject = JS_GetProperty(ctx.get(), outerObject, obj2_atom);
				JS_FreeAtom(ctx.get(), obj2_atom);
				if (JS_IsUndefined(middleObject)) {
					middleObject = JS_NewObject(ctx.get());
				}
				auto obj3_atom = JS_NewAtomLen(ctx.get(), obj3_name.data(), obj3_name.size());
				auto innerObject = JS_GetProperty(ctx.get(), middleObject, obj3_atom);
				JS_FreeAtom(ctx.get(), obj3_atom);
				if (JS_IsUndefined(innerObject)) {
					innerObject = JS_NewObject(ctx.get());
				}
				auto func_atom = JS_NewAtomLen(ctx.get(), function_name.data(), function_name.size());
				JS_DefinePropertyValue(ctx.get(), innerObject, func_atom, 
					JS_NewCFunction(ctx.get(), func, function_name.data(), 0), int{JS_PROP_C_W_E});
				JS_FreeAtom(ctx.get(), func_atom);
				obj3_atom = JS_NewAtomLen(ctx.get(), obj3_name.data(), obj3_name.size());
				JS_DefinePropertyValue(ctx.get(), middleObject, obj3_atom, innerObject, int{JS_PROP_C_W_E});
				JS_FreeAtom(ctx.get(), obj3_atom);
				obj2_atom = JS_NewAtomLen(ctx.get(), obj2_name.data(), obj2_name.size());
				JS_DefinePropertyValue(ctx.get(), outerObject, obj2_atom, middleObject, int{JS_PROP_C_W_E});
				JS_FreeAtom(ctx.get(), obj2_atom); 
				obj1_atom = JS_NewAtomLen(ctx.get(), obj1_name.data(), obj1_name.size());
				JS_DefinePropertyValue(ctx.get(), global_obj, obj1_atom, outerObject, int{JS_PROP_C_W_E});
				JS_FreeAtom(ctx.get(), obj1_atom);
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
				auto obj1_atom = JS_NewAtomLen(ctx.get(), obj1_name.data(), obj1_name.size());
				auto outerObject = JS_GetProperty(ctx.get(), global_obj, obj1_atom);
				JS_FreeAtom(ctx.get(), obj1_atom); 
				if (JS_IsUndefined(outerObject)) {
					outerObject = JS_NewObject(ctx.get());
				}
				auto obj2_atom = JS_NewAtomLen(ctx.get(), obj2_name.data(), obj2_name.size());
				auto middle1Object = JS_GetProperty(ctx.get(), outerObject, obj2_atom);
				JS_FreeAtom(ctx.get(), obj2_atom); 
				if (JS_IsUndefined(middle1Object)) {
					middle1Object = JS_NewObject(ctx.get());
				}
				auto obj3_atom = JS_NewAtomLen(ctx.get(), obj3_name.data(), obj3_name.size());
				auto middle2Object = JS_GetProperty(ctx.get(), middle1Object, obj3_atom);
				JS_FreeAtom(ctx.get(), obj3_atom);
				if (JS_IsUndefined(middle2Object)) {
					middle2Object = JS_NewObject(ctx.get());
				}
				auto obj4_atom = JS_NewAtomLen(ctx.get(), obj4_name.data(), obj4_name.size());
				auto innerObject = JS_GetProperty(ctx.get(), middle2Object, obj4_atom);
				JS_FreeAtom(ctx.get(), obj4_atom);
				if (JS_IsUndefined(innerObject)) {
					innerObject = JS_NewObject(ctx.get());
				}
				auto func_atom = JS_NewAtomLen(ctx.get(), function_name.data(), function_name.size());
				JS_DefinePropertyValue(ctx.get(), innerObject, func_atom, 
					JS_NewCFunction(ctx.get(), func, function_name.data(), 0), int{JS_PROP_C_W_E});
				JS_FreeAtom(ctx.get(), func_atom);
				obj4_atom = JS_NewAtomLen(ctx.get(), obj4_name.data(), obj4_name.size());
				JS_DefinePropertyValue(ctx.get(), middle2Object, obj4_atom, innerObject, int{JS_PROP_C_W_E});
				JS_FreeAtom(ctx.get(), obj4_atom);
				obj3_atom = JS_NewAtomLen(ctx.get(), obj3_name.data(), obj3_name.size());
				JS_DefinePropertyValue(ctx.get(), middle1Object, obj3_atom, middle2Object, int{JS_PROP_C_W_E});
				JS_FreeAtom(ctx.get(), obj3_atom);
				obj2_atom = JS_NewAtomLen(ctx.get(), obj2_name.data(), obj2_name.size());
				JS_DefinePropertyValue(ctx.get(), outerObject, obj2_atom, middle1Object, int{JS_PROP_C_W_E});
				JS_FreeAtom(ctx.get(), obj2_atom);
				obj1_atom = JS_NewAtomLen(ctx.get(), obj1_name.data(), obj1_name.size());
				JS_DefinePropertyValue(ctx.get(), global_obj, obj1_atom, outerObject, int{JS_PROP_C_W_E});
				JS_FreeAtom(ctx.get(), obj1_atom);
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
				auto obj1_atom = JS_NewAtomLen(ctx.get(), obj1_name.data(), obj1_name.size());
				auto outerObject = JS_GetProperty(ctx.get(), global_obj, obj1_atom);
				JS_FreeAtom(ctx.get(), obj1_atom);
				if (JS_IsUndefined(outerObject)) {
					outerObject = JS_NewObject(ctx.get());
				}
				auto obj2_atom = JS_NewAtomLen(ctx.get(), obj2_name.data(), obj2_name.size());
				auto middle1Object = JS_GetProperty(ctx.get(), outerObject, obj2_atom);
				JS_FreeAtom(ctx.get(), obj2_atom);
				if (JS_IsUndefined(middle1Object)) {
					middle1Object = JS_NewObject(ctx.get());
				}
				auto obj3_atom = JS_NewAtomLen(ctx.get(), obj3_name.data(), obj3_name.size());
				auto middle2Object = JS_GetProperty(ctx.get(), middle1Object, obj3_atom);
				JS_FreeAtom(ctx.get(), obj3_atom);
				if (JS_IsUndefined(middle2Object)) {
					middle2Object = JS_NewObject(ctx.get());
				}
				auto obj4_atom = JS_NewAtomLen(ctx.get(), obj4_name.data(), obj4_name.size());
				auto middle3Object = JS_GetProperty(ctx.get(), middle2Object, obj4_atom);
				JS_FreeAtom(ctx.get(), obj4_atom);
				if (JS_IsUndefined(middle3Object)) {
					middle3Object = JS_NewObject(ctx.get());
				}
				auto obj5_atom = JS_NewAtomLen(ctx.get(), obj5_name.data(), obj5_name.size());
				auto innerObject = JS_GetProperty(ctx.get(), middle3Object, obj5_atom);
				JS_FreeAtom(ctx.get(), obj5_atom); 
				if (JS_IsUndefined(innerObject)) {
					innerObject = JS_NewObject(ctx.get());
				}
				auto func_atom = JS_NewAtomLen(ctx.get(), function_name.data(), function_name.size());
				JS_DefinePropertyValue(ctx.get(), innerObject, func_atom, 
					JS_NewCFunction(ctx.get(), func, function_name.data(), 0), int{JS_PROP_C_W_E});
				JS_FreeAtom(ctx.get(), func_atom); 
				obj5_atom = JS_NewAtomLen(ctx.get(), obj5_name.data(), obj5_name.size());
				JS_DefinePropertyValue(ctx.get(), middle3Object, obj5_atom, innerObject, int{JS_PROP_C_W_E});
				JS_FreeAtom(ctx.get(), obj5_atom); 
				obj4_atom = JS_NewAtomLen(ctx.get(), obj4_name.data(), obj4_name.size());
				JS_DefinePropertyValue(ctx.get(), middle2Object, obj4_atom, middle3Object, int{JS_PROP_C_W_E});
				JS_FreeAtom(ctx.get(), obj4_atom);
				obj3_atom = JS_NewAtomLen(ctx.get(), obj3_name.data(), obj3_name.size());
				JS_DefinePropertyValue(ctx.get(), middle1Object, obj3_atom, middle2Object, int{JS_PROP_C_W_E});
				JS_FreeAtom(ctx.get(), obj3_atom);
				obj2_atom = JS_NewAtomLen(ctx.get(), obj2_name.data(), obj2_name.size());
				JS_DefinePropertyValue(ctx.get(), outerObject, obj2_atom, middle1Object, int{JS_PROP_C_W_E});
				JS_FreeAtom(ctx.get(), obj2_atom);
				obj1_atom = JS_NewAtomLen(ctx.get(), obj1_name.data(), obj1_name.size());
				JS_DefinePropertyValue(ctx.get(), global_obj, obj1_atom, outerObject, int{JS_PROP_C_W_E});
				JS_FreeAtom(ctx.get(), obj1_atom);
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
				auto obj1_atom = JS_NewAtomLen(ctx.get(), obj1_name.data(), obj1_name.size());
				auto outerObject = JS_GetProperty(ctx.get(), global_obj, obj1_atom);
				JS_FreeAtom(ctx.get(), obj1_atom);
				if (JS_IsUndefined(outerObject)) {
					outerObject = JS_NewObject(ctx.get());
				}
				auto obj2_atom = JS_NewAtomLen(ctx.get(), obj2_name.data(), obj2_name.size());
				auto middle1Object = JS_GetProperty(ctx.get(), outerObject, obj2_atom);
				JS_FreeAtom(ctx.get(), obj2_atom);
				if (JS_IsUndefined(middle1Object)) {
					middle1Object = JS_NewObject(ctx.get());
				}
				auto obj3_atom = JS_NewAtomLen(ctx.get(), obj3_name.data(), obj3_name.size());
				auto middle2Object = JS_GetProperty(ctx.get(), middle1Object, obj3_atom);
				JS_FreeAtom(ctx.get(), obj3_atom);
				if (JS_IsUndefined(middle2Object)) {
					middle2Object = JS_NewObject(ctx.get());
				}
				auto obj4_atom = JS_NewAtomLen(ctx.get(), obj4_name.data(), obj4_name.size());
				auto middle3Object = JS_GetProperty(ctx.get(), middle2Object, obj4_atom);
				JS_FreeAtom(ctx.get(), obj4_atom);
				if (JS_IsUndefined(middle3Object)) {
					middle3Object = JS_NewObject(ctx.get());
				}
				auto obj5_atom = JS_NewAtomLen(ctx.get(), obj5_name.data(), obj5_name.size());
				auto middle4Object = JS_GetProperty(ctx.get(), middle3Object, obj5_atom);
				JS_FreeAtom(ctx.get(), obj5_atom);
				if (JS_IsUndefined(middle4Object)) {
					middle4Object = JS_NewObject(ctx.get());
				}
				auto obj6_atom = JS_NewAtomLen(ctx.get(), obj6_name.data(), obj6_name.size());
				auto innerObject = JS_GetProperty(ctx.get(), middle4Object, obj6_atom);
				JS_FreeAtom(ctx.get(), obj6_atom);
				if (JS_IsUndefined(innerObject)) {
					innerObject = JS_NewObject(ctx.get());
				}
				auto func_atom = JS_NewAtomLen(ctx.get(), function_name.data(), function_name.size());
				JS_DefinePropertyValue(ctx.get(), innerObject, func_atom, 
					JS_NewCFunction(ctx.get(), func, function_name.data(), 0), int{JS_PROP_C_W_E});
				JS_FreeAtom(ctx.get(), func_atom);
				obj6_atom = JS_NewAtomLen(ctx.get(), obj6_name.data(), obj6_name.size());
				JS_DefinePropertyValue(ctx.get(), middle4Object, obj6_atom, innerObject, int{JS_PROP_C_W_E});
				JS_FreeAtom(ctx.get(), obj6_atom);
				obj5_atom = JS_NewAtomLen(ctx.get(), obj5_name.data(), obj5_name.size());
				JS_DefinePropertyValue(ctx.get(), middle3Object, obj5_atom, middle4Object, int{JS_PROP_C_W_E});
				JS_FreeAtom(ctx.get(), obj5_atom);
				obj4_atom = JS_NewAtomLen(ctx.get(), obj4_name.data(), obj4_name.size());
				JS_DefinePropertyValue(ctx.get(), middle2Object, obj4_atom, middle3Object, int{JS_PROP_C_W_E});
				JS_FreeAtom(ctx.get(), obj4_atom);
				obj3_atom = JS_NewAtomLen(ctx.get(), obj3_name.data(), obj3_name.size());
				JS_DefinePropertyValue(ctx.get(), middle1Object, obj3_atom, middle2Object, int{JS_PROP_C_W_E});
				JS_FreeAtom(ctx.get(), obj3_atom);
				obj2_atom = JS_NewAtomLen(ctx.get(), obj2_name.data(), obj2_name.size());
				JS_DefinePropertyValue(ctx.get(), outerObject, obj2_atom, middle1Object, int{JS_PROP_C_W_E});
				JS_FreeAtom(ctx.get(), obj2_atom);
				obj1_atom = JS_NewAtomLen(ctx.get(), obj1_name.data(), obj1_name.size());
				JS_DefinePropertyValue(ctx.get(), global_obj, obj1_atom, outerObject, int{JS_PROP_C_W_E});
				JS_FreeAtom(ctx.get(), obj1_atom); 
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
				auto obj1_atom = JS_NewAtomLen(ctx.get(), obj1_name.data(), obj1_name.size());
				auto outerObject = JS_GetProperty(ctx.get(), global_obj, obj1_atom);
				JS_FreeAtom(ctx.get(), obj1_atom);
				if (JS_IsUndefined(outerObject)) {
					outerObject = JS_NewObject(ctx.get());
				}
				auto obj2_atom = JS_NewAtomLen(ctx.get(), obj2_name.data(), obj2_name.size());
				auto middle1Object = JS_GetProperty(ctx.get(), outerObject, obj2_atom);
				JS_FreeAtom(ctx.get(), obj2_atom);
				if (JS_IsUndefined(middle1Object)) {
					middle1Object = JS_NewObject(ctx.get());
				}
				auto obj3_atom = JS_NewAtomLen(ctx.get(), obj3_name.data(), obj3_name.size());
				auto middle2Object = JS_GetProperty(ctx.get(), middle1Object, obj3_atom);
				JS_FreeAtom(ctx.get(), obj3_atom);
				if (JS_IsUndefined(middle2Object)) {
					middle2Object = JS_NewObject(ctx.get());
				}
				auto obj4_atom = JS_NewAtomLen(ctx.get(), obj4_name.data(), obj4_name.size());
				auto middle3Object = JS_GetProperty(ctx.get(), middle2Object, obj4_atom);
				JS_FreeAtom(ctx.get(), obj4_atom);
				if (JS_IsUndefined(middle3Object)) {
					middle3Object = JS_NewObject(ctx.get());
				}
				auto obj5_atom = JS_NewAtomLen(ctx.get(), obj5_name.data(), obj5_name.size());
				auto middle4Object = JS_GetProperty(ctx.get(), middle3Object, obj5_atom);
				JS_FreeAtom(ctx.get(), obj5_atom);
				if (JS_IsUndefined(middle4Object)) {
					middle4Object = JS_NewObject(ctx.get());
				}
				auto obj6_atom = JS_NewAtomLen(ctx.get(), obj6_name.data(), obj6_name.size());
				auto middle5Object = JS_GetProperty(ctx.get(), middle4Object, obj6_atom);
				JS_FreeAtom(ctx.get(), obj6_atom);
				if (JS_IsUndefined(middle5Object)) {
					middle5Object = JS_NewObject(ctx.get());
				}
				auto obj7_atom = JS_NewAtomLen(ctx.get(), obj7_name.data(), obj7_name.size());
				auto innerObject = JS_GetProperty(ctx.get(), middle5Object, obj7_atom);
				JS_FreeAtom(ctx.get(), obj7_atom);
				if (JS_IsUndefined(innerObject)) {
					innerObject = JS_NewObject(ctx.get());
				}
				auto func_atom = JS_NewAtomLen(ctx.get(), function_name.data(), function_name.size());
				JS_DefinePropertyValue(ctx.get(), innerObject, func_atom, 
					JS_NewCFunction(ctx.get(), func, function_name.data(), 0), int{JS_PROP_C_W_E});
				JS_FreeAtom(ctx.get(), func_atom);
				obj7_atom = JS_NewAtomLen(ctx.get(), obj7_name.data(), obj7_name.size());
				JS_DefinePropertyValue(ctx.get(), middle5Object, obj7_atom, innerObject, int{JS_PROP_C_W_E});
				JS_FreeAtom(ctx.get(), obj7_atom);
				obj6_atom = JS_NewAtomLen(ctx.get(), obj6_name.data(), obj6_name.size());
				JS_DefinePropertyValue(ctx.get(), middle4Object, obj6_atom, middle5Object, int{JS_PROP_C_W_E});
				JS_FreeAtom(ctx.get(), obj6_atom);
				obj5_atom = JS_NewAtomLen(ctx.get(), obj5_name.data(), obj5_name.size());
				JS_DefinePropertyValue(ctx.get(), middle3Object, obj5_atom, middle4Object, int{JS_PROP_C_W_E});
				JS_FreeAtom(ctx.get(), obj5_atom);
				obj4_atom = JS_NewAtomLen(ctx.get(), obj4_name.data(), obj4_name.size());
				JS_DefinePropertyValue(ctx.get(), middle2Object, obj4_atom, middle3Object, int{JS_PROP_C_W_E});
				JS_FreeAtom(ctx.get(), obj4_atom);
				obj3_atom = JS_NewAtomLen(ctx.get(), obj3_name.data(), obj3_name.size());
				JS_DefinePropertyValue(ctx.get(), middle1Object, obj3_atom, middle2Object, int{JS_PROP_C_W_E});
				JS_FreeAtom(ctx.get(), obj3_atom);
				obj2_atom = JS_NewAtomLen(ctx.get(), obj2_name.data(), obj2_name.size());
				JS_DefinePropertyValue(ctx.get(), outerObject, obj2_atom, middle1Object, int{JS_PROP_C_W_E});
				JS_FreeAtom(ctx.get(), obj2_atom);
				obj1_atom = JS_NewAtomLen(ctx.get(), obj1_name.data(), obj1_name.size());
				JS_DefinePropertyValue(ctx.get(), global_obj, obj1_atom, outerObject, int{JS_PROP_C_W_E});
				JS_FreeAtom(ctx.get(), obj1_atom);
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
				auto atom_obj1_name = JS_NewAtomLen(ctx.get(), obj1_name.data(), obj1_name.size());
				auto atom_obj2_name = JS_NewAtomLen(ctx.get(), obj2_name.data(), obj2_name.size());
				auto atom_obj3_name = JS_NewAtomLen(ctx.get(), obj3_name.data(), obj3_name.size());
				auto atom_obj4_name = JS_NewAtomLen(ctx.get(), obj4_name.data(), obj4_name.size());
				auto atom_obj5_name = JS_NewAtomLen(ctx.get(), obj5_name.data(), obj5_name.size());
				auto atom_obj6_name = JS_NewAtomLen(ctx.get(), obj6_name.data(), obj6_name.size());
				auto atom_obj7_name = JS_NewAtomLen(ctx.get(), obj7_name.data(), obj7_name.size());
				auto atom_obj8_name = JS_NewAtomLen(ctx.get(), obj8_name.data(), obj8_name.size());
				auto atom_function_name = JS_NewAtomLen(ctx.get(), function_name.data(), function_name.size());
				auto outerObject = JS_GetProperty(ctx.get(), global_obj, atom_obj1_name);
				if (JS_IsUndefined(outerObject)) {
					outerObject = JS_NewObject(ctx.get());
				}
				auto middle1Object = JS_GetProperty(ctx.get(), outerObject, atom_obj2_name);
				if (JS_IsUndefined(middle1Object)) {
					middle1Object = JS_NewObject(ctx.get());
				}
				auto middle2Object = JS_GetProperty(ctx.get(), middle1Object, atom_obj3_name);
				if (JS_IsUndefined(middle2Object)) {
					middle2Object = JS_NewObject(ctx.get());
				}
				auto middle3Object = JS_GetProperty(ctx.get(), middle2Object, atom_obj4_name);
				if (JS_IsUndefined(middle3Object)) {
					middle3Object = JS_NewObject(ctx.get());
				}
				auto middle4Object = JS_GetProperty(ctx.get(), middle3Object, atom_obj5_name);
				if (JS_IsUndefined(middle4Object)) {
					middle4Object = JS_NewObject(ctx.get());
				}
				auto middle5Object = JS_GetProperty(ctx.get(), middle4Object, atom_obj6_name);
				if (JS_IsUndefined(middle5Object)) {
					middle5Object = JS_NewObject(ctx.get());
				}
				auto middle6Object = JS_GetProperty(ctx.get(), middle5Object, atom_obj7_name);
				if (JS_IsUndefined(middle6Object)) {
					middle6Object = JS_NewObject(ctx.get());
				}
				auto innerObject = JS_GetProperty(ctx.get(), middle6Object, atom_obj8_name);
				if (JS_IsUndefined(innerObject)) {
					innerObject = JS_NewObject(ctx.get());
				}
				JS_DefinePropertyValue(ctx.get(), innerObject, atom_function_name, JS_NewCFunction(ctx.get(), func, function_name.data(), 0), JS_PROP_C_W_E);
				JS_DefinePropertyValue(ctx.get(), middle6Object, atom_obj8_name, innerObject, JS_PROP_C_W_E);
				JS_DefinePropertyValue(ctx.get(), middle5Object, atom_obj7_name, middle6Object, JS_PROP_C_W_E);
				JS_DefinePropertyValue(ctx.get(), middle4Object, atom_obj6_name, middle5Object, JS_PROP_C_W_E);
				JS_DefinePropertyValue(ctx.get(), middle3Object, atom_obj5_name, middle4Object, JS_PROP_C_W_E);
				JS_DefinePropertyValue(ctx.get(), middle2Object, atom_obj4_name, middle3Object, JS_PROP_C_W_E);
				JS_DefinePropertyValue(ctx.get(), middle1Object, atom_obj3_name, middle2Object, JS_PROP_C_W_E);
				JS_DefinePropertyValue(ctx.get(), outerObject, atom_obj2_name, middle1Object, JS_PROP_C_W_E);
				JS_DefinePropertyValue(ctx.get(), global_obj, atom_obj1_name, outerObject, JS_PROP_C_W_E);
				JS_FreeAtom(ctx.get(), atom_obj1_name);
				JS_FreeAtom(ctx.get(), atom_obj2_name);
				JS_FreeAtom(ctx.get(), atom_obj3_name);
				JS_FreeAtom(ctx.get(), atom_obj4_name);
				JS_FreeAtom(ctx.get(), atom_obj5_name);
				JS_FreeAtom(ctx.get(), atom_obj6_name);
				JS_FreeAtom(ctx.get(), atom_obj7_name);
				JS_FreeAtom(ctx.get(), atom_obj8_name);
				JS_FreeAtom(ctx.get(), atom_function_name);
				M_FREE_GLOBAL_OBJECT();
				return;
			}

			/**
			 * Destructor
			 */

			~Runtime(

			) 
			{
				if (thiz.ctx != nullptr)
				{
					JS_RunGC(thiz.rt.get());
				}
				thiz.ctx.reset();
				thiz.rt.reset();
			}
	};
}