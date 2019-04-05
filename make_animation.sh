#! /usr/bin/env bash

cmake . -DCMAKE_BUILD_TYPE=Release && make -j8 && ./noise_loop | ./display.py
