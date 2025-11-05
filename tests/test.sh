#!/bin/sh

cd ../src/
make build
cd ../tests/

for filename in ./*.adb; do
  echo $filename
  if ../bin/ada $filename >/dev/null 2>&1 && echo worked; then
    echo "$filename success"
  else
    echo "$filename failded"
  fi
done
