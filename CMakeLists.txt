find_package(PkgConfig REQUIRED)

cmake_minimum_required(VERSION 2.6)
project(PerlinNoise)

add_compile_options(-Wall -Wextra -Werror)
if("${CMAKE_BUILD_TYPE}" STREQUAL "Release")
  add_compile_options(-march=native -mtune=native)
endif()

pkg_check_modules(PNG REQUIRED libpng)

add_executable(noise_loop noise_loop.cpp)

set_target_properties(noise_loop PROPERTIES CXX_STANDARD 17)

target_link_libraries(noise_loop ${PNG_LIBRARIES})
target_include_directories(noise_loop PUBLIC ${PNG_INCLUDE_DIRS})
target_compile_options(noise_loop PUBLIC ${PNG_CFLAGS_OTHER})
