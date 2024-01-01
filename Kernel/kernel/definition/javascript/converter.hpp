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
				else if constexpr (std::is_same<T, bool>::value){
					auto m_list = std::vector<bool>{};
					for (auto i : Range<int>(length)) {
						auto value = JS_GetPropertyUint32(context, that, i);
						m_list.push_back(JS::Converter::get_bool(context, value));
					}
					return m_list;
				}
				else if constexpr(std::is_same<T, long long>::value){
					auto m_list = std::vector<long long>{};
					for (auto i : Range<int>(length)) {
						auto val = JS_GetPropertyUint32(context, that, i);
						m_list.push_back(Converter::get_int64(context, val));
					}
					return m_list;
				}
				else if constexpr (std::is_same<T, uint32_t>::value){
					auto m_list = std::vector<unsigned int>{};
					for (auto i : Range<int>(length)) {
						auto val = JS_GetPropertyUint32(context, that, i);
						m_list.push_back(Converter::get_uint32(context, val));
					}
					return m_list;
				}
				else if constexpr (std::is_same<T, uint64_t>::value){
					auto m_list = std::vector<uint64_t>{};
					for (auto i : Range<int>(length)) {
						auto val = JS_GetPropertyUint32(context, that, i);
						m_list.push_back(Converter::get_uint64(context, val));
					}
					return m_list;
				}
				else {
					auto m_list = std::vector<JSValue>{};
					for (auto i : Range<int>(length)) {
						auto val = JS_GetPropertyUint32(context, that, i);
						m_list.push_back(val);
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
					m_list.push_back(Converter::get_string(context, val));
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
					JS_SetPropertyUint32(ctx, js_array, i, JS_NewUint32(ctx, vec[i]));
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
					JS_SetPropertyUint32(ctx, js_array, i, JS_NewString(ctx, vec[i].c_str()));
				}
				return js_array;
			}

			/**
			 * Read and return JS ArrayBuffer
			*/

			inline static auto read_file_as_js_arraybuffer(
				JSContext *ctx, 
				const std::string & source
			) -> JSValue
			{
				FILE *fp = fopen(source.c_str(), "rb");
				auto file_size = long{};
				if (fp == NULL) {
					throw std::runtime_error(fmt::format("File does not found: {}", source));
				}
				fseek(fp, 0, SEEK_END);
				file_size = ftell(fp);
				rewind(fp);
				auto buffer = (char*) malloc(file_size * sizeof(char));
				if (buffer == NULL) {
					fclose(fp);
					throw std::runtime_error(fmt::format("C malloc allocating memory failed, source file: {}", source));
				}
				auto result = fread(buffer, 1, file_size, fp);
				if (result != file_size) {
					free(buffer);
					fclose(fp);
					throw std::runtime_error(fmt::format("Read file {} failed", source));
				}
				auto array_buffer = JS_NewArrayBufferCopy(ctx, reinterpret_cast<uint8_t*>(buffer), file_size);
				free(buffer);
				fclose(fp);
				return array_buffer;
			}

			/**
			 * JS ArrayBuffer write to file
			*/

			inline static auto write_file_as_arraybuffer(
				JSContext *ctx, 
				const std::string & destination,
				const JSValue & that
			) -> void
			{
				auto size = size_t{};
				auto data = JS_GetArrayBuffer(ctx, &size, that);
				if(data == NULL){
					throw std::runtime_error(fmt::format("Failed when get ArrayBuffer from JavaScript"));
				}
				auto ofs = std::ofstream(destination, std::ios::binary | std::ios::out);
				if (!ofs) {
					throw std::runtime_error(fmt::format("File cannot be opened for write operation: {}", destination));
				}
				ofs.write(reinterpret_cast<const char*>(data), size);
				if (!ofs) {
					ofs.close();
					throw std::runtime_error(fmt::format("File cannot be written: {}", destination));
				}
				ofs.close();
				return;
			}
}