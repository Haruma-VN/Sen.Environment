#pragma once

#include "kernel/dependencies/quickjs.hpp"
#include "kernel/definition/macro.hpp"
#include "kernel/definition/library.hpp"
#include "kernel/definition/assert.hpp"

namespace Sen::Kernel::Definition::JavaScript {

	namespace JS = Sen::Kernel::Definition::JavaScript;

	/**
	 * Convert Quick JS value to C++ Value
	*/

	struct Converter {
		
		public:

			/**
			 * Constructor
			*/

			explicit Converter(

			) = default;

			/**
			 * Destructor
			*/

			~Converter(

			) = default;

			/**
			 * JS String to C++ String
			*/

			inline static auto get_string(
				JSContext* context,
				const JSValue & that
			) -> std::string
			{
				auto c_str = JS_ToCString(context, that);
				auto str = std::string{c_str};
				JS_FreeCString(context, c_str);
				return str;
			}

			/**
			 * JS Number to C++ Int32
			*/

			inline static auto get_int32(
				JSContext* context,
				const JSValue & that
			) -> int32_t
			{
				auto m_value = int32_t{};
				if(JS_ToInt32(context, &m_value, that) < 0){
					throw std::runtime_error(fmt::format("Failed when converting JS number to int"));
				}
				return m_value;
			}

			/**
			 * JS Number to C++ Float64
			*/

			inline static auto get_float64(
				JSContext* context,
				const JSValue & that
			) -> double
			{
				auto m_value = double{};
				if(JS_ToFloat64(context, &m_value, that) < 0){
					throw std::runtime_error(fmt::format("Failed when converting JS number to double"));
				}
				return m_value;
			}

			/**
			 * Should not use this
			 * JS Number to C++ Float32
			*/

			inline static auto get_float32(
				JSContext* context,
				const JSValue & that
			) -> float
			{
				auto m_value = double{};
				if(JS_ToFloat64(context, &m_value, that) < 0){
					throw std::runtime_error(fmt::format("Failed when converting JS number to float"));
				}
				return static_cast<float>(m_value);
			}

			/**
			 * JS Number to C++ Int64
			*/

			inline static auto get_int64(
				JSContext* context,
				const JSValue & that
			) -> int64_t
			{
				auto m_value = int64_t{};
				if(JS_ToInt64(context, &m_value, that) < 0){
					throw std::runtime_error(fmt::format("Failed when converting JS number to long long"));
				}
				return m_value;
			}

			/**
			 * JS Number to C++ Uint32
			*/

			inline static auto get_uint32(
				JSContext* context,
				const JSValue & that
			) -> uint32_t
			{
				auto m_value = uint32_t{};
				if(JS_ToUint32(context, &m_value, that) < 0){
					throw std::runtime_error(fmt::format("Failed when converting JS number to C++ unsigned int"));
				}
				return m_value;
			}

			/**
			 * JS Number to C++ Uint64
			*/

			inline static auto get_uint64(
				JSContext* context,
				const JSValue & that
			) -> uint64_t
			{
				auto m_value = uint64_t{};
				if(JS_ToIndex(context, &m_value, that) < 0){
					throw std::runtime_error(fmt::format("Failed when converting JS number to C++ unsigned long long"));
				}
				return m_value;
			}

			/**
			 * C++ String to JS String
			*/

			inline static auto to_string(
				JSContext* context,
				const std::string & content
			) -> JSValue
			{
				return JS_NewStringLen(context, content.c_str(), content.size());
			}

			/**
			 * C++ Bool to JS Bool
			*/

			inline static auto to_bool(
				JSContext* context,
				bool value
			) -> JSValue
			{
				return JS_NewBool(context, value ? 1 : 0);
			}

			/**
			 * C++ Int32 to JS Number
			*/

			inline static auto to_number(
				JSContext* context,
				int32_t value
			) -> JSValue
			{
				return JS_NewInt32(context, value);
			}

			/**
			 * C++ Int64 to JS Number
			*/

			inline static auto to_number(
				JSContext* context,
				int64_t value
			) -> JSValue
			{
				return JS_NewInt64(context, value);
			}

			/**
			 * C++ Uint32 to JS Number
			*/

			inline static auto to_number(
				JSContext* context,
				uint32_t value
			) -> JSValue
			{
				return JS_NewUint32(context, value);
			}

			/**
			 * C++ Uint64 to JS Number
			*/

			inline static auto to_number(
				JSContext* context,
				uint64_t value
			) -> JSValue
			{
				return JS_NewBigUint64(context, value);
			}

			/**
			 * JS Number to C++ boolean
			*/

			inline static auto get_bool(
				JSContext* context,
				const JSValue & that
			) -> bool
			{
				return static_cast<bool>(JS_ToBool(context, that));
			}

			/**
			 * Return JS Undefined
			*/

			inline static auto constexpr get_undefined(

			) -> JSValue
			{
				return JS_UNDEFINED;
			}

			/**
			 * Return JS Null
			*/

			inline static auto constexpr get_null(

			) -> JSValue
			{
				return JS_NULL;
			}

			/**
			 * Convert JS Array to C++ Vector
			*/

			template <typename T>
			inline static auto get_vector(
				JSContext* context,
				const JSValue & that
			) -> std::vector<T>
			{
				auto length_value = JS_GetPropertyStr(context, that, "length");
				auto length = Converter::get_int32(context, length_value);
				JS_FreeValue(context, length_value);
				if constexpr (std::is_same<T, int>::value){
					auto m_list = std::vector<int>{};
					for (auto i : Range<int>(length)) {
						auto value = JS_GetPropertyUint32(context, that, i);
						m_list.push_back(JS::Converter::get_int32(context, value));
					}
					return m_list;
				}
				if constexpr (std::is_same<T, bool>::value){
					auto m_list = std::vector<bool>{};
					for (auto i : Range<int>(length)) {
						auto value = JS_GetPropertyUint32(context, that, i);
						m_list.push_back(JS::Converter::get_bool(context, value));
					}
					return m_list;
				}
				if constexpr(std::is_same<T, long long>::value){
					auto m_list = std::vector<long long>{};
					for (auto i : Range<int>(length)) {
						auto val = JS_GetPropertyUint32(context, that, i);
						m_list.push_back(Converter::get_int64(context, val));
					}
					return m_list;
				}
				if constexpr (std::is_same<T, uint32_t>::value){
					auto m_list = std::vector<unsigned int>{};
					for (auto i : Range<int>(length)) {
						auto val = JS_GetPropertyUint32(context, that, i);
						m_list.push_back(Converter::get_uint32(context, val));
					}
					return m_list;
				}
				if constexpr (std::is_same<T, uint64_t>::value){
					auto m_list = std::vector<uint64_t>{};
					for (auto i : Range<int>(length)) {
						auto val = JS_GetPropertyUint32(context, that, i);
						m_list.push_back(Converter::get_uint64(context, val));
					}
					return m_list;
				}
				auto m_list = std::vector<JSValue>{};
				for (auto i : Range<int>(length)) {
					auto val = JS_GetPropertyUint32(context, that, i);
					m_list.push_back(val);
				}
				return m_list;
			}

			/**
			 * Convert JS Array to C++ Vector
			*/

			template <>
			inline static auto get_vector(
				JSContext* context,
				const JSValue & that
			) -> std::vector<std::string>
			{
				auto len_val = JS_GetPropertyStr(context, that, "length");
				auto length = Converter::get_int32(context, len_val);
				JS_FreeValue(context, len_val);
				auto m_list = std::vector<std::string>{};
				for (auto i : Range<int>(length)) {
					auto val = JS_GetPropertyUint32(context, that, i);
					m_list.push_back(Converter::get_string(context, val));
				}
				return m_list;
			}

			/**
			 * Convert Vector to JS Array
			*/

			template <typename T>
			inline static auto to_array(
				JSContext *ctx, 
				const std::vector<T> & vec
			) -> JSValue
			{
				auto js_array = JS_NewArray(ctx);
				if constexpr (std::is_same_v<T, int32_t>::value){
					for (auto i : Range<size_t>(vec.size())) {
						JS_SetPropertyUint32(ctx, js_array, i, JS_NewInt32(ctx, vec[i]));
					}
				}
				if constexpr (std::is_same_v<T, uint32_t>::value){
					for (auto i : Range<size_t>(vec.size())) {
						JS_SetPropertyUint32(ctx, js_array, i, JS_NewUint32(ctx, vec[i]));
					}
				}
				if constexpr (std::is_same_v<T, uint64_t>::value){
					for (auto i : Range<size_t>(vec.size())) {
						JS_SetPropertyUint32(ctx, js_array, i, JS_NewBigUint64(ctx, vec[i]));
					}
				}
				if constexpr (std::is_same_v<T, bool>::value){
					for (auto i : Range<size_t>(vec.size())) {
						JS_SetPropertyUint32(ctx, js_array, i, JS_NewBool(ctx, vec[i]));
					}
				}
				if constexpr (std::is_same_v<T, long long>::value){
					for (auto i : Range<size_t>(vec.size())) {
						JS_SetPropertyUint32(ctx, js_array, i, JS_NewBigInt64(ctx, vec[i]));
					}
				}
				if constexpr (std::is_same_v<T, unsigned long long>::value){
					for (auto i : Range<size_t>(vec.size())) {
						JS_SetPropertyUint32(ctx, js_array, i, JS_NewBigUint64(ctx, vec[i]));
					}
				}
				return js_array;
			}

			/**
			 * Convert Vector to JS Array
			*/

			template <>
			inline static auto to_array<std::string>(
				JSContext *ctx, 
				const std::vector<std::string> & vec
			) -> JSValue
			{
				auto js_array = JS_NewArray(ctx);
				for (auto i : Range<size_t>(vec.size())) {
					JS_SetPropertyUint32(ctx, js_array, i, JS_NewString(ctx, vec[i].c_str()));
				}
				return js_array;
			}
	};
}