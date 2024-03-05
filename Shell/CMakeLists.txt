cmake_minimum_required (VERSION 3.8)

if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()

project (Shell)

if(MSVC)
    add_compile_options(/utf-8)
else()
    add_compile_options(-finput-charset=UTF-8 -fexec-charset=UTF-8)
endif()

add_definitions(-w)

if (WIN32)
  add_compile_options(/w)
endif()

add_executable (
    ${PROJECT_NAME} 
    ./library/color.hpp
    ./library/dialog.hpp
    ./library/string.hpp
    ./library/macro.hpp
    ./library/callback.hpp
    ./shell.cpp 
    $<$<PLATFORM_ID:Windows>:./shell.rc>
)


if (WIN32)
  set(MY_MANIFEST_FILE ./resource/windows/application.manifest)
  set(CMAKE_SHARED_LINKER_FLAGS "/MANIFEST:NO")
endif()

target_include_directories(
	${PROJECT_NAME} PRIVATE
	..
)

if(NOT (CMAKE_SYSTEM_NAME MATCHES "iOS" OR CMAKE_SYSTEM_NAME MATCHES "Android"))
    add_subdirectory(./deps/tinyfiledialogs)
    target_link_libraries(${PROJECT_NAME} PRIVATE tinyfiledialogs)
endif()


add_subdirectory(./deps/cpp-httplib)

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET Shell PROPERTY CXX_STANDARD 20)
endif()
set(CMAKE_CXX_STANDARD 23)
# TODO: Add tests and install targets if needed.
