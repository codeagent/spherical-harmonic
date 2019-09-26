cmake_minimum_required(VERSION 3.12)
project(SphericalHarmonic)

set(CMAKE_CXX_STANDARD 14)

add_definitions(-DSTB_IMAGE_IMPLEMENTATION -DSTBI_FAILURE_USERMSG -DSTB_IMAGE_WRITE_IMPLEMENTATION)

include_directories(glm)
include_directories(stb)
include_directories(json.h)

add_library(json STATIC ./json.h/json.c)

add_subdirectory(encode)
add_subdirectory(decode)