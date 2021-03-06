Optimized N-dimensional perlin noise in C++
===========================================

Installation
------------

Simply add `perlin_noise.hpp` to your project.

Usage
-----

Create a `Perlin` object and call its `Noise()` function.

```
Perlin<3> perlin;
perlin.Noise(1.0, 2.0, 3.0);  // Returns a value from -0.5 to 0.5
```
Example
-------

To build and run the example, run `make_animation.sh`.  This will
create a file `animation/animation.mp4`.  This example uses 6D fractal
noise to create an animation that loops and also tiles both vertically
and horizontally.
