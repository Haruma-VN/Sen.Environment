#pragma once

#include "kernel/definition/macro.hpp"
#include "kernel/definition/library.hpp"

namespace Sen::Kernel {

	// using without namespace

	using std::runtime_error;

	// string is used

	using std::string;

	// assert: condition & message
	// if not condition, will throw message

	#define try_assert(conditional, message) \
	if (!(conditional)) { \
		throw std::runtime_error(message); \
	}

	/**
	 * Object: nlohmann object
	 * property: json property
	*/

	#define is_not_null_object(object, property) \
		object.find(property) != object.end()

	/**
	 * Object: nlohmann object
	 * property: json property
	*/

	#define is_null_object(object, property) \
		object.find(property) == object.end()
}