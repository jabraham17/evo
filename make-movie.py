#!/usr/bin/env python3
import ffmpeg

stream = ffmpeg.concat(
    ffmpeg.input('out/gen-1-tick-*.qoi', pattern_type='glob', framerate=24),
    ffmpeg.input('out/gen-2-tick-*.qoi', pattern_type='glob', framerate=24),
)
stream = ffmpeg.output(stream, 'out.mp4')
ffmpeg.run(stream)
