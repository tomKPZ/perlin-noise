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

#include <algorithm>
#include <boost/container_hash/hash.hpp>
#include <boost/random/uniform_on_sphere.hpp>
#include <boost/unordered_map.hpp>
#include <cmath>
#include <cstdint>
#include <iostream>
#include <iterator>
#include <numeric>
#include <random>

template <std::size_t N, typename Float = float, typename Integer = int>
class Perlin {
 public:
  explicit Perlin(std::size_t seed = 0) : seed_{seed}, hash_{seed} {}

  // Input vector components are given as parameters.  Example:
  //   Perlin<3> perlin;
  //   perlin.Noise(1, 2, 3);
  template <typename... Ts>
  Float Noise(Ts... ts) {
    static_assert(sizeof...(ts) == N, "Wrong number of arguments for Noise()");
    FVec v{ts...};
    return Noise(v);
  }

  // Input vector given as any iterable type.  Example:
  //   Perlin<3> perlin;
  //   float v[3] = {1, 2, 3};
  //   perlin.Noise(v);
  template <typename T>
  Float Noise(const T& t) {
    FVec v;
    std::copy_n(std::begin(t), N, std::begin(v));
    return Noise(v);
  }

  // Input vector given as a pointer.  Example:
  //   Perlin<3> perlin;
  //   float* v = malloc(3*sizeof(float));
  //   v[0] = 1; v[1] = 2; v[2] = 3;
  //   perlin.Noise(v);
  Float Noise(const Float* f) {
    FVec v;
    std::copy_n(f, N, std::begin(v));
    return Noise(v);
  }
  Float Noise(Float* f) {
    const Float* v = f;
    return Noise(v);
  }

 private:
  template <typename T>
  class Vector {
   public:
    using iterator = T*;
    using Vec = Vector<T>;

    Vector() = default;

    template <typename... Ts>
    explicit Vector(Ts... ts) : dims_{ts...} {}

    // Necessary for boost::uniform_on_sphere.
    explicit Vector(int) {}

    iterator begin() { return std::begin(dims_); }
    iterator end() { return std::end(dims_); }

    bool operator==(const Vec& other) const {
      return std::equal(std::begin(dims_), std::end(dims_),
                        std::begin(other.dims_));
    }

    const T& operator[](std::size_t i) const { return dims_[i]; }
    T& operator[](std::size_t i) { return dims_[i]; }

    template <typename S>
    Vec operator-(const Vector<S>& other) const {
      Vec new_vec;
      for (std::size_t i = 0; i < N; i++) {
        new_vec[i] = dims_[i] - other[i];
      }
      return new_vec;
    }

    T operator*(const Vec& other) const {
      T dot = 0;
      for (std::size_t i = 0; i < N; i++) {
        dot += dims_[i] * other.dims_[i];
      }
      return dot;
    }

    template <typename Func>
    auto Transform(Func func) const -> Vector<decltype(func({}))> {
      Vector<decltype(func({}))> new_vec{};
      for (std::size_t i = 0; i < N; i++) {
        new_vec[i] = func(dims_[i]);
      }
      return new_vec;
    }

   private:
    T dims_[N];
  };

  using FVec = Vector<Float>;
  using IVec = Vector<Integer>;

  class Hasher {
   public:
    explicit Hasher(Perlin<N, Float, Integer>* perlin) : perlin_{perlin} {}
    std::size_t operator()(const IVec&) const { return perlin_->hash_; }

   private:
    Perlin<N, Float, Integer>* perlin_;
  };

  static Float Lerp(Float w, Float a0, Float a1) {
    return w * a1 + (1.0F - w) * a0;
  }

  static Float Fade(Float t) { return t * t * t * (t * (t * 6 - 15) + 10); }

  FVec Gradient() {
    std::size_t before = gradients_.size();
    auto& found = gradients_[v0_];
    if (gradients_.size() == before) {
      return found;
    }

    engine_.seed(hash_);
    found = generator_(engine_);
    return found;
  }

  Float PerlinMerge(std::size_t n) {
    if (n == N) {
      return vd_ * Gradient();
    }
    std::size_t old_hash = hash_;
    boost::hash_combine(hash_, v0_[n]);
    Float l = PerlinMerge(n + 1);
    hash_ = old_hash;
    v0_[n]++;
    vd_[n]--;
    boost::hash_combine(hash_, v0_[n]);
    Float r = PerlinMerge(n + 1);
    hash_ = old_hash;
    v0_[n]--;
    vd_[n]++;
    return Lerp(fade_[n], l, r);
  }

  Float Noise(const FVec& v) {
    v0_ = v.Transform(
        [](Float f) { return static_cast<Integer>(std::floor(f)); });
    vd_ = v - v0_;
    fade_ = vd_.Transform(Fade);

    return PerlinMerge(0);
  }

  const std::size_t seed_;
  std::size_t hash_;

  IVec v0_;
  FVec vd_;
  FVec fade_;

  std::default_random_engine engine_;
  boost::uniform_on_sphere<Float, FVec> generator_{N};

  boost::unordered_map<IVec, FVec, Hasher> gradients_{0, Hasher{this}};
};

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
