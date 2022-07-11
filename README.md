# README


## Sample

```
./build/bin/evo -g 2 -o /Volumes/Evo/test --callback-tick=1 --callback-tick-freq=1 -w 256 -h 256 -n 10000 -g 30 -t 300 -c 20
ffmpeg -r 60 -f image2 -s 256x256 -pattern_type glob -i "/Volumes/Evo/test/gen_*_tick_*.bmp" -vcodec libx264 -crf 25 -pix_fmt yuv420p test.mp4
```
