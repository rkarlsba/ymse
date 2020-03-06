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

#include <cmath>
#include <stdio.h>
#include <string.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <ios>
#include <assert.h>
#include <cfloat>
#include <limits.h>
#include <algorithm>
#include <vector>
#include <list>
#include <set>
#include <map>
#include <unordered_map>
#include <chrono>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

struct MergeOffsets
{
	int deltaNodeId 	= 0;
	int deltaCompId 	= 0;
	int deltaGroupId	= 0;
	int deltaLyr		= 0;
	int deltaRow	 	= 0;
	int deltaCol 		= 0;
};

// The interface definition for handling merge offsets
struct Merge
{
	virtual	void UpdateMergeOffsets(MergeOffsets&) = 0;
	virtual void ApplyMergeOffsets(const MergeOffsets&) = 0;
};
