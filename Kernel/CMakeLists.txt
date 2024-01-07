cmake_minimum_required (VERSION 3.20)

project (
  Kernel
  VERSION 1
  HOMEPAGE_URL https://github.com/Haruma-VN/Sen.Environment
  LANGUAGES CXX
)

add_definitions(-w)

if (WIN32)
add_compile_options(/w)
endif()


if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()

add_subdirectory("./dependencies/md5")
add_subdirectory("./dependencies/json")
add_subdirectory("./dependencies/zlib")
add_subdirectory("./dependencies/fmt")
add_subdirectory("./dependencies/Rijndael")
add_subdirectory("./dependencies/SHA224CryptoServiceProvider")
add_subdirectory("./dependencies/SHA256")
add_subdirectory("./dependencies/cpp-base64")
add_subdirectory("./dependencies/RectangleBinPack")
add_subdirectory("./dependencies/PVRTCCompressor")
add_subdirectory("./dependencies/libpng")
add_subdirectory("./dependencies/etcpak")
add_subdirectory("./dependencies/tinyxml2")
add_subdirectory("./dependencies/ETCPACK")
add_subdirectory("./dependencies/SHA")
add_subdirectory("./dependencies/lzma")
add_subdirectory("./dependencies/avir")
add_subdirectory("./dependencies/bzip2")
add_subdirectory("./dependencies/open_vcdiff")
add_subdirectory("./dependencies/zip")

if(UNIX OR APPLE OR ANDROID OR IOS)
    add_subdirectory("./dependencies/quickjs")
endif()

if(WIN32)
	add_subdirectory("./dependencies/quickjs.c_smile")
endif()

add_subdirectory("./kernel")

if(CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /Z7")
    set(CMAKE_EXE_LINKER_FLAGS_RELEASE "${CMAKE_EXE_LINKER_FLAGS_RELEASE} /DEBUG:NONE")
    set(CMAKE_SHARED_LINKER_FLAGS_RELEASE "${CMAKE_SHARED_LINKER_FLAGS_RELEASE} /NOIMPLIB")
    set(CMAKE_MODULE_LINKER_FLAGS_RELEASE "${CMAKE_MODULE_LINKER_FLAGS_RELEASE} /NOIMPLIB")
endif()

if(ANDROID)
  set (CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -stdlib=libc++ -lc++abi")
  set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++20 -stdlib=libc++")
  set_target_properties(${TARGET_NAME} PROPERTIES
    ANDROID_STL c++_shared
  )
endif()

# Symbol visibility
set(CMAKE_C_VISIBILITY_PRESET hidden)
set(CMAKE_CXX_VISIBILITY_PRESET hidden)

# Pos Independent
set(CMAKE_POSITION_INDEPENDENT_CODE true)

if (MSVC)
    set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /Zi")
else ()
    set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g")
endif ()

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
