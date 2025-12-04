procedure Main is
  V : Integer := 10;
begin
  if  not (V < 3) or  not (V = 6 and V > 10) then
    V := 15;
    Put_Num (V);
  else
    Put_Line ("SIGMA\n");
  end if;
end Main;
