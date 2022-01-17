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

// MH_LRTB = Manhatten "distance" for horizontally/vertically adjacent grid points.
// MH_DIAG = Manhatten "distance" for diagonally adjacent grid points.
// MH_WIRE = Manhatten "distance" for wires, regardless of their length.
// MH_LPIN = Manhatten "distance" for changing layers at a pin (of a component or wire)
// MH_LVIA = Manhatten "distance" for changing layers at a via (i.e. place with no pin)

// Routing algorithm assumes:  MH_LVIA >= MH_DIAG > MH_LRTB > MH_WIRE >= MH_LPIN

// Increasing MH_LVIA will try harder to avoid making vias but makes the algorithm much slower.
// For maximum speed put MH_LVIA = 3.

#define MH_LPIN 1
#define MH_WIRE 1
#define MH_LRTB 2
#define MH_DIAG 3
#define MH_LVIA 6

// Routing methods

void Board::WipeAutoSetPoints(int nodeId)
{
	WIRELIST wireList;	// Helper for chains of wires

	const bool bWipeAll = ( nodeId == BAD_NODEID );
	for (int i = 0, iSize = GetSize(); i < iSize; i++)
	{
		Element* p = GetAt(i);
		if ( !bWipeAll && p->GetNodeId() != nodeId ) continue;	// Skip points with wrong nodeId
		const bool bAllLyrs = p->GetHasPin();
		bool bWipe = p->ReadFlagBits(AUTOSET) && !p->ReadFlagBits(USERSET);
		if ( p->GetHasWire() )
		{
			p->GetWireList(wireList);	// Get list containing p and its wired points ...
			for (auto& o : wireList)	// ... and disable wipe if any of them are USERSET
			{
				const Element* pW = o.first;
				if ( pW == p ) continue;	// Skip p
				bWipe &= ( !pW->ReadFlagBits(USERSET) );
				if ( !bWipe ) break;
			}
			for (auto& o : wireList)
			{
				Element* pW = const_cast<Element*> (o.first);
				if ( bWipe ) SetNodeId(pW, BAD_NODEID, bAllLyrs);
				ClearFlagBits(pW, AUTOSET, bAllLyrs);
				SetFlagBits(pW, USERSET, bAllLyrs);
			}
		}
		else
		{
			if ( bWipe ) SetNodeId(p, BAD_NODEID, bAllLyrs);
			ClearFlagBits(p, AUTOSET, bAllLyrs);
			SetFlagBits(p, USERSET, bAllLyrs);
		}
	}
}

void Board::BuildTargetPins(const int& nodeId)
{
	// Populate m_targetPins with all (non-wire) component pins with the specified NodeId.
	// These are the things on the board that the routing algorithm will try and connect together.

	assert( nodeId != BAD_NODEID );
	m_targetPins.clear();
	for (int i = 0, iSize = GetSize(); i < iSize; i++)
	{
		Element* p = GetAt(i);
		if ( p->IsLayer0() && p->GetHasPin() && p->GetNodeId() == nodeId && !p->GetHasWire() )
			m_targetPins.push_back(p);
	}
}

void Board::Route(bool bMinimal)
{
	m_bRouteMinimal	= bMinimal;

//	const auto start = std::chrono::steady_clock::now();

	// When routing is enabled,  this method will build tracks and update the cost in each NodeInfo.
	// When routing is disabled, this method will update each NodeInfo cost without building new tracks.
	if ( GetRoutingEnabled() ) WipeAutoSetPoints();

	m_nodeInfoMgr.SortByLowestDifficulty(m_compMgr);

	const bool bRipUpEnabled = GetRoutingMethod() == 1;

	const size_t numNodes = m_nodeInfoMgr.GetSize();

	for (size_t i = 0; i < numNodes; i++)
		m_nodeInfoMgr.GetAt(i)->SetCost(UINT_MAX);	// i.e. Mark all nodesIds as unrouted

	int iPasses(0);
	bool bImproved(true), bAllowRipUp( bRipUpEnabled && GetRoutingEnabled() );
	while ( bImproved )
	{
		bImproved = false;	// Gets set true if we manage to lower any route costs on this pass

		// Allowing multiple passes fixes some simplistic cases but kills speed. So only do one pass.
		// The code for multiple passes has been left in place for future tests.
		iPasses++; if ( iPasses > 1 ) break;

		for (size_t i = 0; i < numNodes; i++)	// Loop all nodeIds used by components
		{
			NodeInfo* pI = m_nodeInfoMgr.GetAt(i);
			if ( pI->GetCost() == 0 ) continue;	// Skip if fully routed

			const int& nodeIdI = pI->GetNodeId();

			// Flood with MH values, starting from the m_targetPins
			const unsigned int costI = ( nodeIdI != BAD_NODEID ) ? Flood(nodeIdI) : UINT_MAX;
			if ( costI < pI->GetCost() )
			{
				pI->SetCost( costI );
				bImproved = true;
			}

			if ( bAllowRipUp && nodeIdI != BAD_NODEID && pI->GetCost() > 0 && i > 0 )
			{
				TrackElementGrid Ibest, Iripped;

				CopyTo(Ibest);

				WipeAutoSetPoints(nodeIdI);	// Rip-up I

				CopyTo(Iripped);

				size_t j(i-1);	// Loop j through previously routed nodeIds
				while( pI->GetCost() > 0 )
				{
					NodeInfo* pJ = m_nodeInfoMgr.GetAt(j);
					const int& nodeIdJ = pJ->GetNodeId();
					if ( pJ->GetCost() == 0 )	// Only consider J if it is fully routed
					{
						WipeAutoSetPoints(nodeIdJ);	// Rip-up J

						const unsigned int costI = Flood(nodeIdI);	// Route I ...
						if ( costI < pI->GetCost() )				// ... and if I improved
						{
							const unsigned int costJ = Flood(nodeIdJ);	// Route J ...
							if ( costJ == 0 )							// ... and if J is still fully routed
							{
								bImproved = true;

								pI->SetCost( costI );	// Update cost I

								if ( costI > 0 )		// If we've not solved I ...
									CopyTo(Ibest);		// ... log the improved route (it's the best so far)
							}
						}
						if ( pI->GetCost() > 0 ) CopyFrom(Iripped);	// Revert to ripped-up I
					}
					if ( j == 0 ) break; else j--;
				}
				if ( pI->GetCost() > 0 ) CopyFrom(Ibest);
			}
		}
		if ( !bAllowRipUp ) break;
	}
//	const auto elapsed = std::chrono::steady_clock::now() - start;
//	const auto duration_ms	= std::chrono::duration_cast<std::chrono::milliseconds>(elapsed).count();
//	std::cout << "Time : " << duration_ms << std::endl;
}

void Board::UpdateVias()	// Sets the via flag to true on all candidate vias
{
	m_bRouteMinimal	= true;
	
	// If the ends of a candidate via can be connected through a pin on the board then it is not a via.
	const bool bRoutingEnabled	= GetRoutingEnabled();	// Log routing state
	const bool bViasEnabled		= GetViasEnabled();		// Log vias state
	SetRoutingEnabled(false);	// Don't build tracks
	SetViasEnabled(false);		// Disable routing through vias to perform test
	for (int i = 0, iSize = ( GetLyrs() == 1 ) ? GetSize() : ( GetSize() / 2 ); i < iSize; i++)	// Loop layer 0 only
	{
		Element* p = GetAt(i);
		Element* q = p->GetNbr(NBR_X);
		bool bIsVia = false;
		if ( bViasEnabled && q && !p->GetHasPin() && p->GetNodeId() == q->GetNodeId() && p->GetNodeId() != BAD_NODEID )	// If candidate via ...
		{
			m_targetPins.clear();
			m_targetPins.push_back(p);
			m_targetPins.push_back(q);
			bIsVia = Flood() > 0;
		}
		p->SetIsVia(bIsVia);
	}
	SetViasEnabled(bViasEnabled);		// Restore vias state
	SetRoutingEnabled(bRoutingEnabled);	// Restore routing state
}

unsigned int Board::Flood(const int& iFloodNodeId)
{
	BuildTargetPins(iFloodNodeId);	// Populate m_targetPins
	return Flood();
}

unsigned int Board::Flood()
{
	// Flood the board with MH values, starting from the m_targetPins.
	// The return value is a cost that shows how unconnected the pins are.
	// Zero cost means the pins are all inter-connected.

	if ( m_targetPins.size() < 2 ) return 0;	// Return cost of zero

	// Allocate the connection matrix ppConn[][] to indicate which pairs of targetPins are connected
	const size_t N	= m_targetPins.size();
	const size_t N2	= N * N;
	bool*	pConn	= new bool[N2];
	bool**	ppConn	= new bool*[N];
	memset(pConn, 0, N2 * sizeof(bool));
	for (size_t i = 0; i < N; i++) ppConn[i] = pConn + i * N;
	for (size_t i = 0; i < N; i++) ppConn[i][i] = true;	// Each pin is connected to itself
	unsigned int cost = (unsigned int)(N2 - N);	// Cost = number of false values in the connection matrix

	if ( m_bRouteMinimal )	// For minimal routing, first do a preliminary flood to see which pins are connected
		Flood_Helper(ppConn, cost, false);	// false ==> don't build new tracks

	if ( GetRoutingEnabled() )
		Flood_Helper(ppConn, cost, true);	// true ==> build new tracks

	// Deallocate connection matrix
	delete[] ppConn;
	delete[] pConn;
	return cost;
}

void Board::Flood_Helper(bool** ppConn, unsigned int& cost, const bool bBuildTracks)
{
	for (int i = 0, iSize = GetSize(); i < iSize; i++)	// Loop all grid points
		GetAt(i)->ResetMH();	// Wipe RouteId. Set "infinite" MH distance.  Zero max MH parameter.

	m_tmpVec.resize(GetSize(), nullptr);	// Clear the set of visited points
	m_tmpVecSize = 0;

	unsigned int iMH(0), iMaxMH(0);

	// Add each target pin to the set of visited points, with a unique routeId, and MH value of zero
	unsigned int RID(BAD_ROUTEID);
	for (auto p : m_targetPins)
	{
		m_tmpVec[m_tmpVecSize++] = p;			// Add p to set of visited points
		RID++;	assert( RID < BAD_ROUTEID );	// Should be safely < UINT_MAX in practice
		p->UpdateMH(RID, iMH, iMaxMH);
	}

	const bool			bMultiLayer	 = GetLyrs() > 1;
	const bool			bViasEnabled = bMultiLayer && GetViasEnabled();
	const int&			iFloodNodeId = m_targetPins[0]->GetNodeId();
	const unsigned int	numRIDs		 = RID + 1;	assert( m_targetPins.size() == (size_t) numRIDs );
	const bool			bDiagsOK	 = ( GetDiagsMode() != DIAGSMODE::OFF );
	const unsigned int	iMaxDeltaMH	 = ( bViasEnabled ) ? MH_LVIA : bDiagsOK ? MH_DIAG : MH_LRTB;	// The max MH increment in single-layer mode depends on if diagonals are allowed

	size_t jjStart(0);

	if ( !bMultiLayer ) iMH = MH_LRTB - 1;	// Set iMH so it's incremented to MH_LRTB on loop entry

	bool bDone(false);
	while( !bDone )
	{
		iMH++;	// Increase MH (think of this as distance from start points).

		// Now see what visited points have an MH value that is "one step away" from this target value.
		// For visited wires, both wires-ends are in the set of visited points.
		// Therefore we only need to consider the 8 neighbours for each visited point.

		if ( iMH == BAD_MH ) break;					// Quit if iMH reaches "infinity"
		if ( iMH > iMaxMH + iMaxDeltaMH ) break;	// Can't reach out further from the set of visited points

		const size_t jjSize = m_tmpVecSize;	// m_tmpVecSize gets modified in loop so take a copy
		for (size_t jj = jjStart; jj < jjSize && !bDone; jj++)	// Loop through visited points
		{
			Element* pJ = m_tmpVec[jj];

			if ( pJ->GetMaxMH() + iMaxDeltaMH < iMH )	// If pJ (and all previous points) are too far from the flood boundary
			{
				jjStart = jj + 1;						// ... then skip them from now on (since iMH only ever increases)
				continue;
			}

			const int iTypeMin(bMultiLayer ? 0 : 1), iTypeMax(bViasEnabled ? 2 : 1);
			for (int iType = iTypeMin; iType <= iTypeMax && !bDone; iType++)
			{
				switch( iType )
				{
					case 0:	// Type 0 ==> Change layer at a pin
						if ( pJ->GetHasPin() && pJ->GetMH() + MH_LPIN == iMH )
							Flood_Grow(numRIDs, iFloodNodeId, ppConn, cost, pJ, NBR_X, bBuildTracks, iMH, iMaxMH, bDone);
						break;
					case 1:	// Type 1 ==> Move within layer
						for (int iDiag = 0, iDiagMax = ( bDiagsOK ) ? 2 : 1; iDiag < iDiagMax && !bDone; iDiag++)	// First pass ==> Non-diagonal nbrs.  Second pass diagonal nbrs
						{
							const int iDeltaMH = ( iDiag ) ? MH_DIAG : MH_LRTB;
							if ( pJ->GetMH() + iDeltaMH != iMH ) continue;	// pJ has wrong MH for connection

							for (int iNbr = iDiag; iNbr < 8 && !bDone; iNbr += 2)	// Even/Odd iNbr ==> Non-diagonal/Diagonal
								Flood_Grow(numRIDs, iFloodNodeId, ppConn, cost, pJ, iNbr, bBuildTracks, iMH, iMaxMH, bDone);
						}
						break;
					case 2:	// Type 2 ==> Change layer at a via
						if ( !pJ->GetHasPin() && pJ->GetMH() + MH_LVIA == iMH )
							Flood_Grow(numRIDs, iFloodNodeId, ppConn, cost, pJ, NBR_X, bBuildTracks, iMH, iMaxMH, bDone);
						break;
				}
			}
		}
	}
}

void Board::Flood_Grow(const unsigned int& numRIDs, const int& iFloodNodeId, bool** ppConn, unsigned int& cost, Element* pJ, const int& iNbr, const bool& bBuildTracks, unsigned int& iMH, unsigned int& iMaxMH, bool& bDone)
{
	WIRELIST wireList;	// Helper for chains of wires

	Element* pK = pJ->GetNbr(iNbr);	assert( pK );
	if ( pK == nullptr ) return;

	const bool 			bOK	= pJ->GetNodeId() == iFloodNodeId;	// true ==> pJ already painted with correct NodeId
	const unsigned int& j 	= pJ->GetRouteId();
	const unsigned int& k 	= pK->GetRouteId();

	const bool bDirOK = ( bOK && pJ->GetUsed(iNbr) ) ||	// i.e. if already painted with correct nodeId
						( bBuildTracks && pJ->HaveNoBlankPins(iNbr) && !pJ->IsBlocked(iNbr, iFloodNodeId) && !pJ->IsUselessWire(iNbr, iFloodNodeId) );
	if ( !bDirOK ) return;

	if ( pK->GetMH() == BAD_MH ) // Grow route with RID j (from pJ to pK)
	{
		const int& nodeId = pK->GetNodeId();
		if ( nodeId == iFloodNodeId || nodeId == BAD_NODEID )
		{
			m_tmpVec[m_tmpVecSize++] = pK;	// Add pK to set of visited points
			pK->UpdateMH(j, iMH, iMaxMH);
			const bool bWire = pK->IsLayer0() && pK->GetHasWire();	// Constrain wire-routing to layer 0
			if ( bWire )
			{
				pK->GetWireList(wireList);	// Get list of pK and its wired points
				for (auto& o : wireList)	// Ideally want these in order of increasing MH
				{
					Element* pW = const_cast<Element*> (o.first);
					if ( pW == pK ) continue;	// Skip pK
					assert( pK->GetNodeId() == pW->GetNodeId() );			// Sanity check
					if ( pW->GetMH() != BAD_MH ) continue;					// Don't overwrite visited points (even if MH is improved)
					const unsigned int iOtherMH = iMH + MH_WIRE * o.second;	// Each wire increases MH by MH_WIRE
					m_tmpVec[m_tmpVecSize++] = pW;							// Add pW to set of visited points
					pW->UpdateMH(j, iOtherMH, iMaxMH);
				}
			}
		}
		return;
	}
	if ( ppConn[j][k] ) return;

	// Routes with RIDs j and k have met and don't have a connection yet ...
	if ( bBuildTracks )	// If building tracks ...
	{
		Backtrace(pJ, iFloodNodeId);	// ... trace pJ back to its source, painting iFloodNodeId along the way
		Backtrace(pK, iFloodNodeId);	// ... trace pK back to its source, painting iFloodNodeId along the way
	}

	// Make j-k connection and enforce transitivity
	typedef std::pair<unsigned int, unsigned int> CONNECTION;
	std::list<CONNECTION> list;		// Helper for updating the connection matrix
	list.push_back( CONNECTION(j,k) );
	while ( !list.empty() )
	{
		auto iter = list.begin();	// Read info from first list entry ...
		const auto a = iter->first;
		const auto b = iter->second;
		list.erase( iter );			// ... then remove the list entry

		if ( !ppConn[a][b] )	// If no a-b connection ...
		{
			ppConn[a][b] = ppConn[b][a] = true;	// Make a-b connection ...
			cost -= 2;							// Update cost
			for (unsigned int c = 0; c < numRIDs; c++)	// Update 1st-order transitive relations
			{
				if ( ppConn[a][c] )
				{
					if ( !ppConn[b][c] ) list.push_back( CONNECTION(b,c) );	// a-c connection ==> b-c connection
				}
				else
				{
					if (  ppConn[b][c] ) list.push_back( CONNECTION(a,c) );	// b-c connection ==> a-c connection
				}
			}
		}
	}
	bDone = ( cost == 0 );	// Zero cost ==> done
}

void Board::Backtrace(Element* pEnd, const int& nodeId)
{
	// Backtrace route from pEnd to point with MH = 0

	Element* p = pEnd;
	if ( p->GetMH() == BAD_MH ) return;

	WIRELIST wireList;	// Helper for chains of wires

	const bool bMultiLayer	= GetLyrs() > 1;
	const bool bViasEnabled	= bMultiLayer && GetViasEnabled();
	const bool bDiagsOK		= ( GetDiagsMode() != DIAGSMODE::OFF );

	unsigned int MH = p->GetMH();
	while ( true )	// Backtrace
	{
		assert( !p->GetIsHole() );

		const bool bAllLyrs = p->GetHasPin();

		Element* pW0 = p->GetW(0);
		Element* pW1 = p->GetW(1);
		const bool bWire = p->IsLayer0() && p->GetHasWire();	// Constrain wire-routing to layer 0
		if ( !p->GetHasPin() || bWire ) // For non-pins and wires
		{
			if ( p->GetNodeId() == BAD_NODEID )	// Set NodeId if not set yet.
			{
				SetNodeId(p, nodeId, bAllLyrs); ClearFlagBits(p, USERSET, bAllLyrs); SetFlagBits(p, AUTOSET, bAllLyrs);
				if ( bWire )
				{
					p->GetWireList(wireList);	// Get list of p and its wired points
					for (auto& o : wireList)
					{
						Element* pW = const_cast<Element*> (o.first);
						if ( pW == p ) continue;	// Skip p
						SetNodeId(pW, nodeId, bAllLyrs); ClearFlagBits(pW, USERSET, bAllLyrs); SetFlagBits(pW, AUTOSET, bAllLyrs);
					}
				}
			}
			else if ( p->ReadFlagBits(USERSET) )
			{
				assert( p->GetNodeId() == nodeId );
				SetFlagBits(p, AUTOSET, bAllLyrs);
				if ( bWire )
				{
					p->GetWireList(wireList);	// Get list of p and its wired points
					for (auto& o : wireList)
					{
						Element* pW = const_cast<Element*> (o.first);
						if ( pW == p ) continue;	// Skip p
						SetFlagBits(pW, AUTOSET, bAllLyrs);
					}
				}
			}
		}

		if ( MH == 0 ) break;

		bool bOK(false);
		// Now decide where to back trace to.

		// Check wires first...
		bOK = ( pW0 && pW0->GetMH() == MH - MH_WIRE );
		if ( bOK ) { p = pW0; MH -= MH_WIRE; continue; }
		bOK = ( pW1 && pW1->GetMH() == MH - MH_WIRE );
		if ( bOK ) { p = pW1; MH -= MH_WIRE; continue; }

		for (int iLoop = 0; iLoop < 2 && !bOK; iLoop++)	// First pass to give preference to nbrs that are not wire ends
		{
			const int iTypeMin(bMultiLayer ? 0 : 1), iTypeMax(bViasEnabled ? 2 : 1);
			for (int iType = iTypeMin; iType <= iTypeMax && !bOK; iType++)
			{
				switch( iType )
				{
					case 0:	// Type 0 ==> Change layer at a pin
						if ( p->GetHasPin() ) BacktraceHelper(p, nodeId, MH_LPIN, NBR_X, iLoop, MH, bOK);
						break;
					case 1:	// Type 1 ==> Move within layer
						for (int iDiag = 0, iDiagMax = ( bDiagsOK ) ? 2 : 1; iDiag < iDiagMax && !bOK; iDiag++)	// First pass ==> Non-diagonal nbrs.  Second pass diagonal nbrs
						{
							const int iDeltaMH = ( iDiag ) ? MH_DIAG : MH_LRTB;
							for (int iNbr = iDiag; iNbr < 8 && !bOK; iNbr += 2)	// Even/Odd iNbr ==> Non-diagonal/Diagonal
								BacktraceHelper(p, nodeId, iDeltaMH, iNbr, iLoop, MH, bOK);
						}
						break;
					case 2:	// Type 2 ==> Change layer at a via
						if ( !p->GetHasPin() ) BacktraceHelper(p, nodeId, MH_LVIA, NBR_X, iLoop, MH, bOK);
						break;
				}
			}
		}
		if ( bOK ) continue;

		assert(0);	// Oh dear. Something went badly wrong !!!
		break;
	}
}

void Board::BacktraceHelper(Element*& p, const int& nodeId, const int& iDeltaMH, const int& iNbr, const int& iLoop, unsigned int& MH, bool & bOK)
{
	Element* pNbr = p->GetNbr(iNbr);
	if ( pNbr->GetRouteId() != p->GetRouteId() ) return;		// Skip if nbr has wrong routeId
	const bool bWire = pNbr->IsLayer0() && pNbr->GetHasWire();	// Constrain wire-routing to layer 0
	if ( iLoop == 0 &&  bWire ) return;							// Skip if nbr is a wire
	if ( iLoop == 1 && !bWire ) return;							// Skip if nbr is a non-wire
	if ( p->IsBlocked(iNbr, nodeId) ) return;					// Skip if blocked
	if ( pNbr->GetMH() != MH - iDeltaMH ) return;				// Skip if wrong MH change
	p = pNbr;	MH -= iDeltaMH;		bOK = true;
}

void Board::Manhatten(Element* p)
{
	// Populate the grid with connected Manhatten-style "distances" to p.

	const int iTraceNodeId = p->GetNodeId();	// The NodeID to trace
	if ( iTraceNodeId == BAD_NODEID ) return;	// Don't trace invalid NodeID

	WIRELIST wireList;	// Helper for chains of wires

	for (int i = 0, iSize = GetSize(); i < iSize; i++)	// Loop all grid points
		GetAt(i)->ResetMH();	// Wipe RouteId. Set "infinite" MH distance.  Zero max MH parameter.

	m_tmpVec.resize(GetSize(), nullptr);	// Clear the set of visited points
	m_tmpVecSize = 0;
	
	const bool			bMultiLayer	 = GetLyrs() > 1;
	const bool			bViasEnabled = bMultiLayer && GetViasEnabled();
	const bool			bDiagsOK	 = ( GetDiagsMode() != DIAGSMODE::OFF );
	const unsigned int	iMaxDeltaMH	 = ( bMultiLayer ) ? MH_LVIA : bDiagsOK ? MH_DIAG : MH_LRTB;	// The max MH increment in single-layer mode depends on if diagonals are allowed

	size_t jjStart(0);
	const unsigned int RID(0);
	unsigned int iMH(0), iMaxMH(0);

	// Add p to set of visited points, with MH value of zero
	m_tmpVec[m_tmpVecSize++] = p;
	p->UpdateMH(RID, iMH, iMaxMH);
	const bool bWire = p->IsLayer0() && p->GetHasWire();	// Constrain wire-routing to layer 0
	if ( bWire )
	{
		p->GetWireList(wireList);	// Get list of p and its wired points
		for (auto& o : wireList)	// Ideally want these in order of increasing MH
		{
			Element* pW = const_cast<Element*> (o.first);
			if ( pW == p ) continue;	// Skip p
			assert( p->GetNodeId() == pW->GetNodeId() );			// Sanity check
			if ( pW->GetMH() != BAD_MH ) continue;					// Don't overwrite visited points (even if MH is improved)
			const unsigned int iOtherMH = iMH + MH_WIRE * o.second;	// Each wire increases MH by MH_WIRE
			m_tmpVec[m_tmpVecSize++] = pW;							// Add pW to set of visited points
			pW->UpdateMH(RID, iOtherMH, iMaxMH);
		}
	}
	while ( true )
	{
		iMH++;	// Increase MH (think of this as distance from start point).

		// Now see what visited points have an MH value that is "one step away" from this target value.
		// For visited wires, both wires-ends are in the set of visited points.
		// Therefore we only need to consider the 8 neighbours for each visited point.

		if ( iMH == BAD_MH ) break;					// Quit if iMH reaches "infinity"
		if ( iMH > iMaxMH + iMaxDeltaMH ) break;	// Can't reach out further from the set of visited points

		const size_t jjSize = m_tmpVecSize;	// m_tmpVecSize gets modified in loop so take a copy
		for (size_t jj = jjStart; jj < jjSize; jj++)	// Loop through visited points
		{
			const Element* pJ = m_tmpVec[jj];
			if ( pJ->GetNodeId() != iTraceNodeId ) continue;	// Wrong nodeId

			if ( pJ->GetMaxMH() + iMaxDeltaMH < iMH )	// If pJ (and all previous points) are too far from the flood boundary
			{
				jjStart = jj + 1;						// ... then skip them from now on (since iMH only ever increases)
				continue;
			}

			const int iTypeMin(bMultiLayer ? 0 : 1), iTypeMax(bViasEnabled ? 2 : 1);
			for (int iType = iTypeMin; iType <= iTypeMax; iType++)
			{
				switch( iType )
				{
					case 0:	// Type 0 ==> Change layer at a pin
						if ( pJ->GetHasPin() && pJ->GetMH() + MH_LPIN == iMH )
							ManhattenHelper(pJ, NBR_X, RID, iMH, iMaxMH);
						break;
					case 1:	// Type 1 ==> Move within layer
						for (int iDiag = 0, iDiagMax = ( bDiagsOK ) ? 2 : 1; iDiag < iDiagMax; iDiag++)	// First pass ==> Non-diagonal nbrs.  Second pass diagonal nbrs
						{
							const int iDeltaMH = ( iDiag ) ? MH_DIAG : MH_LRTB;
							if ( pJ->GetMH() + iDeltaMH != iMH ) continue;	// pJ has wrong MH for connection

							for (int iNbr = iDiag; iNbr < 8; iNbr += 2)	// Even/Odd iNbr ==> Non-diagonal/Diagonal
								ManhattenHelper(pJ, iNbr, RID, iMH, iMaxMH);
						}
						break;
					case 2:	// Type 2 ==> Change layer at a via
						if ( !pJ->GetHasPin() && pJ->GetMH() + MH_LVIA == iMH )
							ManhattenHelper(pJ, NBR_X, RID, iMH, iMaxMH);
						break;
				}
			}
		}
	}
}

void Board::ManhattenHelper(const Element* p, const int& iNbr, const int& RID, unsigned int& iMH, unsigned int& iMaxMH)
{
	if ( !ReadCodeBit(iNbr, p->GetRoutable()) ) return;	// Skip non-routable nbrs

	Element* pK	 = p->GetNbr(iNbr);	assert( pK );
	if ( pK == nullptr ) return;

	WIRELIST wireList;	// Helper for chains of wires

	if ( p->GetUsed(iNbr) && pK->GetMH() == BAD_MH )
	{
		m_tmpVec[m_tmpVecSize++] = pK;	// Add pK to set of visited points
		pK->UpdateMH(RID, iMH, iMaxMH);
		const bool bWire = pK->IsLayer0() && pK->GetHasWire();	// Constrain wire-routing to layer 0
		if ( bWire )
		{
			pK->GetWireList(wireList);	// Get list of pK and its wired points
			for (auto& o : wireList)	// Ideally want these in order of increasing MH
			{
				Element* pW = const_cast<Element*> (o.first);
				if ( pW == pK ) continue;	// Skip pK
				assert( pK->GetNodeId() == pW->GetNodeId() );			// Sanity check
				if ( pW->GetMH() != BAD_MH ) continue;					// Don't overwrite visited points (even if MH is improved)
				const unsigned int iOtherMH = iMH + MH_WIRE * o.second;	// Each wire increases MH by MH_WIRE
				m_tmpVec[m_tmpVecSize++] = pW;							// Add pW to set of visited points
				pW->UpdateMH(RID, iOtherMH, iMaxMH);
			}
		}
	}
}

void Board::CheckAllComplete()
{
	assert( !GetRoutingEnabled() );	// If routing is enabled, use the "RoutedOK" flags instead of the "Complete" flags.

	// New algorithm.
	/*
	// Calling Route() when routing is not enabled sets the cost info without building new tracks.
	// So we can use that to set the "Complete" flags
	Route(true);
	for (size_t n = 0, nSize = m_nodeInfoMgr.GetSize(); n < nSize; n++)
	{
		NodeInfo* pNodeInfo = m_nodeInfoMgr.GetAt(n);
		pNodeInfo->SetComplete( pNodeInfo->GetCost() == 0 );
	}
	m_nodeInfoMgr.SortByLowestDifficulty(m_compMgr);
	return;
	*/

	// Old algorithm.  In most cases this is faster than the new algorithm, but worst-case performance is worse
	for (size_t n = 0, nSize = m_nodeInfoMgr.GetSize(); n < nSize; n++)
	{
		NodeInfo* pNodeInfo = m_nodeInfoMgr.GetAt(n);
		pNodeInfo->SetComplete(false);

		const int& nodeId = pNodeInfo->GetNodeId();
		if ( nodeId == BAD_NODEID ) continue;

		BuildTargetPins(nodeId);

		bool bComplete(true);
		for (auto iterI = m_targetPins.begin(), iterEnd = m_targetPins.end(); iterI != iterEnd && bComplete; ++iterI)
		{
			Manhatten(*iterI);

			for (auto iterJ = iterI; iterJ != iterEnd && bComplete; ++iterJ)
				bComplete = ( (*iterJ)->GetMH() != BAD_MH );
		}
		pNodeInfo->SetComplete(bComplete);
	}
	m_nodeInfoMgr.SortByLowestDifficulty(m_compMgr);
}

void Board::PasteTracks(bool bTidy)
{
	assert( GetRoutingEnabled() != bTidy );

	if ( bTidy )
	{
		SetRoutingEnabled(true);
		Route(false);	// false ==> non minimal routing
	}

	size_t	iPinIndex;
	int		tmpCompId;

	for (int i = 0, iSize = GetSize(); i < iSize; i++)
	{
		Element* p = GetAt(i);

		const bool bAllLyrs = p->GetHasPin();

		// Tidy clears all non-pins and wires that are USER_SET ...
		if ( bTidy && ( !p->GetHasPin() || p->GetHasWire() ) && p->ReadFlagBits(USERSET) && !p->ReadFlagBits(AUTOSET|VEROSET) )
		{
			SetNodeId(p, BAD_NODEID, bAllLyrs);
			for (int iSlot = 0; iSlot < 2; iSlot++)
			{
				Element* pW = p->GetW(iSlot);
				if ( pW ) SetNodeId(pW, BAD_NODEID, bAllLyrs);
			}
		}

		ClearFlagBits(p, AUTOSET|VEROSET, bAllLyrs); SetFlagBits(p, USERSET, bAllLyrs);	// Don't do this on pW, or the tidy option will wipe wires !!!

		// For wires, the "Paste" operation either paints the board at the wire-ends or wipes it.
		// Fix-up the nodeId info on any wire components ...
		const int& nodeId = p->GetNodeId();
		for (int iSlot = 0; iSlot < 2; iSlot++)
		{
			Element* pW = p->GetW(iSlot);
			if ( pW == nullptr ) continue;

			p->GetSlotInfo(iSlot, iPinIndex, tmpCompId);
			Component& comp = m_compMgr.GetComponentById( tmpCompId );
			for (size_t i = 0, iSize = comp.GetNumPins(); i < iSize; i++)
			{
				comp.SetNodeId(i, nodeId);
				for (int lyr = 0; lyr < 2; lyr++) comp.SetOrigId(lyr, i, nodeId);
			}
		}
	}
	SetRoutingEnabled(false);
}

void Board::WipeTracks()
{
	FloatAllComps();	// Float all components

	// If we have a placed trax component, only wipe the board within it then destroy it
	Component& trax = m_compMgr.GetTrax();
	if ( trax.GetSize() > 0 && trax.GetIsPlaced() )
	{
		const bool bAllLyrs(false);
		const int& lyr = trax.GetLyr();

		for (int j = 0, jRow = trax.GetRow(), rows = trax.GetCompRows(); j < rows; j++, jRow++)
		for (int i = 0, iCol = trax.GetCol(), cols = trax.GetCompCols(); i < cols; i++, iCol++)
		{
			if ( !trax.GetCompElement(j,i)->ReadFlagBits(RECTSET) ) continue;
			Element* p = Get(lyr, jRow, iCol);
			assert( !p->GetHasPin() && !p->GetIsHole() && !p->GetHasComp() );	// Sanity check

			SetNodeId(p, BAD_NODEID, bAllLyrs);
			p->SetSurface(SURFACE_FREE);
			ClearFlagBits(p, AUTOSET|VEROSET|RECTSET, bAllLyrs);
			SetFlagBits(p, USERSET, bAllLyrs);
		}
		m_compMgr.ClearTrax();
		m_rectMgr.Clear();
	}
	else	// ... otherwise wipe all the points on the board. The floating trax component won't get wiped
	{
		const bool bAllLyrs(true);

		for (int k = 0, lyrs = GetLyrs(); k < lyrs; k++)
		for (int j = 0, rows = GetRows(); j < rows; j++)
		for (int i = 0, cols = GetCols(); i < cols; i++)
		{
			Element* p = Get(k, j, i);
			assert( !p->GetHasPin() && !p->GetIsHole() && !p->GetHasComp() );	// Sanity check
			SetNodeId(p, BAD_NODEID, bAllLyrs);
			p->SetSurface(SURFACE_FREE);
			ClearFlagBits(p, AUTOSET|VEROSET, bAllLyrs);
			SetFlagBits(p, USERSET, bAllLyrs);
		}
	}
	PlaceFloaters();	// Unfloat components
}
