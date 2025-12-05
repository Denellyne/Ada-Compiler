procedure Main is
  V : Integer := 4;
  X : Boolean := True;
begin
  while not (V < 3) and V >= 5 loop
    Put_Num (V);
    V := V - 1;
  end loop;
end Main;
