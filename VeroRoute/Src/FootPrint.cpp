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

#include "FootPrint.h"
#include "CompManager.h"
#include "RectManager.h"

void FootPrint::BuildDefault(const COMP& type)
{
	// This method is for building a regular component (not a "tracks" component, or "custom" component)
	SetType(type);

	int numRows(0), numCols(0);
	const std::string pinStr = GetMakeInstructions(m_type, numRows, numCols);

	const bool bOK = ( (int)pinStr.size() == numRows * numCols );	assert( bOK );	// Check string length OK
	if ( !bOK ) return;

	Allocate(1, numRows, numCols);

	const bool	bPlug	= IsPlug(m_type);	// A "plug" component can plug the gap between rows of IC pins.
	const char*	szpins	= pinStr.c_str();

	const int iSize = GetSize();
	for (int i = 0; i < iSize; i++)
	{
		Pin* p = GetAt(i);
		const char pinChar = szpins[i];
		switch( pinChar )
		{
			case '.':	p->SetSurface(SURFACE_FREE);	break;
			case '-':	p->SetSurface(SURFACE_GAP);		break;
			default:	p->SetSurface(bPlug ? SURFACE_PLUG : SURFACE_FULL);
		}
		p->SetPinIndex(GetPinIndexFromLegacyPinChar(pinChar));
	}
	if ( m_type == COMP::WIRE ) SetupWire();
}

void FootPrint::BuildTrax(CompManager* pCompMgr, const RectManager& rectMgr, const ElementGrid& o, const int& nLyr, const int& nRowMin, const int& nRowMax, const int& nColMin, const int& nColMax)
{
	// This method is for building a "tracks" component
	SetType(COMP::TRACKS);

	const int numRows = 1 + nRowMax - nRowMin;
	const int numCols = 1 + nColMax - nColMin;

	Allocate(1, numRows, numCols);

	size_t	pinIndex;
	int		compId;

	int jRow(nRowMin);
	for (int j = 0; j < numRows; j++, jRow++)
	{
		int iCol(nColMin);
		for (int i = 0; i < numCols; i++, iCol++)
		{
			if ( !rectMgr.ContainsPoint(jRow,iCol) ) continue;

			CompElement*	pTarget = Grid<CompElement>::Get(0, j, i);
			Element*		pSource = o.Get(nLyr, jRow, iCol);	assert( pSource );

			int iNodeId = pSource->GetNodeId();

			if ( pSource->GetHasPin() )	// For pins/wires, let the track contain the origId before the part was placed
			{
				for (int iSlot = 0; iSlot < 2; iSlot++)
				{
					pSource->GetSlotInfo(iSlot, pinIndex, compId);
					if ( pinIndex == BAD_PININDEX ) continue;
					assert( compId != BAD_COMPID );
					const Component& comp = pCompMgr->GetComponentById( compId );
					iNodeId = comp.GetOrigId(nLyr, pinIndex);
					break;
				}
			}
			pTarget->SetNodeId(iNodeId);
			pTarget->SetCode(pSource->GetCode());	// Note: This is wrong at boundaries
			pTarget->SetFlagBits(RECTSET);
		}
	}
}
