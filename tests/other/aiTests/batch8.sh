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
create f1_add_simple 'procedure Main is
  X : Float := 1.0;
  Y : Float := 2.5;
begin
  Put_Num (X + Y);
end Main;' '3.5'

create f2_sub_simple 'procedure Main is
  X : Float := 5.0;
  Y : Float := 2.0;
begin
  Put_Num (X - Y);
end Main;' '3.0'

create f3_mul_simple 'procedure Main is
  X : Float := 1.5;
  Y : Float := 2.0;
begin
  Put_Num (X * Y);
end Main;' '3.0'

create f4_div_simple 'procedure Main is
  X : Float := 7.5;
  Y : Float := 2.5;
begin
  Put_Num (X / Y);
end Main;' '3.0'

create f5_pow_int_exp 'procedure Main is
  X : Float := 2.0;
begin
  Put_Num (X ** 3);
end Main;' '8.0'

create f6_pow_one 'procedure Main is
  X : Float := 3.5;
begin
  Put_Num (X ** 1);
end Main;' '3.5'

create f7_pow_zero 'procedure Main is
  X : Float := 4.0;
begin
  Put_Num (X ** 0);
end Main;' '1.0'

create f8_add_chain 'procedure Main is
  X : Float := 1.0;
  Y : Float := 2.0;
  Z : Float := 3.0;
begin
  Put_Num (X + Y + Z);
end Main;' '6.0'

create f9_mix_add_sub 'procedure Main is
  X : Float := 10.0;
  Y : Float := 2.5;
begin
  Put_Num (X - Y + 1.5);
end Main;' '9.0'

create f10_mix_mul_add 'procedure Main is
  X : Float := 2.0;
  Y : Float := 3.0;
begin
  Put_Num (X * Y + 1.0);
end Main;' '7.0'

create f11_prec_mul_add 'procedure Main is
  X : Float := 2.0;
  Y : Float := 3.0;
begin
  Put_Num (X + Y * 2.0);
end Main;' '8.0'

create f12_prec_parens 'procedure Main is
  X : Float := 2.0;
  Y : Float := 3.0;
begin
  Put_Num ((X + Y) * 2.0);
end Main;' '10.0'

create f13_div_chain 'procedure Main is
  X : Float := 16.0;
begin
  Put_Num (X / 2.0 / 2.0);
end Main;' '4.0'

create f14_neg_result 'procedure Main is
  X : Float := 2.0;
  Y : Float := 5.0;
begin
  Put_Num (X - Y);
end Main;' '-3.0'

create f15_small_fracs 'procedure Main is
  X : Float := 0.5;
  Y : Float := 0.25;
begin
  Put_Num (X + Y);
end Main;' '0.75'

create f16_frac_mul 'procedure Main is
  X : Float := 0.5;
  Y : Float := 0.5;
begin
  Put_Num (X * Y);
end Main;' '0.25'

create f17_frac_div 'procedure Main is
  X : Float := 0.5;
  Y : Float := 0.25;
begin
  Put_Num (X / Y);
end Main;' '2.0'

create f18_pow_frac_base 'procedure Main is
  X : Float := 0.5;
begin
  Put_Num (X ** 2);
end Main;' '0.25'

create f19_pow_even 'procedure Main is
  X : Float := 1.5;
begin
  Put_Num (X ** 2);
end Main;' '2.25'

create f20_pow_odd 'procedure Main is
  X : Float := 1.5;
begin
  Put_Num (X ** 3);
end Main;' '3.375'

create f21_var_assign 'procedure Main is
  X : Float := 1.0;
begin
  X := X + 2.0;
  X := X * 3.0;
  Put_Num (X);
end Main;' '9.0'

create f22_two_vars 'procedure Main is
  X : Float := 2.5;
  Y : Float := 4.5;
begin
  Put_Num (X + Y);
end Main;' '7.0'

create f23_div_not_int 'procedure Main is
  X : Float := 1.0;
  Y : Float := 4.0;
begin
  Put_Num (X / Y);
end Main;' '0.25'

create f24_complex_expr 'procedure Main is
  X : Float := 2.0;
  Y : Float := 3.0;
  Z : Float := 4.0;
begin
  Put_Num ((X + Y) / Z);
end Main;' '1.25'
