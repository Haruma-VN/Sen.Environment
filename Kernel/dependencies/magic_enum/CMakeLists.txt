# Adapted third from TwinStar ToolKit Source

project(
	magic_enum
	VERSION 0.9.6
	HOMEPAGE_URL https://github.com/Neargye/magic_enum
	LANGUAGES CXX
)

add_library(
	${PROJECT_NAME} STATIC
	./magic_enum_all.hpp
	./magic_enum_containers.hpp
	./magic_enum_flags.hpp
	./magic_enum_format.hpp
	./magic_enum_iostream.hpp
	./magic_enum_switch.hpp
	./magic_enum_utility.hpp
	./magic_enum.hpp
)

target_include_directories(
	${PROJECT_NAME} PUBLIC
	../fmt
)

target_link_libraries(
	${PROJECT_NAME} PRIVATE
	fmt
)

if (MSVC)
    set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /Zi")
else ()
    set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g")
endif ()

# Set Kind

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_20)

# UTF8 Support
if(MSVC)
  add_compile_options(/utf-8)
  add_compile_options("$<$<C_COMPILER_ID:MSVC>:/utf-8>")
  add_compile_options("$<$<CXX_COMPILER_ID:MSVC>:/utf-8>")
else()
  add_compile_options(-finput-charset=UTF-8)
endif()

set_target_properties(${PROJECT_NAME} PROPERTIES POSITION_INDEPENDENT_CODE ON)
