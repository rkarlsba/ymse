////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// This tool is used for configuration of system controllers for electric     //
// bicycles manufactured by Bafang. Exact supported models are unknown.       //
// Tested with BBS01 and BB02 mid drive kits                                  //
//                                                                            //
// The original development of this application was made by Bafang and for    //
// some reason released to the public. I have Bafang BBS02 kit on my bicycle  //
// and I was lucky to find that the tool for modifying its configuration is   //
// not only released for public use but it also comes with the full source    //
// code, developed in Delphi 7. I spent a lot of time doing hobby projects    //
// with this compiler and I decided to give a new life to this tool.          //
//                                                                            //
// I have to tell you that the original source code looked like school        //
// homework or project. No offense, please! It was working but was a complete //
// mess combined with quite a few bugs. All controls had just numbers for     //
// names, comments in Chinese, bad unit naming, releasing even temmporary     //
// files, no order of anything, poor GUI design, not very user friendly and   //
// so on. It might be a tool for internal use only but it is still far from   //
// good coding practices. That is why I decided to give it a new touch and    //
// add some useful features. Also fix the bugs of course.                     //
//                                                                            //
// If you wonder who I am and why are you reading this, here is the answer.   //
// My name is Stefan Penov. I am an electronics engineer and such stuff is my //
// hobby. You are reading it because I found it as an open source so I feel   //
// it's fare to give it back to the users with the full source code of the    //
// new version. Feel free to redistribute this tool but keep the information  //
// in the files about my work on it. If you wish to improve it or find more   //
// bugs, feel free to do so. I am sure it's still far from perfect and I know //
// I an not the best software developer also. :)                              //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////
unit Main;

interface

uses
  Windows, SysUtils, Forms, Controls, Graphics, Classes, SPComm, ShellApi,
  ExtCtrls, ComCtrls, Buttons, StdCtrls, Menus, Dialogs, Spin;

type
  TMainForm = class(TForm)
    pGeneral: TPanel;
    lbBafangBrand: TLabel;
    lbSoftwareName: TLabel;
    lbBafangLink: TLabel;
    gbCommIF: TGroupBox;
    lbCOMPort: TLabel;
    btnPortConnect: TBitBtn;
    cbbPort: TComboBox;
    gbControllerInfo: TGroupBox;
    lbManufTitle: TLabel;
    lbModelTitle: TLabel;
    lbHWVerTitle: TLabel;
    lbFWVerTitle: TLabel;
    lbNomVoltTitle: TLabel;
    lbMaxCurrTitle: TLabel;
    MainMenu: TMainMenu;
    mnFile: TMenuItem;
    mnSaveAs: TMenuItem;
    mnLoad: TMenuItem;
    mnHelp: TMenuItem;
    CommPort: TComm;
    TimerConnect: TTimer;
    btnReadFlash: TBitBtn;
    btnWriteFlash: TBitBtn;
    btnPortDisconnect: TBitBtn;
    mnAbout: TMenuItem;
    mnExit: TMenuItem;
    pctrlProfile: TPageControl;
    tabGeneral: TTabSheet;
    tabPedalAssist: TTabSheet;
    tabThrottleHandle: TTabSheet;
    lbBasLowBatProt: TLabel;
    cbbBasLowBat: TComboBox;
    lbBasCurrLimit: TLabel;
    lbBasCurrLimTItle: TLabel;
    lbBasSpdLimTitle: TLabel;
    lbBasAssist0: TLabel;
    lbBasAssist1: TLabel;
    lbBasAssist2: TLabel;
    lbBasAssist3: TLabel;
    lbBasAssist4: TLabel;
    lbBasAssist5: TLabel;
    lbBasAssist6: TLabel;
    lbBasAssist7: TLabel;
    lbBasAssist8: TLabel;
    lbBasAssist9: TLabel;
    cbbBasWheelDiam: TComboBox;
    cbbBasSpdMtrType: TComboBox;
    btnBasWrite: TBitBtn;
    btnBasRead: TBitBtn;
    lbBasSpdMtrSig: TLabel;
    lbBasSpdMtrType: TLabel;
    lbBasWheelDiam: TLabel;
    lbBasAssistTitle: TLabel;
    btnPASWrite: TBitBtn;
    btnPASRead: TBitBtn;
    lbPASKeepCurr: TLabel;
    lbPASStopDecay: TLabel;
    lbPASCurrDecay: TLabel;
    lbPASStopDelay: TLabel;
    cbbPASWorkMode: TComboBox;
    lbPASWorkMode: TLabel;
    lbPASStartDeg: TLabel;
    lbPASSlowStartMode: TLabel;
    cbbPASSlowStartMode: TComboBox;
    lbPASStartCurr: TLabel;
    cbbPASSpdLim: TComboBox;
    lbPASSpdLim: TLabel;
    cbbPASDesigAssist: TComboBox;
    lbPASDesigAssist: TLabel;
    lbPASPedalSensType: TLabel;
    cbbPASPedalSensType: TComboBox;
    lbThrStartVolt: TLabel;
    lbThrEndVolt: TLabel;
    lbThrMode: TLabel;
    lbThrAsistLevel: TLabel;
    lbThrSpdLim: TLabel;
    lbThrStartCurr: TLabel;
    cbbThrMode: TComboBox;
    cbbThrAssistLvl: TComboBox;
    edThrSpeedLim: TComboBox;
    btnThrRead: TBitBtn;
    btnThrWrite: TBitBtn;
    lbUpgradeAuthor: TLabel;
    lbUpgradeAuthorLink: TLabel;
    imgSearchPorts: TImage;
    lbManuf: TLabel;
    lbModel: TLabel;
    lbHWVer: TLabel;
    lbFWVer: TLabel;
    lbNomVolt: TLabel;
    lbMaxCurr: TLabel;
    LoadFileDialog: TOpenDialog;
    SaveFileDialog: TSaveDialog;
    mnSave: TMenuItem;
    speBasCurrLim: TSpinEdit;
    speBasSpdMtrSig: TSpinEdit;
    speBasAssist0CurrLim: TSpinEdit;
    speBasAssist1CurrLim: TSpinEdit;
    speBasAssist2CurrLim: TSpinEdit;
    speBasAssist3CurrLim: TSpinEdit;
    speBasAssist4CurrLim: TSpinEdit;
    speBasAssist5CurrLim: TSpinEdit;
    speBasAssist6CurrLim: TSpinEdit;
    speBasAssist7CurrLim: TSpinEdit;
    speBasAssist8CurrLim: TSpinEdit;
    speBasAssist9CurrLim: TSpinEdit;
    speBasAssist0SpdLim: TSpinEdit;
    speBasAssist1SpdLim: TSpinEdit;
    speBasAssist2SpdLim: TSpinEdit;
    speBasAssist3SpdLim: TSpinEdit;
    speBasAssist4SpdLim: TSpinEdit;
    speBasAssist5SpdLim: TSpinEdit;
    speBasAssist6SpdLim: TSpinEdit;
    speBasAssist7SpdLim: TSpinEdit;
    speBasAssist8SpdLim: TSpinEdit;
    speBasAssist9SpdLim: TSpinEdit;
    spePASStartCurr: TSpinEdit;
    spePASStartDeg: TSpinEdit;
    spePASStopDelay: TSpinEdit;
    spePASCurrDecay: TSpinEdit;
    spePASStopDecay: TSpinEdit;
    spePASKeepCurr: TSpinEdit;
    speThrStartVolt: TSpinEdit;
    speThrEndVolt: TSpinEdit;
    speThrStartCurr: TSpinEdit;
    mnHelpFile: TMenuItem;
    procedure btnPortConnectClick(Sender: TObject);
    procedure TimerConnectTimer(Sender: TObject);
    procedure FormShow(Sender: TObject);
    procedure btnBasReadClick(Sender: TObject);
    procedure CommPortReceiveData(Sender: TObject; Buffer: Pointer;
      BufferLength: Word);
    procedure btnBasWriteClick(Sender: TObject);
    procedure btnThrReadClick(Sender: TObject);
    procedure btnThrWriteClick(Sender: TObject);
    procedure btnPASReadClick(Sender: TObject);
    procedure btnPASWriteClick(Sender: TObject);
    procedure btnReadFlashClick(Sender: TObject);
    procedure btnWriteFlashClick(Sender: TObject);
    procedure mnSaveAsClick(Sender: TObject);
    procedure mnLoadClick(Sender: TObject);
    procedure btnPortDisconnectClick(Sender: TObject);
    procedure lbBafangLinkClick(Sender: TObject);
    procedure mnAboutClick(Sender: TObject);
    procedure mnExitClick(Sender: TObject);
    procedure lbUpgradeAuthorLinkClick(Sender: TObject);
    procedure imgSearchPortsClick(Sender: TObject);
    procedure LoadFileDialogCanClose(Sender: TObject;
      var CanClose: Boolean);
    procedure SaveFileDialogCanClose(Sender: TObject;
      var CanClose: Boolean);
    procedure mnSaveClick(Sender: TObject);
    procedure cbbBasLowBatKeyPress(Sender: TObject; var Key: Char);
    procedure cbbBasLowBatChange(Sender: TObject);
    procedure mnHelpFileClick(Sender: TObject);
  private
    { Private declarations }
  public
    { Public declarations }
    next_op: integer;   // next operation marker for read write process
    ActiveFile: string; // Filename of the currently active file (if any)
  end;

var
  MainForm: TMainForm;

const
  baudrate = 1200;  // all controllers should operate properly at 1200
  rdIgnore = 0;     // marker - ignore further received data
  rdSingle = 1;     // marker - single block read
  wrSingle = 2;     // marker - single block write
  rdAll    = 3;     // marker - full flash read
  wrAll    = 4;     // marker - full flash write
  rdGEN    = 5;     // marker - General data block read
  GEN = $51;        // used in device read/write
  BAS = $52;        // used in device read/write
  PAS = $53;        // used in device read/write
  THR = $54;        // used in device read/write

implementation

uses
  About, CommonFunctions, Communication;

{$R *.dfm}

//Initialization on show
procedure TMainForm.FormShow(Sender: TObject);
var
  i: integer;
begin
  TimerConnect.Enabled := false; // No port selected yet so this must be disabled for sure
  ClientWidth     := 678;
  ClientHeight    := 586;
  ActiveFile      := '';    // DefaultProfile.el should not be changed
  mnSave.Enabled  := false; // DefaultProfile.el should not be changed
  next_op         := 0;     // ignore incoming data

  cbbPort.Items             := ListPorts;
  cbbPort.ItemIndex         :=  0;
  btnPortConnect.Enabled    := true;
  btnPortDisconnect.Enabled := false;

  pctrlProfile.ActivePageIndex := 0;

  // Create wheel size list
  cbbBasWheelDiam.Items.Clear;
  for i := 16 to 30 do
    cbbBasWheelDiam.Items.Add(IntToStr(i));
  cbbBasWheelDiam.Items.Insert(cbbBasWheelDiam.Items.IndexOf('28'), '700C');

  // Create speed meter type list
  cbbBasSpdMtrType.Items.Clear;
  cbbBasSpdMtrType.Items.Add('External, Wheel Meter');
  cbbBasSpdMtrType.Items.Add('Internal, Motor Meter');
  cbbBasSpdMtrType.Items.Add('By Motor Phase');

  // Disable all read and write buttons
  btnThrWrite.Enabled   := false;
  btnThrRead.Enabled    := false;
  btnPASWrite.Enabled   := false;
  btnPASRead.Enabled    := false;
  btnBasWrite.Enabled   := false;
  btnBasRead.Enabled    := false;
  btnWriteFlash.Enabled := false;
  btnReadFlash.Enabled  := false;

  LoadFromFile(ExtractFilePath(Application.ExeName) + 'Profiles\DefaultProfile.el'); // get defaults file
end;

////////////////////////////////////////////////////////////////////////////////
//////////////////////////// Main menu calls start /////////////////////////////
////////////////////////////////////////////////////////////////////////////////

// File menu
procedure TMainForm.mnSaveClick(Sender: TObject);
begin
  if ActiveFile <> '' then
  begin
    if FileExists(ActiveFile) then
      SaveToFile(ActiveFile)
    else
      Application.MessageBox(PChar('File "' + ActiveFile + '" not found!'), 'File load error', MB_ICONERROR + mb_OK);
  end
  else
    mnSave.Enabled := false;  // There is no active file so disable saving for it
end;

procedure TMainForm.mnSaveAsClick(Sender: TObject);
begin
  SaveFileDialog.InitialDir := ExtractFilePath(Application.ExeName) + 'Profiles';
  SaveFileDialog.Execute;
end;

procedure TMainForm.mnLoadClick(Sender: TObject);
begin
  LoadFileDialog.InitialDir := ExtractFilePath(Application.ExeName) + 'Profiles';
  LoadFileDialog.Execute;
end;

procedure TMainForm.mnExitClick(Sender: TObject);
begin
  Close;
end;

// Help menu
procedure TMainForm.mnHelpFileClick(Sender: TObject);
begin
  ShellExecute(Handle, 'open', PAnsiChar(ExtractFilePath(Application.ExeName) + 'Help.pdf'), nil, nil, SW_SHOWNORMAL) ;
end;

procedure TMainForm.mnAboutClick(Sender: TObject);
begin
  ShowAbout;
end;

////////////////////////////////////////////////////////////////////////////////
///////////////////////////// Main menu calls end //////////////////////////////
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
/////////////////////////// pGeneral Functions start ///////////////////////////
////////////////////////////////////////////////////////////////////////////////

procedure TMainForm.lbBafangLinkClick(Sender: TObject);
begin
  ShellExecute(0, 'OPEN', PChar(lbBafangLink.Caption), '', '', SW_SHOWNORMAL);  // Go to BaFang's website
end;

procedure TMainForm.lbUpgradeAuthorLinkClick(Sender: TObject);
begin
  ShellExecute(0, 'OPEN', PChar('https://penoff.wordpress.com'), '', '', SW_SHOWNORMAL);  // Go to Penoff's website
end;

procedure TMainForm.imgSearchPortsClick(Sender: TObject);
begin
  cbbPort.Items     := ListPorts;
  cbbPort.ItemIndex := 0;
end;

procedure TMainForm.btnPortConnectClick(Sender: TObject);
begin
  // Connect to COM port
  CommPort.CommName := cbbPort.Text;
  CommPort.BaudRate := baudrate;
  CommPort.StartComm;
  // Enable disconnect button and search for controller
  TimerConnect.Enabled      := false;
  btnPortDisconnect.Enabled := true;
  btnPortConnect.Enabled    := false;
  next_op := rdGEN;
  ConnectDev;
end;

procedure TMainForm.btnPortDisconnectClick(Sender: TObject);
begin
  // Disconnect from COM port
  TimerConnect.Enabled      := false;
  CommPort.StopComm;
  btnPortDisconnect.Enabled := false;
  btnPortConnect.Enabled    := true;
  // Disable all read and write buttons
  btnThrWrite.Enabled       := false;
  btnThrRead.Enabled        := false;
  btnPASWrite.Enabled       := false;
  btnPASRead.Enabled        := false;
  btnBasWrite.Enabled       := false;
  btnBasRead.Enabled        := false;
  btnWriteFlash.Enabled     := false;
  btnReadFlash.Enabled      := false;
end;

////////////////////////////////////////////////////////////////////////////////
//////////////////////////// pGeneral Functions end ////////////////////////////
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
/////////////////////////// Controls' functions start //////////////////////////
////////////////////////////////////////////////////////////////////////////////

// Ensures digital entry in Low Battery Protection field
procedure TMainForm.cbbBasLowBatKeyPress(Sender: TObject; var Key: Char);
begin
  if not(key in['0'..'9',#8])then key := #0;
end;

// Ensures entry within limits in Low Battery Protection field
procedure TMainForm.cbbBasLowBatChange(Sender: TObject);
begin
  if Length(cbbBasLowBat.Text) > 0 then // check for empty field
  begin
    if StrToInt(cbbBasLowBat.Text) > StrToInt(cbbBasLowBat.Items[cbbBasLowBat.Items.Count-1]) then  // if above upper limit
      cbbBasLowBat.ItemIndex := cbbBasLowBat.Items.Count - 1;                                       // set to maximum if originally above limit
    if StrToInt(cbbBasLowBat.Text) < StrToInt(cbbBasLowBat.Items[0]) then                           // if below lower limit
      cbbBasLowBat.ItemIndex := 0;                                                                  // set to minimum if below limit
  end;
end;

procedure TMainForm.TimerConnectTimer(Sender: TObject);
begin
   TimerConnect.Enabled := false;
   ConnectDev;
end;

// Load profile
procedure TMainForm.LoadFileDialogCanClose(Sender: TObject;
  var CanClose: Boolean);
begin
  if LoadFileDialog.FileName <> '' then
  begin
    if FileExists(LoadFileDialog.FileName) then
    begin
      ActiveFile        := LoadFileDialog.FileName; // Keep the file name
      mnSave.Enabled    := true;                    // Enable saving to active file
      MainForm.Caption  := 'Bafang Configuration Tool - ' + ExtractFileName(LoadFileDialog.FileName);
      LoadFromFile(LoadFileDialog.FileName);
    end
    else
      Application.MessageBox(PChar('File "' + LoadFileDialog.FileName + '" not found!'), 'File load error', MB_ICONERROR + mb_OK);
  end;
end;

// Save As... profile
procedure TMainForm.SaveFileDialogCanClose(Sender: TObject;
  var CanClose: Boolean);
begin
  if SaveFileDialog.FileName <> '' then // File selected?
  begin
    if FileExists(SaveFileDialog.FileName) then // if file already exists ask user to replace it, if denied exit the procedure
      if Application.MessageBox('File already exists. Do you want to replace it?', 'Question', mb_YESNO) = IDNO then Exit;

    if ExtractFileExt(SaveFileDialog.FileName) <> '.el' then
      begin
        ActiveFile := PChar(SaveFileDialog.FileName + '.el'); // Keep the file name
        SaveToFile(ActiveFile);
      end
    else
      begin
        ActiveFile := SaveFileDialog.FileName; // Keep the file name
        SaveToFile(ActiveFile);
      end;
    mnSave.Enabled    := true; // Enable saving to active file
    MainForm.Caption  := 'Bafang Configuration Tool - ' + ExtractFileName(ActiveFile);
  end;
end;

////////////////////////////////////////////////////////////////////////////////
/////////////////////////// Controls' functions end ////////////////////////////
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
///////////////////////// Read and write buttons start /////////////////////////
////////////////////////////////////////////////////////////////////////////////

procedure TMainForm.btnBasReadClick(Sender: TObject);
begin
  next_op := rdSingle;
  ReadDev(BAS); // Read location for Basic settings block
end;

procedure TMainForm.btnPASReadClick(Sender: TObject);
begin
  next_op := rdSingle;
  ReadDev(PAS); // Read location for PAS settings block
end;

procedure TMainForm.btnThrReadClick(Sender: TObject);
begin
  next_op := rdSingle;
  ReadDev(THR); // Read location for Throttle Handle settings block
end;

procedure TMainForm.btnBasWriteClick(Sender: TObject);
begin
  next_op := wrSingle;
  WriteDev(BAS); // Write settings to controller
end;

procedure TMainForm.btnPASWriteClick(Sender: TObject);
begin
  next_op := wrSingle;
  WriteDev(PAS); // Write settings to controller
end;

procedure TMainForm.btnThrWriteClick(Sender: TObject);
begin
  next_op := wrSingle;
  WriteDev(THR); // Write settings to controller
end;

procedure TMainForm.btnReadFlashClick(Sender: TObject);
begin
  next_op := rdAll;
  ReadDev(BAS); // Write settings to controller
end;

procedure TMainForm.btnWriteFlashClick(Sender: TObject);
begin
  next_op := wrAll;
  WriteDev(BAS); // Write settings to controller
end;

////////////////////////////////////////////////////////////////////////////////
////////////////////////// Read and write buttons end //////////////////////////
////////////////////////////////////////////////////////////////////////////////

// COM port received data processing
procedure TMainForm.CommPortReceiveData(Sender: TObject; Buffer: Pointer;
  BufferLength: Word);
var
  redata: array[0..290] of byte;
begin
  move(Buffer^, pchar(@redata)^, BufferLength); // copy data from COM buffer
  case redata[0] of
  GEN: if (next_op = rdGEN) then DecodeGEN(redata); // if General data requested, decode it
  BAS: if (next_op = rdSingle) or (next_op = rdAll) then DecodeBAS(redata) else // if BAS or All data requested, decode BAS (and continue if All)
       if (next_op = wrSingle) or (next_op = wrAll) then DecodeBASerr(redata);  // if BAS or All data written, decode BAS response (and continue if All)
  PAS: if (next_op = rdSingle) or (next_op = rdAll) then DecodePAS(redata) else // if PAS or All data requested, decode PAS (and continue if All)
       if (next_op = wrSingle) or (next_op = wrAll) then DecodePASerr(redata);  // if PAS or All data written, decode PAS response (and continue if All)
  THR: if (next_op = rdSingle) or (next_op = rdAll) then DecodeTHR(redata) else // if THR or All data requested, decode THR (and and discard further data)
       if (next_op = wrSingle) or (next_op = wrAll) then DecodeTHRerr(redata);  // if THR or All data written, decode THR response (and discard further data)
  end;
end;


end.
