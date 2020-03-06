/*
	VeroRoute - Qt based Veroboard/Perfboard/PCB layout & routing application.

	Copyright (C) 2017  Alex Lawrow    ( dralx@users.sourceforge.net )

	This program is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#pragma once

#include <QApplication>
#include <QMainWindow>
#include <QDockWidget>
#include <QListWidgetItem>
#include <QtGui>
#include <QWidget>
#include <QFileDialog>
#include <QColorDialog>
#include <QMessageBox>
#include <QLabel>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include "HistoryManager.h"
#include "GWriter.h"
#include "myscrollarea.h"

namespace Ui { class MainWindow; }

class ControlDialog;
class TemplatesDialog;
class RenderingDialog;
class WireDialog;
class HotkeysDialog;
class InfoDialog;
class CompDialog;
class TextDialog;
class BomDialog;
class PinDialog;

#define MAX_RECENT_FILES 10

class MainWindow : public QMainWindow
{
	friend class ControlDialog;
	friend class TemplatesDialog;
	friend class RenderingDialog;
	friend class WireDialog;
	friend class HotkeysDialog;
	friend class InfoDialog;
	friend class CompDialog;
	friend class TextDialog;
	friend class BomDialog;
	friend class PinDialog;

	Q_OBJECT

public:
	explicit MainWindow(const QString& localDataPathStr, const QString& tutorialsPathStr, QWidget* parent = 0);
	~MainWindow();

	void ResetView(bool bTutorial = false);
	void CheckFolders();
	// Helper for mouse
	void GetPixMapXY(const QPoint& currentPoint, int& pixmapX, int& pixmapY) const;
	void GetRowCol(const QPoint& currentPoint, int& row, int& col, double& deltaRow, double& deltaCol) const;
	void GetRowCol(const QPoint& currentPoint, const int rows, const int cols, int& row, int& col, double& deltaRow, double& deltaCol) const;

	// Helpers for rendering
	void GetFirstRowCol(int& iRow, int& iCol) const;
	void GetXY(const GuiControl& guiCtrl, double row, double col, int& X, int& Y) const;
	void GetLRTB(const GuiControl& guiCtrl, double percent, double row, double col, int& L, int& R, int& T, int& B) const;
	void GetLRTB(const GuiControl& guiCtrl, const Component& comp, int& L, int& R, int& T, int& B) const;
	void GetLRTB(const GuiControl& guiCtrl, const Rect& rect, int& L, int& R, int& T, int& B) const;
	void GetXY(const GuiControl& guiCtrl, const Component& comp, int& X, int& Y) const;

	const bool&	GetCtrlKeyDown() const	{ return m_bCtrlKeyDown;	}
	const bool&	GetShiftKeyDown() const	{ return m_bShiftKeyDown;	}
	const bool&	GetPaintPins() const	{ return m_bPaintPins;		}
	const bool&	GetPaintBoard() const	{ return m_bPaintBoard;		}
	const bool&	GetPaintFlood() const	{ return m_bPaintFlood;		}
	const bool&	GetDefiningRect() const	{ return m_bDefiningRect;	}
	const bool&	GetResizingText() const	{ return m_bResizingText;	}
	void		SetCtrlKeyDown(bool b)	{ m_bCtrlKeyDown	= b; }
	void		SetShiftKeyDown(bool b)	{ m_bShiftKeyDown	= b; }
	void		SetPaintPins(bool b)	{ m_bPaintPins		= b; }
	void		SetPaintBoard(bool b)	{ m_bPaintBoard		= b; }
	void		SetPaintFlood(bool b)	{ m_bPaintFlood		= b; }
	void		SetDefiningRect(bool b)	{ m_bDefiningRect	= b; }
	void		SetResizingText(bool b)	{ m_bResizingText	= b; }
protected:
	void paintEvent(QPaintEvent* event);
	void wheelEvent(QWheelEvent* event);
	void mousePressEvent(QMouseEvent* event);
	void mouseDoubleClickEvent(QMouseEvent* event);
	void mouseMoveEvent(QMouseEvent* event);
	void mouseReleaseEvent(QMouseEvent* event);
	void keyPressEvent(QKeyEvent* event);
	void keyReleaseEvent(QKeyEvent* event);
	void commonKeyPressEvent(QKeyEvent* event);		// So child dialogs can relay Ctrl and Shift to the main window
	void commonKeyReleaseEvent(QKeyEvent* event);	// So child dialogs can relay Ctrl and Shift to the main window
	void specialKeyPressEvent(QKeyEvent* event);	// So child dialogs can do Ctrl+Q etc
	void dragEnterEvent(QDragEnterEvent *e);
	void dropEvent(QDropEvent *e);
public slots:
	void Startup();
	// File menu items
	void New();
	void Open();
	void OpenRecent();
	void Merge();
	void Save();
	void SaveAs();
	void ImportTango();
	void ImportOrcad();
	void WritePDF();
	void WritePNG();
	void WriteGerber(const bool& bTwoLayers = false);
	void WriteGerber2() { WriteGerber(true); }
	void ClearRecentFiles();
	void Quit();
	// View menu items
	void ZoomIn();
	void ZoomOut();
	void Crop();
	void ToggleGrid();
	void ToggleText();
	void ToggleFlipH();
	void ToggleFlipV();
	void TogglePinLabels();
	// Toolbar items
	void VeroV();
	void VeroH();
	void Fat();
	void Thin();
	void Curved();
	void ToggleDiagsMin();
	void ToggleDiagsMax();
	void ToggleFill();
	void ToggleSelectArea();
	// Edit menu items
	void Undo();
	void Redo();
	void Copy();
	void Group();
	void Ungroup();
	void SelectAll();
	void SelectAllInRects();
	void Delete();
	// Add menu items
	void AddMarker()			{ AddPart(COMP::MARK); }
	void AddPad()				{ AddPart(COMP::PAD); }
	void AddWire()				{ AddPart(COMP::WIRE); }
	void AddResistor()			{ AddPart(COMP::RESISTOR); }
	void AddInductor()			{ AddPart(COMP::INDUCTOR); }
	void AddCrystal()			{ AddPart(COMP::CRYSTAL); }
	void AddDiode()				{ AddPart(COMP::DIODE); }
	void AddLED()				{ AddPart(COMP::LED); }
	void AddCapCeramic()		{ AddPart(COMP::CAP_CERAMIC); }
	void AddCapFilm()			{ AddPart(COMP::CAP_FILM); }
	void AddCapFilmWide()		{ AddPart(COMP::CAP_FILM_WIDE); }
	void AddCapElectro200NP()	{ AddPart(COMP::CAP_ELECTRO_200_NP); }
	void AddCapElectro250NP()	{ AddPart(COMP::CAP_ELECTRO_250_NP); }
	void AddCapElectro300NP()	{ AddPart(COMP::CAP_ELECTRO_300_NP); }
	void AddCapElectro400NP()	{ AddPart(COMP::CAP_ELECTRO_400_NP); }
	void AddCapElectro500NP()	{ AddPart(COMP::CAP_ELECTRO_500_NP); }
	void AddCapElectro600NP()	{ AddPart(COMP::CAP_ELECTRO_600_NP); }
	void AddCapElectro200()		{ AddPart(COMP::CAP_ELECTRO_200); }
	void AddCapElectro250()		{ AddPart(COMP::CAP_ELECTRO_250); }
	void AddCapElectro300()		{ AddPart(COMP::CAP_ELECTRO_300); }
	void AddCapElectro400()		{ AddPart(COMP::CAP_ELECTRO_400); }
	void AddCapElectro500()		{ AddPart(COMP::CAP_ELECTRO_500); }
	void AddCapElectro600()		{ AddPart(COMP::CAP_ELECTRO_600); }
	void AddTO92()				{ AddPart(COMP::TO92); }
	void AddTO18()				{ AddPart(COMP::TO18); }
	void AddTO39()				{ AddPart(COMP::TO39); }
	void AddTO220()				{ AddPart(COMP::TO220); }
	void AddTrimVert()			{ AddPart(COMP::TRIM_VERT); }
	void AddTrimVertOffset()	{ AddPart(COMP::TRIM_VERT_OFFSET); }
	void AddTrimVertOffsetWide(){ AddPart(COMP::TRIM_VERT_OFFSET_WIDE); }
	void AddTrimFlat()			{ AddPart(COMP::TRIM_FLAT); }
	void AddTrimFlatWide()		{ AddPart(COMP::TRIM_FLAT_WIDE); }
	void AddSIP()				{ AddPart(COMP::SIP); }
	void AddDIP()				{ AddPart(COMP::DIP); }
	void AddStrip100()			{ AddPart(COMP::STRIP_100); }
	void AddBlock100()			{ AddPart(COMP::BLOCK_100); }
	void AddBlock200()			{ AddPart(COMP::BLOCK_200); }
	void AddSwitchST()			{ AddPart(COMP::SWITCH_ST); }
	void AddSwitchDT()			{ AddPart(COMP::SWITCH_DT); }
	void AddSwitchST_DIP()		{ AddPart(COMP::SWITCH_ST_DIP); }
	void AddTextBox()
	{
		SetCurrentTextId(BAD_TEXTID);
		int iRow, iCol;
		GetFirstRowCol(iRow, iCol);
		m_board.AddTextBox(iRow, iCol);
		UpdateHistory("Add text");
		UpdateControls();
		RepaintSkipRouting();
		ShowTextDialog();
	}
	// Windows menu items
	void ShowControlDialog();
	void HideControlDialog();
	void ShowTemplatesDialog();
	void ShowRenderingDialog();
	void ShowWireDialog();
	void ShowHotkeysDialog();
	void ShowInfoDialog();
	void ShowCompDialog();
	void HideCompDialog();
	void ShowTextDialog();
	void ShowBomDialog();
	void ShowPinDialog();
	void HidePinDialog();
	void ShowAbout();
	void ShowSupport();
	// Layers menu items
	void AddLayer();
	void RemoveLayer();
	void SwitchLayer();
	void ToggleVias();
	// Help menu items
	void LoadFirstTutorial();
	void LoadPrevTutorial();
	void LoadNextTutorial();
	void LoadTutorial();		// Helper
	// Check version against Sourceforge
	void UpdateCheck();
	void HandleNetworkReply(QNetworkReply* reply);

	// Helpers for child dialogs
	void ShowDlg(QWidget* p);

	// View controls
	void TrackSliderChanged(int i);
	void SaturationSliderChanged(int i);
	void CompSliderChanged(int i);
	void FillSliderChanged(int i);
	void MarginChanged(int i);
	void SetShowGrid(bool b);
	void SetShowText(bool b);
	void SetFlipH(bool b);
	void SetFlipV(bool b);
	void SetShowPinLabels(bool b);
	void SetFill(bool b);
	// Part controls
	void SetCompName(const QString& str);
	void SetCompValue(const QString& str);
	void SetCompType(const QString& str);
	void CompRotateCCW()	{ CompRotate(false); }
	void CompRotateCW()		{ CompRotate(true); }
	void CompGrow()			{ CompStretch(true); }
	void CompShrink()		{ CompStretch(false); }
	void CompGrow2()		{ CompStretchWidth(true); }
	void CompShrink2()		{ CompStretchWidth(false); }
	void CompTextCentre()	{ CompTextMove(0,  0); }
	void CompTextL()		{ CompTextMove(0, -1); }
	void CompTextR()		{ CompTextMove(0,  1); }
	void CompTextT()		{ CompTextMove(-1, 0); }
	void CompTextB()		{ CompTextMove( 1, 0); }
	// Bad Nodes lists
	void SetNodeId(QListWidgetItem* item);
	void ListNodes(bool bRebuild = true);
	// Routing controls
	void EnableRouting(bool b);
	void EnableFastRouting(bool b);
	void Paste();
	void Tidy();
	void WipeTracks();
	// Track controls
	void SetTracksVeroV(bool b);
	void SetTracksVeroH(bool b);
	void SetTracksFat(bool b);
	void SetTracksThin(bool b);
	void SetTracksCurved(bool b);
	void SetDiagonalsOff(bool b);
	void SetDiagonalsMin(bool b);
	void SetDiagonalsMax(bool b);
	void SetTrackWidth(int i);
	void SetHoleWidth(int i);
	void SetPadWidth(int i);
	void SetGapWidth(int i);
	void SetMaskWidth(int i);
	void SetSilkWidth(int i);
	void SetEdgeWidth(int i);
	void SetViaPadWidth(int i);
	void SetViaHoleWidth(int i);

	// Rendering
	void SetTextSizeComp(int i);
	void SetTextSizePins(int i);
	void SetTargetRows(int i);
	void SetTargetCols(int i);
	void SetShowTarget(bool b);
	void SetAntialiasOff(bool b);
	void SetAntialiasOn(bool b);
	void SetAntialiasHigh(bool b);

	// Wire Options
	void SetWireShare(bool b);
	void SetWireCross(bool b);
	// For text box dialog
	void SizeChanged(int i);
	void ToggleBold();
	void ToggleItalic();
	void ToggleUnderline();
	void AlignL();
	void AlignC();
	void AlignR();
	void AlignJ();
	void SetText(const QString&);
	void ChooseTextColor();
	// For component editor dialog
	CompDefiner& GetCompDefiner() { return m_board.GetCompDefiner(); }
	// For template dialog
	TemplateManager& GetTemplateManager() { return m_templateMgr; }
	// Info dialog
	void SetInfoStr(const QString& str)	{ m_board.SetInfoStr(str.toStdString()); }
	void OpenVrt(const QString& fileName, bool bMerge = false);	// Helper for opening a vrt using Open(), Merge(), dropEvent(), or the command line
	// Component editor
	void DefinerSetValueStr(const QString& str);
	void DefinerSetPrefixStr(const QString& str);
	void DefinerSetTypeStr(const QString& str);
	void DefinerSetImportStr(const QString& str);
	void DefinerWidthChanged(int i);
	void DefinerHeightChanged(int i);
	void DefinerSetPinShapeType(const QString& str);
	void DefinerToggledPinLabels(bool b);
	void DefinerToggleShapeLine(bool b);
	void DefinerToggleShapeFill(bool b);
	void DefinerSetPinNumber(int i);
	void DefinerIncPinNumber(bool b);
	void DefinerSetShapeType(const QString& str);
	void DefinerSetCX(double d);
	void DefinerSetCY(double d);
	void DefinerSetDX(double d);
	void DefinerSetDY(double d);
	void DefinerSetA1(double d);
	void DefinerSetA2(double d);
	void DefinerSetA3(double d);
	void DefinerBuild();
	void DefinerToggleEditor();
	void DefinerAddLine();
	void DefinerAddRect();
	void DefinerAddRoundedRect();
	void DefinerAddEllipse();
	void DefinerAddArc();
	void DefinerAddChord();
	void DefinerChooseColor();
	void DefinerRaise();
	void DefinerLower();
private:
	void DestroyPixmapCache();
	void CreatePixmapCache(const GuiControl& guiCtrl, ColorManager& colorManager);
	void PaintViaPad(const GuiControl& guiCtrl, QPainter& painter,  const QColor& color, const QPointF& pC, const bool& bGap = false);	// Helper
	void PaintPad(const GuiControl& guiCtrl, QPainter& painter,  const QColor& color, const QPointF& pC, const bool& bGap = false);		// Helper
	void PaintDiag(const GuiControl& guiCtrl, QPainter& painter, const QColor& color, const QPointF& pCorner, const int& H, bool bLT);
	void PaintBlob(const GuiControl& guiCtrl, QPainter& painter, const QColor& color, const QPointF& pC, const int& iPerimeterCode, const bool bGap = false);	// Helper
	void PaintBoard();
	void PaintCompDefiner();
	void HandleRouting();
	void RepaintWithRouting(bool bNow = false);
	void RepaintSkipRouting(bool bNow = false);
	void ShowCurrentRectSize();	// Show current rect size in status bar
	int	 GetCurrentLayer() const			{ return m_board.GetCurrentLayer(); }
	int	 GetCurrentNodeId() const			{ return m_board.GetCurrentNodeId(); }
	int  GetCurrentCompId() const			{ return m_board.GetCurrentCompId(); }
	int  GetCurrentTextId() const			{ return m_board.GetCurrentTextId(); }
	int  GetCurrentPinId() const			{ return m_board.GetCurrentPinId(); }
	int  GetCurrentShapeId() const			{ return m_board.GetCurrentShapeId(); }
	void SetCurrentNodeId(const int& i)		{ m_board.SetCurrentNodeId(i);	ListNodes(false); }
	void SetCurrentCompId(const int& i)		{ m_board.SetCurrentCompId(i);	UpdateControls(); }
	void SetCurrentTextId(const int& i)		{ m_board.SetCurrentTextId(i);	UpdateControls(); }
	void SetCurrentPinId(const int& i)		{ m_board.SetCurrentPinId(i);	UpdateCompDialog(); }
	void SetCurrentShapeId(const int& i)	{ m_board.SetCurrentShapeId(i);	UpdateCompDialog(); }
	TextRect& GetCurrentTextRect()			{ return m_board.GetTextMgr().GetTextRectById( GetCurrentTextId() ); }
	// Helpers for slots
	void ZoomHelper(int delta);
	void AddPart(COMP eType)
	{
		if ( m_board.GetCompEdit() ) return;	// Do nothing in component editor mode

		int iRow, iCol;
		GetFirstRowCol(iRow, iCol);
		const int compId = m_board.CreateComponent(iRow, iCol, eType);
		if ( compId == BAD_COMPID ) return;	// Reached component limit

		GroupManager& groupMgr = m_board.GetGroupMgr();
		groupMgr.ResetUserGroup( compId );	// Reset the user group with the current comp (and its siblings)
		SetCurrentCompId(compId);

		UpdateHistory("Add part"); UpdateControls(); UpdateBOM(); RepaintSkipRouting();
	}
	void AddFromTemplate(const Component& compTemp)
	{
		if ( m_board.GetCompEdit() ) return;	// Do nothing in component editor mode

		int iRow, iCol;
		GetFirstRowCol(iRow, iCol);
		const int compId = m_board.CreateComponent(iRow, iCol, compTemp.GetType(), &compTemp);
		if ( compId == BAD_COMPID ) return;	// Reached component limit

		GroupManager& groupMgr = m_board.GetGroupMgr();
		groupMgr.ResetUserGroup( compId );	// Reset the user group with the current comp (and its siblings)
		SetCurrentCompId(compId);

		UpdateHistory("Add part"); UpdateControls(); UpdateBOM(); RepaintSkipRouting();
	}
	void CompRotate(const bool& bCW);
	void CompStretch(const bool& bGrow);
	void CompStretchWidth(const bool& bGrow);
	void CompTextMove(const int& deltaRow, const int& deltaCol);

	// GUI update
	void UpdateRecentFiles(const QString* pFileName, bool bAdd);
	void UpdateWindowTitle();
	void UpdateControls();
	void UpdateCompDialog();
	void EnableCompDialogControls();
	void UpdateBOM();
	void UpdateTemplatesDialog();
	void UpdateTextDialog(bool bFull = false);

	// Helpers
	void SetQuality(QPainter& p);
	bool CanZoomIn() const;
	bool CanZoomOut() const;
	bool GetIsModified() const;
	bool GetMatchesVrtFile(const std::string& fileName) const;
	void ResetHistory(const std::string& str);
	void UpdateHistory(const std::string& str, const int compId = BAD_COMPID);
	void UpdateUndoRedoControls();

	// Helper to auto-append suffix when writing a file
	QString GetSaveFileName(const QString& caption, const QString& nameFilter, const QString& defaultSuffix);

	QColor	GetBackgroundColor() const;	// For screen only.  PDF is always white.

	// Pens
	QPen	m_yellowPen;
	QPen	m_backgroundPen;
	QPen	m_darkGreyPen;
	QPen	m_blackPen;
	QPen	m_whitePen;
	QPen	m_redPen;
	QPen	m_lightBluePen;
	QPen	m_varPen;
	QPen	m_dotPen;
	QPen	m_dashPen;
	QBrush	m_backgroundBrush;
	QBrush	m_darkBrush;
	QBrush	m_varBrush;
private:
	Ui::MainWindow*			ui;
	QAction*				m_recentFileAction[MAX_RECENT_FILES];
	QAction*				m_separator;	// At the end of the recent files list

	QNetworkAccessManager	m_networkMgr;	// For checking version against Sourceforge

	MyScrollArea*			m_scrollArea;	// The mainwindow contains a scrollable area ...
	QLabel*					m_label;		// ... for a QLabel widget that ...
	QPixmap					m_mainPixmap;	// ... contains an image of the whole board

	QLabel*					m_labelStatus;	// For permanent status bar text

	QDockWidget*			m_dockControlDlg;
	QDockWidget*			m_dockCompDlg;
	ControlDialog*			m_controlDlg;
	RenderingDialog*		m_renderingDlg;
	WireDialog*				m_wireDlg;
	HotkeysDialog*			m_hotkeysDlg;
	InfoDialog*				m_infoDlg;
	CompDialog*				m_compDlg;
	TextDialog*				m_textDlg;
	BomDialog*				m_bomDlg;
	TemplatesDialog*		m_templatesDlg;
	PinDialog*				m_pinDlg;

	Board					m_board;			// *** The main object ***
	HistoryManager			m_historyMgr;		// Class to manage undo/redo
	TemplateManager			m_templateMgr;		// For component templates
	GWriter					m_gWriter;			// Writes Gerber files
	QString					m_fileName;			// The loaded/saved .vrt file
	QString					m_pdfFileName;		// The saved PDF file
	QString					m_gerberFileName;	// The saved Gerber file
	std::string				m_localDataPathStr;	// The path to the "history" and "templates" folders
	std::string				m_tutorialsPathStr;	// The path to the "tutorials" folder and "veroroute.png"

	// Cached pixmaps containing pre-colored pads and blobs.
	QPixmap**	m_ppPixmapPad;		// A pad in the host element
	QPixmap**	m_ppPixmapVia;		// A via in the host element
	QPixmap**	m_ppPixmapDiag;		// For filling small diagonal gaps not covered by blob pixmaps
	QPixmap**	m_ppPixmapBlob;		// A composite shape with all the host element connections
	QPoint		m_mousePos;
	bool		m_bRepaint;			// Flag to make paintEvent() do something useful
	bool		m_bMouseClick;		// Flag of click beginning
	bool		m_bLeftClick;
	bool		m_bRightClick;
	bool		m_bCtrlKeyDown;
	bool		m_bShiftKeyDown;
	bool		m_bPaintPins;		// true ==> allow paint the component pins (and the board)
	bool		m_bPaintBoard;		// true ==> allow paint the board only (not the component pins)
	bool		m_bPaintFlood;		// true ==> allow flood-fill all connected tracks & pins
	bool		m_bDefiningRect;	// true ==> user is defining a rectangle
	bool		m_bResizingText;	// true ==> user is resizing a text rectangle
	bool		m_bWritePDF;		// true ==> draw to PDF file instead of screen
	bool		m_bWriteGerber;		// true ==> draw to Gerber file instead of screen
	bool		m_bTwoLayers;		// true ==> 2-layer Gerber output instead of 1-layer
	bool		m_bHistoryDir;		// true ==> have "history" folder
	bool		m_bTemplatesDir;	// true ==> have "templates" folder
	int			m_XGRIDOFFSET;		// So we can centre when writing to PDF
	int			m_YGRIDOFFSET;		// So we can centre when writing to PDF
	int			m_gridRow;			// Board row corresponding to mouse position
	int			m_gridCol;			// Board col correspondong to mouse position
	int			m_iTutorialNumber;	// Tutorial file number 0,1,2,... (or -1 if not in tutorial mode)
};
