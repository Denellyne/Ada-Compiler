procedure Main is
  V : Integer := 10;
  X : Boolean := True;
  Z : Boolean := False;
begin
  if not (V < 3) or (not (V = 6 and V > 10) and not (not (X))) and False then
    V := 15;
    Put_Line ("15");
  else
    Put_Line ("SIGMA\n");
  end if;
end Main;
