#!/usr/bin/env bash

FILEPATH=$1
if [ -z $FILEPATH ]; then
  echo "usage ./viewqoi <qoi file>"
  exit 1
fi
FILENAME=$(basename -- "$FILEPATH")
EXT="${FILENAME##*.}"
if [ $EXT != "qoi" ]; then 
  echo "'$FILEPATH' has invalid extension"
  exit 1
fi

QOICONV=./build/qoiconv
if [ ! -z $2 ]; then
  QOICONV=$2
fi

OUT=`mktemp --suffix=.png`
$QOICONV $FILEPATH $OUT
/usr/bin/xdg-open $OUT
