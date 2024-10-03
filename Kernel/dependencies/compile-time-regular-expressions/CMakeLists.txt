project(
	compile-time-regular-expressions
  	VERSION 3.9.0
	HOMEPAGE_URL https://github.com/hanickadot/compile-time-regular-expressions
	LANGUAGES CXX
)

add_library(
	${PROJECT_NAME} INTERFACE
	./ctre-unicode.hpp
	./ctre.hpp
	./unicode-db.hpp
	./ctll.hpp
	./unicode-db/unicode-db.hpp
	./unicode-db/unicode_interface.hpp
	./ctll/actions.hpp
	./ctll/fixed_string.hpp
	./ctll/grammars.hpp
	./ctll/list.hpp
	./ctll/parser.hpp
	./ctll/utilities.hpp
	./ctre/actions/asserts.inc.hpp
	./ctre/actions/atomic_group.inc.hpp
	./ctre/actions/backreference.inc.hpp
	./ctre/actions/boundaries.inc.hpp
	./ctre/actions/capture.inc.hpp
	./ctre/actions/characters.inc.hpp
	./ctre/actions/class.inc.hpp
	./ctre/actions/fusion.inc.hpp
	./ctre/actions/hexdec.inc.hpp
	./ctre/actions/look.inc.hpp
	./ctre/actions/mode.inc.hpp
	./ctre/actions/named_class.inc.hpp
	./ctre/actions/options.inc.hpp
	./ctre/actions/properties.inc.hpp
	./ctre/actions/repeat.inc.hpp
	./ctre/actions/sequence.inc.hpp
	./ctre/actions/set.inc.hpp
	./ctre/atoms_characters.hpp
	./ctre/atoms_unicode.hpp
	./ctre/atoms.hpp
	./ctre/evaluation.hpp
	./ctre/find_captures.hpp
	./ctre/first.hpp
	./ctre/flags_and_modes.hpp
	./ctre/functions.hpp
	./ctre/id.hpp
	./ctre/iterators.hpp
	./ctre/literals.hpp
	./ctre/operators.hpp
	./ctre/pcre_actions.hpp
	./ctre/pcre.hpp
	./ctre/range.hpp
	./ctre/return_type.hpp
	./ctre/rotate.hpp
	./ctre/starts_with_anchor.hpp
	./ctre/utf8.hpp
	./ctre/utility.hpp
	./ctre/wrapper.hpp
)



# Set Kind

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# UTF8 Support
if(MSVC)
  add_compile_options(/utf-8)
  add_compile_options("$<$<C_COMPILER_ID:MSVC>:/utf-8>")
  add_compile_options("$<$<CXX_COMPILER_ID:MSVC>:/utf-8>")
else()
  add_compile_options(-finput-charset=UTF-8)
endif()

if (MSVC)
    set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /Zi")
else ()
    set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g")
endif ()

set_target_properties(${PROJECT_NAME} PROPERTIES LINKER_LANGUAGE CXX)