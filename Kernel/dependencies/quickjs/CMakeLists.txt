cmake_minimum_required(VERSION 3.7)

project(
	quickjs
	VERSION 0 # none
	HOMEPAGE_URL https://github.com/bellard/quickjs
	LANGUAGES C
)

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
	./unicode_gen_def.h
	./cutils.c
	./libbf.c
	./libregexp.c
	./libunicode.c
	./quickjs.c
	./unicode_gen.c
)

target_compile_definitions(
	${PROJECT_NAME} PRIVATE
	CONFIG_BIGNUM
	CONFIG_VERSION="2021-03-27"
)
