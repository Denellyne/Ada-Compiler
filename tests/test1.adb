procedure Main is
  a : Integer := 5;
begin
  if not (a < 4) then
    Put_Num (a);
  else
    Put_Line ("Can't Happen");
  end if;
end Main;
