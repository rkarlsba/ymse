object AboutForm: TAboutForm
  Left = 589
  Top = 383
  AlphaBlend = True
  BorderStyle = bsToolWindow
  Caption = 'About'
  ClientHeight = 176
  ClientWidth = 544
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'MS Sans Serif'
  Font.Style = []
  OldCreateOrder = False
  Position = poScreenCenter
  PixelsPerInch = 96
  TextHeight = 13
  object lbAppName: TLabel
    Left = 16
    Top = 12
    Width = 202
    Height = 23
    Caption = 'BafangConfigTool   V2.0'
    Font.Charset = RUSSIAN_CHARSET
    Font.Color = clBlack
    Font.Height = -19
    Font.Name = 'Tahoma'
    Font.Style = []
    ParentFont = False
  end
  object lbOriginalAuthor: TLabel
    Left = 16
    Top = 102
    Width = 498
    Height = 23
    Caption = 'Suzhou Bafang Electric Motor Science-Technology Co., LTD'
    Font.Charset = RUSSIAN_CHARSET
    Font.Color = clBlack
    Font.Height = -19
    Font.Name = 'Tahoma'
    Font.Style = []
    ParentFont = False
  end
  object lbAuthor: TLabel
    Left = 16
    Top = 42
    Width = 225
    Height = 23
    Caption = 'Upgraded by Stefan Penov'
    Font.Charset = RUSSIAN_CHARSET
    Font.Color = clBlack
    Font.Height = -19
    Font.Name = 'Tahoma'
    Font.Style = []
    ParentFont = False
  end
  object lbOriginalInfo: TLabel
    Left = 16
    Top = 72
    Width = 268
    Height = 23
    Caption = 'Original application designed by'
    Font.Charset = RUSSIAN_CHARSET
    Font.Color = clBlack
    Font.Height = -19
    Font.Name = 'Tahoma'
    Font.Style = []
    ParentFont = False
  end
  object lbPenoffLink: TLabel
    Left = 266
    Top = 42
    Width = 250
    Height = 23
    Cursor = crHandPoint
    Caption = 'https://penoff.wordpress.com'
    Font.Charset = RUSSIAN_CHARSET
    Font.Color = clBlue
    Font.Height = -19
    Font.Name = 'Tahoma'
    Font.Style = []
    ParentFont = False
    OnClick = lbPenoffLinkClick
  end
  object btnClose: TButton
    Left = 242
    Top = 136
    Width = 76
    Height = 25
    Caption = 'Close'
    ModalResult = 1
    TabOrder = 0
    OnKeyPress = btnCloseKeyPress
  end
end
