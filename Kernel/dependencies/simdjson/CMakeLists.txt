cmake_minimum_required(VERSION 3.14)

project(
    simdjson
    # The version number is modified by tools/release.py
    VERSION 3.6.3
    DESCRIPTION "Parsing gigabytes of JSON per second"
    HOMEPAGE_URL "https://simdjson.org/"
    LANGUAGES CXX
)

add_library(
	${PROJECT_NAME} STATIC
	./simdjson.h
	./simdjson.cpp
)

if (MSVC)
    set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /Zi")
else ()
    set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g")
endif ()

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

set_target_properties(${PROJECT_NAME} PROPERTIES POSITION_INDEPENDENT_CODE ON)

