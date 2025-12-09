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

# 1–20: basic arithmetic and simple ifs

create b1_arith_add1 'procedure Main is
  X : Integer := 1;
begin
  X := X + 2;
  Put_Num (X);
end Main;' '3'

create b1_arith_add2 'procedure Main is
  X : Integer := 10;
begin
  X := X + 15;
  Put_Num (X);
end Main;' '25'

create b1_arith_sub1 'procedure Main is
  X : Integer := 7;
begin
  X := X - 4;
  Put_Num (X);
end Main;' '3'

create b1_arith_mul1 'procedure Main is
  X : Integer := 6;
begin
  X := X * 7;
  Put_Num (X);
end Main;' '42'

create b1_arith_div1 'procedure Main is
  X : Integer := 18;
begin
  X := X / 3;
  Put_Num (X);
end Main;' '6'

create b1_arith_mix1 'procedure Main is
  X : Integer := 0;
begin
  X := 2 * 5 + 3;
  Put_Num (X);
end Main;' '13'

create b1_arith_mix2 'procedure Main is
  X : Integer := 0;
begin
  X := (2 + 5) * 3;
  Put_Num (X);
end Main;' '21'

create b1_pow1 'procedure Main is
  X : Integer := 3;
begin
  X := X ** 3;
  Put_Num (X);
end Main;' '27'

create b1_pow2 'procedure Main is
  X : Integer := 2;
begin
  X := X ** 5;
  Put_Num (X);
end Main;' '32'

create b1_chain_assign 'procedure Main is
  X : Integer := 1;
begin
  X := X + 1;
  X := X * 4;
  X := X - 2;
  Put_Num (X);
end Main;' '6'

create b1_if_true1 'procedure Main is
begin
  if 4 > 1 then
    Put_Num (9);
  end if;
end Main;' '9'

create b1_if_false1 'procedure Main is
begin
  if 2 > 5 then
    Put_Num (1);
  end if;
end Main;' ''

create b1_if_else1 'procedure Main is
begin
  if 2 = 3 then
    Put_Num (1);
  else
    Put_Num (2);
  end if;
end Main;' '2'

create b1_if_else2 'procedure Main is
  X : Integer := 5;
begin
  if X > 3 then
    Put_Num (7);
  else
    Put_Num (8);
  end if;
end Main;' '7'

create b1_if_var1 'procedure Main is
  X : Integer := 3;
begin
  if X * 2 = 6 then
    Put_Num (4);
  end if;
end Main;' '4'

create b1_if_var2 'procedure Main is
  X : Integer := 3;
begin
  if X * 2 > 6 then
    Put_Num (1);
  else
    Put_Num (0);
  end if;
end Main;' '0'

create b1_if_bool1 'procedure Main is
  V : Boolean := True;
begin
  if V then
    Put_Num (5);
  end if;
end Main;' '5'

create b1_if_bool2 'procedure Main is
  V : Boolean := False;
begin
  if V then
    Put_Num (1);
  else
    Put_Num (2);
  end if;
end Main;' '2'

create b1_if_and1 'procedure Main is
begin
  if (3 > 1) and (4 > 2) then
    Put_Num (6);
  end if;
end Main;' '6'

create b1_if_or1 'procedure Main is
begin
  if (3 > 5) or (1 < 2) then
    Put_Num (7);
  end if;
end Main;' '7'
