project(
	kernel
	LANGUAGES CXX
)

add_library(
	${PROJECT_NAME} SHARED
	./definition/macro.hpp
	./definition/library.hpp
	./definition/assert.hpp
	./definition/filesystem/common.hpp
	./definition/string/common.hpp
	./definition/path/common.hpp
	./definition/pointer/common.hpp
	./definition/encryption/md5/common.hpp
	./definition/encryption/Rijndael/common.hpp
	./definition/encryption/sha224/common.hpp
	./definition/encryption/sha256/common.hpp
	./definition/encryption/base64/common.hpp
	./definition/encryption/XOR/common.hpp

	./dependencies/fmt.hpp
	./dependencies/json.hpp
	./dependencies/md5.hpp
	./dependencies/zlib.hpp
	./dependencies/SHA256.hpp
	./dependencies/Rijndael.hpp
	./dependencies/SHA224CryptoServiceProvider.hpp
	./dependencies/cpp-base64.hpp
	./dependencies/RectangleBinPack.hpp
	./dependencies/PVRTCCompressor.hpp
	./dependencies/libpng.hpp
	./dependencies/etcpak.hpp
	./dependencies/tinyxml2.hpp
	./dependencies/ETCPACK.hpp

	./support/popcap/resource_group/common.hpp
	./support/utility.hpp

	./interface/data.hpp
	./interface/cli.hpp
	./interface/callback.h
	./interface/callback.cpp
	./interface/interface.cpp
)


include_directories(${CMAKE_CURRENT_SOURCE_DIR}/dependencies/fmt)

set_target_properties(${PROJECT_NAME} PROPERTIES LINKER_LANGUAGE CXX)

target_include_directories(
	${PROJECT_NAME} PRIVATE
	..
)

set(MSVC_COMPILER  0)
set(CLANG_COMPILER 0)
if(CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
	set(MSVC_COMPILER 1)
endif()
if(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
	set(CLANG_COMPILER 1)
endif()


target_compile_definitions( 
    ${PROJECT_NAME} PRIVATE 
    _SILENCE_CXX17_CODECVT_HEADER_DEPRECATION_WARNING 
) 
  
target_compile_definitions( 
    ${PROJECT_NAME} PRIVATE 
    CONFIG_BIGNUM 
)

target_include_directories(
	${PROJECT_NAME} PRIVATE
	../dependencies/fmt
)


target_link_libraries(
	${PROJECT_NAME} PRIVATE
	md5
	zlib
	json
	fmt
	Rijndael
	SHA224CryptoServiceProvider
	SHA256
	cpp-base64
	RectangleBinPack
	PVRTCCompressor
	libpng
	etcpak
	tinyxml2
	ETCPACK
)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)