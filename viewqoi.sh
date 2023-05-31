#!/usr/bin/env bash

if [ $# -eq 0 ]; then
  echo "usage ./viewqoi <qoi file>..."
  exit 1
fi

QOICONV_=${QOICONV:-./build/qoiconv}

for FILEPATH in "$@"; do

  FILENAME=$(basename -- "$FILEPATH")
  EXT="${FILENAME##*.}"
  FILENAME="${FILENAME%.*}"
  if [ $EXT != "qoi" ]; then 
    echo "'$FILEPATH' has invalid extension"
    exit 1
  fi

  OUT=`mktemp -p /tmp $FILENAME.XXX --suffix=.png`
  $QOICONV_ $FILEPATH $OUT
  /usr/bin/xdg-open $OUT
done
