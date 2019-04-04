cmake_minimum_required(VERSION 2.6)
project(PerlinNoise)

add_compile_options(-Wall -Wextra -Werror)
if("${CMAKE_BUILD_TYPE}" STREQUAL "Release")
  add_compile_options(-march=native -mtune=native)
endif()

add_executable(perlin_noise perlin_noise.cpp)

set_target_properties(perlin_noise PROPERTIES CXX_STANDARD 20)
