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

#include "Grid.h"
#include "Shape.h"

class Component;

// Class used to build a description of a custom component

// The following flags are used to form a bitfield that determine how pins may be drawn
const uchar PIN_RECT	= 1;	// Draw pin as a rectangle instead of circle (e.g. for switches/relays)
const uchar PIN_LABELS	= 2;	// Allow pin labels to be drawn

const int BAD_ID = -1;

// Quicker to use struct than a std::pair
struct IntShape
{
	IntShape(const int& i, Shape s) : first(i), second(s) {}
	IntShape(const IntShape& o) { *this = o; }
	IntShape& operator=(const IntShape& o)
	{
		first	= o.first;
		second	= o.second;
		return *this;
	}
	bool operator<(const IntShape& o) const
	{
		if ( second != o.second ) return second < o.second;
		return first < o.first;
	}
	bool operator==(const IntShape& o) const
	{
		return first == o.first && second == o.second;
	}
	bool operator!=(const IntShape& o) const
	{
		return !(*this == o);
	}
	int		first;
	Shape	second;
};

class CompDefiner : public Persist
{
public:
	CompDefiner() { Clear(); }
	CompDefiner(const CompDefiner& o) { *this = o; }
	void Populate(const Component& o);	// Set up using an existing component
	void Clear()
	{
		m_currentPinId = m_currentShapeId = BAD_ID;
		m_iPinFlags = 0;
		m_valueStr = m_prefixStr = m_typeStr = m_importStr = "";
		m_grid.Allocate(1,4,4);
		m_grid.Clear( Pin(BAD_PINCHAR, SURFACE_FULL, HOLE_FREE) );
		m_mapShapes.clear();
		AddRect();	// Provide a Rect by default
	}
	CompDefiner& operator=(const CompDefiner& o)
	{
		m_currentPinId		= o.m_currentPinId;
		m_currentShapeId	= o.m_currentShapeId;
		m_iPinFlags			= o.m_iPinFlags;
		m_valueStr			= o.m_valueStr;
		m_prefixStr			= o.m_prefixStr;
		m_typeStr			= o.m_typeStr;
		m_importStr			= o.m_importStr;
		m_grid				= o.m_grid;
		m_mapShapes.clear();
		for (const auto& mapObj : o.m_mapShapes) m_mapShapes.push_back(mapObj);
		return *this;
	}
	bool operator==(const CompDefiner& o) const	// Compare persisted info
	{
		bool bOK = m_currentPinId		== o.m_currentPinId
				&& m_currentShapeId		== o.m_currentShapeId
				&& m_iPinFlags			== o.m_iPinFlags
				&& m_valueStr			== o.m_valueStr
				&& m_prefixStr			== o.m_prefixStr
				&& m_typeStr			== o.m_typeStr
				&& m_importStr			== o.m_importStr
				&& m_grid				== o.m_grid
				&& m_mapShapes.size()	== o.m_mapShapes.size();
		if ( !bOK ) return false;
		for (auto iterA = m_mapShapes.begin(), iterB = o.m_mapShapes.begin(); iterA != m_mapShapes.end() && iterB != o.m_mapShapes.end(); ++iterA, ++iterB)
			if ( (*iterA) != (*iterB) ) return false;
		return true;
	}
	~CompDefiner()
	{
		m_grid.DeAllocate();
		m_mapShapes.clear();
	}
	bool SetCurrentPinId(const int& i)				{ const bool bChanged = ( m_currentPinId	!= i );	m_currentPinId		= i; return bChanged; }
	bool SetCurrentShapeId(const int& i)			{ const bool bChanged = ( m_currentShapeId	!= i );	m_currentShapeId	= i; return bChanged; }
	bool SetPinFlags(const uchar& i)				{ const bool bChanged = ( m_iPinFlags		!= i );	m_iPinFlags			= i; return bChanged; }
	bool SetValueStr(const std::string& s)			{ const bool bChanged = ( m_valueStr		!= s );	m_valueStr			= s; return bChanged; }
	bool SetPrefixStr(const std::string& s)			{ const bool bChanged = ( m_prefixStr		!= s );	m_prefixStr			= s; return bChanged; }
	bool SetTypeStr(const std::string& s)			{ const bool bChanged = ( m_typeStr			!= s );	m_typeStr			= s; return bChanged; }
	bool SetImportStr(const std::string& s)			{ const bool bChanged = ( m_importStr		!= s );	m_importStr			= s; return bChanged; }
	bool SetGrid(const PinGrid& o)					{ const bool bChanged = ( m_grid			!= o );	m_grid				= o; return bChanged; }
	void AddShape(const int& id, const Shape& o)	{ assert( id != BAD_ID );	m_mapShapes.push_back( IntShape(id, o) ); }
	const int&				GetCurrentPinId() const		{ return m_currentPinId; }
	const int&				GetCurrentShapeId() const	{ return m_currentShapeId; }
	const uchar&			GetPinFlags() const			{ return m_iPinFlags; }
	const std::string&		GetValueStr() const			{ return m_valueStr; }
	const std::string&		GetPrefixStr() const		{ return m_prefixStr; }
	const std::string&		GetTypeStr() const			{ return m_typeStr; }
	const std::string&		GetImportStr() const		{ return m_importStr; }
	const PinGrid&			GetGrid() const				{ return m_grid; }
	std::list<IntShape>&	GetShapes()					{ return m_mapShapes; }
	int  GetMinMargin() const;	// The margin around the footprint on the screen
	int  GetScreenRows() const	{ return 2 * GetMinMargin() + GetGridRows(); }
	int  GetScreenCols() const	{ return 2 * GetMinMargin() + GetGridCols(); }

	// Footprint size and extents
	int  GetGridRows() const	{ return m_grid.GetRows(); }
	int  GetGridCols() const	{ return m_grid.GetCols(); }
	int  GetGridRowMin() const	{ return GetMinMargin(); }
	int  GetGridColMin() const	{ return GetMinMargin(); }
	int  GetGridRowMax() const	{ return GetGridRowMin() + GetGridRows() - 1; }
	int  GetGridColMax() const	{ return GetGridColMin() + GetGridCols() - 1; }
	void GetGridCentre(double& dCentreRow, double& dCentreCol) const	// Footprint centre w.r.t. screen
	{
		dCentreRow = 0.5 * ( GetGridRowMin() + GetGridRowMax() );
		dCentreCol = 0.5 * ( GetGridColMin() + GetGridColMax() );
	}

	Pin&	GetCurrentPin()		{ assert( m_currentPinId != BAD_ID ); return *m_grid.GetAt(m_currentPinId); }
	Shape&	GetCurrentShape()
	{
		assert( m_currentShapeId != BAD_ID );
		for (auto& s : m_mapShapes)
			if ( s.first == m_currentShapeId ) return s.second;
		assert(0);
		return m_mapShapes.begin()->second;
	}
	void	MoveCurrentShape(const double& dDown, const double& dRight);
	size_t	GetNumTruePins() const
	{
		int count(0);
		for (int i = 0, iSize = m_grid.GetSize(); i < iSize; i++)
			if ( m_grid.GetAtConst(i)->GetIsPin() ) count++;
		return count;
	}
	void Build(Component& comp) const;
	bool SetPinNumber(const int& i)
	{
		if ( GetCurrentPinId() == BAD_ID ) return false;
		auto& o =  GetCurrentPin();
		o.SetPinIndex( i - 1 );
		o.SetSurface( SURFACE_FULL );
		o.SetHoleUse( HOLE_FULL );
		return true;
	}
	bool IncPinNumber(bool bInc)
	{
		if ( GetCurrentPinId() == BAD_ID ) return false;
		auto& o = GetCurrentPin();
		const size_t iPinIndex = o.GetPinIndex();
		if ( bInc )
		{
			if ( iPinIndex == BAD_PININDEX )
			{
				switch( o.GetSurface() )
				{
					case SURFACE_HOLE:	o.SetSurface(SURFACE_FREE);	o.SetHoleUse(HOLE_FREE);	return true;
					case SURFACE_FREE:	o.SetSurface(SURFACE_FULL);	o.SetHoleUse(HOLE_FREE);	return true;
					case SURFACE_FULL:	o.SetPinIndex(0);			o.SetHoleUse(HOLE_FULL);	return true;
					default:			assert(0);					return false;	// Don't yet handle SURFACE_GAP / SURFACE_PLUG
				}
			}
			if ( iPinIndex < 254 )	// We're limited to (0 <= pinIndex <= 254)
			{
				o.SetPinIndex(iPinIndex+1);
				return true;
			}
			return false;
		}
		else
		{
			if ( iPinIndex == BAD_PININDEX )
			{
				switch( o.GetSurface() )
				{
					case SURFACE_HOLE:								return false;
					case SURFACE_FREE:	o.SetSurface(SURFACE_HOLE);	o.SetHoleUse(HOLE_FREE);	return true;
					case SURFACE_FULL:	o.SetSurface(SURFACE_FREE);	o.SetHoleUse(HOLE_FREE);	return true;
					default:			assert(0);					return false;	// Don't yet handle SURFACE_GAP / SURFACE_PLUG
				}
			}
			else if ( iPinIndex > 0 )
			{
				o.SetPinIndex(iPinIndex-1);
				o.SetHoleUse(HOLE_FULL);
			}
			else
			{
				o.SetPinIndex(BAD_PININDEX);
				o.SetHoleUse(HOLE_FREE);
			}
			return true;
		}
	}
	bool SetType(const std::string& str)
	{
		if ( GetCurrentShapeId() == BAD_ID ) return false;
		MakeMapShapeStrings();
		for (const auto& mapObj : mapShapeToStr)
		{
			if ( mapObj.second == str )
			{
				GetCurrentShape().SetType(mapObj.first);
				return true;
			}
		}
		return false;
	}
	bool SetPinType(const std::string& str)
	{
		if ( GetCurrentShapeId() == BAD_ID ) return false;
		const bool bRect = ( str == "Rectangle" );
		if ( bRect )
			return SetPinFlags( GetPinFlags() | PIN_RECT );		// Set bit
		else
			return SetPinFlags( GetPinFlags() & ~PIN_RECT );	// Clear bit
	}
	bool SetCX(const double& d)			{ if ( GetCurrentShapeId() == BAD_ID ) return false; GetCurrentShape().SetCX(d); return true; }
	bool SetCY(const double& d)			{ if ( GetCurrentShapeId() == BAD_ID ) return false; GetCurrentShape().SetCY(d); return true; }
	bool SetDX(const double& d)			{ if ( GetCurrentShapeId() == BAD_ID ) return false; GetCurrentShape().SetDX(d); return true; }
	bool SetDY(const double& d)			{ if ( GetCurrentShapeId() == BAD_ID ) return false; GetCurrentShape().SetDY(d); return true; }
	bool SetA1(const double& d)			{ if ( GetCurrentShapeId() == BAD_ID ) return false; GetCurrentShape().SetA1(d); return true; }
	bool SetA2(const double& d)			{ if ( GetCurrentShapeId() == BAD_ID ) return false; GetCurrentShape().SetA2(d); return true; }
	bool SetA3(const double& d)			{ if ( GetCurrentShapeId() == BAD_ID ) return false; GetCurrentShape().SetA3(d); return true; }
	bool SetLine(const bool& b)			{ if ( GetCurrentShapeId() == BAD_ID ) return false; GetCurrentShape().SetDrawLine(b); return true; }
	bool SetFill(const bool& b)			{ if ( GetCurrentShapeId() == BAD_ID ) return false; GetCurrentShape().SetDrawFill(b); return true; }
	bool SetFillColor(const MyRGB& r)	{ if ( GetCurrentShapeId() == BAD_ID ) return false; GetCurrentShape().SetFillColor(r); return true; }
	bool GetCanLower() const
	{
		const int& id = GetCurrentShapeId();
		return ( id != BAD_ID ) && ( id != m_mapShapes.begin()->first );
	}
	bool GetCanRaise() const
	{
		const int& id = GetCurrentShapeId();
		return ( id != BAD_ID ) && ( id != m_mapShapes.rbegin()->first );
	}
	bool Lower()
	{
		assert( GetCanLower() );
		auto iterPrior	= m_mapShapes.begin();
		auto iter		= iterPrior; ++iter;
		for (; iter != m_mapShapes.end(); ++iter, iterPrior++)
			if ( iter->first == GetCurrentShapeId() ) { std::swap(*iter, *iterPrior); return true; }
		return false;
	}
	bool Raise()
	{
		assert( GetCanRaise() );
		auto iterPrior	= m_mapShapes.rbegin();
		auto iter		= iterPrior; ++iter;
		for (; iter != m_mapShapes.rend(); ++iter, iterPrior++)
			if ( iter->first == GetCurrentShapeId() ) { std::swap(*iter, *iterPrior); return true; }
		return false;
	}
	int  AddLine()			{ return AddDefaultShape(SHAPE::LINE); }
	int  AddRect()			{ return AddDefaultShape(SHAPE::RECT); }
	int  AddRoundedRect()	{ return AddDefaultShape(SHAPE::ROUNDED_RECT); }
	int  AddEllipse()		{ return AddDefaultShape(SHAPE::ELLIPSE); }
	int  AddArc()			{ return AddDefaultShape(SHAPE::ARC); }
	int  AddChord()			{ return AddDefaultShape(SHAPE::CHORD); }
	int  AddDefaultShape(SHAPE eType)
	{
		const double dX = 0.5 * GetGridCols();
		const double dY = 0.5 * GetGridRows();
		if ( eType == SHAPE::ARC || eType == SHAPE::CHORD )
			return AddShape( Shape(eType, true, false, -dX, dX, -dY, dY, 0, 90) );
		else
			return AddShape( Shape(eType, true, false, -dX, dX, -dY, dY) );
	}
	// Helpers
	void DestroyShape();
	int  GetNewShapeId() const;
	bool SetWidth(const int& i);
	bool SetHeight(const int& i);
	int  GetPinId(const int& row, const int& col) const;	// Pick the most relevant pin at the location
	int  GetShapeId(const double& dRowIn, const double& dColIn) const;	// Pick the most relevant shape at the location
	bool GetIsValid() const;
	// Persist functions
	virtual void Load(DataStream& inStream) override
	{
		inStream.Load(m_currentPinId);
		inStream.Load(m_currentShapeId);
		inStream.Load(m_iPinFlags);
		inStream.Load(m_valueStr);
		inStream.Load(m_prefixStr);
		inStream.Load(m_typeStr);
		inStream.Load(m_importStr);
		m_grid.Load(inStream);

		unsigned int numShapes(0);
		inStream.Load(numShapes);
		m_mapShapes.clear();
		for (unsigned int i = 0; i < numShapes; i++)
		{
			int		shapeId(BAD_ID);
			Shape	tmp;
			inStream.Load(shapeId);
			tmp.Load(inStream);
			m_mapShapes.push_back( IntShape(shapeId, tmp) );
		}
	}
	virtual void Save(DataStream& outStream) override
	{
		outStream.Save(m_currentPinId);
		outStream.Save(m_currentShapeId);
		outStream.Save(m_iPinFlags);
		outStream.Save(m_valueStr);
		outStream.Save(m_prefixStr);
		outStream.Save(m_typeStr);
		outStream.Save(m_importStr);
		m_grid.Save(outStream);

		const unsigned int numShapes = static_cast<unsigned int>( m_mapShapes.size() );
		outStream.Save(numShapes);
		for (auto& mapObj : m_mapShapes)
		{
			int		shapeId	= mapObj.first;
			Shape&	shape	= mapObj.second;
			outStream.Save(shapeId);
			shape.Save(outStream);
		}
	}
private:
	int AddShape(const Shape& o)
	{
		const int id = GetNewShapeId();
		if ( id != BAD_ID ) AddShape(id, o);
		return id;
	}
private:
	// GUI control
	int						m_currentPinId;		// Current index into m_grid
	int						m_currentShapeId;	// Current selected shape
	// Component description
	uchar					m_iPinFlags;		// 1 ==> PIN_RECT, 2 ==> PIN_LABELS
	std::string				m_valueStr;			// Value label (e.g. "MN3004")
	std::string				m_prefixStr;		// Prefix string (e.g. "IC")
	std::string				m_typeStr;			// Component type (e.g. "BBD")
	std::string				m_importStr;		// For Planet/Tango import
	PinGrid					m_grid;
	std::list<IntShape>		m_mapShapes;		// "Map" of shapeId to Shape.	Coordinates are RELATIVE to footprint centre.
};
