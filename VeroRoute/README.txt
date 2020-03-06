VeroRoute - Qt based Veroboard/Perfboard/PCB layout & routing application.

Version 1.84

Copyright (C) 2017  Alex Lawrow    ( dralx@users.sourceforge.net )



LICENSING
=========

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.



COMPILING THE SOURCE
====================

The application is written in C++ and uses the Qt cross platform framework.
It was built using the QtCreator IDE and the source code includes the Qt Creator project file.
To compile the source code yourself, you will need to download Qt and QtCreator.
It has been tested on Linux (both 32-bit and 64-bit) and on Windows 7 (both 32-bit and 64-bit).
Once compiled you can run it from within QtCreator by hitting the Green run button.
Note:  The Qt project file has been configured so that the executable VeroRoute.exe will be created just outside the Src folder. It needs to be there so that it can find the Tutorials folder.



PRE-COMPILED VERSIONS
=====================

For MS Windows:  Pre-compiled 32-bit and 64-bit versions of VeroRoute for Windows 7 are available.
They should also work on later versions of windows but have not been tested.
There is no installer for windows.  Just unzip everything to a folder and run it from there.

For Linux:  A precompiled 64-bit version for Linux Mint 18.3 is available.
It may also work on other recent 64-bit Linux distributions depending on whether
they have the necessary Qt libraries installed (it needs Qt version 5).

To install on Linux:  Unzip everything to a folder.  Then open a command line prompt in that folder and type

    sudo ./veroroute-install.sh

To uninstall on Linux:  Open a command line prompt in the unzipped folder and type

    sudo ./veroroute-uninstall.sh



REQUIREMENTS
============

Apart from the software and a computer, you need a keyboard and a 2-button mouse with a mousewheel.



IMPORTING FROM A TINYCAD SCHEMATIC
==================================

TinyCAD is a free schematic editor which can be downloaded from SourceForge.
Since VeroRoute V1.10, it is possible to produce a schematic using TinyCAD,
export the netlist to a file in Protel 1 format, and then import the file in VeroRoute.
TinyCAD must first have the VeroRoute symbols added to its libraries, and the schematic
must be produced using those symbols in order for VeroRoute to understand the package descriptions.
See the document "Using TinyCAD with VeroRoute.pdf" which can be downloaded from the Files section of the VeroRoute page on Sourceforge.
https://sourceforge.net/projects/veroroute/files/.



IMPORTING FROM A gEDA SCHEMATIC
===============================

Since VeroRoute V1.22, it is possible to produce a schematic using gEDA,
export the netlist to a file in Tango format, and then import the file in VeroRoute.
gEDA must first have the VeroRoute symbols added to its libraries, and the schematic
must be produced using those symbols in order for VeroRoute to understand the footprint descriptions.
See the file "Using_gEDA_with_VeroRoute.txt" in the "libraries/gEDA" folder where VeroRoute is installed.



TUTORIAL
========

There is an in-built tutorial under the Help->Tutorial menu item in the main window.  The tutorial files are stored as *.vrt files in the Tutorials folder.



UNDO/REDO
=========

The undo/redo functionality reads and writes temporary files to the folder

  "~/.local/share/veroroute/history" on Linux, 

or to the folder

  "C:/Users/<USER>/AppData/Roaming/veroroute/history" on Windows.

Any *.vrt files in that folder can be removed if the app is not running.
In the event that VeroRoute crashes, then the latest files written to that folder can be loaded to recover the circuit before the crash.



TEMPLATES
=========

Any user-defined part templates are stored in the file

  "~/.local/share/veroroute/templates/user.vrt" on Linux, 

or in the file

  "C:/Users/<USER>/AppData/Roaming/veroroute/templates/user.vrt" on Windows.


