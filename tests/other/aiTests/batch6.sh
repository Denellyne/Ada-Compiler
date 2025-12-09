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

# Arithmetic
create arith_add 'procedure Main is
  x : Integer := 1;
begin
  x := x + 5;
  Put_Num(x);
end Main;' '6'

create arith_sub 'procedure Main is
  x : Integer := 10;
begin
  x := x - 3;
  Put_Num(x);
end Main;' '7'

create arith_mul 'procedure Main is
  x : Integer := 4;
begin
  x := x * 3;
  Put_Num(x);
end Main;' '12'

create arith_div 'procedure Main is
  x : Integer := 20;
begin
  x := x / 4;
  Put_Num(x);
end Main;' '5'

create arith_pow 'procedure Main is
  x : Integer := 2;
begin
  x := x ** 3;
  Put_Num(x);
end Main;' '8'

create arith_precedence 'procedure Main is
  x : Integer := 0;
begin
  x := 2 + 3 * 4;
  Put_Num(x);
end Main;' '14'

create arith_parens 'procedure Main is
  x : Integer := 0;
begin
  x := (2 + 3) * 4;
  Put_Num(x);
end Main;' '20'

create arith_complex 'procedure Main is
  x : Integer := 0;
begin
  x := 10 * 2 + 50 / 10;
  Put_Num(x);
end Main;' '25'

create arith_reassign 'procedure Main is
  x : Integer := 5;
begin
  x := x + 1;
  x := x * 2;
  Put_Num(x);
end Main;' '12'

create arith_vars 'procedure Main is
  x : Integer := 2;
  y : Integer := 3;
begin
  Put_Num(x + y);
end Main;' '5'

# Conditionals
create cond_true 'procedure Main is
begin
  if True then
    Put_Num(1);
  end if;
end Main;' '1'

create cond_false 'procedure Main is
begin
  if False then
    Put_Num(1);
  end if;
end Main;' ''

create cond_else 'procedure Main is
begin
  if False then
    Put_Num(1);
  else
    Put_Num(2);
  end if;
end Main;' '2'

create cond_gt 'procedure Main is
begin
  if 5 > 3 then
    Put_Num(1);
  end if;
end Main;' '1'

create cond_logic_and 'procedure Main is
begin
  if (3 > 1) and (2 > 1) then
    Put_Num(7);
  end if;
end Main;' '7'

create cond_nested 'procedure Main is
begin
  if True then
    if True then
      Put_Num(9);
    end if;
  end if;
end Main;' '9'

create cond_nested_else 'procedure Main is
begin
  if True then
    if False then
      Put_Num(1);
    else
      Put_Num(2);
    end if;
  end if;
end Main;' '2'

create cond_vars 'procedure Main is
  x : Integer := 10;
begin
  if x > 5 then
    Put_Num(x);
  end if;
end Main;' '10'

create cond_bool_var 'procedure Main is
  v : Boolean := True;
begin
  if v then
    Put_Num(1);
  end if;
end Main;' '1'

create cond_complex 'procedure Main is
  x : Integer := 1;
begin
  if x > 0 then
    x := 0;
  else
    x := 1;
  end if;
  Put_Num(x);
end Main;' '0'

# Loops
create loop_basic 'procedure Main is
  x : Integer := 3;
begin
  while x > 0 loop
    Put_Num(x);
    x := x - 1;
  end loop;
end Main;' '321'

create loop_zero_iter 'procedure Main is
begin
  while False loop
    Put_Num(1);
  end loop;
end Main;' ''

create loop_count_up 'procedure Main is
  x : Integer := 1;
begin
  while 4 > x loop
    Put_Num(x);
    x := x + 1;
  end loop;
end Main;' '123'

create loop_nested 'procedure Main is
  x : Integer := 2;
  y : Integer := 2;
begin
  while x > 0 loop
    y := 2;
    while y > 0 loop
      Put_Num(y);
      y := y - 1;
    end loop;
    x := x - 1;
  end loop;
end Main;' '2121'

create loop_logic 'procedure Main is
  x : Integer := 5;
begin
  while x > 0 loop
    if x > 3 then
       Put_Num(x);
    end if;
    x := x - 1;
  end loop;
end Main;' '54'

create loop_pow_limit 'procedure Main is
  x : Integer := 1;
begin
  while 10 > x loop
    x := x * 2;
  end loop;
  Put_Num(x);
end Main;' '16'

create loop_sum 'procedure Main is
  i : Integer := 1;
  s : Integer := 0;
begin
  while 5 > i loop
    s := s + i;
    i := i + 1;
  end loop;
  Put_Num(s);
end Main;' '10'

create loop_put_line 'procedure Main is
  x : Integer := 2;
begin
  while x > 0 loop
    Put_Line("A");
    x := x - 1;
  end loop;
end Main;' 'AA'

create loop_complex_cond 'procedure Main is
  x : Integer := 6;
begin
  while (x > 0) and (x > 3) loop
    Put_Num(x);
    x := x - 1;
  end loop;
end Main;' '654'

create loop_var_init 'procedure Main is
  x : Integer := 3;
begin
  while x > 0 loop
    x := x - 1;
  end loop;
  Put_Num(x);
end Main;' '0'
