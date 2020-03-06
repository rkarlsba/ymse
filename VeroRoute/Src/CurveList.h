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

#include <list>

class QPoint;
class QPolygon;

enum class GPEN {UNKNOWN = 0, MIL10, PAD, VIA, TRACK, PAD_GAP, VIA_GAP, TRACK_GAP, PAD_MASK, VIA_MASK, SILK, PAD_HOLE, VIA_HOLE};

// A class describing a curve as a set of points, with functionality for combining curves.
// Used for processing data before writing to Gerber file.

class Curve : public std::list<QPoint>	// A curve drawn in a fixed size pen
{
public:
	Curve() {}
	Curve(const GPEN& pen, const QPoint& p);
	Curve(const GPEN& pen, const QPolygon& polygon);
	~Curve()					{ clear(); }
	void Compress();		// Removes redundant points
	bool Splice(Curve* pB);	// Tries to splice curve B to this
	struct HasSmallerPen	// Predicate for sorting
	{
		bool operator() (const Curve* p1, const Curve* p2) const
		{
			return (int)(p1->m_pen) < (int)(p2->m_pen);
		}
	};
	GPEN m_pen = GPEN::UNKNOWN;
};

class CurveList : public std::list<Curve*>
{
public:
	CurveList()		{}
	~CurveList()	{ Clear(); }
	void Clear()	{ for (auto& p : *this) p->clear(); clear(); }
	void SpliceAll();
};
