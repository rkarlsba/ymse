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

#include "Board.h"


// Methods to get objects at a grid location

int Board::GetComponentId(int row, int col)	// Pick the most relevant component at the location
{
	MakeToroid(row, col);	// Make co-ordinates wrap around at grid edges

	// Most to least prefered order is ...
	// ... unplaced plugs, unplaced non-plugs, placed plugs, placed non-plugs
	for (int iLoop = 0; iLoop < 4; iLoop++)
	{
		const bool bReqPlaced = ( iLoop / 2 == 1 );	// true ==> Only consider placed components
		const bool bReqPlug   = ( iLoop % 2 == 0 );	// true ==> Only consider components that can go under ICs
		for (const auto& mapObj : m_compMgr.GetMapIdToComp())
		{
			const Component&	comp	= mapObj.second;
			const bool			bPlug	= IsPlug( comp.GetType() );
			const bool			bPlaced = comp.GetIsPlaced();
			if ( bPlaced != bReqPlaced ) continue;
			if ( bPlug != bReqPlug ) continue;
			int rowTL = comp.GetRow();
			int colTL = comp.GetCol();

			MakeToroid(rowTL, colTL);	// Make co-ordinates wrap around at grid edges

			if ( row >= rowTL && row < rowTL + comp.GetCompRows() &&
				 col >= colTL && col < colTL + comp.GetCompCols() )
				return comp.GetId();
		}
		Component& trax = m_compMgr.GetTrax();
		if ( trax.GetSize() > 0 )
		{
			const bool bPlug	= IsPlug( trax.GetType() );
			const bool bPlaced	= trax.GetIsPlaced();
			if ( bPlaced != bReqPlaced ) continue;
			if ( bPlug != bReqPlug ) continue;
			int rowTL = trax.GetRow();
			int colTL = trax.GetCol();

			MakeToroid(rowTL, colTL);	// Make co-ordinates wrap around at grid edges

			if ( row >= rowTL && row < rowTL + trax.GetCompRows() &&
				 col >= colTL && col < colTL + trax.GetCompCols() )
			{
				if ( trax.GetCompElement(row-rowTL, col-colTL)->ReadFlagBits(RECTSET) )
					return trax.GetId();
			}
		}
	}
	return BAD_COMPID;
}

int Board::GetTextId(int row, int col)	// Pick the most relevant text box at the location
{
	MakeToroid(row, col);	// Make co-ordinates wrap around at grid edges

	TextRect bestRect;
	int bestId(BAD_TEXTID);
	for (const auto& mapObj : m_textMgr.m_mapIdtoText)
	{
		const TextRect& rect = mapObj.second;
		if ( !rect.ContainsPoint(row, col) ) continue;
		if ( !bestRect.GetIsValid() || rect.GetArea() < bestRect.GetArea() )
		{
			bestRect = rect;
			bestId	 = mapObj.first;
		}
	}
	return bestId;
}


// Methods to paint/unpaint nodeIds

void Board::SetNodeId(Element* p, const int& nodeId, const bool bAllLyrs)	// Helper to make sure we do UpdateCounts() before painting an element
{
	// For all layers case, always write base layer first
	Element* q	= bAllLyrs ? p->GetNbr(NBR_X) : nullptr;
	Element* p1	= std::min(p, q);
	Element* p2	= std::max(p, q);
	if ( p1 )
	{
		m_adjInfoMgr.UpdateCounts(p1, nodeId);	// Do this BEFORE we call SetNodeId() on the element
		p1->SetNodeId(nodeId);					// Write node value
	}
	if ( p2 )
	{
		m_adjInfoMgr.UpdateCounts(p2, nodeId);	// Do this BEFORE we call SetNodeId() on the element
		p2->SetNodeId(nodeId);					// Write node value
	}
}

void Board::ClearFlagBits(Element* p, const char& i, const bool bAllLyrs)
{
	// For all layers case, always write base layer first
	Element* q	= bAllLyrs ? p->GetNbr(NBR_X) : nullptr;
	Element* p1	= std::min(p, q);
	Element* p2	= std::max(p, q);
	if ( p1 ) p1->ClearFlagBits(i);
	if ( p2 ) p2->ClearFlagBits(i);
}

void Board::SetFlagBits(Element* p, const char& i, const bool bAllLyrs)
{
	// For all layers case, always write base layer first
	Element* q	= bAllLyrs ? p->GetNbr(NBR_X) : nullptr;
	Element* p1	= std::min(p, q);
	Element* p2	= std::max(p, q);
	if ( p1 ) p1->SetFlagBits(i);
	if ( p2 ) p2->SetFlagBits(i);
}

bool Board::SetNodeIdByUser(const int& lyr, const int& row, const int& col, const int& nodeId, const bool& bPaintPins)
{
	Element*		p			= Get(lyr, row, col);
	const bool		bHole		= p->GetIsHole();
	if ( bHole ) return false;	// No change
	const bool		bWire		= p->GetHasWire();
	const bool		bPin		= p->GetHasPin();
	assert( !bPin || p->GetHasComp() );	// Sanity check
	assert( !bWire || bPin );			// Wires must have pins

	WIRELIST wireList;	// Helper for chains of wires

	// Handle special case first.
	if ( bPin && !bWire && !bPaintPins )
	{
		// If trying to paint the board under a non-wire pin ...
		// ... we can modify the "origId" for the pin, but are only allowed
		// ... to wipe it or make it match the nodeId of the pin. Then quit.

		const size_t	pinIndex	= p->GetPinIndex();
		const int&		compId		= p->GetCompId();
		Component&		comp		= m_compMgr.GetComponentById(compId);
		assert( comp.GetType() != COMP::WIRE );	// Sanity check

		if ( nodeId != BAD_NODEID && nodeId != comp.GetNodeId(pinIndex) ) return false;	// Can't set a bad origId

		if ( comp.GetOrigId(lyr, pinIndex) == nodeId ) return false;	// origId is already as required

		// Need to do (RemoveComp/ SetNodeId/ AddComp) to ensure m_nodeInfoMgr is updated OK
		m_nodeInfoMgr.RemoveComp(comp);
		comp.SetOrigId(lyr, pinIndex, nodeId);
		m_nodeInfoMgr.AddComp(comp);
		return true;
	}

	// Now do regular cases:  Paint the board as needed...
	if ( bWire )
	{
		// Just need to get origId.  Any used slot will do
		size_t	pinIndex;
		int		compId;
		p->GetSlotInfo(p->GetUsedSlot(), pinIndex, compId);

		Component& comp = m_compMgr.GetComponentById(compId);	assert( comp.GetType() == COMP::WIRE );
		const int origId = comp.GetOrigId(lyr, pinIndex);

		if ( nodeId == p->GetNodeId() && origId == nodeId && p->ReadFlagBits(USERSET) )
			return false;	// No change
	}
	else if ( nodeId == p->GetNodeId() && p->ReadFlagBits(USERSET) )
		return false;		// No change

	// Set the NodeId on the element and all connected wire points
	p->GetWireList(wireList);	// Get list containing p and its wired points
	for (auto& o : wireList)
	{
		Element* pW = const_cast<Element*> (o.first);
		const bool bAllLyrs = pW->GetHasPin();
		SetNodeId(pW, nodeId, bAllLyrs);
		ClearFlagBits(pW, AUTOSET|VEROSET, bAllLyrs);
		SetFlagBits(pW, USERSET, bAllLyrs);
	}

	// Set the nodeId at the component pin
	if ( bPin )
	{
		if ( bWire )	// Wire
		{
			size_t	pinIndex;
			int		compId;
			for (auto& o : wireList)
			{
				Element* pL = const_cast<Element*> (o.first);
				for (int iSlot = 0; iSlot < 2; iSlot++)
				{
					Element* pW = pL->GetW(iSlot);

					pL->GetSlotInfo(iSlot, pinIndex, compId);
					assert( (pW == nullptr && compId == BAD_COMPID && pinIndex == BAD_PININDEX) ||
							(pW != nullptr && compId != BAD_COMPID && pinIndex != BAD_PININDEX) );

					if ( pW == nullptr ) continue;

					Component& comp = m_compMgr.GetComponentById(compId);
					assert( comp.GetType() == COMP::WIRE );	// Sanity check
					const size_t otherPinIndex = ( pinIndex == 0 ) ? 1 : 0;
					if ( pL == p )	// If it's the point that was clicked on, then set both the nodeId and origId
					{
						comp.SetNodeId(pinIndex, nodeId);
						for (int iLyr = 0; iLyr < 2; iLyr++)
							comp.SetOrigId(iLyr, pinIndex, nodeId);
					}
					comp.SetNodeId(otherPinIndex, nodeId);
					for (int iLyr = 0; iLyr < 2; iLyr++)
						comp.SetOrigId(iLyr, otherPinIndex, ( comp.GetOrigId(iLyr, otherPinIndex) != BAD_NODEID ) ? nodeId : BAD_NODEID);
				}
			}
		}
		else			// Regular component
		{
			const size_t	pinIndex	= p->GetPinIndex();
			const int&		compId		= p->GetCompId();
			Component&		comp		= m_compMgr.GetComponentById(compId);
			assert( bPaintPins && comp.GetType() != COMP::WIRE );	// Sanity check

			// Need to do (RemoveComp/ SetNodeId/ AddComp) to ensure m_nodeInfoMgr is updated OK
			m_nodeInfoMgr.RemoveComp(comp);
			comp.SetNodeId(pinIndex, nodeId);
			for (int iLyr = 0; iLyr < 2; iLyr++)
				if ( comp.GetOrigId(iLyr, pinIndex) != nodeId )	// Modifying a pin on a previously painted track ...
					comp.SetOrigId(iLyr, pinIndex, BAD_NODEID);	// ... should wipe the track under the pin
			m_nodeInfoMgr.AddComp(comp);
		}
	}
	return true;
}

void Board::FloodNodeId(const int& nodeId)
{
	for (int lyr = 0, lyrs = GetLyrs(); lyr < lyrs; lyr++)
	for (int row = 0, rows = GetRows(); row < rows; row++)
	for (int col = 0, cols = GetCols(); col < cols; col++)
	{
		Element* p = Get(lyr, row, col);
		if ( p->GetMH() == BAD_MH) continue;
		if ( p->GetHasWire() )
		{
			// Just need to get origId.  Any used slot will do
			size_t pinIndex;
			int compId;
			p->GetSlotInfo(p->GetUsedSlot(), pinIndex, compId);

			Component& comp = m_compMgr.GetComponentById(compId); assert( comp.GetType() == COMP::WIRE );
			const int origId = comp.GetOrigId(lyr, pinIndex);

			if ( origId != p->GetNodeId() || !p->ReadFlagBits(USERSET) ) continue; // Don't paint directly if it wasn't painted directly in the first place
		}
		SetNodeIdByUser(lyr, row, col, nodeId, true);	// true ==> paint pins
	}
}

void Board::AutoFillVero()
{
	int minRow, minCol, maxRow, maxCol;
	GetBounds(minRow, minCol, maxRow, maxCol);

	const bool& bVertical = GetVerticalStrips();

	// Note: The terms "top" and "bot" in the following code should be
	//		 taken to mean "left" and "right" if making horizontal strips.
	const int k(0);
	const int jMin	= ( bVertical ) ? minCol : minRow;
	const int jMax	= ( bVertical ) ? maxCol : maxRow;
	const int iMin	= ( bVertical ) ? minRow : minCol;
	const int iMax	= ( bVertical ) ? maxRow : maxCol;
	for (int j = jMin; j <= jMax; j++)
	{
		int nodeIdTop(BAD_NODEID), lenTop(INT_MAX);
		for (int i = iMin; i <= iMax; i++)
		{
			Element*	pC		= ( bVertical ) ? Get(k,i,j) : Get(k,j,i);
			const int&	nodeId	= pC->GetNodeId();
			if ( nodeId != BAD_NODEID )
			{
				if ( nodeIdTop == nodeId )
					lenTop++;		// Increment top count
				else
				{
					nodeIdTop = nodeId; lenTop = 1;	// Start top count
				}
				continue;
			}
			if ( pC->GetHasPin() ) continue;	// Can't assign pins, so skip
			if ( pC->GetIsHole() ) continue;	// Can't assign holes, so skip

			// Have a blank non-pin element at this point
			// Search for first used nodeId below

			int nodeIdBot(BAD_NODEID), lenBot(INT_MAX);
			for (int ii = i+1; ii <= iMax; ii++)
			{
				const Element*	pB		= ( bVertical ) ? Get(k,ii,j) : Get(k,j,ii);
				const int&		nodeId	= pB->GetNodeId();

				if ( nodeId == BAD_NODEID )
				{
					if ( nodeIdBot == BAD_NODEID )
						continue;					// Skip
					else
						break;						// End bottom count
				}
				else if ( nodeIdBot == BAD_NODEID )
				{
					nodeIdBot = nodeId; lenBot = 1;	// Start bottom count
				}
				else if ( nodeIdBot == nodeId )
					lenBot++;						// Increment bottom count
				else
					break;							// End bottom count
			}

			const bool bAllLyrs(false);
			if ( lenTop == INT_MAX && lenBot == INT_MAX )
				SetNodeId(pC, GetNewNodeId(), bAllLyrs);	// Unused strip, so make a new nodeId
			else if ( lenTop < lenBot )
				SetNodeId(pC, nodeIdTop, bAllLyrs);
			else
				SetNodeId(pC, nodeIdBot, bAllLyrs);
			ClearFlagBits(pC, USERSET|AUTOSET, bAllLyrs);
			SetFlagBits(pC, VEROSET, bAllLyrs);

			nodeIdTop = pC->GetNodeId(); lenTop = 1;	// Start top count
		}
	}
	m_colorMgr.ReAssignColors();	// Forces colors to be worked out again
}

void Board::CalcSolder()	// Work out locations of solder blobs to join veroboard tracks together
{
	for (int i = 0, iSize = GetSize(); i < iSize; i++)
	{
		Element* p = GetAt(i);
		p->SetSolderR(false);	// Clear solder
	}
	const bool& bVertical = GetVerticalStrips();	// Strip direction
	for (size_t n = 0, N = m_adjInfoMgr.GetSize(); n < N; n++)
	{
		const int& nodeId = m_adjInfoMgr.GetAt(n)->GetNodeId();
		if ( nodeId == BAD_NODEID ) continue;	// Want valid nodeIds
		const int& numCols = ( bVertical ) ? GetCols() : GetRows();
		for (int col = 0; col < numCols; col++)	// Loop cols/rows
			SetSolder(nodeId, col, bVertical);
	}
}

void Board::SetSolder(const int& nodeId, const int& col, const bool& bVertical)
{
	// Sets elements of bSolderR to indicate solder blob between col and col+1
	if (  bVertical && col == GetCols()-1 ) return;
	if ( !bVertical && col == GetRows()-1 ) return;

	assert( nodeId != BAD_NODEID );
	int bestRow(-1), bestRowPins(-INT_MAX), bestRowPads(INT_MAX);
	const int lyr(0);
	const int rowMax = ( bVertical ) ? GetRows() : GetCols();
	for (int row = 0; row < rowMax; row++)
	{
		const Element*	pC			= bVertical ? Get(lyr, row, col)	: Get(lyr, col,   row);
		const Element*	pR			= bVertical ? Get(lyr, row, col+1)	: Get(lyr, col+1, row);
		const bool		bMatch		= pC->GetNodeId() == nodeId && pR->GetNodeId() == nodeId;
		const bool		bLastRow	= row == rowMax-1;
		if ( bMatch )
		{
			int rowPins(0), rowPads(0);	// Try avoid putting a blob where we have a pad. Otherwise prefer pin locations.

			if ( pC->GetHasPin() )
			{
				if ( !pC->GetHasWire() && m_compMgr.GetComponentById(pC->GetCompId()).GetType() == COMP::PAD )
					rowPads++;
				else
					rowPins++;
			}
			if ( pR->GetHasPin() )
			{
				if ( !pR->GetHasWire() && m_compMgr.GetComponentById(pR->GetCompId()).GetType() == COMP::PAD )
					rowPads++;
				else
					rowPins++;
			}
			const bool bIsBetter =	( bestRow == -1 ) ||
									( rowPads < bestRowPads ) ||
									( rowPads == bestRowPads &&	rowPins > bestRowPins );
			if ( bIsBetter )
			{
				bestRow		= row;
				bestRowPins	= rowPins;
				bestRowPads	= rowPads;
			}
		}
		if ( bLastRow || !bMatch )
		{
			if ( bestRow != -1 )
			{
				if ( bVertical )
					Get(lyr, bestRow, col)->SetSolderR(true);
				else
					Get(lyr, col, bestRow)->SetSolderR(true);
			}
			bestRow		= -1;		// Reset
			bestRowPins	= -INT_MAX;	// Reset
			bestRowPads	=  INT_MAX;	// Reset
		}
	}
}


// Command enablers for GUI

bool Board::GetDisableCompText()
{
	if ( GetMirrored() ) return true;
	if ( ( GetGroupMgr().GetNumUserComps() != 1 ) || ( GetCompMode() == COMPSMODE::OFF || GetCompMode() == COMPSMODE::OUTLINE ) ) return true;
	const Component& comp	= GetUserComponent();
	const COMP&		 eType	= comp.GetType();
	return ( eType == COMP::WIRE || eType == COMP::MARK );	// No labels for wires and markers
}

bool Board::GetDisableMove()
{
	if ( GetMirrored() ) return true;
	Component& trax			= m_compMgr.GetTrax();
	const bool bHidingComps	= ( GetGroupMgr().GetNumUserComps() > 0 ) && ( GetCompMode()  == COMPSMODE::OFF );
	const bool bHidingTrax	= ( trax.GetSize() > 0 ) && ( GetTrackMode() == TRACKMODE::OFF );
	if ( bHidingComps || bHidingTrax ) return true;
	const bool bNoComps		= ( GetGroupMgr().GetNumUserComps() == 0 );
	const bool bNoTrax		= ( trax.GetSize() == 0 );
	return bNoComps && bNoTrax;
}

bool Board::GetDisableRotate()
{
	if ( GetMirrored() ) return true;
	Component& trax			= m_compMgr.GetTrax();
	const bool bHidingComps	= ( GetGroupMgr().GetNumUserComps() > 0 ) && ( GetCompMode()  == COMPSMODE::OFF );
	const bool bHidingTrax	= ( trax.GetSize() > 0 ) && ( GetTrackMode() == TRACKMODE::OFF );
	if ( bHidingComps || bHidingTrax ) return true;
	const bool bNoComps		= ( GetGroupMgr().GetNumUserComps() == 0 );
	const bool bMark		= ( GetGroupMgr().GetNumUserComps() == 1 && GetUserComponent().GetType() == COMP::MARK );	// Can't rotate a marker
	const bool bNoTrax		= ( trax.GetSize() == 0 );
	return ( bNoComps || bMark ) && bNoTrax;
}

bool Board::GetDisableStretch(bool bGrow)
{
	if ( GetMirrored() ) return true;
	if ( ( GetGroupMgr().GetNumUserComps() != 1 ) || ( GetCompMode() == COMPSMODE::OFF ) ) return true;
	const Component& comp = GetUserComponent();
	return !comp.CanStretch(bGrow);
}

bool Board::GetDisableStretchWidth(bool bGrow)
{
	if ( GetMirrored() ) return true;
	if ( ( GetGroupMgr().GetNumUserComps() != 1 ) || ( GetCompMode() == COMPSMODE::OFF ) ) return true;
	const Component& comp = GetUserComponent();
	return !comp.CanStretchWidth(bGrow);
}

bool Board::GetDisableChangeType()
{
	if ( GetMirrored() ) return true;
	if ( ( GetGroupMgr().GetNumUserComps() != 1 ) || ( GetCompMode() == COMPSMODE::OFF ) ) return true;
	const Component& comp	= GetUserComponent();
	const COMP		 eType	= comp.GetType();
	return !AllowTypeChange(eType, eType);
}

bool Board::GetDisableWipe() const
{
	return GetTrackMode() == TRACKMODE::OFF;
}
