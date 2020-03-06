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

#include "CompManager.h"

// Class to record info for a particular NodeId used by components (including floating components)

class NodeInfo
{
public:
	NodeInfo(int nodeId = BAD_NODEID)
	: m_bComplete(false)
	, m_iCost(UINT_MAX)
	, m_nodeId(nodeId)
	, m_minRow(INT_MAX)
	, m_minCol(INT_MAX)
	, m_maxRow(0)
	, m_maxCol(0)
	{}
	~NodeInfo()					{ m_compIds.clear(); }
	NodeInfo(const NodeInfo& o)	{ *this = o; }
	NodeInfo& operator=(const NodeInfo& o)
	{
		m_bComplete	= o.m_bComplete;
		m_iCost		= o.m_iCost;
		m_nodeId	= o.m_nodeId;
		m_minRow	= o.m_minRow;
		m_maxRow	= o.m_maxRow;
		m_minCol	= o.m_minCol;
		m_maxCol	= o.m_maxCol;
		m_compIds.resize(o.m_compIds.size());
		std::copy(o.m_compIds.begin(), o.m_compIds.end(), m_compIds.begin());
		return *this;
	}
	void				SetComplete(bool b)		{ m_bComplete = b; }
	void				SetCost(unsigned int i)	{ m_iCost = i; }
	void				SetNodeId(int i)		{ m_nodeId = i; }
	const bool&			GetComplete() const		{ return m_bComplete; }
	const unsigned int&	GetCost() const			{ return m_iCost; }
	const int&			GetNodeId() const		{ return m_nodeId; }
	size_t		GetNumComps() const				{ return m_compIds.size(); }
	const int&	GetCompId(size_t i) const		{ return m_compIds[i]; }
	bool		GetHasCompId(int compId) const	{ return std::find(m_compIds.begin(), m_compIds.end(), compId) != m_compIds.end(); }
	void AddComp(const Component& comp)
	{
		assert( comp.GetIsTrueComp() );	// A true component has pins and "owns" the nodeIds on them
		if ( !comp.GetHasNodeId(m_nodeId) ) return;	// No such NodeId on component
		if ( GetHasCompId( comp.GetId() ) ) return;	// compId already in the list
		m_compIds.push_back( comp.GetId() );
	}
	void RemoveComp(const Component& comp)
	{
		assert( comp.GetIsTrueComp() );	// A true component has pins and "owns" the nodeIds on them
		if ( !comp.GetHasNodeId(m_nodeId) ) return;	// No such NodeId on component
		auto iter = std::find(m_compIds.begin(), m_compIds.end(), comp.GetId() );
		if ( iter == m_compIds.end() ) return;		// compId not in the list
		*iter = m_compIds.back();					// Swap with last item ...
		m_compIds.pop_back();						// ... then erase it
	}
	// Following are for ranking nodes based on the spread of component pins
	int	 GetWidth() const	{ assert( m_maxCol >= m_minCol );	return m_maxCol - m_minCol; }
	int	 GetHeight() const	{ assert( m_maxRow >= m_minRow );	return m_maxRow - m_minRow; }
	int	 GetArea() const	{ return GetWidth() * GetHeight(); }
	int	 GetLength() const	{ return std::max(GetWidth(), GetHeight()); }
	bool GetHasFloatingComp(CompManager& compMgr) const
	{
		for (size_t i = 0, iSize = GetNumComps(); i < iSize; i++)
		{
			const Component& comp = compMgr.GetComponentById( GetCompId(i) );
			assert( comp.GetIsTrueComp() );
			if ( !comp.GetIsPlaced() ) return true;
		}
		return false;
	}
	void CalcBoundingRectangle(CompManager& compMgr, bool bIgnoreFloating)
	{
		m_minRow = m_minCol = INT_MAX;
		m_maxRow = m_maxCol = 0;
		bool bHavePins(false);	// Remains false if no component pins have the nodeId
		for (size_t i = 0, iSize = GetNumComps(); i < iSize; i++)	// Loop all comps with the nodeId
		{
			Component& comp = compMgr.GetComponentById( GetCompId(i) );
			if ( bIgnoreFloating && !comp.GetIsPlaced() ) continue;	// Skip floating components if needed

			// Loop footprint and find the relevant pin locations
			for (int iRow = 0, rows = comp.GetCompRows(); iRow < rows; iRow++)
			for (int iCol = 0, cols = comp.GetCompCols(); iCol < cols; iCol++)
			{
				const size_t pinIndex = comp.GetCompElement(iRow, iCol)->GetPinIndex();
				if ( pinIndex == BAD_PININDEX ) continue;
				if ( comp.GetNodeId(pinIndex) != GetNodeId() ) continue;
				m_minRow = std::min(m_minRow, comp.GetRow() + iRow);
				m_maxRow = std::max(m_maxRow, comp.GetRow() + iRow);
				m_minCol = std::min(m_minCol, comp.GetCol() + iCol);
				m_maxCol = std::max(m_maxCol, comp.GetCol() + iCol);
				bHavePins = true;
			}
		}
		if ( !bHavePins )
		{
			m_minRow = m_minCol = 0;
			m_maxRow = m_maxCol = INT_MAX;
		}
	}
private:
	bool				m_bComplete;	// true ==> All component pins with the nodeId are connected (before routing)
	unsigned int		m_iCost;		// zero ==> All component pins with the nodeId are connected (after routing)
	int					m_nodeId;		// The NodeID
	int					m_minRow;		// For the bounding rectangle based on comp pins
	int					m_minCol;		// For the bounding rectangle based on comp pins
	int					m_maxRow;		// For the bounding rectangle based on comp pins
	int					m_maxCol;		// For the bounding rectangle based on comp pins
	std::vector<int>	m_compIds;		// ComponentIDs of components (not markers/wires) that use the nodeId
};
