project(
	fmt
	VERSION 3.11.3
	HOMEPAGE_URL https://github.com/fmtlib/fmt
	LANGUAGES CXX
)

add_library(${PROJECT_NAME} INTERFACE)

target_sources(${PROJECT_NAME} INTERFACE
	./include/fmt/args.h
	./include/fmt/chrono.h
	./include/fmt/color.h
	./include/fmt/compile.h
	./include/fmt/core.h
	./include/fmt/format-inl.h
	./include/fmt/format.h
	./include/fmt/os.h
	./include/fmt/ostream.h
	./include/fmt/printf.h
	./include/fmt/ranges.h
	./include/fmt/std.h
	./include/fmt/xchar.h
)

target_compile_definitions(${PROJECT_NAME} INTERFACE FMT_HEADER_ONLY)


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