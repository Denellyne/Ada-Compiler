#!/usr/bin/env bash

mkdir -p ada_tests
cd ada_tests

create() {
  name="$1"
  code="$2"
  out="$3"
  in="$4"

  printf '%s\n' "$code" >"$name.adb"
  printf '%s\n' "$out" >"$name.adb.txt"
  printf '%s' "$in" >"$name.adb.in"
}

# 81–100: final mix

create b5_num_only1 'procedure Main is
begin
  Put_Num (0);
end Main;' '0'

create b5_num_only2 'procedure Main is
begin
  Put_Num (9);
end Main;' '9'

create b5_line_only1 'procedure Main is
begin
  Put_Line ("test");
end Main;' 'test'

create b5_line_only2 'procedure Main is
begin
  Put_Line ("abc");
end Main;' 'abc'
