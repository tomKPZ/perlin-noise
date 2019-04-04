cmake_minimum_required(VERSION 2.6)
project(PerlinNoise)

set(CMAKE_CXX_FLAGS "-Wall -Wextra -Werror")
set(CMAKE_CXX_FLAGS_RELEASE "-O3 -march=native -mtune=native")
set(CMAKE_CXX_FLAGS_DEBUG "-O0 -g")

add_executable(perlin_noise perlin_noise.cpp)

set_target_properties(perlin_noise PROPERTIES CXX_STANDARD 20)
