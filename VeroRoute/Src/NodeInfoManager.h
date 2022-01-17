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

#include "NodeInfo.h"
#include "AdjInfoManager.h"

// Manager class to handle info on all nodeIds used by components (including floating components)

class NodeInfoManager
{
public:
	NodeInfoManager()	{}
	~NodeInfoManager()	{ DeAllocate(); }
	void DeAllocate()
	{
		for (auto& pNodeInfo : m_list) delete pNodeInfo;
		m_list.clear();
	}
	size_t		GetSize() const	{ return m_list.size(); }
	NodeInfo*	GetAt(size_t i)	{ return m_list[i]; }
	void AddComp(const Component& comp)	// Call this after creating a comp, or calling comp.SetNodeId()
	{
		if ( !comp.GetIsTrueComp() ) return;
		for (size_t n = 0, N = comp.GetNumPins(); n < N; n++)	// Loop pins on the component
		{
			const int& nodeId = comp.GetNodeId(n);
			// Find the relevant list object for the nodeId, or create a new one if it doesn't exist
			NodeInfo* pFound(nullptr);
			for (auto& pNodeInfo : m_list)
				if ( pNodeInfo->GetNodeId() == nodeId ) { pFound = pNodeInfo; break; }
			if ( !pFound ) { pFound = new NodeInfo(nodeId); m_list.push_back(pFound); }
			pFound->AddComp(comp);
		}
	}
	void RemoveComp(const Component& comp)	// Call this before destroying a comp, or calling comp.SetNodeId()
	{
		if ( !comp.GetIsTrueComp() ) return;
		for (size_t n = 0, N = comp.GetNumPins(); n < N; n++)	// Loop pins on the component
		{
			const int& nodeId = comp.GetNodeId(n);
			// Find the relevant list member for the nodeId and remove the comp from it
			for (auto& pNodeInfo : m_list)
				if ( pNodeInfo->GetNodeId() == nodeId ) { pNodeInfo->RemoveComp(comp); break; }
		}
	}
	int GetNewNodeId(const AdjInfoManager& m_adjInfoManager)	// Returns lowest nodeId not used by a component or the board
	{
		// "this" has info on nodeIds use by components.
		// m_adjInfoManager has info on nodeIds on the board.

		const size_t iSize = GetSize();
		if ( iSize > 0 ) SortByLowestNodeId();	// Sort "this" nodeIds by increasing value
		const int iMaxCompNodeId = ( iSize == 0 ) ? 0 : m_list[iSize-1]->GetNodeId();
		if ( iSize > 0 )	// We could skip this whole "if".  It's just an optimisation
		{
			// First try to use a zombie nodeId in the list (i.e. one with no comps) if one exists
			for (size_t i = 0; i < iSize; i++)
			{
				const int nodeId = m_list[i]->GetNodeId();
				if ( nodeId != BAD_NODEID && !m_adjInfoManager.GetNodeIdExists(nodeId) && m_list[i]->GetNumComps() == 0 ) return nodeId;
			}
			// Then check for gaps between nodeIds in the list
			for (size_t i = 0; i < iSize-1; i++)
			{
				const int iTestNodeId = m_list[i]->GetNodeId() + 1;
				if ( !m_adjInfoManager.GetNodeIdExists(iTestNodeId) && iTestNodeId < m_list[i+1]->GetNodeId() ) return iTestNodeId;	// Use gap between nodeIds
			}
		}
		// Use a new nodeId that is not used by either the board or the components
		for (int nodeId = iMaxCompNodeId + 1; nodeId < INT_MAX; nodeId++)
			if ( nodeId != BAD_NODEID && !m_adjInfoManager.GetNodeIdExists(nodeId) ) return nodeId;
		assert(0);	// We've run out of nodeIds.  Unlikely to happen.
		return BAD_NODEID;
	}
	void CalcBoundingRectangles(CompManager& compMgr, bool bIgnoreFloating)
	{
		for (auto& pNodeInfo : m_list) pNodeInfo->CalcBoundingRectangle(compMgr, bIgnoreFloating);
	}
	// Sort functions
	void SortByLowestDifficulty(CompManager& compMgr)
	{
		CalcBoundingRectangles(compMgr, true);	// true ==> Ignore floating components
		std::stable_sort(m_list.begin(), m_list.end(), HasLowerDifficulty());
	}
private:
	void SortByLowestNodeId()	{ std::stable_sort(m_list.begin(), m_list.end(), HasLowerNodeId()); }
	struct HasLowerDifficulty
	{
		bool operator()(const NodeInfo* pA, const NodeInfo* pB) const
		{
			// Put complete things on the end of the list
			if ( pA->GetComplete() && !pB->GetComplete() ) return false;
			if ( pB->GetComplete() && !pA->GetComplete() ) return true;
			if ( pA->GetArea()		!= pB->GetArea() )	 return ( pA->GetArea()   < pB->GetArea()   );
			if ( pA->GetLength()	!= pB->GetLength() ) return ( pA->GetLength() < pB->GetLength() );
			return pA->GetNumComps() < pB->GetNumComps();
		}
	};
	struct HasLowerNodeId
	{
		bool operator()(const NodeInfo* pA, const NodeInfo* pB) const
		{
			return pA->GetNodeId() < pB->GetNodeId();
		}
	};
private:
	std::vector<NodeInfo*>	m_list;	// Store pointers to avoid object copies on sort
};
