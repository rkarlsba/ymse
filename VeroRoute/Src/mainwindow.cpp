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

#include "Version.h"
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "controldialog.h"
#include "renderingdialog.h"
#include "wiredialog.h"
#include "hotkeysdialog.h"
#include "infodialog.h"
#include "compdialog.h"
#include "textdialog.h"
#include "bomdialog.h"
#include "templatesdialog.h"
#include "pindialog.h"

MainWindow::MainWindow(const QString& localDataPathStr, const QString& tutorialsPathStr, QWidget* parent)
: QMainWindow(parent)
, ui(new Ui::MainWindow)
, m_localDataPathStr(localDataPathStr.toStdString())
, m_tutorialsPathStr(tutorialsPathStr.toStdString())
, m_ppPixmapPad(nullptr)
, m_ppPixmapVia(nullptr)
, m_ppPixmapDiag(nullptr)
, m_ppPixmapBlob(nullptr)
, m_bRepaint(false)
, m_iTutorialNumber(-1)	// -1 ==> we're not in a tutorial
{
	m_historyMgr.SetPathStr(m_localDataPathStr);
	m_templateMgr.SetPathStr(m_localDataPathStr);

	InitMapsCompTypeToStr();			// Put all comp types into a list ...
	GetTemplateManager().AddDefaults();	// .. and create default templates

	ui->setupUi(this);

	// Create menu actions for recent files
	for (int i = 0; i < MAX_RECENT_FILES; ++i)
	{
		m_recentFileAction[i] = new QAction(this);
		m_recentFileAction[i]->setVisible(false);
		connect(m_recentFileAction[i], SIGNAL(triggered()), this, SLOT(OpenRecent()));
		ui->menuFile->insertAction(ui->actionQuit, m_recentFileAction[i]);	// Insert before the Quit action
	}
	m_separator = ui->menuFile->insertSeparator(ui->actionQuit);	// Insert separator before the Quit action
	UpdateRecentFiles(nullptr, false);	// nullptr ==> read list without updating it

	const std::string iconStr = m_tutorialsPathStr + "/veroroute.png";
	setWindowIcon(QIcon( iconStr.c_str() ));

	// Create the scrollable area and make it occuoy the main window area
	m_label			= new QLabel(this);
	m_scrollArea	= new MyScrollArea(this);
	m_label->setBackgroundRole(QPalette::Base);
	m_label->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
	m_label->setScaledContents(true);
	m_scrollArea->setBackgroundRole(QPalette::Dark);
	m_scrollArea->setWidget(m_label);
	setCentralWidget(m_scrollArea);

	// Put control dialog in right dock area
	m_dockControlDlg = new QDockWidget(tr("  Control"), this);
	m_dockControlDlg->setAllowedAreas(Qt::RightDockWidgetArea);
	m_dockControlDlg->setStyleSheet("QDockWidget { font: bold }");
	m_controlDlg	= new ControlDialog(m_dockControlDlg);
	m_dockControlDlg->setWidget(m_controlDlg);
	addDockWidget(Qt::RightDockWidgetArea, m_dockControlDlg);
	m_controlDlg->SetMainWindow(this);

	// Put component editor dialog in right dock area
	m_dockCompDlg = new QDockWidget(tr("  Component Definition"), this);
	m_dockCompDlg->setAllowedAreas(Qt::RightDockWidgetArea);
	m_dockCompDlg->setStyleSheet("QDockWidget { font: bold }");
	m_compDlg	= new CompDialog(m_dockCompDlg);
	m_dockCompDlg->setWidget(m_compDlg);
	addDockWidget(Qt::RightDockWidgetArea, m_dockCompDlg);
	m_compDlg->SetMainWindow(this);

	m_renderingDlg	= new RenderingDialog(this);
	m_wireDlg		= new WireDialog(this);
	m_hotkeysDlg	= new HotkeysDialog(this);
	m_infoDlg		= new InfoDialog(this);
	m_textDlg		= new TextDialog(this);
	m_bomDlg		= new BomDialog(this);
	m_templatesDlg	= new TemplatesDialog(this);
	m_pinDlg		= new PinDialog(this);

	m_templatesDlg->move(940,50);
	move(320,50);
	m_infoDlg->move(940,50);

	// Do multipart status bar
	m_labelStatus		= new QLabel("Left", this);
	m_labelStatus->setFrameStyle(QFrame::NoFrame);
	ui->statusBar->addPermanentWidget(m_labelStatus, 0);

	m_yellowPen			= QPen(Qt::yellow, 0,					Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);
	m_backgroundPen		= QPen(Qt::white, 0,					Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);
	m_darkGreyPen		= QPen(QColor(96,96,96,255), 0,			Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);
	m_blackPen			= QPen(Qt::black, 0,					Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);
	m_whitePen			= QPen(Qt::white, 0,					Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);
	m_redPen			= QPen(Qt::red, 0,						Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);
	m_lightBluePen		= QPen(QColor(128, 128, 255, 255), 0,	Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);
	m_varPen			= QPen(Qt::black, 0,					Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);
	m_dotPen			= QPen(QColor(96,96,96,255), 0,			Qt::DotLine,   Qt::RoundCap, Qt::RoundJoin);
	m_dashPen			= QPen(QColor(96,96,96,255), 0,			Qt::DashLine,  Qt::RoundCap, Qt::RoundJoin);
	m_backgroundBrush	= QBrush(Qt::white,						Qt::SolidPattern);
	m_darkBrush			= QBrush(QColor(0, 0, 0, 150),			Qt::SolidPattern);	// using alpha
	m_varBrush			= QBrush(QColor(255,255,255,0),			Qt::SolidPattern);

	QObject::connect(ui->actionNew,						SIGNAL(triggered()), this, SLOT(New()));
	QObject::connect(ui->actionOpen,					SIGNAL(triggered()), this, SLOT(Open()));
	QObject::connect(ui->actionSave,					SIGNAL(triggered()), this, SLOT(Save()));
	QObject::connect(ui->actionSave_As,					SIGNAL(triggered()), this, SLOT(SaveAs()));
	QObject::connect(ui->actionMerge,					SIGNAL(triggered()), this, SLOT(Merge()));
	QObject::connect(ui->actionImportTango,				SIGNAL(triggered()), this, SLOT(ImportTango()));
	QObject::connect(ui->actionImportOrcad,				SIGNAL(triggered()), this, SLOT(ImportOrcad()));
	QObject::connect(ui->actionWrite_PDF,				SIGNAL(triggered()), this, SLOT(WritePDF()));
	QObject::connect(ui->actionWrite_PNG,				SIGNAL(triggered()), this, SLOT(WritePNG()));
	QObject::connect(ui->actionWrite_Gerber,			SIGNAL(triggered()), this, SLOT(WriteGerber()));
	QObject::connect(ui->actionWrite_Gerber2,			SIGNAL(triggered()), this, SLOT(WriteGerber2()));
	QObject::connect(ui->actionClearRecent,				SIGNAL(triggered()), this, SLOT(ClearRecentFiles()));
	QObject::connect(ui->actionQuit,					SIGNAL(triggered()), this, SLOT(Quit()));
	QObject::connect(ui->actionZoom_In,					SIGNAL(triggered()), this, SLOT(ZoomIn()));
	QObject::connect(ui->actionZoom_Out,				SIGNAL(triggered()), this, SLOT(ZoomOut()));
	QObject::connect(ui->actionCrop,					SIGNAL(triggered()), this, SLOT(Crop()));
	QObject::connect(ui->actionToggleGrid,				SIGNAL(triggered()), this, SLOT(ToggleGrid()));
	QObject::connect(ui->actionToggleText,				SIGNAL(triggered()), this, SLOT(ToggleText()));
	QObject::connect(ui->actionToggleFlipH,				SIGNAL(triggered()), this, SLOT(ToggleFlipH()));
	QObject::connect(ui->actionToggleFlipV,				SIGNAL(triggered()), this, SLOT(ToggleFlipV()));
	QObject::connect(ui->actionTogglePinLabels,			SIGNAL(triggered()), this, SLOT(TogglePinLabels()));
	QObject::connect(ui->actionVeroV,					SIGNAL(triggered()), this, SLOT(VeroV()));
	QObject::connect(ui->actionVeroH,					SIGNAL(triggered()), this, SLOT(VeroH()));
	QObject::connect(ui->actionFat,						SIGNAL(triggered()), this, SLOT(Fat()));
	QObject::connect(ui->actionThin,					SIGNAL(triggered()), this, SLOT(Thin()));
	QObject::connect(ui->actionCurved,					SIGNAL(triggered()), this, SLOT(Curved()));
	QObject::connect(ui->actionDiagsMin,				SIGNAL(triggered()), this, SLOT(ToggleDiagsMin()));
	QObject::connect(ui->actionDiagsMax,				SIGNAL(triggered()), this, SLOT(ToggleDiagsMax()));
	QObject::connect(ui->actionFill,					SIGNAL(triggered()), this, SLOT(ToggleFill()));
	QObject::connect(ui->actionSelectArea,				SIGNAL(triggered()), this, SLOT(ToggleSelectArea()));
	QObject::connect(ui->actionUndo,					SIGNAL(triggered()), this, SLOT(Undo()));
	QObject::connect(ui->actionRedo,					SIGNAL(triggered()), this, SLOT(Redo()));
	QObject::connect(ui->actionCopy,					SIGNAL(triggered()), this, SLOT(Copy()));
	QObject::connect(ui->actionGroup,					SIGNAL(triggered()), this, SLOT(Group()));
	QObject::connect(ui->actionUngroup,					SIGNAL(triggered()), this, SLOT(Ungroup()));
	QObject::connect(ui->actionSelectAll,				SIGNAL(triggered()), this, SLOT(SelectAll()));
	QObject::connect(ui->actionDelete,					SIGNAL(triggered()), this, SLOT(Delete()));
	QObject::connect(ui->actionMarker,					SIGNAL(triggered()), this, SLOT(AddMarker()));
	QObject::connect(ui->actionPad,						SIGNAL(triggered()), this, SLOT(AddPad()));
	QObject::connect(ui->actionStrip100,				SIGNAL(triggered()), this, SLOT(AddStrip100()));
	QObject::connect(ui->actionBlock100,				SIGNAL(triggered()), this, SLOT(AddBlock100()));
	QObject::connect(ui->actionBlock200,				SIGNAL(triggered()), this, SLOT(AddBlock200()));
	QObject::connect(ui->actionWire,					SIGNAL(triggered()), this, SLOT(AddWire()));
	QObject::connect(ui->actionResistor,				SIGNAL(triggered()), this, SLOT(AddResistor()));
	QObject::connect(ui->actionInductor,				SIGNAL(triggered()), this, SLOT(AddInductor()));
	QObject::connect(ui->actionCrystal,					SIGNAL(triggered()), this, SLOT(AddCrystal()));
	QObject::connect(ui->actionDiode,					SIGNAL(triggered()), this, SLOT(AddDiode()));
	QObject::connect(ui->actionLED,						SIGNAL(triggered()), this, SLOT(AddLED()));
	QObject::connect(ui->actionCapCeramic,				SIGNAL(triggered()), this, SLOT(AddCapCeramic()));
	QObject::connect(ui->actionCapFilm,					SIGNAL(triggered()), this, SLOT(AddCapFilm()));
	QObject::connect(ui->actionCapFilmWide,				SIGNAL(triggered()), this, SLOT(AddCapFilmWide()));
	QObject::connect(ui->actionCapElectro200NP,			SIGNAL(triggered()), this, SLOT(AddCapElectro200NP()));
	QObject::connect(ui->actionCapElectro250NP,			SIGNAL(triggered()), this, SLOT(AddCapElectro250NP()));
	QObject::connect(ui->actionCapElectro300NP,			SIGNAL(triggered()), this, SLOT(AddCapElectro300NP()));
	QObject::connect(ui->actionCapElectro400NP,			SIGNAL(triggered()), this, SLOT(AddCapElectro400NP()));
	QObject::connect(ui->actionCapElectro500NP,			SIGNAL(triggered()), this, SLOT(AddCapElectro500NP()));
	QObject::connect(ui->actionCapElectro600NP,			SIGNAL(triggered()), this, SLOT(AddCapElectro600NP()));
	QObject::connect(ui->actionCapElectro200,			SIGNAL(triggered()), this, SLOT(AddCapElectro200()));
	QObject::connect(ui->actionCapElectro250,			SIGNAL(triggered()), this, SLOT(AddCapElectro250()));
	QObject::connect(ui->actionCapElectro300,			SIGNAL(triggered()), this, SLOT(AddCapElectro300()));
	QObject::connect(ui->actionCapElectro400,			SIGNAL(triggered()), this, SLOT(AddCapElectro400()));
	QObject::connect(ui->actionCapElectro500,			SIGNAL(triggered()), this, SLOT(AddCapElectro500()));
	QObject::connect(ui->actionCapElectro600,			SIGNAL(triggered()), this, SLOT(AddCapElectro600()));
	QObject::connect(ui->actionTO92,					SIGNAL(triggered()), this, SLOT(AddTO92()));
	QObject::connect(ui->actionTO18,					SIGNAL(triggered()), this, SLOT(AddTO18()));
	QObject::connect(ui->actionTO39,					SIGNAL(triggered()), this, SLOT(AddTO39()));
	QObject::connect(ui->actionTO220,					SIGNAL(triggered()), this, SLOT(AddTO220()));
	QObject::connect(ui->actionTrimVertical,			SIGNAL(triggered()), this, SLOT(AddTrimVert()));
	QObject::connect(ui->actionTrimFlat,				SIGNAL(triggered()), this, SLOT(AddTrimFlat()));
	QObject::connect(ui->actionTrimFlatWide,			SIGNAL(triggered()), this, SLOT(AddTrimFlatWide()));
	QObject::connect(ui->actionTrimVerticalOffset,		SIGNAL(triggered()), this, SLOT(AddTrimVertOffset()));
	QObject::connect(ui->actionTrimVerticalOffsetWide,	SIGNAL(triggered()), this, SLOT(AddTrimVertOffsetWide()));
	QObject::connect(ui->actionSIP,						SIGNAL(triggered()), this, SLOT(AddSIP()));
	QObject::connect(ui->actionDIP,						SIGNAL(triggered()), this, SLOT(AddDIP()));
	QObject::connect(ui->actionSwitchST,				SIGNAL(triggered()), this, SLOT(AddSwitchST()));
	QObject::connect(ui->actionSwitchDT,				SIGNAL(triggered()), this, SLOT(AddSwitchDT()));
	QObject::connect(ui->actionSwitchST_DIP,			SIGNAL(triggered()), this, SLOT(AddSwitchST_DIP()));
	QObject::connect(ui->actionTextBox,					SIGNAL(triggered()), this, SLOT(AddTextBox()));
	QObject::connect(ui->actionRenderingDlg,			SIGNAL(triggered()), this, SLOT(ShowRenderingDialog()));
	QObject::connect(ui->actionWireDlg,					SIGNAL(triggered()), this, SLOT(ShowWireDialog()));
	QObject::connect(ui->actionControlDlg,				SIGNAL(triggered()), this, SLOT(ShowControlDialog()));
	QObject::connect(ui->actionTemplatesDlg,			SIGNAL(triggered()), this, SLOT(ShowTemplatesDialog()));
	QObject::connect(ui->actionHotkeysDlg,				SIGNAL(triggered()), this, SLOT(ShowHotkeysDialog()));
	QObject::connect(ui->actionInfoDlg,					SIGNAL(triggered()), this, SLOT(ShowInfoDialog()));
	QObject::connect(ui->actionBomDlg,					SIGNAL(triggered()), this, SLOT(ShowBomDialog()));
	QObject::connect(ui->actionTemplatesDlg,			SIGNAL(triggered()), this, SLOT(ShowTemplatesDialog()));
	QObject::connect(ui->actionPinDlg,					SIGNAL(triggered()), this, SLOT(ShowPinDialog()));
	QObject::connect(ui->actionCompDlg,					SIGNAL(triggered()), this, SLOT(ShowCompDialog()));
	QObject::connect(ui->actionAddLayer,				SIGNAL(triggered()), this, SLOT(AddLayer()));
	QObject::connect(ui->actionRemoveLayer,				SIGNAL(triggered()), this, SLOT(RemoveLayer()));
	QObject::connect(ui->actionSwitchLayer,				SIGNAL(triggered()), this, SLOT(SwitchLayer()));
	QObject::connect(ui->actionToggleVias,				SIGNAL(triggered()), this, SLOT(ToggleVias()));
	QObject::connect(ui->actionAbout,					SIGNAL(triggered()), this, SLOT(ShowAbout()));
	QObject::connect(ui->actionTutorial,				SIGNAL(triggered()), this, SLOT(LoadFirstTutorial()));
	QObject::connect(ui->actionSupport,					SIGNAL(triggered()), this, SLOT(ShowSupport()));
	QObject::connect(ui->actionUpdateCheck,				SIGNAL(triggered()), this, SLOT(UpdateCheck()));
	QObject::connect(&m_networkMgr,	SIGNAL(finished(QNetworkReply*)), this, SLOT(HandleNetworkReply(QNetworkReply*)));
	QObject::connect(ui->actionEditor,					SIGNAL(triggered()), this, SLOT(DefinerToggleEditor()));
	QObject::connect(ui->actionAddLine,					SIGNAL(triggered()), this, SLOT(DefinerAddLine()));
	QObject::connect(ui->actionAddRect,					SIGNAL(triggered()), this, SLOT(DefinerAddRect()));
	QObject::connect(ui->actionAddRoundedRect,			SIGNAL(triggered()), this, SLOT(DefinerAddRoundedRect()));
	QObject::connect(ui->actionAddEllipse,				SIGNAL(triggered()), this, SLOT(DefinerAddEllipse()));
	QObject::connect(ui->actionAddArc,					SIGNAL(triggered()), this, SLOT(DefinerAddArc()));
	QObject::connect(ui->actionAddChord,				SIGNAL(triggered()), this, SLOT(DefinerAddChord()));
	m_fileName.clear();
	CheckFolders();
	ResetHistory("Empty");

	setAcceptDrops(true);

	QTimer::singleShot(0, this, SLOT(Startup()));
}

MainWindow::~MainWindow()
{
	DestroyPixmapCache();
	delete m_compDlg;
	delete m_dockCompDlg;
	delete m_controlDlg;
	delete m_dockControlDlg;
	delete m_templatesDlg;
	delete m_renderingDlg;
	delete m_wireDlg;
	delete m_infoDlg;
	delete m_bomDlg;
	delete m_pinDlg;
	delete m_labelStatus;
	delete m_label;
	delete m_scrollArea;
	delete ui;
}

void MainWindow::Startup()
{
	ResetView();
}

void MainWindow::CheckFolders()
{
	// Check for "history" folder
	m_bHistoryDir = false;
	const std::string historyFolder = m_localDataPathStr + "/history";
	std::string testStr = historyFolder + "/TEST\0";

	DataStream outStream(DataStream::WRITE);
	if ( outStream.Open( testStr.c_str() ) )
	{
		outStream.Close();
		remove( testStr.c_str() );
		m_bHistoryDir = true;
	}
	else
	{
		std::string messageStr;
		if ( QDir( historyFolder.c_str() ).exists() )
			messageStr += "Cannot write to the folder " + historyFolder;
		else
			messageStr += "Cannot find the folder " + historyFolder;

		messageStr += "\n\nUndo/Redo and other functionality will not work.";

		QMessageBox::warning(this, tr("History folder is not available "), tr(messageStr.c_str()));
		ui->actionUndo->setDisabled(true);
		ui->actionRedo->setDisabled(true);
	}

	// Check for "tutorials" folder
	const std::string tutorialsFolder = m_tutorialsPathStr + "/tutorials";
	testStr = tutorialsFolder + "/tutorial_0.vrt\0";

	DataStream inStream(DataStream::READ);
	if ( !inStream.Open( testStr.c_str() ) )
	{
		std::string messageStr;
		if ( QDir( tutorialsFolder.c_str() ).exists() )
			messageStr += "Cannot find tutorial_0.vrt in the folder " + tutorialsFolder;
		else
			messageStr += "Cannot find the folder " + tutorialsFolder;

		messageStr += "\n\nTutorials will be disabled.";

		QMessageBox::information(this, tr("Tutorials folder is not available"), tr(messageStr.c_str()));
		ui->actionTutorial->setDisabled(true);
	}

	// Check for "templates" folder
	m_bTemplatesDir = false;
	const std::string templatesFolder = m_localDataPathStr + "/templates";
	testStr = templatesFolder + "/TEST\0";

	if ( outStream.Open( testStr.c_str() ) )
	{
		outStream.Close();
		remove( testStr.c_str() );
		m_bTemplatesDir = true;
	}
	else
	{
		std::string messageStr;
		if ( QDir( templatesFolder.c_str() ).exists() )
			messageStr += "Cannot write to the folder " + templatesFolder;
		else
			messageStr += "Cannot find the folder " + templatesFolder;

		messageStr += "\n\nAny part templates you create will not be saved.";

		QMessageBox::warning(this, tr("Templates folder is not available "), tr(messageStr.c_str()));
	}
}

void MainWindow::ResetView(bool bTutorial)
{
	m_mousePos = QPoint(0,0);
	m_bMouseClick	= m_bLeftClick	= m_bRightClick = m_bCtrlKeyDown  = m_bShiftKeyDown	=false;
	m_bPaintPins	= m_bPaintBoard	= m_bPaintFlood = m_bDefiningRect = m_bResizingText	= m_bWritePDF = m_bWriteGerber = m_bTwoLayers = false;
	m_XGRIDOFFSET	= m_YGRIDOFFSET	= 0;

	// Try to set m_gridRow, m_gridCol to match the current NodeId in the board
	m_gridRow = m_gridCol = 0;
	bool bOK(false);	// true ==> found
	const int k = m_board.GetCurrentLayer();
	for (int j = 0, jMax = m_board.GetRows(); j < jMax && !bOK; j++)
	for (int i = 0, iMax = m_board.GetCols(); i < iMax && !bOK; i++)
	{
		const Element* pC = m_board.Get(k,j,i);
		bOK = ( pC->GetNodeId() == m_board.GetCurrentNodeId() );
		if ( bOK ) { m_gridRow = j;	m_gridCol = i; }
	}

	m_infoDlg->Update();
	m_infoDlg->SetReadOnly(bTutorial);
	m_infoDlg->ShowButtons(bTutorial);
	m_infoDlg->EnablePrev(bTutorial && m_iTutorialNumber > 0);	// Tutorials go to 0 to 21
	m_infoDlg->EnableNext(bTutorial && m_iTutorialNumber < 21);	// Tutorials go to 0 to 21
	if ( !bTutorial ) m_iTutorialNumber = -1;	// Cancel tutorial mode
	UpdateControls();
	UpdateBOM();
	UpdateTemplatesDialog();
	UpdateCompDialog();
	UpdateWindowTitle();

	if ( m_board.GetCompEdit() )
	{
		HidePinDialog();		// Hide pin dialog
		HideControlDialog();	// Hide control dialog
		ShowCompDialog();
	}
	else
	{
		HideCompDialog();		// Hide component definition dialog
		ShowControlDialog();
	}

	if ( bTutorial ) ShowInfoDialog();			// Always show Info dialog in Tutorial Mode
	ui->actionSave->setEnabled(!bTutorial);		// Disable "Save" in Tutorial Mode
	ui->actionSave_As->setEnabled(!bTutorial);	// Disable "Save As" in Tutorial Mode
	UpdateUndoRedoControls();
	activateWindow();	// Select mainwindow rather than child dialogs
	DestroyPixmapCache();

	RepaintWithRouting();
	ListNodes();		// Slow due lots of MH calcs
}

void MainWindow::HandleRouting()
{
	if ( m_board.GetTrackMode() == TRACKMODE::OFF ) return;

	if ( m_board.GetRoutingEnabled() )
	{
		grabMouse(Qt::WaitCursor);
		m_board.Route(true);	// true ==> Use minimal routing
		releaseMouse();
	}
	m_board.UpdateVias();
	if ( GetCurrentNodeId() != BAD_NODEID )
	{
		grabMouse(Qt::WaitCursor);

		const int k = m_board.GetCurrentLayer();
		Element* pC = m_board.Get(k, m_gridRow, m_gridCol);
		bool bOK = ( pC->GetNodeId() == GetCurrentNodeId() );
		// If current element has wrong NodeID, search the grid for the first element with the correct NodeID
		const int iSize = m_board.GetSize();
		for (int i = 0; i < iSize && !bOK; i++)
		{
			pC = m_board.GetAt(i);
			bOK = ( pC->GetNodeId() == GetCurrentNodeId() );
		}
		if ( bOK ) m_board.Manhatten(pC);	// Only calc MH over relevant elements
		releaseMouse();
	}
}

void MainWindow::RepaintWithRouting(bool bNow)
{
	UpdateWindowTitle();
	HandleRouting();
	m_bRepaint = true;
	if ( bNow ) repaint(); else update();
}

void MainWindow::RepaintSkipRouting(bool bNow)
{
	UpdateWindowTitle();
	m_bRepaint = true;
	if ( bNow ) repaint(); else update();
}

void MainWindow::ShowCurrentRectSize()
{
	auto& rect = m_board.GetRectMgr().GetCurrent();
	char buffer[256] = {'\0'};
	sprintf(buffer,"Current rectangle (%d x %d)    (%gmm x %gmm)", rect.GetCols(), rect.GetRows(), rect.GetCols()*2.54, rect.GetRows()*2.54);
	ui->statusBar->showMessage(QString(buffer), 1000);
}

void MainWindow::OpenVrt(const QString& fileName, bool bMerge)	// Helper for opening a vrt using Open(), Merge(), dropEvent(), or the command line
{
	bool bOK(false);

	DataStream inStream(DataStream::READ);
	const std::string fileNameStr = fileName.toStdString();
	if ( inStream.Open( fileNameStr.c_str() ) )
	{
		ui->statusBar->showMessage( bMerge ? tr("Merging...") : tr("Opening..."), 500 );

		Board tmp;	// Load using a temporary board in case there is a problem with the file
		tmp.Load(inStream);
		inStream.Close();
		if ( inStream.GetOK() ) // If it loaded OK ...
		{
			if ( bMerge )
				m_board.Merge(tmp);	// ... merge in the temporary board
			else
				m_board	= tmp;		// ... copy the temporary board
			if ( !bMerge )
				m_fileName = fileName;	// Only a regular open (not a merge) should update the filename
			ResetView();
			if ( bMerge )
				UpdateHistory("File->Open (merge into current)");
			else
				ResetHistory("File->Open");

			bOK = true;
		}
		else
			QMessageBox::information(this, tr("Unsupported VRT version"), tr(fileNameStr.c_str()));
	}
	else
		QMessageBox::information(this, tr("Unable to open file"), tr(fileNameStr.c_str()));

	if ( !bMerge || !bOK )	// A successful merge should not add the merged-in file to the recent files list
		UpdateRecentFiles(&fileName, bOK);	// Remove file from list if bOK == false
}

// File menu items
void MainWindow::New()
{
	SetCtrlKeyDown(false);	// May have just done a Ctrl+N.  Clear flag since key release can get missed.
	if ( GetIsModified() )
	{
		if ( QMessageBox::question(this, tr("Confirm New"),
										 tr("Your circuit is not saved. You will lose it if you make a new one.  Continue?"),
										 QMessageBox::Yes | QMessageBox::No, QMessageBox::No) == QMessageBox::No ) return;
	}
	m_board.Clear();
	m_fileName.clear();
	ResetView();
	ResetHistory("File->New");
}

void MainWindow::Open()
{
	SetCtrlKeyDown(false);	// May have just done a Ctrl+O.  Clear flag since key release can get missed.
	if ( GetIsModified() )
	{
		if ( QMessageBox::question(this, tr("Confirm Open"),
										 tr("Your circuit is not saved. You will lose it if you open a new one.  Continue?"),
										 QMessageBox::Yes | QMessageBox::No, QMessageBox::No) == QMessageBox::No ) return;
	}
	QString fileName = QFileDialog::getOpenFileName(this, tr("Open file"), ""/*directory*/,	tr("VeroRoute (*.vrt);;All Files (*)"));
	if ( !fileName.isEmpty() )
		OpenVrt(fileName);
}

void MainWindow::OpenRecent()
{
	if ( GetIsModified() )
	{
		if ( QMessageBox::question(this, tr("Confirm Open"),
										 tr("Your circuit is not saved. You will lose it if you open a new one.  Continue?"),
										 QMessageBox::Yes | QMessageBox::No, QMessageBox::No) == QMessageBox::No ) return;
	}

	QAction* pAction = qobject_cast<QAction*> ( sender() );
	if ( pAction )
	{
		QString fileName = pAction->data().toString();
		if ( !fileName.isEmpty() )
			OpenVrt(fileName);
	}
}

void MainWindow::Merge()
{
	SetCtrlKeyDown(false);	// May have just done a Ctrl+M.  Clear flag since key release can get missed.
	const Component& trax = m_board.GetCompMgr().GetTrax();
	if ( trax.GetSize() > 0 && !trax.GetIsPlaced() )
	{
		if ( QMessageBox::question(this, tr("Confirm merge into current"),
										 tr("Some areas of the grid are highlighted and floating. You will lose the tracks there.  Continue?"),
										 QMessageBox::Yes | QMessageBox::No, QMessageBox::No) == QMessageBox::No ) return;
	}
	QString fileName = QFileDialog::getOpenFileName(this, tr("Merge file"), ""/*directory*/,	tr("VeroRoute (*.vrt);;All Files (*)"));
	if ( !fileName.isEmpty() )
		OpenVrt(fileName, true);	// true ==> Merge
}

void MainWindow::Save()
{
	SetCtrlKeyDown(false);	// May have just done a Ctrl+S.  Clear flag since key release can get missed.

	if ( !GetIsModified() ) return;	// Already up-to-date
	if ( m_fileName.isEmpty() ) return SaveAs();

	DataStream outStream(DataStream::WRITE);
	const std::string fileNameStr = m_fileName.toStdString();
	if ( outStream.Open( fileNameStr.c_str() ) )
	{
		ui->statusBar->showMessage( tr("Saving..."), 500 );

		m_board.Save(outStream);
		outStream.Close();
		m_infoDlg->Update();	// Don't need a ResetView() but MUST update the initial string for the info dialog
	}
	else
		QMessageBox::information(this, tr("Unable to save file"), tr(fileNameStr.c_str()));
}

void MainWindow::SaveAs()
{
	bool bOK(false);

	SetCtrlKeyDown(false);	SetShiftKeyDown(false);	// May have just done a Ctrl+Shift+S.  Clear flags since key release can get missed.

	const QString fileName = GetSaveFileName(tr("Save file as"), tr("VeroRoute (*.vrt);;All Files (*)"), QString("vrt"));
	if ( !fileName.isEmpty() )
	{
		DataStream outStream(DataStream::WRITE);
		const std::string fileNameStr = fileName.toStdString();
		if ( outStream.Open( fileNameStr.c_str() ) )
		{
			ui->statusBar->showMessage( tr("Saving..."), 500 );

			m_board.Save(outStream);
			outStream.Close();
			m_infoDlg->Update();	// Don't need a ResetView() but MUST update the initial string for the info dialog
			m_fileName = fileName;

			bOK = true;

			UpdateWindowTitle();
		}
		else
			QMessageBox::information(this, tr("Unable to save file"), tr(fileNameStr.c_str()));
	}

	UpdateRecentFiles(&fileName, bOK);	// Remove file from list if bOK == false
}

void MainWindow::ImportTango()
{
	if ( GetIsModified() )
	{
		if ( QMessageBox::question(this, tr("Confirm Import"),
									 tr("Your circuit is not saved. You will lose it if you Import a new one.  Continue?"),
									 QMessageBox::Yes | QMessageBox::No, QMessageBox::No) == QMessageBox::No ) return;
	}
	QString fileName = QFileDialog::getOpenFileName(this, tr("Open file"), ""/*directory*/,	tr("Protel Netlist (*.net);;All Files (*)"));
	if ( !fileName.isEmpty() )
	{
		ui->statusBar->showMessage( tr("Importing..."), 500 );

		const std::string	fileNameStr = fileName.toStdString();
		std::string			errorStr;
		const bool bOK = m_board.ImportTango(GetTemplateManager(), fileNameStr, errorStr);
		m_fileName.clear();
		ResetView();
		ResetHistory("File->Import Netlist");
		if ( !bOK )
		{
			QMessageBox::information(this, tr("Error Importing Netlist"), tr(errorStr.c_str()));
		}
	}
}

void MainWindow::ImportOrcad()
{
	if ( GetIsModified() )
	{
		if ( QMessageBox::question(this, tr("Confirm Import"),
									 tr("Your circuit is not saved. You will lose it if you Import a new one.  Continue?"),
									 QMessageBox::Yes | QMessageBox::No, QMessageBox::No) == QMessageBox::No ) return;
	}
	QString fileName = QFileDialog::getOpenFileName(this, tr("Open file"), ""/*directory*/,	tr("Orcad2 Netlist (*.net);;All Files (*)"));
	if ( !fileName.isEmpty() )
	{
		ui->statusBar->showMessage( tr("Importing..."), 500 );

		const std::string	fileNameStr = fileName.toStdString();
		std::string			errorStr;
		const bool bOK = m_board.ImportOrcad(GetTemplateManager(), fileNameStr, errorStr);
		m_fileName.clear();
		ResetView();
		ResetHistory("File->Import Netlist");
		if ( !bOK )
		{
			QMessageBox::information(this, tr("Error Importing Netlist"), tr(errorStr.c_str()));
		}
	}
}

void MainWindow::WritePDF()
{
	m_pdfFileName = GetSaveFileName(tr("Choose a PDF File"), tr("PDF (*.pdf);;All Files (*)"), QString("pdf"));
	if ( !m_pdfFileName.isEmpty() )
	{
		ui->statusBar->showMessage( tr("Exporting to PDF..."), 500 );

		const int oldGridPixels	= m_board.GetGRIDPIXELS();
		const int pdfGridPixels = 120;	// 1200 dpi
		m_board.SetGRIDPIXELS(pdfGridPixels);
		m_XGRIDOFFSET = pdfGridPixels * ( 58 - m_board.GetCols() / 2 );	// A4 landscape is about 116 * 0.1 inches wide
		m_YGRIDOFFSET = pdfGridPixels * ( 41 - m_board.GetRows() / 2 );	// A4 landscape is about  82 * 0.1 inches tall

		m_bWritePDF = true;			// Makes paintEvent() write to PDF instead of pixmap
		RepaintSkipRouting(true);	// true  ==> force use of repaint() rather than update()
		m_bWritePDF = false;		// Makes paintEvent() go back to writing to pixmap

		m_XGRIDOFFSET = m_YGRIDOFFSET = 0;		// Restore zero offsets
		m_board.SetGRIDPIXELS(oldGridPixels);	// Restore number of pixels per grid square

		QDesktopServices::openUrl(m_pdfFileName);	// Ask the system to open the PDF file.
	}
}

void MainWindow::WriteGerber(const bool& bTwoLayers)
{
	m_bTwoLayers = bTwoLayers;
	m_board.SetHoleType(m_bTwoLayers ? HOLETYPE::PTH : HOLETYPE::NPTH);

	m_gerberFileName = GetSaveFileName(tr("Choose a Gerber File Prefix"), tr("All Files (*)"), QString(""));
	if ( !m_gerberFileName.isEmpty() )
	{
		ui->statusBar->showMessage( tr("Exporting to Gerber..."), 500 );

		const int oldGridPixels		= m_board.GetGRIDPIXELS();
		const int gerberGridPixels	= 1000;	// Gerber file had 4 decimal places per inch
		m_board.SetGRIDPIXELS(gerberGridPixels);

		m_bWriteGerber = true;		// Makes paintEvent() write to Gerber file instead of pixmap

		if ( m_gWriter.Open(m_gerberFileName.toStdString().c_str(), m_board, m_bTwoLayers) )
		{
			const int origlayer = m_board.GetCurrentLayer();
			for (int lyr = 0, lyrs = m_board.GetLyrs(); lyr < lyrs; lyr++)
			{
				m_board.SetCurrentLayer(lyr);
				RepaintSkipRouting(true);	// true  ==> force use of repaint() rather than update()
			}
			m_board.SetCurrentLayer(origlayer);
			m_gWriter.Close();
		}
		m_bWriteGerber = false;		// Makes paintEvent() go back to writing to pixmap

		m_board.SetGRIDPIXELS(oldGridPixels);	// Restore number of pixels per grid square

		// Ask the system to open the Gerber files
		QDesktopServices::openUrl(m_gerberFileName + ".GKO");
		QDesktopServices::openUrl(m_gerberFileName + ".GBL");
		QDesktopServices::openUrl(m_gerberFileName + ".GBS");
//		QDesktopServices::openUrl(m_gerberFileName + ".GBO");
		if ( m_bTwoLayers )
		{
			QDesktopServices::openUrl(m_gerberFileName + ".GTL");
			QDesktopServices::openUrl(m_gerberFileName + ".GTS");
		}
		QDesktopServices::openUrl(m_gerberFileName + ".GTO");
		QDesktopServices::openUrl(m_gerberFileName + ".DRL");
	}
}

void MainWindow::ClearRecentFiles()
{
	QSettings	settings("veroroute","veroroute");	// Organisation = "veroroute", Application = "veroroute"
	settings.setValue("recentFiles", QStringList());
	for (int i = 0; i < MAX_RECENT_FILES; i++)
		m_recentFileAction[i]->setVisible(false);
	ui->actionClearRecent->setEnabled(false);
	m_separator->setVisible(false);
}

void MainWindow::WritePNG()
{
	const QString fileName = GetSaveFileName(tr("Choose a PNG File"), tr("PNG (*.png);;All Files (*)"), QString("png"));
	if ( !fileName.isEmpty() )
	{
		ui->statusBar->showMessage( tr("Exporting to PNG..."), 500 );

		QFile file(fileName);
		file.open(QIODevice::WriteOnly);
		m_mainPixmap.save(&file, "PNG");

		QDesktopServices::openUrl(fileName);	// Ask the system to open the PNG file.
	}
}

void MainWindow::Quit()
{
	SetCtrlKeyDown(false);	// May have just done a Ctrl+Q.  Clear flag since key release can get missed.

	if ( GetIsModified() )
	{
		if ( QMessageBox::question(this, tr("Really Quit?"),
										tr("Your circuit is not saved. You will lose it if you quit.  Continue?"),
										QMessageBox::Yes | QMessageBox::No, QMessageBox::No) == QMessageBox::No ) return;
	}
	QApplication::quit();
}

// View menu items
void MainWindow::ZoomIn()
{
	if ( !CanZoomIn() ) return;		// Need this check for wheel zoom
	ZoomHelper(2);	// +2 ==> Change in GRIDPIXELS
	UpdateHistory("Zoom in");
}
void MainWindow::ZoomOut()
{
	if ( !CanZoomOut() ) return;	// Need this check for wheel zoom
	ZoomHelper(-2);	// -2 ==> Change in GRIDPIXELS
	UpdateHistory("Zoom out");
}
void MainWindow::ZoomHelper(int delta)	// delta == change in GRIDPIXELS
{
	int X, Y;
	GetPixMapXY(m_mousePos, X, Y);
	auto*		pH	= m_scrollArea->horizontalScrollBar();
	auto*		pV	= m_scrollArea->verticalScrollBar();
	const int	L	= pH->value();	// Left of visible area (measured in pixmap pixels)
	const int	T	= pV->value();	// Top  of visible area (measured in pixmap pixels)
	const int	W	= m_board.GetGRIDPIXELS();	// Current scale
	m_board.SetGRIDPIXELS(W + delta);			// Change scale

	DestroyPixmapCache();
	UpdateControls();
	RepaintSkipRouting();

	// Try to have same grid position under mouse after zoom
	pH->setValue(L + X * delta * 1.0 / W);
	pV->setValue(T + Y * delta * 1.0 / W);
}

// Edit menu items
void MainWindow::Undo()
{
	m_historyMgr.Lock();
	if ( m_historyMgr.Undo(m_board) )
	{
		const bool bCtrlKeyDown		= GetCtrlKeyDown();
		const bool bShiftKeyDown	= GetShiftKeyDown();
		ResetView(m_iTutorialNumber >= 0);	// This resets ctrl and shift key down
		SetCtrlKeyDown(bCtrlKeyDown);
		SetShiftKeyDown(bShiftKeyDown);
	}
	m_historyMgr.UnLock();
}
void MainWindow::Redo()
{
	m_historyMgr.Lock();
	if ( m_historyMgr.Redo(m_board) )
	{
		const bool bCtrlKeyDown		= GetCtrlKeyDown();
		const bool bShiftKeyDown	= GetShiftKeyDown();
		ResetView(m_iTutorialNumber >= 0);	// This resets ctrl and shift key down
		SetCtrlKeyDown(bCtrlKeyDown);
		SetShiftKeyDown(bShiftKeyDown);
	}
	m_historyMgr.UnLock();
}
void MainWindow::Copy()
{
	if ( GetCurrentTextId() != BAD_TEXTID )
	{
		if ( StringHelper::IsEmptyStr( GetCurrentTextRect().GetStr() ) ) return;	// Don't copy empty text boxes
		int iRow, iCol;
		GetFirstRowCol(iRow, iCol);
		m_board.AddTextBox(iRow, iCol);
		UpdateHistory("Copy text box");
		UpdateControls();
		RepaintSkipRouting();
		ShowTextDialog();
	}
	else
	{
		m_board.CopyUserComps();
		UpdateHistory("Copy part(s)");
		UpdateControls();
		UpdateBOM();
		RepaintSkipRouting();
	}
}
void MainWindow::Group()
{
	GroupManager& groupMgr = m_board.GetGroupMgr();
	groupMgr.Group();
	UpdateHistory("Group parts");
	UpdateControls();
	RepaintSkipRouting();
}
void MainWindow::Ungroup()
{
	GroupManager& groupMgr = m_board.GetGroupMgr();
	groupMgr.UnGroup(GetCurrentCompId());
	UpdateHistory("Ungroup parts");
	UpdateControls();
	RepaintSkipRouting();
}
void MainWindow::SelectAll()
{
	GroupManager& groupMgr = m_board.GetGroupMgr();

	// Clear current selection
	SetCurrentCompId(BAD_COMPID);
	groupMgr.ResetUserGroup( GetCurrentCompId() );	// Wipe user group

	const bool bRestrictToRects(false);
	m_board.SelectAllComps(bRestrictToRects);	// Put components in user-group
	UpdateHistory("Select all parts");
	UpdateControls();
	RepaintSkipRouting();
}
void MainWindow::SelectAllInRects()
{
	GroupManager& groupMgr = m_board.GetGroupMgr();

	// Clear current selection
	SetCurrentCompId(BAD_COMPID);
	groupMgr.ResetUserGroup( GetCurrentCompId() );	// Wipe user group

	const bool bRestrictToRects(true);
	m_board.SelectAllComps(bRestrictToRects);	// Put components in user-group
	UpdateControls();
}
void MainWindow::Delete()
{
	if ( m_board.GetCompEdit() && GetCurrentShapeId() != BAD_ID )
	{
		GetCompDefiner().DestroyShape();
		SetCurrentShapeId(BAD_ID);
		UpdateHistory("Delete shape");
		RepaintSkipRouting();
	}
	else if ( GetCurrentTextId() != BAD_TEXTID )
	{
		m_board.GetTextMgr().DestroyRect(GetCurrentTextId());
		SetCurrentTextId(BAD_TEXTID);
		SetResizingText(false);
		UpdateHistory("Delete text box");
		RepaintSkipRouting();
	}
	else
	{
		// Ask for confirmation unless deleting only wires and markers
		if ( m_board.ConfirmDestroyUserComps() &&
			 QMessageBox::question(this, tr("Confirm Delete"),
										 tr("Are you sure you want to delete the selected part(s)?"),
										 QMessageBox::Yes | QMessageBox::No, QMessageBox::No) == QMessageBox::No ) return;
		m_board.DestroyUserComps();
		SetCurrentCompId(BAD_COMPID);
		UpdateHistory("Delete part(s)");
		UpdateControls();
		UpdateBOM();
		RepaintWithRouting();
		ListNodes();
	}
}

// Windows menu items
void MainWindow::ShowDlg(QWidget* p)	{ p->showNormal();	p->raise();	p->activateWindow(); }
void MainWindow::ShowControlDialog()	{ ShowDlg(m_dockControlDlg); }
void MainWindow::HideControlDialog()	{ m_dockControlDlg->hide(); }
void MainWindow::ShowRenderingDialog()	{ ShowDlg(m_renderingDlg); }
void MainWindow::ShowWireDialog()		{ ShowDlg(m_wireDlg); }
void MainWindow::ShowHotkeysDialog()	{ ShowDlg(m_hotkeysDlg); }
void MainWindow::ShowInfoDialog()		{ ShowDlg(m_infoDlg); }
void MainWindow::ShowCompDialog()		{ ShowDlg(m_dockCompDlg); }
void MainWindow::HideCompDialog()		{ m_dockCompDlg->hide(); }
void MainWindow::ShowTextDialog()		{ ShowDlg(m_textDlg); }
void MainWindow::ShowBomDialog()		{ UpdateBOM();				ShowDlg(m_bomDlg); }
void MainWindow::ShowTemplatesDialog()	{ UpdateTemplatesDialog();	ShowDlg(m_templatesDlg); }
void MainWindow::ShowPinDialog()		{ m_pinDlg->Update();		ShowDlg(m_pinDlg); }
void MainWindow::HidePinDialog()		{ m_pinDlg->hide(); }

// Layers menu items
void MainWindow::AddLayer()
{
	assert( m_board.GetLyrs() == 1 );
	m_board.GrowThenPan(1, 0, 0, 0, 0);
	m_board.SetCurrentLayer(1);
	m_board.SetViasEnabled(true);
	UpdateHistory("Add top layer");
	UpdateControls();
	RepaintWithRouting();
	ListNodes();
}
void MainWindow::RemoveLayer()
{
	assert( m_board.GetLyrs() == 2 );
	m_board.GrowThenPan(-1, 0, 0, 0, 0);
	m_board.SetCurrentLayer(0);
	UpdateHistory("Remove top layer");
	UpdateControls();
	RepaintWithRouting();
	ListNodes();
}
void MainWindow::SwitchLayer()
{
	assert( m_board.GetLyrs() == 2 );
	m_board.SetCurrentLayer( ( m_board.GetCurrentLayer() + 1 ) % 2 );
	UpdateHistory("Toggle layer");
	UpdateControls();
	RepaintWithRouting();
}
void MainWindow::ToggleVias()
{
	assert( m_board.GetLyrs() == 2 );
	m_board.SetViasEnabled( !m_board.GetViasEnabled() );
	UpdateHistory("Toggle vias");
	UpdateControls();
	RepaintWithRouting();
	ListNodes();
}

// Help menu items
void MainWindow::ShowAbout()
{
	std::string str = std::string("VeroRoute - Qt based Veroboard/Perfboard/PCB layout & routing application.\n\n")
					+ std::string("Version ") + std::string(szVEROROUTE_VERSION) + std::string("\n\n")
					+ std::string("Copyright (C) 2017  Alex Lawrow    ( dralx@users.sourceforge.net )\n\n")
					+ std::string("This program is free software: you can redistribute it and/or modify\n")
					+ std::string("it under the terms of the GNU General Public License as published by\n")
					+ std::string("the Free Software Foundation, either version 3 of the License, or\n")
					+ std::string("(at your option) any later version.\n\n")
					+ std::string("This program is distributed in the hope that it will be useful,\n")
					+ std::string("but WITHOUT ANY WARRANTY; without even the implied warranty of\n")
					+ std::string("MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.\n")
					+ std::string("See the GNU General Public License for more details.\n\n")
					+ std::string("You should have received a copy of the GNU General Public License\n")
					+ std::string("along with this program.  If not, see <http://www.gnu.org/licenses/>.");
	QMessageBox::information(this, tr("About"), tr(str.c_str()));
}
void MainWindow::ShowSupport()
{
	QDesktopServices::openUrl(QString("https://sourceforge.net/p/veroroute/discussion/"));
}
void MainWindow::LoadFirstTutorial()
{
	if ( GetIsModified() )
	{
		if ( QMessageBox::question(this, tr("Confirm Load Tutorials"),
										 tr("Your circuit is not saved. You will lose it if you load the tutorials.  Continue?"),
										 QMessageBox::Yes | QMessageBox::No, QMessageBox::No) == QMessageBox::No ) return;
	}
	m_iTutorialNumber = 0;	LoadTutorial();
}
void MainWindow::LoadPrevTutorial()	{ m_iTutorialNumber--;	LoadTutorial(); }
void MainWindow::LoadNextTutorial()	{ m_iTutorialNumber++;	LoadTutorial(); }
void MainWindow::LoadTutorial()
{
	char fileName[256] = {'\0'};
	sprintf(fileName, "%s/tutorials/tutorial_%d.vrt", m_tutorialsPathStr.c_str(), m_iTutorialNumber);
	DataStream inStream(DataStream::READ);
	if ( inStream.Open( fileName ) )
	{
		m_board.Load(inStream);
		inStream.Close();
		if ( inStream.GetOK() )
		{
			m_fileName = fileName;
			ResetView(true);	// true ==> tutorial mode
			ResetHistory("File->Open");
		}
		else
			QMessageBox::information(this, tr("Unsupported VRT version"), tr(fileName));
	}
	else
		QMessageBox::information(this, tr("Unable to open tutorial file"), tr(fileName));
}

// Check version against Sourceforge
void MainWindow::UpdateCheck()
{
	m_networkMgr.get(QNetworkRequest(QUrl("https://sourceforge.net/projects/veroroute/files/")));
}
void MainWindow::HandleNetworkReply(QNetworkReply* pReply)
{
	pReply->deleteLater();

	if ( pReply->error() == QNetworkReply::UnknownNetworkError )
	{
		QMessageBox::information(this, "Check failed", "There was a network error.\nRead the file OpenSSL.txt provided with VeroRoute.");
		return;
	}
	else if ( pReply->error() != QNetworkReply::NoError )
	{
		QMessageBox::information(this, "Check failed", "There was a network error.");
		return;
	}
	const QString contentType = pReply->header(QNetworkRequest::ContentTypeHeader).toString();
	if ( !contentType.contains("charset=utf-8") )
	{
		QMessageBox::information(this, "Check failed", "Unsupported character set.");
		return; // Content charsets other than utf-8 are not implemented yet
	}
	const QString		qstrHtml	= QString::fromUtf8(pReply->readAll());
	const std::string	strHtml		= qstrHtml.toStdString();
	const auto			pos			= strHtml.find("Version ");	// Search for the text "Version " on the page
	if ( pos == std::string::npos )
	{
		QMessageBox::information(this, "Check failed", "Could not find Version number on Sourceforge.");
		return;
	}

	const std::string	versionStr	 = strHtml.substr(pos+8, 4);	// Should be something like "1.23"
	const double		dSiteVersion = atof(versionStr.c_str()) ;
	const double		dThisVersion = atof(szVEROROUTE_VERSION);

	static char bufferThis[8];
	sprintf(bufferThis, "V%s", szVEROROUTE_VERSION);

	static char bufferOther[128];
	sprintf(bufferOther, "A new version is available!\nV%s can be downloaded from\nhttps://sourceforge.net/projects/veroroute/files/", versionStr.c_str());

	if ( dSiteVersion > dThisVersion )
		QMessageBox::information(this, bufferThis, bufferOther);
	else if ( dSiteVersion == dThisVersion )
		QMessageBox::information(this, bufferThis, "You have the latest version.");
	else
		QMessageBox::information(this, bufferThis, "You have a newer version than Sourceforge.");
}

// View controls (Update history BEFORE calling UpdateControls() since that triggers more history writes)
void MainWindow::TrackSliderChanged(int i)		{ if ( m_board.SetTrackSliderValue(i) )	{ UpdateHistory("Track slider change");		UpdateControls(); DestroyPixmapCache(); RepaintSkipRouting(); } }
void MainWindow::SaturationSliderChanged(int i) { if ( m_board.SetSaturation(i) )		{ UpdateHistory("Saturation change");		UpdateControls(); DestroyPixmapCache(); RepaintSkipRouting();  } }
void MainWindow::CompSliderChanged(int i)		{ if ( m_board.SetCompSliderValue(i) )	{ UpdateHistory("Part slider change");		UpdateControls(); RepaintSkipRouting();  } }
void MainWindow::FillSliderChanged(int i)		{ if ( m_board.SetFillSaturation(i) )	{ UpdateHistory("Fill opacity change");		UpdateControls(); DestroyPixmapCache();	RepaintSkipRouting();  } }
void MainWindow::SetShowGrid(bool b)			{ if ( m_board.SetShowGrid(b) )			{ UpdateHistory("Toggle grid");				UpdateControls(); RepaintSkipRouting();  } }
void MainWindow::SetShowText(bool b)			{ if ( m_board.SetShowText(b) )			{ UpdateHistory("Toggle text");				UpdateControls(); RepaintSkipRouting();  } }
void MainWindow::SetFlipH(bool b)				{ if ( m_board.SetFlipH(b) )			{ UpdateHistory("Toggle flip horizontal");	UpdateControls(); RepaintSkipRouting();  } }
void MainWindow::SetFlipV(bool b)				{ if ( m_board.SetFlipV(b) )			{ UpdateHistory("Toggle flip vertical");	UpdateControls(); RepaintSkipRouting();  } }
void MainWindow::SetShowPinLabels(bool b)		{ if ( m_board.SetShowPinLabels(b) )	{ UpdateHistory("Toggle pin labels");		UpdateControls(); RepaintSkipRouting();  } }
void MainWindow::SetFill(bool b)
{
	if ( m_board.SetGroundFill(b) )
	{
		if ( b && m_board.GetCurrentLayer() == 0 ) m_board.SetGroundNodeId0( m_board.GetCurrentNodeId() );
		if ( b && m_board.GetCurrentLayer() == 1 ) m_board.SetGroundNodeId1( m_board.GetCurrentNodeId() );
		UpdateHistory("Toggle ground-fill");
		UpdateControls();
		RepaintSkipRouting();
	}
}
void MainWindow::Crop()					{ if ( m_board.Crop() ) { UpdateHistory("Crop");	RepaintSkipRouting(); } }
void MainWindow::MarginChanged(int i)	{ if ( m_board.SetCropMargin(i) && m_board.Crop() ) { UpdateHistory("Margin change");	RepaintSkipRouting(); } }
void MainWindow::ToggleGrid()			{ SetShowGrid( !m_board.GetShowGrid() ); }
void MainWindow::ToggleText()			{ SetShowText( !m_board.GetShowText() ); if ( !m_board.GetShowText() ) SetCurrentTextId(BAD_TEXTID); }
void MainWindow::ToggleFlipH()			{ SetFlipH( !m_board.GetFlipH() ); }
void MainWindow::ToggleFlipV()			{ SetFlipV( !m_board.GetFlipV() ); }
void MainWindow::TogglePinLabels()		{ SetShowPinLabels( !m_board.GetShowPinLabels() ); }

// Toolbar items
void MainWindow::VeroV()				{ SetTracksVeroV(true); }
void MainWindow::VeroH()				{ SetTracksVeroH(true); }
void MainWindow::Fat()					{ SetTracksFat(true);}
void MainWindow::Thin()					{ SetTracksThin(true); }
void MainWindow::Curved()				{ SetTracksCurved(true); }
void MainWindow::ToggleDiagsMin()		{ if ( m_board.GetDiagsMode() == DIAGSMODE::MIN ) SetDiagonalsOff(true); else SetDiagonalsMin(true); }
void MainWindow::ToggleDiagsMax()		{ if ( m_board.GetDiagsMode() == DIAGSMODE::MAX ) SetDiagonalsOff(true); else SetDiagonalsMax(true); }
void MainWindow::ToggleFill()			{ SetFill( !m_board.GetGroundFill() ); }
void MainWindow::ToggleSelectArea()
{
	SetDefiningRect( !GetDefiningRect() );
	if ( GetDefiningRect() )
		centralWidget()->setCursor(Qt::SizeFDiagCursor);
	else
		centralWidget()->setCursor(Qt::OpenHandCursor);
}

// Part controls
void MainWindow::SetCompName(const QString& str)
{
	if ( m_board.GetGroupMgr().GetNumUserComps() != 1 ) return;
	Component&			comp	= m_board.GetUserComponent();
	const std::string	newStr	= str.toStdString();
	if ( newStr == comp.GetNameStr() ) return;
	comp.SetNameStr(newStr);
	UpdateHistory("Part name change", comp.GetId());
	UpdateBOM();
	RepaintSkipRouting();
}
void MainWindow::SetCompValue(const QString& str)
{
	if ( m_board.GetGroupMgr().GetNumUserComps() != 1 ) return;
	Component&			comp	= m_board.GetUserComponent();
	const std::string	newStr	= str.toStdString();
	if ( newStr == comp.GetValueStr() ) return;
	comp.SetValueStr(newStr);
	UpdateHistory("Part value change", comp.GetId());
	UpdateBOM();
	RepaintSkipRouting();
}
void MainWindow::SetCompType(const QString& str)
{
	if ( m_board.GetDisableChangeType() || str.isEmpty() ) return;
	if ( m_board.GetGroupMgr().GetNumUserComps() != 1 ) return;
	Component&	comp	= m_board.GetUserComponent();
	const COMP	eType	= GetTypeFromTypeStr( str.toStdString() );
	if ( comp.GetType() == eType ) return;	// No change

	m_board.ChangeTypeUserComp(eType);
	UpdateHistory("Change part type");
	UpdateControls();
	RepaintWithRouting();
	ListNodes();	// Slow due lots of MH calcs
}
void MainWindow::CompRotate(const bool& bCW)
{
	if ( m_board.GetDisableRotate() ) return;

	m_board.RotateUserComps(bCW);

	UpdateHistory(bCW ? "Rotate part CW" : "Rotate part CCW");
	UpdateControls();
	RepaintWithRouting();
	ListNodes();	// Slow due lots of MH calcs
}
void MainWindow::CompStretch(const bool& bGrow)
{
	if ( m_board.GetDisableStretch(bGrow) ) return;

	m_board.StretchUserComp(bGrow);

	UpdateHistory(bGrow ? "Increase part length" : "Decrease part length");
	UpdateControls();
	RepaintWithRouting();
	ListNodes();	// Slow due lots of MH calcs
}
void MainWindow::CompStretchWidth(const bool& bGrow)
{
	if ( m_board.GetDisableStretchWidth(bGrow) ) return;

	m_board.StretchWidthUserComp(bGrow);

	UpdateHistory(bGrow ? "Increase part width" : "Decrease part width");
	UpdateControls();
	RepaintWithRouting();
	ListNodes();	// Slow due lots of MH calcs
}
void MainWindow::CompTextMove(const int& deltaRow, const int& deltaCol)
{
	if ( m_board.GetDisableCompText() ) return;

	m_board.MoveUserCompText(deltaRow, deltaCol);
	if		( deltaRow < 0 ) UpdateHistory("Move label up");
	else if ( deltaRow > 0 ) UpdateHistory("Move label down");
	else if ( deltaCol < 0 ) UpdateHistory("Move label left");
	else if ( deltaCol > 0 ) UpdateHistory("Move label right");
	else					 UpdateHistory("Move label centre");

	RepaintSkipRouting();
}

// Bad Nodes lists
void MainWindow::SetNodeId(QListWidgetItem* item)
{
	SetCurrentNodeId( atoi(item->text().toStdString().c_str()) );
	RepaintWithRouting();
	// Grab focus so we can accept keyboard/mouse input to paint the selected nodeId
	activateWindow();
	setFocus(Qt::ActiveWindowFocusReason);
}
void MainWindow::ListNodes(bool bRebuild)
{
	if ( bRebuild )
	{
		const bool& bAutoRouting = m_board.GetRoutingEnabled();

		// If auto-routing is enabled, then the routing costs will have been set
		// and we can check them instead of looking at "Complete" flags.

		// If auto-routing is disabled, we need to call CheckAllComplete() with a copy of
		// the board so that current routing results are not wiped (e.g. we may have a nodeId selected and showing connectivity).

		Board* pBoard = ( bAutoRouting ) ? &m_board : new Board(m_board, false);	// false ==> fast copy without RebuildAdjacencies()

		if ( !bAutoRouting ) pBoard->CheckAllComplete();	// Slow !!!

		m_controlDlg->ClearLists();

		CompManager&		compMgr		= pBoard->GetCompMgr();
		NodeInfoManager&	nodeInfoMgr	= pBoard->GetNodeInfoMgr();
		for (size_t i = 0; i < nodeInfoMgr.GetSize(); i++)
		{
			NodeInfo* p = nodeInfoMgr.GetAt(i);
			if ( p->GetNodeId() == BAD_NODEID ) continue;

			std::stringstream mystream;
			mystream << p->GetNodeId();
			std::string myStr = mystream.str();

			const bool bFloating = p->GetHasFloatingComp(compMgr);
			const bool bComplete = ( bAutoRouting ) ? ( p->GetCost() == 0 ) : p->GetComplete();
			const bool bBroken	 = bFloating || !bComplete;
			m_controlDlg->AddListItem(myStr, bBroken, bFloating);
		}
		if ( !bAutoRouting) delete pBoard;	// If we made a copy of the board then delete it
	}
	m_controlDlg->SetListItems( GetCurrentNodeId() );	// Highlight current NodeId in the lists
}

// Routing controls
void MainWindow::EnableRouting(bool b)
{
	if ( !m_board.SetRoutingEnabled(b) ) return;	// Quit if no change
	if ( !b ) m_board.WipeAutoSetPoints();
	UpdateHistory(b ? "Enable Auto-Routing" : "Disable Auto-Routing");
	UpdateControls();
	RepaintWithRouting();
	ListNodes();
}

void MainWindow::EnableFastRouting(bool b)
{
	const int iMethod = ( b ) ? 0 : 1;
	if ( !m_board.SetRoutingMethod(iMethod) ) return;	// Quit if no change
	if ( !b ) m_board.WipeAutoSetPoints();
	UpdateHistory(b ? "Enable Fast-Routing" : "Disable Fast-Routing");
	UpdateControls();
	RepaintWithRouting();
	ListNodes();
}

void MainWindow::Paste()		// On hitting the Paste button ...
{
	if ( !m_board.GetRoutingEnabled() ) return;
	m_board.PasteTracks(false);	// false ==> Don't wipe redundant track portions
	if ( m_board.GetVeroTracks() )  m_board.AutoFillVero();
	UpdateHistory("Paste Track");
	UpdateControls();
	RepaintWithRouting();
	ListNodes();
}
void MainWindow::Tidy()	// On hitting the Paste+Tidy button ...
{
	if ( m_board.GetRoutingEnabled() ) return;
	m_board.PasteTracks(true);	// true ==> Wipe redundant track portions
	if ( m_board.GetVeroTracks() ) m_board.AutoFillVero();
	UpdateHistory("Tidy Tracks");
	UpdateControls();
	RepaintWithRouting();
	ListNodes();
}
void MainWindow::WipeTracks()	// On hitting the Wipe All button ...
{
	if ( m_board.GetDisableWipe() ) return;
	m_board.WipeTracks();
	m_board.SetRoutingEnabled(false);
	UpdateHistory("Wipe Tracks");
	UpdateControls();
	RepaintWithRouting();
	ListNodes();
}

// Track controls
DIAGSMODE oldDiagsMode(DIAGSMODE::OFF);

void MainWindow::SetTracksVeroV(bool b)
{
	if (!b) return;
	if ( m_board.GetVeroTracks() && m_board.GetVerticalStrips() ) return UpdateControls();
	if ( !m_board.GetVeroTracks() )	oldDiagsMode = m_board.GetDiagsMode();	// Log old diag mode before setting DIAGSMODE::OFF
	const bool bDiagsModeChanged = m_board.SetDiagsMode(DIAGSMODE::OFF);
	m_board.SetVeroTracks(true);
	m_board.SetVerticalStrips(true);
	UpdateHistory("Vero tracks (vertical)");
	UpdateControls();
	if ( bDiagsModeChanged ) { RepaintWithRouting(); ListNodes(); } else RepaintSkipRouting();
}
void MainWindow::SetTracksVeroH(bool b)
{
	if (!b) return;
	if ( m_board.GetVeroTracks() && !m_board.GetVerticalStrips() ) return UpdateControls();
	if ( !m_board.GetVeroTracks() )	oldDiagsMode = m_board.GetDiagsMode();	// Log old diag mode before setting DIAGSMODE::OFF
	const bool bDiagsModeChanged = m_board.SetDiagsMode(DIAGSMODE::OFF);
	m_board.SetVeroTracks(true);
	m_board.SetVerticalStrips(false);
	UpdateHistory("Vero tracks (horizontal)");
	UpdateControls();
	if ( bDiagsModeChanged ) { RepaintWithRouting(); ListNodes(); } else RepaintSkipRouting();
}
void MainWindow::SetTracksFat(bool b)
{
	if (!b) return;
	if ( !m_board.GetCurvedTracks() && !m_board.GetVeroTracks() && m_board.GetFatTracks() ) return UpdateControls();
	bool bDiagsModeChanged(false);
	if ( m_board.SetVeroTracks(false) )	// If changed from Vero style ...
		bDiagsModeChanged = m_board.SetDiagsMode(oldDiagsMode);	// ... restore old diag mode
	m_board.SetCurvedTracks(false);
	m_board.SetFatTracks(true);
	UpdateHistory("Fat tracks");
	UpdateControls();
	DestroyPixmapCache();
	if ( bDiagsModeChanged ) { RepaintWithRouting(); ListNodes(); } else RepaintSkipRouting();
}
void MainWindow::SetTracksThin(bool b)
{
	if (!b) return;
	if ( !m_board.GetCurvedTracks() && !m_board.GetVeroTracks() && !m_board.GetFatTracks() ) return UpdateControls();
	bool bDiagsModeChanged(false);
	if ( m_board.SetVeroTracks(false) )	// If changed from Vero style ...
		bDiagsModeChanged = m_board.SetDiagsMode(oldDiagsMode);	// ... restore old diag mode
	m_board.SetCurvedTracks(false);
	m_board.SetFatTracks(false);
	UpdateHistory("Thin tracks");
	UpdateControls();
	DestroyPixmapCache();
	if ( bDiagsModeChanged ) { RepaintWithRouting(); ListNodes(); } else RepaintSkipRouting();
}
void MainWindow::SetTracksCurved(bool b)
{
	if (!b) return;
	if ( m_board.GetCurvedTracks() && !m_board.GetVeroTracks() ) return UpdateControls();
	bool bDiagsModeChanged(false);
	if ( m_board.SetVeroTracks(false) )	// If changed from Vero style ...
		bDiagsModeChanged = m_board.SetDiagsMode(oldDiagsMode);	// ... restore old diag mode
	m_board.SetCurvedTracks(true);
	UpdateHistory("Curved tracks");
	UpdateControls();
	DestroyPixmapCache();
	if ( bDiagsModeChanged ) { RepaintWithRouting(); ListNodes(); } else RepaintSkipRouting();
}
void MainWindow::SetDiagonalsOff(bool b)
{
	if ( b && m_board.SetDiagsMode(DIAGSMODE::OFF) )	{ UpdateHistory("Diagonals off"); UpdateControls(); DestroyPixmapCache(); RepaintWithRouting(); ListNodes(); }
}
void MainWindow::SetDiagonalsMin(bool b)
{
	const bool bListNodes = ( m_board.GetDiagsMode() == DIAGSMODE::OFF );	// Only ListNodes() again if necessary
	if ( b && m_board.SetDiagsMode(DIAGSMODE::MIN) )	{ UpdateHistory("Diagonals min"); UpdateControls(); DestroyPixmapCache(); RepaintWithRouting(); if ( bListNodes ) ListNodes(); }
}
void MainWindow::SetDiagonalsMax(bool b)
{
	const bool bListNodes = ( m_board.GetDiagsMode() == DIAGSMODE::OFF );	// Only ListNodes() again if necessary
	if ( b && m_board.SetDiagsMode(DIAGSMODE::MAX) )	{ UpdateHistory("Diagonals max"); UpdateControls(); DestroyPixmapCache(); RepaintWithRouting(); if ( bListNodes ) ListNodes(); }
}
void MainWindow::SetPadWidth(int i)			{ if ( m_board.SetPAD_PERCENT(i)   ) { UpdateHistory("Pad width change");				UpdateControls();	DestroyPixmapCache();	RepaintSkipRouting(); } }
void MainWindow::SetTrackWidth(int i)		{ if ( m_board.SetTRACK_PERCENT(i) ) { UpdateHistory("Track width change");				UpdateControls();	DestroyPixmapCache();	RepaintSkipRouting(); } }
void MainWindow::SetHoleWidth(int i)		{ if ( m_board.SetHOLE_PERCENT(i)  ) { UpdateHistory("Hole width change");				UpdateControls();	RepaintSkipRouting(); } }
void MainWindow::SetGapWidth(int i)			{ if ( m_board.SetGAP_PERCENT(i)   ) { UpdateHistory("Gap width change");				UpdateControls();	RepaintSkipRouting(); } }
void MainWindow::SetMaskWidth(int i)		{ if ( m_board.SetMASK_PERCENT(i)  ) { UpdateHistory("Solder mask margin change");		UpdateControls();	RepaintSkipRouting(); } }
void MainWindow::SetSilkWidth(int i)		{ if ( m_board.SetSILK_PERCENT(i)  ) { UpdateHistory("Silkscreen line width change");	UpdateControls();	RepaintSkipRouting(); } }
void MainWindow::SetEdgeWidth(int i)		{ if ( m_board.SetEDGE_PERCENT(i)  ) { UpdateHistory("Board edge margin change");		UpdateControls();	RepaintSkipRouting(); } }
void MainWindow::SetViaPadWidth(int i)		{ if ( m_board.SetVIAPAD_PERCENT(i)) { UpdateHistory("Via pad width change");			UpdateControls();	RepaintSkipRouting(); } }
void MainWindow::SetViaHoleWidth(int i)		{ if ( m_board.SetVIAHOLE_PERCENT(i)){ UpdateHistory("Via hole width change");			UpdateControls();	RepaintSkipRouting(); } }

// Rendering dialog
void MainWindow::SetTextSizeComp(int i)		{ if ( m_board.SetTextSizeComp(i) )		  { UpdateHistory("Text size change (component)");	RepaintSkipRouting(); } }
void MainWindow::SetTextSizePins(int i)		{ if ( m_board.SetTextSizePins(i) )		  { UpdateHistory("Text size change (pins)");		RepaintSkipRouting(); } }
void MainWindow::SetTargetRows(int i)		{ if ( m_board.SetTargetRows(i) )		  { UpdateHistory("Target board height change");	RepaintSkipRouting(); } }
void MainWindow::SetTargetCols(int i)		{ if ( m_board.SetTargetCols(i) )		  { UpdateHistory("Target board width change");		RepaintSkipRouting(); } }
void MainWindow::SetShowTarget(bool b)		{ if ( m_board.SetShowTarget(b) )		  { UpdateHistory("Target board on/off");			RepaintSkipRouting(); } }
void MainWindow::SetAntialiasOff(bool b)	{ if ( b && m_board.SetRenderQuality(0) ) { UpdateHistory("Anti-alias off");  DestroyPixmapCache(); RepaintSkipRouting(); } }
void MainWindow::SetAntialiasOn(bool b)		{ if ( b && m_board.SetRenderQuality(1) ) { UpdateHistory("Anti-alias on");   DestroyPixmapCache(); RepaintSkipRouting(); } }
void MainWindow::SetAntialiasHigh(bool b)	{ if ( b && m_board.SetRenderQuality(2) ) { UpdateHistory("Anti-alias high"); DestroyPixmapCache(); RepaintSkipRouting(); } }

// Wire dialog
void MainWindow::SetWireShare(bool b)		{ if ( m_board.SetWireShare(b) )		  { UpdateHistory("Wire hole-sharing on/off");	RepaintSkipRouting(); } }
void MainWindow::SetWireCross(bool b)		{ if ( m_board.SetWireCross(b) )		  { UpdateHistory("Wire crossing on/off");		RepaintSkipRouting(); } }

// Text box dialog
void MainWindow::SizeChanged(int i)			{ if ( GetCurrentTextId() != BAD_TEXTID && GetCurrentTextRect().SetSize(i) )												{ UpdateTextDialog(); RepaintSkipRouting(); } }
void MainWindow::ToggleBold()				{ if ( GetCurrentTextId() != BAD_TEXTID && GetCurrentTextRect().SetStyle(GetCurrentTextRect().GetStyle() ^ TEXT_BOLD     ) ){ UpdateTextDialog(); RepaintSkipRouting(); } }
void MainWindow::ToggleItalic()				{ if ( GetCurrentTextId() != BAD_TEXTID && GetCurrentTextRect().SetStyle(GetCurrentTextRect().GetStyle() ^ TEXT_ITALIC   ) ){ UpdateTextDialog(); RepaintSkipRouting(); } }
void MainWindow::ToggleUnderline()			{ if ( GetCurrentTextId() != BAD_TEXTID && GetCurrentTextRect().SetStyle(GetCurrentTextRect().GetStyle() ^ TEXT_UNDERLINE) ){ UpdateTextDialog(); RepaintSkipRouting(); } }
void MainWindow::AlignL()					{ if ( GetCurrentTextId() != BAD_TEXTID && GetCurrentTextRect().SetFlags(Qt::AlignLeft) )									{ UpdateTextDialog(); RepaintSkipRouting(); } }
void MainWindow::AlignC()					{ if ( GetCurrentTextId() != BAD_TEXTID && GetCurrentTextRect().SetFlags(Qt::AlignHCenter) )								{ UpdateTextDialog(); RepaintSkipRouting(); } }
void MainWindow::AlignR()					{ if ( GetCurrentTextId() != BAD_TEXTID && GetCurrentTextRect().SetFlags(Qt::AlignRight) )									{ UpdateTextDialog(); RepaintSkipRouting(); } }
void MainWindow::AlignJ()					{ if ( GetCurrentTextId() != BAD_TEXTID && GetCurrentTextRect().SetFlags(Qt::AlignJustify) )								{ UpdateTextDialog(); RepaintSkipRouting(); } }
void MainWindow::SetText(const QString& s)	{ if ( GetCurrentTextId() != BAD_TEXTID && GetCurrentTextRect().SetStr(s.toStdString()) )									{ UpdateTextDialog(); RepaintSkipRouting(); } }
void MainWindow::ChooseTextColor()
{
	if ( GetCurrentTextId() == BAD_TEXTID ) return;
	TextRect& rect = GetCurrentTextRect();
	QColor oldColor = QColor(rect.GetR(), rect.GetG(), rect.GetB());
	QColor newColor	= QColorDialog::getColor(oldColor, this );
	if ( newColor.isValid() && oldColor != newColor )
	{
		int r(0), g(0), b(0);
		newColor.getRgb(&r,&g,&b);
		if ( rect.SetRGB(r,g,b) ) { UpdateTextDialog(); RepaintSkipRouting(); }
	}
}

// Component editor
void MainWindow::DefinerSetValueStr(const QString& str)		{ if ( GetCompDefiner().SetValueStr(str.toStdString())	) { UpdateHistory("Edit Value string", 0); EnableCompDialogControls(); } }
void MainWindow::DefinerSetPrefixStr(const QString& str)	{ if ( GetCompDefiner().SetPrefixStr(str.toStdString())	) { UpdateHistory("Edit Prefix string",0); EnableCompDialogControls(); } }
void MainWindow::DefinerSetTypeStr(const QString& str)		{ if ( GetCompDefiner().SetTypeStr(str.toStdString())	) { UpdateHistory("Edit Type string",  0); EnableCompDialogControls(); } }
void MainWindow::DefinerSetImportStr(const QString& str)	{ if ( GetCompDefiner().SetImportStr(str.toStdString())	) { UpdateHistory("Edit Import string",0); EnableCompDialogControls(); } }
void MainWindow::DefinerSetPinShapeType(const QString& str)	{ if ( GetCompDefiner().SetPinType(str.toStdString())	) { UpdateHistory("Action"); EnableCompDialogControls(); } }
void MainWindow::DefinerSetShapeType(const QString& str)	{ if ( GetCompDefiner().SetType(str.toStdString())		) { UpdateHistory("Action"); EnableCompDialogControls(); RepaintSkipRouting(); } }
void MainWindow::DefinerToggledPinLabels(bool b)
{
	CompDefiner& compDefiner = GetCompDefiner();
	bool bChanged(false);
	if ( b )
		bChanged = compDefiner.SetPinFlags( compDefiner.GetPinFlags() | PIN_LABELS );
	else
		bChanged = compDefiner.SetPinFlags( compDefiner.GetPinFlags() & ~PIN_LABELS );
	if ( bChanged )
	{
		UpdateHistory("Toggle pin labels");
		EnableCompDialogControls();
	}
}
void MainWindow::DefinerToggleShapeLine(bool b)
{
	const bool bChanged = GetCompDefiner().SetLine(b);
	if ( bChanged )
	{
		UpdateHistory("Toggle shape line");
		EnableCompDialogControls();
		RepaintSkipRouting();
	}
}
void MainWindow::DefinerToggleShapeFill(bool b)
{
	const bool bChanged = GetCompDefiner().SetFill(b);
	if ( bChanged )
	{
		UpdateHistory("Toggle shape fill");
		EnableCompDialogControls();
		RepaintSkipRouting();
	}
}
void MainWindow::DefinerWidthChanged(int i)	{ if ( GetCompDefiner().SetWidth(i)		) { UpdateHistory("Action"); EnableCompDialogControls(); UpdateCompDialog(); RepaintSkipRouting(); } }
void MainWindow::DefinerHeightChanged(int i){ if ( GetCompDefiner().SetHeight(i)	) { UpdateHistory("Action"); EnableCompDialogControls(); UpdateCompDialog(); RepaintSkipRouting(); } }
void MainWindow::DefinerSetPinNumber(int i)	{ if ( GetCompDefiner().SetPinNumber(i)	) { UpdateHistory("Action"); EnableCompDialogControls(); RepaintSkipRouting(); } }
void MainWindow::DefinerIncPinNumber(bool b){ if ( GetCompDefiner().IncPinNumber(b)	) { UpdateHistory("Action"); UpdateCompDialog();		 RepaintSkipRouting(); } }	// Called using mouse wheel in view
void MainWindow::DefinerSetCX(double d)		{ if ( GetCompDefiner().SetCX(d)		) { UpdateHistory("Action"); EnableCompDialogControls(); RepaintSkipRouting(); } }
void MainWindow::DefinerSetCY(double d)		{ if ( GetCompDefiner().SetCY(-d)		) { UpdateHistory("Action"); EnableCompDialogControls(); RepaintSkipRouting(); } }	// Control assumes CY goes up
void MainWindow::DefinerSetDX(double d)		{ if ( GetCompDefiner().SetDX(d)		) { UpdateHistory("Action"); EnableCompDialogControls(); RepaintSkipRouting(); } }
void MainWindow::DefinerSetDY(double d)		{ if ( GetCompDefiner().SetDY(d)		) { UpdateHistory("Action"); EnableCompDialogControls(); RepaintSkipRouting(); } }
void MainWindow::DefinerSetA1(double d)		{ if ( GetCompDefiner().SetA1(d)		) { UpdateHistory("Action"); EnableCompDialogControls(); RepaintSkipRouting(); } }
void MainWindow::DefinerSetA2(double d)		{ if ( GetCompDefiner().SetA2(d)		) { UpdateHistory("Action"); EnableCompDialogControls(); RepaintSkipRouting(); } }
void MainWindow::DefinerSetA3(double d)		{ if ( GetCompDefiner().SetA3(d)		) { UpdateHistory("Action"); EnableCompDialogControls(); RepaintSkipRouting(); } }
void MainWindow::DefinerBuild()
{
	assert( GetCompDefiner().GetIsValid() );
	const Component comp( GetCompDefiner() );

	std::string errorStr;
	const bool bOK = GetTemplateManager().Add(false, comp, &errorStr);
	if ( !bOK )
		QMessageBox::warning(this, tr("Failed to add part to templates"), tr(errorStr.c_str()));
	if ( bOK )
		UpdateHistory("Build shape");
	EnableCompDialogControls();
	ShowTemplatesDialog();
}
void MainWindow::DefinerToggleEditor()
{
	m_board.SetCompEdit( !m_board.GetCompEdit() );
	if ( m_board.GetCompEdit() )
	{
		if ( m_board.GetGroupMgr().GetNumUserComps() == 1 )
		{
			const Component& comp = m_board.GetUserComponent();
			if ( !comp.GetShapes().empty() )
				GetCompDefiner().Populate( comp );
		}
		HidePinDialog();		// Hide pin dialog
		HideControlDialog();	// Hide control dialog
		UpdateCompDialog();		// Show component definition dialog
		ShowCompDialog();
		UpdateHistory("Open component editor");
	}
	else
	{
		HideCompDialog();		// Hide component definition dialog
		ShowControlDialog();	// Show control dialog
		UpdateHistory("Close component editor");
	}
	UpdateControls();
	if ( m_board.GetCompEdit() )
		RepaintSkipRouting();
	else
		RepaintWithRouting();
	activateWindow();			// Stop MS Windows showing the Menu greyed out
}
void MainWindow::DefinerAddLine()		{ const int id = GetCompDefiner().AddLine();		if ( id != BAD_ID ) { SetCurrentShapeId(id); UpdateCompDialog(); UpdateHistory("Add shape"); RepaintSkipRouting(); } }
void MainWindow::DefinerAddRect()		{ const int id = GetCompDefiner().AddRect();		if ( id != BAD_ID ) { SetCurrentShapeId(id); UpdateCompDialog(); UpdateHistory("Add shape"); RepaintSkipRouting(); } }
void MainWindow::DefinerAddRoundedRect(){ const int id = GetCompDefiner().AddRoundedRect();	if ( id != BAD_ID ) { SetCurrentShapeId(id); UpdateCompDialog(); UpdateHistory("Add shape"); RepaintSkipRouting(); } }
void MainWindow::DefinerAddEllipse()	{ const int id = GetCompDefiner().AddEllipse();		if ( id != BAD_ID ) { SetCurrentShapeId(id); UpdateCompDialog(); UpdateHistory("Add shape"); RepaintSkipRouting(); } }
void MainWindow::DefinerAddArc()		{ const int id = GetCompDefiner().AddArc();			if ( id != BAD_ID ) { SetCurrentShapeId(id); UpdateCompDialog(); UpdateHistory("Add shape"); RepaintSkipRouting(); } }
void MainWindow::DefinerAddChord()		{ const int id = GetCompDefiner().AddChord();		if ( id != BAD_ID ) { SetCurrentShapeId(id); UpdateCompDialog(); UpdateHistory("Add shape"); RepaintSkipRouting(); } }
void MainWindow::DefinerChooseColor()
{
	auto& def = GetCompDefiner();	assert( def.GetCurrentShapeId() != BAD_ID );
	if ( def.GetCurrentShapeId() == BAD_ID ) return;
	const MyRGB& rgb		= def.GetCurrentShape().GetFillColor();
	const QColor oldColor	= QColor(rgb.GetR(), rgb.GetG(), rgb.GetB());
	QColor		 newColor	= QColorDialog::getColor(oldColor, this );
	if ( newColor.isValid() && oldColor != newColor )
	{
		int r(0), g(0), b(0);
		newColor.getRgb(&r,&g,&b);
		if ( def.SetFillColor( MyRGB((r<<16) + (g<<8) + b) ) )
		{
			UpdateCompDialog(); UpdateHistory("Set fill color"); RepaintSkipRouting();
		}
	}
}
void MainWindow::DefinerRaise()
{
	auto& def = GetCompDefiner();	assert( def.GetCurrentShapeId() != BAD_ID );
	if ( def.Raise() )
	{
		UpdateCompDialog(); UpdateHistory("Raise"); RepaintSkipRouting();
	}
}
void MainWindow::DefinerLower()
{
	auto& def = GetCompDefiner();	assert( def.GetCurrentShapeId() != BAD_ID );
	if ( def.Lower() )
	{
		UpdateCompDialog(); UpdateHistory("Lower"); RepaintSkipRouting();
	}
}

// GUI update
void MainWindow::UpdateRecentFiles(const QString* pFileName, bool bAdd)
{
	QSettings	settings("veroroute","veroroute");	// Organisation = "veroroute", Application = "veroroute"
	QStringList	files = settings.value("recentFiles").toStringList();

	if ( pFileName )
	{
		const QString& fileName = *pFileName;
		files.removeAll(fileName);
		if ( bAdd )
			files.prepend(fileName);
		while ( files.size() > MAX_RECENT_FILES )
			files.removeLast();
		settings.setValue("recentFiles", files);
	}

	const int numFiles = std::min(files.size(), MAX_RECENT_FILES);
	for (int i = 0; i < numFiles; i++)
	{
		const QString&	fileName	= files[i];
		const QString	text		= tr("&%1 %2").arg( i + 1 ).arg( fileName );
		m_recentFileAction[i]->setText(text);
		m_recentFileAction[i]->setData(fileName);
		m_recentFileAction[i]->setVisible(true);
	}
	for (int i = numFiles; i < MAX_RECENT_FILES; i++)
		m_recentFileAction[i]->setVisible(false);

	ui->actionClearRecent->setEnabled(numFiles > 0);
	m_separator->setVisible(numFiles > 0);
}

void MainWindow::UpdateWindowTitle()
{
	const bool bCompEdit = m_board.GetCompEdit();	// true ==> Component Editor mode
	QString title = ( bCompEdit ) ? "Component Editor Mode" : ( m_fileName.isEmpty() ) ? "Untitled" : m_fileName;
	if ( !bCompEdit )
	{
		char buffer[256] = {'\0'};
		sprintf(buffer, "    (%d x %d)     (%gmm x %gmm)     Layer %d/%d", m_board.GetRows(), m_board.GetCols(), m_board.GetRows()*2.54, m_board.GetCols()*2.54, m_board.GetCurrentLayer() + 1, m_board.GetLyrs());
		title += buffer;
	}
	setWindowTitle(title);
}
void MainWindow::UpdateControls()
{
	GroupManager&	groupMgr		=  m_board.GetGroupMgr();
	CompManager&	compMgr			=  m_board.GetCompMgr();
	const bool		bCompEdit		=  m_board.GetCompEdit();
	const bool		bCompActionsOK	= !bCompEdit && !m_board.GetMirrored() && ( m_board.GetCompMode() != COMPSMODE::OFF );
	const bool		bTextActionsOK	= !bCompEdit && !m_board.GetMirrored() && ( m_board.GetShowText() );
	const bool		bTextOK			=  bTextActionsOK && GetCurrentTextId() != BAD_TEXTID;
	const bool		bCompOK			=  bCompActionsOK && groupMgr.GetNumUserComps();
	const bool		bMono			= !bCompEdit && m_board.GetTrackMode() == TRACKMODE::MONO;
	const bool		bPCB			= !bCompEdit && m_board.GetTrackMode() == TRACKMODE::PCB;
	const bool		bTracks			= !bCompEdit && m_board.GetTrackMode() != TRACKMODE::OFF;
	const bool		bVeroV			=  m_board.GetVeroTracks() &&  m_board.GetVerticalStrips();
	const bool		bVeroH			=  m_board.GetVeroTracks() && !m_board.GetVerticalStrips();
	const bool		bFat			= !m_board.GetVeroTracks() && !m_board.GetCurvedTracks() &&  m_board.GetFatTracks();
	const bool		bThin			= !m_board.GetVeroTracks() && !m_board.GetCurvedTracks() && !m_board.GetFatTracks();
	const bool		bCurved			= !m_board.GetVeroTracks() &&  m_board.GetCurvedTracks();

	ui->actionWrite_Gerber->setEnabled( bPCB && !bCompEdit && !m_board.GetMirrored() && !m_board.GetVeroTracks() && m_board.GetLyrs() == 1);
	ui->actionWrite_Gerber2->setEnabled(bPCB && !bCompEdit && !m_board.GetMirrored() && !m_board.GetVeroTracks());
	ui->actionMerge->setEnabled(    !bPCB && !bCompEdit);
	ui->actionWrite_PDF->setEnabled(!bPCB && !bCompEdit);
	ui->menuAdd->setEnabled( !bCompEdit && m_board.GetCompMode() != COMPSMODE::OFF && !m_board.GetMirrored() );
	ui->menuLayers->setEnabled( !bCompEdit );
	ui->actionAddLayer->setEnabled(		m_board.GetLyrs() == 1 );
	ui->actionRemoveLayer->setEnabled(	m_board.GetLyrs() != 1 );
	ui->actionToggleVias->setEnabled(	m_board.GetLyrs() != 1 );
	ui->actionSwitchLayer->setEnabled(	m_board.GetLyrs() != 1 );
	ui->actionToggleVias->setText(		m_board.GetLyrs() != 1 && m_board.GetViasEnabled() ? QString("Disable Vias") : QString("Enable Vias") );
	ui->actionSwitchLayer->setText(		m_board.GetCurrentLayer() == 0 ? QString("Switch to Top Layer") : QString("Switch to Bottom Layer") );
	ui->actionSwitchLayer->setIcon(		m_board.GetCurrentLayer() == 0 ? QIcon(":/images/layertop.png") : QIcon(":/images/layerbot.png"));
	m_labelStatus->setText(m_board.GetCurrentLayer() == 0 ? QString("   Layer = Bottom   ") : QString("   Layer = Top   "));

	ui->actionCopy->setEnabled( bTextOK || bCompOK );
	if ( bTextOK )	// Text Box takes precedence over comps
		ui->actionCopy->setText("Copy+Paste Selected Text Box");
	else if ( bCompOK )
		ui->actionCopy->setText("Copy+Paste Selected Part(s)");
	else
		ui->actionCopy->setText("Copy+Paste Selected Part(s) / Text Box");
	ui->actionGroup->setEnabled( bCompOK && groupMgr.CanGroup() );
	ui->actionUngroup->setEnabled( bCompOK && groupMgr.CanUnGroup() );
	ui->actionSelectAll->setEnabled( bCompActionsOK && !compMgr.GetMapIdToComp().empty() );
	ui->actionDelete->setEnabled( bTextOK || bCompOK );
	if ( bTextOK )	// Text Box takes precedence over comps
		ui->actionDelete->setText("Delete Selected Text Box");
	else if ( bCompOK )
		ui->actionDelete->setText("Delete Selected Part(s)");
	else
		ui->actionDelete->setText("Delete Selected Part(s) / Text Box");

	ui->actionCrop->setEnabled( !bCompEdit );
	ui->actionTextBox->setEnabled( !bCompEdit && m_board.GetShowText() );
	ui->actionZoom_In->setEnabled( CanZoomIn() );
	ui->actionZoom_Out->setEnabled( CanZoomOut() ) ;
	ui->actionToggleText->setEnabled(  !bCompEdit );
	ui->actionToggleFlipH->setEnabled( !bCompEdit );
	ui->actionToggleFlipV->setEnabled( !bCompEdit );
	ui->actionTogglePinLabels->setEnabled( !bCompEdit
											&& m_board.GetCompMode()  != COMPSMODE::OFF
											&& m_board.GetTrackMode() != TRACKMODE::MONO
											&& m_board.GetTrackMode() != TRACKMODE::PCB	);
	ui->actionToggleGrid->setChecked( m_board.GetShowGrid() );
	ui->actionToggleText->setChecked( m_board.GetShowText() );
	ui->actionToggleFlipH->setChecked( m_board.GetFlipH() );
	ui->actionToggleFlipV->setChecked( m_board.GetFlipV() );
	ui->actionTogglePinLabels->setChecked( m_board.GetShowPinLabels() );

	ui->actionPinDlg->setEnabled( !bCompEdit );
	ui->actionControlDlg->setEnabled( !bCompEdit );
	ui->actionCompDlg->setEnabled( bCompEdit );

	ui->actionVeroV->setEnabled(		bTracks );
	ui->actionVeroH->setEnabled(		bTracks );
	ui->actionFat->setEnabled(			bTracks );
	ui->actionThin->setEnabled(			bTracks );
	ui->actionCurved->setEnabled(		bTracks );
	ui->actionDiagsMin->setEnabled(		bTracks && !bVeroV && ! bVeroH );
	ui->actionDiagsMax->setEnabled(		bTracks && !bVeroV && ! bVeroH );
	ui->actionFill->setEnabled(			( bMono || bPCB ) && !m_board.GetVeroTracks() );
	ui->actionSelectArea->setEnabled(	!bCompEdit );

	ui->actionVeroV->setChecked( bVeroV );
	ui->actionVeroH->setChecked( bVeroH );
	ui->actionFat->setChecked( bFat );
	ui->actionThin->setChecked( bThin );
	ui->actionCurved->setChecked( bCurved );
	ui->actionDiagsMin->setChecked( m_board.GetDiagsMode() == DIAGSMODE::MIN );
	ui->actionDiagsMax->setChecked( m_board.GetDiagsMode() == DIAGSMODE::MAX );
	ui->actionFill->setChecked( m_board.GetGroundFill() );

	ui->actionSelectArea->setChecked( !bCompEdit && GetDefiningRect() );

	ui->actionEditor->setChecked(			bCompEdit );
	ui->actionAddLine->setEnabled(			bCompEdit );
	ui->actionAddRect->setEnabled(			bCompEdit );
	ui->actionAddRoundedRect->setEnabled(	bCompEdit );
	ui->actionAddEllipse->setEnabled(		bCompEdit );
	ui->actionAddArc->setEnabled(			bCompEdit );
	ui->actionAddChord->setEnabled(			bCompEdit );

	UpdateTextDialog(true);	// true ==> full

	m_pinDlg->Update();
	m_renderingDlg->UpdateControls();
	m_wireDlg->UpdateControls();
	m_controlDlg->UpdateCompControls();
	m_controlDlg->UpdateControls();

	UpdateUndoRedoControls();
}
void MainWindow::UpdateCompDialog()			{ m_compDlg->Update(); }
void MainWindow::EnableCompDialogControls()	{ m_compDlg->EnableControls(); }
void MainWindow::UpdateBOM()				{ m_bomDlg->Update(); }
void MainWindow::UpdateTemplatesDialog()	{ m_templatesDlg->Update(); }
void MainWindow::UpdateTextDialog(bool bFull)
{
	if ( GetCurrentTextId() != BAD_TEXTID )
		m_textDlg->Update( GetCurrentTextRect(), bFull );
	else
	{
		m_textDlg->Clear();
		m_textDlg->close();
	}
}

// Helpers
void MainWindow::SetQuality(QPainter& painter)
{
	switch( m_board.GetRenderQuality() )
	{
		case 0:		return painter.setRenderHint(QPainter::Antialiasing, false);
		case 1:		return painter.setRenderHint(QPainter::Antialiasing, true);
		default:	return painter.setRenderHint(QPainter::HighQualityAntialiasing, true);
	}
}
bool MainWindow::CanZoomIn() const	{ return m_board.GetGRIDPIXELS() < 64; }	// 64 == MAX_GRIDPIXELS
bool MainWindow::CanZoomOut() const	{ return m_board.GetGRIDPIXELS() > 6;  }	//  6 == MIN_GRIDPIXELS
bool MainWindow::GetIsModified() const
{
	if ( m_iTutorialNumber >= 0 ) return false;	// Skip check if in tutorial mode
	if ( !m_fileName.isEmpty() ) return !GetMatchesVrtFile( m_fileName.toStdString() );
	// If file doesn't exist, use the history instead
	if ( !m_bHistoryDir ) return true;				// No history directory, so assume modified by default so Save() works
	if ( m_historyMgr.GetCanUndo() ) return true;	// Something has changed since New() or Open() or Import()
	return m_infoDlg->GetIsModified();				// Info dialog changes are not tracked by the History
}
bool MainWindow::GetMatchesVrtFile(const std::string& fileName) const
{
	bool bIsSame(false);
	if ( !fileName.empty() )
	{
		DataStream inStream(DataStream::READ);
		if ( inStream.Open( fileName.c_str() ) )
		{
			Board tempBoard;
			tempBoard.Load(inStream);
			inStream.Close();
			bIsSame = inStream.GetOK() && ( m_board == tempBoard );
		}
	}
	return bIsSame;
}
void MainWindow::ResetHistory(const std::string& str)
{
	m_historyMgr.Reset(str, m_board);
	UpdateUndoRedoControls();
}
void MainWindow::UpdateHistory(const std::string& str, const int compId)
{
	if ( !m_bHistoryDir ) return;	// No History folder
	if ( GetMatchesVrtFile( m_historyMgr.GetCurrentFilename() ) ) return;	// No change
	m_historyMgr.Update(str, compId, m_board);
	UpdateUndoRedoControls();
}
void MainWindow::UpdateUndoRedoControls()
{
	const bool bCanUndo = m_bHistoryDir && m_historyMgr.GetCanUndo();
	const bool bCanRedo = m_bHistoryDir && m_historyMgr.GetCanRedo();
	QString undoText("Undo "); if ( bCanUndo ) undoText += QString::fromStdString(m_historyMgr.GetUndoText());
	QString redoText("Redo "); if ( bCanRedo ) redoText += QString::fromStdString(m_historyMgr.GetRedoText());
	ui->actionUndo->setText( undoText );
	ui->actionRedo->setText( redoText );
	ui->actionUndo->setEnabled( bCanUndo );
	ui->actionRedo->setEnabled( bCanRedo );
}
QString MainWindow::GetSaveFileName(const QString& caption, const QString& nameFilter, const QString& defaultSuffix)
{
	QString fileName;
	QFileDialog fileDialog(this, caption);
	fileDialog.setAcceptMode(QFileDialog::AcceptSave);
	fileDialog.setNameFilter(nameFilter);
	fileDialog.setDefaultSuffix(defaultSuffix);
	if ( fileDialog.exec() )
	{
		QStringList fileNames = fileDialog.selectedFiles();
		if ( !fileNames.isEmpty() ) fileName = fileNames.at(0);
	}
	return fileName;
}

QColor MainWindow::GetBackgroundColor() const
{
	return ( m_board.GetTrackMode() == TRACKMODE::PCB ) ? Qt::black : Qt::white;	// For screen only.  PDF is always white.
}
