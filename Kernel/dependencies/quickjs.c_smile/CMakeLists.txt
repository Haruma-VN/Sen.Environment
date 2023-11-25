project(
	quickjs
	VERSION 0 # none
	HOMEPAGE_URL https://github.com/c-smile/quickjspp
	LANGUAGES C
)

if(MSVC)
    add_compile_options(/W0)
elseif(CMAKE_COMPILER_IS_GNUCC OR CMAKE_COMPILER_IS_GNUCXX OR "${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
    add_compile_options(-w)
endif()


add_library(
	${PROJECT_NAME} STATIC
	./cutils.h
	./libbf.h
	./libregexp.h
	./libregexp-opcode.h
	./libunicode.h
	./libunicode-table.h
	./list.h
	./quickjs.h
	./quickjs-atom.h
	./quickjs-opcode.h
	./quickjs-version.h
	./unicode_gen_def.h
	./win/dirent.h
	./cutils.c
	./libbf.c
	./libregexp.c
	./libunicode.c
	./quickjs.c
	./unicode_gen.c
)

target_compile_definitions(
	${PROJECT_NAME} PRIVATE
	JS_STRICT_NAN_BOXING
	CONFIG_BIGNUM
)
target_compile_options(
	${PROJECT_NAME} PRIVATE
	/Oi-
)
