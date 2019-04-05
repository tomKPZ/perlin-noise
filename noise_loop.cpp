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
#include <string>

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
  const std::size_t image_size = 128;
  const std::size_t animation_steps = 1;
  const Float space_r = 3.0;
  const Float time_r = 3.0;
  Perlin<6, Float, Integer> perlin;
  for (std::size_t i = 0; i < animation_steps; i++) {
    Float progress = static_cast<Float>(i) / animation_steps;
    std::cerr << "Progress: " << progress * 100 << std::endl;

    png_byte bytes[image_size][image_size];
    png_bytep png_rows[image_size];
    for (std::size_t j = 0; j < image_size; j++) {
      png_rows[j] = bytes[j];
    }

    for (std::size_t x = 0; x < image_size; x++) {
      for (std::size_t y = 0; y < image_size; y++) {
        Float noise = perlin.Noise(
            static_cast<Float>(space_r * std::sin(2 * M_PI / image_size * x)),
            static_cast<Float>(space_r * std::cos(2 * M_PI / image_size * x)),
            static_cast<Float>(space_r * std::sin(2 * M_PI / image_size * y)),
            static_cast<Float>(space_r * std::cos(2 * M_PI / image_size * y)),
            static_cast<Float>(time_r * std::sin(2 * M_PI * progress)),
            static_cast<Float>(time_r * std::cos(2 * M_PI * progress)));
        bytes[y][x] = (noise + 1) / 2.0 * 255.0;
      }
    }

    WritePngFile("image.png", image_size, image_size, PNG_COLOR_TYPE_GRAY, 8,
                 png_rows);
  }
}

int main() {
  RenderFrames<float, int>();
  return 0;
}
