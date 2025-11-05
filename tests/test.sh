#!/bin/sh

cd ../src/
make build
cd ../tests/

tests=0
successes=0
fails=0

for filename in ./*.adb; do
  tests=$((tests + 1))
  if ../bin/ada $filename >/dev/null 2>&1; then
    echo $filename Success
    successes=$((successes + 1))
  else
    echo $filename Failed
    fails=$((fails + 1))
  fi
done

echo "Tests:$tests | Success:$successes | Fail:$fails"
