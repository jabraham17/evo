#!/usr/bin/env bash

echo "I am broken, do not use"
exit

# ffmpeg -r 60 -f image2 -s 128x128 -pattern_type glob -i "/Volumes/Evo/$1/gen_*_tick_*.bmp" -vcodec libx264 -crf 25 -pix_fmt yuv420p $1.mp4


ROOT_DIR=/Volumes/Evo/test2
OUT_NAME=comp.mp4
FILES=`ls $ROOT_DIR/gen_*_tick_*.bmp.gz`

set -e
set -x

rm -f image_pipe
mkfifo image_pipe

# ffmpeg -y $IN_ARGS $OUT_ARGS test2.mp4 <image_pipe &

# ffmpeg -y -loop 1 -re -f image2 -r 1 -s 256x256 -pix_fmt bgr24 -c:v bmp -i image_pipe -c:v libx264 -crf 25 -pix_fmt yuv420p test2.mp4 &

IN_ARGS='-f image2pipe -r 1 -s 256x256 -pix_fmt bgr24 -c:v bmp -i -'
OUT_ARGS='-c:v libx264 -crf 25 -pix_fmt yuv420p'
for f in $FILES; do
    echo $f
    cat $f | ./build/bin/zpipe -d >image_pipe 
done;

# cat /Volumes/Evo/test2/gen_*_tick_*.bmp | ffmpeg -y -f image2pipe -r 1 -s 256x256 -pix_fmt bgr24 -c:v bmp -i - -c:v libx264 -crf 25 -pix_fmt yuv420p test2.mp4

# echo $FILES
#  ./build/bin/zpipe -d | ffmpeg -y -f image2pipe -r 30000/1001 -pix_fmt bgr24 -s 256x256 -c:v bmp -i - -c:v libx264 -crf 25 -pix_fmt yuv420p test2.mp4

rm -f image_pipe
