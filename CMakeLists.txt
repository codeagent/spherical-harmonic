cmake_minimum_required(VERSION 3.12)
project(SH)

set(CMAKE_CXX_STANDARD 14)

add_subdirectory(glm)

include_directories(stb glm)

add_executable(SH main.cpp PixelArray.h pixel_format.h CubeMap.h sampling.h utils.h spherical_harmonic.h math.h CubeMapPolarFunction.h CliInput.h)

add_definitions(-DSTB_IMAGE_IMPLEMENTATION -DSTBI_FAILURE_USERMSG)