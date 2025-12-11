#!/bin/sh

rm -f *.in *.adb *.bin *.txt *.out >/dev/null 2>&1

create() {
  name="$1"
  code="$2"
  out="$3"
  in="$4"

  printf '%s\n' "$code" >"$name.adb"
  printf '%s\n' "$out" >"$name.adb.txt"
  printf '%s' "$in" >"$name.adb.in"
}

create floatSubTest 'procedure Main is
  X : Float := 5.0;
  Y : Float := 2.0;
begin
  Put_Num (X - Y);
end Main;' '3.0'

create floatMultTest 'procedure Main is
  X : Float := 2.0;
  Y : Float := 3.0;
begin
  Put_Num (X + Y * 2.0);
end Main;' '8.0'

create floatDivTest 'procedure Main is
  X : Float := 7.5;
  Y : Float := 2.5;
begin
  Put_Num (X / Y);
end Main;' '3.0'
create floatAddTest 'procedure Main is
  X : Float := 1.0;
  Y : Float := 2.5;
  Z : Float := 3.5;
begin
  Put_Num (X + Y + Z);
end Main;' '7.0'
create floatPowTest 'procedure Main is
  V : Float := 2.0;
begin
  V := V ** 20;
  Put_Num(V);
end Main;' '1048576.0'
create floatTest 'procedure Main is
  V : Float := 3.141591;
begin
  V := V + 1.0;
  Put_Num(V);
end Main;' '4.141591'

create ifsmall7 'procedure Main is
  V : Boolean := False;
  X : String := "a";
begin
  if V  then
    Put_Num (0);
  else
    Put_Num (1);
  end if;
end Main;' '1'

create ifsmall 'procedure Main is
  V : Boolean := True;
  X : String := "a";
begin
  if V and 3 > 1 then
    Put_Num (0);
  else
    Put_Num (1);
  end if;
end Main;' '0'

create ifsmall2 'procedure Main is
  V : Integer := 3;
begin
  if V >= 5 then
    V := 5 * 2;
  end if;
  Put_Num (V);
end Main;' '3'

create ifsmall3 'procedure Main is
  V : Integer := 10;
begin
  if V <= 5 then
    V := 15;
    Put_Num (V);
  else
    Put_Line ("SIGMA");
  end if;
end Main;' 'SIGMA'

create ifsmall4 'procedure Main is
  V : Integer := 10;
begin
  if False or V >= 10 then
    V := 15;
    Put_Num (V);
  else
    Put_Line ("SIGMA\n");
  end if;
end Main;' '15'

create ifsmall5 'procedure Main is
  V : Integer := 10;
begin
  if not (V < 3) then
    V := 15;
    Put_Line ("15");
  else
    Put_Line ("SIGMA\n");
  end if;
end Main;' '15'

create ifsmall6 'procedure Main is
  V : Boolean := True;
begin
  if not (0) then
    Put_Line ("15");
  else
    Put_Line ("SIGMA\n");
  end if;
end Main;' '15'

create add 'procedure Main is
  x : Integer := 5;
  y : Integer := 3;
  z : Integer := 5;
begin
  x := x+y+z;
  Put_Num (x);

end Main;' '13'

create pow 'procedure Main is
  x : Integer := 5;
begin
  x := x**5;
  Put_Num (x);

end Main;' '3125'

create print 'procedure Main is
  str : String := "test\n";
  num : Integer := 5;
begin
  Put_Line ("aaa");
  Put_Line (str);
  Get_Line (str,num);
  Put_Line (str);
  Put_Num (num);
end Main;' 'aaatest
lindo
5' 'lindo'

create blanco 'procedure Main is
  z : Integer;
  p : String;
  k : String := "oSASASa";
  l : String := "EE";
begin
  Get_Line (k, z);
  Put_Line (k);
  l := "ola";
  l := "ola";
  l := "ola";
  l := "ola";
  l := "ola";
  l := "ola";
  l := "ola";
  l := "adeus";
  l := k;
  Put_Line (l);
  Put_Line (k);
end Main;' 'blanco
blanco
blanco
' 'blanco'

create blanco2 'procedure Main is
  score : Integer := 75;
begin
  if score >= 60 then
    Put_Line("Aprovado");
  else
    if score >= 50 then
      Put_Line("Suficiente");
    else
      Put_Line("Reprovado");
    end if;
  end if;
end Main;' 'Aprovado' ''

create blanco3 'procedure Main is
  x : Integer := -5;
  y : Integer := 10;
  b : Boolean := True;
begin

  if (not (x = y)) and (x > -y) or (not b) then
    Put_Line("Resultado verdadeiro");
  else
    Put_Line("Resultado falso");
  end if;
end Main;' 'Resultado verdadeiro' ''

create whilesmall 'procedure Main is
  V : Integer := 10;
begin
  while V > 5 loop
    V := V - 1;
    Put_Line ("noice");
  end loop;

end Main; ' 'noicenoicenoicenoicenoice'

create while 'procedure Main is
  V : Integer := 10;
  X : Boolean := True;
begin
  while not (V < 3) or (not (V = 6 and V > 10) and not (not (X))) loop
    Put_Num (V);
    V := V - 1;
  end loop;
end Main;' '109876543'

create xor 'procedure Main is
  V : Integer := 5;
begin
  Put_Num(V xor 1);
end Main;' '4'
