#!/bin/sh

cd ../src/
make build
cd ../tests/

for filename in ./*.adb; do
  if ../bin/ada $filename >/dev/null 2>&1; then
    echo $filename Success
  else
    echo $filename Failed
  fi
done
