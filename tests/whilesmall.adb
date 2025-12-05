procedure Main is
  V : Integer := 2;
  X : Boolean := True;
begin
  while not (V < 3) or (not (V = 6 and V > 10) and not (not (X))) and V >= 5
  loop
    Put_Num (V);
    V := V - 1;
  end loop;
end Main;
