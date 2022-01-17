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

#include "bomdialog.h"
#include "ui_bomdialog.h"
#include "mainwindow.h"

BomDialog::BomDialog(MainWindow* parent)
: QDialog(parent)
, ui(new Ui_BomDialog)
, m_pMainWindow(parent)
{
	ui->setupUi(this);

	QObject::connect(ui->pushButton,	SIGNAL(clicked()),	this,	SLOT(WriteToFile()));
}

BomDialog::~BomDialog()
{
	delete ui;
}

struct IsEarlierInBOM
{
	bool operator()(const Component* pA, const Component* pB) const
	{
		// First order by type ...
		if ( pA->GetType() != pB->GetType() ) return (int)pA->GetType() < (int)pB->GetType();
		// ... then by value string comparison
		const int i = pA->GetValueStr().compare( pB->GetValueStr() );
		if ( i != 0 ) return i < 0;
		// ... then by name length
		if ( pA->GetNameStr().length() != pB->GetNameStr().length() ) return pA->GetNameStr().length() < pB->GetNameStr().length();
		// ... then by name string comparison
		return pA->GetNameStr().compare( pB->GetNameStr() ) < 0;
	}
};

void BomDialog::Update()
{
	std::vector<const Component*> pComps;	//  Make list of pointers to all components for the B.O.M.
	for (const auto& mapObj : m_pMainWindow->m_board.GetCompMgr().GetMapIdToComp())
	{
		const Component& comp = mapObj.second;
		switch( comp.GetType() )
		{
			case COMP::MARK:
			case COMP::PAD:
			case COMP::WIRE:	break;	// Not true components
			default:			pComps.push_back(&comp);
		}
	}
	std::stable_sort(pComps.begin(), pComps.end(), IsEarlierInBOM());	// Sort the list appropriately

	// Count the number of values in the list.  This determines the number of rows in the table
	int numValues(0);
	std::string currentValue("");
	for (auto& p : pComps) if ( p->GetValueStr() != currentValue ) { currentValue = p->GetValueStr(); numValues++; }

	// Set up the table
	ui->tableWidget->clear();
	ui->tableWidget->setRowCount(numValues);
	ui->tableWidget->setColumnCount(4);
	ui->tableWidget->setColumnWidth(0,130);
	ui->tableWidget->setColumnWidth(1,250);
	ui->tableWidget->setColumnWidth(2,100);
	ui->tableWidget->setColumnWidth(3, (numValues > 9) ? 80 :100); // Small reduction when have a vertical scroll bar
	m_tableHeader << "Name" << "Type" << "Value" << "Quantity";
	ui->tableWidget->setHorizontalHeaderLabels(m_tableHeader);
	ui->tableWidget->verticalHeader()->setVisible(false);
	ui->tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
	ui->tableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);
	ui->tableWidget->setSelectionMode(QAbstractItemView::NoSelection);
	ui->tableWidget->setShowGrid(true);

	// Populate the table with data
	std::string rowNames(""), rowTypes(""), rowValue("");
	int row(-1), rowQuantity(0);
	char buffer[256] = {'\0'};
	for (auto& p : pComps)
	{
		const bool bLast		= ( p == pComps.back() );
		const bool bNewValue	= ( p->GetValueStr() != rowValue );
		if ( bNewValue )
		{
			if ( row != -1 )	// If have a previous row ...
			{
				sprintf(buffer,"%d",rowQuantity);
				// Write previous row to table.	Note: No memory leak since setItem() takes ownership.
				ui->tableWidget->setItem(row, 0, new QTableWidgetItem(QString::fromStdString(rowNames)));
				ui->tableWidget->setItem(row, 1, new QTableWidgetItem(QString::fromStdString(rowTypes)));
				ui->tableWidget->setItem(row, 2, new QTableWidgetItem(QString::fromStdString(rowValue)));
				ui->tableWidget->setItem(row, 3, new QTableWidgetItem(QString(buffer)));
			}
			// Initialise data for new row
			row++;
			rowQuantity = 1;
			rowNames = p->GetNameStr();
			rowTypes = GetFamilyStr( p->GetType() );
			if ( !StringHelper::IsEmptyStr(rowTypes) ) rowTypes += ": ";
			rowTypes += p->GetTypeStr();
			rowValue = p->GetValueStr();
			if ( p->GetType() == COMP::DIP || p->GetType() == COMP::SIP )
			{
				sprintf(buffer, "%d", (int)p->GetNumPins());
				rowTypes += std::string(buffer);	// e.g. "DIP16"
			}
			if ( p->GetType() == COMP::STRIP_100 || p->GetType() == COMP::BLOCK_100 || p->GetType() == COMP::BLOCK_200 )
			{
				sprintf(buffer, " (%d pins)", (int)p->GetNumPins());
				rowTypes += std::string(buffer);
			}
		}
		else
		{
			// Update data for row
			rowQuantity++;
			rowNames = rowNames + ", " + p->GetNameStr();
		}
		if ( bLast )	// Very last component in the B.O.M.
		{
			sprintf(buffer,"%d",rowQuantity);
			// Write current row to table.	Note: No memory leak since setItem() takes ownership.
			ui->tableWidget->setItem(row, 0, new QTableWidgetItem(QString::fromStdString(rowNames)));
			ui->tableWidget->setItem(row, 1, new QTableWidgetItem(QString::fromStdString(rowTypes)));
			ui->tableWidget->setItem(row, 2, new QTableWidgetItem(QString::fromStdString(rowValue)));
			ui->tableWidget->setItem(row, 3, new QTableWidgetItem(QString(buffer)));
		}
	}
	ui->pushButton->setDisabled( pComps.empty() );
}

void BomDialog::WriteToFile()
{
	QString fileName = QFileDialog::getSaveFileName(this, tr("Save file as"), ""/*directory*/,	tr("Text (*.txt);;All Files (*)"));
	if ( !fileName.isEmpty() )
	{
		const std::string fileNameStr = fileName.toStdString();

		std::ofstream outStream;
		outStream.open(fileNameStr.c_str(), std::ios::out);
		const bool bOK = outStream.is_open();
		if ( bOK )
		{
			outStream << "Name" << "\t" << "Type" << "\t" << "Value" << "\t" << "Quantity" << std::endl;
			const int numRows = ui->tableWidget->rowCount();
			const int numCols = ui->tableWidget->columnCount();
			for (int i = 0; i < numRows; i++)
			{
				for(int j = 0; j < numCols; j++)
				{
					outStream << ui->tableWidget->item(i,j)->text().toStdString();
					if ( j != numCols - 1 ) outStream << "\t";
				}
				outStream << std::endl;
			}
			outStream << std::endl;
			outStream.close();
		}
		else
			QMessageBox::information(this, tr("Unable to save file"), tr(fileNameStr.c_str()));
	}
}

void BomDialog::keyPressEvent(QKeyEvent* event)
{
	m_pMainWindow->specialKeyPressEvent(event);
	QDialog::keyPressEvent(event);
}

void BomDialog::keyReleaseEvent(QKeyEvent* event)
{
	m_pMainWindow->commonKeyReleaseEvent(event);
	QDialog::keyReleaseEvent(event);
}
