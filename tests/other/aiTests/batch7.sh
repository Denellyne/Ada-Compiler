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
create gl1_basic_read 'procedure Main is
  str : String := "          ";
  num : Integer := 0;
begin
  Get_Line (str, num);
  Put_Line (str);
  Put_Num (num);
end Main;' 'hello
5' 'hello'

create gl2_two_reads 'procedure Main is
  str1 : String := "          ";
  str2 : String := "          ";
  num : Integer := 0;
begin
  Get_Line (str1, num);
  Get_Line (str2, num);
  Put_Line (str1);
  Put_Line (str2);
end Main;' 'first
second
' 'first
second'

create gl3_num_then_read 'procedure Main is
  str : String := "          ";
  num : Integer := 0;
begin
  Put_Num (123);
  Get_Line (str, num);
  Put_Line (str);
end Main;' '123test
' 'test'

create gl4_read_then_num 'procedure Main is
  str : String := "          ";
  num : Integer := 0;
begin
  Get_Line (str, num);
  Put_Num (num);
  Put_Line (str);
end Main;' '4word
' 'word'

create gl5_multiple_outputs 'procedure Main is
  str : String := "          ";
  num : Integer := 0;
begin
  Put_Line ("start");
  Get_Line (str, num);
  Put_Line (str);
  Put_Num (num);
  Put_Line ("end");
end Main;' 'startdata
4end' 'data'

create gl6_reuse_buffer 'procedure Main is
  str : String := "          ";
  num : Integer := 0;
begin
  Get_Line (str, num);
  Put_Line (str);
  Get_Line (str, num);
  Put_Line (str);
end Main;' 'abc
xyz
' 'abc
xyz'

create gl8_long_input 'procedure Main is
  str : String := "12345";
  num : Integer := 0;
begin
  Get_Line (str, num);
  Put_Line (str);
  Put_Num (num);
end Main;' 'abcdefghij
10' 'abcdefghij'

create gl9_if_after_read 'procedure Main is
  str : String := "          ";
  num : Integer := 0;
begin
  Get_Line (str, num);
  if num > 3 then
    Put_Line (str);
  else
    Put_Num (0);
  end if;
end Main;' 'hello
' 'hello'

create gl10_if_after_read2 'procedure Main is
  str : String := "          ";
  num : Integer := 0;
begin
  Get_Line (str, num);
  if num > 5 then
    Put_Line (str);
  else
    Put_Num (num);
  end if;
end Main;' '2' 'ab'

create gl11_loop_read 'procedure Main is
  str : String := "          ";
  num : Integer := 0;
  i : Integer := 0;
begin
  while i < 2 loop
    Get_Line (str, num);
    Put_Line (str);
    i := i + 1;
  end loop;
end Main;' 'one
two
' 'one
two'

create gl12_read_calc 'procedure Main is
  str : String := "          ";
  num : Integer := 0;
begin
  Get_Line (str, num);
  Put_Num (num * 2);
end Main;' '6' 'hey'

create gl13_bool_read 'procedure Main is
  str : String := "          ";
  num : Integer := 0;
  v : Boolean := True;
begin
  if v then
    Get_Line (str, num);
    Put_Line (str);
  end if;
end Main;' 'yes
' 'yes'

create gl14_read_xor 'procedure Main is
  str : String := "          ";
  num : Integer := 0;
begin
  Get_Line (str, num);
  Put_Num (num xor 1);
end Main;' '5' 'test'

create gl15_arith_len 'procedure Main is
  str : String := "          ";
  num : Integer := 0;
begin
  Get_Line (str, num);
  Put_Num (num + 10);
end Main;' '13' 'abc'

create gl16_pow_len 'procedure Main is
  str : String := "          ";
  num : Integer := 0;
begin
  Get_Line (str, num);
  num := num ** 2;
  Put_Num (num);
end Main;' '9' 'dog'

create gl17_read_assign 'procedure Main is
  str : String := "          ";
  num : Integer := 5;
begin
  Get_Line (str, num);
  Put_Num (num);
  Put_Line (str);
end Main;' '4data
' 'data'

create gl18_nested_if_read 'procedure Main is
  str : String := "          ";
  num : Integer := 0;
begin
  Get_Line (str, num);
  if num > 0 then
    if num < 5 then
      Put_Line (str);
    else
      Put_Num (num);
    end if;
  end if;
end Main;' 'hi
' 'hi'

create gl19_elsif_read 'procedure Main is
  str : String := "          ";
  num : Integer := 0;
begin
  Get_Line (str, num);
  if num = 0 then
    Put_Num (0);
  elsif num < 3 then
    Put_Num (1);
  else
    Put_Line (str);
  end if;
end Main;' 'word
' 'word'

create gl20_while_len 'procedure Main is
  str : String := "          ";
  num : Integer := 0;
  i : Integer := 0;
begin
  Get_Line (str, num);
  while i < num loop
    Put_Num (i);
    i := i + 1;
  end loop;
end Main;' '012' 'cat'

create gl21_read_multi_num 'procedure Main is
  str : String := "          ";
  num : Integer := 0;
begin
  Get_Line (str, num);
  Put_Num (num);
  Put_Num (num);
  Put_Num (num);
end Main;' '222' 'ab'

create gl22_line_read_line 'procedure Main is
  str : String := "          ";
  num : Integer := 0;
begin
  Put_Line ("before");
  Get_Line (str, num);
  Put_Line ("after");
  Put_Num (num);
end Main;' 'beforeafter6' 'middle'

create gl23_read_bool_check 'procedure Main is
  str : String := "          ";
  num : Integer := 0;
  v : Boolean := False;
begin
  Get_Line (str, num);
  if num > 2 then
    v := True;
  end if;
  if v then
    Put_Num (1);
  else
    Put_Num (0);
  end if;
end Main;' '1' 'test'

create gl24_read_not 'procedure Main is
  str : String := "          ";
  num : Integer := 0;
begin
  Get_Line (str, num);
  if not (num < 3) then
    Put_Line (str);
  else
    Put_Num (0);
  end if;
end Main;' 'hello
' 'hello'

create gl25_while_read_inside 'procedure Main is
  str : String := "          ";
  num : Integer := 0;
  i : Integer := 0;
begin
  while i < 3 loop
    Get_Line (str, num);
    Put_Num (num);
    i := i + 1;
  end loop;
end Main;' '345' 'abc
test
hello'

create gl26_read_comparison 'procedure Main is
  str : String := "          ";
  num : Integer := 0;
begin
  Get_Line (str, num);
  if num >= 4 then
    Put_Num (1);
  else
    Put_Num (0);
  end if;
end Main;' '1' 'four'

create gl27_read_or 'procedure Main is
  str : String := "          ";
  num : Integer := 0;
begin
  Get_Line (str, num);
  if (num < 2) or (num > 5) then
    Put_Num (1);
  else
    Put_Line (str);
  end if;
end Main;' 'test
' 'test'

create gl28_read_and 'procedure Main is
  str : String := "          ";
  num : Integer := 0;
begin
  Get_Line (str, num);
  if (num > 1) and (num < 6) then
    Put_Line (str);
  else
    Put_Num (0);
  end if;
end Main;' 'word
' 'word'

create gl29_read_then_pow 'procedure Main is
  str : String := "          ";
  num : Integer := 0;
  x : Integer := 2;
begin
  Get_Line (str, num);
  x := x ** num;
  Put_Num (x);
end Main;' '8' 'abc'

create gl30_complex_read 'procedure Main is
  str : String := "          ";
  num : Integer := 0;
  x : Integer := 10;
begin
  Get_Line (str, num);
  x := x + num;
  if x > 12 then
    Put_Line (str);
    Put_Num (x);
  else
    Put_Num (0);
  end if;
end Main;' 'data
14' 'data'
