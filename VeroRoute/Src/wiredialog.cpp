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

#include "wiredialog.h"
#include "ui_wiredialog.h"
#include "mainwindow.h"

WireDialog::WireDialog(MainWindow* parent)
: QDialog(parent)
, ui(new Ui_WireDialog)
, m_pMainWindow(parent)
{
	ui->setupUi(this);
	QObject::connect(ui->checkBox_share,	SIGNAL(toggled(bool)),	m_pMainWindow, SLOT(SetWireShare(bool)));
	QObject::connect(ui->checkBox_cross,	SIGNAL(toggled(bool)),	m_pMainWindow, SLOT(SetWireCross(bool)));
}

WireDialog::~WireDialog()
{
	delete ui;
}

void WireDialog::UpdateControls()
{
	Board& board = m_pMainWindow->m_board;
	ui->checkBox_share->setChecked( board.GetWireShare() );
	ui->checkBox_cross->setChecked( board.GetWireCross() );
}

void WireDialog::keyPressEvent(QKeyEvent* event)
{
	m_pMainWindow->specialKeyPressEvent(event);
	QDialog::keyPressEvent(event);
}

void WireDialog::keyReleaseEvent(QKeyEvent* event)
{
	m_pMainWindow->commonKeyReleaseEvent(event);
	QDialog::keyReleaseEvent(event);
}
