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

#include "SimplexFont.h"
#include "Transform.h"
#include "GWriter.h"
#include <QPainter>
#include <QPolygonF>

// Painter class that supports Gerber export, supporting transformations
// with same convention as some QPainter methods

struct GPainter : public QPainter, public std::list<Transform>
{
	GPainter() : QPainter()					{}
	~GPainter()								{ clear(); }
	void SetGStream(GStream* p)				{ m_pStream = p; }
	virtual bool begin(QPaintDevice* p)		{ if ( !m_pStream ) return QPainter::begin(p);	return true; }
	virtual bool end()						{ if ( !m_pStream ) return QPainter::end();		return true; }
	virtual void setFont(const QFont& o)	{ if ( !m_pStream ) return QPainter::setFont(o); }
	virtual void setPen(const QPen& o)		{ if ( !m_pStream ) return QPainter::setPen(o); }
	virtual void setBrush(const QBrush& o)	{ if ( !m_pStream ) return QPainter::setBrush(o); }
	virtual void drawText(int x, int y, int w, int h, int flags, const QString& str, bool bPCB = false)
	{
		if ( !bPCB ) return QPainter::drawText(x, y, w, h, flags, str);

		double X(x), Y(y);

		std::string	stdStr	= str.toStdString();
		const char*	sz		= stdStr.c_str();
		const auto	length	= strlen(sz);

		const double dLetterSep(6);	// Letter separation (hard-coded)

		double dStrWidth(0), dLetterWidth(0);
		for (size_t i = 0; i < length; i++)
		{
			const int simplexIndex = Simplex::GetLetterIndex( sz[i] );
			if ( simplexIndex == -1 ) continue;	// Unsupported character

			const auto& limits = Simplex::GetXlimits(simplexIndex);
			dLetterWidth = limits.second - limits.first;	// xMax - xMin
			dStrWidth	+= dLetterSep + dLetterWidth;
		}
		dStrWidth -= dLetterSep;	// End correction

		if ( flags & Qt::AlignCenter )
			X -= dStrWidth * 0.5;
		else
			assert(0);

		QPointF L, R;	// Ends of a line segment
		const double dScale(0.5);	// Hard-coded scale
		for (size_t i = 0; i < length; i++)
		{
			const int simplexIndex = Simplex::GetLetterIndex( sz[i] );
			if ( simplexIndex == -1 ) continue;	// Unsupported character

			const auto& limits = Simplex::GetXlimits(simplexIndex);
			dLetterWidth = limits.second - limits.first;	// xMax - xMin
			bool bPenUp(true);
			const int jEnd = 2 + 2 * Simplex::GetLetterData(simplexIndex, 0);
			for (int j = 2; j < jEnd; j += 2)
			{
				const int	ix	= Simplex::GetLetterData(simplexIndex, j);
				const int	iy	= Simplex::GetLetterData(simplexIndex, j+1);
				const bool	bOK	= !( ix == -1 && iy == -1 );	// (-1,-1) ==> A pen up command
				if ( bOK )
				{
					R.setX(dScale*(X + ix - limits.first)); R.setY(dScale*(10 + Y - iy));
					if ( !bPenUp ) drawLine(L, R);	// Draw L to R
					L = R;
				}
				bPenUp = !bOK;	// Update bPenUp
			}
			X += dLetterWidth + dLetterSep;
		}
	}
	virtual void scale(qreal sx, qreal sy)
	{
		if ( !m_pStream ) return QPainter::scale(sx, sy);
		push_front( Transform(TRANSFORM::SCALE, sx, sy) );
	}
	virtual void translate(qreal dx, qreal dy)
	{
		if ( !m_pStream ) return QPainter::translate(dx, dy);
		push_front( Transform(TRANSFORM::TRANSLATE, dx, dy) );
	}
	virtual void rotate(qreal a)
	{
		if ( !m_pStream ) return QPainter::rotate(a);
		push_front( Transform(TRANSFORM::ROTATE, a) );
	}
	virtual void save()
	{
		if ( !m_pStream ) return QPainter::save();
		push_front( Transform(TRANSFORM::NONE) );	// Add save point
	}
	virtual void restore()
	{
		if ( !m_pStream ) return QPainter::restore();
		bool bDone(false);
		while( !empty() && !bDone )
		{
			bDone = front().GetType() == TRANSFORM::NONE;	// Reached save point
			pop_front();
		}
	}
	virtual void drawPoint(const QPointF& p)
	{
		if ( !m_pStream ) return QPainter::drawPoint(p);
		m_polygon.clear();
		m_polygon << p;
		AddPad();
	}
	virtual void drawPoint(int x, int y)
	{
		if ( !m_pStream ) return QPainter::drawPoint(x, y);
		m_polygon.clear();
		m_polygon << QPointF(x,y);
		AddPad();
	}
	virtual void drawLine(int x1, int y1, int x2, int y2)
	{
		if ( !m_pStream ) return QPainter::drawLine(x1, y1, x2, y2);
		m_polygon.clear();
		m_polygon << QPointF(x1,y1) << QPointF(x2,y2);
		AddTrack();
	}
	virtual inline void drawLine(const QPointF& p1, const QPointF& p2)
	{
		if ( !m_pStream ) return QPainter::drawLine(p1, p2);
		m_polygon.clear();
		m_polygon << p1 << p2;
		AddTrack();
	}
	virtual void drawRect(int x, int y, int w, int h)
	{
		if ( !m_pStream ) return QPainter::drawRect(x, y, w, h);
		m_polygon.clear();
		m_polygon << QPointF(x,y) << QPointF(x+w,y) << QPointF(x+w,y+h) << QPointF(x,y+h) << QPointF(x,y);
		AddTrack();
	}
	virtual void drawRoundedRect(int x, int y, int w, int h, qreal xRadius, qreal yRadius)
	{
		if ( !m_pStream ) return QPainter::drawRoundedRect(x, y, w, h, xRadius, yRadius);
		xRadius = std::min(xRadius, 0.5 * w);
		yRadius = std::min(yRadius, 0.5 * h);
		const auto xDiameter = xRadius + xRadius;
		const auto yDiameter = yRadius + yRadius;
		drawArc( x+w-xDiameter, y, xDiameter, yDiameter, 0,1440);
		drawLine(x+w-xRadius, y, x+xRadius, y);
		drawArc(x, y, xDiameter, yDiameter, 1440, 1440);
		drawLine(x, y+yRadius,x, y+h-yRadius);
		drawArc(x, y+h-yDiameter, xDiameter, yDiameter, 2880,1440);
		drawLine(x+xRadius, y+h, x+w-xRadius, y+h);
		drawArc(x+w-xDiameter, y+h-yDiameter, xDiameter, yDiameter, 4320,1440);
		drawLine(x+w, y+h-yRadius, x+w, y+yRadius);
	}
	virtual void drawArc(int x, int y, int w, int h, int a, int alen, bool bClose = false)
	{
		if ( !m_pStream ) return QPainter::drawArc(x, y, w, h, a, alen);
		const double rX		= 0.5 * w;
		const double rY		= 0.5 * h;
		const double cX		= x + rX;
		const double cY		= y + rY;
		const double dFact	= M_PI / 2880;	// To convert from 16ths of a degree to radians
		const double dA_lo	= dFact * a;
		const double dA_hi	= dFact * ( a + alen );
		m_polygon.clear();
		for (int i = 0; i <= 20; i++)
		{
			const double A = dA_lo + ( dA_hi - dA_lo ) * 0.05 * i;	// Radians
			m_polygon << QPointF(cX + rX * cos(A), cY - rY * sin(A));
		}
		if ( bClose ) m_polygon << m_polygon.first();
		AddTrack();
	}
	virtual void drawChord(int x, int y, int w, int h, int a, int alen)
	{
		if ( !m_pStream ) return QPainter::drawChord(x, y, w, h, a, alen);
		drawArc(x, y, w, h, a, alen, true);	// true ==> close
	}
	virtual void drawEllipse(int x, int y, int w, int h)
	{
		if ( !m_pStream ) return QPainter::drawEllipse(x, y, w, h);
		drawArc(x, y, w, h, 0, 5760, true);	// true ==> close
	}
private:
	std::list<Transform>& GetTransforms() { return *this; }
	void AddTrack()
	{
		for (auto& o : m_polygon)	// Loop polygon points
			for (auto& t : GetTransforms()) t.Do(o);	// Apply set of transforms to each
		if ( m_pStream ) m_pStream->AddTrack(GPEN::SILK, m_polygon);
	}
	void AddPad()
	{
		for (auto& o : m_polygon)	// Loop polygon points
			for (auto& t : GetTransforms()) t.Do(o);	// Apply set of transforms to each
		if ( m_pStream ) m_pStream->AddPad(GPEN::SILK, m_polygon.first());
	}
	GStream*	m_pStream	= nullptr;
	QPolygonF	m_polygon;	// Helper to avoid passing things around
};
