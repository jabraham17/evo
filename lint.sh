#!/usr/bin/env bash

find . -name "*.c" -type f -exec clang-format -i {} +
find . -name "*.h" -type f -exec clang-format -i {} +

CLANG=`make dump_paths | grep "^CC" | sed s/^CC=//`
LLVM=`dirname $CLANG`
SCANBUILD=$LLVM/scan-build
SCANVIEW=$LLVM/scan-view
ANALYZER=$LLVM/../libexec/ccc-analyzer
CWD=`pwd`
$SCANBUILD --keep-cc --view make CC="$ANALYZER" -B -j$(nproc)

# find lint-result -regex "lint-result/.*" -type d -exec /usr/bin/env python3 $SCANVIEW {} \;
# rm -rf lint-result
