#!/bin/sh

rm *.adb *.txt

create() {
  name="$1"
  code="$2"
  out="$3"
  printf '%s\n' "$code" >"$name.adb"
  printf '%s\n' "$out" >"$name.adb.txt"
}

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

create pow 'procedure Main is
  x : Integer := 5;
begin
  x := x**5;
  Put_Num (x);

end Main;' '3125'

create print 'procedure Main is
  str : String := "test";
begin
  Put_Line ("aaa");
  Put_Line (str);
end Main;' 'aaatest'

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
end Main;' '10987'
