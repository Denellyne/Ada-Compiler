procedure Main is
begin
  while V > 5 loop
    V := V - 1;
    Put_Line ("noice");
    thisFunctionDoesntExist (True);
  end loop;

end Main;
