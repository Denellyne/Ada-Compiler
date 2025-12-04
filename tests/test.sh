#!/bin/sh

testASM() {
  if [[ -e mars.jar && -e "$1.txt" ]]; then
    java -jar ./mars.jar nc sm out.bin >out.txt
    if cmp --silent -- "$1.txt" "out.txt"; then
      return 0
    else
      return 1
    fi
  else
    if [ ! -e "$1.txt" ]; then
      echo -e "\033[0;31m $1 doesn't have a result file to test\033[0m"
    fi
    return 0
  fi
}

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
    if testASM $filename; then
      echo -e "\033[0;32m$filename Success\033[0m"
      successes=$((successes + 1))
    else
      echo -e "\033[0;31m$filename Failed\033[0m"
      fails=$((fails + 1))
    fi
  else
    echo -e "\033[0;31m$filename Failed\033[0m"
    fails=$((fails + 1))
  fi
done

echo -e -n Tests:$tests
echo -e -n " |\033[0;32m Success:$successes\033[0m |"
echo -e "\033[0;31m Fail:$fails\033[0m"

rm out.bin >/dev/null 2>&1
rm out.txt >/dev/null 2>&1
