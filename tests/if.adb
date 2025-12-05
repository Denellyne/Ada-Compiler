procedure Main is
  V : Integer := 10;
begin
  if V > 5 then
    V := V - 1;
  end if;
  while V > 5 loop
    V := V - 1;
  end loop;

  if V > 5 then
    V := 5 * 2;
  else
    Get_Line (True);
  end if;
end Main;
