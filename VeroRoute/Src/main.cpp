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

#include "mainwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
	QString	pathStr(".");	// By default, take the home directory to be the current one

	// See if any command line arguments contain the "-p" instruction
	int iArgP(0);	// The index of the argument containing the "-p" instruction
	for (int i = 1; i < argc-1; i++)	// The "-p" can't come last
	{
		const QString str( argv[i] );
		if ( str == "-p" )
		{
			iArgP	= i;
			pathStr	= argv[i+1];
			break;
		}
	}

	QCoreApplication::addLibraryPath(pathStr);	// Search "platforms" sub-directory for plugin DLLs
	QApplication a(argc, argv);

	QString appDataPathStr = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
	// appDataPathStr on Linux   = "~/.local/share/veroroute"
	// appDataPathStr on Windows = "C:/Users/<USER>/AppData/Roaming/veroroute"

	// If the folder doesn't exist then create it
	QDir dir(appDataPathStr);
	if ( !dir.exists() ) dir.mkpath(".");

	// If the "history" folder doesn't exist then create it
	QDir historyDir(appDataPathStr + QString("/history"));
	if ( !historyDir.exists() ) historyDir.mkpath(".");

	// If the "templates" folder doesn't exist then create it
	QDir templatesDir(appDataPathStr + QString("/templates"));
	if ( !templatesDir.exists() ) templatesDir.mkpath(".");

	// Fallback "tutorials" path should be in same folder as the exe (until distribution method for Windows changes)
	QString tutorialsPathStr = pathStr;

	// Search for relative "tutorials" path assuming the binary is installed in usr/bin
	QString relativeTutorialsPathStr = ("../share/veroroute");
	QDir tutorialsDir(relativeTutorialsPathStr + QString("/tutorials"));
	if ( tutorialsDir.exists() )
		tutorialsPathStr = relativeTutorialsPathStr;
	else	// Search for system wide "tutorials" path
	{
		for (auto dataLocationPath : QStandardPaths::standardLocations(QStandardPaths::AppDataLocation))
		{
			QDir tutorialsDir(dataLocationPath + QString("/tutorials"));
			// Take first hit
			if ( tutorialsDir.exists() )
			{
				tutorialsPathStr = dataLocationPath;
				break;
			}
		}
	}

	// Spawn main window
	MainWindow w( appDataPathStr, tutorialsPathStr );

	for (int i = 1; i < argc; i++)
	{
		if ( iArgP > 0 && ( i == iArgP || i == iArgP+1 ) ) continue;	// If "-p" was specified, ignore it and the subsequent argv

		QString fileName( argv[i] );
		w.OpenVrt(fileName);
		break;
	}

	w.show();
	return a.exec();
}
