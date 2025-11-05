procedure Main is
  a : Boolean;
  b : String := False;
begin
  if V > 5 then
    Compute_A (V - 1);
  end if;
  while V > 5 loop
    V := V - 1;
  end loop;

  if V > 5 then
    V := 5 * 2;
  elsif V > 6 then
    Put_Line ("OIIAI");
  else
    Get_Line (True);
  end if;
end Main;
