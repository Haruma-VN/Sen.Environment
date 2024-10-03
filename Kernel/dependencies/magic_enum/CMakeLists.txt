project(
    magic_enum
    VERSION 0.9.6
    HOMEPAGE_URL https://github.com/Neargye/magic_enum
    LANGUAGES CXX
)

add_library(
    ${PROJECT_NAME} INTERFACE
)

target_sources(${PROJECT_NAME} INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR}/magic_enum_all.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/magic_enum_containers.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/magic_enum_flags.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/magic_enum_format.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/magic_enum_iostream.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/magic_enum_switch.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/magic_enum_utility.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/magic_enum.hpp
)

target_include_directories(
    ${PROJECT_NAME} INTERFACE
    ../fmt
)

target_link_libraries(
    ${PROJECT_NAME} INTERFACE
    fmt
)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
target_compile_features(${PROJECT_NAME} INTERFACE cxx_std_20)

if(MSVC)
    target_compile_options(${PROJECT_NAME} INTERFACE /utf-8)
else()
    target_compile_options(${PROJECT_NAME} INTERFACE -finput-charset=UTF-8)
endif()

set_target_properties(${PROJECT_NAME} PROPERTIES POSITION_INDEPENDENT_CODE ON)
