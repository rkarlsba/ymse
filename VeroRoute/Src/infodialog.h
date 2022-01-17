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

#include <QDialog>

class MainWindow;
class Ui_InfoDialog;

class InfoDialog : public QDialog
{
	Q_OBJECT

public:
	explicit InfoDialog(MainWindow* parent = 0);
	~InfoDialog();

	void Update();
	bool GetIsModified();

public slots:
	void TextChanged();
	void SetReadOnly(bool b);
	void ShowButtons(bool b);
	void EnablePrev(bool b);
	void EnableNext(bool b);
protected:
	void keyPressEvent(QKeyEvent* event);
	void keyReleaseEvent(QKeyEvent* event);
private:
	Ui_InfoDialog*	ui;
	MainWindow*		m_pMainWindow;
	std::string		m_initialStr;	// Set by Update() when we do a New, Open, Save, SaveAs, Undo, Redo, etc
};
