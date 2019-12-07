unit About;

interface

uses
  Windows, Messages, SysUtils, Variants, Classes, Graphics, Controls, Forms,
  Dialogs, StdCtrls, ShellApi;

type
  TAboutForm = class(TForm)
    lbAppName: TLabel;
    lbOriginalAuthor: TLabel;
    lbAuthor: TLabel;
    lbOriginalInfo: TLabel;
    btnClose: TButton;
    lbPenoffLink: TLabel;
    procedure btnCloseKeyPress(Sender: TObject; var Key: Char);
    procedure lbPenoffLinkClick(Sender: TObject);
  private
    { Private declarations }
  public
    { Public declarations }
  end;

var
  AboutForm: TAboutForm;

procedure ShowAbout;

implementation

Uses Main;

procedure ShowAbout; { procedure for creating and destroying the About window }
begin
  with TAboutForm.Create(Application) do
    try
      ShowModal;
    finally
      Free;
    end;
end;

{$R *.dfm}

procedure TAboutForm.btnCloseKeyPress(Sender: TObject; var Key: Char);
begin
  if key=#27 then Close; {  Close window if Esc is clicked }
end;

procedure TAboutForm.lbPenoffLinkClick(Sender: TObject);
begin
  ShellExecute(0, 'OPEN', PChar(lbPenoffLink.Caption), '', '', SW_SHOWNORMAL);  {  Go to Penoff's website }
end;

end.
