#!/bin/sh

testASM() {
  base="$1"

  outb="${base}.bin"
  outt="${base}.out"
  outr="${base}_raw.txt"
  inb="${base}.in"

  if [ -e mars.jar ] && [ -e "$1.txt" ]; then
    # Run MARS with a 15-second timeout
    if cat "${inb}" | java -jar ./mars.jar nc sm "$outb" >"$outr"; then
      sed 's/Enter string (max 64 chars): //g' "${outr}" >"${outt}"
      cmp --silent "$1.txt" "$outt"
      result=$?
    else
      # Timeout occurred
      result=1
    fi

    rm -f "$outb" "$outt"
    return $result
  else
    [ ! -e "$1.txt" ] &&
      printf "\033[0;31m %s doesn't have a result file to test\033[0m\n" "$name"
    return 0
  fi
}

compile() {
  cd ../../../../src/
  make clean >/dev/null 2>&1
  make build
  mv ../bin/ada ../tests/other/aiTests/ada_tests/ada
  cd ../tests/other/aiTests/ada_tests/
}

build=1
while getopts "e" option; do
  [ "$option" = "e" ] && build=0
done

[ $build = 1 ] && compile

cd ..
bash ./gen.sh
cd ./ada_tests

limit_jobs() {
  while [ "$(jobs | wc -l)" -ge 25 ]; do
    wait -n 2>/dev/null || wait
  done
}

rm -f results.tmp
touch results.tmp

for filename in ./*.adb; do
  limit_jobs

  (
    if ./ada "$filename" >/dev/null 2>&1 && testASM "$filename"; then
      echo "OK $filename"
    else
      echo "FAIL $filename"
    fi
  ) >>results.tmp &
done

wait

# Summaries:
tests=$(wc -l <results.tmp)
successes=$(grep -c "^OK" results.tmp)
fails=$(grep -c "^FAIL" results.tmp)

while read -r line; do
  if echo "$line" | grep -q "^OK"; then
    printf "\033[0;32m%s Success\033[0m\n" "$(echo "$line" | cut -d' ' -f2)"
  else
    printf "\033[0;31m%s Failed\033[0m\n" "$(echo "$line" | cut -d' ' -f2)"
  fi
done <results.tmp

printf "Tests:%s | \033[0;32mSuccess:%s\033[0m | \033[0;31mFail:%s\033[0m\n" \
  "$tests" "$successes" "$fails"

rm -f *.adb *.in *.bin *.txt *.out results.tmp
