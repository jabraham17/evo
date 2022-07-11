# README


## Sample

```
./build/bin/evo -g 2 -o /Volumes/Evo/test --callback-tick=1 --callback-tick-freq=1 -w 256 -h 256 -n 10000 -g 30 -t 300 -c 20
./build/bin/evo -g 2 -o /Volumes/Evo/test --callback-tick=1 --callback-start=0 --callback-end=0 --callback-tick-freq=10 -w 256 -h 256 -n 3000 -g 50 -t 300 -c 20
ffmpeg -r 60 -f image2 -s 256x256 -pattern_type glob -i "/Volumes/Evo/test/gen_*_tick_*.bmp" -vcodec libx264 -crf 25 -pix_fmt yuv420p test.mp4
```


## TODO

- Commandline args
  - selection criteria
  - species
- Expand what species do
- add more genes
- add more complex enviroments
  - add obstacles to the path
  - load from a file description?
- make a primitve langiuage to describe simualtions
  - langauge describes the enviroment, and how the simulation should run
  - can modify the simulation mid run
  - maybe we write in something like python, that generates a C file that you compile?
