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

}