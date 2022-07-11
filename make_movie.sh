#!/usr/bin/env bash

ffmpeg -r 60 -f image2 -s 128x128 -pattern_type glob -i "/Volumes/Evo/$1/gen_*_tick_*.bmp" -vcodec libx264 -crf 25 -pix_fmt yuv420p $1.mp4
