program ListChannels;

uses
  Forms,
  main in 'main.pas' {MainForm},
  detail in 'detail.pas' {DetailForm},
  CAN in 'CAN.PAS';

{$R *.RES}

begin
  Application.Initialize;
  Application.CreateForm(TMainForm, MainForm);
  Application.CreateForm(TDetailForm, DetailForm);
  Application.Run;
end.
