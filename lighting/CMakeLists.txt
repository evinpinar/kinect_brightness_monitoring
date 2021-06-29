# Copyright (c) Microsoft Corporation. All rights reserved.
# Licensed under the MIT License.

cmake_minimum_required(VERSION 3.20)
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_compile_options(-std=c++11)

project(Lighting)

set("k4a_DIR" "/usr/lib/x86_64-linux-gnu/cmake/k4a/k4aConfig.cmake")
find_package(k4a REQUIRED)
FIND_PACKAGE( OpenCV REQUIRED )                              
INCLUDE_DIRECTORIES( ${OpenCV_INCLUDE_DIRS} )

add_executable(enumerate_devices enumerate.cpp)

target_compile_features(enumerate_devices PUBLIC cxx_std_14)

target_link_libraries(enumerate_devices PRIVATE k4a::k4a ${OpenCV_LIBS})
