cmake_minimum_required(VERSION 3.15)

project(SHA256)

add_library(
    ${PROJECT_NAME} STATIC
    ./SHA256.h
    ./SHA256.cpp
)

if (MSVC)
    set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /Zi")
else ()
    set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g")
endif ()