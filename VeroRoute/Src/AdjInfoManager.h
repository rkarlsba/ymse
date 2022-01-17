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

#include "AdjInfo.h"

// Manager class to record the adjacency info for all NodeIds on the board (NOT floating components)

class AdjInfoManager
{
public:
	AdjInfoManager() {}
	~AdjInfoManager() { DeAllocate(); }
	void DeAllocate()
	{
		for (auto& pAdjInfo : m_list) delete pAdjInfo;
		m_list.clear();
	}
	size_t	 GetSize() const		{ return m_list.size(); }
	AdjInfo* GetAt(size_t i) const	{ return m_list[i]; }
	bool	 GetNodeIdExists(const int& nodeId) const	// true ==> the nodeId exists on the board
	{
		for (const auto& pAdjInfo : m_list) if ( pAdjInfo->GetNodeId() == nodeId ) return true;
		return false;
	}
	void InitCounts(Element* p)	// Pretend we're assigning "p" for the first time
	{
		const int& newNodeId = p->GetNodeId();
		if ( newNodeId == BAD_NODEID ) return;

		// Search m_list for adjacency info on newNodeId
		AdjInfo* pNew(nullptr);
		for (const auto& pAdjInfo : m_list) if ( pAdjInfo->GetNodeId() == newNodeId ) { pNew = pAdjInfo; break; }
		if ( !pNew )									// If no info on newNodeId in m_list ...
		{
			m_list.push_back(new AdjInfo(newNodeId));	// ... add it
			pNew = m_list.back();
		}
		// Modify adjacency info for newNodeId
		assert( pNew->GetNodeId() == newNodeId );	// Sanity check.
		for (int iNbr = 0; iNbr < NUM_NBRS; iNbr++)
		{
			if ( p->GetNbr(iNbr) && ReadCodeBit(iNbr, p->GetRoutable()) )
				ModifyCount(nullptr, pNew, p->GetNbr(iNbr)->GetNodeId());
		}
		// Don't do p->GetW() !! That will be handled by painting the other wire end
	}
	void UpdateCounts(Element* p, int newNodeId)	// Only called by Board::SetNodeId()
	{
		const int& oldNodeId = p->GetNodeId();
		if ( oldNodeId == newNodeId ) return;	// No change in nodeId for element

		// Search m_list for adjacency info on oldNodeId and newNodeId
		AdjInfo *pOld(nullptr), *pNew(nullptr);
		for (const auto& pAdjInfo : m_list)
		{
			const int& nodeId = pAdjInfo->GetNodeId();
			if ( nodeId == oldNodeId ) pOld = pAdjInfo;
			if ( nodeId == newNodeId ) pNew = pAdjInfo;
			if ( pOld && pNew ) break;
		}
		if ( !pNew && newNodeId != BAD_NODEID )			// If no info on newNodeId in m_list ...
		{
			m_list.push_back(new AdjInfo(newNodeId));	// ... add it
			pNew = m_list.back();
		}
		// Modify adjacency info for oldNodeId and newNodeId
		assert( pNew == nullptr || pNew->GetNodeId() == newNodeId );	// Sanity check.
		for (int iNbr = 0; iNbr < NUM_NBRS; iNbr++)
		{
			if ( p->GetNbr(iNbr) && ReadCodeBit(iNbr, p->GetRoutable()) )
				ModifyCount(pOld, pNew, p->GetNbr(iNbr)->GetNodeId());
		}
		// Don't do p->GetW() !! That will be handled on painting the other wire end
	}
	void SortByLowestNodeId()
	{
		std::stable_sort(m_list.begin(), m_list.end(), HasLowerNodeId());
	}
private:
	void ModifyCount(AdjInfo* pOld, AdjInfo* pNew, const int& nbrNodeId)
	{
		if ( nbrNodeId == BAD_NODEID ) return;
		if ( pOld && pNew && pOld->GetNodeId() == pNew->GetNodeId() ) return;	// No change in nodeId

		AdjInfo* pNbr(nullptr);
		for (const auto& pAdjInfo : m_list) if ( pAdjInfo->GetNodeId() == nbrNodeId ) { pNbr = pAdjInfo; break; }

		if ( pOld && pNbr && pOld->GetNodeId() != pNbr->GetNodeId() )
		{
			pOld->DecCount(pNbr->GetNodeId());
			pNbr->DecCount(pOld->GetNodeId());
		}
		if ( pNew && pNbr && pNew->GetNodeId() != pNbr->GetNodeId())
		{
			pNew->IncCount(pNbr->GetNodeId());
			pNbr->IncCount(pNew->GetNodeId());
		}
	}
	struct HasLowerNodeId
	{
		bool operator()(const AdjInfo* pA, const AdjInfo* pB) const
		{
			return pA->GetNodeId() < pB->GetNodeId();
		}
	};
private:
	std::vector<AdjInfo*>	m_list;	// Store pointers to avoid object copies on sort
};
