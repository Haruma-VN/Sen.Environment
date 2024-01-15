#pragma once

#include <source_location>
#include <string>
#include <exception>

namespace Sen::Kernel {

	// assert: condition & message
	// if not condition, will throw message

	#define try_assert(conditional, message) \
	if (!(conditional)) { \
		throw Exception(message); \
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
		public:
			Exception() = default;
			Exception(const Exception &that) = default;
			Exception(Exception &&that) = default;
			Exception(const std::string &arg, const std::source_location &loc = std::source_location::current()) : std::runtime_error(arg)
			{
				{
					auto current_stack = std::string{loc.file_name()} + std::string{":"} + std::to_string(loc.line());
					#if _WIN32 // Windows using seperator '\\'
						std::replace(current_stack.begin(), current_stack.end(), '\\', '/');
					#endif
					msg = current_stack;
				}
				msg += arg;
				msg += '\n';
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
		catch(const Exception &ex){
			return ex;
		}
		catch(const std::exception &ex){
			return static_cast<Exception>(ex.what());
		}
		catch(...){
			return static_cast<Exception>("Undefined Exception caught");
		}
		return Exception{"Undefined exception"};
	}
}