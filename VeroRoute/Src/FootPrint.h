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

#include "Grid.h"	// For CompElementGrid

class CompManager;
class RectManager;

// The representation of a component's pin layout and surface occupancy and track pattern

class FootPrint : public CompElementGrid
{
public:
	FootPrint() : CompElementGrid(), m_type(COMP::INVALID) {}
	FootPrint(const FootPrint& o) : CompElementGrid(o), m_type(o.m_type) {}
	FootPrint& operator=(const FootPrint& o)
	{
		CompElementGrid::operator=(o);	// Call operator= in base class
		m_type = o.m_type;
		return *this;
	}
	bool operator==(const FootPrint& o) const	// Compare persisted info
	{
		return CompElementGrid::operator==(o)
			&& m_type == o.m_type;
	}
	bool operator!=(const FootPrint& o) const
	{
		return !(*this == o);
	}
	~FootPrint() {}
	void SetType(const COMP& type)	{ m_type = type; }
	const COMP& GetType() const		{ return m_type; }
	void BuildDefault(const COMP& type);
	void BuildTrax(CompManager* pCompMgr, const RectManager& rectMgr, const ElementGrid& o,
				   const int& nLyr, const int& nRowMin, const int& nRowMax, const int& nColMin, const int& nColMax);
	bool CanStretch(const bool& bGrow) const
	{
		switch( m_type )
		{
			case COMP::WIRE:
			case COMP::DIODE:
			case COMP::RESISTOR:
			case COMP::INDUCTOR:
			case COMP::CAP_CERAMIC:
			case COMP::CAP_FILM:
			case COMP::CAP_FILM_WIDE:
			case COMP::SIP:
			case COMP::DIP:
			case COMP::STRIP_100:
			case COMP::BLOCK_100:
			case COMP::BLOCK_200:
			case COMP::SWITCH_ST:
			case COMP::SWITCH_DT:
			case COMP::SWITCH_ST_DIP:	return (  bGrow && GetCols() < GetMaxLength(m_type) )
											|| ( !bGrow && GetCols() > GetMinLength(m_type) );
			default:					return false;
		}
	}
	bool CanStretchWidth(const bool& bGrow) const
	{
		const int& width = GetRows();
		switch( m_type )
		{
			case COMP::DIP:	return ( bGrow && width < 16 ) || ( !bGrow && width > 2 );	// Widths can be 2,...,16
			default:		return false;
		}
	}
	void Stretch(const bool& bGrow)
	{
		assert( CanStretch(bGrow) );	// Sanity check.  We should have already checked that we can stretch
		const bool	bPlug = IsPlug(m_type);
		CompElement initVal;
		assert( initVal.GetPinIndex() == BAD_PININDEX );
		assert( initVal.GetHoleUse() == HOLE_FREE );
		initVal.SetSurface(bPlug ? SURFACE_PLUG : SURFACE_FULL);

		switch( m_type )
		{
			case COMP::WIRE:	// Special case for wires
				StretchSimple(bGrow, initVal);
				return SetupWire();
			case COMP::DIODE:
			case COMP::RESISTOR:
			case COMP::INDUCTOR:
			case COMP::CAP_CERAMIC:
			case COMP::CAP_FILM:		return StretchSimple(bGrow, initVal);
			case COMP::CAP_FILM_WIDE:
				StretchComplex(m_type, bGrow);
				for (int iRow = 0, rows = GetRows(); iRow < rows; iRow++)
				for (int iCol = 0, cols = GetCols(); iCol < cols; iCol++)
				{
					CompElement* p = Get(iRow,iCol);
					p->SetPinIndex( ( iRow == 1 && iCol == 0 ) ? 0 :
									( iRow == 1 && iCol == GetCols()-1 ) ? 1 : BAD_PININDEX );
					p->SetSurface( SURFACE_FULL );
					p->SetHoleUse( p->GetIsPin() ? HOLE_FULL : HOLE_FREE );
				}
				return;
			case COMP::SIP:
				StretchComplex(m_type, bGrow);
				for (int i = 0; i < GetSize(); i++)
				{
					CompElement* p = GetAt(i);
					p->SetPinIndex( i );
					p->SetSurface( SURFACE_FULL );
					p->SetHoleUse( HOLE_FULL );
				}
				return;
			case COMP::DIP:
				StretchComplex(m_type, bGrow);
				for (int iRow = 0, rows = GetRows(); iRow < rows; iRow++)
				for (int iCol = 0, cols = GetCols(); iCol < cols; iCol++)
				{
					CompElement* p = Get(iRow,iCol);
					p->SetPinIndex( ( iRow == 0 ) ? 2*GetCols()-1-iCol :
									( iRow == GetRows()-1 ) ? iCol : BAD_PININDEX );
					p->SetSurface( p->GetIsPin() ? SURFACE_FULL : SURFACE_GAP );
					p->SetHoleUse( p->GetIsPin() ? HOLE_FULL    : HOLE_FREE );
				}
				return;
			case COMP::STRIP_100:
				StretchComplex(m_type, bGrow);
				for (int i = 0, iSize = GetSize(); i < iSize; i++)
				{
					CompElement* p = GetAt(i);
					p->SetPinIndex( i );
					p->SetSurface( SURFACE_FULL );
					p->SetHoleUse( HOLE_FULL );
				}
				return;
			case COMP::BLOCK_100:
				StretchComplex(m_type, bGrow);
				for (int iRow = 0, rows = GetRows(); iRow < rows; iRow++)
				for (int iCol = 0, cols = GetCols(); iCol < cols; iCol++)
				{
					CompElement* p = Get(iRow,iCol);
					p->SetPinIndex( ( iRow == 1 ) ? iCol : BAD_PININDEX );
					p->SetSurface( SURFACE_FULL );
					p->SetHoleUse( p->GetIsPin() ? HOLE_FULL : HOLE_FREE );
				}
				return;
			case COMP::BLOCK_200:
				StretchComplex(m_type, bGrow);
				for (int iRow = 0, rows = GetRows(); iRow < rows; iRow++)
				for (int iCol = 0, cols = GetCols(); iCol < cols; iCol++)
				{
					CompElement* p = Get(iRow,iCol);
					p->SetPinIndex( ( iRow == 1 && iCol % 2 == 1 ) ? ( iCol - 1 ) / 2 : BAD_PININDEX );
					p->SetSurface( ( iCol == 0 || iCol == GetCols()-1 ) ? SURFACE_FREE : SURFACE_FULL );
					p->SetHoleUse( p->GetIsPin() ? HOLE_FULL : HOLE_FREE );
				}
				return;
			case COMP::SWITCH_ST:
			case COMP::SWITCH_DT:
				StretchComplex(m_type, bGrow);
				for (int iRow = 0, rows = GetRows(); iRow < rows; iRow++)
				for (int iCol = 0, cols = GetCols(); iCol < cols; iCol++)
				{
					CompElement* p = Get(iRow,iCol);
					p->SetPinIndex( ( iCol % 2 == 0 && iRow % 2 == 0 ) ? (iCol/2 + (iRow/2)*((1 + GetCols())/2)) : BAD_PININDEX );
					p->SetSurface( SURFACE_FULL );
					p->SetHoleUse( p->GetIsPin() ? HOLE_FULL : HOLE_FREE );
				}
				return;
			case COMP::SWITCH_ST_DIP:
				assert( GetRows() == 4 );	// DIPs should have 4 rows on construction
				StretchComplex(m_type, bGrow);
				for (int iRow = 0, rows = GetRows(); iRow < rows; iRow++)
				for (int iCol = 0, cols = GetCols(); iCol < cols; iCol++)
				{
					CompElement* p = Get(iRow,iCol);
					p->SetPinIndex( ( iRow == 0 ) ? iCol :
									( iRow == 3 ) ? iCol + GetCols() : BAD_PININDEX );
					p->SetSurface( SURFACE_FULL );
					p->SetHoleUse( p->GetIsPin() ? HOLE_FULL : HOLE_FREE );
				}
				return;
			default:	assert(0);	// Unhandled m_type
		}
	}
	void StretchWidth(const bool& bGrow)
	{
		assert( CanStretchWidth(bGrow) );	// Sanity check.  We should have already checked that we can stretch the width

		StretchWidthIC(bGrow);
		for (int iRow = 0, rows = GetRows(); iRow < rows; iRow++)
		for (int iCol = 0, cols = GetCols(); iCol < cols; iCol++)
		{
			CompElement* p = Get(iRow,iCol);
			p->SetPinIndex( ( iRow == 0 ) ? 2*GetCols()-1-iCol :
							( iRow == GetRows()-1 ) ? iCol : BAD_PININDEX );
			p->SetSurface( ( iRow == 0 || iRow == GetRows()-1 ) ? SURFACE_FULL : SURFACE_GAP );
			p->SetHoleUse( p->GetIsPin() ? HOLE_FULL : HOLE_FREE );
		}
		return;
	}
	// Persist interface functions
	virtual void Load(DataStream& inStream) override
	{
		CompElementGrid::Load(inStream);	// Load() base class
		int type(0);
		inStream.Load(type);
		m_type = static_cast<COMP> (type);
		if ( inStream.GetVersion() < VRT_VERSION_26 )
			if ( m_type == COMP::WIRE ) SetupWire();
	}
	virtual void Save(DataStream& outStream) override
	{
		CompElementGrid::Save(outStream);	// Save() base class
		outStream.Save((int)m_type);
	}
private:
	COMP m_type;	// Type
};
