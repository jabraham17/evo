#!/usr/bin/env bash

set -e

CLANG=`make dump_paths | grep "^CC" | sed s/^CC=//`
LLVM=`dirname $CLANG`
SCANBUILD=$LLVM/scan-build
SCANVIEW=$LLVM/scan-view
ANALYZER=/usr/local/opt/llvm/libexec/ccc-analyzer
CWD=`pwd`
$SCANBUILD --keep-cc --view make CC="$ANALYZER" -B -j$(nproc)
