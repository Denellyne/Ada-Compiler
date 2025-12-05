procedure Main is
  V : Integer := 10;
begin
  if False or V >= 10 then
    V := 15;
    Put_Num (V);
  else
    Put_Line ("SIGMA\n");
  end if;
end Main;
