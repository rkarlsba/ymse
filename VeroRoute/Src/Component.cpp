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

#include "Component.h"

void Component::SetDefaultPinFlags()
{
	switch ( GetType() )
	{
		case COMP::TO92					:
		case COMP::TO18					:
		case COMP::TO39					:
		case COMP::TO220				:
		case COMP::SIP					:
		case COMP::DIP					:
		case COMP::DIP_RECTIFIER		:
		case COMP::STRIP_100			:
		case COMP::BLOCK_100			:
		case COMP::BLOCK_200			: return SetPinFlags(PIN_LABELS);
		case COMP::SWITCH_ST			:
		case COMP::SWITCH_DT			:
		case COMP::SWITCH_ST_DIP		:
		case COMP::SWITCH_BUTTON_4PIN	:
		case COMP::RELAY_HK19F			:
		case COMP::RELAY_HJR_4102		:
		case COMP::RELAY_FTR_B3C		:
		case COMP::RELAY_G2R_2			:
		case COMP::RELAY_G2R_2A			:
		case COMP::RELAY_G2RK_2			:
		case COMP::RELAY_G2RK_2A		:
		case COMP::RELAY_G3MB_202P		:
		case COMP::RELAY_JQC_3F_APPROX	:
		case COMP::RELAY_S1A050000		:
		case COMP::RELAY_TRCD			:
		case COMP::FUSE_HOLDER			: return SetPinFlags(PIN_RECT);
		case COMP::RELAY_DIP_4PIN		:
		case COMP::RELAY_DIP_8PIN		: return SetPinFlags(PIN_RECT | PIN_LABELS);
		case COMP::MARK					:
		case COMP::PAD					:
		case COMP::WIRE					:
		case COMP::RESISTOR				:
		case COMP::INDUCTOR				:
		case COMP::DIODE				:
		case COMP::LED					:
		case COMP::CAP_CERAMIC			:
		case COMP::CAP_FILM				:
		case COMP::CAP_FILM_WIDE		:
		case COMP::CAP_ELECTRO_200_NP	:
		case COMP::CAP_ELECTRO_200		:
		case COMP::CAP_ELECTRO_250_NP	:
		case COMP::CAP_ELECTRO_250		:
		case COMP::CAP_ELECTRO_300_NP	:
		case COMP::CAP_ELECTRO_300		:
		case COMP::CAP_ELECTRO_400_NP	:
		case COMP::CAP_ELECTRO_400		:
		case COMP::CAP_ELECTRO_500_NP	:
		case COMP::CAP_ELECTRO_500		:
		case COMP::CAP_ELECTRO_600_NP	:
		case COMP::CAP_ELECTRO_600		:
		case COMP::TRIM_VERT			:
		case COMP::TRIM_VERT_OFFSET		:
		case COMP::TRIM_VERT_OFFSET_WIDE:
		case COMP::TRIM_FLAT			:
		case COMP::TRIM_FLAT_WIDE		:
		case COMP::TRIM_3006P			:
		case COMP::TRIM_3006W			:
		case COMP::TRIM_3006Y			:
		case COMP::TRIM_3329H			:
		case COMP::TRIM_3329P_DK9_RC	:
		case COMP::CRYSTAL				: assert( GetPinFlags() == 0 ); return SetPinFlags(0);
		case COMP::CUSTOM				:
		case COMP::TRACKS				: return;
		default:	assert(0);			  return;	// Unhandled eType
	}
}

void Component::AddDefaultShapes()
{
	switch( GetType() )
	{
		case COMP::CUSTOM:
		case COMP::TRACKS:	return;
		default:			m_shapes.clear();
	}
	switch( GetType() )
	{
		case COMP::MARK:					AddOne( Shape(SHAPE::ELLIPSE,	true, false,	-0.30,  0.30, -0.30,  0.30) );
											AddOne( Shape(SHAPE::LINE,		true, false,	-0.21,  0.21, -0.21,  0.21) );
											AddOne( Shape(SHAPE::LINE,		true, false,	-0.21,  0.21,  0.21, -0.21) );	break;
		case COMP::PAD:						AddTwo( Shape(SHAPE::ELLIPSE,	true, true,		-0.45,  0.45, -0.45,  0.45) );	break;
		case COMP::LED:						AddTwo( Shape(SHAPE::CHORD,		true, true,		-0.75,  0.75, -0.75,  0.75, 30, -30) );break;
		case COMP::CAP_ELECTRO_200:			AddOne( Shape(SHAPE::LINE,		true, false,	 0.77,  0.77, -0.64,  0.64) );
		case COMP::CAP_ELECTRO_200_NP:		AddTwo( Shape(SHAPE::ELLIPSE,	true, true,		-1.00,  1.00, -1.00,  1.00) );	break;
		case COMP::CAP_ELECTRO_250:			AddOne( Shape(SHAPE::LINE,		true, false,	 0.96,  0.96, -0.80,  0.80) );
		case COMP::CAP_ELECTRO_250_NP:		AddTwo( Shape(SHAPE::ELLIPSE,	true, true,		-1.25,  1.25, -1.25,  1.25) );	break;
		case COMP::CAP_ELECTRO_300:			AddOne( Shape(SHAPE::LINE,		true, true,		 1.15,  1.15, -0.96,  0.96) );
		case COMP::CAP_ELECTRO_300_NP:		AddTwo( Shape(SHAPE::ELLIPSE,	true, true,		-1.50,  1.50, -1.50,  1.50) );	break;
		case COMP::CAP_ELECTRO_400:			AddOne( Shape(SHAPE::LINE,		true, false,	 1.53,  1.53, -1.29,  1.29) );
		case COMP::CAP_ELECTRO_400_NP:		AddTwo( Shape(SHAPE::ELLIPSE,	true, true,		-2.00,  2.00, -2.00,  2.00) );	break;
		case COMP::CAP_ELECTRO_500:			AddOne( Shape(SHAPE::LINE,		true, false,	 1.92,  1.92, -1.61,  1.61) );
		case COMP::CAP_ELECTRO_500_NP:		AddTwo( Shape(SHAPE::ELLIPSE,	true, true,		-2.50,  2.50, -2.50,  2.50) );	break;
		case COMP::CAP_ELECTRO_600:			AddOne( Shape(SHAPE::LINE,		true, false,	 2.30,  2.30, -1.93,  1.93) );
		case COMP::CAP_ELECTRO_600_NP:		AddTwo( Shape(SHAPE::ELLIPSE,	true, true,		-3.00,  3.00, -3.00,  3.00) );	break;
		case COMP::TRIM_VERT:				AddTwo( Shape(SHAPE::RECT,		true, true,		-1.50,  1.50, -0.50,  0.50) );	break;
		case COMP::TRIM_VERT_OFFSET:		AddTwo( Shape(SHAPE::RECT,		true, true,		-1.50,  1.50, -0.75,  0.75) );	break;
		case COMP::TRIM_VERT_OFFSET_WIDE:	AddTwo( Shape(SHAPE::RECT,		true, true,		-1.50,  1.50, -1.00,  1.00) );	break;
		case COMP::TRIM_FLAT:				AddTwo( Shape(SHAPE::RECT,		true, true,		-1.50,  1.50, -1.50,  1.50) );	break;
		case COMP::TRIM_FLAT_WIDE:			AddTwo( Shape(SHAPE::RECT,		true, true,		-1.50,  1.50, -1.50,  1.50) );	break;
		case COMP::TRIM_3006P:				AddTwo( Shape(SHAPE::RECT,		true, true,		-3.75,  3.75, -0.50,  0.50) );	break;
		case COMP::TRIM_3006W:				AddTwo( Shape(SHAPE::RECT,		true, true,		-3.75,  3.75, -1.45,  1.45) );	break;
		case COMP::TRIM_3006Y:				AddTwo( Shape(SHAPE::RECT,		true, true,		-3.75,  3.75, -0.50,  0.50) );	break;
		case COMP::TRIM_3329H:				AddTwo( Shape(SHAPE::ELLIPSE,	true, true,		-1.25,  1.25, -1.25,  1.25) );	break;
		case COMP::TRIM_3329P_DK9_RC:		AddTwo( Shape(SHAPE::ELLIPSE,	true, true,		-1.25,  1.25, -1.25,  1.25) );	break;
		case COMP::CRYSTAL:					AddTwo( Shape(SHAPE::ELLIPSE,	true, true,		-1.00,  1.00, -1.00,  1.00) );	break;
		case COMP::TO92:					AddTwo( Shape(SHAPE::CHORD,		true, true,		-1.40,  1.40, -0.65,  1.15, -20, 200) );break;
		case COMP::TO18:					AddOne( Shape(SHAPE::ARC,		true, false,	-0.95,  0.95, -0.95,  0.95, 101, 80) );
											AddOne( Shape(SHAPE::LINE,		true, false,	-0.17, -0.17, -0.94, -1.13) );
											AddOne( Shape(SHAPE::LINE,		true, false,	-0.17,  0.17, -1.13, -1.13) );
											AddOne( Shape(SHAPE::LINE,		true, false,	 0.17,  0.17, -1.13, -0.94) );
											// Fill ...
											AddOne( Shape(SHAPE::ELLIPSE,	false, true,	-0.95,  0.95, -0.95,  0.95) );
											AddOne( Shape(SHAPE::RECT,		false, true,	-0.17,  0.17, -1.13, -0.90) );	break;
		case COMP::TO39:					AddOne( Shape(SHAPE::ARC,		true, false,	-1.80,  1.80, -1.80,  1.80, 145, 125) );
											AddOne( Shape(SHAPE::LINE,		true, false,	-1.47, -1.71, -1.03, -1.27) );
											AddOne( Shape(SHAPE::LINE,		true, false,	-1.71, -1.27, -1.27, -1.71) );
											AddOne( Shape(SHAPE::LINE,		true, false,	-1.27, -1.03, -1.71, -1.47) );
											// Fill ...
											AddOne( Shape(SHAPE::ELLIPSE,	false, true,	-1.80,  1.80, -1.80,  1.80) );
											AddOne( Shape(SHAPE::RECT,		false, true,	-1.68, -1.08, -1.53, -1.10, 0, 0, 45) );	break;
		case COMP::TO220:					AddTwo( Shape(SHAPE::RECT,		true, true,		-1.56,  1.56, -0.56,  0.56) );
											AddOne( Shape(SHAPE::LINE,		true, false,	-1.56,  1.56, -0.31, -0.31) );	break;
		case COMP::SWITCH_BUTTON_4PIN:		AddTwo( Shape(SHAPE::RECT,		true, true,		-1.35,  1.35, -1.35,  1.35) );	break;
		case COMP::RELAY_HK19F:				AddTwo( Shape(SHAPE::RECT,		true, true,		-3.98,  3.98, -1.97,  1.97) );	break;
		case COMP::RELAY_HJR_4102:			AddTwo( Shape(SHAPE::RECT,		true, true,		-3.09,  3.09, -2.05,  2.05) );	break;
		case COMP::RELAY_FTR_B3C:			AddTwo( Shape(SHAPE::RECT,		true, true,		-2.35,  2.35, -1.42,  1.42) );	break;
		case COMP::RELAY_G2R_2:				AddTwo( Shape(SHAPE::RECT,		true, true,		-5.71,  5.71, -2.56,  2.56) );	break;
		case COMP::RELAY_G2R_2A:			AddTwo( Shape(SHAPE::RECT,		true, true,		-5.71,  5.71, -2.56,  2.56) );	break;
		case COMP::RELAY_G2RK_2:			AddTwo( Shape(SHAPE::RECT,		true, true,		-5.71,  5.71, -2.56,  2.56) );	break;
		case COMP::RELAY_G2RK_2A:			AddTwo( Shape(SHAPE::RECT,		true, true,		-5.71,  5.71, -2.56,  2.56) );	break;
		case COMP::RELAY_G3MB_202P:			AddTwo( Shape(SHAPE::RECT,		true, true,		-4.82,  4.82, -1.08,  1.08) );	break;
		case COMP::RELAY_JQC_3F_APPROX:		AddTwo( Shape(SHAPE::RECT,		true, true,		-3.74,  3.74, -3.05,  3.05) );	break;
		case COMP::RELAY_S1A050000:			AddTwo( Shape(SHAPE::RECT,		true, true,		-3.74,  3.74, -1.00,  1.00) );	break;
		case COMP::RELAY_TRCD:				AddTwo( Shape(SHAPE::RECT,		true, true,		-4.53,  4.53, -3.17,  3.17) );	break;
		case COMP::FUSE_HOLDER:				AddTwo( Shape(SHAPE::RECT,		true, true,		-4.85,  4.85, -1.35,  1.35) );	break;
		// Following handle variable length components
		case COMP::DIP:
		case COMP::SIP:
		case COMP::DIP_RECTIFIER:
		case COMP::RELAY_DIP_4PIN:
		case COMP::RELAY_DIP_8PIN:
		{
			double w(0.35 + 0.5*(GetCols() - 1)), h(0.35 + 0.5*(GetRows()-1));
			AddOne( Shape(SHAPE::LINE,	true, false,	-w, -w,  0.25,  h) );
			AddOne( Shape(SHAPE::LINE,	true, false,	-w,  w,  h,  h) );
			AddOne( Shape(SHAPE::LINE,	true, false,	 w,  w,  h, -h) );
			AddOne( Shape(SHAPE::LINE,	true, false,	 w, -w, -h, -h) );
			AddOne( Shape(SHAPE::LINE,	true, false,	-w, -w, -h, -0.25) );
			AddOne( Shape(SHAPE::ARC,	true, false,	-w-0.25, -w+0.25, -0.25,  0.25, -90, 90) );
			// Fill ...
			AddOne( Shape(SHAPE::RECT,	false, true,	-w, w, -h,  h));
			break;
		}
		case COMP::CAP_CERAMIC:
		{
			double w(0.4 + 0.5*(GetCols() - 1)), h(0.4 + 0.5*(GetRows()-1));
			AddTwo( Shape(SHAPE::ELLIPSE, true, true, -w,  w, -h,  h) );
			break;
		}
		case COMP::CAP_FILM:
		case COMP::CAP_FILM_WIDE:
		{
			double w(0.45 + 0.5*(GetCols() - 1)), h(0.45 + 0.5*(GetRows()-1));
			AddTwo( Shape(SHAPE::ROUNDED_RECT, true, true, -w,  w, -h,  h) );
			break;
		}
		case COMP::SWITCH_ST:
		case COMP::SWITCH_DT:
		{
			double w(0.7 + 0.5*(GetCols() - 1)), h(0.7 + 0.5*(GetRows()-1));
			AddTwo( Shape(SHAPE::ROUNDED_RECT,	true, true, -w,  w, -h,  h) );
			break;
		}
		case COMP::SWITCH_ST_DIP:
		{
			double w(0.35 + 0.5*(GetCols() - 1)), h(0.35 + 0.5*(GetRows()-1));
			AddTwo( Shape(SHAPE::RECT,	true, true, -w,  w, -h,  h) );
			break;
		}
		case COMP::RESISTOR:
		case COMP::INDUCTOR:
		{
			double w(-0.32 + 0.5*(GetCols() - 1)), h(0.32 + 0.5*(GetRows()-1));
			AddOne( Shape(SHAPE::ARC,		true, false,	-w - 0.64, -w + 0.16, -h -0.08, -h + 0.72,  53, -53) );
			AddOne( Shape(SHAPE::ARC,		true, false,	 w - 0.16,  w + 0.64, -h -0.08, -h + 0.72, 233, 127) );
			AddOne( Shape(SHAPE::LINE,		true, false,	-w, w, -h, -h) );
			AddOne( Shape(SHAPE::LINE,		true, false,	-w, w,  h,  h) );
			// Fill ...
			AddOne( Shape(SHAPE::ELLIPSE,	false, true,	-w - 0.64, -w + 0.16, -h -0.08, -h + 0.72) );
			AddOne( Shape(SHAPE::ELLIPSE,	false, true,	 w - 0.16,  w + 0.64, -h -0.08, -h + 0.72) );
			AddOne( Shape(SHAPE::RECT,		false, true,	-w, w, -h,  h));
			break;
		}
		case COMP::WIRE:
		{
			double w(0.5*(GetCols() - 1)), h(0.1 + 0.5*(GetRows()-1));
			AddTwo( Shape(SHAPE::ROUNDED_RECT, true, true, -w,  w, -h,  h) );
			break;
		}
		case COMP::DIODE:
		{
			double w(0.35 + 0.5*(GetCols() - 1)), h(0.35 + 0.5*(GetRows()-1));
			AddTwo( Shape(SHAPE::RECT,	true, true,	-w,  w, -h,  h) );
			AddOne( Shape(SHAPE::LINE,	true, false, w - 0.1666, w - 0.1666, -h, h) );
			break;
		}
		case COMP::STRIP_100:
		case COMP::BLOCK_100:
		case COMP::BLOCK_200:
		{
			const int		jj = GetRows() / 2;	// Middle row
			const double	dx = ( GetType() == COMP::BLOCK_200 ) ? 0.5 : 0;
			for (int ii = 0; ii < GetCols(); ii++)
			{
				if ( !Get(jj,ii)->GetIsPin() ) continue;
				double w(0.5*GetCols()- ii), h( 0.5 * GetRows() );
				AddTwo( Shape(SHAPE::ROUNDED_RECT,	true, true, -w-dx,  -w+dx+1, -h,  h) );
			}
			break;
		}
		default:	assert(0);	// Unhandled eType
	}
	std::sort(m_shapes.begin(), m_shapes.end());	// Sort shapes so fills are rendered before lines
	SetDefaultColor();
}

void Component::SetDefaultColor()
{
	switch( GetType() )
	{
		case COMP::CUSTOM:
		case COMP::TRACKS:
		case COMP::MARK:					return;
		case COMP::PAD:						return SetFillColor(MyRGB(0xFFFFDF));
		case COMP::LED:						return SetFillColor(MyRGB(0xFF6644));
		case COMP::CAP_ELECTRO_200:
		case COMP::CAP_ELECTRO_200_NP:
		case COMP::CAP_ELECTRO_250:
		case COMP::CAP_ELECTRO_250_NP:
		case COMP::CAP_ELECTRO_300:
		case COMP::CAP_ELECTRO_300_NP:
		case COMP::CAP_ELECTRO_400:
		case COMP::CAP_ELECTRO_400_NP:
		case COMP::CAP_ELECTRO_500:
		case COMP::CAP_ELECTRO_500_NP:
		case COMP::CAP_ELECTRO_600:
		case COMP::CAP_ELECTRO_600_NP:		return SetFillColor(MyRGB(0x8A8AE4));
		case COMP::TRIM_VERT:
		case COMP::TRIM_VERT_OFFSET:
		case COMP::TRIM_VERT_OFFSET_WIDE:
		case COMP::TRIM_FLAT:
		case COMP::TRIM_FLAT_WIDE:
		case COMP::TRIM_3006P:
		case COMP::TRIM_3006W:
		case COMP::TRIM_3006Y:
		case COMP::TRIM_3329H:
		case COMP::TRIM_3329P_DK9_RC:		return SetFillColor(MyRGB(0x506BFD));
		case COMP::CRYSTAL:					return SetFillColor(MyRGB(0xC8C8C8));
		case COMP::TO92:
		case COMP::TO18:
		case COMP::TO39:
		case COMP::TO220:					return SetFillColor(MyRGB(0xA0A0A0));
		case COMP::SWITCH_BUTTON_4PIN:		return SetFillColor(MyRGB(0x3299CC));
		case COMP::RELAY_HK19F:
		case COMP::RELAY_HJR_4102:
		case COMP::RELAY_FTR_B3C:
		case COMP::RELAY_G2R_2:
		case COMP::RELAY_G2R_2A:
		case COMP::RELAY_G2RK_2:
		case COMP::RELAY_G2RK_2A:
		case COMP::RELAY_G3MB_202P:
		case COMP::RELAY_JQC_3F_APPROX:
		case COMP::RELAY_S1A050000:
		case COMP::RELAY_TRCD:				return SetFillColor(MyRGB(0x84C0D0));
		case COMP::FUSE_HOLDER:				return SetFillColor(MyRGB(0x909090));
		case COMP::DIP:
		case COMP::SIP:
		case COMP::DIP_RECTIFIER:
		case COMP::RELAY_DIP_4PIN:
		case COMP::RELAY_DIP_8PIN:			return SetFillColor(MyRGB(0xA0A0A0));
		case COMP::CAP_CERAMIC:				return SetFillColor(MyRGB(0xFFA050));
		case COMP::CAP_FILM:				return SetFillColor(MyRGB(0x1EB450));
		case COMP::CAP_FILM_WIDE:			return SetFillColor(MyRGB(0x1EB450));
		case COMP::SWITCH_ST:				return SetFillColor(MyRGB(0x3299CC));
		case COMP::SWITCH_DT:				return SetFillColor(MyRGB(0x3299CC));
		case COMP::SWITCH_ST_DIP:			return SetFillColor(MyRGB(0x3299CC));
		case COMP::RESISTOR:				return SetFillColor(MyRGB(0x82CFFD));
		case COMP::INDUCTOR:				return SetFillColor(MyRGB(0xFFE080));
		case COMP::WIRE:					return SetFillColor(MyRGB(0xDFFFFF));
		case COMP::DIODE:					return SetFillColor(MyRGB(0xFF6644));
		case COMP::STRIP_100:				return SetFillColor(MyRGB(0xFFFFDF));
		case COMP::BLOCK_100:				return SetFillColor(MyRGB(0xFFFFDF));
		case COMP::BLOCK_200:				return SetFillColor(MyRGB(0xFFFFDF));
		default:	assert(0);	// Unhandled eType
	}
}

// Helpers for labels
void Component::SetDefaultLabelOffsets()
{
	m_iLabelOffsetCol = 0;
	switch( GetType() )
	{
//		case COMP::PAD:					m_iLabelOffsetRow = 14;	return;
		case COMP::LED:					m_iLabelOffsetRow = 8;	return;
		case COMP::CRYSTAL:
		case COMP::CAP_ELECTRO_200:
		case COMP::CAP_ELECTRO_200_NP:	m_iLabelOffsetRow = 10;	return;
		case COMP::CAP_ELECTRO_250:
		case COMP::CAP_ELECTRO_250_NP:	m_iLabelOffsetRow = 11;	return;
		case COMP::STRIP_100:			m_iLabelOffsetRow = 14;	return;
		case COMP::BLOCK_100:			m_iLabelOffsetRow = 30;	return;
		case COMP::BLOCK_200:			m_iLabelOffsetRow = 30;	return;
		default:						m_iLabelOffsetRow = 0;
	}
}

void Component::GetLabelOffsets(int& offsetRow, int& offsetCol) const	// w.r.t. screen, not comp rotation
{
	switch( GetDirection() )
	{
		case 'W':	offsetRow =  m_iLabelOffsetRow;	offsetCol =  m_iLabelOffsetCol;	return;
		case 'E':	offsetRow = -m_iLabelOffsetRow;	offsetCol = -m_iLabelOffsetCol;	return;
		case 'N':	offsetRow =  m_iLabelOffsetCol;	offsetCol = -m_iLabelOffsetRow;	return;
		case 'S':	offsetRow = -m_iLabelOffsetCol;	offsetCol =  m_iLabelOffsetRow;	return;
	}
}

void Component::MoveLabelOffsets(const int& deltaRow, const int& deltaCol)	// w.r.t. screen, not comp rotation
{
	switch( GetDirection() )
	{
		case 'W':	m_iLabelOffsetRow += deltaRow;	m_iLabelOffsetCol += deltaCol;	return;
		case 'E':	m_iLabelOffsetRow -= deltaRow;	m_iLabelOffsetCol -= deltaCol;	return;
		case 'N':	m_iLabelOffsetCol += deltaRow;	m_iLabelOffsetRow -= deltaCol;	return;
		case 'S':	m_iLabelOffsetCol -= deltaRow;	m_iLabelOffsetRow += deltaCol;	return;
	}
}

void Component::HandleLegacyLabelOffsets()	// For old VRT files
{
	switch( GetDirection() )
	{
		case 'E':	m_iLabelOffsetRow = -m_iLabelOffsetRow;				m_iLabelOffsetCol = -m_iLabelOffsetCol;	return;
		case 'N':	std::swap(m_iLabelOffsetRow, m_iLabelOffsetCol);	m_iLabelOffsetRow = -m_iLabelOffsetRow;	return;
		case 'S':	std::swap(m_iLabelOffsetRow, m_iLabelOffsetCol);	m_iLabelOffsetCol = -m_iLabelOffsetCol;	return;
	}
}
