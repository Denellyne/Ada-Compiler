procedure Main is
  V : Integer := 10;
begin
  if V > 5 then
    V := 5 * 2;
  elsif V > 6 then
    Put_Line ("OIIAI");
  elsif V = 3 then
    s := "a";
  else
    V := 10;
    Get_Line (True);
  end if;
end Main;
