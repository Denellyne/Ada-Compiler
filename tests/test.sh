#!/bin/sh

cd ../src/
make clean >/dev/null 2>&1
make build
cd ../tests/

tests=0
successes=0
fails=0

for filename in ./*.adb; do
  tests=$((tests + 1))
  if ../bin/ada $filename >/dev/null 2>&1; then
    echo -e "\033[0;32m$filename Success\033[0m"
    successes=$((successes + 1))
  else
    echo -e "\033[0;31m$filename Failed\033[0m"
    fails=$((fails + 1))
  fi
done

echo -e -n Tests:$tests
echo -e -n " |\033[0;32m Success:$successes\033[0m |"
echo -e "\033[0;31m Fail:$fails\033[0m"
