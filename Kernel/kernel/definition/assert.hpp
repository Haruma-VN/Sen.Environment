#pragma once

#include <source_location>
#include <string>
#include <exception>

namespace Sen::Kernel {

	// assert: condition & message
	// if not condition, will throw message

	#define try_assert(conditional, message) \
	if (!(conditional)) { \
		throw Exception(message, std::source_location::current()); \
	}

	#define assert_conditional(conditional, message, function_name)\
	if (!(conditional)) { \
		throw Exception(message, std::source_location::current(), function_name); \
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


	class Exception : public std::runtime_error {
	std::string msg;
	std::string arg;

	public:
		std::string function_name;
		std::string source;
		Exception() = default;
		Exception(const Exception &that) = default;
		Exception(Exception &&that) = default;

		inline auto message(

		) -> std::string
		{
			return this->arg;
		}

		Exception(const std::string &arg, const std::source_location &loc = std::source_location::current(), const std::string & function_name = "") : std::runtime_error(arg), source(std::string{loc.file_name()} + std::string{":"} + std::to_string(loc.line())), arg(arg), function_name(function_name)
		{
			{
					auto current_stack = source;
					#if _WIN32 // Windows using seperator '\\'
						std::replace(current_stack.begin(), current_stack.end(), '\\', '/');
					#endif
					msg = current_stack;
				}
				msg += '\n';
				msg += arg;
				msg += ' ';
			}
			~Exception () throw () {}
			const char *what () const throw () {
				return this->msg.c_str();
			}
	};

	inline static auto parse_exception(

	) -> Exception
	{
		try {
			std::rethrow_exception(std::current_exception());
		}
		catch(Exception &ex){
			return Exception{ ex };
		}
		catch (std::system_error& ex) {
			return static_cast<Exception>(ex.what());
		}
		catch(const std::exception &ex){
			return static_cast<Exception>(ex.what());
		}
		catch(...){
			return static_cast<Exception>("Undefined exception caught");
		}
		return Exception{"Undefined exception"};
	}
}