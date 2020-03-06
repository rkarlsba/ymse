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

#include "Persist.h"

// A "TrackElement" object provides a local description of the track pattern at a grid point.
// It stores the node ID at a grid point, along with an 8-bit code describing the
// connections to the 8 surrounding points.
// This connection code solves the "competing diagonals" problem.
// So if there are 4 grid points painted with two node IDs (A and B) as follows
//    A B
//    B A
// then setting the code appropriately at each point will allow
// a single diagonal connection to exist, either (A-A) or (B-B).

const int NUM_NBRS = 9;	// (NBR_L to NBR_LB) + NBR_X

// Indexes for the 8 neighbour elements in the same layer, starting on the left and going clockwise
const int	NBR_L(0), NBR_LT(1), NBR_T(2), NBR_RT(3),	// Left,  Left-Top,     Top, Right-Top,
			NBR_R(4), NBR_RB(5), NBR_B(6), NBR_LB(7);	// Right, Right-Bottom, Bottom, Left-Bottom
const int	NBR_X(8);	// Index for the neighbour element in the layer above/below

static int	Opposite(int NBR)	// Helper to get opposite neighbour index
{
	return ( NBR == NBR_X ) ? NBR_X : (  ( NBR + 4 ) % 8 );
}

// Functions for mapping NBR indices to "code bits" and manipulating them
static bool ReadCodeBit(const int& NBR, const int& iCode)	{ return ( iCode & (1<<NBR) ) != 0; }
static void SetCodeBit(const int& NBR, int& iCode)			{ iCode |=  (1<<NBR); }
static void ClearCodeBit(const int& NBR, int& iCode)		{ iCode &= ~(1<<NBR); }
static void ToggleCodeBit(const int& NBR, int& iCode)		{ iCode ^=  (1<<NBR); }
const int CODEBITS_LYR = 0xFF;
const int CODEBITS_ALL = 0x1FF;

// Flag is a bitfield describing the status of the nodeId at point.
// USERSET points will not have their nodeId modified during the auto-routing.
// The algorithm will change the flag from USERSET to AUTOKEPT if it thinks the point is useful.
// On hitting "Tidy", only AUTOKEPT and AUTOSET points are kept, and any USERSET
// points will be wiped (if they are not component pins).
const char USERSET	= 1;					// ==> user assigned the nodeId
const char AUTOSET	= 2;					// ==> routing algorithm assigned the nodeId
const char AUTOKEPT	= (USERSET | AUTOSET);	// ==> user assigned the nodeId, and routing algorithm agrees it is useful
const char VEROSET	= 4;					// ==> auto assigned to create Vero strips
const char RECTSET	= 8;					// ==> is within a user-defined rect

const int BAD_NODEID = 0;	// Invalid node ID (i.e. netlist ID)

class TrackElement : public Persist, public Merge
{
	friend class Element;
public:
	TrackElement() {}
	TrackElement(const TrackElement& o) { *this = o; }
	~TrackElement() {}
	TrackElement& operator=(const TrackElement& o)
	{
		m_nodeId	= o.m_nodeId;
		m_iCode		= o.m_iCode;
		m_flag		= o.m_flag;
		return *this;
	}
	bool operator==(const TrackElement& o) const	// Compare persisted info
	{
		return	m_nodeId	== o.m_nodeId
			&&	m_iCode		== o.m_iCode
			&&	m_flag		== o.m_flag;
	}
	bool operator!=(const TrackElement& o) const
	{
		return !(*this == o);
	}
	void SetNodeId(const int& i)	{ m_nodeId	= i; }
	void SetCode(const int& i)		{ m_iCode	= i; }
	void SetFlag(const char& i)		{ m_flag	= i; }

	const int&	GetNodeId() const	{ return m_nodeId; }
	const int&	GetCode() const		{ return m_iCode; }
	const char&	GetFlag() const		{ return m_flag; }

	// Connectivity helpers
	void SetUsed(const int& iNbr, const bool& b)
	{
		if ( b ) SetCodeBit(iNbr, m_iCode); else ClearCodeBit(iNbr, m_iCode);
	}
	bool GetUsed(const int& iNbr) const
	{
		return ReadCodeBit(iNbr, m_iCode);
	}
	bool IsClash(const int& nodeId) const
	{
		return nodeId != BAD_NODEID && m_nodeId != BAD_NODEID && nodeId != m_nodeId;
	}
	int GetPerimeterCode(const bool& bDiagsOK, const bool& bMinDiags)	const // Helper for the GUI "blobs"
	{
		int iCode = GetCode() & CODEBITS_LYR;	// Take a copy of the connection code, and restrict to same-layer neighbours

		// Enforce any restrictions on diagonal connections acccording to the GUI options on diagonals
		const bool	bL( ReadCodeBit(NBR_L, iCode) ),	bT( ReadCodeBit(NBR_T, iCode) ),
					bR( ReadCodeBit(NBR_R, iCode) ),	bB( ReadCodeBit(NBR_B, iCode) );
		if ( ( !bDiagsOK && !(bL && bT) ) || ( bMinDiags && (bL != bT) ) ) ClearCodeBit(NBR_LT, iCode);
		if ( ( !bDiagsOK && !(bR && bT) ) || ( bMinDiags && (bR != bT) ) ) ClearCodeBit(NBR_RT, iCode);
		if ( ( !bDiagsOK && !(bL && bB) ) || ( bMinDiags && (bL != bB) ) ) ClearCodeBit(NBR_LB, iCode);
		if ( ( !bDiagsOK && !(bR && bB) ) || ( bMinDiags && (bR != bB) ) ) ClearCodeBit(NBR_RB, iCode);
		return iCode;
	}

	// Flag Helpers
	bool ReadFlagBits(const char& i) const	{ return ( m_flag & i ) != 0; }
	void SetFlagBits(const char& i)			{ m_flag |=  i; }
	void ClearFlagBits(const char& i)		{ m_flag &= ~i; }

	// Merge interface functions
	virtual void UpdateMergeOffsets(MergeOffsets& o) override
	{
		if ( m_nodeId != BAD_NODEID  ) o.deltaNodeId = std::max(o.deltaNodeId, m_nodeId + 1);
	}
	virtual void ApplyMergeOffsets(const MergeOffsets& o) override
	{
		if ( m_nodeId != BAD_NODEID ) m_nodeId += o.deltaNodeId;
	}
	void Merge(const TrackElement& o)
	{
		*this = o;
	}
	// Persist interface functions
	virtual void Load(DataStream& inStream) override
	{
		inStream.Load(m_nodeId);
		inStream.Load(m_iCode);
		inStream.Load(m_flag);
		if ( inStream.GetVersion() < VRT_VERSION_11 )
			ConvertLegacyFlag();
	}
	virtual void Save(DataStream& outStream) override
	{
		outStream.Save(m_nodeId);
		outStream.Save(m_iCode);
		outStream.Save(m_flag);
	}
private:
	void ConvertLegacyFlag()	// Map legacy flag values to new bitfield based values
	{
		switch( m_flag )
		{
			case 0:	m_flag = USERSET;	break;
			case 1: m_flag = AUTOKEPT;	break;
			case 2: m_flag = AUTOSET;	break;
			case 3: m_flag = VEROSET;	break;
			default: assert(0);
		}
	}
private:
	int		m_nodeId = BAD_NODEID;	// The netlist value assigned to the element (or BAD_NODEID if not set)
	int		m_iCode	 = 0;			// An 8-bit code describing connections to the 8 neighbours.  Bit set ==> connection used
	char	m_flag	 = USERSET;		// For routing:  USERSET/AUTOKEPT/AUTOSET/VEROSET/RECTSET
};
