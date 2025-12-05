procedure Main is
  a : Integer := 5;
begin
  a := a xor 1;
  Put_Num (a);
  Put_Line ("\n");
  a := a or 9;
  Put_Num (a);
  Put_Line ("\n");
  a := a and 15;
  Put_Num (a);
end Main;
