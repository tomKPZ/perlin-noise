#! /usr/bin/env bash

set -e

cd $(dirname "$0")

rm -f animation/*.png
rm -f animation/animation.mp4
mkdir -p animation

cmake . -DCMAKE_BUILD_TYPE=Release
make noise_loop

./noise_loop
ffmpeg -r 30 -i animation/image_%d.png -vcodec libx264 -crf 15 -pix_fmt gray \
       animation/animation.mp4
