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

#include "FootPrint.h"
#include "RectManager.h"
#include "CompDefiner.h"

class CompManager;

// Class to describe a component.
// Wires (i.e. jumpers) and markers also use this class even though they are not true components.

class Component : public FootPrint
{
public:
	Component()	: FootPrint() { Clear(); }
	Component(const Component& o) : FootPrint() { *this = o; }
	void Clear()
	{
		FootPrint::DeAllocate();
		m_id		= 0;
		m_nameStr	= m_valueStr = m_prefixStr = m_typeStr = m_importStr = "";
		m_lyr = m_row = m_col = m_iLabelOffsetRow = m_iLabelOffsetCol = 0;
		m_direction	= 'W';
		m_bIsPlaced	= false;
		m_iPinFlags = 0;
		m_nodeIdPins.clear();
		m_origIdPins1.clear();
		m_origIdPins2.clear();
		m_pinLabels.clear();
		m_pinAligns.clear();
		m_shapes.clear();
	}
	Component(const CompDefiner& definer)	// This method is for building a custom component
	{
		Clear();

		definer.Build(*this);		// Use component definer to make the footprint and shapes
	}
	Component(CompManager* pCompMgr, const RectManager& rectMgr, const ElementGrid& grid, const int& nLyr, const int& nRowMin, const int& nRowMax, const int& nColMin, const int& nColMax)
	{
		Clear();

		BuildTrax(pCompMgr, rectMgr, grid, nLyr, nRowMin, nRowMax, nColMin, nColMax);	// Build method for "tracks" component

		SetDefaultStrings();

		m_lyr = nLyr;
		m_row = nRowMin;
		m_col = nColMin;
		m_bIsPlaced = false;	// Trax are always created from the board
	}
	Component(const std::string& name, const std::string& value, const COMP& eType, std::vector<int>& nodeIdPins)
	{
		Clear();

		BuildDefault(eType);	// Build method for default component

		SetDefaultPinFlags();
		SetDefaultStrings();
		SetDefaultLabelOffsets();

		const size_t numPins = nodeIdPins.size();
		if ( eType == COMP::DIP || eType == COMP::SIP || eType == COMP::SWITCH_ST_DIP )	// Resize DIP/SIP/SWITCH_DIP as needed
		{
			const int reqLength = static_cast<int>( ( eType == COMP::SIP ) ? numPins : numPins / 2 );
			while ( GetCols() < reqLength )	Stretch(true);	// true  ==> grow
			while ( GetCols() > reqLength )	Stretch(false);	// false ==> shrink
		}
		if ( eType == COMP::STRIP_100 || eType == COMP::BLOCK_100 || eType == COMP::BLOCK_200 )
		{
			const int reqLength = static_cast<int>( ( eType == COMP::BLOCK_200 ) ? 1 + 2 * numPins : numPins );
			while ( GetCols() < reqLength )	Stretch(true);	// true  ==> grow
			while ( GetCols() > reqLength )	Stretch(false);	// false ==> shrink
		}
		if ( eType == COMP::SWITCH_ST || eType == COMP::SWITCH_DT )	// Resize non-DIP switches
		{
			const int numPoles  = static_cast<int>( ( eType == COMP::SWITCH_ST ) ? numPins / 2 : numPins / 3 );
			const int reqLength = ( 2 * numPoles ) - 1;
			while ( GetCols() < reqLength )	Stretch(true);	// true  ==> grow
			while ( GetCols() > reqLength )	Stretch(false);	// false ==> shrink
		}
		m_nameStr	= name;
		m_valueStr	= value;
		AllocatePins( numPins );
		std::copy(nodeIdPins.begin(), nodeIdPins.end(), m_nodeIdPins.begin());

		AddDefaultShapes();
	}
	Component& operator=(const Component& o)
	{
		FootPrint::operator=(o);	// Call operator= in base class
		m_id				= o.m_id;
		m_nameStr			= o.m_nameStr;
		m_valueStr			= o.m_valueStr;
		m_prefixStr			= o.m_prefixStr;
		m_typeStr			= o.m_typeStr;
		m_importStr			= o.m_importStr;
		m_lyr				= o.m_lyr;
		m_row				= o.m_row;
		m_col				= o.m_col;
		m_iLabelOffsetRow	= o.m_iLabelOffsetRow;
		m_iLabelOffsetCol	= o.m_iLabelOffsetCol;
		m_direction			= o.m_direction;
		m_bIsPlaced			= o.m_bIsPlaced;
		m_iPinFlags			= o.m_iPinFlags;
		AllocatePins( o.GetNumPins() );
		std::copy(o.m_nodeIdPins.begin(),	o.m_nodeIdPins.end(),	m_nodeIdPins.begin());
		std::copy(o.m_origIdPins1.begin(),	o.m_origIdPins1.end(),	m_origIdPins1.begin());
		std::copy(o.m_origIdPins2.begin(),	o.m_origIdPins2.end(),	m_origIdPins2.begin());
		std::copy(o.m_pinLabels.begin(),	o.m_pinLabels.end(),	m_pinLabels.begin());
		std::copy(o.m_pinAligns.begin(),	o.m_pinAligns.end(),	m_pinAligns.begin());
		CopyShapes( o );
		return *this;
	}
	void ClearNodeIds()
	{
		for (auto& o : m_nodeIdPins)  o = BAD_NODEID;
		for (auto& o : m_origIdPins1) o = BAD_NODEID;
		for (auto& o : m_origIdPins2) o = BAD_NODEID;
	}
	bool IsEqual(const Component& o) const	// Compare persisted info
	{
		bool bOK = FootPrint::operator==(o)
				&& m_id					== o.m_id
				&& m_nameStr			== o.m_nameStr
				&& m_valueStr			== o.m_valueStr
				&& m_prefixStr			== o.m_prefixStr
				&& m_typeStr			== o.m_typeStr
				&& m_importStr			== o.m_importStr
				&& m_lyr				== o.m_lyr
				&& m_row				== o.m_row
				&& m_col				== o.m_col
				&& m_iLabelOffsetRow	== o.m_iLabelOffsetRow
				&& m_iLabelOffsetCol	== o.m_iLabelOffsetCol
				&& m_direction			== o.m_direction
				&& m_bIsPlaced			== o.m_bIsPlaced
				&& m_iPinFlags			== o.m_iPinFlags
				&& GetNumPins()			== o.GetNumPins()
				&& GetNumShapes()		== o.GetNumShapes();
		for (size_t i = 0; i < GetNumPins() && bOK; i++)
		{
			bOK =  m_nodeIdPins[i]	== o.m_nodeIdPins[i]
				&& m_origIdPins1[i]	== o.m_origIdPins1[i]
				&& m_origIdPins2[i] == o.m_origIdPins2[i]
				&& m_pinLabels[i]	== o.m_pinLabels[i]
				&& m_pinAligns[i]	== o.m_pinAligns[i];
		}
		for (size_t i = 0; i < GetNumShapes() && bOK; i++)
		{
			bOK = m_shapes[i] == o.m_shapes[i];
		}
		return bOK;
	}
	bool operator<(const Component& o) const	{ return m_id < o.m_id; }
	bool operator==(const Component& o) const	{ return m_id == o.m_id; }
	~Component()
	{
		m_nodeIdPins.clear();
		m_origIdPins1.clear();
		m_origIdPins2.clear();
		m_pinLabels.clear();
		m_pinAligns.clear();
		m_shapes.clear();
	}
	void SetId(const int& i)										{ m_id = i; }
	void SetNameStr(const std::string& s)							{ m_nameStr = s; }
	void SetValueStr(const std::string& s)							{ m_valueStr = s; }
	void SetPrefixStr(const std::string& s)							{ m_prefixStr = s; }
	void SetTypeStr(const std::string& s)							{ m_typeStr = s; }
	void SetImportStr(const std::string& s)							{ m_importStr = s; }
	void SetNodeId(const size_t& iPinIndex, const int& i)			{ m_nodeIdPins[iPinIndex] = i; }
	void SetOrigId(const int& lyr, const size_t& iPinIndex, const int& i)
	{
		assert( lyr == 0 || lyr == 1 );
		if ( lyr == 0 ) m_origIdPins1[iPinIndex] = i; else m_origIdPins2[iPinIndex] = i;
	}
	void SetPinLabel(const size_t& iPinIndex, const std::string& s)	{ m_pinLabels[iPinIndex] = s; }
	void SetPinAlign(const size_t& iPinIndex, const int& i)			{ m_pinAligns[iPinIndex] = i; }
	void SetShape(const size_t& iShapeIndex, const Shape& o)		{ m_shapes[iShapeIndex] = o; }
	void CopyPinLabels(const Component& o)
	{
		assert( m_pinLabels.size() == o.m_pinLabels.size() );
		assert( m_pinAligns.size() == o.m_pinAligns.size() );
		std::copy(o.m_pinLabels.begin(), o.m_pinLabels.end(), m_pinLabels.begin());
		std::copy(o.m_pinAligns.begin(), o.m_pinAligns.end(), m_pinAligns.begin());
	}
	void CopyShapes(const Component& o)
	{
		AllocateShapes( o.GetNumShapes() );
		std::copy(o.m_shapes.begin(), o.m_shapes.end(), m_shapes.begin());
	}
	void AllocatePins(const size_t numPins)
	{
		m_nodeIdPins.clear();	m_nodeIdPins.resize(numPins, BAD_NODEID);
		m_origIdPins1.clear();	m_origIdPins1.resize(numPins, BAD_NODEID);
		m_origIdPins2.clear();	m_origIdPins2.resize(numPins, BAD_NODEID);
		m_pinLabels.clear();	m_pinLabels.resize(numPins, "");
		m_pinAligns.clear();	m_pinAligns.resize(numPins, Qt::AlignHCenter);
		SetDefaultPinLabels();
	}
	void AllocateShapes(const size_t numShapes)
	{
		m_shapes.clear();	m_shapes.resize(numShapes, Shape());
	}
	void SetLyr(const int& i)										{ m_lyr = i; }
	void SetRow(const int& i)										{ m_row = i; }
	void SetCol(const int& i)										{ m_col = i; }
//	void SetLabelOffsetRow(const int& i)							{ m_iLabelOffsetRow = i; }
//	void SetLabelOffsetCol(const int& i)							{ m_iLabelOffsetCol = i; }
	void SetDirection(const char& d)								{ m_direction = d; }
	void SetIsPlaced(const bool& b)									{ m_bIsPlaced = b; }
	void SetPinFlags(const uchar& i)								{ m_iPinFlags = i; }
	void AddOne(const Shape& s)										{ m_shapes.push_back(s); }
	void AddTwo(const Shape& s)	// Adds the shape twice.  Once with fill only, and once with line only
	{
		Shape tmp(s);
		tmp.SetDrawFill(true);	tmp.SetDrawLine(false);	m_shapes.push_back(tmp);
		tmp.SetDrawFill(false);	tmp.SetDrawLine(true);	m_shapes.push_back(tmp);
	}
	bool				GetIsTemplate() const						{ return GetId() == BAD_COMPID; }
	const int&			GetId() const								{ return m_id; }
	const std::string&	GetNameStr() const							{ return m_nameStr; }
	const std::string&	GetValueStr() const							{ return m_valueStr; }
	const std::string&	GetPrefixStr() const						{ return m_prefixStr; }
	const std::string&	GetTypeStr() const							{ return m_typeStr; }
	const std::string&	GetImportStr() const						{ return m_importStr; }
	size_t				GetNumPins() const							{ return m_nodeIdPins.size(); }
	const int&			GetNodeId(const size_t& iPinIndex) const	{ return m_nodeIdPins[iPinIndex]; }
	const int&			GetOrigId(const int& lyr, const size_t& iPinIndex) const
	{
		return ( lyr == 0 ) ? m_origIdPins1[iPinIndex] : m_origIdPins2[iPinIndex];
	}
	const std::string&	GetPinLabel(const size_t& iPinIndex) const	{ return m_pinLabels[iPinIndex]; }
	const int&			GetPinAlign(const size_t& iPinIndex) const	{ return m_pinAligns[iPinIndex]; }
	size_t				GetNumShapes() const						{ return m_shapes.size(); }
	const Shape&		GetShape(const size_t& iShapeIndex) const	{ return m_shapes[iShapeIndex]; }
	const int&			GetLyr() const								{ return m_lyr; }
	const int&			GetRow() const								{ return m_row; }
	const int&			GetCol() const								{ return m_col; }
//	const int&			GetLabelOffsetRow() const					{ return m_iLabelOffsetRow; }
//	const int&			GetLabelOffsetCol() const					{ return m_iLabelOffsetCol; }
	const char&			GetDirection() const						{ return m_direction; }
	const bool&			GetIsPlaced() const							{ return m_bIsPlaced; }
	const uchar&		GetPinFlags() const							{ return m_iPinFlags; }
	const std::vector<Shape>&	GetShapes() const					{ return m_shapes; }

	// Helpers for labels
	void SetDefaultLabelOffsets();
	void GetLabelOffsets(int& offsetRow, int& offsetCol) const;			// w.r.t. screen, not comp rotation
	void MoveLabelOffsets(const int& deltaRow, const int& deltaCol);	// w.r.t. screen, not comp rotation
	void HandleLegacyLabelOffsets();	// For old VRT files

	void GetSafeBounds(double& L, double& R, double& T, double& B) const
	{
		L = T =  DBL_MAX;
		R = B = -DBL_MAX;
		double l,r,t,b;
		for (auto& o : m_shapes)
		{
			o.GetSafeBounds(l,r,t,b);
			L = std::min(L,l);	T = std::min(T,t);
			R = std::max(R,r);	B = std::max(B,b);
		}
	}
	std::string GetFullTypeStr() const		// For SIP/DIP types, append the number of pins
	{
		std::string str = GetTypeStr();
		if ( GetType() == COMP::DIP || GetType() == COMP::SIP )
		{
			char buffer[32] = {'\0'};
			sprintf(buffer, "%d", (int) GetNumPins());
			str += std::string(buffer);		// e.g. "DIP16"
		}
		return str;
	}
	std::string GetFullImportStr() const	// For SIP/DIP/SWITCH/STRIP/BLOCK types, append the number of pins
	{
		std::string str = GetImportStr();
		if ( GetType() == COMP::DIP || GetType() == COMP::SIP ||
			 GetType() == COMP::SWITCH_DT || GetType() == COMP::SWITCH_ST || GetType() == COMP::SWITCH_ST_DIP ||
			 GetType() == COMP::STRIP_100 || GetType() == COMP::BLOCK_100 || GetType() == COMP::BLOCK_200 )
		{
			char buffer[32] = {'\0'};
			sprintf(buffer, "%d", (int) GetNumPins());
			str += std::string(buffer);		// e.g. "DIP16"
		}
		return str;
	}
	// Helpers (account for component direction)
	const int&			GetCompRows() const	{ return GetRows( GetDirection() ); }
	const int&			GetCompCols() const	{ return GetCols( GetDirection() ); }
	int					GetLastRow() const	{ return GetRow() + GetCompRows() - 1; }
	int					GetLastCol() const	{ return GetCol() + GetCompCols() - 1; }
	const CompElement*	GetCompElement(const int& compRow, const int& compCol) const
	{
		return FootPrint::Get(0, compRow, compCol, (char)GetDirection());
	}
	bool GetHasNodeId(int nodeId) const
	{
		for (const auto& i : m_nodeIdPins) if ( i == nodeId ) return true;
		return false;
	}
	void Rotate(const bool& bClockWise)
	{
		switch( GetDirection() )	// Component direction: 'W','E','N','S'
		{
			case 'W': return SetDirection( bClockWise ? 'N' : 'S' );
			case 'E': return SetDirection( bClockWise ? 'S' : 'N' );
			case 'N': return SetDirection( bClockWise ? 'E' : 'W' );
			case 'S': return SetDirection( bClockWise ? 'W' : 'E' );
		}
	}
	bool GetHasAssignedPins() const	// Check if any pins have a nodeId or pin label set
	{
		for (size_t i = 0; i < GetNumPins(); i++)
		{
			if ( m_nodeIdPins[i] != BAD_NODEID ) return true;
			if ( m_pinLabels[i]  != GetDefaultPinLabel(i) ) return true;
			if ( m_pinAligns[i]  != GetDefaultPinAlign(i, GetNumPins(), GetType()) ) return true;
		}
		return false;
	}
	bool CanStretch(const bool& bGrow) const
	{
		if ( !FootPrint::CanStretch(bGrow) ) return false;
		switch( GetType() )
		{
			case COMP::SIP:
			case COMP::DIP:
			case COMP::STRIP_100:
			case COMP::BLOCK_100:
			case COMP::BLOCK_200:
			case COMP::SWITCH_ST:
			case COMP::SWITCH_DT:
			case COMP::SWITCH_ST_DIP:	return !GetHasAssignedPins();
			default:					return true;
		}
	}
	void Stretch(const bool& bGrow)
	{
		FootPrint::Stretch(bGrow);

		// Rebuild the shapes list
		m_shapes.clear();
		AddDefaultShapes();

		switch( GetType() )
		{
			case COMP::SIP:				return AllocatePins( GetCols() );
			case COMP::DIP:				return AllocatePins( 2 * GetCols() );
			case COMP::STRIP_100:		return AllocatePins( GetCols() );
			case COMP::BLOCK_100:		return AllocatePins( GetCols() );
			case COMP::BLOCK_200:		return AllocatePins( ( GetCols() - 1 ) / 2 );
			case COMP::SWITCH_ST:		return AllocatePins( GetCols() + 1 );
			case COMP::SWITCH_DT:		return AllocatePins( 3 * ( GetCols() + 1 ) / 2 );
			case COMP::SWITCH_ST_DIP:	return AllocatePins( 2 * GetCols() );
			default:					return;
		}
	}
	void StretchWidth(const bool& bGrow)
	{
		FootPrint::StretchWidth(bGrow);

		// Rebuild the shapes list
		m_shapes.clear();
		AddDefaultShapes();
	}
	void SetDefaultPinLabels()
	{
		for (size_t i = 0; i < GetNumPins(); i++)
		{
			m_pinLabels[i] = GetDefaultPinLabel(i);
			m_pinAligns[i] = GetDefaultPinAlign(i, GetNumPins(), GetType());
		}
	}
	Rect GetFootprintRect() const
	{
		return Rect(GetRow(), GetLastRow(), GetCol(), GetLastCol());
	}
	bool GetIsTrueComp() const	// A true component has pins and "owns" the nodeIds on them
	{
		switch( GetType() )
		{
			case COMP::MARK:
			case COMP::WIRE:
			case COMP::TRACKS:	return false;
			default:			return true;
		}
	}
	void SetFillColor(const MyRGB& r)	// Gives all shapes the same fill color
	{
		for (auto& o : m_shapes) o.SetFillColor(r);
	}
	MyRGB GetNewColor() const	// returns an un-used color
	{
		for (int iColor = 1; iColor <= 0xFFFFFF; iColor++)	// Black is used for outlines so start at 1
		{
			bool bOK(true);
			MyRGB tmp(iColor);
			for (auto& o : m_shapes)
				if ( o.GetFillColor() == tmp ) { bOK = false; break; }
			if ( bOK ) return tmp;
		}
		assert(0);
		return MyRGB(0x000000);
	}
	// Merge interface functions
	virtual void UpdateMergeOffsets(MergeOffsets& o) override
	{
		FootPrint::UpdateMergeOffsets(o);	// Call UpdateMergeOffsets in base class

		if ( m_id != BAD_COMPID && m_id != TRAX_COMPID ) o.deltaCompId  = std::max(o.deltaCompId, m_id + 1);
//		o.deltaLyr = std::max(o.deltaLyr, m_lyr + GetCompLyrs() + 1);
		o.deltaRow = std::max(o.deltaRow, m_row + GetCompRows() + 1);
//		o.deltaCol = std::max(o.deltaCol, m_col + GetCompCols() + 1);
		for (size_t i = 0; i < GetNumPins(); i++)
		{
			if ( m_nodeIdPins[i]  != BAD_NODEID ) o.deltaNodeId = std::max(o.deltaNodeId, m_nodeIdPins[i]  + 1);
			if ( m_origIdPins1[i] != BAD_NODEID ) o.deltaNodeId = std::max(o.deltaNodeId, m_origIdPins1[i] + 1);
			if ( m_origIdPins2[i] != BAD_NODEID ) o.deltaNodeId = std::max(o.deltaNodeId, m_origIdPins2[i] + 1);
		}
	}
	virtual void ApplyMergeOffsets(const MergeOffsets& o) override
	{
		FootPrint::ApplyMergeOffsets(o);	// Call ApplyMergeOffsets in base class

		if ( m_id != BAD_COMPID && m_id != TRAX_COMPID) m_id += o.deltaCompId;
		m_lyr += o.deltaLyr;
		m_row += o.deltaRow;
		m_col += o.deltaCol;
		for (size_t i = 0; i < GetNumPins(); i++)
		{
			if ( m_nodeIdPins[i]  != BAD_NODEID ) m_nodeIdPins[i]  += o.deltaNodeId;
			if ( m_origIdPins1[i] != BAD_NODEID ) m_origIdPins1[i] += o.deltaNodeId;
			if ( m_origIdPins2[i] != BAD_NODEID ) m_origIdPins2[i] += o.deltaNodeId;
		}
	}
	void SetDefaultPinFlags();
	void SetDefaultStrings(bool bForce = true)
	{
		if ( GetPrefixStr().empty() || bForce )
			SetPrefixStr( GetDefaultPrefixStr( GetType() ) );
		if ( GetTypeStr().empty() || bForce )
			SetTypeStr( GetDefaultTypeStr( GetType() ) );
		if ( GetImportStr().empty() || bForce || GetType() != COMP::CUSTOM )
			SetImportStr( GetDefaultImportStr( GetType() ) );
	}
	void AddDefaultShapes();
	void SetDefaultColor();
	// Persist interface functions
	virtual void Load(DataStream& inStream) override
	{
		FootPrint::Load(inStream);	// Load() base class
		inStream.Load(m_id);
		if ( m_id == -2 ) SetType(COMP::TRACKS);	// Needed to stop old VRTs from crashing
		inStream.Load(m_nameStr);
		inStream.Load(m_valueStr);
		if ( inStream.GetVersion() >= VRT_VERSION_19 )
			inStream.Load(m_prefixStr);				// Added in VRT_VERSION_19
		if ( inStream.GetVersion() >= VRT_VERSION_18 )
		{
			inStream.Load(m_typeStr);				// Added in VRT_VERSION_18
			inStream.Load(m_importStr);				// Added in VRT_VERSION_18
		}
		m_lyr = 0;
		if ( inStream.GetVersion() >= VRT_VERSION_34 )
			inStream.Load(m_lyr);					// Added in VRT_VERSION_34
		inStream.Load(m_row);
		inStream.Load(m_col);
		inStream.Load(m_iLabelOffsetRow);
		inStream.Load(m_iLabelOffsetCol);
		if ( inStream.GetVersion() < VRT_VERSION_22 )	// Units changed from 1/4 square to 1/16 square in VRT_VERSION_22
		{
			m_iLabelOffsetRow *= 4;
			m_iLabelOffsetCol *= 4;
		}
		inStream.Load(m_direction);
		inStream.Load(m_bIsPlaced);
		if ( inStream.GetVersion() >= VRT_VERSION_19 )
			inStream.Load(m_iPinFlags);				// Added in VRT_VERSION_19
		unsigned int numPins(0);
		inStream.Load(numPins);
		AllocatePins(numPins);
		for (unsigned int i = 0; i < numPins; i++)
		{
			inStream.Load(m_nodeIdPins[i]);
			inStream.Load(m_origIdPins1[i]);
			if ( inStream.GetVersion() >= VRT_VERSION_34 )
				inStream.Load(m_origIdPins2[i]);	// Added in VRT_VERSION_34
			if ( inStream.GetVersion() >= VRT_VERSION_7 )
				inStream.Load(m_pinLabels[i]);		// Added in VRT_VERSION_7
			if ( inStream.GetVersion() >= VRT_VERSION_30 )
				inStream.Load(m_pinAligns[i]);		// Added in VRT_VERSION_30
		}
		if ( inStream.GetVersion() >= VRT_VERSION_18 )
		{
			unsigned int numShapes(0);
			inStream.Load(numShapes);				// Added in VRT_VERSION_18
			AllocateShapes(numShapes);
			for (unsigned int i = 0; i < numShapes; i++)
				m_shapes[i].Load(inStream);			// Added in VRT_VERSION_18
		}
		if ( inStream.GetVersion() < VRT_VERSION_31 )
			HandleLegacyLabelOffsets();
		// Try to fix any missing definitions
		SetDefaultPinFlags();
		SetDefaultStrings(false);	// false ==> only set empty (m_prefixStr, m_guiStr, m_importStr)
		AddDefaultShapes();
	}
	virtual void Save(DataStream& outStream) override
	{
		FootPrint::Save(outStream);	// Save() base class
		outStream.Save(m_id);
		outStream.Save(m_nameStr);
		outStream.Save(m_valueStr);
		outStream.Save(m_prefixStr);			// Added in VRT_VERSION_19
		outStream.Save(m_typeStr);				// Added in VRT_VERSION_18
		outStream.Save(m_importStr);			// Added in VRT_VERSION_18
		outStream.Save(m_lyr);					// Added in VRT_VERSION_34
		outStream.Save(m_row);
		outStream.Save(m_col);
		outStream.Save(m_iLabelOffsetRow);
		outStream.Save(m_iLabelOffsetCol);
		outStream.Save(m_direction);
		outStream.Save(m_bIsPlaced);
		outStream.Save(m_iPinFlags);			// Added in VRT_VERSION_19
		const unsigned int numPins = static_cast<unsigned int>( GetNumPins() );
		outStream.Save(numPins);
		for (unsigned int i = 0; i < numPins; i++)
		{
			outStream.Save(m_nodeIdPins[i]);
			outStream.Save(m_origIdPins1[i]);
			outStream.Save(m_origIdPins2[i]);	// Added in VRT_VERSION_34
			outStream.Save(m_pinLabels[i]);		// Added in VRT_VERSION_7
			outStream.Save(m_pinAligns[i]);		// Added in VRT_VERSION_30
		}
		const unsigned int numShapes = static_cast<unsigned int>( GetNumShapes() );
		outStream.Save(numShapes);				// Added in VRT_VERSION_18
		for (unsigned int i = 0; i < numShapes; i++)
			m_shapes[i].Save(outStream);		// Added in VRT_VERSION_18
	}
private:
	int							m_id;				// Component ID.  (BAD_COMPID ==> a component template)
	std::string					m_nameStr;			// Name label
	std::string					m_valueStr;			// Value label
	std::string					m_prefixStr;		// The prefix for new components (overridden for CUSTOM components).
	std::string					m_typeStr;			// The footprint type (overridden for CUSTOM components).
	std::string					m_importStr;		// Protel/Tango/OrCAD2 footprint name. Only for CUSTOM components !!!
	std::vector<int>			m_nodeIdPins;		// NodeIds of the pins
	std::vector<int>			m_origIdPins1;		// NodeIds under the pins BEFORE placement (1st layer)
	std::vector<int>			m_origIdPins2;		// NodeIds under the pins BEFORE placement (2nd Layer)
	std::vector<std::string>	m_pinLabels;		// Pin labels
	std::vector<int>			m_pinAligns;		// Pin label alignments (Qt::AlignLeft,Qt::AlignRight,Qt::AlignHCenter)
	std::vector<Shape>			m_shapes;			// For rendering components. Coordinates are RELATIVE to footprint centre.
	uchar						m_iPinFlags;		// 1 ==> PIN_RECT, 2 ==> PIN_LABELS
	// Current placement in board
	int							m_lyr;				// Board layer for component
	int							m_row;				// Board row for top-left element of footprint
	int							m_col;				// Board col for top-left element of footprint
	int							m_iLabelOffsetRow;	// Label offset in units of 1/16 of a grid square
	int							m_iLabelOffsetCol;	// Label offset in units of 1/16 of a grid square
	char						m_direction;		// Component orienation:  'W', 'E', 'N', 'S'
	bool						m_bIsPlaced;		// true ==> placed on board,  false ==> floating
};
