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

#include "CompElement.h"	// For BAD_NODEID

// Class to record the adjacency info for a NodeId on the board (NOT floating components).
// Every board element has 8 adjacent elements regardless of whether diagonals are allowed.

class AdjInfo
{
public:
	AdjInfo(int nodeId = BAD_NODEID) : m_nodeId(nodeId) {}
	~AdjInfo()					{ m_adjCount.clear(); }
	AdjInfo(const AdjInfo& o)	{ *this = o; }
	AdjInfo& operator=(const AdjInfo& o)
	{
		m_nodeId = o.m_nodeId;
		m_adjCount.clear();
		m_adjCount.insert(o.m_adjCount.begin(), o.m_adjCount.end());
		return *this;
	}
	void		SetNodeId(int i)			{ m_nodeId = i; }
	const int&	GetNodeId() const			{ return m_nodeId; }
	bool		GetHasAdj(int nodeId) const	{ return m_adjCount.find(nodeId) != m_adjCount.end(); }
	void IncCount(int nodeId)
	{
		assert( m_nodeId != nodeId );	// Sanity check.  Can't be adjacent to self
		const auto iter = m_adjCount.find(nodeId);
		if ( iter != m_adjCount.end() ) iter->second++; else m_adjCount[nodeId] = 1;
	}
	void DecCount(int nodeId)
	{
		assert( m_nodeId != nodeId );	// Sanity check.  Can't be adjacent to self
		const auto iter = m_adjCount.find(nodeId);
		if ( iter != m_adjCount.end() ) { iter->second--; if ( iter->second == 0 ) m_adjCount.erase(iter); }
	}
private:
	int							m_nodeId;	// The NodeID of interest
	std::unordered_map<int,int>	m_adjCount;	// Map of adjacent NodeID to element count
};
