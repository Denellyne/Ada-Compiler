#!/bin/sh

testASM() {
  if [[ -e mars.jar && -e "$1.txt" ]]; then
    cat "$1.in" | java -jar ./mars.jar nc sm "$1.bin" >out_raw.txt
    sed 's/Enter string (max 64 chars): //g' out_raw.txt >out.txt
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

compile() {
  cd ../src/
  make clean >/dev/null 2>&1
  make build
  cd ../tests/
}

build=1
tests=0
successes=0
fails=0
while getopts "e" option; do
  case $option in
  e)
    build=0
    ;;
  \?) # Invalid option
    echo "Error: Invalid option"
    exit
    ;;
  esac
done

if [ $build = 1 ]; then
  compile
fi
bash ./gen.sh

for filename in ./*.adb; do
  tests=$((tests + 1))
  if ../bin/ada $filename -o >/dev/null 2>&1; then
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

rm -f *.in *.adb *.bin *.txt *.out >/dev/null 2>&1

# For Github Actions
if [ "$fail" -ne 0 ]; then
    exit 1
fi
