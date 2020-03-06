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

#include "CompElement.h" // For BAD_COMPID, TRAX_COMPID

// The class that manages grouping/ungrouping of components.
// Every group has a "groupId", and every component has a "compId".
// The manager maintains a list of <groupId, compId> pairs, ordered by increasing groupId.
// For any component, the highest groupId that it is paired with (its "sibling group")
// contains all the component's siblings.
// The very first group in the list (with groupId of 0) is called the "user-group", and
// only ever contains the component(s) that are currently selected by the user in the view.
// If the user only has a single component selected, then the "user-group" only contains
// that component.
// Using the SHIFT key to multi-select components puts them all in the "user-group".
// On pressing "G", all the components in the "user-group" are put into a new group with a
// group ID that is larger than all others.
// On pressing "U" to ungroup components, then if all the components in the "user-group"
// are siblings, the sibling group is destroyed.

class Board;

const int USER_GROUPID = 0;		// Kept at the beginning of the list
const int BAD_GROUPID  = -1;	// Never allowed in the list.

class GroupManager : public Persist, public Merge
{
public:
	GroupManager()	{}
	~GroupManager()	{ Clear(); }
	GroupManager(const GroupManager& o) { *this = o; }
	GroupManager& operator=(const GroupManager& o)
	{
		m_list.clear();
		for (auto& oo : o.m_list) m_list.push_back( std::pair<int,int>(oo.first, oo.second) );
		return *this;
	}
	bool operator==(const GroupManager& o) const	// Compare persisted info
	{
		if ( GetSize() != o.GetSize() ) return false;
		bool bOK(true);
		auto iter1 =   m_list.begin();
		auto iter2 = o.m_list.begin();
		while( iter1 != m_list.end() && bOK )
		{
			bOK = ( iter1->first == iter2->first ) && ( iter1->second == iter2->second );
			++iter1; ++iter2;
		}
		return bOK;
	}
	bool operator!=(const GroupManager& o) const
	{
		return !(*this == o);
	}
	void   Clear()			{ m_list.clear(); }
	size_t GetSize() const	{ return m_list.size(); }
	bool GetIsUserComp(const int& compId) const	// Check if the component is in the user-group
	{
		if ( compId == TRAX_COMPID ) return true;
		for (auto iter = m_list.begin(); iter != m_list.end() && iter->first == USER_GROUPID; ++iter)
			if ( iter->second == compId ) return true;
		return false;
	}
	int  GetNumUserComps() const	{ return GetNumGroupComps(USER_GROUPID); }	// Get number of user-group components
	int  GetUserCompId() const	// Get the only component in the user-group
	{
		assert( GetNumUserComps() == 1 );
		return m_list.begin()->second;
	}
	void ResetUserGroup(const int& compId)	// Reset the user-group with the comp (and its siblings)
	{
		if ( compId == TRAX_COMPID ) return;
		RemoveGroup(USER_GROUPID);	// Wipe the user-group ...
		UpdateUserGroup(compId);	// ... then put the comp (and its siblings) in it
	}
	void UpdateUserGroup(const int& compId)	// Add/Remove comp (and its siblings) to user-group
	{
		if ( compId == TRAX_COMPID ) return;
		if ( !GetIsUserComp(compId) ) return AddToUserGroup(compId);		// Comp not in group, so add it
		if ( GetNumUserComps() > 1 ) return RemoveFromUserGroup(compId);	// Comp is not the last, so remove it ...
	}
	int  GetNewGroupId()	{ Compact();	return m_list.back().first + 1; };	// Get new groupId bigger than all others
	bool CanGroup() const	{ return GetNumUserComps() > 1 && GetSiblingGroupId() == USER_GROUPID; }
	bool CanUnGroup() const	{ return GetNumUserComps() > 1 && GetSiblingGroupId() != USER_GROUPID; }
	void Group()	// When user hits "G"
	{
		if ( !CanGroup() ) return;				// User-group components are already grouped so quit
		const int newGroupId = GetNewGroupId();	// Make new groupId bigger than all others
		if ( newGroupId == INT_MAX ) return;	// Fail if we've reached the max allowed groupId
		for (auto iter = m_list.begin(); iter != m_list.end() && iter->first == USER_GROUPID; ++iter)
			Add(newGroupId, iter->second);		// Copy comps from the user-group to the new group
	}
	void UnGroup(const int& compId)		// When user hits "U"
	{
		if ( !CanUnGroup() ) return;		// User-group components are not grouped so quit
		RemoveGroup( GetSiblingGroupId() );	// Wipe the sibling group
		RemoveGroup(USER_GROUPID);			// Wipe the user-group ...
		UpdateUserGroup(compId);			// ... and put the specified comp (and its siblings) in it
	}
	void RemoveComp(const int& compId)	// Remove all entries with the specified compId
	{
		if ( compId == TRAX_COMPID ) return;
		while(true)	// Keep going till we've erased all relevant entries
		{
			bool bErased(false);
			for (auto iter = m_list.begin(), iterEnd = m_list.end(); iter != iterEnd && !bErased; ++iter)
				if ( iter->second == compId ) { m_list.erase(iter); bErased = true; }
			if ( !bErased ) return;
		}
		DeleteSingleCompGroups();	// Remove (non-user) groups with a single component
	}
	void Add(const int& groupId, const int& compId)
	{
		if ( groupId == BAD_GROUPID || compId == BAD_COMPID || compId == TRAX_COMPID ) return;
		const std::pair<int,int> entry(groupId, compId);
		if ( std::find(m_list.begin(), m_list.end(), entry) != m_list.end() ) return;	// Entry exists
		if ( groupId == USER_GROUPID ) m_list.push_front(entry); else m_list.push_back(entry);
	}
	void Remove(const int& groupId, const int& compId)
	{
		if ( groupId == BAD_GROUPID || compId == BAD_COMPID || compId == TRAX_COMPID ) return;
		const std::pair<int,int> entry(groupId, compId);
		const auto iterFind = std::find(m_list.begin(), m_list.end(), entry);
		if ( iterFind != m_list.end() ) m_list.erase(iterFind);	// Erase entry if it exists
	}
	// Merge interface functions
	virtual void UpdateMergeOffsets(MergeOffsets& o) override
	{
		o.deltaGroupId = GetNewGroupId();	// This compacts the current groupIds
		for (auto& oo : m_list)
			o.deltaCompId = std::max(o.deltaCompId, oo.second + 1);
	}
	virtual void ApplyMergeOffsets(const MergeOffsets& o) override
	{
		Compact();	// Compact BEFORE applying the offsets
		for (auto& oo : m_list)
		{
			if ( oo.first != USER_GROUPID ) oo.first += o.deltaGroupId;	// Don't apply merge offsets to the user group
			oo.second += o.deltaCompId;
		}
	}
	void Merge(const GroupManager& o)
	{
		RemoveGroup(USER_GROUPID);	// Wipe existing user group.  It will be replaced by the one in 'o'
		for (auto& oo : o.m_list)
		{
			if ( !GetEntryIsOK(oo) ) continue;	// The merge offsets must have blown the compId or groupId limits
			if ( oo.first == USER_GROUPID )
				m_list.push_front( std::pair<int,int>(oo.first, oo.second) );
			else
				m_list.push_back( std::pair<int,int>(oo.first, oo.second) );
		}
	}
	// Persist interface functions
	virtual void Load(DataStream& inStream) override
	{
		Clear();
		unsigned int iSize(0);
		int groupId(0), compId(0);
		inStream.Load(iSize);
		for (unsigned int i = 0; i < iSize; i++)
		{
			inStream.Load(groupId);
			inStream.Load(compId);
			const std::pair<int,int> entry(groupId, compId);
			m_list.push_back(entry);
		}
	}
	virtual void Save(DataStream& outStream) override
	{
		const unsigned int iSize = static_cast<unsigned int>( GetSize() );
		outStream.Save(iSize);
		for (auto& o : m_list)
		{
			outStream.Save(o.first);
			outStream.Save(o.second);
		}
	}
	void GetGroupCompIds(const int& groupId, std::list<int>& compIds) const	// Get a copy of the component IDs for a group
	{
		compIds.clear();
		for (auto iter = m_list.begin(); iter != m_list.end() && iter->first <= groupId; ++iter)
			if ( iter->first == groupId ) compIds.push_back(iter->second);
	}
private:
	int  GetSiblingGroupId(const int& compId) const	// Get highest groupId that the comp belongs to
	{
		for (auto riter = m_list.rbegin(); riter != m_list.rend(); ++riter)	// Loop groups in reverse order
			if ( riter->second == compId ) return riter->first;
		return BAD_GROUPID;
	}
	void AddToUserGroup(const int& compId)	// Add comp (and its siblings) to user-group
	{
		const int iSiblingGroupId = GetSiblingGroupId(compId);
		if ( iSiblingGroupId <= USER_GROUPID ) return Add(USER_GROUPID, compId);	// No siblings. Add comp only.
		for (auto iter = m_list.begin(); iter != m_list.end() && iter->first <= iSiblingGroupId; ++iter)
			if ( iter->first == iSiblingGroupId ) Add(USER_GROUPID, iter->second);
	}
	int  GetNumGroupComps(const int& groupId) const	// Get number of components with specified groupId
	{
		int count(0);
		for (auto iter = m_list.begin(); iter != m_list.end() && iter->first <= groupId; ++iter)
			if ( iter->first == groupId ) count++;
		return count;
	}
	void DeleteSingleCompGroups()	// Remove (non-user) groups with a single component
	{
		std::list<int> badGroupIds;
		for (auto riter = m_list.rbegin(); riter != m_list.rend() && riter->first != USER_GROUPID; ++riter)	// Loop non-user groups in reverse order
			if ( GetNumGroupComps( riter->first ) == 1 ) badGroupIds.push_back( riter->first );
		badGroupIds.unique();
		for (const auto& groupId : badGroupIds) RemoveGroup(groupId);
	}
	void RemoveGroup(const int& groupId)	// Remove all entries with the specified groupId
	{
		auto iterBegin = m_list.begin();	// Slide iterBegin to the first entry with the correct group
		while( iterBegin != m_list.end() && iterBegin->first != groupId ) ++iterBegin;
		auto iterEnd = iterBegin;			// Slide iterEnd past the last entry with the correct group
		while( iterEnd != m_list.end() && iterEnd->first == groupId ) ++iterEnd;
		if ( iterBegin != iterEnd ) m_list.erase(iterBegin, iterEnd);
	}
	int  GetSiblingGroupId() const	// Considers all components in the user-group
	{
		int iSiblingGroupId(BAD_GROUPID);
		for (auto iter = m_list.begin(); iter != m_list.end() && iter->first == USER_GROUPID; ++iter)
		{
			if ( iSiblingGroupId == BAD_GROUPID )
				iSiblingGroupId = GetSiblingGroupId(iter->second);
			else if ( iSiblingGroupId != GetSiblingGroupId(iter->second) )
				return USER_GROUPID;	// Some comps in the user-group are not siblings
		}
		return iSiblingGroupId;	// > USER_GROUPID only if all comps in the user-group are siblings
	}
	void RemoveFromUserGroup(const int& compId)	// Remove comp (and its siblings) from user-group
	{
		const int iSiblingGroupId = GetSiblingGroupId(compId);
		if ( iSiblingGroupId <= USER_GROUPID ) return Remove(USER_GROUPID, compId);	// No siblings. Remove comp only.
		for (auto iter = m_list.begin(); iter != m_list.end() && iter->first <= iSiblingGroupId; ++iter)
			if ( iter->first == iSiblingGroupId ) Remove(USER_GROUPID, iter->second);
	}
	bool GetEntryIsOK(const std::pair<int,int>& o) const
	{
		return o.first  >= USER_GROUPID && o.first  <  INT_MAX
			&& o.second != BAD_COMPID   && o.second != TRAX_COMPID;
	}
	void Compact()	// Make groupId's increment by 1
	{
		int newGroupId(0);	// Start at lowest groupId
		for (auto& o : m_list)
		{
			const int groupId = o.first;
			if ( groupId == newGroupId + 1 )
				newGroupId++;
			else if ( groupId > newGroupId + 1 )	// Remap groupId if it has increased by more than 1
			{
				newGroupId++;
				for (auto i = m_list.begin(); i != m_list.end() && i->first <= groupId; ++i)
					if ( i->first == groupId ) i->first = newGroupId;
			}
		}
	}
private:
	std::list<std::pair<int,int>> m_list;	// List of <groupId, compId> pairs, ordered by increasing groupId
};
