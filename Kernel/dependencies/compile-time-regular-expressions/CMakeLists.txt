project(
	compile-time-regular-expressions
  	VERSION 3.8.1
	HOMEPAGE_URL https://github.com/hanickadot/compile-time-regular-expressions
	LANGUAGES CXX
)

add_library(
	${PROJECT_NAME} INTERFACE
	./ctre-unicode.hpp
	./ctre.hpp
	./ctre-unicode.hpp
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