procedure Main is
  V : Integer := 10;
  X : Boolean := True;
  Z : Boolean := False;
begin
  if not (V < 3) and not (not (X)) then
    V := 15;
    Put_Num (V);
  else
    Put_Line ("SIGMA\n");
  end if;
end Main;
