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

# 21–40: more booleans, nested ifs, simple while

create b2_not1 'procedure Main is
begin
  if not (2 > 3) then
    Put_Num (1);
  end if;
end Main;' '1'

create b2_not2 'procedure Main is
  V : Boolean := False;
begin
  if not V then
    Put_Num (2);
  end if;
end Main;' '2'

create b2_and_or1 'procedure Main is
begin
  if (2 > 1) and ((3 < 4) or (5 < 2)) then
    Put_Num (3);
  end if;
end Main;' '3'

create b2_nested_if1 'procedure Main is
  X : Integer := 4;
begin
  if X > 0 then
    if X < 10 then
      Put_Num (1);
    end if;
  end if;
end Main;' '1'

create b2_nested_if2 'procedure Main is
  X : Integer := 4;
begin
  if X > 10 then
    if X < 20 then
      Put_Num (1);
    end if;
  else
    Put_Num (2);
  end if;
end Main;' '2'

create b2_while_dec1 'procedure Main is
  X : Integer := 3;
begin
  while X > 0 loop
    Put_Num (X);
    X := X - 1;
  end loop;
end Main;' '321'

create b2_while_inc1 'procedure Main is
  X : Integer := 1;
begin
  while X < 4 loop
    Put_Num (X);
    X := X + 1;
  end loop;
end Main;' '123'

create b2_while_zero 'procedure Main is
  X : Integer := 0;
begin
  while X > 0 loop
    Put_Num (1);
  end loop;
end Main;' ''

create b2_while_if2 'procedure Main is
  X : Integer := 6;
begin
  while X > 0 loop
    if X <= 3 then
      Put_Num (X);
    end if;
    X := X - 1;
  end loop;
end Main;' '321'

create b2_while_bool1 'procedure Main is
  X : Integer := 3;
  V : Boolean := True;
begin
  while (X > 0) and V loop
    Put_Num (X);
    X := X - 1;
  end loop;
end Main;' '321'

create b2_while_break_cond 'procedure Main is
  X : Integer := 5;
begin
  while 10 > X loop
    Put_Num (X);
    X := X + 2;
  end loop;
end Main;' '579'

create b2_sum1 'procedure Main is
  I : Integer := 1;
  S : Integer := 0;
begin
  while I <= 4 loop
    S := S + I;
    I := I + 1;
  end loop;
  Put_Num (S);
end Main;' '10'

create b2_sum2 'procedure Main is
  I : Integer := 1;
  S : Integer := 0;
begin
  while I <= 5 loop
    S := S + I * I;
    I := I + 1;
  end loop;
  Put_Num (S);
end Main;' '55'

create b2_pow_loop1 'procedure Main is
  X : Integer := 1;
  I : Integer := 0;
begin
  while I < 4 loop
    X := X * 2;
    I := I + 1;
  end loop;
  Put_Num (X);
end Main;' '16'

create b2_num_then_num 'procedure Main is
begin
  Put_Num (1);
  Put_Num (2);
  Put_Num (3);
end Main;' '123'

create b2_num_then_line 'procedure Main is
begin
  Put_Num (1);
  Put_Line ("A");
  Put_Num (2);
end Main;' '1A2'

create b2_two_lines 'procedure Main is
begin
  Put_Line ("hi");
  Put_Line ("bye");
end Main;' 'hibye'
