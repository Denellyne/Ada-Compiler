procedure Main is
  V : Integer := 10;
  X : Boolean := True;
begin
  while not (V < 3) or (not (V = 6 and V > 10) and not (not (X))) loop
    Put_Num (V);
    V := V - 1;
  end loop;
end Main;
