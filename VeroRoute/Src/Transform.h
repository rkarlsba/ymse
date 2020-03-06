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

#include "Common.h"
#include <QPointF>

// Class to transform a QPointF (i.e. translate/rotate it w.r.t. the origin)

enum class TRANSFORM {NONE = 0, SCALE, TRANSLATE, ROTATE};

struct Transform
{
	Transform(TRANSFORM eType, qreal a = 0, qreal b = 0) : m_eType(eType), m_A(a), m_B(b) {}
	~Transform() {}
	Transform(const Transform& o) { *this = o; }
	Transform& operator=(const Transform& o)
	{
		m_eType	= o.m_eType;
		m_A		= o.m_A;
		m_B		= o.m_B;
		return *this;
	}
	void Do(QPointF& o) const	// Transform the passed in point
	{
		qreal dRadians(0), C(0), S(0);
		const qreal x = o.x();	// Take a copy (not a reference)
		const qreal y = o.y();	// Take a copy (not a reference)
		switch( m_eType )
		{
			case TRANSFORM::NONE:		return;
			case TRANSFORM::SCALE:		o.setX(x * m_A);	o.setY(y * m_B);	return;
			case TRANSFORM::TRANSLATE:	o.setX(x + m_A);	o.setY(y + m_B);	return;
			case TRANSFORM::ROTATE:		dRadians = m_A * M_PI / 180;
										C = cos(dRadians);	S = sin(dRadians);
										o.setX(x*C - y*S);	o.setY(x*S + y*C);	return;
			default:					assert(0);								return;
		}
	}
	const TRANSFORM& GetType() const { return m_eType; }
private:
	TRANSFORM	m_eType	= TRANSFORM::TRANSLATE;	// Transform type
	qreal		m_A		= 0;					// Parameter 1
	qreal		m_B		= 0;					// Parameter 2
};
