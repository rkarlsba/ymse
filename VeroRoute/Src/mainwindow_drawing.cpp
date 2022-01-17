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

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "GPainter.h"

void MainWindow::DestroyPixmapCache()
{
	if ( m_ppPixmapPad )	for (int i = 0; i <     NUM_PIXMAP_COLORS; i++) delete m_ppPixmapPad[i];	delete[] m_ppPixmapPad;		m_ppPixmapPad	= nullptr;
	if ( m_ppPixmapVia )	for (int i = 0; i <     NUM_PIXMAP_COLORS; i++) delete m_ppPixmapVia[i];	delete[] m_ppPixmapVia;		m_ppPixmapVia	= nullptr;
	if ( m_ppPixmapDiag )	for (int i = 0; i < 2 * NUM_PIXMAP_COLORS; i++) delete m_ppPixmapDiag[i];	delete[] m_ppPixmapDiag;	m_ppPixmapDiag	= nullptr;
	if ( m_ppPixmapBlob )	for (int i = 0; i < 256; i++)					delete m_ppPixmapBlob[i];	delete[] m_ppPixmapBlob;	m_ppPixmapBlob	= nullptr;
}

void MainWindow::CreatePixmapCache(const GuiControl& guiCtrl, ColorManager& colorMgr)
{
	colorMgr.SetSaturation( guiCtrl.GetSaturation() );			// Must do this BEFORE making pixmaps
	colorMgr.SetFillSaturation( guiCtrl.GetFillSaturation() );	// Must do this BEFORE making pixmaps

	if ( m_ppPixmapPad ) return;	// Cache exists

	grabMouse(Qt::WaitCursor);

	// The set of used connections for a grid point are represented by an 8-bit "perimeter code".
	// There are 256 possible ways for a grid point to have connections to its 8 neigbours.
	// Each way produces a different local track pattern (or "blob") for the grid point.
	// The pixmap cache provides a way of quickly mapping the "perimeter code" to a "blob" pixmap.

	const int&	W	= guiCtrl.GetGRIDPIXELS();		// Square width size in pixels
	const int	C	= W >> 1;						// Half square width in pixels
	const int	D	= guiCtrl.GetHalfPadWidth();	// Half pad width in pixels
	const int	H	= (int) ceil(1.414 * guiCtrl.GetHalfTrackWidth());
	m_ppPixmapPad	= new QPixmap*[NUM_PIXMAP_COLORS];
	m_ppPixmapVia	= new QPixmap*[NUM_PIXMAP_COLORS];
	m_ppPixmapDiag	= new QPixmap*[2 * NUM_PIXMAP_COLORS];
	m_ppPixmapBlob	= new QPixmap*[256];

	QPainter painter;
	for (int i = 0; i < NUM_PIXMAP_COLORS; i++)
	{
		int R(0), G(0), B(0);
		colorMgr.GetPixmapRGB(i, R, G, B);
		const QColor color(R, G, B, 255);

		m_ppPixmapPad[i] = new QPixmap(2*D, 2*D);
		m_ppPixmapPad[i]->fill(Qt::transparent);

		painter.begin(m_ppPixmapPad[i]);
		PaintPad(guiCtrl, painter, color, QPointF(D,D));
		painter.end();

		m_ppPixmapVia[i] = new QPixmap(2*D, 2*D);
		m_ppPixmapVia[i]->fill(Qt::transparent);

		painter.begin(m_ppPixmapVia[i]);
		PaintViaPad(guiCtrl, painter, color, QPointF(D,D));
		painter.end();

		for (int jDiagCode = 0; jDiagCode < 2; jDiagCode++)	// 0 ==> LT, 1 ==> RT
		{
			const int ii = i + jDiagCode * NUM_PIXMAP_COLORS;

			m_ppPixmapDiag[ii] = new QPixmap(2*H, 2*H);
			m_ppPixmapDiag[ii]->fill(Qt::transparent);

			painter.begin(m_ppPixmapDiag[ii]);
			PaintDiag(guiCtrl, painter, color, QPointF(H,H), H, jDiagCode == 0);
			painter.end();
		}
	}
	const QColor backgroundColor = GetBackgroundColor();
	for (int i = 0; i < 256; i++)	// Loop all possible perimeter codes
	{
		m_ppPixmapBlob[i] = new QPixmap(2*C, 2*C);
		m_ppPixmapBlob[i]->fill(backgroundColor);

		painter.begin(m_ppPixmapBlob[i]);
		PaintBlob(guiCtrl, painter, Qt::black, QPointF(C,C), i);
		painter.end();

		// Now turn the black blob area transparent, so we can overlay it over colored nodes.
		QBitmap mask = m_ppPixmapBlob[i]->createMaskFromColor(backgroundColor, Qt::MaskOutColor);
		m_ppPixmapBlob[i]->setMask(mask);
	}

	releaseMouse();
}

void MainWindow::PaintViaPad(const GuiControl& guiCtrl, QPainter& painter, const QColor& color, const QPointF& pC, const bool& bGap)
{
	if ( m_bWriteGerber )
	{
		m_gWriter.GetStream(GFILE::GTL).AddViaPad(bGap ? GPEN::VIA_GAP : GPEN::VIA, pC);	// Top    copper layer
		m_gWriter.GetStream(GFILE::GBL).AddViaPad(bGap ? GPEN::VIA_GAP : GPEN::VIA, pC);	// Bottom copper layer
		if ( !bGap )
		{
			m_gWriter.GetStream(GFILE::GTS).AddPad(GPEN::VIA_MASK, pC);		// Top    solder mask layer
			m_gWriter.GetStream(GFILE::GBS).AddPad(GPEN::VIA_MASK, pC);		// Bottom solder mask layer
			m_gWriter.GetStream(GFILE::DRL).AddViaHole(GPEN::VIA_HOLE, pC);	// Drill file
		}
	}
	else
	{
		const int gapWidth = ( bGap ) ? guiCtrl.GetGapWidth() : 0;
		const int padWidth = ( guiCtrl.GetHalfViaPadWidth() + gapWidth ) << 1;
		static QPen	pen(Qt::black, 2, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);
		pen.setColor(color);
		pen.setWidth(padWidth);
		painter.setPen(pen);
		painter.setBrush(Qt::NoBrush);
		painter.drawPoint(pC);
	}
}

void MainWindow::PaintPad(const GuiControl& guiCtrl, QPainter& painter, const QColor& color, const QPointF& pC, const bool& bGap)
{
	if ( m_bWriteGerber )
	{
		m_gWriter.GetStream(GFILE::GTL).AddPad(bGap ? GPEN::PAD_GAP : GPEN::PAD, pC); // Top    copper layer
		m_gWriter.GetStream(GFILE::GBL).AddPad(bGap ? GPEN::PAD_GAP : GPEN::PAD, pC); // Bottom copper layer
		if ( !bGap )
		{
			m_gWriter.GetStream(GFILE::GBS).AddPad(GPEN::PAD_MASK, pC);		// Bottom solder mask layer
			m_gWriter.GetStream(GFILE::DRL).AddPadHole(GPEN::PAD_HOLE, pC);	// Drill file
		}
	}
	else
	{
		const int gapWidth = ( bGap ) ? guiCtrl.GetGapWidth() : 0;
		const int padWidth = ( guiCtrl.GetHalfPadWidth() + gapWidth ) << 1;
		static QPen	pen(Qt::black, 2, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);
		pen.setColor(color);
		pen.setWidth(padWidth);
		painter.setPen(pen);
		painter.setBrush(Qt::NoBrush);
		painter.drawPoint(pC);
	}
}

void MainWindow::PaintDiag(const GuiControl& guiCtrl, QPainter& painter, const QColor& color, const QPointF& pCorner, const int& H, bool bLT)
{
	const int	trackWidth	= guiCtrl.GetHalfTrackWidth() << 1;	// Track width in pixels

	static QPen	pen(Qt::black, 2, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);
	pen.setColor(color);
	pen.setWidth(trackWidth);
	painter.setPen(pen);
	painter.setBrush(Qt::NoBrush);
	if ( bLT )
		painter.drawLine(pCorner + QPoint(-H,-H), pCorner + QPoint(H, H));
	else // bRT
		painter.drawLine(pCorner + QPoint(-H, H), pCorner + QPoint(H,-H));
}

void MainWindow::PaintBlob(const GuiControl& guiCtrl, QPainter& painter, const QColor& color, const QPointF& pC, const int& iPerimeterCode, const bool bGap)
{
	const bool	bMaxDiags		= ( guiCtrl.GetDiagsMode() == DIAGSMODE::MAX );
	const int&	W				= guiCtrl.GetGRIDPIXELS();	// Square width in pixels
	const int	C				= W >> 1;					// Half square width in pixels
	const int	gapWidth		= ( bGap ) ? guiCtrl.GetGapWidth() : 0;
	const int	padWidth		= ( guiCtrl.GetHalfPadWidth()   + gapWidth ) << 1;	// Pad width in pixels
	const int	trackWidth		= ( guiCtrl.GetHalfTrackWidth() + gapWidth ) << 1;	// Track width in pixels
	const bool&	bCurvedTracks	= guiCtrl.GetCurvedTracks();
	const bool&	bFatTracks		= !bCurvedTracks && guiCtrl.GetFatTracks();
	QPolygonF	polygon;

	// Clockwise-ordered array of perimeter points around the square, starting at left...
	const QPointF p[8] = { pC+QPointF(-C,0), pC+QPointF(-C,-C), pC+QPointF(0,-C), pC+QPointF(C,-C),
						   pC+QPointF( C,0), pC+QPointF( C, C), pC+QPointF(0, C), pC+QPointF(-C,C) };
	// Clockwise-ordered array of perimeter point usage, starting at left...
	bool bUsed[8];
	for (int iNbr = 0; iNbr < 8; iNbr++) bUsed[iNbr] = ReadCodeBit(iNbr, iPerimeterCode);

	if ( bMaxDiags )	// For "max diagonals mode", force relevant corner perimeter points to be used
	{
		if ( bUsed[NBR_L] && bUsed[NBR_T] ) bUsed[NBR_LT] = true;
		if ( bUsed[NBR_R] && bUsed[NBR_T] ) bUsed[NBR_RT] = true;
		if ( bUsed[NBR_L] && bUsed[NBR_B] ) bUsed[NBR_LB] = true;
		if ( bUsed[NBR_R] && bUsed[NBR_B] ) bUsed[NBR_RB] = true;
	}

	// Construct polygon ("blob") based on used perimeter points
	polygon.clear();

	// Count used perimeter points and find the first
	int iFirst(-1), N(0);	// N ==> number of perimeter points
	for (int i = 0; i < 8; i++)
		if ( bUsed[i] ) { N++; if ( iFirst == -1 ) iFirst = i; }

	bool bStraight(false), bOrtho(false), bObtuse(false);	// Flags to help describe track sections
	// bStraight	==> Track goes straight across the centre point
	// bOrtho		==> Track bends 90 degrees
	// bObtuse		==> Track bends < 90 degrees

	if ( N == 0 )
		polygon << pC;
	else if ( N == 1 )
	{
		polygon << p[iFirst] << pC;
		bStraight = true;
	}
	else
	{
		bool bOpenLine(false);	// true ==> don't draw a closed polygon
		if ( N == 2 )
		{
			int  nCount(0);					// Perimeter point counter
			int  iL(iFirst), iR(iFirst);	// Indexes of consecutive used perimeter points
			for (int ii = 1; ii <= 8 && nCount < N; ii++)	// A full clockwise loop around the perimeter back to the start
			{
				const int jj = ( ii + iFirst ) % 8;
				if ( !bUsed[jj] ) continue;
				iL = iR;	iR = jj;	// Update iL and iR
				const int iDiff = ( 8 + iR - iL ) % 8;
				bStraight	= ( iDiff == 4 );
				bOrtho		= ( iDiff == 2 || iDiff == 6 );
				bObtuse		= ( iDiff == 3 || iDiff == 5 );
				nCount++;
			}
			bOpenLine = ( bOrtho || bObtuse || bStraight );
		}
		int  nCount(0);					// Perimeter point counter
		int  iL(iFirst), iR(iFirst);	// Indexes of consecutive used perimeter points
		for (int ii = 1; ii <= 8 && nCount < N; ii++)	// A full clockwise loop around the perimeter back to the start
		{
			if ( bOpenLine && ii == 8 ) break;	// bOpenLine ==> don't close the polygon
			const int jj = ( ii + iFirst ) % 8;
			if ( !bUsed[jj] ) continue;
			iL = iR;	iR = jj;	// Update iL and iR
			const int iDiff = ( 8 + iR - iL ) % 8;
			bStraight	= ( iDiff == 4 );
			bOrtho		= ( iDiff == 2 || iDiff == 6 );
			bObtuse		= ( iDiff == 3 || iDiff == 5 );
			nCount++;
			if ( bOrtho || bObtuse )	// Bend <= 90 degrees
			{
				if ( bCurvedTracks )
				{
					// Make an N-point curve from L to R passing near central control point C
					// Current interpolation is quadratic.
					// Using higher order (e.g. 2.5) gives bends passing closer to C (hence sharper corners)
					const int		N = 10;
					const double	d = 1.0 / N;
					const QPointF	pLC(p[iL] - pC), pRC(p[iR] -pC);
					for (int i = 0; i <= N; i++)
					{
						const double t(i * d), u(1 - t);
						polygon << pC + pLC*(u*u) + pRC*(t*t);	// Bezier curve (quadratic interpolation)
					//	polygon << pC + pLC*pow(u,2.5) + pRC*pow(t,2.5);	// Sharper bends
					}
				}
				else if ( bOrtho )	// Bend == 90 degrees (chosen to approximate the above curve)
				{
					const double r = 0.5;			// i.e. 2*t^2	when t = 0.5
				//	const double r = 0.25*sqrt(2);	// i.e. 2*t^2.5	when t = 0.5
					const double s = 1 - r;
					polygon << p[iL] << p[iL]*r + pC*s << p[iR]*r + pC*s << p[iR];	// Draw mitred corner instead of 90 degree bend for L-C-R
				}
				else
					polygon << p[iL] << pC << p[iR];	// Draw a sharp bend for L-C-R instead of a smooth curve
			}
			else
			{
				if ( iL == iFirst ) polygon << p[iL];	// Add "L" to the polygon is it's the first point
				if ( iR != iFirst ) polygon << p[iR];	// Add "R" to the polygon if it isn't the first point
			}
		}
		if ( polygon.size() == 2 && !bStraight )	// If points are not directly opposite the centre ...
			polygon << pC;							// ... add centre point
	}

	// Pens for drawing (not Gerber)
	static QPen		pen(Qt::black, 2, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);
	static QBrush	brush(Qt::black,  Qt::SolidPattern);
	pen.setColor(color);
	brush.setColor(color);

	// Draw
	if ( N == 0 )	// Isolated node drawn as a pad
	{
		if ( m_bWriteGerber )
		{
			assert( polygon.size() == 1 );
			m_gWriter.GetStream(GFILE::GTL).AddTrack(bGap ? GPEN::PAD_GAP : GPEN::PAD, polygon);	// Top    copper layer
			m_gWriter.GetStream(GFILE::GBL).AddTrack(bGap ? GPEN::PAD_GAP : GPEN::PAD, polygon);	// Bottom copper layer
		}
		else
		{
			pen.setWidth(padWidth);
			painter.setPen(pen);
			painter.drawPoint(pC);
		}
	}
	else if ( N <= 2 && ( bOrtho || bObtuse || bStraight ) )	// Draw open line segment
	{
		if ( m_bWriteGerber )
		{
			auto& osT = m_gWriter.GetStream(GFILE::GTL);	// Top    copper layer
			auto& osB = m_gWriter.GetStream(GFILE::GBL);	// Bottom copper layer
			const GPEN ePen		= bGap ? GPEN::TRACK_GAP : GPEN::TRACK;
			const GPEN ePenHV	= bGap ? GPEN::PAD_GAP   : GPEN::PAD;

			if ( bFatTracks && padWidth > trackWidth )
			{
				if ( m_board.GetLyrs() > 1 )
					osT.AddVariTrack(ePenHV, ePen, polygon);
				osB.AddVariTrack(ePenHV, ePen, polygon);
			}
			else
			{
				if ( m_board.GetLyrs() > 1 )
					osT.AddTrack(ePen, polygon);
				osB.AddTrack(ePen, polygon);
			}
		}
		else
		{
			pen.setWidth(trackWidth);
			painter.setPen(pen);
			painter.setBrush(brush);
			auto iterA = polygon.begin();
			auto iterB = iterA; iterB++;
			while( iterB != polygon.end() )
			{
				painter.drawLine(*iterA, *iterB);
				++iterA; ++iterB;
			}
		}
	}
	else	// Draw closed polygon
	{
		if ( m_bWriteGerber )
		{
			auto& osT = m_gWriter.GetStream(GFILE::GTL);	// Top    copper layer
			auto& osB = m_gWriter.GetStream(GFILE::GBL);	// Bottom copper layer
			if ( m_board.GetLyrs() > 1 )
			{
				osT.AddLoop(bGap ? GPEN::TRACK_GAP : GPEN::TRACK, polygon);		// Closed polygon outline
				if ( !bGap ) osT.AddRegion(polygon);	// Only non-Gap polygon needs filling
			}
			osB.AddLoop(bGap ? GPEN::TRACK_GAP : GPEN::TRACK, polygon);			// Closed polygon outline
			if ( !bGap ) osB.AddRegion(polygon);		// Only non-Gap polygon needs filling
		}
		else
		{
			pen.setWidth(trackWidth);
			painter.setPen(pen);
			painter.setBrush(brush);
			painter.drawPolygon(polygon);
		}
	}

	if ( bFatTracks && padWidth > trackWidth )	// Widen H and V tracks to pad width
	{
		if ( m_bWriteGerber )
		{
			auto& osT = m_gWriter.GetStream(GFILE::GTL);	// Top    copper layer
			auto& osB = m_gWriter.GetStream(GFILE::GBL);	// Bottom copper layer
			const GPEN ePen = bGap ? GPEN::PAD_GAP : GPEN::PAD;
			for (int iNbr = 0; iNbr < 8; iNbr += 2)	// Loop non-diagonal perimeter points
			{
				if ( !bUsed[iNbr] ) continue;

				int iNbrOpp = (iNbr + 12 ) % 8;
				if ( bUsed[iNbrOpp] )	// If can go straight across, do so
				{
					if ( iNbr <= 2 )	// No overlay
					{
						polygon.clear();
						polygon << p[iNbr] << p[iNbrOpp];
						if ( m_board.GetLyrs() > 1 )
							osT.AddTrack(ePen, polygon);	// Draw track across
						osB.AddTrack(ePen, polygon);		// Draw track across
					}
				}
				else
				{
					polygon.clear();
					polygon << pC << p[iNbr];
					if ( m_board.GetLyrs() > 1 )
						osT.AddTrack(ePen, polygon);	// Draw track from centre to perimeter point
					osB.AddTrack(ePen, polygon);		// Draw track from centre to perimeter point
				}
			}
		}
		else
		{
			pen.setWidth(padWidth);
			painter.setPen(pen);
			painter.setBrush(brush);
			for (int iNbr = 0; iNbr < 8; iNbr += 2)	// Loop non-diagonal perimeter points
				if ( bUsed[iNbr] ) painter.drawLine(pC, p[iNbr]);	// Draw track from centre to perimeter point
		}
	}
}

void MainWindow::paintEvent(QPaintEvent*)
{
	if ( !m_bRepaint ) return;

	if ( m_board.GetCompEdit() )
		PaintCompDefiner();
	else
		PaintBoard();

	// Tidy up
	m_label->setPixmap(m_mainPixmap);
	m_scrollArea->setVisible(true);
	m_scrollArea->setWidgetResizable(false);
	m_label->adjustSize();

	m_bRepaint = false;
}

void MainWindow::PaintCompDefiner()	// The paint method in "component editor mode"
{
	Board&			board		= m_board;
	CompDefiner&	def			= board.GetCompDefiner();
	const PinGrid&	grid		= def.GetGrid();

	const int&		 W			= board.GetGRIDPIXELS();	// Square width in pixels
	const int		 C			= W >> 1;					// Half square width in pixels
	const double	 dTextScale	= W / 24.0;					// For scaling text when zooming

	// Shift comp to near grid centre
	const int ROWS( def.GetScreenRows() );
	const int COLS( def.GetScreenCols() );

	const Rect rect(def.GetGridRowMin(), def.GetGridRowMax(), def.GetGridColMin(), def.GetGridColMax());

	QPainter painter;
	const int reqWidth  = W * COLS;
	const int reqHeight = W * ROWS;
	if ( m_mainPixmap.width() != reqWidth || m_mainPixmap.height() != reqHeight )
	{
		m_mainPixmap = QPixmap(reqWidth, reqHeight);
		m_mainPixmap.setDevicePixelRatio(1.0);
	}
	painter.begin(&m_mainPixmap);	// Paint to main pixmap

	SetQuality(painter);

	const QColor backgroundColor = GetBackgroundColor();
	m_backgroundPen.setColor(backgroundColor);
	m_backgroundBrush.setColor(backgroundColor);

	painter.fillRect(m_XGRIDOFFSET, m_YGRIDOFFSET, reqWidth, reqHeight, backgroundColor);

	m_blackPen.setWidth(0);
	m_whitePen.setWidth(0);
	painter.setPen(m_blackPen);
	painter.setBrush(Qt::NoBrush);

	int X(0), Y(0), L(0), R(0), T(0), B(0);

	// Draw rect around whole board area =========================================================
	int dummy;
	GetLRTB(board, 110, 0, 0, L, dummy, T, dummy);				// 110% size square
	GetLRTB(board, 110, ROWS-1, COLS-1, dummy, R, dummy, B);	// 110% size square
	painter.drawRect(L, T, R-L, B-T);

	// Get footprint bounds ======================================================================
	GetLRTB(board, rect, L, R, T, B);
	L -= C; R += C; T -= C; B += C;
	const int AXIS_X = ( L + R ) / 2;
	const int AXIS_Y = ( T + B ) / 2;

	// Draw shapes ===============================================================================
	painter.save();
	painter.translate(AXIS_X, AXIS_Y);

	for (const auto& mapObj : def.GetShapes() )
	{
		const Shape& s = mapObj.second;

		if ( s.GetDrawFill() )
		{
			const MyRGB& fillColor = s.GetFillColor();
			m_varBrush.setColor(QColor(fillColor.GetR(),fillColor.GetG(),fillColor.GetB(),255));
		}
		const bool bCurrentShape = ( mapObj.first == def.GetCurrentShapeId() );
		m_blackPen.setWidth( bCurrentShape ? 3 : 2 );

		painter.setPen( s.GetDrawLine() ? m_blackPen : Qt::NoPen );
		painter.setBrush( s.GetDrawFill() ? m_varBrush : Qt::NoBrush );

		painter.save();
		auto DX = s.GetDX() * W;
		auto DY = s.GetDY() * W;
		auto X  = DX * 0.5;
		auto Y  = DY * 0.5;
		painter.translate( s.GetCX() * W, s.GetCY() * W );
		painter.rotate( -s.GetA3() );	// A3 > 0 ==> CCW
		switch( s.GetType() )
		{
			case SHAPE::LINE:			painter.drawLine(-X, -Y, X, Y);		break;
			case SHAPE::RECT:			painter.drawRect(-X, -Y, DX, DY);	break;
			case SHAPE::ROUNDED_RECT:	painter.drawRoundedRect(-X, -Y, DX, DY, 0.35 * W, 0.35 * W);	break;
			case SHAPE::ELLIPSE:		painter.drawEllipse(-X, -Y, DX, DY);	break;
			case SHAPE::ARC:			painter.drawArc(  -X, -Y, DX, DY, s.GetA1() * 16, s.GetAlen() * 16);	break;
			case SHAPE::CHORD:			painter.drawChord(-X, -Y, DX, DY, s.GetA1() * 16, s.GetAlen() * 16);	break;
			default: assert(0);	// Unhandled shape
		}
		if ( bCurrentShape )
		{
			// Show the base rect/ellipse for line/arc/chord
			painter.setPen(m_dotPen);
			painter.setBrush(Qt::NoBrush);
			switch( s.GetType() )
			{
				case SHAPE::LINE:	painter.drawRect(-X, -Y, DX, DY);	break;
				case SHAPE::ARC:
				case SHAPE::CHORD:	painter.drawEllipse(-X, -Y, DX, DY);	break;
				default:			break;
			}
		}
		painter.restore();
	}
	painter.restore();
	//============================================================================================

	// Draw grid points ==========================================================================
	if ( board.GetShowGrid() )
	{
		for (int j = 0; j < ROWS; j++)	for (int i = 0; i < COLS; i++)
		{
			GetXY(board, j, i, X, Y);
			painter.drawPoint(X, Y);
		}
	}

	// Draw dashed rect around footprint boundary and along central axes =========================
	painter.setPen(m_dashPen);
	painter.setBrush(Qt::NoBrush);
	painter.drawRect(L, T, R-L, B-T);
	painter.setPen(m_dotPen);
	painter.drawLine(0, AXIS_Y, reqWidth, AXIS_Y);
	painter.drawLine(AXIS_X, 0, AXIS_X, reqHeight);

	// Draw pins =================================================================================
	QFont pinsFont = painter.font();	// Copy of current font
	pinsFont.setPointSize( m_board.GetTextSizePins() );
	painter.setFont(pinsFont);

	m_varBrush.setColor(QColor(192,192,255,128));	// Light blue

	int iPinId(0);
	for (int j = 0, jMax = grid.GetRows(); j < jMax; j++)
	for (int i = 0, iMax = grid.GetCols(); i < iMax; i++, iPinId++)
	{
		auto p = grid.Get(0,j,i);	// Always layer 0 for component editor
		GetXY(board, def.GetGridRowMin() + j, def.GetGridColMin() + i, X, Y );

		// Write pin labels
		painter.save();
		painter.translate(X, Y);

		painter.setPen(Qt::NoPen);
		painter.setBrush(p->GetSurface() == SURFACE_FREE ? Qt::NoBrush :
						 p->GetSurface() == SURFACE_HOLE ? m_darkBrush : m_varBrush);	//TODO Handle SURFACE_GAP, SURFACE PLUG in future
		painter.drawRect(-C,-C,W,W);
		if ( iPinId == def.GetCurrentPinId() )
		{
			painter.setPen(m_redPen);
			painter.drawEllipse(-C,-C,W,W);
		}
		if ( p->GetIsPin() )
		{
			painter.setPen(m_blackPen);
			painter.rotate(270);
			painter.scale(dTextScale, dTextScale);
			painter.drawText(0,0,0,0, Qt::TextDontClip | Qt::AlignCenter, GetDefaultPinLabel(p->GetPinIndex()).c_str());
		}
		painter.restore();
	}

	painter.end();
}

void MainWindow::PaintBoard()	// The paint method in "circuit layout mode"
{
	Board&			 board			= m_board;
	CompManager&	 compMgr		= board.GetCompMgr();
	ColorManager&	 colorMgr		= board.GetColorMgr();

	CreatePixmapCache(board, colorMgr);	// Sets color saturation, then builds pixmaps if the cache is empty

	const TRACKMODE& trackMode		= board.GetTrackMode();
	const COMPSMODE& compMode		= board.GetCompMode();
	const bool&		 bVero			= board.GetVeroTracks();
	const bool		 bDiagsOK		= ( board.GetDiagsMode() != DIAGSMODE::OFF );
	const bool		 bMinDiags		= ( board.GetDiagsMode() == DIAGSMODE::MIN );
	const bool		 bMono			= trackMode == TRACKMODE::MONO;
	const bool		 bPCB			= trackMode == TRACKMODE::PCB;
	const bool		 bGroundFill	= !bVero && ( bMono || bPCB ) && board.GetGroundFill();
	const bool		 bPixmapCache	= !bVero && !bPCB && !bGroundFill && !m_bWritePDF;
	const bool		 bDirect		= !bVero && !bPixmapCache && !bGroundFill;
	const int&		 layer			= board.GetCurrentLayer();
	const int&		 groundNodeId	= ( layer == 0 ) ? board.GetGroundNodeId0() :  board.GetGroundNodeId1();
	const bool		 bWiresAsTracks	= m_bWriteGerber && m_bTwoLayers && board.GetLyrs() == 1;	// true ==> Convert wires to tracks on the top layer
	const int&		 W				= board.GetGRIDPIXELS();		// Square width in pixels
	const int		 C				= W >> 1;						// Half square width in pixels
	const int		 D				= board.GetHalfPadWidth();		// Half pad width in pixels
	const int		 H				= (int) ceil(1.414 * board.GetHalfTrackWidth());
	const int		 iHalfGap		= std::max(1, W / 12);			// For vero only
	const int		 iGap			= iHalfGap + iHalfGap;			// For vero only
	const int		 iWirePenWidth	= D / 4;						// For wires with no NodeID
	const int		 iWireBoxWidth	= 3 * iWirePenWidth;			// For wires with no NodeID
	const double	 dTextScale		= ( m_bWritePDF ) ? (48.0 / W) : (W / 24.0);	// For scaling text when zooming
	if ( bVero && trackMode != TRACKMODE::OFF ) board.CalcSolder();	// Calculate positions of solder blobs for stripboard builds

	// Pre-process component list for rendering
	std::vector<const Component*> sortedComps;
	if ( compMode != COMPSMODE::OFF || bMono || bPCB )
	{
		compMgr.CalculateWireShifts();			// Work out shifts for overlaid wires
		compMgr.GetSortedComps(sortedComps);	// Sorted so "plug" components get rendered last
	}

	board.CalculateColors();	// Work out best way to color things

	// Get bounds to minimise looping
	int minRow, minCol, maxRow, maxCol;
	board.GetBounds(minRow, minCol, maxRow, maxCol);

	GPainter painter;	// Works like QPainter unless you give it a GStream for Gerber

	QPdfWriter*	pdfWriter = nullptr;
	if ( m_bWritePDF )
	{
		pdfWriter = new QPdfWriter(m_pdfFileName);
		pdfWriter->setCreator("VeroRoute");
		pdfWriter->setPageSize(QPagedPaintDevice::A4);
		pdfWriter->setPageOrientation(QPageLayout::Landscape);
		pdfWriter->setResolution(1200);
		painter.begin(pdfWriter);	// Paint to PDF file
	}
	else if ( m_bWriteGerber )
	{
		assert( bPCB );
		if ( board.GetFlipH() || board.GetFlipV() ) return;		// No mirrored Gerber

		auto& os = m_gWriter.GetStream(GFILE::GTO);	// Top silk layer
		os.SetPolarity(GPOLARITY::DARK);
		os.ClearBuffers();
		painter.SetGStream(&os);
	}
	else
	{
		const int reqWidth  = W * board.GetCols();
		const int reqHeight = W * board.GetRows();
		if ( m_mainPixmap.width() != reqWidth || m_mainPixmap.height() != reqHeight )
		{
			m_mainPixmap = QPixmap(reqWidth, reqHeight);
			m_mainPixmap.setDevicePixelRatio(1.0);
		}
		painter.begin(&m_mainPixmap);	// Paint to main pixmap
	}

	if ( !bPCB )
	{
		SetQuality(painter);

		if ( board.GetFlipH() )
		{
			painter.translate(2*m_XGRIDOFFSET + W * board.GetCols(), 0);
			painter.scale(-1, 1);	// Mirror L-R
		}
		if ( board.GetFlipV() )
		{
			painter.translate(0, 2*m_YGRIDOFFSET + W * board.GetRows());
			painter.scale(1, -1);	// Mirror T-B
		}
	}

	const QColor backgroundColor = ( m_bWritePDF ) ? Qt::white : GetBackgroundColor();
	m_backgroundPen.setColor(backgroundColor);
	m_backgroundBrush.setColor(backgroundColor);

	int X(0), Y(0), L(0), R(0), T(0), B(0), cR(0), cG(0), cB(0);

	// Draw board background
	QPolygonF edge;		edge.clear();		// The board outline in Gerber
	QPolygonF gndPoly;	gndPoly.clear();	// The rectangle for ground fill
	if ( m_bWriteGerber )
	{
		// Grow board outline to guarantee separation from tracks and ground
		const double dEdge = board.GetEdgeWidth();
		const double R(W * board.GetCols()), B(W * board.GetRows());
		const int& X = m_XGRIDOFFSET; const int& Y = m_YGRIDOFFSET;
		gndPoly << QPointF(X,     Y);		edge << QPointF(X	  - dEdge, Y	 - dEdge);
		gndPoly << QPointF(X + R, Y);		edge << QPointF(X + R + dEdge, Y	 - dEdge);
		gndPoly << QPointF(X + R, Y + B);	edge << QPointF(X + R + dEdge, Y + B + dEdge);
		gndPoly << QPointF(X,     Y + B);	edge << QPointF(X	  - dEdge, Y + B + dEdge);
		if ( bGroundFill )
		{
			m_gWriter.GetStream(GFILE::GTL).DrawRegion(gndPoly);	// Top    copper layer
			m_gWriter.GetStream(GFILE::GBL).DrawRegion(gndPoly);	// Bottom copper layer
		}
	}
	else
	{
		const int iGroundFillColor = ( bPCB ) ? ( layer == 0 ? MY_LYR_BOT : MY_LYR_TOP ) : MY_BLACK;
		colorMgr.GetPixmapRGB(iGroundFillColor, cR, cG, cB);
		const QColor groundFillColor(cR, cG, cB, 255);
		painter.fillRect(m_XGRIDOFFSET, m_YGRIDOFFSET, W * board.GetCols(), W * board.GetRows(), bGroundFill ? groundFillColor : backgroundColor);
	}

	// Draw rect around whole board area =========================================================
	int dummy;
	GetLRTB(board, 110, 0, 0, L, dummy, T, dummy);	// 110% size square
	GetLRTB(board, 110, board.GetRows()-1, board.GetCols()-1, dummy, R, dummy, B);	// 110% size square

	if ( m_bWriteGerber )
	{
		m_gWriter.GetStream(GFILE::GKO).DrawLoop(GPEN::MIL10, edge);	// Board outline layer
	}
	else
	{
		const int iPenWidth = ( bPCB ) ? W * 0.100 : 0;	// Like GPEN::MIL10 used for Gerber
		m_blackPen.setWidth(iPenWidth);
		m_whitePen.setWidth(iPenWidth);
		painter.setPen(GetBackgroundColor() == Qt::black ? m_whitePen : m_blackPen);
		painter.setBrush(Qt::NoBrush);
		painter.drawRect(L, T, R-L, B-T);
	}

	// Draw grid points ==========================================================================
	if ( !bPCB && board.GetShowGrid() )
	{
		for (int j = 0; j < board.GetRows(); j++)	for (int i = 0; i < board.GetCols(); i++)
		{
			const Element* pC = board.Get(layer, j, i);
			if ( trackMode == TRACKMODE::OFF || ( !pC->GetHasPin() && pC->GetNodeId() == BAD_NODEID ) )
			{
				GetXY(board, j, i, X, Y);
				painter.drawPoint(X, Y);
			}
		}
	}

	// Draw tracks ===============================================================================
	if ( bPCB || trackMode != TRACKMODE::OFF )	// Force tracks in PCB mode
	{
		painter.save();

		const bool	bGreyPads	= bPCB && !m_bWriteGerber;
		const int	numLoops	= ( ( bPixmapCache || bGroundFill ) ? 2 : 1 ) + ( bGreyPads ? 1 : 0);
		const QColor	padGrey(200,200,200,255);
		// bGroundFill		==> 1st pass draws fat tracks in white, 2nd pass draws tracks
		// bPixmapCache		==> 1st pass draws the pixmaps,			2nd pass fixes up diagonals
		// bGreyPads		==> A final pass will draw the pads in grey
		for (int iLoop = 0; iLoop < numLoops; iLoop++)
		{
			const bool bLastPass = ( iLoop == numLoops - 1 );
			const bool bGap		 = bGroundFill && iLoop == 0;
			const bool bDrawGrey = ( bGreyPads && bLastPass );
			if ( m_bWriteGerber )
			{
				m_gWriter.GetStream(GFILE::GTS).ClearBuffers();	// Top solder mask layer
				m_gWriter.GetStream(GFILE::GTL).ClearBuffers();	// Top copper layer
				m_gWriter.GetStream(GFILE::GTL).SetPolarity(bGap ? GPOLARITY::CLEAR : GPOLARITY::DARK);
				m_gWriter.GetStream(GFILE::GBS).ClearBuffers();	// Bottom solder mask layer
				m_gWriter.GetStream(GFILE::GBL).ClearBuffers();	// Bottom copper layer
				m_gWriter.GetStream(GFILE::GBL).SetPolarity(bGap ? GPOLARITY::CLEAR : GPOLARITY::DARK);
				m_gWriter.GetStream(GFILE::DRL).ClearBuffers();	// Drill file
			}

			for (int j = minRow; j <= maxRow; j++)
			for (int i = minCol; i <= maxCol; i++)
			{
				const Element*	pC				= board.Get(layer, j, i);
				const int&		nodeId			= pC->GetNodeId();
				const int		colorId			= colorMgr.GetColorId(nodeId);
				const bool		bPin			= pC->GetHasPin();	// true ==> real pin
				const bool		bWire			= pC->GetHasWire();
				const bool		bWireAsVia		= bWire && bWiresAsTracks;	// true ==> draw small via pad
				const int		iPerimeterCode	= pC->GetPerimeterCode(bDiagsOK, bMinDiags);	// 0 to 255
				const bool		bVia			= pC->GetIsVia();

				if ( colorId == BAD_COLORID && !bWire ) continue;	// Usually don't color places with no NodeID assigned unless they are wire ends

				// Use GetPixmapRGB for pixmaps.  It can handle MY_GREY, MY_BLACK as special cases
				const bool		bInvalidColor	=  colorId == BAD_COLORID ||
												  ( ( bMono || bPCB ) && nodeId != GetCurrentNodeId() );
				const int		iEffColorId		= ( bInvalidColor )	? ( bPCB ? ( layer == 0 ? MY_LYR_BOT : MY_LYR_TOP ) : MY_BLACK )
												: ( nodeId == GetCurrentNodeId() ) ? MY_GREY : ( colorId % MYNUMCOLORS );

				colorMgr.GetPixmapRGB(iEffColorId, cR, cG, cB);
				const QColor color(cR, cG, cB, 255);

				GetLRTB(board, 100, j, i, L, R, T, B);	// 100% size square
				const int X((L+R)/2), Y((T+B)/2);
				const QPointF pCentre(X,Y);

				// Common special case: Draw blank wire-ends as squares (so we can easily see them)
				if ( colorId == BAD_COLORID && bWire )
				{
					QPen& wirePen = ( bGroundFill ) ? m_whitePen : m_blackPen;
					wirePen.setWidth(iWirePenWidth);
					painter.setPen(wirePen);
					painter.setBrush(Qt::NoBrush);
					painter.drawRect(X-iWireBoxWidth, Y-iWireBoxWidth, iWireBoxWidth*2, iWireBoxWidth*2);
					wirePen.setWidth(0);
					continue;	// Next grid square
				}

				// Note that bVero, bPixmapCache, bGroundFill, bDirect are mutually exclusive

				if ( bVero ) // Vero shows squares and strips with holes
				{
					if ( bPCB ) continue;	// Vero mode not compatible with PCB mode

					const bool bVertical = board.GetVerticalStrips();
					if ( bVertical )
					{
						L += iHalfGap;
						R -= iHalfGap;
					}
					else
					{
						T += iHalfGap;
						B -= iHalfGap;
					}
					painter.setBrush(color);
					painter.setPen(Qt::NoPen);
					painter.drawRect(L, T, R-L, B-T);
					m_backgroundPen.setWidth(std::max(1, W/4));
					painter.setPen(m_backgroundPen);
					painter.setBrush(Qt::NoBrush);
					painter.drawPoint(pCentre);

					// Emphasize strip breaks
					painter.setPen(Qt::NoPen);
					painter.setBrush(m_backgroundBrush);
					if ( bVertical )
					{
						if ( j > minRow && pC->GetNbr(NBR_T)->IsClash(nodeId) ) painter.drawRect(L, T-iHalfGap, R-L, iGap);
						if ( j < maxRow && pC->GetNbr(NBR_B)->IsClash(nodeId) ) painter.drawRect(L, B-iHalfGap, R-L, iGap);
					}
					else
					{
						if ( i > minCol && pC->GetNbr(NBR_L)->IsClash(nodeId) ) painter.drawRect(L-iHalfGap, T, iGap, B-T);
						if ( i < maxCol && pC->GetNbr(NBR_R)->IsClash(nodeId) ) painter.drawRect(R-iHalfGap, B, iGap, B-T);
					}
				}
				if ( bPixmapCache )	// Draw track "blobs" and pads using pre-calculated pixmaps for speed
				{
					assert( !bPCB );
					if ( iLoop == 0 )
					{
						// Draw background square first in relevant color
						painter.setPen(Qt::NoPen);
						painter.setBrush(color);
						painter.drawRect(L, T, R-L, B-T);

						// Set the area that is not in the "blob" to the background color
						painter.drawPixmap(L, T, *(m_ppPixmapBlob[iPerimeterCode]));

						// Draw pad/via
						if ( bPin ) painter.drawPixmap(L+C-D, T+C-D, *(m_ppPixmapPad[iEffColorId]));
						if ( bVia ) painter.drawPixmap(L+C-D, T+C-D, *(m_ppPixmapVia[iEffColorId]));
					}
					else if ( iLoop == 1 )
					{
						// Read flags for LT and RT so we can fill diagonal gaps produced on previous iLoop
						const bool bUsedLT = ReadCodeBit(NBR_LT, iPerimeterCode);
						const bool bUsedRT = ReadCodeBit(NBR_RT, iPerimeterCode);
						if ( bUsedLT ) painter.drawPixmap(L-H, T-H, *(m_ppPixmapDiag[iEffColorId]));
						if ( bUsedRT ) painter.drawPixmap(R-H, T-H, *(m_ppPixmapDiag[iEffColorId + NUM_PIXMAP_COLORS]));
					}
				}
				if ( bGroundFill )	// Draw track "blobs" and pads directly (PDF/Gerber)
				{
					if ( iLoop == 0 )
					{
						if ( nodeId != groundNodeId )	// Only the non-ground tracks have a "white" surround
							PaintBlob(board, painter, backgroundColor, pCentre, iPerimeterCode, true);	// Draw fat "white" track blob
						if ( bWireAsVia || bVia )
							PaintViaPad(board, painter, backgroundColor, pCentre, true);				// Draw fat "white" via-pad
						else if ( bPin )
							PaintPad(board, painter, backgroundColor, pCentre, true);					// Draw fat "white" pad
					}
					else if ( iLoop == 1 )	// Draw track "blobs" and pads directly
					{
						PaintBlob(board, painter, color, pCentre, iPerimeterCode);	// Draw track blob
						if ( !bGreyPads )
						{
							if ( bWireAsVia || bVia )
								PaintViaPad(board, painter, color, pCentre);		// Draw via-pad same color as track
							else if ( bPin )
								PaintPad(board, painter, color, pCentre);			// Draw pad same color as track
						}
					}
					else if ( bDrawGrey )
					{
						if ( bPin )
							PaintPad(board, painter, padGrey, pCentre);		// Draw grey pad
						else if ( bVia )
							PaintViaPad(board, painter, padGrey, pCentre);	// Draw grey via-pad
					}
				}
				if ( bDirect )	// Draw track "blobs" and pads directly (PDF/Gerber)
				{
					if ( iLoop == 0 )
					{
						PaintBlob(board, painter, color, pCentre, iPerimeterCode);				// Draw track blob
						if ( !bGreyPads )
						{
							if ( bWireAsVia || bVia )
								PaintViaPad(board, painter, color, pCentre);	// Draw via-pad same color as track
							else if ( bPin )
								PaintPad(board, painter, color, pCentre);		// Draw pad same color as track
						}
					}
					else if ( bDrawGrey )
					{
						if ( bPin )
							PaintPad(board, painter, padGrey, pCentre);		// Draw grey pad
						else if ( bVia )
							PaintViaPad(board, painter, padGrey, pCentre);	// Draw grey via-pad
					}
				}
			}
			if ( bWiresAsTracks )
			{
				for (const auto& pComp : sortedComps)	// Iterate sorted components
				{
					const Component& comp = *pComp;
					if ( comp.GetType() != COMP::WIRE || !comp.GetIsPlaced() ) continue;	// Only want placed wires
					if ( compMgr.GetWireShift( &comp ) != 0 ) continue;	//TODO Probably not a good enough check since wires may cross yet have no shift

					QPolygonF polygonF; polygonF.clear();
					GetXY(board, comp.GetRow(), comp.GetCol(), X, Y);
					polygonF << QPointF(X, Y);
					GetXY(board, comp.GetRow() + comp.GetCompRows() - 1, comp.GetCol() + comp.GetCompCols() - 1, X, Y);
					polygonF << QPointF(X, Y);
					m_gWriter.GetStream(GFILE::GTL).AddTrack(bGap ? GPEN::TRACK_GAP : GPEN::TRACK, polygonF);
				}
			}
			if ( m_bWriteGerber )
			{
				m_gWriter.GetStream(GFILE::GTS).DrawBuffers();	// Top solder mask layer
				m_gWriter.GetStream(GFILE::GTL).DrawBuffers();	// Top copper layer
				m_gWriter.GetStream(GFILE::GBS).DrawBuffers();	// Bottom solder mask layer
				m_gWriter.GetStream(GFILE::GBL).DrawBuffers();	// Bottom copper layer
				m_gWriter.GetStream(GFILE::DRL).DrawBuffers();	// Drill file
			}
		}	// Next iLoop
		painter.restore();
	}

	// Draw target board area ====================================================================
	if ( m_board.GetShowTarget() && !bMono && !bPCB )
	{
		const int targetT = ( board.GetRows() - m_board.GetTargetRows() ) / 2;
		const int targetB = targetT + m_board.GetTargetRows() - 1;
		const int targetL = ( board.GetCols() - m_board.GetTargetCols() ) / 2;
		const int targetR = targetL + m_board.GetTargetCols() - 1;

		int l(0), r(0), t(0), b(0);
		GetXY(board, 0, 0, L, T);
		GetXY(board, board.GetRows()-1, board.GetCols()-1, R, B);
		GetXY(board, targetT, targetL, l, t);
		GetXY(board, targetB, targetR, r, b);
		L -= C; l -= C; T -= C; t -= C;
		R += C; r += C; B += C; b += C;

		painter.save();
		m_varBrush.setColor( QColor(0,128,128,32) );	// Very transparent light cyan tint
		painter.setPen(Qt::NoPen);
		painter.setBrush(m_varBrush);
		painter.drawRect(L, T, R-L, t-T);
		painter.drawRect(L, t, l-L, b-t);
		painter.drawRect(r, t, R-r, b-t);
		painter.drawRect(L, b, R-L, B-b);
		painter.restore();
	}

	// Draw hatched lines ========================================================================
	if ( !m_bWriteGerber && trackMode != TRACKMODE::OFF )
	{
		if ( board.GetRoutingEnabled() || GetCurrentNodeId() != BAD_NODEID )
		{
			painter.save();
			m_redPen.setWidth(W / 8);	// Use red pen in PCB/Mono mode (easier to see)
			m_yellowPen.setWidth(W / 8);
			m_backgroundPen.setWidth(0);
			painter.setBrush(Qt::NoBrush);
			for (int j = minRow; j <= maxRow; j++)
			for (int i = minCol; i <= maxCol; i++)
			{
				const Element* pC = board.Get(layer, j, i);
				if ( pC->GetNodeId() == BAD_NODEID ) continue;
				GetLRTB(board, 100, j, i, L, R, T, B);	// 100% size square

				if ( pC->ReadFlagBits(AUTOSET) && !pC->ReadFlagBits(USERSET) )
				{
					painter.setPen(m_backgroundPen);
					painter.drawLine(L, T, R, B);		// Draw "\" (hatched) line
					painter.drawLine(L+C, T, R, B-C);	// Draw "\" (hatched) line
					painter.drawLine(L, T+C, R-C, B);	// Draw "\" (hatched) line
					painter.drawLine(L, B, R, T);		// Draw "/" (hatched) line
					painter.drawLine(L+C, B, R, T+C);	// Draw "/" (hatched) line
					painter.drawLine(L, B-C, R-C, T);	// Draw "/" (hatched) line
				}
				if ( pC->GetNodeId() == GetCurrentNodeId() && pC->GetMH() == BAD_MH )
				{
					painter.setPen(bPCB || bMono ? m_redPen : m_yellowPen);
					painter.drawLine(L, B, R, T);		// Draw "/" (hatched) line
				}
			}
			painter.restore();
		}
	}

	// Draw solder ===============================================================================
	if ( !bPCB && bVero && trackMode != TRACKMODE::OFF )
	{
		const bool bVertical = board.GetVerticalStrips();
		painter.save();
		painter.setPen(Qt::NoPen);
		painter.setBrush(m_darkBrush);
		for (int j = minRow; j <= maxRow; j++)
		for (int i = minCol; i <= maxCol; i++)
		{
			const Element* pC = board.Get(layer, j, i);
			if ( !pC->GetSolderR() ) continue;
			GetLRTB(board, 100, j, i, L, R, T, B);	// 100% size square
			if ( bVertical )
				painter.drawEllipse(R-(W/3), (T+B)/2 - (W/6), (2*W)/3, W/3);
			else
				painter.drawEllipse((L+R)/2 - (W/6), B-(W/3), W/3, (2*W)/3);
		}
		painter.restore();
	}

	// Draw vias =================================================================================
	if ( !m_bWriteGerber && !bVero && ( bPCB || trackMode != TRACKMODE::OFF ) )	// Force in PCB mode
	{
		painter.save();
		m_backgroundPen.setWidth(0);
		painter.setPen(m_backgroundPen);
		painter.setBrush(m_backgroundBrush);
		for (int j = minRow; j <= maxRow; j++)
		for (int i = minCol; i <= maxCol; i++)
		{
			const Element*	pC		= board.Get(layer, j, i);
			const bool		bVia	= pC->GetIsVia();
			if ( bVia )
			{
				GetLRTB(board, board.GetVIAHOLE_PERCENT(), j, i, L, R, T, B);						
				painter.drawEllipse(L, T, R-L, B-T);	// A pin is drawn with a circle
			}
		}
		painter.restore();
	}

	// Draw Component outlines and pins ==========================================================
	QPen& penPlaced	= ( bMono ) ? m_lightBluePen : ( bPCB ) ? m_whitePen : m_blackPen;	// For placed (non-floating) components
	QPen  fillBlackPen = m_blackPen;	// Used for lines in the component pixmap
	fillBlackPen.setWidth(2);

	if ( compMode != COMPSMODE::OFF || bMono || bPCB )	// Mono/PCB modes still need pin holes drawn
	{
		for (const auto& pComp : sortedComps)	// Iterate sorted components
		{
			const Component& comp			= *pComp;
			const COMP&		 compType		= comp.GetType();
			const char&		 compDirection	= comp.GetDirection();
			const bool		 bMark			= compType == COMP::MARK;
			const bool		 bWire			= compType == COMP::WIRE;
			if ( bPCB && bMark )	continue;	// Don't show markers in PCB mode
			if ( m_bWriteGerber && !comp.GetIsPlaced() ) continue;	// Don't write floating components to Gerber
			if ( bWiresAsTracks && bWire && compMgr.GetWireShift(&comp) == 0 ) continue;	//TODO Probably not a good enough check since wires may cross yet have no shift
			const bool		 bPinLabels		= (comp.GetPinFlags() & PIN_LABELS) > 0;
			const bool		 bRectPins		= (comp.GetPinFlags() & PIN_RECT)   > 0;
			const bool		 bHighlightComp	= board.GetGroupMgr().GetIsUserComp( comp.GetId() );
			const int		 jComp			= comp.GetRow();
			const int		 iComp			= comp.GetCol();

			// Begin draw component fill + outline -----------------------------------------------
			if ( compMode != COMPSMODE::OFF && !comp.GetShapes().empty() )
			{
				// Set pen width.  Selected component shown thicker than normal components
				if ( comp.GetIsPlaced() )
				{
					if ( bPCB )	// Use floating point pen width to better match Gerber output
						penPlaced.setWidthF( bHighlightComp ? ( board.GetSilkWidth() * 1.5 )
															: ( bMark ? ( board.GetSilkWidth() * 0.5 )
																	  :   board.GetSilkWidth() ) );
					else
						penPlaced.setWidth( bHighlightComp ? 3 : bMark ? 1 : 2 );
				}
				else
					m_redPen.setWidth(4);	// Make floating components stand out in red

				QPen& linePen = ( comp.GetIsPlaced() ) ? penPlaced : m_redPen;
				painter.setPen(linePen);
				painter.setBrush(Qt::NoBrush);

				GetXY(board, comp, X, Y);	// Get footprint centre

				// Implement wire shift
				if ( bWire && comp.GetIsPlaced() )
				{
					if ( comp.GetCompRows() == 1 )	// Horizontal
						Y += compMgr.GetWireShift( &comp ) * 0.1 * W;
					else
						X += compMgr.GetWireShift( &comp ) * 0.1 * W;
				}

				const bool bFill = !bMono && !bPCB && board.GetFillSaturation() > 0;	// No component fill in Mono/PCB mode

				double SL,ST,SR,SB;
				comp.GetSafeBounds(SL,SR,ST,SB);
				const double dReqW = (1 + SR - SL) * W;
				const double dReqH = (1 + SB - ST) * W;
				GPainter painterTmp;
				QPixmap tmpPixmap(dReqW, dReqH);
				tmpPixmap.setDevicePixelRatio(1.0);
				const MyRGB		msk	= comp.GetNewColor();	// We'll mask out pixels with this color at the end
				const QColor	maskColor(msk.GetR(), msk.GetG(), msk.GetB());

				if ( bFill )
				{
					painterTmp.begin(&tmpPixmap);
					painterTmp.fillRect(0,0,dReqW, dReqH, maskColor);	// This will be turned transparent later
					painterTmp.setPen(Qt::NoPen);
				}

				const int iLoopStart = ( bFill ) ? 0 : 1;
				for (int iLoop = iLoopStart; iLoop < 2; iLoop++)
				{
					GPainter* pPainter = ( iLoop == 0 ) ? &painterTmp : &painter;

					pPainter->save();	// Save (original axes)
					if ( iLoop == 0 )
						pPainter->translate(dReqW*0.5, dReqH*0.5);	// Shape coordinates are relative to pixmap centre
					else
						pPainter->translate(X, Y);					// Shape coordinates are relative to footprint centre

					if ( bFill && iLoop == 1 )	// Draw the pixmap created on the previous pass
					{
						painter.setOpacity( /*bWire ? 1.0 :*/ board.GetFillSaturation() * 0.01);
						painter.drawPixmap(-dReqW*0.5, -dReqH*0.5, tmpPixmap);
						painter.setOpacity(1.0);
					}

					switch( compDirection )	// Rotated axes at footprint centre
					{
						case 'W':	break;
						case 'E':	pPainter->rotate(180);	break;
						case 'N':	pPainter->rotate(90);	break;
						case 'S':	pPainter->rotate(270);	break;
					}

					const size_t numShapes = comp.GetNumShapes();
					for (size_t i = 0; i < numShapes; i++)
					{
						const Shape& s = comp.GetShape(i);

						if ( iLoop == 0 && !s.GetDrawFill() ) continue;
						if ( iLoop == 1 && (s.GetDrawFill() || !s.GetDrawLine()) ) continue;
						if ( iLoop == 0 )	// Drawing fill
						{
							const MyRGB& rgb	= s.GetFillColor();
							m_varBrush.setColor( QColor(rgb.GetR(), rgb.GetG(), rgb.GetB()) );
							pPainter->setBrush(m_varBrush);
							pPainter->setPen( s.GetDrawLine() ? fillBlackPen : Qt::NoPen );
						}

						pPainter->save();
						pPainter->translate( s.GetCX() * W, s.GetCY() * W );	// Translate to shape centre
						pPainter->rotate( -s.GetA3() );	// A3 > 0 ==> CCW		// Rotate about shape centre

						auto DX = s.GetDX() * W;
						auto DY = s.GetDY() * W;
						auto X  = DX * 0.5;
						auto Y  = DY * 0.5;

						switch( s.GetType() )
						{
							case SHAPE::LINE:			pPainter->drawLine(-X, -Y, X, Y);	break;
							case SHAPE::RECT:			pPainter->drawRect(-X, -Y, DX, DY);	break;
							case SHAPE::ROUNDED_RECT:	pPainter->drawRoundedRect(-X, -Y, DX, DY, 0.35 * W, 0.35 * W);	break;
							case SHAPE::ELLIPSE:		pPainter->drawEllipse(-X, -Y, DX, DY);	break;
							case SHAPE::ARC:			pPainter->drawArc(  -X, -Y, DX, DY, s.GetA1() * 16, s.GetAlen() * 16);	break;
							case SHAPE::CHORD:			pPainter->drawChord(-X, -Y, DX, DY, s.GetA1() * 16, s.GetAlen() * 16);	break;
							default: assert(0);	// Unhandled shape
						}
						pPainter->restore();
					}
					pPainter->restore();	// Restore (original axes)

					if ( iLoop == 0 )	// Finish off making the pixmap
					{
						painterTmp.end();
						// Turn the "mask" pixels transparent
						tmpPixmap.setMask( tmpPixmap.createMaskFromColor(maskColor, Qt::MaskInColor) );
					}
				}
			}
			// End draw component fill + outline -------------------------------------------------

			// Begin draw component pins ---------------------------------------------------------
			if ( !m_bWriteGerber )
			{
				QFont pinsFont = painter.font();	// Copy of current font
				pinsFont.setPointSize( m_board.GetTextSizePins() );
				painter.setFont(pinsFont);

				painter.save();
				if ( bMono || bPCB )
				{
					m_backgroundPen.setWidth(0);
					painter.setPen(m_backgroundPen);
					painter.setBrush(m_backgroundBrush);
				}
				else
				{
					penPlaced.setWidth(0);		// For pin labels
					m_redPen.setWidth(0);		// For pin labels
					m_darkGreyPen.setWidth(0);	// For pins
					painter.setPen(m_darkGreyPen);
					painter.setBrush(Qt::NoBrush);
				}

				if ( bMark )	// Markers are a special case since they don't actually have a pin !!!
				{
					if ( bMono )  // Only draw markers as pins in MONO mode
					{
						GetLRTB(board, board.GetHOLE_PERCENT(), jComp, iComp, L, R, T, B);
						painter.drawEllipse(L, T, R-L, B-T);	// A pin is drawn with a circle
					}
				}
				else		// Regular components/pads/wires ...
				{
					for (int jj = 0; jj < comp.GetCompRows(); jj++)
					for (int ii = 0; ii < comp.GetCompCols(); ii++)
					{
						const int j = jComp + jj;
						const int i = iComp + ii;

						const size_t iPinIndex = comp.GetCompElement(jj,ii)->GetPinIndex();
						if ( iPinIndex == BAD_PININDEX ) continue;

						if ( !comp.GetIsPlaced() )	// Color pins of floating components (if in Color mode)
						{
							const int&	nodeId	= comp.GetNodeId(iPinIndex);
							int			colorId	= colorMgr.GetColorId(nodeId);

							if ( colorId != BAD_COLORID && nodeId == GetCurrentNodeId() )
								colorId = MY_GREY;

							int cR, cG, cB;
							colorMgr.GetPixmapRGB(colorId, cR, cG, cB);

							const QColor color(cR, cG, cB, 255);
							m_varBrush.setColor(color);
							painter.setBrush( (bMono || bPCB) ? Qt::NoBrush : m_varBrush);	// No pin color fill in Mono/PCB mode
						}
						const int iPinSize  = ( bMono || bPCB || comp.GetIsPlaced() )
											? board.GetHOLE_PERCENT()
											: std::min(3*board.GetHOLE_PERCENT()/2, board.GetPAD_PERCENT());
						GetLRTB(board, iPinSize, j, i, L, R, T, B);
						// Stop pins vanishing if zoomed too far out
						if ( L == R ) { L--, R++; }
						if ( T == B ) { T--, B++; }

						if ( bPinLabels && !bMono && !bPCB && board.GetShowPinLabels() )	// No pin labels in Mono/PCB mode
						{
							// Write pin labels
							painter.save();

							painter.translate((L+R)/2, (T+B)/2);

							// Set text orientation
							switch( compDirection )
							{
								case 'W':
								case 'E':	painter.rotate(270);	break;
							}

							// Handle L/R pin label alignment
							int iFlag = comp.GetPinAlign(iPinIndex);
							if ( iFlag == Qt::AlignLeft || iFlag == Qt::AlignRight )
							{
								const bool bLeft = ( iFlag == Qt::AlignLeft );
								switch( compDirection )
								{
									case 'E':
									case 'S':	iFlag = bLeft ? Qt::AlignRight : Qt::AlignLeft;	// Swap align L/R
												painter.translate(bLeft ? C/2 : -C/2, 0);
												break;
									default:	painter.translate(bLeft ? -C/2 : C/2, 0);
								}
							}
							iFlag |= ( Qt::TextDontClip | Qt::AlignVCenter );

							painter.scale(dTextScale, dTextScale);
							painter.setPen( comp.GetIsPlaced() ? penPlaced : m_redPen);
							painter.drawText(0,0,0,0, iFlag, comp.GetPinLabel(iPinIndex).c_str());
							painter.restore();
						}
						else if ( bRectPins && !bMono && !bPCB )	// Draw switch pins as rectangles
						{
							const int d = std::max(1, static_cast<int>(iPinSize * W * 0.005));
							switch( compDirection )
							{
								case 'W':
								case 'E':	L -= d; R += d;	break;
								case 'N':
								case 'S':	T -= d; B += d;	break;
							}
							painter.drawRect(L, T, R-L, B-T);
						}
						else
							painter.drawEllipse(L, T, R-L, B-T);	// A regular pin is drawn as a circle
					}
				}
				painter.restore();
			}
			// End draw component pins -----------------------------------------------------------
		}
	}

	// Draw Component Text =======================================================================
	if ( compMode != COMPSMODE::OFF )
	{
		double dCopyTextScale = dTextScale;
		if ( bPCB )
		{
			dCopyTextScale *= m_board.GetTextSizeComp() * (20.0 / 243 );	// Scale to make the Gerber font size similar to regular component font size
		}
		else
		{
			QFont compFont = painter.font();	// Copy of current font
			compFont.setPointSize( m_board.GetTextSizeComp() );
			painter.setFont(compFont);
		}

		// Use floating point pen width to better match Gerber output.
		// Scale the pen width down to compensate for painter.scale() scaling things up in the loop below.
		const double dPenWidth = ( bPCB ) ? board.GetSilkWidth() / dCopyTextScale : 0;
		m_redPen.setWidthF(dPenWidth);	// Use red text for floating components
		penPlaced.setWidthF(dPenWidth);	// Use this for placed components

		for (const auto& mapObj : compMgr.GetMapIdToComp())	// Iterate components
		{
			const Component& comp			= mapObj.second;
			const COMP&		 compType		= comp.GetType();
			const char&		 compDirection	= comp.GetDirection();
			if ( compType == COMP::MARK || compType == COMP::WIRE ) continue;

			GetXY(board, comp, X, Y);	// Get footprint centre

			int offsetRow(0), offsetCol(0);
			comp.GetLabelOffsets(offsetRow, offsetCol);

			X += W * 0.0625 * offsetCol; // Offset for text is 1/16 of a grid square
			Y += W * 0.0625 * offsetRow; // Offset for text is 1/16 of a grid square

			painter.save();
			painter.translate(X, Y);
			if ( compDirection == 'N' || compDirection == 'S' )
				painter.rotate(270);

			const std::string& myStr = ( compMode == COMPSMODE::NAME )  ? comp.GetNameStr() :
									   ( compMode == COMPSMODE::VALUE ) ? comp.GetValueStr() : "";

			painter.scale(dCopyTextScale, dCopyTextScale);
			painter.setPen( comp.GetIsPlaced() ? penPlaced : m_redPen );
			painter.drawText(0,0,0,0, Qt::AlignCenter | Qt::TextDontClip, myStr.c_str(), bPCB);
			painter.restore();
		}
	}

	// Draw the User-defined "trax" component ====================================================
	if ( !m_bWriteGerber && ( compMode != COMPSMODE::OFF || trackMode != TRACKMODE::OFF ) )
	{
		Component& trax = compMgr.GetTrax();
		if ( trax.GetSize() > 0 )
		{
			if ( trax.GetIsPlaced() )
				m_varBrush.setColor( QColor(128,128,128,128) );	// light grey
			else
				m_varBrush.setColor( QColor(192,128,192,128) );	// magenta tint
			painter.setPen(Qt::NoPen);
			painter.setBrush(m_varBrush);

			const int&	rowTL		= trax.GetRow();
			const int&	colTL		= trax.GetCol();
			const int&	compCols	= trax.GetCompCols();
			const int&	compRows	= trax.GetCompRows();
			int jRow(rowTL);
			for (int j = 0; j < compRows; j++, jRow++)
			{
				int iCol(colTL);
				for (int i = 0; i < compCols; i++, iCol++)
				{
					if ( trax.GetCompElement(j, i)->ReadFlagBits(RECTSET) )
					{
						GetLRTB(board, 100, jRow, iCol, L, R, T, B);	// 100% size square
						painter.drawRect(L,T,R-L,B-T);
					}
				}
			}
		}
	}

	// Draw User-defined labels ==================================================================
	if ( !bPCB && board.GetShowText() )
	{
		QFont font = painter.font();	// Copy of current font
		TextManager& textMgr = m_board.GetTextMgr();
		for (const auto& mapObj : textMgr.GetMapIdToText())
		{
			const TextRect& rect = mapObj.second;
			if ( !rect.GetIsValid() ) continue;

			GetXY(board, rect.m_rowMin, rect.m_colMin, L, T);
			GetXY(board, rect.m_rowMax, rect.m_colMax, R, B);
			L -= C; T -= C; R += C; B += C;

			font.setBold( rect.GetStyle() & TEXT_BOLD );
			font.setItalic( rect.GetStyle() & TEXT_ITALIC );
			font.setUnderline( rect.GetStyle() & TEXT_UNDERLINE );
			font.setPointSize( rect.GetSize() );
			painter.setFont(font);

			m_varPen.setColor( QColor(rect.GetR(),rect.GetG(),rect.GetB(),255) );
			painter.setPen(m_varPen);
			painter.setBrush(Qt::NoBrush);
			painter.save();
			painter.translate((bMono && layer == 0) ? R : L, T);							// Mirror all text boxes in Mono mode for bottom layer
			painter.scale((bMono && layer == 0) ? -dTextScale : dTextScale, dTextScale);	// Mirror all text boxes in Mono mode for bottom layer
			painter.drawText(0,0,(R-L)/dTextScale,(B-T)/dTextScale, Qt::TextWordWrap | rect.GetFlags(), QString::fromStdString(rect.GetStr()));
			painter.restore();

			if ( mapObj.first == GetCurrentTextId() )
			{
				painter.setPen(m_dotPen);
				painter.drawRect(L, T, R-L, B-T);
				painter.drawRect(R-C, B-C, C, C);
			}
		}
	}

	if ( m_bWriteGerber )
		m_gWriter.GetStream(GFILE::GTO).DrawBuffers();	// Top silk layer

	painter.end();

	delete pdfWriter;
}

void MainWindow::GetFirstRowCol(int& iRow, int& iCol) const
{
	const int& W = m_board.GetGRIDPIXELS();	// Square width in pixels
	iRow = 1 + m_scrollArea->verticalScrollBar()->value()   / W;	// The first fully visible row
	iCol = 1 + m_scrollArea->horizontalScrollBar()->value() / W;	// The first fully visible col
}
		
void MainWindow::GetXY(const GuiControl& guiCtrl, double row, double col, int& X, int& Y) const
{
	// For rendering.
	// Takes a point in the Board and returns coordinates in the drawn image.
	const int& W = guiCtrl.GetGRIDPIXELS();	// Square width in pixels
	const int  C = W >> 1;					// Half square width in pixels
	X = m_XGRIDOFFSET + C + col * W;
	Y = m_YGRIDOFFSET + C + row * W;
}

void MainWindow::GetLRTB(const GuiControl& guiCtrl, double percent, double row, double col, int& L, int& R, int& T, int& B) const
{
	// For rendering.
	// Takes a point in the Board and returns bounding box coordinates in the drawn image.
	// Setting percent to 100 will make the bounding box the same size as the grid square.
	const int& W = guiCtrl.GetGRIDPIXELS();	// Square width in pixels
	int X(0), Y(0);
	GetXY(guiCtrl, row, col, X, Y);
	const int S = ( W * 0.005 * percent );
	L = X - S;	T = Y - S;
	R = X + S;	B = Y + S;
}

void MainWindow::GetLRTB(const GuiControl& guiCtrl, const Component& comp, int& L, int& R, int& T, int& B) const
{
	// For rendering.
	// Takes a component in the Board and returns bounding box coordinates in the drawn image.
	GetXY(guiCtrl, comp.GetRow(),		comp.GetCol(),		L, T);
	GetXY(guiCtrl, comp.GetLastRow(),	comp.GetLastCol(),	R, B);
}

void MainWindow::GetLRTB(const GuiControl& guiCtrl, const Rect& rect, int& L, int& R, int& T, int& B) const
{
	// For rendering.
	// Takes a Rect returns bounding box coordinates in the drawn image.
	GetXY(guiCtrl, rect.m_rowMin, rect.m_colMin, L, T);
	GetXY(guiCtrl, rect.m_rowMax, rect.m_colMax, R, B);
}

void MainWindow::GetXY(const GuiControl& guiCtrl, const Component& comp, int& X, int& Y) const
{
	// For rendering.
	// Takes a component in the Board and returns the footprint centre in the drawn image.
	int L, R, T, B;
	GetLRTB(guiCtrl, comp, L, R, T, B);
	X = ( L + R ) / 2;
	Y = ( T + B ) / 2;
}
