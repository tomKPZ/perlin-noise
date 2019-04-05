// perlin-noise: Optimized N-dimensional perlin noise in C++
// Copyright (C) 2019 <tomKPZ@gmail.com>
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software Foundation,
// Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301  USA

#include <png.h>

#include <iostream>
#include <mutex>
#include <string>
#include <thread>
#include <vector>

#include "perlin_noise.hpp"

template <typename T>
T check_aux(T t, const char* message) {
  if (!t) {
    std::cerr << "CHECK failed: " << message << std::endl;
    std::abort();
  }
  return t;
}

#define CHECK(x) check_aux(x, #x)

void WritePngFile(const std::string& file_name,
                  int width,
                  int height,
                  png_byte color_type,
                  png_byte bit_depth,
                  png_bytep* row_pointers) {
  // create file
  FILE* fp = CHECK(fopen(file_name.c_str(), "wb"));

  // initialize stuff
  png_structp png_ptr = CHECK(png_create_write_struct(
      PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr));

  png_infop info_ptr = CHECK(png_create_info_struct(png_ptr));

  CHECK(!setjmp(png_jmpbuf(png_ptr)));

  png_init_io(png_ptr, fp);

  // write header
  CHECK(!setjmp(png_jmpbuf(png_ptr)));

  png_set_IHDR(png_ptr, info_ptr, width, height, bit_depth, color_type,
               PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_BASE,
               PNG_FILTER_TYPE_BASE);

  png_write_info(png_ptr, info_ptr);

  // write bytes
  CHECK(!setjmp(png_jmpbuf(png_ptr)));

  png_write_image(png_ptr, row_pointers);

  // end write
  CHECK(!setjmp(png_jmpbuf(png_ptr)));

  png_write_end(png_ptr, nullptr);

  fclose(fp);
}

template <typename Float, typename Integer>
void RenderFrames() {
  constexpr std::size_t image_size = 256;
  constexpr std::size_t animation_steps = 100;
  constexpr Float space_r = 2.0;
  constexpr Float time_r = 1.5;
  constexpr Float two_pi = 2 * M_PI;

  std::size_t frames_rendered = 0;
  std::mutex frames_rendered_mutex;

  auto render_frames = [&](std::size_t frame_begin, std::size_t frame_end) {
    Perlin<6, Float, Integer> perlin;
    for (std::size_t i = frame_begin; i < frame_end; i++) {
      Float progress = static_cast<Float>(i) / animation_steps;

      png_byte bytes[image_size][image_size];
      png_bytep png_rows[image_size];
      for (std::size_t j = 0; j < image_size; j++) {
        png_rows[j] = bytes[j];
      }

      for (std::size_t y = 0; y < image_size; y++) {
        for (std::size_t x = 0; x < image_size; x++) {
          Float noise =
              perlin.Noise(space_r * std::sin(two_pi / image_size * x),
                           space_r * std::cos(two_pi / image_size * x),
                           space_r * std::sin(two_pi / image_size * y),
                           space_r * std::cos(two_pi / image_size * y),
                           time_r * std::sin(two_pi * progress),
                           time_r * std::cos(two_pi * progress));
          bytes[y][x] = (noise + 0.5) * 255.0;
        }
      }

      WritePngFile("animation/image_" + std::to_string(i) + ".png", image_size,
                   image_size, PNG_COLOR_TYPE_GRAY, 8, png_rows);

      {
        std::lock_guard<std::mutex> lock(frames_rendered_mutex);
        std::cout << "Progress: " << ++frames_rendered << "/" << animation_steps
                  << std::endl;
      }
    }
  };

  unsigned int n_threads = std::thread::hardware_concurrency();
  if (n_threads == 0) {
    render_frames(0, animation_steps);
  } else {
    std::vector<std::thread> threads;
    for (unsigned int i = 0; i < n_threads; i++) {
      threads.emplace_back(render_frames, i * animation_steps / n_threads,
                           (i + 1) * animation_steps / n_threads);
    }
    for (std::thread& thread : threads) {
      thread.join();
    }
  }
}

int main() {
  RenderFrames<float, int>();
  return 0;
}
