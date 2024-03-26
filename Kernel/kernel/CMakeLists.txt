project(
	kernel
	LANGUAGES CXX
)

add_library(
	${PROJECT_NAME} SHARED
	./definition/macro.hpp
	./definition/range.hpp
	./definition/library.hpp
	./definition/literal.hpp
	./definition/assert.hpp
	./definition/utility.hpp
	./definition/basic/filesystem.hpp
	./definition/basic/string.hpp
	./definition/basic/math.hpp
	./definition/basic/language.hpp
	./definition/basic/path.hpp
	./definition/basic/pointer.hpp
	./definition/basic/timer.hpp
	./definition/basic/image.hpp
	./definition/basic/json.hpp
	./definition/basic/buffer.hpp
	./definition/basic/array.hpp
	./definition/basic/list.hpp
	./definition/basic/system.hpp
	./definition/basic/thread.hpp
	./definition/basic/collections.hpp
	./definition/basic/json_writer.hpp
	./definition/basic/apng_maker.hpp

	./definition/encryption/md5.hpp
	./definition/encryption/Rijndael.hpp
	./definition/encryption/sha224.hpp
	./definition/encryption/sha256.hpp
	./definition/encryption/base64.hpp
	./definition/encryption/XOR.hpp
	./definition/encryption/crc32.hpp
	./definition/encryption/fnv.hpp
	./definition/compression/zlib.hpp
	./definition/compression/lzma.hpp
	./definition/compression/bzip2.hpp
	./definition/javascript/converter.hpp
	./definition/javascript/runtime.hpp
	./definition/diff/vcdiff.hpp

	./dependencies/fmt.hpp
	./dependencies/json.hpp
	./dependencies/md5.hpp
	./dependencies/zlib.hpp
	./dependencies/SHA256.hpp
	./dependencies/Rijndael.hpp
	./dependencies/SHA224CryptoServiceProvider.hpp
	./dependencies/cpp-base64.hpp
	./dependencies/libpng.hpp
	./dependencies/etcpak.hpp
	./dependencies/tinyxml2.hpp
	./dependencies/ETCPACK.hpp
	./dependencies/SHA.hpp
	./dependencies/lzma.hpp
	./dependencies/avir.hpp
	./dependencies/open_vcdiff.hpp
	./dependencies/bzip2.hpp
	./dependencies/quickjs.hpp
	./dependencies/zip.hpp
	./dependencies/dirent.hpp
	./dependencies/simdjson.hpp
	./dependencies/compile-time-regular-expressions.hpp
	./dependencies/canvas_ity.hpp
	./dependencies/tsl.hpp

	./support/popcap/resource_group/common.hpp
	./support/popcap/resource_group/convert.hpp
	./support/popcap/res_info/common.hpp
	./support/popcap/res_info/convert.hpp
	./support/popcap/zlib/common.hpp
	./support/popcap/zlib/uncompress.hpp
	./support/popcap/zlib/compress.hpp
	./support/popcap/newton/decode.hpp
	./support/popcap/newton/encode.hpp
	./support/popcap/compiled_text/decode.hpp
	./support/popcap/compiled_text/encode.hpp
	./support/popcap/render_effects/common.hpp
	./support/popcap/render_effects/definition.hpp
	./support/popcap/render_effects/decode.hpp
	./support/popcap/render_effects/encode.hpp
	./support/popcap/particles/definition.hpp
	./support/popcap/particles/version.hpp
	./support/popcap/particles/common.hpp
	./support/popcap/particles/encode.hpp
	./support/popcap/particles/decode.hpp
	./support/popcap/rsb/definition.hpp
	./support/popcap/rsb/unpack.hpp
	./support/popcap/rsb/pack.hpp
	./support/popcap/rsb/miscellaneous/unpack_cipher.hpp
	./support/popcap/rsg/definition.hpp
	./support/popcap/rsg/unpack.hpp
	./support/popcap/rsg/pack.hpp
	./support/popcap/rton/decode.hpp
	./support/popcap/rton/encode.hpp
	./support/popcap/animation/definition.hpp
	./support/popcap/animation/decode.hpp
	./support/popcap/animation/encode.hpp
	./support/popcap/animation/convert/definition.hpp
	./support/popcap/animation/convert/to_flash.hpp
	./support/popcap/animation/convert/from_flash.hpp
	./support/popcap/animation/convert/instance.hpp
	./support/popcap/crypt_data/definition.hpp
	./support/popcap/crypt_data/encrypt.hpp
	./support/popcap/crypt_data/decrypt.hpp
	./support/popcap/cfw2/definition.hpp
	./support/popcap/cfw2/decode.hpp
	./support/popcap/cfw2/encode.hpp
	./support/popcap/rsb_patch/common.hpp
	./support/popcap/rsb_patch/decode.hpp
	./support/popcap/rsb_patch/encode.hpp
	./support/texture/compression/pvrtc/color.hpp
	./support/texture/compression/pvrtc/packet.hpp
	./support/texture/compression/pvrtc/pvrtc.hpp
	./support/texture/common.hpp
	./support/texture/encode.hpp
	./support/texture/decode.hpp
	./support/texture/invoke.hpp
	./support/wwise/sound_bank/definition.hpp
	./support/wwise/sound_bank/encode.hpp
	./support/wwise/sound_bank/decode.hpp
	./support/utility.hpp

	./interface/data.hpp
	./interface/cli.hpp
	./interface/callback.hpp
	./interface/version.hpp
	./interface/shell.hpp
	./interface/script.hpp
	./interface/interface.cpp
)


include_directories(${CMAKE_CURRENT_SOURCE_DIR}/dependencies/fmt)

set_target_properties(${PROJECT_NAME} PROPERTIES LINKER_LANGUAGE CXX)

target_include_directories(
	${PROJECT_NAME} PRIVATE
	..
)

if (MSVC)
    set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /Zi")
else ()
    set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g")
endif ()

if(CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
	target_compile_definitions(${PROJECT_NAME} PRIVATE MSVC_COMPILER)
endif()
if(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
	target_compile_definitions(${PROJECT_NAME} PRIVATE CLANG_COMPILER)
endif()

target_compile_definitions(
	${PROJECT_NAME} PRIVATE
  	CANVAS_ITY_IMPLEMENTATION
)



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

if (MSVC)
target_link_libraries(
	${PROJECT_NAME} PRIVATE
	dirent
)
endif()
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
	libpng
	etcpak
	tinyxml2
	ETCPACK
	SHA
	lzma
	avir
	open_vcdiff
	bzip2
	quickjs
	zip
	simdjson
	compile-time-regular-expressions
	canvas_ity
	tsl
)



if(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    target_compile_options(
      ${PROJECT_NAME} PRIVATE
        -Weverything
        -Wno-c++98-compat
        -Wno-c++98-compat-pedantic
        -Wno-pre-c++14-compat
        -Wno-pre-c++17-compat
        -Wno-c++20-compat
        -Wno-c99-extensions
        -Wno-padded
        -Wno-redundant-parens
        -Wno-missing-field-initializers
        -Wno-unused-parameter
        -Wno-unused-variable
        -Wno-missing-noreturn
        -Wno-gnu-zero-variadic-macro-arguments
        -Wno-ctad-maybe-unsupported
        -Wno-global-constructors
        -Wno-exit-time-destructors
        -Wno-weak-vtables
        -Wno-self-assign
        -Wno-switch-enum
        -Wno-shadow-field 
        -Wno-shadow-field-in-constructor 
        -Wno-shadow-uncaptured-local 
        -Wno-unsafe-buffer-usage 
        -Wno-float-equal 
        -Wno-gnu-line-marker 
)

if (MSVC)
    add_compile_options(/bigobj)
else ()
    add_compile_options(-Wa,-mbig-obj)
endif ()

endif()

set(CMAKE_CXX_STANDARD 23)

set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(MSVC)
	target_compile_definitions(
		${PROJECT_NAME} PRIVATE
		JS_STRICT_NAN_BOXING
	)
endif()


if(CMAKE_C_COMPILER_ID MATCHES "GNU|Clang")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wno-strict-prototypes")
endif()