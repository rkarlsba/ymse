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

#include "hotkeysdialog.h"
#include "ui_hotkeysdialog.h"
#include "mainwindow.h"

HotkeysDialog::HotkeysDialog(MainWindow* parent)
: QDialog(parent)
, ui(new Ui_HotkeysDialog)
, m_pMainWindow(parent)
{
	ui->setupUi(this);

	Setup();
}

HotkeysDialog::~HotkeysDialog()
{
	delete ui;
}

void HotkeysDialog::Setup()
{
	typedef std::pair<std::string, std::string> KEY_ACTION ;
	std::list<KEY_ACTION> list;
	list.push_back( KEY_ACTION("Ctrl + click-drag mouse",		"Move whole circuit") );
	list.push_back( KEY_ACTION("R + click-drag mouse",			"Select parts/tracks by area") );
	list.push_back( KEY_ACTION("Ctrl+A",						"Select all parts") );
	list.push_back( KEY_ACTION("Shift + mouse button",			"Add/Remove to selected parts") );
	list.push_back( KEY_ACTION("Z",								"Rotate selected parts counter-clockwise") );
	list.push_back( KEY_ACTION("X",								"Rotate selected parts clockwise") );
	list.push_back( KEY_ACTION("V",								"Copy and paste selected parts") );
	list.push_back( KEY_ACTION("G",								"Group selected parts") );
	list.push_back( KEY_ACTION("U",								"Un-group selected parts") );
	list.push_back( KEY_ACTION("W",								"Wipe all tracks") );
	list.push_back( KEY_ACTION("L",								"Switch layer (Top/Bottom)") );
	list.push_back( KEY_ACTION("P + left mouse button",			"Paint pin") );
	list.push_back( KEY_ACTION("P + right mouse button",		"Un-paint pin") );
	list.push_back( KEY_ACTION("SPACE + left mouse button",		"Paint track (excluding pins)") );
	list.push_back( KEY_ACTION("SPACE + right mouse button",	"Un-paint track (excluding pins)") );
	list.push_back( KEY_ACTION("F + left mouse button",			"Paint with flood-fill (all connected tracks + pins)") );

	const int numRows = (int) ( list.size() );
	// Set up the table
	ui->tableWidget->clear();
	ui->tableWidget->setRowCount(numRows);
	ui->tableWidget->setColumnCount(2);
	ui->tableWidget->setColumnWidth(0,259);
	ui->tableWidget->setColumnWidth(1,400);
	m_tableHeader << "Action" << "Behaviour";
	ui->tableWidget->setHorizontalHeaderLabels(m_tableHeader);
	ui->tableWidget->verticalHeader()->setVisible(false);
	ui->tableWidget->setEditTriggers(QAbstractItemView::AllEditTriggers);
	ui->tableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);
	ui->tableWidget->setSelectionMode(QAbstractItemView::NoSelection);
	ui->tableWidget->setShowGrid(true);

	// Populate the table with data
	int iRow(0);
	for (auto& o : list)
	{
		auto pItem0 = new QTableWidgetItem(QString::fromStdString( o.first ));
		pItem0->setFlags(Qt::NoItemFlags);
		ui->tableWidget->setItem(iRow, 0, pItem0);

		auto pItem1 = new QTableWidgetItem(QString::fromStdString( o.second ));
		pItem1->setFlags(Qt::NoItemFlags);
		ui->tableWidget->setItem(iRow, 1, pItem1);

		iRow++;
	}
}

void HotkeysDialog::keyPressEvent(QKeyEvent* event)
{
	m_pMainWindow->specialKeyPressEvent(event);
	QDialog::keyPressEvent(event);
}

void HotkeysDialog::keyReleaseEvent(QKeyEvent* event)
{
	m_pMainWindow->commonKeyReleaseEvent(event);
	QDialog::keyReleaseEvent(event);
}
