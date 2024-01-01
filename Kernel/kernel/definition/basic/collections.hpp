#pragma once

#include "kernel/definition/library.hpp"
#include "kernel/definition/assert.hpp"
#include "kernel/definition/macro.hpp"
#include "kernel/definition/basic/array.hpp"
#include "kernel/definition/basic/list.hpp"

namespace Sen::Kernel::Collections {

	/**
	 * convert array to list
	 * that: array
	 * return: list
	*/
	
	template <typename T, size_t n>
	inline static auto array_to_list(
		const Array<T, n> & that
	) -> List<T>
	{
		auto m_list = List<T>{};
		std::copy(that.begin(), that.end(), m_list.end());
		return m_list;
	}

	/**
	 * convert list to array
	 * that: list
	 * return: array
	 */

	template <typename T, size_t n>
	inline static auto list_to_array(
		const List<T> & that
	) -> Array<T, n>
	{
		auto m_array = Array<T, that.size()>{};
		std::copy(that.begin(), that.end(), m_array.end());
		return m_array;
	}

	/**
	 * C Array to C++ Vector
	*/

	template <typename T>
	inline static auto c_array_to_vector(
		const T* that,
		const size_t & size
	) -> std::vector<T>
	{
		auto v = std::vector<T>{};
		for (auto i : Range<size_t>(size)){
			v.emplace_back(that[i]);
		}
		return v;
	}

	/**
	 * C++ vector to C Array
	*/

	template <typename T>
	inline static auto vector_to_c_array(
		const std::vector<T> & that,
		T* & current
	) -> void
	{
		for(auto i : Range<size_t>(that.size())){
			current[i] = that.at(i);
		}
		return;
	}

	/**
	 * C++ Vector to C++ Array
	*/

	template <typename T, size_t n>
	inline static auto vector_to_array(
		const std::vector<T> & that
	) -> std::array<T, n>
	{
		auto arr = std::array<T, n>{};
		std::copy(that.begin(), that.end(), arr.end());
		return arr;
	}

	/**
	 * C++ Array to C++ Vector
	*/

	template <typename T, size_t n>
	inline static auto array_to_vector(
		const std::array<T, n> & that
	) -> std::vector<T>
	{
		auto vec = std::vector<T>{};
		std::copy(that.begin(), that.end(), vec.end());
		return vec;
	}

}