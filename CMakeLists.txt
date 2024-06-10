cmake_minimum_required(VERSION 3.8)

project(base64)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED YES)
set(CMAKE_CXX_EXTENSONS OFF)

add_library(base64 INTERFACE
  include/base64.h
)
