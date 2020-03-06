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

class Rect : public Persist, public Merge
{
public:
	Rect()	{ SetInvalid(); }
	Rect(int rowMin, int rowMax, int colMin, int colMax) : m_rowMin(rowMin), m_rowMax(rowMax), m_colMin(colMin), m_colMax(colMax) {}
	~Rect()	{}
	Rect(const Rect& o)						{ *this = o; }
	Rect&	operator=(const Rect& o)		{ m_rowMin = o.m_rowMin; m_rowMax = o.m_rowMax; m_colMin = o.m_colMin, m_colMax = o.m_colMax; return *this; }
	bool	operator==(const Rect& o) const	{ return m_rowMin == o.m_rowMin && m_rowMax == o.m_rowMax && m_colMin == o.m_colMin && m_colMax == o.m_colMax; }
	bool	operator!=(const Rect& o) const	{ return !(*this == o); }
	bool	operator<(const Rect& o) const	{ return GetArea() > o.GetArea(); }	// Bigger area is prefered
	Rect&	operator|=(const Rect& o)
	{
		if ( !o.GetIsValid() ) return *this;
		m_colMin = std::min(m_colMin, o.m_colMin);
		m_colMax = std::max(m_colMax, o.m_colMax);
		m_rowMin = std::min(m_rowMin, o.m_rowMin);
		m_rowMax = std::max(m_rowMax, o.m_rowMax);
		return *this;
	}
	Rect	operator|(const Rect& o)
	{
		Rect out(*this);
		out |= o;
		return out;
	}
	void	SetInvalid()							{ m_rowMin = m_colMin = INT_MAX; m_rowMax = m_colMax = INT_MIN; }
	bool	GetIsValid() const						{ return m_rowMin <= m_rowMax && m_colMin <= m_colMax ; }
	int		GetRows() const							{ return GetIsValid() ? 1 + m_rowMax - m_rowMin : 0; }
	int		GetCols() const							{ return GetIsValid() ? 1 + m_colMax - m_colMin : 0; }
	int		GetArea() const							{ return GetRows() * GetCols(); }
	bool	ContainsPoint(int row, int col) const	{ return GetIsValid() ? row >= m_rowMin && row <= m_rowMax && col >= m_colMin && col <= m_colMax : false; }
	void	Move(int iDown, int iRight)				{ if ( !GetIsValid() ) return; m_rowMin += iDown; m_rowMax += iDown; m_colMin += iRight; m_colMax += iRight; }
	// Merge interface functions
	virtual void UpdateMergeOffsets(MergeOffsets& o) override
	{
		o.deltaRow = std::max(o.deltaRow, m_rowMax + 1);
		//o.deltaCol = std::max(o.deltaCol, m_colMax + 1);
	}
	virtual void ApplyMergeOffsets(const MergeOffsets& o) override
	{
		m_rowMin += o.deltaRow;	m_rowMax += o.deltaRow;
		m_colMin += o.deltaCol;	m_colMax += o.deltaCol;
	}
	// Persist interface functions
	virtual void Load(DataStream& inStream) override
	{
		inStream.Load(m_rowMin);
		inStream.Load(m_rowMax);
		inStream.Load(m_colMin);
		inStream.Load(m_colMax);
	}
	virtual void Save(DataStream& outStream) override
	{
		outStream.Save(m_rowMin);
		outStream.Save(m_rowMax);
		outStream.Save(m_colMin);
		outStream.Save(m_colMax);
	}
	int m_rowMin, m_rowMax, m_colMin, m_colMax;
};
