#!/bin/sh
rm -f ./ada_tests/*.adb

for f in batch*.sh; do
  if [ -f "$f" ]; then
    echo "Running $f..."
    "./$f"
  fi
done

echo "Created .adb and .adb.txt tests in ./ada_tests"
echo "All batch scripts executed."
