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

# 41–60: more while + booleans + strings

create b3_line_var 'procedure Main is
  S : String := "ok";
begin
  Put_Line ("hi");
  Put_Line (S);
end Main;' 'hiok'

create b3_if_string1 'procedure Main is
  S : String := "x";
begin
  if 2 > 1 then
    Put_Line (S);
  end if;
end Main;' 'x'

create b3_if_string2 'procedure Main is
  S : String := "z";
begin
  if 2 < 1 then
    Put_Line (S);
  end if;
end Main;' ''

create b3_while_line1 'procedure Main is
  X : Integer := 3;
begin
  while X > 0 loop
    Put_Line ("a");
    X := X - 1;
  end loop;
end Main;' 'aaa'

create b3_while_line_num 'procedure Main is
  X : Integer := 2;
begin
  while X > 0 loop
    Put_Line ("b");
    X := X - 1;
  end loop;
  Put_Num (5);
end Main;' 'bb5'

create b3_while_line_var 'procedure Main is
  X : Integer := 2;
  S : String := "c";
begin
  while X > 0 loop
    Put_Line (S);
    X := X - 1;
  end loop;
end Main;' 'cc'

create b3_bool_flip 'procedure Main is
  V : Boolean := True;
begin
  if V then
    Put_Num (1);
  end if;
  V := False;
  if V then
    Put_Num (2);
  end if;
end Main;' '1'

create b3_bool_expr 'procedure Main is
  A : Boolean := True;
  B : Boolean := False;
begin
  if A and not B then
    Put_Num (3);
  end if;
end Main;' '3'

create b3_bool_expr2 'procedure Main is
  A : Boolean := False;
  B : Boolean := True;
begin
  if (A or B) and not (A and B) then
    Put_Num (4);
  end if;
end Main;' '4'

create b3_if_complex 'procedure Main is
  X : Integer := 5;
begin
  if (X > 0) and (X < 10) and (X /= 3) then
    Put_Num (1);
  else
    Put_Num (0);
  end if;
end Main;' '1'

create b3_if_complex2 'procedure Main is
  X : Integer := 3;
begin
  if (X > 0) and not (X > 2 and X < 4) then
    Put_Num (1);
  else
    Put_Num (0);
  end if;
end Main;' '0'

create b3_while_complex1 'procedure Main is
  X : Integer := 5;
begin
  while (X > 0) and (X /= 2) loop
    Put_Num (X);
    X := X - 1;
  end loop;
end Main;' '543'

create b3_while_complex2 'procedure Main is
  X : Integer := 0;
begin
  while (X < 5) or (X = 10) loop
    Put_Num (X);
    X := X + 2;
  end loop;
end Main;' '024'

create b3_while_nested1 'procedure Main is
  I : Integer := 1;
  J : Integer := 1;
begin
  while I <= 2 loop
    J := 1;
    while J <= 2 loop
      Put_Num (I);
      Put_Num (J);
      J := J + 1;
    end loop;
    I := I + 1;
  end loop;
end Main;' '11122122'

create b3_counter_stop 'procedure Main is
  X : Integer := 10;
begin
  while X > 3 loop
    Put_Num (X);
    X := X - 3;
  end loop;
end Main;' '1074'

create b3_mult_acc 'procedure Main is
  X : Integer := 1;
  I : Integer := 1;
begin
  while I <= 4 loop
    X := X * I;
    I := I + 1;
  end loop;
  Put_Num (X);
end Main;' '24'

create b3_line_then_num_seq 'procedure Main is
begin
  Put_Line ("x");
  Put_Num (1);
  Put_Num (2);
  Put_Line ("y");
end Main;' 'x12y'

create b3_num_mix 'procedure Main is
begin
  Put_Num (10);
  Put_Num (20);
  Put_Num (3);
end Main;' '10203'
