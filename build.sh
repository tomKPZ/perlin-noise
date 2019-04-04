#! /usr/bin/env bash

clang++ perlin_noise.cpp -o perlin_noise -std=c++2a -Wall -Wextra -Werror -lcrypto -O3 -g -march=native -mtune=native
