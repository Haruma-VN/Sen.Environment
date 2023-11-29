cmake_minimum_required(VERSION 3.15)

project(SHA256)

add_library(
    ${PROJECT_NAME} STATIC
    ./SHA256.h
    ./SHA256.cpp
)