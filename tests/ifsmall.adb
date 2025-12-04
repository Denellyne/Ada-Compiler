procedure Main is
  V : Integer := 10;
begin
  if V > 5 then
    V := 5 * 10;
  else
    V := 10;
  end if;
  Put_Num (V);
end Main;
