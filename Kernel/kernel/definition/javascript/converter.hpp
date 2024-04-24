#pragma once

#include "kernel/dependencies/quickjs.hpp"
#include "kernel/definition/macro.hpp"
#include "kernel/definition/library.hpp"
#include "kernel/definition/assert.hpp"

namespace Sen::Kernel::Definition::JavaScript::Converter {

	namespace JS = Sen::Kernel::Definition::JavaScript;

			/**
			 * JS String to C++ String
			*/
			inline static auto get_string(
				JSContext* context,
				const JSValue & that
			) -> std::string
			{
				auto size = std::size_t{};
				auto c_str = JS_ToCStringLen(context, &size, that);
				auto str = std::string { c_str, size };
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
					throw Exception(fmt::format("{}", Language::get("js.converter.failed_convert_number_to_int")), std::source_location::current(), "get_int32");
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
					throw Exception(fmt::format("{}", Language::get("js.converter.failed_convert_number_to_double")), std::source_location::current(), "get_float64");
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
					throw Exception(fmt::format("{}", Language::get("js.converter.failed_convert_number_to_float")), std::source_location::current(), "get_float32");
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
					throw Exception(fmt::format("{}", Language::get("js.converter.failed_convert_number_to_long_long")), std::source_location::current(), "get_int64");
				}
				return m_value;
			}

			/**
			 * JS Number to C++ Int64
			*/

			inline static auto get_bigint64(
				JSContext* context,
				const JSValue & that
			) -> int64_t
			{
				auto m_value = int64_t{};
				if(JS_ToBigInt64(context, &m_value, that) < 0){
					throw Exception(fmt::format("{}", Language::get("js.converter.failed_convert_bigint_to_long_long")), std::source_location::current(), "get_bigint64");
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
					throw Exception(fmt::format("{}", Language::get("js.converter.failed_convert_number_to_unsigned_int")), std::source_location::current(), "get_uint32");
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
					throw Exception(fmt::format("{}", Language::get("js.converter.failed_convert_number_to_unsigned_long_long")), std::source_location::current(), "get_uint64");
				}
				return m_value;
			}

			/**
			 * C++ String to JS String
			 * Because StringView is cheap
			*/

			inline static auto to_string(
				JSContext* context,
				std::string_view content
			) -> JSValue
			{
				return JS_NewStringLen(context, content.data(), content.size());
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
			 * C++ Float to JS Number
			*/

			inline static auto to_number(
				JSContext* context,
				float value
			) -> JSValue
			{
				return JS_NewFloat64(context, static_cast<double>(value));
			}

			/**
			 * C++ Int32 to JS Number
			*/

			inline static auto to_number(
				JSContext* context,
				double value
			) -> JSValue
			{
				return JS_NewFloat64(context, value);
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

			
			template <typename T> requires std::is_integral<T>::value
			inline static auto to_bigint(
				JSContext* context,
				T value
			) -> JSValue
			{
				if constexpr (std::is_same<T, std::uint64_t>::value) {
					return JS_NewBigUint64(context, value);
				}
				else {
					return JS_NewBigInt64(context, static_cast<int64_t>(value));
				}
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
						m_list.emplace_back(JS::Converter::get_int32(context, value));
					}
					return m_list;
				}
				else if constexpr (std::is_same<T, bool>::value){
					auto m_list = std::vector<bool>{};
					for (auto i : Range<int>(length)) {
						auto value = JS_GetPropertyUint32(context, that, i);
						m_list.emplace_back(JS::Converter::get_bool(context, value));
					}
					return m_list;
				}
				else if constexpr(std::is_same<T, long long>::value){
					auto m_list = std::vector<long long>{};
					for (auto i : Range<int>(length)) {
						auto val = JS_GetPropertyUint32(context, that, i);
						m_list.emplace_back(Converter::get_int64(context, val));
					}
					return m_list;
				}
				else if constexpr (std::is_same<T, uint32_t>::value){
					auto m_list = std::vector<unsigned int>{};
					for (auto i : Range<int>(length)) {
						auto val = JS_GetPropertyUint32(context, that, i);
						m_list.emplace_back(Converter::get_uint32(context, val));
					}
					return m_list;
				}
				else if constexpr (std::is_same<T, uint64_t>::value){
					auto m_list = std::vector<uint64_t>{};
					for (auto i : Range<int>(length)) {
						auto val = JS_GetPropertyUint32(context, that, i);
						m_list.emplace_back(Converter::get_uint64(context, val));
					}
					return m_list;
				}
				else if constexpr (std::is_same<T, float>::value){
					auto m_list = std::vector<float>{};
					for (auto i : Range<int>(length)) {
						auto val = JS_GetPropertyUint32(context, that, i);
						m_list.emplace_back(Converter::get_float32(context, val));
					}
					return m_list;
				}
				else if constexpr (std::is_same<T, double>::value){
					auto m_list = std::vector<double>{};
					for (auto i : Range<int>(length)) {
						auto val = JS_GetPropertyUint32(context, that, i);
						m_list.emplace_back(Converter::get_float64(context, val));
					}
					return m_list;
				}
				else {
					auto m_list = std::vector<JSValue>{};
					for (auto i : Range<int>(length)) {
						auto val = JS_GetPropertyUint32(context, that, i);
						m_list.emplace_back(val);
					}
					return m_list;
				}
			}

			/**
			 * Convert JS Array to C++ Vector
			*/

			template <typename T>
			inline static auto get_array(
				JSContext* context,
				const JSValue& that
			) -> std::vector<T>
			{
				auto length_value = JS_GetPropertyStr(context, that, "length");
				auto length = Converter::get_int32(context, length_value);
				JS_FreeValue(context, length_value);
				if constexpr (std::is_same<T, int>::value) {
					auto m_list = std::vector<int>{};
					for (auto i : Range<int>(length)) {
						auto value = JS_GetPropertyUint32(context, that, i);
						m_list.emplace_back(static_cast<T>(JS::Converter::get_bigint64(context, value)));
					}
					return m_list;
				}
				else if constexpr (std::is_same<T, bool>::value) {
					auto m_list = std::vector<bool>{};
					for (auto i : Range<int>(length)) {
						auto value = JS_GetPropertyUint32(context, that, i);
						m_list.emplace_back(JS::Converter::get_bool(context, value));
					}
					return m_list;
				}
				else if constexpr (std::is_same<T, long long>::value) {
					auto m_list = std::vector<long long>{};
					for (auto i : Range<int>(length)) {
						auto value = JS_GetPropertyUint32(context, that, i);
						m_list.emplace_back(static_cast<T>(JS::Converter::get_bigint64(context, value)));
					}
					return m_list;
				}
				else if constexpr (std::is_same<T, uint32_t>::value) {
					auto m_list = std::vector<unsigned int>{};
					for (auto i : Range<int>(length)) {
						auto value = JS_GetPropertyUint32(context, that, i);
						m_list.emplace_back(static_cast<T>(JS::Converter::get_bigint64(context, value)));
					}
					return m_list;
				}
				else if constexpr (std::is_same<T, uint64_t>::value) {
					auto m_list = std::vector<uint64_t>{};
					for (auto i : Range<int>(length)) {
						auto value = JS_GetPropertyUint32(context, that, i);
						m_list.emplace_back(static_cast<T>(JS::Converter::get_bigint64(context, value)));
					}
					return m_list;
				}
				else if constexpr (std::is_same<T, float>::value) {
					auto m_list = std::vector<float>{};
					for (auto i : Range<int>(length)) {
						auto value = JS_GetPropertyUint32(context, that, i);
						m_list.emplace_back(static_cast<T>(JS::Converter::get_float32(context, value)));
					}
					return m_list;
				}
				else if constexpr (std::is_same<T, double>::value) {
					auto m_list = std::vector<double>{};
					for (auto i : Range<int>(length)) {
						auto value = JS_GetPropertyUint32(context, that, i);
						m_list.emplace_back(static_cast<T>(JS::Converter::get_float64(context, value)));
					}
					return m_list;
				}
				else {
					auto m_list = std::vector<JSValue>{};
					for (auto i : Range(length)) {
						auto value = JS_GetPropertyUint32(context, that, i);
						m_list.emplace_back(value);
					}
					return m_list;
				}
			}

			/**
			 * Convert JS Array to C++ Vector of String
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
					m_list.emplace_back(Converter::get_string(context, val));
				}
				return m_list;
			}

			/**
			 * Convert JS Array to C++ Vector of String View
			*/

			template <>
			inline static auto get_vector(
				JSContext* context,
				const JSValue& that
			) -> std::vector<std::string_view>
			{
				auto len_val = JS_GetPropertyStr(context, that, "length");
				auto length = Converter::get_int32(context, len_val);
				JS_FreeValue(context, len_val);
				auto m_list = std::vector<std::string_view>{};
				for (auto i : Range<int>(length)) {
					auto val = JS_GetPropertyUint32(context, that, i);
					m_list.emplace_back(Converter::get_string(context, val));
				}
				return m_list;
			}

			/**
			 * Convert Vector to JS Array
			*/

			inline static auto to_array(
				JSContext *ctx, 
				const std::vector<int32_t> & vec
			) -> JSValue
			{
				auto js_array = JS_NewArray(ctx);
				for (auto i : Range<size_t>(vec.size())) {
					JS_SetPropertyUint32(ctx, js_array, i, JS_NewInt32(ctx, vec[i]));
				}
				return js_array;
			}

			/**
			 * Convert Vector to JS Array
			*/

			inline static auto to_array(
				JSContext *ctx, 
				const std::vector<uint32_t> & vec
			) -> JSValue
			{
				auto js_array = JS_NewArray(ctx);
				for (auto i : Range<size_t>(vec.size())) {
					JS_SetPropertyUint32(ctx, js_array, i, JS_NewBigInt64(ctx, static_cast<int64_t>(vec[i])));
				}
				return js_array;
			}

			/**
			 * Convert Vector to JS Array
			*/

			inline static auto to_array(
				JSContext* ctx,
				const std::vector<float>& vec
			) -> JSValue
			{
				auto js_array = JS_NewArray(ctx);
				for (auto i : Range<size_t>(vec.size())) {
					JS_SetPropertyUint32(ctx, js_array, i, JS_NewFloat64(ctx, static_cast<float>(vec[i])));
				}
				return js_array;
			}

			/**
			 * Convert Vector to JS Array
			*/

			inline static auto to_array(
				JSContext* ctx,
				const std::vector<double>& vec
			) -> JSValue
			{
				auto js_array = JS_NewArray(ctx);
				for (auto i : Range<size_t>(vec.size())) {
					JS_SetPropertyUint32(ctx, js_array, i, JS_NewFloat64(ctx, static_cast<double>(vec[i])));
				}
				return js_array;
			}

			/**
			 * Convert Vector to JS Array
			*/

			inline static auto to_array(
				JSContext *ctx, 
				const std::vector<uint64_t> & vec
			) -> JSValue
			{
				auto js_array = JS_NewArray(ctx);
				for (auto i : Range<size_t>(vec.size())) {
					JS_SetPropertyUint32(ctx, js_array, i, JS_NewBigUint64(ctx, vec[i]));
				}
				return js_array;
			}

			/**
			 * Convert Vector to JS Array
			*/

			inline static auto to_array(
				JSContext *ctx, 
				const std::vector<bool> & vec
			) -> JSValue
			{
				auto js_array = JS_NewArray(ctx);
				for (auto i : Range<size_t>(vec.size())) {
					JS_SetPropertyUint32(ctx, js_array, i, JS_NewBool(ctx, vec[i]));
				}
				return js_array;
			}

			/**
			 * Convert Vector to JS Array
			*/

			inline static auto to_array(
				JSContext *ctx, 
				const std::vector<long long> & vec
			) -> JSValue
			{
				auto js_array = JS_NewArray(ctx);
				for (auto i : Range<size_t>(vec.size())) {
					JS_SetPropertyUint32(ctx, js_array, i, JS_NewBigInt64(ctx, vec[i]));
				}
				return js_array;
			}

			/**
			 * Convert Vector to JS Array
			*/

			inline static auto to_array(
				JSContext *ctx, 
				const std::vector<std::string> & vec
			) -> JSValue
			{
				auto js_array = JS_NewArray(ctx);
				for (auto i : Range<size_t>(vec.size())) {
					JS_SetPropertyUint32(ctx, js_array, i, JS_NewStringLen(ctx, vec[i].data(), vec[i].size()));
				}
				return js_array;
			}

			inline static auto constexpr close_buffer = [](auto buffer)
			{ free(buffer); return; };

			/**
			 * Read and return JS ArrayBuffer
			*/

			inline static auto read_file_as_js_arraybuffer(
				JSContext *ctx, 
				std::string_view source
			) -> JSValue
			{
				#if WINDOWS
				auto fp = std::unique_ptr<FILE, decltype(Language::close_file)>(_wfopen(String::utf8_to_utf16(source.data()).data(), L"rb"), Language::close_file);
				#else
				auto fp = std::unique_ptr<FILE, decltype(Language::close_file)>(std::fopen(source.data(), "rb"), Language::close_file);
				#endif
				if (fp == nullptr) {
					throw Exception(fmt::format("{}: {}", Language::get("cannot_read_file"), source), std::source_location::current(), "read_file_as_js_arraybuffer");
				}
				#if WINDOWS
				auto file_size = std::filesystem::file_size(std::filesystem::path{ String::utf8_to_utf16(source.data()) });
				#else
				auto file_size = std::filesystem::file_size(std::filesystem::path{ source });
				#endif
				auto buffer = std::unique_ptr<char[], decltype(close_buffer)>((char*) std::malloc(file_size * sizeof(char)), close_buffer);
				if (buffer == nullptr) {
					throw Exception(fmt::format("C malloc allocating memory failed, source file: {}", source), std::source_location::current(), "read_file_as_js_arraybuffer");
				}
				auto result = std::fread(buffer.get(), 1, file_size, fp.get());
				if (result != file_size) {
					throw Exception(fmt::format("{}: {}", Language::get("cannot_read_file"), source), std::source_location::current(), "read_file_as_js_arraybuffer");
				}
				auto array_buffer = JS_NewArrayBufferCopy(ctx, reinterpret_cast<uint8_t*>(buffer.get()), file_size);
				return array_buffer;
			}

			/**
			 * JS ArrayBuffer write to file
			*/

			inline static auto write_file_as_arraybuffer(
				JSContext* ctx,
				std::string_view destination,
				const JSValue& that
			) -> void
			{
				auto size = size_t{};
				auto data = JS_GetArrayBuffer(ctx, &size, that);
				if (data == nullptr) {
					throw Exception(fmt::format("{}", Language::get("js.converter.failed_to_get_js_array_buffer")), std::source_location::current(), "write_file_as_arraybuffer");
				}
				#if WINDOWS
					auto ofs = std::unique_ptr<FILE, decltype(Language::close_file)>(_wfopen(String::utf8_to_utf16(fmt::format("\\\\?\\{}", String::to_windows_style(destination.data()))).data(), L"wb"), Language::close_file);
				#else
					auto ofs = std::unique_ptr<FILE, decltype(Language::close_file)>(std::fopen(destination.data(), "wb"), Language::close_file);
				#endif
				if (ofs.get() == nullptr) {
					throw Exception(fmt::format("{}", Language::get("open_write_failed"), destination), std::source_location::current(), "write_file_as_arraybuffer");
				}
				std::fwrite(reinterpret_cast<char*>(data), 1, size, ofs.get());
				return;
			}

			// JS ArrayBuffer to C++ uint8_t vector

			inline static auto to_binary_list(
				JSContext* context,
				JSValue array_buffer
			) -> std::vector<uint8_t>
			{
				auto byte_len = std::size_t{};
				auto data = JS_GetArrayBuffer(context, &byte_len, array_buffer);
				return std::vector<std::uint8_t>(data, data + byte_len);
			}

			// C++ vector to JS ArrayBuffer

			inline static auto toArrayBuffer(
				JSContext* ctx,
				const std::vector<uint8_t> & v
			) -> JSValue
			{
				return JS_NewArrayBufferCopy(ctx, v.data(), v.size());
			}


			/**
			 * Convert JSValue to std::map
			*/

			inline static auto get_map(
				JSContext *ctx, 
				JSValueConst val
			) -> std::map<std::string, std::string>
			{
				auto result = std::map<std::string, std::string>{};
				if (JS_IsObject(val)) {
					auto tab = static_cast<JSPropertyEnum *>(nullptr);
					auto tab_len = uint32_t{};
					if (JS_GetOwnPropertyNames(ctx, &tab, &tab_len, val, JS_GPN_STRING_MASK | JS_GPN_ENUM_ONLY) == 0) {
						for (auto i : Range<uint32_t>(tab_len)) {
							auto atom = tab[i].atom;
							auto prop_val = JS_GetProperty(ctx, val, atom);
							if (JS_IsString(prop_val)) {
								auto prop_key = JS_AtomToCString(ctx, atom);
								auto str_len = std::strlen(prop_key);
								auto prop_str_val = JS_ToCStringLen(ctx, &str_len, prop_val);
								if (prop_key && prop_str_val) {
									result[prop_key] = prop_str_val;
								}
								JS_FreeCString(ctx, prop_key);
								JS_FreeCString(ctx, prop_str_val);
							}
							JS_FreeValue(ctx, prop_val);
							JS_FreeAtom(ctx, atom);
						}
						js_free(ctx, tab);
					}
				}
				return result;
			}
}