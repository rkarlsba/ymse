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

class MyRGB : public Persist
{
public:
	MyRGB(int i = 0)
	{
		m_B = (i & 0xFF); i >>= 8;
		m_G = (i & 0xFF); i >>= 8;
		m_R = (i & 0XFF);
	}
	MyRGB(const MyRGB& o) { *this = o; }
	~MyRGB() {}
	MyRGB& operator=(const MyRGB& o)
	{
		m_R	= o.m_R;
		m_G	= o.m_G;
		m_B	= o.m_B;
		return *this;
	}
	bool operator==(const MyRGB& o) const	// Compare persisted info
	{
		return m_R == o.m_R
			&& m_G == o.m_G
			&& m_B == o.m_B;
	}
	bool operator!=(const MyRGB& o) const
	{
		return !(*this == o);
	}
	bool operator<(const MyRGB& o) const
	{
		if ( m_R != o.m_R ) return m_R < o.m_R;
		if ( m_G != o.m_G ) return m_G < o.m_G;
		return m_B < o.m_B;
	}
	bool SetRGB(const int& R, const int& G, const int& B)
	{
		const bool bChanged = (m_R != R || m_G != G || m_B != B);
		m_R = R; m_G = G; m_B = B;
		return bChanged;
	}
	const int&	GetR() const	{ return m_R; }
	const int&	GetG() const	{ return m_G; }
	const int&	GetB() const	{ return m_B; }
	// Persist interface functions
	virtual void Load(DataStream& inStream) override
	{
		inStream.Load(m_R);
		inStream.Load(m_G);
		inStream.Load(m_B);
	}
	virtual void Save(DataStream& outStream) override
	{
		outStream.Save(m_R);
		outStream.Save(m_G);
		outStream.Save(m_B);
	}
private:
	int m_R, m_G, m_B;	// Color
};

