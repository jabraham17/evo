ffmpeg -framerate 24 -r 30 -s 1024x1024 -f image2 -pattern_type glob -i "out/gen-*-tick-*.qoi" -vcodec libopenh264 -pix_fmt yuv420p out.mp4


# make movie for eacg gen, and overkay the gen number

# ./configure --prefix=/opt/ffmpeg --enable-gpl --enable-libopenh264 && make all -j8 && make install
