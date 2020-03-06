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

#include "renderingdialog.h"
#include "ui_renderingdialog.h"
#include "mainwindow.h"

RenderingDialog::RenderingDialog(MainWindow* parent)
: QDialog(parent)
, ui(new Ui_RenderingDialog)
, m_pMainWindow(parent)
{
	ui->setupUi(this);

	ui->antiAliasOn->setChecked(true);
	QObject::connect(ui->antiAliasOff,	SIGNAL(toggled(bool)),		m_pMainWindow, SLOT(SetAntialiasOff(bool)));
	QObject::connect(ui->antiAliasOn,	SIGNAL(toggled(bool)),		m_pMainWindow, SLOT(SetAntialiasOn(bool)));
	QObject::connect(ui->antiAliasHigh,	SIGNAL(toggled(bool)),		m_pMainWindow, SLOT(SetAntialiasHigh(bool)));
	QObject::connect(ui->checkBox,		SIGNAL(toggled(bool)),		m_pMainWindow, SLOT(SetShowTarget(bool)));
	QObject::connect(ui->spinBox,		SIGNAL(valueChanged(int)),	m_pMainWindow, SLOT(SetTextSizeComp(int)));
	QObject::connect(ui->spinBox_2,		SIGNAL(valueChanged(int)),	m_pMainWindow, SLOT(SetTextSizePins(int)));
	QObject::connect(ui->spinBox_Height,SIGNAL(valueChanged(int)),	m_pMainWindow, SLOT(SetTargetRows(int)));
	QObject::connect(ui->spinBox_Width,	SIGNAL(valueChanged(int)),	m_pMainWindow, SLOT(SetTargetCols(int)));
	QObject::connect(ui->padWidth,		SIGNAL(valueChanged(int)),	m_pMainWindow, SLOT(SetPadWidth(int)));
	QObject::connect(ui->trackWidth,	SIGNAL(valueChanged(int)),	m_pMainWindow, SLOT(SetTrackWidth(int)));
	QObject::connect(ui->holeWidth,		SIGNAL(valueChanged(int)),	m_pMainWindow, SLOT(SetHoleWidth(int)));
	QObject::connect(ui->gapWidth,		SIGNAL(valueChanged(int)),	m_pMainWindow, SLOT(SetGapWidth(int)));
	QObject::connect(ui->maskWidth,		SIGNAL(valueChanged(int)),	m_pMainWindow, SLOT(SetMaskWidth(int)));
	QObject::connect(ui->silkWidth,		SIGNAL(valueChanged(int)),	m_pMainWindow, SLOT(SetSilkWidth(int)));
	QObject::connect(ui->edgeWidth,		SIGNAL(valueChanged(int)),	m_pMainWindow, SLOT(SetEdgeWidth(int)));
	QObject::connect(ui->viapadWidth,	SIGNAL(valueChanged(int)),	m_pMainWindow, SLOT(SetViaPadWidth(int)));
	QObject::connect(ui->viaholeWidth,	SIGNAL(valueChanged(int)),	m_pMainWindow, SLOT(SetViaHoleWidth(int)));
}

RenderingDialog::~RenderingDialog()
{
	delete ui;
}

void RenderingDialog::UpdateControls()
{
	Board& board = m_pMainWindow->m_board;

	ui->spinBox->setValue( board.GetTextSizeComp() );
	ui->spinBox_2->setValue( board.GetTextSizePins() );
	ui->spinBox_Height->setValue( board.GetTargetRows() );
	ui->spinBox_Width->setValue( board.GetTargetCols() );
	ui->checkBox->setChecked( board.GetShowTarget() );
	switch( board.GetRenderQuality() )
	{
		case 0:		ui->antiAliasOff->setChecked(true);		break;
		case 1:		ui->antiAliasOn->setChecked(true);		break;
		case 2:		ui->antiAliasHigh->setChecked(true);	break;
		default:	ui->antiAliasOn->setChecked(true);		break;
	}

	const bool bCompEdit		= board.GetCompEdit();
	const bool bPCB				= board.GetTrackMode() == TRACKMODE::PCB;
	const bool bMonoPCB			= board.GetTrackMode() == TRACKMODE::MONO || bPCB;
	const bool bNoTrackOptions	= board.GetTrackMode() == TRACKMODE::OFF;
	const bool bGndFill			= board.GetGroundFill();
	const bool bVero			= board.GetVeroTracks();
	const bool bVias			= board.GetViasEnabled();

	ui->padWidth->setDisabled(		bCompEdit || bNoTrackOptions || bVero );
	ui->trackWidth->setDisabled(	bCompEdit || bNoTrackOptions || bVero );
	ui->holeWidth->setDisabled(		bCompEdit || bNoTrackOptions || bVero );
	ui->gapWidth->setDisabled(		bCompEdit || bVero || !bMonoPCB || !bGndFill );
	ui->groupBox_pcb->setDisabled(	bCompEdit || bVero || !bPCB );
	ui->viapadWidth->setDisabled(	bCompEdit || bVero || !bPCB || !bVias);
	ui->viaholeWidth->setDisabled(	bCompEdit || bVero || !bPCB || !bVias);

	// ... and corresponding labels
	ui->label_pad->setDisabled(		bCompEdit || bNoTrackOptions || bVero );
	ui->label_track->setDisabled(	bCompEdit || bNoTrackOptions || bVero );
	ui->label_hole->setDisabled(	bCompEdit || bNoTrackOptions || bVero );
	ui->label_gap->setDisabled(		bCompEdit || bVero || !bMonoPCB || !bGndFill );
	ui->label_viapad->setDisabled(	bCompEdit || bVero || !bPCB || !bVias);
	ui->label_viahole->setDisabled(	bCompEdit || bVero || !bPCB || !bVias);

	ui->padWidth->setValue(		board.GetPAD_PERCENT()		);
	ui->trackWidth->setValue(	board.GetTRACK_PERCENT()	);
	ui->holeWidth->setValue(	board.GetHOLE_PERCENT()		);
	ui->gapWidth->setValue(		board.GetGAP_PERCENT()		);
	ui->maskWidth->setValue(	board.GetMASK_PERCENT()		);
	ui->silkWidth->setValue(	board.GetSILK_PERCENT()		);
	ui->edgeWidth->setValue(	board.GetEDGE_PERCENT()		);
	ui->viapadWidth->setValue(	board.GetVIAPAD_PERCENT()	);
	ui->viaholeWidth->setValue(	board.GetVIAHOLE_PERCENT()	);

	const int minTrackSep = board.GetMIN_TRACK_SEPARATION_PERCENT();
	const std::string str = "Guaranteed minimum track separation = " + std::to_string(minTrackSep) + " mil";
	ui->label_info->setText( QString::fromStdString(str) );
}

void RenderingDialog::keyPressEvent(QKeyEvent* event)
{
	m_pMainWindow->specialKeyPressEvent(event);
	QDialog::keyPressEvent(event);
}

void RenderingDialog::keyReleaseEvent(QKeyEvent* event)
{
	m_pMainWindow->commonKeyReleaseEvent(event);
	QDialog::keyReleaseEvent(event);
}
