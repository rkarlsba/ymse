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

#include "CompElement.h"
#include "Element.h"
#include "CompTypes.h"	// For component length limits

// Grid is a templatized 3-dimensional array, with data that can be indexed by layer, row and column

template<class T> 
class Grid : public Persist, public Merge
{
public:
	Grid() { Allocate(0, 0, 0); }
	Grid(const Grid& o) { *this = o; }
	virtual ~Grid() { DeAllocate(); }
	Grid& operator=(const Grid& o)
	{
		Allocate(o.m_lyrs, o.m_rows, o.m_cols);
		for (int i = 0, iSize = GetSize(); i < iSize; i++) m_pData[i] = o.m_pData[i];
		return *this;
	}
	bool operator==(const Grid& o) const	// Compare persisted info
	{
		bool bOK = m_lyrs == o.m_lyrs
				&& m_rows == o.m_rows
				&& m_cols == o.m_cols;
		for (int i = 0, iSize = GetSize(); i < iSize && bOK; i++) bOK = ( m_pData[i] == o.m_pData[i] );
		return bOK;
	}
	bool operator!=(const Grid& o) const
	{
		return !(*this == o);
	}
	void Clear(const T& val)
	{
		for (int i = 0, iSize = GetSize(); i < iSize; i++) m_pData[i] = val;
	}
	void Allocate(int lyrs, int rows, int cols)
	{
		DeAllocate();
		m_lyrs		= lyrs;
		m_rows		= rows;
		m_cols		= cols;
		m_pData		= new T[m_lyrs * m_rows * m_cols];
		m_ppData	= new T*[m_lyrs * m_rows];
		m_pppData	= new T**[m_lyrs];
		for (int i = 0, iSize = m_lyrs * m_rows; i < iSize; i++) m_ppData[i]  = m_pData	 + i * m_cols;
		for (int i = 0, iSize = m_lyrs; 		 i < iSize; i++) m_pppData[i] = m_ppData + i * m_rows;
	}
	void DeAllocate()
	{
		if ( m_pppData ) delete[] m_pppData;	m_pppData = nullptr;
		if ( m_ppData  ) delete[] m_ppData;		m_ppData  = nullptr;
		if ( m_pData   ) delete[] m_pData;		m_pData   = nullptr;
		m_lyrs = m_rows = m_cols = 0;
	}
	const int& GetLyrs() const	{ return m_lyrs; }
	const int& GetRows() const	{ return m_rows; }
	const int& GetCols() const	{ return m_cols; }
	int		   GetSize() const	{ return m_lyrs * m_rows * m_cols; }
	void GetRowCol(T* p, int& row, int& col) const
	{
		int lyr;	// dummy
		GetLyrRowCol(p, lyr, row, col);
	}
	void GetLyrRowCol(T* p, int& lyr, int& row, int& col) const
	{
		const size_t ii		= ( p - m_pData );	assert( ii < static_cast<size_t> ( GetSize() ) );
		const int i			= static_cast<int> (ii);
		const int rowLyr 	= i / m_cols;
		col					= i % m_cols;
		lyr	 				= rowLyr / m_rows;
		row 	 			= rowLyr % m_rows;
	}
	T*	 GetAt(int i)								{ return m_pData + i; }
	T*	 GetAtConst(int i) const					{ return m_pData + i; }
	T*	 Get(int lyr, int row, int col) const		{ return m_pppData[lyr][row] + col;}
	void SetAt(int i, const T& val)					{ m_pData[i] = val; }
	void Set(int lyr, int row, int col, const T& o)	{ m_pppData[lyr][row][col] = o; }
	void Grow(int incLyrs, int incRows, int incCols)	// Grow/shrink the current array
	{
		if ( incLyrs == 0 && incRows == 0 && incCols == 0 ) return;
		Grid<T> tmp(*this);	// Make a temporary copy of this grid
		Allocate(m_lyrs + incLyrs, m_rows + incRows, m_cols + incCols);	// Re-allocate this grid to make it larger/smaller

		// Now copy the tmp data into the new grid
		for (int lyr = 0, lyrs = std::min(GetLyrs(), tmp.GetLyrs()); lyr < lyrs; lyr++)
		for (int row = 0, rows = std::min(GetRows(), tmp.GetRows()); row < rows; row++)
		for (int col = 0, cols = std::min(GetCols(), tmp.GetCols()); col < cols; col++)
			m_pppData[lyr][row][col] = tmp.m_pppData[lyr][row][col];
	}
	// Merge interface functions
	virtual void UpdateMergeOffsets(MergeOffsets& o) override
	{
//		o.deltaLyr	= std::max(o.deltaLyr, GetLyrs() + 1);
		o.deltaRow	= std::max(o.deltaRow, GetRows() + 1);
//		o.deltaCol	= std::max(o.deltaCol, GetCols() + 1);
		for (int i = 0, iSize = GetSize(); i < iSize; i++) m_pData[i].UpdateMergeOffsets(o);
	}
	virtual void ApplyMergeOffsets(const MergeOffsets& o) override
	{
		for (int i = 0, iSize = GetSize(); i < iSize; i++) m_pData[i].ApplyMergeOffsets(o);
	}
	void Merge(const Grid& src, const MergeOffsets& o)
	{
		for (int lyr = 0, lyrs = src.GetLyrs(); lyr < lyrs; lyr++)
		for (int row = 0, rows = src.GetRows(); row < rows; row++)
		for (int col = 0, cols = src.GetCols(); col < cols; col++)
			Get(lyr + o.deltaLyr, row + o.deltaRow, col + o.deltaCol)->Merge(*src.Get(lyr, row, col));
	}
	// Persist interface functions
	virtual void Load(DataStream& inStream) override
	{
		int lyrs(1), rows(1), cols(1);
		inStream.Load(rows);
		inStream.Load(cols);
		if ( inStream.GetVersion() >= VRT_VERSION_34 )
			inStream.Load(lyrs);	// Added in VRT_VERSION_34
		Allocate(lyrs, rows, cols);
		for (int i = 0, iSize = GetSize(); i < iSize; i++) m_pData[i].Load(inStream);
	}
	virtual void Save(DataStream& outStream) override
	{
		outStream.Save(m_rows);
		outStream.Save(m_cols);
		outStream.Save(m_lyrs);		// Added in VRT_VERSION_34
		for (int i = 0, iSize = GetSize(); i < iSize; i++) m_pData[i].Save(outStream);
	}
private:
	int 	m_lyrs		= 0;
	int		m_rows		= 0;
	int		m_cols		= 0;
	T*		m_pData		= nullptr;
	T**		m_ppData	= nullptr;
	T***	m_pppData	= nullptr;	// So we can access data using [][][]
};

// The PinGrid class is used by the component editor class (CompDefiner)
typedef Grid<Pin> PinGrid;

typedef Grid<TrackElement> TrackElementGrid;

// The CompElementGrid class is used for component footprints and "track footprints".
// It can handle simple transformations like stretching and rotating.
// The internal "direction" of a CompElementGrid is always West ('W')
// meaning that Pin 1 is to the West (left) of the centre.
// For example, a resistor footprint is always stored as a CompElementGrid with a
// single horizontal row, with the first pin on the far left and the second pin on the far right.
// If we rotate a component so it points in another direction on screen, then any
// methods that access the CompElementGrid information in a spatial way should specify
// the component direction so the internal data can be read correctly.

class CompElementGrid : public Grid<CompElement>
{
public:
	CompElementGrid() : Grid<CompElement>() {}
	CompElementGrid(const CompElementGrid& o) : Grid<CompElement>(o) { *this = o; }
	virtual ~CompElementGrid() {}
	CompElementGrid& operator=(const CompElementGrid& o) { Grid<CompElement>::operator=(o); return *this; }
	bool operator==(const CompElementGrid& o) const { return Grid<CompElement>::operator==(o); }
	bool operator!=(const CompElementGrid& o) const	{ return Grid<CompElement>::operator!=(o); }
	const int& GetCols(char direction = 'W') const
	{
		switch( direction )	// Component direction: 'W','E','N','S'
		{
			case 'N':
			case 'S':	return Grid<CompElement>::GetRows();
			default:	return Grid<CompElement>::GetCols();
		}
	}
	const int& GetRows(char direction = 'W') const
	{
		switch( direction )	// Component direction: 'W','E','N','S'
		{
			case 'N':
			case 'S':	return Grid<CompElement>::GetCols();
			default:	return Grid<CompElement>::GetRows();
		}
	}
	CompElement* Get(int row, int col) const
	{
		assert( GetLyrs() == 1 );
		return Grid<CompElement>::Get(0, row, col);
	}
	CompElement* Get(int lyr, int row, int col, char direction = 'W') const
	{
		assert( lyr == 0 && GetLyrs() == 1 );
		Transform(row, col, direction);	// Handle direction transformation
		return Grid<CompElement>::Get(lyr, row, col);
	}
	void SetupWire()
	{
		assert( GetLyrs() == 1 && GetRows() == 1 && GetCols() > 1 );
		for (int i = 0, iSize = GetSize(); i < iSize; i++) GetAt(i)->SetWireOccupancies();
	}
	void StretchSimple(bool bGrow, const CompElement& initVal)	// For simple 2-pin components like resistors, wires, diodes, caps
	{
		assert( GetLyrs() == 1 );	
		// Pins are assumed to be first and last element on the row
		if ( GetRows() == 1 && ( bGrow || GetCols() > 2 ) )
		{
			CompElement a(*GetAt(0)), b(*GetAt(GetCols()-1));	// Read ends
			Allocate(GetLyrs(), GetRows(), bGrow ? GetCols() + 1 : GetCols() - 1);	// Resize
			Clear(initVal);
			*GetAt(0) = a; *GetAt(GetCols()-1) = b;	// Set ends
		}
	}
	void StretchComplex(const COMP& eType, bool bGrow)	// For ICs and switches
	{
		assert( GetLyrs() == 1 );	
		if ( !bGrow && GetCols() == GetMinLength(eType) ) return; 	// Don't shrink to less than min allowed length
		if (  bGrow && GetCols() == GetMaxLength(eType) ) return; 	// Don't expand to more than max allowed length
		const int iDelta = GetStretchIncrement(eType);
		return Allocate(GetLyrs(), GetRows(), bGrow ? GetCols() + iDelta : GetCols() - iDelta);	// Resize
	}
	void StretchWidthIC(bool bGrow)	// For DIPs only
	{
		assert( GetLyrs() == 1 );	
		if ( !bGrow && GetRows() == 2 ) return;	// Don't shrink DIP width to less than 2 rows
		return Allocate(GetLyrs(), bGrow ? GetRows() + 1 : GetRows() - 1, GetCols());	// Resize
	}
	virtual void Load(DataStream& inStream) override
	{
		if ( inStream.GetVersion() >= VRT_VERSION_11 )
			return Grid<CompElement>::Load(inStream);

		// Handle legacy VRTs where components used a simple PinGrid instead of an CompElementGrid
		int rows(0), cols(0);
		inStream.Load(rows);
		inStream.Load(cols);
		const int lyrs = ( rows > 0 && cols > 0 ) ? 1 : 0;
		Allocate(lyrs, rows, cols);
		CompElement tmp;
		const int iSize = GetSize();
		for (int i = 0; i < iSize; i++)
		{
			tmp.Pin::Load(inStream);	// Just load the Pin info
			SetAt(i, tmp);
		}
	}
private:
	void Transform(int& row, int& col, char direction) const
	{
		assert( GetLyrs() == 1 );
		// On input:  (row,col) are the "footprint" co-ordinates (as seen on screen).
		// On output: (row,col) have been set to the corresponding internal values.
		const int rTmp(row), cTmp(col);
		switch( direction ) // Component direction: 'W','E','N','S'
		{
			case 'W':	return;
			case 'E':	row = Grid<CompElement>::GetRows()-1 - rTmp;
						col = Grid<CompElement>::GetCols()-1 - cTmp;	return;
			case 'N':	row = Grid<CompElement>::GetRows()-1 - cTmp;
						col = rTmp;								return;
			case 'S':	row = cTmp;
						col = Grid<CompElement>::GetCols()-1 - rTmp;	return;
		}
	}
};


// The ElementGrid is used for the board and has a hidden toroidal behaviour.
// So things that move too far right can appear on the left.
// Similarly for top and bottom.
// It's a lazy way of avoiding bounds checking during development.
// More importantly, it makes the neighbouring element concept simpler
// since every element will always have non-null neighbour pointers.

class ElementGrid : public Grid<Element>
{
public:
	ElementGrid() : Grid<Element>() {}
	ElementGrid(const ElementGrid& o) : Grid<Element>(o) { *this = o; }
	virtual ~ElementGrid() {}
	ElementGrid& operator=(const ElementGrid& o) { Grid<Element>::operator=(o); return *this; }
	bool operator==(const ElementGrid& o) const { return Grid<Element>::operator==(o); }
	bool operator!=(const ElementGrid& o) const	{ return Grid<Element>::operator!=(o); }
	void MakeToroid(int& row, int& col) const	// Make co-ordinates wrap around at grid edges
	{
		assert( GetRows() > 0 );
		if ( GetRows() > 0 )
		{
			while ( row <  0 )			{ row += GetRows(); }
			while ( row >= GetRows() )	{ row -= GetRows(); }
		}
		assert( GetCols() > 0 );
		if ( GetCols() > 0 )
		{
			while ( col <  0 )			{ col += GetCols(); }
			while ( col >= GetCols() )	{ col -= GetCols(); }
		}
	}
	void MakeToroid(int& lyr, int& row, int& col) const	// Make co-ordinates wrap around at grid edges
	{
		assert( GetLyrs() > 0 );
		if ( GetLyrs() > 0 )
		{
			while ( lyr <  0 )			{ lyr += GetLyrs(); }
			while ( lyr >= GetLyrs() )	{ lyr -= GetLyrs(); }
		}
		MakeToroid(row, col);
	}
	Element* Get(int lyr, int row, int col) const
	{
		MakeToroid(lyr, row, col);	// Make co-ordinates wrap around at grid edges
		return Grid<Element>::Get(lyr, row, col);
	}
	void Pan(int iDown, int iRight)	// Input arguments can be +ve or -ve.
	{
		if ( iDown == 0 && iRight == 0 ) return;

		ElementGrid tmp(*this);	// Make a temporary copy of this grid

		for (int iLyr = 0, lyrs = GetLyrs(); iLyr < lyrs; iLyr++)
		for (int iRow = 0, rows = GetRows(); iRow < rows; iRow++)
		for (int iCol = 0, cols = GetCols(); iCol < cols; iCol++)
			Set(iLyr, iRow, iCol, *(tmp.Get(iLyr, iRow - iDown, iCol - iRight)));	// ElementGrid::Get() accounts for toroidal behaviour
	}
	bool CopyFrom(const TrackElementGrid& o)
	{
		if ( o.GetLyrs() != GetLyrs() || o.GetRows() != GetRows() || o.GetCols() != GetCols() ) return false;
		for (int i = 0, iSize = GetSize(); i < iSize; i++)
			GetAt(i)->TrackElement::operator=(*o.GetAtConst(i));
		return true;
	}
	void CopyTo(TrackElementGrid& o) const
	{
		if ( o.GetLyrs() != GetLyrs() || o.GetRows() != GetRows() || o.GetCols() != GetCols() )
			o.Allocate(GetLyrs(), GetRows(), GetCols());
		for (int i = 0, iSize = GetSize(); i < iSize; i++)
			o.GetAt(i)->operator=(*GetAtConst(i));
	}
	size_t GetNumNodeIds() const
	{
		std::set<int> nodeIds;
		for (int i = 0, iSize = GetSize(); i < iSize; i++)
		{
			const int& iNodeId = GetAtConst(i)->GetNodeId();
			if ( iNodeId != BAD_NODEID )
				nodeIds.insert(iNodeId);
		}
		return nodeIds.size();
	}
};
