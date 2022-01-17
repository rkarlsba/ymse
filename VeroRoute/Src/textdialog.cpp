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

#include "textdialog.h"
#include "ui_textdialog.h"
#include "mainwindow.h"

TextDialog::TextDialog(MainWindow* parent)
: QDialog(parent)
, ui(new Ui::TextDialog)
, m_pMainWindow(parent)
{
	ui->setupUi(this);
	QObject::connect(ui->spinBox,		SIGNAL(valueChanged(int)),	m_pMainWindow,	SLOT(SizeChanged(int)));
	QObject::connect(ui->pushButtonB,	SIGNAL(clicked()),			m_pMainWindow,	SLOT(ToggleBold()));
	QObject::connect(ui->pushButtonI,	SIGNAL(clicked()),			m_pMainWindow,	SLOT(ToggleItalic()));
	QObject::connect(ui->pushButtonU,	SIGNAL(clicked()),			m_pMainWindow,	SLOT(ToggleUnderline()));
	QObject::connect(ui->pushButtonRGB,	SIGNAL(clicked()),			m_pMainWindow,	SLOT(ChooseTextColor()));
	QObject::connect(ui->pushButtonL,	SIGNAL(clicked()),			m_pMainWindow,	SLOT(AlignL()));
	QObject::connect(ui->pushButtonC,	SIGNAL(clicked()),			m_pMainWindow,	SLOT(AlignC()));
	QObject::connect(ui->pushButtonR,	SIGNAL(clicked()),			m_pMainWindow,	SLOT(AlignR()));
	QObject::connect(ui->pushButtonJ,	SIGNAL(clicked()),			m_pMainWindow,	SLOT(AlignJ()));
	QObject::connect(ui->plainTextEdit,	SIGNAL(textChanged()),		this,			SLOT(TextChanged()));
}

TextDialog::~TextDialog()
{
	delete ui;
}

void TextDialog::Clear()
{
	ui->plainTextEdit->setPlainText( QString("") );
}

void TextDialog::Update(const TextRect& rect, bool bFull)
{
	ui->spinBox->setValue( rect.GetSize() );
	ui->pushButtonB->setChecked(rect.GetStyle() & TEXT_BOLD);
	ui->pushButtonI->setChecked(rect.GetStyle() & TEXT_ITALIC);
	ui->pushButtonU->setChecked(rect.GetStyle() & TEXT_UNDERLINE);
	ui->pushButtonL->setChecked(rect.GetFlags() == Qt::AlignLeft);
	ui->pushButtonC->setChecked(rect.GetFlags() == Qt::AlignHCenter);
	ui->pushButtonR->setChecked(rect.GetFlags() == Qt::AlignRight);
	ui->pushButtonJ->setChecked(rect.GetFlags() == Qt::AlignJustify);

	QPalette pal = ui->pushButtonRGB->palette();
	pal.setColor(QPalette::Button, QColor(rect.GetR(),rect.GetG(),rect.GetB()));
	ui->pushButtonRGB->setAutoFillBackground(true);
	ui->pushButtonRGB->setPalette(pal);
	ui->pushButtonRGB->update();

	if ( bFull )
		ui->plainTextEdit->setPlainText( QString::fromStdString(rect.GetStr()) );
}

void TextDialog::TextChanged()
{
	m_pMainWindow->SetText( ui->plainTextEdit->toPlainText() );
}

void TextDialog::keyPressEvent(QKeyEvent* event)
{
	m_pMainWindow->specialKeyPressEvent(event);
	QDialog::keyPressEvent(event);
}

void TextDialog::keyReleaseEvent(QKeyEvent* event)
{
	m_pMainWindow->commonKeyReleaseEvent(event);
	QDialog::keyReleaseEvent(event);
}
