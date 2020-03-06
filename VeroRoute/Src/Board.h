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

#include "CompDefiner.h"
#include "TemplateManager.h"
#include "NodeInfoManager.h"
#include "GroupManager.h"
#include "ColorManager.h"
#include "TextManager.h"

class MyScrollArea;

// Board is the main algorithm class.
// Just about everything apart from the GUI rendering code is in here.
// Holds a description of the circuit and handles all manipulation & routing.

class Board : public ElementGrid, public GuiControl
{
public:
	Board(int lyrs = 1, int rows = 35, int cols = 35)
	: ElementGrid()
	, GuiControl()
	, m_infoStr("Use this box to enter a circuit description or other info")
	, m_tmpVecSize(0)
	, m_bRouteMinimal(true)
	{
		Allocate(lyrs, rows, cols);
		GlueNbrs();		// Set pointers between neighbouring grid elements
	}

	Board(const Board& o, bool bFullCopy = true)
	: ElementGrid()
	, GuiControl()
	{
		PartialCopy(o);
		if ( bFullCopy ) RebuildAdjacencies();	// Slow
	}

	Board& operator=(const Board& o)
	{
		Clear();
		PartialCopy(o);
		RebuildAdjacencies();	// Slow
		return *this;
	}

	Board& PartialCopy(const Board& o)	// Copies everything but omits RebuildAdjacencies()
	{
		m_infoStr = o.m_infoStr;

		ElementGrid::operator=(o);	// Call operator= in base class
		GuiControl::operator=(o);	// Call operator= in base class

		GlueNbrs();		// Set pointers between neighbouring grid elements

		// Need all component locations before calling GlueWires()
		m_compMgr		= o.m_compMgr;

		for (const auto& mapObj : m_compMgr.GetMapIdToComp())
		{
			const Component& comp = mapObj.second;
			m_nodeInfoMgr.AddComp(comp);
		}

		GlueWires();	// Set pointers between wired grid elements

		//m_nodeInfoMgr	= o.m_nodeInfoMgr;	// Don't copy!	This manager is populated via the above calls to AddComp()
		//m_adjInfoMgr	= o.m_adjInfoMgr;	// Don't copy!	This manager is populated via the above calls to AddComp()
		m_groupMgr		= o.m_groupMgr;
		m_rectMgr		= o.m_rectMgr;
		m_textMgr		= o.m_textMgr;
		m_colorMgr		= o.m_colorMgr;
		m_compDefiner	= o.m_compDefiner;

		// Routing algorithm variables are cleared, not copied
		m_targetPins.clear();
		m_tmpVec.clear();
		m_tmpVecSize	= 0;
		m_bRouteMinimal	= true;

		return *this;
	}

	~Board()
	{
		m_infoStr.clear();
		m_targetPins.clear();
		m_tmpVec.clear();
	}

	bool operator==(const Board& o) const	// Compare persisted info
	{
		if ( m_infoStr != o.m_infoStr ) return false;
		if ( GuiControl::operator!=(o) ) return false;
		if ( ElementGrid::operator!=(o) ) return false;
		return m_compMgr		== o.m_compMgr
			&& m_groupMgr		== o.m_groupMgr
			&& m_rectMgr		== o.m_rectMgr
			&& m_textMgr		== o.m_textMgr
			&& m_compDefiner	== o.m_compDefiner;
	}
	bool operator!=(const Board& o) const
	{
		return !(*this == o);
	}
	void Clear()
	{
		Allocate(GetLyrs(), GetRows(), GetCols());
		GlueNbrs();	// Set pointers between neighbouring grid elements
		SetInfoStr("Use this box to enter a circuit description or other info");
		m_compMgr.Clear();
		m_targetPins.clear();
		m_tmpVec.clear();
		m_nodeInfoMgr.DeAllocate();
		m_adjInfoMgr.DeAllocate();
		m_groupMgr.Clear();
		m_rectMgr.Clear();
		m_textMgr.Clear();
		m_compDefiner.Clear();
		m_colorMgr.ReAssignColors();
		GuiControl::Clear();	// Clear() base class
	}

	void GlueNbrs()	// Set pointers between neighbouring grid elements
	{
		for (int iLyr = 0, iLyrs = GetLyrs(); iLyr < iLyrs; iLyr++)
		for (int iRow = 0, iRows = GetRows(); iRow < iRows; iRow++)
		{
			const int iT(iRow-1), iB(iRow+1);
			for (int iCol = 0, iCols = GetCols(); iCol < iCols; iCol++)
			{
				const int iL(iCol-1), iR(iCol+1);

				Element* p = Get(iLyr, iRow, iCol);
				p->SetNbr(NBR_L,	Get(iLyr, iRow, iL));
				p->SetNbr(NBR_LT,	Get(iLyr, iT,   iL));
				p->SetNbr(NBR_T,	Get(iLyr, iT,   iCol));
				p->SetNbr(NBR_RT,	Get(iLyr, iT,   iR));
				p->SetNbr(NBR_R,	Get(iLyr, iRow, iR));
				p->SetNbr(NBR_RB,	Get(iLyr, iB,   iR));
				p->SetNbr(NBR_B,	Get(iLyr, iB,   iCol));
				p->SetNbr(NBR_LB,	Get(iLyr, iB,   iL));
				p->SetNbr(NBR_X, iLyrs == 1 ? nullptr : Get((iLyr + 1 ) % 2, iRow, iCol) );

				p->ClearWires();	// Wires must be set by GlueWires()

				// Prevent toroidal routing at board edges
				int okDirs = CODEBITS_ALL;	// All neighbour directions OK by default
				if ( iRow == 0 )			{ ClearCodeBit(NBR_LT, okDirs); ClearCodeBit(NBR_T, okDirs); ClearCodeBit(NBR_RT, okDirs); }
				if ( iRow == GetRows()-1 )	{ ClearCodeBit(NBR_LB, okDirs); ClearCodeBit(NBR_B, okDirs); ClearCodeBit(NBR_RB, okDirs); }
				if ( iCol == 0 )			{ ClearCodeBit(NBR_LT, okDirs); ClearCodeBit(NBR_L, okDirs); ClearCodeBit(NBR_LB, okDirs); }
				if ( iCol == GetCols()-1 )	{ ClearCodeBit(NBR_RT, okDirs); ClearCodeBit(NBR_R, okDirs); ClearCodeBit(NBR_RB, okDirs); }
				p->SetRoutable(okDirs);
			}
		}
	}

	void GlueWires()	// Set pointers between wired grid elements
	{
		for (const auto& mapObj : m_compMgr.GetMapIdToComp())	// Iterate components
		{
			const int&			compId	= mapObj.first;
			const Component&	comp	= mapObj.second;
			if ( comp.GetType() == COMP::WIRE && comp.GetIsPlaced() )
			{
				const int& lyr  = comp.GetLyr();	assert( lyr == 0 );
				const int& rowA = comp.GetRow();
				const int& colA = comp.GetCol();
				const int  rowB = comp.GetLastRow();
				const int  colB = comp.GetLastCol();

				Element* pA = Get(lyr, rowA, colA);	assert( pA->GetNumWires() < 2 );
				Element* pB = Get(lyr, rowB, colB);	assert( pB->GetNumWires() < 2 );

				assert( pA->GetNumCompIds() > 0 && pA->GetNumCompIds() < 3 );
				assert( pB->GetNumCompIds() > 0 && pB->GetNumCompIds() < 3 );
				assert( pB->GetNodeId() == pA->GetNodeId() );	// Wire ends must have same NodeId

				const int iSlotA = pA->GetSlotFromCompId(compId);
				const int iSlotB = pB->GetSlotFromCompId(compId);
				pA->SetW( iSlotA, pB );	// Give pA a pointer to pB
				pB->SetW( iSlotB, pA );	// Give pB a pointer to pA				
			}
		}
	}

	bool Pan(int iDown, int iRight)	// Pan whole circuit w.r.t. the grid. This can grow the grid but never shrink it
	{
		if ( iDown == 0 && iRight == 0 ) return false;

		int incRows(0), incCols(0);	// What we need to grow the grid by if we move too far

		int minRow, minCol, maxRow, maxCol;
		const bool bOK = GetBounds(minRow, minCol, maxRow, maxCol);	// Get the circuit bounds
		if ( !bOK )	return false;	// Can't move an empty circuit

		if ( minRow + iDown  < 0 )						// Too far up ...
		{
			incRows	= -(minRow + iDown);				// ... so grow grid from bottom
			iDown	= -minRow;							// ... and pan to very top
		}
		else if ( maxRow + iDown  >= GetRows() )		// Too far down ...
			incRows = 1 + maxRow + iDown - GetRows();	// ...so grow grid from bottom

		if ( minCol + iRight < 0 )						// Too far left ...
		{
			incCols	= -(minCol + iRight);				// ... so grow grid from right
			iRight	= -minCol;							// ... and pan to very left
		}
		else if ( maxCol + iRight >= GetCols() )		// Too far right ...
			incCols = 1 + maxCol + iRight - GetCols();	// ... so grow grid from right

		GrowThenPan(0, incRows, incCols, iDown, iRight);
		return true;
	}

	void SmartPan(int iDown, int iRight)	// Pan whole circuit w.r.t. the grid, and grow/shrink grid as needed
	{
		if ( iDown == 0 && iRight == 0 ) return;

		int incRows(0), incCols(0);	// What we need to grow the grid by if we pan too far

		int minRow, minCol, maxRow, maxCol;
		const bool bOK = GetBounds(minRow, minCol, maxRow, maxCol);	// Get the circuit bounds

		if ( !bOK )	// Have empty board, so there is nothing to pan. Just grow or shrink instead.
		{
			if ( iDown  < 0 && GetRows() > 1 )	incRows--;
			if ( iDown  > 0 )					incRows++;
			if ( iRight < 0 && GetCols() > 1 )	incCols--;
			if ( iRight > 0 )					incCols++;

			return GrowThenPan(0, incRows, incCols, 0, 0);
		}

		if ( minRow + iDown  < 0 )							// Too far up ...
		{
			iDown = -minRow;								// ... so pan to very top
			incRows = std::min(0, maxRow + 1 - GetRows());	// ... and shrink grid from bottom
		}
		else if ( maxRow + iDown  >= GetRows() )			// Too far down ...
			incRows	= 1 + maxRow + iDown - GetRows();		// ...so grow grid from bottom

		if ( minCol + iRight < 0 )							// Too far left ...
		{
			iRight	= -minCol;								// ... so pan to very left
			incCols	= std::min(0, maxCol + 1 - GetCols());	// ... and shrink grid from right
		}
		else if ( maxCol + iRight >= GetCols() )			// Too far right ...
			incCols	= 1 + maxCol + iRight - GetCols();		// ... so grow grid from right

		GrowThenPan(0, incRows, incCols, iDown, iRight);
	}

	bool Crop(int iRowMargin = -1, int iColMargin = -1)	// Moves whole circuit to the top-left, then crops the grid from the bottom-right, then adds margin
	{
		int minRow, minCol, maxRow, maxCol;
		const bool bOK = GetBounds(minRow, minCol, maxRow, maxCol);	// Get the circuit bounds
		if ( !bOK ) return false;

		Pan(-minRow, -minCol);	// First pan to top-left corner of grid
		GrowThenPan(0, maxRow - minRow + 1 - GetRows(), maxCol - minCol + 1 - GetCols(), 0, 0); // Then shrink grid from bottom-right

		if ( iRowMargin == -1 ) iRowMargin = GetCropMargin();	// -1 ==> use default
		if ( iColMargin == -1 ) iColMargin = GetCropMargin();	// -1 ==> use defaul
		if ( iRowMargin > 0 || iColMargin > 0 )
		{
			const int incRows = 2 * iRowMargin;
			const int incCols = 2 * iColMargin;
			GrowThenPan(0, incRows, incCols, iRowMargin, iColMargin);
		}
		return true;
	}

	void GrowThenPan(const int& incLyrs, const int& incRows, const int& incCols, const int& iDown, const int& iRight)
	{
		ElementGrid::Grow(incLyrs, incRows, incCols);	// Grow the base class
		ElementGrid::Pan(iDown, iRight);				// Pan the base class

		GlueNbrs();		// Set pointers between neighbouring grid elements

		// Need all component locations before calling GlueWires()
		for (auto& mapObj : m_compMgr.m_mapIdToComp)
		{
			Component& comp	= mapObj.second;
			int newRow = comp.GetRow() + iDown;
			int newCol = comp.GetCol() + iRight;

			MakeToroid(newRow, newCol);	// Make co-ordinates wrap around at grid edges

			comp.SetRow(newRow);
			comp.SetCol(newCol);
		}
		Component& trax = m_compMgr.GetTrax();
		if ( trax.GetSize() > 0 )	// If have a trax pattern
		{
			int newRow = trax.GetRow() + iDown;
			int newCol = trax.GetCol() + iRight;

			MakeToroid(newRow, newCol);	// Make co-ordinates wrap around at grid edges

			trax.SetRow(newRow);
			trax.SetCol(newCol);
		}

		GlueWires();	// Set pointers between wired grid elements

		// Move all user-defined rectangles
		GetRectMgr().MoveAll(iDown, iRight);

		// Move all user-defined text
		GetTextMgr().MoveAll(iDown, iRight);

		if ( incLyrs > 0 )	// If we added a layer ...
		{
			// ... do TakeOff() and PutDown() for all placed parts
			// ... so new layer is set correctly
			for (auto& mapObj : m_compMgr.m_mapIdToComp)
			{
				Component& comp	= mapObj.second;
				if ( !comp.GetIsPlaced() ) continue;
				TakeOff(comp);
				PutDown(comp);
			}
		}
	}

	bool GetBounds(int& minRow, int& minCol, int& maxRow, int& maxCol) const
	{
		bool bOK(false);
		// First consider all painted nodeIds on the board
		const int numLyrs( GetLyrs() ), numRows( GetRows() ), numCols( GetCols() );
		minRow = numRows - 1;	maxRow = 0;	// Start with min and max at the wrong ends
		minCol = numCols - 1;	maxCol = 0;	// Start with min and max at the wrong ends
		for (int k = 0; k < numLyrs; k++)
		for (int j = 0; j < numRows; j++)
		for (int i = 0; i < numCols; i++)
		{
			if ( Get(k,j,i)->GetNodeId() == BAD_NODEID ) continue;
			minRow = std::min(minRow, j);	maxRow = std::max(maxRow, j);
			minCol = std::min(minCol, i);	maxCol = std::max(maxCol, i);
			bOK = true;
		}
		if ( !m_compMgr.GetIsEmpty() )
		{
			const Rect rect = m_compMgr.GetBounding();
			minRow = std::min(minRow, rect.m_rowMin);	maxRow = std::max(maxRow, rect.m_rowMax);
			minCol = std::min(minCol, rect.m_colMin);	maxCol = std::max(maxCol, rect.m_colMax);
			bOK = true;
		}
		const Rect rect = m_textMgr.GetBounding();
		if ( rect.GetIsValid() )
		{
			minRow = std::min(minRow, rect.m_rowMin);	maxRow = std::max(maxRow, rect.m_rowMax);
			minCol = std::min(minCol, rect.m_colMin);	maxCol = std::max(maxCol, rect.m_colMax);
			bOK = true;
		}
		return bOK;
	}

	void RebuildAdjacencies()
	{
		m_adjInfoMgr.DeAllocate();
		for (int i = 0, iSize = GetSize(); i < iSize; i++) m_adjInfoMgr.InitCounts( GetAt(i) );
	}

	Component& GetUserComponent()	// The currently selected component
	{
		assert( m_groupMgr.GetNumUserComps() == 1 );	// Should only have one component selected
		return m_compMgr.GetComponentById( m_groupMgr.GetUserCompId() );
	}

	// Methods to get objects at a grid location
	int  GetComponentId(int row, int col);	// Pick the most relevant component at the location
	int  GetTextId(int row, int col);		// Pick the most relevant text box at the location

	// Methods to paint/unpaint nodeIds
	void SetNodeId(Element* p, const int& nodeId, const bool bAllLyrs);	// Helper to make sure we do UpdateCounts() before painting an element
	void ClearFlagBits(Element* p, const char& i, const bool bAllLyrs);
	void SetFlagBits(Element* p, const char& i, const bool bAllLyrs);
	bool SetNodeIdByUser(const int& lyr, const int& row, const int& col, const int& nodeId, const bool& bPaintPins);
	void FloodNodeId(const int& nodeId);
	void AutoFillVero();
	void CalcSolder();	// Work out locations of solder blobs to join veroboard tracks together
	void SetSolder(const int& nodeId, const int& col, const bool& bVertical);

	// Routing methods
	void WipeAutoSetPoints(const int nodeId = BAD_NODEID);
	void BuildTargetPins(const int& nodeId);
	void Route(bool bMinimal);
	void UpdateVias();
	unsigned int Flood(const int& nodeId);
	unsigned int Flood();
	void Flood_Helper(bool** ppConn, unsigned int& cost, const bool bBuildTracks);
	void Flood_Grow(const unsigned int& numRIDs, const int& iFloodNodeId, bool** ppConn, unsigned int& cost, Element* pJ, const int& iNbr, const bool& bBuildTracks, unsigned int& iMH, unsigned int& iMaxMH, bool& bDone);
	void Backtrace(Element* pEnd, const int& nodeId);
	void BacktraceHelper(Element*& p, const int& nodeId, const int& iDeltaMH, const int& iNbr, const int& iLoop, unsigned int& MH, bool& bOK);
	void Manhatten(Element* p);
	void ManhattenHelper(const Element* p, const int& iNbr, const int& RID, unsigned int& iMH, unsigned int& iMaxMH);
	void CheckAllComplete();
	void PasteTracks(bool bTidy);
	void WipeTracks();

	// Methods for component creation/destruction
	void DestroyComponent(Component& comp);	// Destroys a component on the board
	int  CreateComponent(int iRow, int iCol, const COMP& eType, const Component* pComp = nullptr);
	int  AddComponent(int iRow, int iCol, const Component& tmp, bool bDoPlace = true);
	void AddTextBox(int iRow, int iCol);

	// Methods for component placement/removal
	bool CanPutDown(Component& comp);	// Checks if its possible to place the (floating) component on the board
	bool PutDown(Component& comp);		// Tries to place the (floating) component on the board
	bool TakeOff(Component& comp);
	void FloatAllComps();				// Float all components (i.e. take them off the board)
	void PlaceFloaters();				// Try to place down all the floating components

	// GUI helpers for manipulating user-selected components
	void SelectAllComps(bool bRestrictToRects);
	bool ConfirmDestroyUserComps();	// returns false if user-group is empty or has only wires and markers
	void DestroyUserComps();		// Destroy components in the user-group
	void MoveUserCompText(const int& deltaRow, const int& deltaCol);	// Move text label
	void StretchUserComp(const bool& bGrow);		// Stretch the selected component length
	void StretchWidthUserComp(const bool& bGrow);	// Stretch the selected component width (just for DIPs)
	void ChangeTypeUserComp(const COMP& eType);
	void CopyUserComps();											// Make a blank copy of the user-group components and float them
	bool MoveUserComps(const int& deltaRow, const int& deltaCol);	// Move user-group components, and return true if the grid was panned
	void RotateUserComps(const bool& bCW);							// Rotate the selected components
	void CopyComps(const std::list<int>& compIds);					// Make a blank copy of the components and float them
	bool MoveTextBox(const int& deltaRow, const int& deltaCol);		// Move text box, and return true if the grid was panned
	bool MoveComps(const std::list<int>& compIds, const int& deltaRow, const int& deltaCol);	// Move components and return true if the grid was panned
	void RotateComps(const std::list<int>& compIds, const bool& bCW);	// Rotate components
	Rect GetFootprintBounds(const std::list<int>& compIds);

	// Command enablers for GUI
	bool GetDisableCompText();
	bool GetDisableMove();
	bool GetDisableRotate();
	bool GetDisableStretch(bool bGrow);
	bool GetDisableStretchWidth(bool bGrow);
	bool GetDisableChangeType();
	bool GetDisableWipe() const;

	void				SetInfoStr(const std::string& str)	{ m_infoStr = str; }
	const std::string&	GetInfoStr() const	{ return m_infoStr; }
	void				CalculateColors()	{ m_colorMgr.CalculateColors(m_adjInfoMgr, this); }
	int					GetNewNodeId()		{ return m_nodeInfoMgr.GetNewNodeId(m_adjInfoMgr); }
	CompManager&		GetCompMgr()		{ return m_compMgr; }
	NodeInfoManager&	GetNodeInfoMgr()	{ return m_nodeInfoMgr; }
	GroupManager&		GetGroupMgr()		{ return m_groupMgr; }
	RectManager&		GetRectMgr()		{ return m_rectMgr; }
	TextManager&		GetTextMgr()		{ return m_textMgr; }
	ColorManager&		GetColorMgr()		{ return m_colorMgr; }
	CompDefiner&		GetCompDefiner()	{ return m_compDefiner; }
	int	 GetCurrentPinId() const			{ return m_compDefiner.GetCurrentPinId(); }
	int	 GetCurrentShapeId() const			{ return m_compDefiner.GetCurrentShapeId(); }
	bool SetCurrentPinId(const int& i)		{ return m_compDefiner.SetCurrentPinId(i); }
	bool SetCurrentShapeId(const int& i)	{ return m_compDefiner.SetCurrentShapeId(i); }

	// Import Protel V1 / Tango netlist (exported from TinyCAD / gEDA)
	bool ImportTango(const TemplateManager& templateMgr, const std::string& filename, std::string& errorStr);
	// Import OrCAD2 netlist (exported from KiCAD)
	bool ImportOrcad(const TemplateManager& templateMgr, const std::string& filename, std::string& errorStr);
	bool BreakComponentIntoPads(Component& comp);
	bool GetPinRowCol(const int& compId, const size_t& iPinIndex, int& row, int& col) const;

	// Merge interface functions
	virtual void UpdateMergeOffsets(MergeOffsets& o) override
	{
		ElementGrid::UpdateMergeOffsets(o);	// Call UpdateMergeOffsets in base class
		GuiControl::UpdateMergeOffsets(o);	// Call UpdateMergeOffsets in base class
		m_compMgr.UpdateMergeOffsets(o);
		m_groupMgr.UpdateMergeOffsets(o);
		m_textMgr.UpdateMergeOffsets(o);
	}
	virtual void ApplyMergeOffsets(const MergeOffsets& o) override	// Called on the source board
	{
		ElementGrid::ApplyMergeOffsets(o);	// Call ApplyMergeOffsets in base class
		GuiControl::ApplyMergeOffsets(o);	// Call ApplyMergeOffsets in base class
		m_compMgr.ApplyMergeOffsets(o);
		m_groupMgr.ApplyMergeOffsets(o);
		m_textMgr.ApplyMergeOffsets(o);
	}
	void Merge(Board& src)
	{
		GetRectMgr().Clear();	// Wipe rect manager

		Crop(GetCropMargin(), 0);	// Crop this board (the destination board) with zero margin for cols
		src.Crop(0, 0);				// Crop the source board (the one to merge in) with zero margins

		// Get merge offsets from this board
		MergeOffsets o;
		UpdateMergeOffsets(o);

		// Grow this board to make room for the merge
		Grow(std::max(0, src.GetLyrs() - GetLyrs()),
			 src.GetRows()+1,
			 std::max(0, src.GetCols() - GetCols()));
		GlueNbrs();	// Set pointers between neighbouring grid elements

		// Apply offsets to the source board, so we can merge it with no conflicts
		src.ApplyMergeOffsets(o);

		// Now merge ...
		ElementGrid::Merge(src, o);	// Call Merge in base class
		GuiControl::Merge(src);		// Call Merge in base class

		// Merge the components.  Need all component locations before calling GlueWires()
		m_compMgr.Merge(src.m_compMgr);
		for (const auto& mapObj : m_compMgr.GetMapIdToComp())
		{
			const Component& comp = mapObj.second;
			if ( comp.GetId() >= o.deltaCompId )	// Only want to add the new components
				m_nodeInfoMgr.AddComp(comp);
		}

		GlueWires();	// Set pointers between wired grid elements

		// Merge the group info
		m_groupMgr.Merge(src.m_groupMgr);
		RebuildAdjacencies();	// Slow

		// Merge the text label
		m_textMgr.Merge(src.m_textMgr);

		Crop();	// Final crop (with default margin)
	}

	// Persist functions
	virtual void Load(DataStream& inStream) override
	{
		Clear();

		int iVrtVersion(0);
		inStream.Load(iVrtVersion);
		inStream.SetVersion(iVrtVersion);
		inStream.SetOK(iVrtVersion <= VRT_VERSION_CURRENT);
		if ( !inStream.GetOK() ) return;	// Unsupported VRT version

		inStream.Load(m_infoStr);

		GuiControl::Load(inStream);			// Call Load() on base class
		ElementGrid::Load(inStream);		// Call Load() on base class

		GlueNbrs();		// Set pointers between neighbouring grid elements

		// Need all component locations before calling GlueWires()
		m_compMgr.Load(inStream);			// Call Load() on component manager

		for (const auto& mapObj : m_compMgr.GetMapIdToComp())
			m_nodeInfoMgr.AddComp(mapObj.second);
		
		GlueWires();	// Set pointers between wired grid elements

		if ( inStream.GetVersion() < VRT_VERSION_26 )
			FixLegacyWires();

		m_groupMgr.Load(inStream);			// Call Load() on group manager

		if ( inStream.GetVersion() >= VRT_VERSION_10 )
			m_rectMgr.Load(inStream);		// Call Load() on rect manager

		if ( inStream.GetVersion() >= VRT_VERSION_14 )
			m_textMgr.Load(inStream);		// Call Load() on text manager

		if ( inStream.GetVersion() >= VRT_VERSION_23 )
			m_compDefiner.Load(inStream);	// Call Load() on component definer

		RebuildAdjacencies();
	}

	virtual void Save(DataStream& outStream) override
	{
		outStream.Save(VRT_VERSION_CURRENT);
		outStream.Save(m_infoStr);
		GuiControl::Save(outStream);	// Call Save() on base class
		ElementGrid::Save(outStream);	// Call Save() on base class
		m_compMgr.Save(outStream);		// Call Save() on the component manager
		m_groupMgr.Save(outStream);		// Call Save() on the group manager
		m_rectMgr.Save(outStream);		// Call Save() on rect manager			// Added in VRT_VERSION_10
		m_textMgr.Save(outStream);		// Call Save() on text manager			// Added in VRT_VERSION_14
		m_compDefiner.Save(outStream);	// Call Save() on component definer		// Added in VRT_VERSION_23
	}
private:
	void FixLegacyWires()
	{
		// Find all placed wires and update the surface and hole codes on the board
		for (const auto& mapObj : m_compMgr.GetMapIdToComp())
		{
			const Component& comp = mapObj.second;
			if ( comp.GetType() == COMP::WIRE && comp.GetIsPlaced() )
			{
				const int& lyr = comp.GetLyr();	assert( lyr == 0 );
				int jRow( comp.GetRow() );
				for (int j = 0, jRows = comp.GetCompRows(); j < jRows; j++, jRow++)
				{
					int iCol( comp.GetCol() );
					for (int i = 0, iCols = comp.GetCompCols(); i < iCols; i++, iCol++)
					{
						Element* p = Get(lyr, jRow, iCol);
						// Want GetIsPin() methods to be private so commented out following assert
						// assert( comp.GetCompElement(j, i)->GetIsPin() == p->GetIsPin() );
						assert( p->GetSurface() == SURFACE_PLUG || p->GetSurface() == SURFACE_FULL );
						const bool bGap = ( p->GetSurface() & SURFACE_GAP ) > 0;
						p->SetWireOccupancies();
						if ( bGap ) p->SetSurface( p->GetSurface() + SURFACE_GAP );
					}
				}
			}
		}
	}
private:
	std::string				m_infoStr;		// General info

	// Component definer
	CompDefiner				m_compDefiner;	// For defining a custom component

	// Managers
	CompManager				m_compMgr;		// Manages components on the board
	NodeInfoManager			m_nodeInfoMgr;	// Info on the components that use each nodeId
	AdjInfoManager			m_adjInfoMgr;	// Manages info on adjacencies between nodeIds (for coloring algorithm)
	GroupManager			m_groupMgr;		// Handles grouping of components for the GUI
	RectManager				m_rectMgr;		// Handles the set of user-defined rectangles
	TextManager				m_textMgr;		// Handles the set of user-defined text labels
	ColorManager			m_colorMgr;		// Handles color assignment to nodeIds

	// Routing algorithm // Don't persist or copy
	std::vector<Element*>	m_targetPins;	// Set of pins to route.
	std::vector<Element*>	m_tmpVec;		// The set of visited points.
	size_t					m_tmpVecSize;	// The number of visited points.
	bool					m_bRouteMinimal;// true ==> don't build tracks between pins that are already connected
};
