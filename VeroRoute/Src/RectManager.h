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

#include "Rect.h"

class RectManager : public Persist
{
public:
	RectManager()	{ Clear(); }
	~RectManager()	{ Clear(); }
	void Clear()	{ m_startRow = m_startCol = 0; m_current.SetInvalid(); m_bounding.SetInvalid(), m_list.clear(); }
	RectManager(const RectManager& o)	{ *this = o; }
	RectManager& operator=(const RectManager& o)
	{
		m_startRow	= o.m_startRow;
		m_startCol	= o.m_startCol;
		m_current	= o.m_current;
		m_bounding	= o.m_bounding;
		m_list.clear();
		for (auto& oo : o.m_list) m_list.push_back( oo );
		return *this;
	}
	bool operator==(const RectManager& o) const	// Compare persisted info
	{
		bool bOK = ( m_startRow	== o.m_startRow	)
				&& ( m_startCol	== o.m_startCol	)
				&& ( m_current	== o.m_current	)
				&& ( m_bounding	== o.m_bounding	)
				&& ( GetSize()	== o.GetSize()	);
		auto iter1 =   m_list.begin();
		auto iter2 = o.m_list.begin();
		while( iter1 != m_list.end() && bOK )
		{
			bOK = ( *iter1 == *iter2 );
			++iter1; ++iter2;
		}
		return bOK;
	}
	bool operator!=(const RectManager& o) const
	{
		return !(*this == o);
	}
	void StartNewRect(const int& row, const int& col)
	{
		m_current.m_rowMin = m_current.m_rowMax = m_startRow = row;
		m_current.m_colMin = m_current.m_colMax = m_startCol = col;
	}
	void UpdateNewRect(const int& row, const int& col)
	{
		m_current.m_rowMin = std::min(m_startRow, row);
		m_current.m_rowMax = std::max(m_startRow, row);
		m_current.m_colMin = std::min(m_startCol, col);
		m_current.m_colMax = std::max(m_startCol, col);
	}
	void	EndNewRect()			{ Add(m_current);	m_current.SetInvalid(); }	// Update list
	size_t	GetSize() const			{ return m_list.size(); }
	Rect&	GetCurrent() const		{ return m_current; }
	Rect&	GetBounding() const		{ return m_bounding; }
	Rect&	GetAt(size_t i)			{ return m_list[i]; }
	bool ContainsPoint(int row, int col) const
	{
		if ( m_current.ContainsPoint(row, col) ) return true;		// Check current before the list
		if ( !m_bounding.ContainsPoint(row, col) ) return false;	// Check bounding rect of the list first
		for (auto& o : m_list) if ( o.ContainsPoint(row,col) ) return true;
		return false;
	}
	void MoveAll(int iDown, int iRight)
	{
		m_startRow += iDown;
		m_startCol += iRight;
		m_current.Move(iDown, iRight);
		m_bounding.Move(iDown,iRight);
		for (auto& rect : m_list)
			rect.Move(iDown, iRight);
	}
	// Persist interface functions
	virtual void Load(DataStream& inStream) override
	{
		Clear();
		inStream.Load(m_startRow);
		inStream.Load(m_startCol);
		m_current.Load(inStream);
		if ( inStream.GetVersion() >= VRT_VERSION_11 )
			m_bounding.Load(inStream);
		unsigned int iSize(0);
		inStream.Load(iSize);
		Rect entry;
		for (unsigned int i = 0; i < iSize; i++)
		{
			entry.Load(inStream);
			m_list.push_back(entry);
		}
	}
	virtual void Save(DataStream& outStream) override
	{
		outStream.Save(m_startRow);
		outStream.Save(m_startCol);
		m_current.Save(outStream);
		m_bounding.Save(outStream);	// Added in VRT_VERSION_11
		const unsigned int iSize = static_cast<unsigned int>( GetSize() );
		outStream.Save(iSize);
		for (auto& rect : m_list)
			rect.Save(outStream);
	}
private:
	void Add(const Rect& rect)
	{
		if ( !rect.GetIsValid() ) return;
		m_list.push_back(rect); std::sort(m_list.begin(), m_list.end());
		m_bounding |= rect;	// Update bounding rectangle
	}
private:
	int					m_startRow;
	int					m_startCol;
	mutable Rect		m_current;	// The rectangle currently being defined by the user
	mutable Rect		m_bounding;	// The bounding rectangle of m_list
	std::vector<Rect>	m_list;		// The list of completed rectangles
};
