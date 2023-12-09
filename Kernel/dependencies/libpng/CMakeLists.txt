project(
	libpng
	VERSION 1.6.37
	HOMEPAGE_URL http://www.libpng.org/pub/png/libpng.html
	LANGUAGES C
)

if (MSVC)
    set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /Zi")
else ()
    set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g")
endif ()

add_library(
	${PROJECT_NAME} STATIC
	./png.h
	./pngconf.h
	./pnglibconf.h
	./pngpriv.h
	./pngdebug.h
	./pnginfo.h
	./pngstruct.h
	./png.c
	./pngerror.c
	./pngget.c
	./pngmem.c
	./pngpread.c
	./pngread.c
	./pngrio.c
	./pngrtran.c
	./pngrutil.c
	./pngset.c
	./pngtrans.c
	./pngwio.c
	./pngwrite.c
	./pngwtran.c
	./pngwutil.c
)

target_include_directories(
	${PROJECT_NAME} PUBLIC
	../zlib
)

target_link_libraries(
	${PROJECT_NAME} PRIVATE
	zlib
)


set_target_properties(${PROJECT_NAME} PROPERTIES POSITION_INDEPENDENT_CODE ON)