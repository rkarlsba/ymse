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

#include "templatesdialog.h"
#include "ui_templatesdialog.h"
#include "mainwindow.h"
#include "TemplateManager.h"

TemplatesDialog::TemplatesDialog(MainWindow* parent)
: QDialog(parent)
, ui(new Ui_TemplatesDialog)
, m_pMainWindow(parent)
, m_iRowL(-1)
, m_iRowR(-1)
{
	ui->setupUi(this);
	QObject::connect(ui->tableWidget,	SIGNAL(cellClicked(int,int)),		this,	SLOT(GenericClicked(int,int)));
	QObject::connect(ui->tableWidget,	SIGNAL(cellDoubleClicked(int,int)),	this,	SLOT(GenericDoubleClicked(int,int)));
	QObject::connect(ui->tableWidget_2,	SIGNAL(cellClicked(int,int)),		this,	SLOT(UserClicked(int,int)));
	QObject::connect(ui->tableWidget_2,	SIGNAL(cellDoubleClicked(int,int)),	this,	SLOT(UserDoubleClicked(int,int)));
	QObject::connect(ui->pushButton,	SIGNAL(clicked()),					this,	SLOT(AddTemplates()));
	QObject::connect(ui->pushButton_2,	SIGNAL(clicked()),					this,	SLOT(DeleteTemplate()));
	QObject::connect(ui->pushButton_3,	SIGNAL(clicked()),					this,	SLOT(LoadFromVrt()));
	LoadFromUserVrt(false);	// false ==> no message box
}

TemplatesDialog::~TemplatesDialog()
{
	delete ui;
}

void TemplatesDialog::Update()
{
	TemplateManager& mgr = m_pMainWindow->GetTemplateManager();

	for (int iTable = 0; iTable < 2; iTable++)
	{
		const bool bGeneric = ( iTable == 0 );

		const int numRows = (int)mgr.GetSize(bGeneric);

		auto* pTableWidget	= ( bGeneric ) ? ui->tableWidget : ui->tableWidget_2;
		auto& tableHeader	= ( bGeneric ) ? m_tableHeaderL : m_tableHeaderR;
		auto& iRow			= ( bGeneric ) ? m_iRowL : m_iRowR;

		tableHeader.clear();
		if ( bGeneric )
			tableHeader << "Type";
		else
			tableHeader << "Type" << "Value";

		// Set up the table
		pTableWidget->clear();
		pTableWidget->setRowCount(numRows);
		pTableWidget->setColumnCount(tableHeader.size());
		for (int i = 0, iSize = tableHeader.size(); i < iSize; i++)
		{
			int iWidth(0);
			switch( i )
			{
				case 0: iWidth = bGeneric ? 220 : 160; break;
				case 1: iWidth = (numRows > 20) ? 160 : 180; // Small reduction when have a vertical scroll bar
			}
			pTableWidget->setColumnWidth(i,iWidth);
		}
		pTableWidget->setHorizontalHeaderLabels(tableHeader);
		pTableWidget->verticalHeader()->setVisible(false);
		pTableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
		pTableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);
		pTableWidget->setSelectionMode(QAbstractItemView::SingleSelection);
		pTableWidget->setShowGrid(true);

		// Populate the table with data
		std::string strType(""), strValue(""), strFull("");
		for (int i = 0; i < numRows; i++)
		{
			const Component&	comp	= mgr.GetNth(bGeneric, i);
			const COMP&			eType	= comp.GetType();

			strType		= comp.GetFullTypeStr();	// e.g. "Film" or "DIP16"
			strValue	= comp.GetValueStr();
			strFull		= GetFamilyStr( eType );	// e.g. "Capacitor"
			if ( !StringHelper::IsEmptyStr(strFull) ) strFull += ": ";
			strFull	+= strType;						// e.g. "Capacitor: Film"

			// Write current row to table.	Note: No memory leak since setItem() takes ownership.
			if ( bGeneric )
			{
				pTableWidget->setItem(i, 0, new QTableWidgetItem(QString::fromStdString(strFull)));
			}
			else
			{
				pTableWidget->setItem(i, 0, new QTableWidgetItem(QString::fromStdString(strType)));
				pTableWidget->setItem(i, 1, new QTableWidgetItem(QString::fromStdString(strValue)));
			}
		}
		iRow = ( numRows > 0 ) ? 0 : -1;
		if ( iRow >= 0 ) pTableWidget->selectRow(iRow);
		if ( !bGeneric )
			ui->pushButton_2->setDisabled(m_iRowR == -1);
	}
	SaveToUserVrt();
}

void TemplatesDialog::GenericClicked(int row, int)
{
	m_iRowL = row;
	ui->tableWidget->selectRow(m_iRowL);
}

void TemplatesDialog::GenericDoubleClicked(int row, int col)
{
	GenericClicked(row, col);

	TemplateManager& mgr = m_pMainWindow->GetTemplateManager();

	const bool bGeneric = true;
	if ( m_iRowL >= 0 && m_iRowL < (int)mgr.GetSize(bGeneric) )
		m_pMainWindow->AddFromTemplate(mgr.GetNth(bGeneric, m_iRowL));
}

void TemplatesDialog::UserClicked(int row, int)
{
	m_iRowR = row;
	ui->tableWidget_2->selectRow(m_iRowR);
}

void TemplatesDialog::UserDoubleClicked(int row, int col)
{
	UserClicked(row, col);

	TemplateManager& mgr = m_pMainWindow->GetTemplateManager();

	const bool bGeneric = false;
	if ( m_iRowR >= 0 && m_iRowR < (int)mgr.GetSize(bGeneric) )
		m_pMainWindow->AddFromTemplate(mgr.GetNth(bGeneric, m_iRowR));
}

void TemplatesDialog::AddTemplates()
{
	AddTemplatesFromBoard(m_pMainWindow->m_board, false, true);	// false ==> Restrict to user-group
}

void TemplatesDialog::DeleteTemplate()
{
	TemplateManager& mgr = m_pMainWindow->GetTemplateManager();

	const bool bGeneric = false;

	if ( m_iRowR >= 0 && m_iRowR < (int)mgr.GetSize(bGeneric) )
	{
		if ( QMessageBox::question(this, tr("Confirm Delete Template"),
										 tr("There is no undo for this operation.  Continue?"),
										 QMessageBox::Yes | QMessageBox::No, QMessageBox::No) == QMessageBox::No ) return;
		const Component comp = mgr.GetNth(bGeneric, m_iRowR);
		if ( mgr.Remove(comp) )
			Update();
	}
}

const QString TemplatesDialog::GetUserFilename() const
{
	TemplateManager& mgr = m_pMainWindow->GetTemplateManager();
	char buffer[256] = {'\0'};
	sprintf(buffer, "%s/templates/user.vrt", mgr.GetPathStr().c_str());
	return QString(buffer);
}

void TemplatesDialog::LoadFromVrt()
{
	QString fileName = QFileDialog::getOpenFileName(this, tr("Open file"), ""/*directory*/,	tr("VeroRoute (*.vrt);;All Files (*)"));
	if ( !fileName.isEmpty() )
		Load(fileName, true);
}

void TemplatesDialog::LoadFromUserVrt(bool bInfoMsg)
{
	QString fileName = GetUserFilename();
	if ( !fileName.isEmpty() )
		Load(fileName, bInfoMsg);
}

void TemplatesDialog::Load(QString& fileName, bool bInfoMsg)
{
	const std::string fileNameStr = fileName.toStdString();

	DataStream inStream(DataStream::READ);
	if ( inStream.Open( fileNameStr.c_str() ) )
	{
		Board tmp;	// Load using a temporary board in case there is a problem with the file
		tmp.Load(inStream);
		inStream.Close();
		if ( inStream.GetOK() ) // If it loaded OK ...
			AddTemplatesFromBoard(tmp, true, bInfoMsg);	// true ==> Don't restrict to user group
		else
			QMessageBox::information(this, tr("Unsupported VRT version"), tr(fileNameStr.c_str()));
	}
	else if ( fileName != GetUserFilename() )
		QMessageBox::information(this, tr("Unable to open file"), tr(fileNameStr.c_str()));
}

void TemplatesDialog::SaveToUserVrt()
{
	if ( !m_pMainWindow->m_bTemplatesDir ) return;

	QString fileName = GetUserFilename();
	if ( !fileName.isEmpty() )
	{
		DataStream outStream(DataStream::WRITE);
		const std::string fileNameStr = fileName.toStdString();
		if ( outStream.Open( fileNameStr.c_str() ) )
		{
			Board tmp;	// Load using a temporary board in case there is a problem with the file

			const bool bGeneric = false;
			TemplateManager& mgr = m_pMainWindow->GetTemplateManager();
			for (size_t i = 0, iSize = mgr.GetSize(bGeneric); i < iSize; i++)
			{
				const Component& comp = mgr.GetNth(bGeneric,i);
				tmp.AddComponent(-1, -1, comp, false);
			}
			tmp.Save(outStream);
			outStream.Close();
		}
		else
			QMessageBox::information(this, tr("Unable to save file"), tr(fileNameStr.c_str()));
	}
}

void TemplatesDialog::AddTemplatesFromBoard(Board& board, bool bAllComps, bool bInfoMsg)
{
	CompManager&	 compMgr	= board.GetCompMgr();
	GroupManager&	 groupMgr	= board.GetGroupMgr();
	TemplateManager& mgr		= m_pMainWindow->GetTemplateManager();

	if ( !bAllComps && groupMgr.GetNumUserComps() == 0 && bInfoMsg )
	{
		char buffer[64] = {'\0'};
		sprintf(buffer, "No parts are currently selected in the main view.");
		QMessageBox::information(this, "Information", tr(buffer));
		return;
	}
	
	const bool bGeneric = false;

	int nCount(0);
	for (const auto& mapObj : compMgr.GetMapIdToComp())
	{
		if ( bAllComps || groupMgr.GetIsUserComp(mapObj.first) )
			if ( mgr.Add(bGeneric, mapObj.second) ) nCount++;
	}
	if ( nCount > 0 ) Update();

	if ( bInfoMsg )
	{
		char buffer[64] = {'\0'};
		sprintf(buffer, "%d new templates added.", nCount);
		QMessageBox::information(this, "Information", tr(buffer));
	}
}

void TemplatesDialog::keyPressEvent(QKeyEvent* event)
{
	m_pMainWindow->specialKeyPressEvent(event);
	QDialog::keyPressEvent(event);
}

void TemplatesDialog::keyReleaseEvent(QKeyEvent* event)
{
	m_pMainWindow->commonKeyReleaseEvent(event);
	QDialog::keyReleaseEvent(event);
}
