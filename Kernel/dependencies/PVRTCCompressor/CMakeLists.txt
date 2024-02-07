cmake_minimum_required (VERSION 3.20)
# PVRTCCompressor Third, but adapted from TwinStar ToolKit for the fix

project(
	PVRTCCompressor
	HOMEPAGE_URL https://github.com/brenwill/PVRTCCompressor
	LANGUAGES C CXX
)

if (MSVC)
    set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /Zi")
else ()
    set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g")
endif ()

add_library(
	${PROJECT_NAME} STATIC
	./AlphaBitmap.h
	./Bitmap.h
	./BitScale.h
	./BitUtility.h
	./ColorRgba.h
	./Interval.h
	./MortonTable.h
	./Point2.h
	./PvrTcDecoder.h
	./PvrTcEncoder.h
	./PvrTcPacket.h
	./RgbaBitmap.h
	./RgbBitmap.h
	./BitScale.cpp
	./MortonTable.cpp
	./PvrTcDecoder.cpp
	./PvrTcEncoder.cpp
	./PvrTcPacket.cpp
	./main.cpp
)

# UTF8 Support
if(MSVC)
  add_compile_options(/utf-8)
  add_compile_options("$<$<C_COMPILER_ID:MSVC>:/utf-8>")
  add_compile_options("$<$<CXX_COMPILER_ID:MSVC>:/utf-8>")
else()
  add_compile_options(-finput-charset=UTF-8)
endif()

set_target_properties(PVRTCCompressor PROPERTIES POSITION_INDEPENDENT_CODE ON)
