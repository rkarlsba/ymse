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

#include "CurveList.h"
#include <QPolygon>

Curve::Curve(const GPEN& pen, const QPoint& p) : m_pen(pen)
{
	push_back(p);
}

Curve::Curve(const GPEN& pen, const QPolygon& polygon) : m_pen(pen)
{
	for (auto& p : polygon) push_back(p);
	// DO NOT COMPRESS BY DEFAULT.  That's only OK for open line segments
}

void Curve::Compress()	// Removes redundant points
{
	unique();
	bool bDone = size() < 3;
	while ( !bDone )
	{
		bDone = true;

		auto A = begin();
		auto B = A; ++B;
		auto C = B; ++C;
		for (; C != end(); ++A, ++B, ++C)
		{
			if ( A->x() == B->x() && B->x() == C->x() )	// Vertical
			{
				if ( ( B->y() >= A->y() && B->y() <= C->y() ) ||
					 ( B->y() >= C->y() && B->y() <= A->y() ) )
				{
					(*B) = (*A);	// .. make B == A so we can remove it as not unique
					bDone = false;
				}
			}
			else if ( A->y() == B->y() && B->y() == C->y() )	// Horizontal
			{
				if ( ( B->x() >= A->x() && B->x() <= C->x() ) ||
					 ( B->x() >= C->x() && B->x() <= A->x() ) )
				{
					(*B) = (*A);	// .. make B == A so we can remove it as not unique
					bDone = false;
				}
			}
		}
		if ( !bDone )
		{
			unique();
			bDone = size() < 3;
		}
	}
}

bool Curve::Splice(Curve* pB)	// Tries to splice curve B to this
{
	if	( m_pen != pB->m_pen ) return false;		// Pens must match
	if	( empty() || pB->empty() ) return false;	// Curves must have points
	// Try to get back of 'this' matching front of 'pB', then splice 'pB' to 'this'
	if		( front() == pB->back()  ) { reverse(); pB->reverse(); }
	else if	( front() == pB->front() ) { reverse(); }
	else if	( back()  == pB->back()  ) { pB->reverse(); }
	if		( back()  == pB->front() ) { splice(end(), *pB); Compress(); return true; }
	return false;
}

void CurveList::SpliceAll()
{
	sort(Curve::HasSmallerPen());	// Sort list of curves by pen type

	bool bDone(false);	// Keep splicing curves together till no more splices are possible.
	while ( !bDone )
	{
		bDone = true;
		for (auto iterA = begin(); iterA != end(); ++iterA)
		{
			auto iterB = iterA; ++iterB;
			for (; iterB != end(); ++iterB)
				if ( (*iterA)->Splice(*iterB) ) bDone = false;
		}
	}
}
