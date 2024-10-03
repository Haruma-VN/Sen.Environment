cmake_minimum_required(VERSION 3.14)

project(
  dirent #MSVC
  LANGUAGES C
  VERSION 1.24
  HOMEPAGE_URL https://github.com/tronkko/dirent
)

add_library(
  ${PROJECT_NAME} INTERFACE
  ./dirent.h
)