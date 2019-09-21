cmake_minimum_required(VERSION 3.12)
project(SphericalHarmonic)

set(CMAKE_CXX_STANDARD 14)

add_definitions(-DSTB_IMAGE_IMPLEMENTATION -DSTBI_FAILURE_USERMSG)

add_subdirectory(encode)
#add_subdirectory(decode)