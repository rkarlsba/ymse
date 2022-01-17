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

#include <QtGui>
#include <QScrollArea>
#include <QScrollBar>

// Wrapper to send mouse wheel and key press events to the mainwindow instead of the scroll bars

class MyScrollArea : public QScrollArea
{
	Q_OBJECT

public:
	MyScrollArea(QWidget* parent = 0) : QScrollArea(parent), m_parent(parent) {}
	~MyScrollArea() {}

protected:
	void wheelEvent(QWheelEvent* event)
	{
		if ( m_parent ) QCoreApplication::sendEvent(m_parent, event);
	}
	void keyPressEvent(QKeyEvent* event)
	{
		if ( m_parent ) QCoreApplication::sendEvent(m_parent, event);
	}
	void keyReleaseEvent(QKeyEvent* event )
	{
		if ( m_parent ) QCoreApplication::sendEvent(m_parent, event);
	}
private:
	QWidget* m_parent;
};
