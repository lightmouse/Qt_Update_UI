program Test1;

uses
  Forms,
  T1main in 'T1MAIN.PAS' {Form1},
  CAN in 'CAN.PAS',
  CanChanEx in 'canchanex.pas';

{$R *.RES}

begin
  Application.CreateForm(TForm1, Form1);
  Application.Run;
end.
