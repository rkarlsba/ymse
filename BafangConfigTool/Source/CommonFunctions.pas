unit CommonFunctions;

interface

uses
  SysUtils, IniFiles, Main;

  procedure LoadFromFile(FileName: string);   {  Loads profile data from file }
  procedure SaveToFile(FileName: string);     {  Saves profile data to file }
  function ByteArrToStr(data_in: array of byte; start: integer; length: integer): string; {  convert byte array to string }

implementation

{  Load profile }
procedure LoadFromFile(FileName: string);
var
  ProfileFile: TInifile;
begin
  ProfileFile := TInifile.Create(FileName);
  {  Load from file }
  {  Load basic settings }
  MainForm.cbbBasLowBat.Text             := IntToStr(ProfileFile.Readinteger('Basic','LBP',10));
  MainForm.speBasCurrLim.Value           := ProfileFile.Readinteger('Basic','LC',10);
  MainForm.speBasAssist0CurrLim.Value    := ProfileFile.Readinteger('Basic','ALC0',10);
  MainForm.speBasAssist1CurrLim.Value    := ProfileFile.Readinteger('Basic','ALC1',10);
  MainForm.speBasAssist2CurrLim.Value    := ProfileFile.Readinteger('Basic','ALC2',10);
  MainForm.speBasAssist3CurrLim.Value    := ProfileFile.Readinteger('Basic','ALC3',10);
  MainForm.speBasAssist4CurrLim.Value    := ProfileFile.Readinteger('Basic','ALC4',10);
  MainForm.speBasAssist5CurrLim.Value    := ProfileFile.Readinteger('Basic','ALC5',10);
  MainForm.speBasAssist6CurrLim.Value    := ProfileFile.Readinteger('Basic','ALC6',10);
  MainForm.speBasAssist7CurrLim.Value    := ProfileFile.Readinteger('Basic','ALC7',10);
  MainForm.speBasAssist8CurrLim.Value    := ProfileFile.Readinteger('Basic','ALC8',10);
  MainForm.speBasAssist9CurrLim.Value    := ProfileFile.Readinteger('Basic','ALC9',10);
  MainForm.speBasAssist0SpdLim.Value     := ProfileFile.Readinteger('Basic','ALBP0',10);
  MainForm.speBasAssist1SpdLim.Value     := ProfileFile.Readinteger('Basic','ALBP1',10);
  MainForm.speBasAssist2SpdLim.Value     := ProfileFile.Readinteger('Basic','ALBP2',10);
  MainForm.speBasAssist3SpdLim.Value     := ProfileFile.Readinteger('Basic','ALBP3',10);
  MainForm.speBasAssist4SpdLim.Value     := ProfileFile.Readinteger('Basic','ALBP4',10);
  MainForm.speBasAssist5SpdLim.Value     := ProfileFile.Readinteger('Basic','ALBP5',10);
  MainForm.speBasAssist6SpdLim.Value     := ProfileFile.Readinteger('Basic','ALBP6',10);
  MainForm.speBasAssist7SpdLim.Value     := ProfileFile.Readinteger('Basic','ALBP7',10);
  MainForm.speBasAssist8SpdLim.Value     := ProfileFile.Readinteger('Basic','ALBP8',10);
  MainForm.speBasAssist9SpdLim.Value     := ProfileFile.Readinteger('Basic','ALBP9',10);
  MainForm.cbbBasWheelDiam.ItemIndex     := ProfileFile.Readinteger('Basic','WD',10);
  MainForm.cbbBasSpdMtrType.ItemIndex    := ProfileFile.Readinteger('Basic','SMM',0);
  MainForm.speBasSpdMtrSig.Value         := ProfileFile.Readinteger('Basic','SMS',10);
  {  Load pedal assist settings }
  MainForm.cbbPASPedalSensType.ItemIndex := ProfileFile.Readinteger('Pedal Assist','PT',0);
  MainForm.cbbPASDesigAssist.ItemIndex   := ProfileFile.Readinteger('Pedal Assist','DA',0);
  MainForm.cbbPASSpdLim.ItemIndex        := ProfileFile.Readinteger('Pedal Assist','SL',0);
  MainForm.cbbPASSlowStartMode.ItemIndex := ProfileFile.Readinteger('Pedal Assist','SSM',0);
  MainForm.cbbPASWorkMode.ItemIndex      := ProfileFile.Readinteger('Pedal Assist','WM',0);
  MainForm.spePASStartCurr.Value         := ProfileFile.Readinteger('Pedal Assist','SC',10);
  MainForm.spePASStartDeg.Value          := ProfileFile.Readinteger('Pedal Assist','SDN',10);
  MainForm.spePASStopDelay.Value         := ProfileFile.Readinteger('Pedal Assist','TS',10);
  MainForm.spePASCurrDecay.Value         := ProfileFile.Readinteger('Pedal Assist','CD',10);
  MainForm.spePASStopDecay.Value         := ProfileFile.Readinteger('Pedal Assist','SD',10);
  MainForm.spePASKeepCurr.Value          := ProfileFile.Readinteger('Pedal Assist','KC',10);
  {  Load throttle handle settings }
  MainForm.speThrStartVolt.Value         := ProfileFile.Readinteger('Throttle Handle','SV',10);
  MainForm.speThrEndVolt.Value           := ProfileFile.Readinteger('Throttle Handle','EV',10);
  MainForm.cbbThrMode.ItemIndex          := ProfileFile.Readinteger('Throttle Handle','MODE',0);
  MainForm.cbbThrAssistLvl.ItemIndex     := ProfileFile.Readinteger('Throttle Handle','DA',0);
  MainForm.edThrSpeedLim.ItemIndex       := ProfileFile.Readinteger('Throttle Handle','SL',0);
  MainForm.speThrStartCurr.Value         := ProfileFile.Readinteger('Throttle Handle','SC',10);

  ProfileFile.Destroy; {  Close file }
end;

{  Save profile }
procedure SaveToFile(FileName: string);
var
  ProfileFile: TInifile;
begin
  ProfileFile := TInifile.Create(FileName);
  {  Save to file }
  {  Save basic settings }
  ProfileFile.WriteInteger('Basic','LBP',StrToInt(MainForm.cbbBasLowBat.Text));
  ProfileFile.WriteInteger('Basic','LC',MainForm.speBasCurrLim.Value);
  ProfileFile.WriteInteger('Basic','ALC0',MainForm.speBasAssist0CurrLim.Value);
  ProfileFile.WriteInteger('Basic','ALC1',MainForm.speBasAssist1CurrLim.Value);
  ProfileFile.WriteInteger('Basic','ALC2',MainForm.speBasAssist2CurrLim.Value);
  ProfileFile.WriteInteger('Basic','ALC3',MainForm.speBasAssist3CurrLim.Value);
  ProfileFile.WriteInteger('Basic','ALC4',MainForm.speBasAssist4CurrLim.Value);
  ProfileFile.WriteInteger('Basic','ALC5',MainForm.speBasAssist5CurrLim.Value);
  ProfileFile.WriteInteger('Basic','ALC6',MainForm.speBasAssist6CurrLim.Value);
  ProfileFile.WriteInteger('Basic','ALC7',MainForm.speBasAssist7CurrLim.Value);
  ProfileFile.WriteInteger('Basic','ALC8',MainForm.speBasAssist8CurrLim.Value);
  ProfileFile.WriteInteger('Basic','ALC9',MainForm.speBasAssist9CurrLim.Value);
  ProfileFile.WriteInteger('Basic','ALBP0',MainForm.speBasAssist0SpdLim.Value);
  ProfileFile.WriteInteger('Basic','ALBP1',MainForm.speBasAssist1SpdLim.Value);
  ProfileFile.WriteInteger('Basic','ALBP2',MainForm.speBasAssist2SpdLim.Value);
  ProfileFile.WriteInteger('Basic','ALBP3',MainForm.speBasAssist3SpdLim.Value);
  ProfileFile.WriteInteger('Basic','ALBP4',MainForm.speBasAssist4SpdLim.Value);
  ProfileFile.WriteInteger('Basic','ALBP5',MainForm.speBasAssist5SpdLim.Value);
  ProfileFile.WriteInteger('Basic','ALBP6',MainForm.speBasAssist6SpdLim.Value);
  ProfileFile.WriteInteger('Basic','ALBP7',MainForm.speBasAssist7SpdLim.Value);
  ProfileFile.WriteInteger('Basic','ALBP8',MainForm.speBasAssist8SpdLim.Value);
  ProfileFile.WriteInteger('Basic','ALBP9',MainForm.speBasAssist9SpdLim.Value);
  ProfileFile.WriteInteger('Basic','WD',MainForm.cbbBasWheelDiam.ItemIndex);
  ProfileFile.WriteInteger('Basic','SMM',MainForm.cbbBasSpdMtrType.ItemIndex);
  ProfileFile.WriteInteger('Basic','SMS',MainForm.speBasSpdMtrSig.Value);
  {  Save pedal assist settings }
  ProfileFile.WriteInteger('Pedal Assist','PT',MainForm.cbbPASPedalSensType.ItemIndex);
  ProfileFile.WriteInteger('Pedal Assist','DA',MainForm.cbbPASDesigAssist.ItemIndex);
  ProfileFile.WriteInteger('Pedal Assist','SL',MainForm.cbbPASSpdLim.ItemIndex);
  ProfileFile.WriteInteger('Pedal Assist','SSM',MainForm.cbbPASSlowStartMode.ItemIndex);
  ProfileFile.WriteInteger('Pedal Assist','WM',MainForm.cbbPASWorkMode.ItemIndex);
  ProfileFile.WriteInteger('Pedal Assist','SC',MainForm.spePASStartCurr.Value);
  ProfileFile.WriteInteger('Pedal Assist','SDN',MainForm.spePASStartDeg.Value);
  ProfileFile.WriteInteger('Pedal Assist','TS',MainForm.spePASStopDelay.Value);
  ProfileFile.WriteInteger('Pedal Assist','CD',MainForm.spePASCurrDecay.Value);
  ProfileFile.WriteInteger('Pedal Assist','SD',MainForm.spePASStopDecay.Value);
  ProfileFile.WriteInteger('Pedal Assist','KC',MainForm.spePASKeepCurr.Value);
  {  Save throttle handle settings }
  ProfileFile.WriteInteger('Throttle Handle','SV',MainForm.speThrStartVolt.Value);
  ProfileFile.WriteInteger('Throttle Handle','EV',MainForm.speThrEndVolt.Value);
  ProfileFile.WriteInteger('Throttle Handle','MODE',MainForm.cbbThrMode.ItemIndex);
  ProfileFile.WriteInteger('Throttle Handle','DA',MainForm.cbbThrAssistLvl.ItemIndex);
  ProfileFile.WriteInteger('Throttle Handle','SL',MainForm.edThrSpeedLim.ItemIndex);
  ProfileFile.WriteInteger('Throttle Handle','SC',MainForm.speThrStartCurr.Value);

  ProfileFile.Destroy;  {  Close file }
end;

{  extracts part of byte array as a string }
{  if length = 0 then converts the whole array }
function ByteArrToStr(data_in: array of byte; start: integer; length: integer): string;
var
  i:  integer; {  counter }
begin
  if length = 0 then {  used to convert the full array to string }
  begin
    for i := 0 to (SizeOf(data_in) - start - 1) do
      Result := Result + Chr(data_in[i + start])
  end
  else {  partial conversion }
  begin
    if length <= (SizeOf(data_in) - start) then
    begin
      for i := 0 to (length - 1) do
        Result := Result + Chr(data_in[i + start])
    end
    else {  convert the whole array if trying to convert more than available }
      begin
        for i := 0 to (SizeOf(data_in) - start - 1) do
          Result := Result + Chr(data_in[i + start])
      end
  end;
end;

end.
