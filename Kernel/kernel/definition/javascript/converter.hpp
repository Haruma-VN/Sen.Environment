#pragma once

#include "kernel/dependencies/quickjs.hpp"
#include "kernel/definition/macro.hpp"
#include "kernel/definition/library.hpp"
#include "kernel/definition/assert.hpp"

namespace Sen::Kernel::Definition::JavaScript { 

	/**
	 * Convert Quick JS value to C++ Value
	*/

	struct Converter {
		
		public:

			/**
			 * Constructor
			*/

			Converter(

			) = default;

			/**
			 * Destructor
			*/

			~Converter(

			) = default;

			/**
			 * JS String to C++ String
			*/

			static auto to_string(
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
			 * Int32 converter
			*/

			static auto to_int32(
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
			 * Int64 converter
			*/

			static auto to_int64(
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
			 * Uint32 converter
			*/

			static auto to_uint32(
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
			 * Uint64 converter
			*/

			static auto to_uint64(
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
			 * To Boolean
			*/

			static auto to_bool(
				JSContext* context,
				const JSValue & that
			) -> bool
			{
				return static_cast<bool>(JS_ToBool(context, that));
			}

			/**
			 * Convert JS Array to C++ Vector
			*/

			template <typename T>
			static auto to_vector(
				JSContext* context,
				const JSValue & that
			) -> std::vector<T>
			{
				auto len_val = JS_GetPropertyStr(context, that, "length");
				auto length = Converter::to_int32(context, len_val);
				JS_FreeValue(context, len_val);
				if (std::is_same<T, std::string>::value){
					auto m_list = std::vector<std::string>{};
					for (auto i : Range<int>(length)) {
						auto val = JS_GetPropertyUint32(context, that, i);
						m_list.push_back(Converter::to_string(context, val));
					}
					return m_list;
				}
				else if(std::is_same<T, int>::value){
					auto m_list = std::vector<int>{};
					for (auto i : Range<int>(length)) {
						auto val = JS_GetPropertyUint32(context, that, i);
						m_list.push_back(Converter::to_int32(context, val));
					}
					return m_list;
				}
				else if(std::is_same<T, bool>::value){
					auto m_list = std::vector<bool>{};
					for (auto i : Range<int>(length)) {
						auto val = JS_GetPropertyUint32(context, that, i);
						m_list.push_back(Converter::to_bool(context, val));
					}
					return m_list;
				}
				else if(std::is_same<T, long long>::value){
					auto m_list = std::vector<long long>{};
					for (auto i : Range<int>(length)) {
						auto val = JS_GetPropertyUint32(context, that, i);
						m_list.push_back(Converter::to_int64(context, val));
					}
					return m_list;
				}
				else if(std::is_same<T, uint32_t>::value){
					auto m_list = std::vector<unsigned int>{};
					for (auto i : Range<int>(length)) {
						auto val = JS_GetPropertyUint32(context, that, i);
						m_list.push_back(Converter::to_uint32(context, val));
					}
					return m_list;
				}
				else if(std::is_same<T, uint64_t>::value){
					auto m_list = std::vector<uint64_t>{};
					for (auto i : Range<int>(length)) {
						auto val = JS_GetPropertyUint32(context, that, i);
						m_list.push_back(Converter::to_uint64(context, val));
					}
					return m_list;
				}
				else{
					auto m_list = std::vector<JSValue>{};
					for (auto i : Range<int>(length)) {
						auto val = JS_GetPropertyUint32(context, that, i);
						m_list.push_back(val);
					}
					return m_list;
				}
			}
	};
}