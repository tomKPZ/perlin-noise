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

#include <iostream>

#include "perlin_noise.hpp"

template <typename Float, typename Integer>
void RenderFrames() {
  const std::size_t image_size = 128;
  const std::size_t animation_steps = 50;
  const Float space_r = 3.0;
  const Float time_r = 3.0;
  Perlin<6, Float, Integer> perlin;
  std::cout << '[';
  for (std::size_t i = 0; i < animation_steps; i++) {
    Float progress = static_cast<Float>(i) / animation_steps;
    std::cerr << "Progress: " << progress * 100 << std::endl;
    std::cout << '[';
    for (std::size_t x = 0; x < image_size; x++) {
      std::cout << '[';
      for (std::size_t y = 0; y < image_size; y++) {
        Float noise = perlin.Noise(
            static_cast<Float>(space_r * std::sin(2 * M_PI / image_size * x)),
            static_cast<Float>(space_r * std::cos(2 * M_PI / image_size * x)),
            static_cast<Float>(space_r * std::sin(2 * M_PI / image_size * y)),
            static_cast<Float>(space_r * std::cos(2 * M_PI / image_size * y)),
            static_cast<Float>(time_r * std::sin(2 * M_PI * progress)),
            static_cast<Float>(time_r * std::cos(2 * M_PI * progress)));
        std::cout << noise << ',';
      }
      std::cout << "],";
    }
    std::cout << "],";
  }
  std::cout << "]";
}

int main() {
  RenderFrames<float, int>();
  return 0;
}
