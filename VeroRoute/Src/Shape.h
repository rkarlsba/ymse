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

#include "MyRGB.h"

// Class to describe a simple shape (e.g. for drawing component outlines)

enum class SHAPE { LINE = 0, RECT, ROUNDED_RECT, ELLIPSE, ARC, CHORD};

static std::map<SHAPE, std::string>	mapShapeToStr;	// Mapping of SHAPE enum to strings

static void MakeMapShapeStrings()
{
	if ( !mapShapeToStr.empty() ) return;
	mapShapeToStr[SHAPE::LINE]			= "Line";
	mapShapeToStr[SHAPE::RECT]			= "Rectangle";
	mapShapeToStr[SHAPE::ROUNDED_RECT]	= "Rounded Rectangle";
	mapShapeToStr[SHAPE::ELLIPSE]		= "Ellipse";
	mapShapeToStr[SHAPE::ARC]			= "Arc";
	mapShapeToStr[SHAPE::CHORD]			= "Chord";
}

class Shape : public Persist
{
public:
	Shape(SHAPE type = SHAPE::RECT, bool bLine = true, bool bFill = false, double x1 = 0, double x2 = 0, double y1 = 0, double y2 = 0, double a3 = 0)
	: m_type(type)
	, m_x1(x1), m_x2(x2)
	, m_y1(y1), m_y2(y2)
	, m_a1(0), m_a2(90), m_a3(a3)
	, m_bDrawLine(bLine)
	, m_bDrawFill(bFill)
	, m_lineColor(0x000000)
	, m_fillColor(0x00FF00)	// Green
	{
	}
	Shape(SHAPE type, bool bLine, bool bFill, double x1, double x2, double y1, double y2, double a1, double a2, double a3 = 0)
	: m_type(type)
	, m_x1(x1), m_x2(x2)
	, m_y1(y1), m_y2(y2)
	, m_a1(a1), m_a2(a2), m_a3(a3)
	, m_bDrawLine(bLine)
	, m_bDrawFill(bFill)
	, m_lineColor(0x000000)
	, m_fillColor(0x00FF00)	// Green
	{
	}
	Shape(const Shape& o) { *this = o; }
	~Shape() {}
	Shape& operator=(const Shape& o)
	{
		m_type		= o.m_type;
		m_x1		= o.m_x1;	m_x2 = o.m_x2;
		m_y1		= o.m_y1;	m_y2 = o.m_y2;
		m_a1		= o.m_a1;	m_a2 = o.m_a2;	m_a3 = o.m_a3;
		m_bDrawLine	= o.m_bDrawLine;
		m_bDrawFill	= o.m_bDrawFill;
		m_lineColor	= o.m_lineColor;
		m_fillColor	= o.m_fillColor;
		return *this;
	}
	bool operator==(const Shape& o) const	// Compare persisted info
	{
		return m_type		== o.m_type
			&& m_x1			== o.m_x1	&& m_x2 == o.m_x2
			&& m_y1			== o.m_y1	&& m_y2	== o.m_y2
			&& m_a1			== o.m_a1	&& m_a2	== o.m_a2	&& m_a3	== o.m_a3
			&& m_bDrawLine	== o.m_bDrawLine
			&& m_bDrawFill	== o.m_bDrawFill
			&& m_lineColor	== o.m_lineColor
			&& m_fillColor	== o.m_fillColor;
	}
	bool operator!=(const Shape& o) const
	{
		return !(*this == o);
	}
	bool operator<(const Shape& o) const	// For determining render order
	{
		if ( m_bDrawLine	!= o.m_bDrawLine )	return !m_bDrawLine;	// Prefer no line
		if ( m_bDrawFill	!= o.m_bDrawFill )	return m_bDrawFill;	// Prefer fill
		if ( m_lineColor	!= o.m_lineColor )	return m_lineColor < o.m_lineColor;
		if ( m_fillColor	!= o.m_fillColor )	return m_fillColor < o.m_fillColor;
		if ( m_type			!= o.m_type )		return static_cast<int>(m_type) < static_cast<int>(o.m_type);
		if ( m_x1			!= o.m_x1 )			return m_x1 < o.m_x1;
		if ( m_x2			!= o.m_x2 )			return m_x2 < o.m_x2;
		if ( m_y1			!= o.m_y1 )			return m_y1 < o.m_y1;
		if ( m_y2			!= o.m_y2 )			return m_y2 < o.m_y2;
		if ( m_a1			!= o.m_a1 )			return m_a1 < o.m_a1;
		if ( m_a2			!= o.m_a2 )			return m_a2 < o.m_a2;
		return m_a3 < o.m_a3;
	}
	void SetType(const SHAPE& c)		{ m_type = c; }
	void SetX1(const double& d)			{ m_x1 = d; }
	void SetX2(const double& d)			{ m_x2 = d; }
	void SetY1(const double& d)			{ m_y1 = d; }
	void SetY2(const double& d)			{ m_y2 = d; }
	void SetA1(const double& d)			{ m_a1 = d; }
	void SetA2(const double& d)			{ m_a2 = d; }
	void SetA3(const double& d)			{ m_a3 = d; }
	void SetDrawLine(const bool& b)		{ m_bDrawLine = b; }
	void SetDrawFill(const bool& b)		{ m_bDrawFill = b; }
	void SetLineColor(const MyRGB& r)	{ m_lineColor = r; }
	void SetFillColor(const MyRGB& r)	{ m_fillColor = r; }
	void Set(const double& x1, const double& x2,
			 const double& y1, const double& y2,
			 const double& a1, const double& a2, const double& a3)
	{
		m_x1 = x1;	m_x2 = x2;
		m_y1 = y1;	m_y2 = y2;
		m_a1 = a1;	m_a2 = a2;	m_a3 = a3;
	}

	void SetCX(const double& d)		{ Move(0, d - GetCX()); }
	void SetCY(const double& d)		{ Move(d - GetCY(), 0); }
	void SetDX(const double& d)		{ Expand( d - GetDX(), 0 ); }
	void SetDY(const double& d)		{ Expand( 0, d - GetDY() ); }
	void Move(const double& dDown, const double& dRight)
	{
		m_x1 += dRight;		m_x2 += dRight;
		m_y1 += dDown;		m_y2 += dDown;
	}
	void Expand(const double& dx, const double& dy)
	{
		m_x1 -= 0.5 * dx;	m_x2 += 0.5 * dx;
		m_y1 -= 0.5 * dy;	m_y2 += 0.5 * dy;
	}
	const SHAPE&	GetType() const			{ return m_type; }
	const double&	GetX1() const			{ return m_x1; }
	const double&	GetX2() const			{ return m_x2; }
	const double&	GetY1() const			{ return m_y1; }
	const double&	GetY2() const			{ return m_y2; }
	const double&	GetA1() const			{ return m_a1; }
	const double&	GetA2() const			{ return m_a2; }
	const double&	GetA3() const			{ return m_a3; }
	const bool&		GetDrawLine() const		{ return m_bDrawLine; }
	const bool&		GetDrawFill() const		{ return m_bDrawFill; }
	const MyRGB&	GetLineColor() const	{ return m_lineColor; }
	const MyRGB&	GetFillColor() const	{ return m_fillColor; }
	const double&	GetXmin() const	{ return std::min(m_x1, m_x2); }
	const double&	GetXmax() const	{ return std::max(m_x1, m_x2); }
	const double&	GetYmin() const	{ return std::min(m_y1, m_y2); }
	const double&	GetYmax() const	{ return std::max(m_y1, m_y2); }
	double			GetCX() const	{ return 0.5 * ( m_x1 + m_x2 ); }
	double			GetCY() const	{ return 0.5 * ( m_y1 + m_y2 ); }
	double			GetDX() const	{ return m_x2 - m_x1; }
	double			GetDY() const	{ return m_y2 - m_y1; }
	double			GetXlen() const	{ return fabs(GetDX()); }
	double			GetYlen() const	{ return fabs(GetDY()); }
	double			GetAlen() const	{ double d = m_a2 - m_a1; while ( d < 0 ) d += 360.0; return d; }
	void GetSafeBounds(double& L, double& R, double& T, double& B) const	// Allows for worst case rotation "A3"
	{
		const double DX( GetDX() ), DY( GetDY() );
		const double d = sqrt(DX*DX + DY*DY);
		L = GetCX() - d;
		R = GetCX() + d;
		T = GetCY() - d;
		B = GetCY() + d;
	}
	// Persist interface functions
	virtual void Load(DataStream& inStream) override
	{
		int type(0);
		inStream.Load(type);
		m_type = static_cast<SHAPE> (type);
		inStream.Load(m_x1);
		inStream.Load(m_x2);
		inStream.Load(m_y1);
		inStream.Load(m_y2);
		inStream.Load(m_a1);
		inStream.Load(m_a2);
		if ( inStream.GetVersion() >= VRT_VERSION_29 )
		{
			inStream.Load(m_a3);		// Added in VRT_VERSION_29
			inStream.Load(m_bDrawLine);	// Added in VRT_VERSION_29
			inStream.Load(m_bDrawFill);	// Added in VRT_VERSION_29
			m_lineColor.Load(inStream);	// Added in VRT_VERSION_29
			m_fillColor.Load(inStream);	// Added in VRT_VERSION_29
		}
		else
		{
			m_a3		= 0;
			m_bDrawLine	= true;
			m_bDrawFill	= false;
			m_lineColor.SetRGB(0,0,0);
			m_fillColor.SetRGB(0,255,0);	// Green
		}
	}
	virtual void Save(DataStream& outStream) override
	{
		outStream.Save((int)m_type);
		outStream.Save(m_x1);
		outStream.Save(m_x2);
		outStream.Save(m_y1);
		outStream.Save(m_y2);
		outStream.Save(m_a1);
		outStream.Save(m_a2);
		outStream.Save(m_a3);			// Added in VRT_VERSION_29
		outStream.Save(m_bDrawLine);	// Added in VRT_VERSION_29
		outStream.Save(m_bDrawFill);	// Added in VRT_VERSION_29
		m_lineColor.Save(outStream);	// Added in VRT_VERSION_29
		m_fillColor.Save(outStream);	// Added in VRT_VERSION_29
	}
private:
	// Data (x1,y1) and (x2,y2) are either
	// the start and end points of a line, or the TL and BR corners of a rectangle.
	// Ellipses fit to the rectangle.
	// An arc is a CCW part of the ellipse, starting at "a1" and ending at "a2".
	// Note the angle is w.r.t. a circle rather than an ellipse.
	// The angle "a3" can be used to provide a final CCW rotation of the shape about its centre.
	SHAPE	m_type;
	double	m_x1, m_x2;			// Units of 100 mil w.r.t. footprint centre
	double	m_y1, m_y2;			// Units of 100 mil w.r.t. footprint centre
	double	m_a1, m_a2, m_a3;	// Angles measured CCW degrees.  Zero at 3 o'clock.
	bool	m_bDrawLine;		// Draw line (can only be false if m_bFill is true)
	bool	m_bDrawFill;		// Fill with color
	MyRGB	m_lineColor;		// Line color.	Always black
	MyRGB	m_fillColor;		// Fill color.
};
