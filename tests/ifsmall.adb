procedure Main is
  V : Boolean := True;
  X : String := "a";
begin
  if V and True or V then
    Put_Num (0);
  else
    Put_Num (1);
  end if;
end Main;
