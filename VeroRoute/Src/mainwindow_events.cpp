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

// Following 2 are to slow down the auto-panning while moving components with the mouse
static std::chrono::steady_clock::time_point g_lastAutoPanTime;
static bool g_bHaveAutoPanned = false;

std::string mouseActionString("Action");	// For the undo/redo history


void MainWindow::GetPixMapXY(const QPoint& currentPoint, int& pixmapX, int& pixmapY) const
{
	const int iToolbarHeight = ( ui->toolBar->isFloating() || ui->toolBar->isHidden() ) ? 0 : ui->toolBar->height();
	pixmapX = currentPoint.x() + m_scrollArea->horizontalScrollBar()->value();
	pixmapY = currentPoint.y() + m_scrollArea->verticalScrollBar()->value() - ui->menuBar->height()- iToolbarHeight;
}

void MainWindow::GetRowCol(const QPoint& currentPoint, int& row, int& col, double& deltaRow, double& deltaCol) const
{
	GetRowCol(currentPoint, m_board.GetRows(), m_board.GetCols(), row, col, deltaRow, deltaCol);
}

void MainWindow::GetRowCol(const QPoint& currentPoint, const int rows, const int cols, int& row, int& col, double& deltaRow, double& deltaCol) const
{
	const int& W = m_board.GetGRIDPIXELS();	// Square width in pixels

	int pixmapX(0), pixmapY(0);
	GetPixMapXY(currentPoint, pixmapX, pixmapY);

	deltaRow = pixmapY * 1.0 / W;
	deltaCol = pixmapX * 1.0 / W;
	row		 = pixmapY / W;
	col		 = pixmapX / W;
	deltaRow -= row;	// We just want an error in terms of grid squares
	deltaCol -= col;
	row = std::max(0, std::min(rows-1, row));
	col = std::max(0, std::min(cols-1, col));
}

void MainWindow::wheelEvent(QWheelEvent* event)
{
	m_mousePos = event->pos();
	if ( GetShiftKeyDown() ) return;		// Ignore wheel events while trying to group components
	const bool bBack = ( event->delta() < 0 );
	if ( GetCtrlKeyDown() )
	{
		if ( bBack ) ZoomOut(); else ZoomIn();
	}
	else if ( !m_board.GetMirrored() )
	{
		if ( m_board.GetCompEdit() )
			DefinerIncPinNumber(!bBack);
		else
			CompStretch(!bBack);
	}
	event->accept();	// If we don't do this, we can get the same event passed multiple times if we're on MS Windows.
}

void MainWindow::mousePressEvent(QMouseEvent* event)
{
	m_mousePos = event->pos();
	if ( m_board.GetMirrored() ) return;

	const TRACKMODE&	trackMode	= m_board.GetTrackMode();
	const COMPSMODE&	compMode	= m_board.GetCompMode();
	CompDefiner&		compDefiner	= m_board.GetCompDefiner();
	const int&			layer		= m_board.GetCurrentLayer();

	m_bMouseClick	= true;		// Set the flag meaning "click begin"
	m_bLeftClick	= ( event->button() & Qt::LeftButton );
	m_bRightClick	= ( event->button() & Qt::RightButton );

	// Get row col
	double dRow(0), dCol(0);	// Fractional correction to row, col for SwapDiagLinks() call

	if ( m_board.GetCompEdit() )
		GetRowCol(event->pos(), compDefiner.GetScreenRows(), compDefiner.GetScreenCols(), m_gridRow, m_gridCol, dRow, dCol);
	else
		GetRowCol(event->pos(), m_gridRow, m_gridCol, dRow, dCol);

	if ( m_board.GetCompEdit() )
	{
		// Pin/Shape selection
		const int pinId		= compDefiner.GetPinId(m_gridRow, m_gridCol);
		const int shapeId	= compDefiner.GetShapeId(m_gridRow + dRow - 0.5, m_gridCol + dCol - 0.5);
		if ( GetCurrentPinId() != pinId )
		{
			SetCurrentPinId(pinId);
			//if ( pinId == BAD_ID )
			//	mouseActionString = ( pinId == BAD_ID ) ? "Unselect footprint grid point" : "Select footprint grid point";
		}
		if ( GetCurrentShapeId() != shapeId )
		{
			SetCurrentShapeId(shapeId);
			//if ( shapeId == BAD_ID )
			//	mouseActionString = ( shapeId == BAD_ID ) ? "Unselect shape" : "Select shape";
		}
		UpdateCompDialog();
		return RepaintSkipRouting();
	}
	if ( GetDefiningRect() )
	{
		if ( m_bLeftClick)	// Only define rectangles using left-click
		{
			centralWidget()->setCursor(Qt::SizeFDiagCursor);
			m_board.GetRectMgr().StartNewRect(m_gridRow, m_gridCol);
			SelectAllInRects();
			UpdateHistory("Select parts in area(s)");
			ShowCurrentRectSize();
		}
		else				// Right/middle click quits rectangle mode
		{
			centralWidget()->setCursor(Qt::OpenHandCursor);
			SetDefiningRect( false );
			UpdateControls();
		}
		return RepaintSkipRouting();
	}

	if ( !GetCtrlKeyDown() && !GetShiftKeyDown() )
		m_board.GetRectMgr().Clear();

	SetResizingText(false);

	if ( !GetCtrlKeyDown() )	// If not grabbing the board ...
	{
		// Text box selection
		if ( m_board.GetShowText() && m_board.GetTrackMode() != TRACKMODE::PCB )
		{
			const int textId = m_board.GetTextId(m_gridRow, m_gridCol);
			if ( GetCurrentTextId() != textId )	// If we're changing textId
			{
				// If we've clicked away from an empty text box, then delete it
				if ( GetCurrentTextId() != BAD_TEXTID && StringHelper::IsEmptyStr( GetCurrentTextRect().GetStr() ) )
					m_board.GetTextMgr().DestroyRect( GetCurrentTextId() );

				if ( textId != BAD_TEXTID )
					mouseActionString = "Select text box";

				SetCurrentTextId(textId);
			}
			if ( GetCurrentTextId() != BAD_TEXTID )
			{
				// If we're on the bottom-right corner of the text box, then resize it rather than move it
				const TextRect& rect = GetCurrentTextRect();
				if ( rect.m_rowMax == m_gridRow && rect.m_colMax == m_gridCol && dRow >= 0.5 && dCol >= 0.5 )
					SetResizingText(true);
			}
		}
		if ( compMode != COMPSMODE::OFF )
		{
			// Component selection (if text is not selected)
			const int compId = ( GetCurrentTextId() == BAD_TEXTID ) ? m_board.GetComponentId(m_gridRow, m_gridCol) : BAD_COMPID;
			if ( GetCurrentCompId() != compId ) SetCurrentCompId(compId);

			// Group manipulation
			GroupManager& groupMgr = m_board.GetGroupMgr();
			if ( GetShiftKeyDown() )
			{
				groupMgr.UpdateUserGroup( GetCurrentCompId() );	// Add/remove current comp (and its siblings) to user group
				UpdateControls();
				if ( GetCurrentTextId() == BAD_TEXTID )
					mouseActionString = "(Un)select part(s)";
			}
			else if ( !groupMgr.GetIsUserComp( GetCurrentCompId() ) )
			{
				CompManager& compMgr = m_board.GetCompMgr();
				groupMgr.ResetUserGroup( GetCurrentCompId() );	// Reset the user group with the current comp (and its siblings)
				compMgr.ClearTrax();
				UpdateControls();
				if ( GetCurrentTextId() == BAD_TEXTID )
					mouseActionString = "(Un)select part(s)";
			}
		}
	}

	// Cursor modification
	if ( GetCtrlKeyDown() )
		centralWidget()->setCursor(Qt::ClosedHandCursor);
	else if ( GetPaintPins() || GetPaintBoard() || GetPaintFlood() )
		centralWidget()->setCursor(Qt::CrossCursor);
	else if ( GetResizingText() )
		centralWidget()->setCursor(Qt::SizeFDiagCursor);
	else if ( GetCurrentTextId() != BAD_TEXTID || GetCurrentCompId() != BAD_COMPID )
		centralWidget()->setCursor(Qt::ClosedHandCursor);
	else
		centralWidget()->setCursor(Qt::OpenHandCursor);

	// Painting/Unpainting the component pins or board
	if ( GetShiftKeyDown() || GetCtrlKeyDown() || trackMode == TRACKMODE::OFF ) return;

	if ( GetCurrentTextId() != BAD_TEXTID )
		return RepaintWithRouting();	// Don't modify nodeId or paint if editing text

	if ( GetPaintPins() || GetPaintBoard() || GetPaintFlood() )
	{
		if ( m_bLeftClick )		// Paint
		{
			if ( GetCurrentNodeId() == BAD_NODEID )	// If trying to left-click paint an BAD_NODEID ...
				SetCurrentNodeId( m_board.GetNewNodeId() ); // ... use a new NodeId instead

			m_board.GetColorMgr().ReAssignColors();	// Forces colors to be worked out again
			if ( GetPaintFlood() )
			{
				assert( GetCurrentNodeId() != BAD_NODEID );
				assert( !m_board.GetRoutingEnabled() );	// Sanity check

				const int tmp = GetCurrentNodeId();	// Need to temporarily change current nodeId for HandleRouting()
				SetCurrentNodeId( m_board.Get(layer, m_gridRow, m_gridCol)->GetNodeId() );
				HandleRouting();		// Work out MH distances for the flood
				SetCurrentNodeId(tmp);	// Restore current nodeId

				m_board.FloodNodeId( GetCurrentNodeId() );
				mouseActionString = "Paint (flood)";
			}
			else
			{
				const bool bChanged = m_board.SetNodeIdByUser(layer, m_gridRow, m_gridCol, GetCurrentNodeId(), GetPaintPins());
				if ( !bChanged ) return;
				mouseActionString = "Paint";
			}
		}
		if ( m_bRightClick )	// Unpaint (i.e. erase)
		{
			const bool bChanged = m_board.SetNodeIdByUser(layer, m_gridRow, m_gridCol, BAD_NODEID, GetPaintPins());
			if ( !bChanged ) return;
			mouseActionString = "Erase";
		}
	}
	else	// Set/Unset current nodeId from board
	{
		if ( m_bLeftClick )
			SetCurrentNodeId( m_board.Get(layer, m_gridRow, m_gridCol)->GetNodeId() );
		if ( m_bRightClick )
			SetCurrentNodeId( BAD_NODEID );
		if ( m_bLeftClick || m_bRightClick )
			mouseActionString = ( GetCurrentNodeId() == BAD_NODEID) ? "Unselect NodeId" : "Select NodeId";
	}

	m_board.PlaceFloaters();	// See if we can now place floating components down
	RepaintWithRouting();
}

void MainWindow::mouseDoubleClickEvent(QMouseEvent* event)
{
	m_mousePos = event->pos();
	if ( m_board.GetMirrored() ) return;
	if ( m_board.GetCompEdit() ) return;
	if ( m_board.GetTrackMode() == TRACKMODE::OFF ) return;
	if ( GetCtrlKeyDown() || GetShiftKeyDown() ) return;

	if ( GetCurrentTextId() != BAD_TEXTID )
		return ShowTextDialog();

	// Get row col
	double dRow(0), dCol(0);	// Fractional correction to row, col for SwapDiagLinks() call
	GetRowCol(event->pos(), m_gridRow, m_gridCol, dRow, dCol);

	// Cursor modification
	centralWidget()->setCursor(Qt::CrossCursor);

	const int	dR = ( dRow > 0.5 ) ? 1 : 0;	// Correct row, col to account for crossing ...
	const int	dC = ( dCol > 0.5 ) ? 1 : 0;	// ... point being near corner of element
	const int&	layer	 = m_board.GetCurrentLayer();
	const bool	bSwapped = m_board.Get(layer, m_gridRow + dR, m_gridCol + dC)->SwapDiagLinks();
	if ( bSwapped )
	{
		m_board.PlaceFloaters();	// See if we can now place floating components down
		UpdateHistory("Toggle competing diagonals");

		// mouseReleaseEvent() will do RepaintWithRouting() and ListNodes().  No need to do it here.
	}
}

void MainWindow::mouseMoveEvent(QMouseEvent* event)
{
	m_mousePos = event->pos();
	if ( m_board.GetMirrored() ) return;
	if ( !m_bMouseClick ) return;
	const TRACKMODE&	trackMode	= m_board.GetTrackMode();
	const COMPSMODE&	compMode	= m_board.GetCompMode();
	CompDefiner&		compDefiner	= m_board.GetCompDefiner();
	const int&			W			= m_board.GetGRIDPIXELS();
	const int&			layer		= m_board.GetCurrentLayer();

	if ( GetPaintPins() || GetPaintFlood() ) return;// Ignore mouse move while painting pins or flooding
	if ( GetShiftKeyDown() ) return;				// Ignore mouse move while trying to group components

	if ( m_board.GetCompEdit() && GetCurrentShapeId() != BAD_ID && m_bMouseClick )
		centralWidget()->setCursor(Qt::ClosedHandCursor);
	else if ( GetDefiningRect() || GetResizingText() )
		centralWidget()->setCursor(Qt::SizeFDiagCursor);
	else if ( GetCtrlKeyDown() )
		centralWidget()->setCursor(Qt::ClosedHandCursor);
	else if ( GetPaintBoard() )
		centralWidget()->setCursor(Qt::CrossCursor);
	else if ( GetCurrentTextId() != BAD_TEXTID && m_bMouseClick )
		centralWidget()->setCursor(Qt::ClosedHandCursor);
	else if ( GetCurrentCompId() != BAD_COMPID && m_bMouseClick )
		centralWidget()->setCursor(Qt::ClosedHandCursor);
	else
		centralWidget()->setCursor(Qt::OpenHandCursor);

	// Get row col
	int row(0), col(0);
	double dRow(0), dCol(0);	// Fractional correction to row, col for SwapDiagLinks() call
	if ( m_board.GetCompEdit() )
		GetRowCol(event->pos(), compDefiner.GetScreenRows(), compDefiner.GetScreenCols(), row, col, dRow, dCol);
	else
		GetRowCol(event->pos(), row, col, dRow, dCol);

	int deltaRow = row - m_gridRow;
	int deltaCol = col - m_gridCol;

	m_gridRow = row;
	m_gridCol = col;

	if ( m_board.GetCompEdit() )
	{
		if ( GetCurrentShapeId() != BAD_ID )
		{
			int pixmapX(0), pixmapY(0);
			GetPixMapXY(event->pos(), pixmapX, pixmapY);

			if ( deltaRow == 0 ) { if ( pixmapY > W * compDefiner.GetScreenRows() ) deltaRow++; }
			if ( deltaCol == 0 ) { if ( pixmapX > W * compDefiner.GetScreenCols() ) deltaCol++; }
			if ( deltaRow == 0 ) { if ( pixmapY < 0 ) deltaRow--; }
			if ( deltaCol == 0 ) { if ( pixmapX < 0 ) deltaCol--; }
			if ( deltaRow == 0 && deltaCol == 0 ) return;	// No change

			compDefiner.SetCurrentPinId(BAD_ID);				// Clear pin selection
			compDefiner.MoveCurrentShape(deltaRow, deltaCol);	// Move the shape
			UpdateCompDialog();
		}
	}
	else if ( GetDefiningRect() )
	{
		m_board.GetRectMgr().UpdateNewRect(m_gridRow, m_gridCol);
		SelectAllInRects();
	}
	else if ( GetCtrlKeyDown() )
	{
		int pixmapX(0), pixmapY(0);
		GetPixMapXY(event->pos(), pixmapX, pixmapY);

		if ( deltaRow == 0 ) { if ( pixmapY > W * m_board.GetRows() ) deltaRow = 1; }
		if ( deltaCol == 0 ) { if ( pixmapX > W * m_board.GetCols() ) deltaCol = 1; }
		if ( deltaRow == 0 ) { if ( pixmapY < W ) deltaRow = -1; }
		if ( deltaCol == 0 ) { if ( pixmapX < W ) deltaCol = -1; }
		if ( deltaRow == 0 && deltaCol == 0 ) return;	// No change
		m_board.SmartPan(deltaRow, deltaCol);	// Pan whole circuit w.r.t. grid area, growing/shrinking as needed
		mouseActionString = "Smart move/grow/crop";
	}
	else if ( GetPaintBoard() )	// (Un)Paint nodeId on board but NOT pins
	{
		assert( !GetPaintPins() && !GetPaintFlood() );	// Sanity check
		if ( trackMode == TRACKMODE::OFF ) return;
		if ( m_bLeftClick )		// Paint
		{
			const bool bChanged = m_board.SetNodeIdByUser(layer, m_gridRow, m_gridCol, GetCurrentNodeId(), GetPaintPins());	// Only allow paint board (not pins)
			if ( !bChanged ) return;	// No change
			mouseActionString = "Paint";
		}
		if ( m_bRightClick )	// Erase
		{
			const bool bChanged = m_board.SetNodeIdByUser(layer, m_gridRow, m_gridCol, BAD_NODEID, GetPaintPins());		// Only allow paint board (not pins)
			if ( !bChanged ) return;	// No change
			mouseActionString = "Erase";
		}
		m_board.PlaceFloaters();	// See if we can now place floating components down
	}
	else if ( GetCurrentTextId() != BAD_TEXTID )
	{
		int pixmapX(0), pixmapY(0);
		GetPixMapXY(event->pos(), pixmapX, pixmapY);

		if ( deltaRow == 0 ) { if ( pixmapY > W * m_board.GetRows() ) deltaRow++; }
		if ( deltaCol == 0 ) { if ( pixmapX > W * m_board.GetCols() ) deltaCol++; }
		if ( deltaRow == 0 ) { if ( pixmapY < 0 ) deltaRow--; }
		if ( deltaCol == 0 ) { if ( pixmapX < 0 ) deltaCol--; }
		if ( deltaRow == 0 && deltaCol == 0 ) return;	// No change

		if ( g_bHaveAutoPanned )	// If we've auto-panned the grid before ...
		{
			const auto elapsed		= std::chrono::steady_clock::now() - g_lastAutoPanTime;
			const auto duration_ms	= std::chrono::duration_cast<std::chrono::milliseconds>(elapsed).count();
			if ( duration_ms >= 0 && duration_ms < 40 ) return;	// ... do nothing if within 40 ms of the last auto-pan
		}

		if ( GetResizingText() )
		{
			m_board.GetTextMgr().UpdateRect( GetCurrentTextId(), m_gridRow, m_gridCol );
		}
		else
		{
			const bool bAutoPanned = m_board.MoveTextBox(deltaRow, deltaCol);	// Move the text box and note if the grid was auto-panned
			if ( bAutoPanned )
			{
				g_bHaveAutoPanned = true;
				g_lastAutoPanTime = std::chrono::steady_clock::now();
			}
		}
		mouseActionString = ( GetResizingText() ) ? "Resize text box" : "Move text box";
	}
	else if ( GetCurrentCompId() != BAD_COMPID )	// Move user-group components
	{
		if ( compMode == COMPSMODE::OFF ) return;

		int pixmapX(0), pixmapY(0);
		GetPixMapXY(event->pos(), pixmapX, pixmapY);

		if ( deltaRow == 0 ) { if ( pixmapY > W * m_board.GetRows() ) deltaRow++; }
		if ( deltaCol == 0 ) { if ( pixmapX > W * m_board.GetCols() ) deltaCol++; }
		if ( deltaRow == 0 ) { if ( pixmapY < 0 ) deltaRow--; }
		if ( deltaCol == 0 ) { if ( pixmapX < 0 ) deltaCol--; }
		if ( deltaRow == 0 && deltaCol == 0 ) return;	// No change

		if ( g_bHaveAutoPanned )	// If we've auto-panned the grid before ...
		{
			const auto elapsed		= std::chrono::steady_clock::now() - g_lastAutoPanTime;
			const auto duration_ms	= std::chrono::duration_cast<std::chrono::milliseconds>(elapsed).count();
			if ( duration_ms >= 0 && duration_ms < 40 ) return;	// ... do nothing if within 40 ms of the last auto-pan
		}
		const bool bAutoPanned = m_board.MoveUserComps(deltaRow, deltaCol);	// Move the components and note if the grid was auto-panned
		if ( bAutoPanned )
		{
			g_bHaveAutoPanned = true;
			g_lastAutoPanTime = std::chrono::steady_clock::now();
		}
		mouseActionString = "Move part(s)";
	}
	else 
		return;

	if ( abs(deltaRow) <= 1 && abs(deltaCol) <= 1 )	// If not moved mouse too fast ...
	{
		if ( GetDefiningRect() )
			ShowCurrentRectSize();
		if ( GetResizingText() || GetDefiningRect() || m_board.GetCompEdit() )
			RepaintSkipRouting();
		else
			RepaintWithRouting();
	}
}

void MainWindow::mouseReleaseEvent(QMouseEvent* event)
{
	m_mousePos = event->pos();
	releaseMouse();

	if ( m_board.GetMirrored() ) return;

	m_bMouseClick = false;

	if ( m_board.GetCompEdit() )
	{
		UpdateHistory(mouseActionString);
		return RepaintSkipRouting();
	}
	if ( GetResizingText() )
	{
		centralWidget()->setCursor(Qt::OpenHandCursor);
		SetResizingText(false);
		UpdateHistory("Resize text box");
		return RepaintSkipRouting();
	}
	if ( GetDefiningRect() )
	{
		centralWidget()->setCursor(Qt::SizeFDiagCursor);
		m_board.GetRectMgr().EndNewRect();
		SelectAllInRects();
		ShowCurrentRectSize();
		return RepaintSkipRouting();
	}
	if ( GetPaintPins() || GetPaintBoard() || GetPaintFlood() )
		centralWidget()->setCursor(Qt::CrossCursor);
	else
		centralWidget()->setCursor(Qt::OpenHandCursor);

	UpdateHistory(mouseActionString);
	RepaintWithRouting();
	ListNodes();	// Slow due lots of MH calcs
}

void MainWindow::keyPressEvent(QKeyEvent* event)
{
	commonKeyPressEvent(event);

	if ( GetCtrlKeyDown() ) return;		// Try to keep Ctrl key input handled by menu items
	if ( GetShiftKeyDown() ) return;	// Ignore other key presses while trying to group components

	if ( m_board.GetMirrored() ) return;

	const TRACKMODE&	trackMode		= m_board.GetTrackMode();
	const COMPSMODE&	compMode		= m_board.GetCompMode();
	CompDefiner&		compDefiner		= m_board.GetCompDefiner();
	GroupManager&		groupMgr		= m_board.GetGroupMgr();
	const int			nComps			= groupMgr.GetNumUserComps();
	const bool			bIsAutoRepeat	= event->isAutoRepeat();

	if ( m_board.GetCompEdit() )
	{
		if ( GetCurrentShapeId() != BAD_ID )
		{
			switch( event->key() )
			{
				case Qt::Key_Left:	compDefiner.MoveCurrentShape( 0, -1); UpdateCompDialog(); RepaintSkipRouting(); break;
				case Qt::Key_Right:	compDefiner.MoveCurrentShape( 0,  1); UpdateCompDialog(); RepaintSkipRouting(); break;
				case Qt::Key_Up:	compDefiner.MoveCurrentShape(-1,  0); UpdateCompDialog(); RepaintSkipRouting(); break;
				case Qt::Key_Down:	compDefiner.MoveCurrentShape( 1,  0); UpdateCompDialog(); RepaintSkipRouting(); break;
			}
			if ( !bIsAutoRepeat )
			{
				switch( event->key() )
				{
					case Qt::Key_Backspace:
					case Qt::Key_Delete:	Delete();	break;
				}
			}
		}
		event->accept();
		return;
	}

	bool bUpdateControls(false);

	// Rectangle mode
	if ( !bIsAutoRepeat )
	{
		switch( event->key() )
		{
			case Qt::Key_R:	SetDefiningRect(true);	centralWidget()->setCursor(Qt::SizeFDiagCursor); bUpdateControls = true; break;
		}
	}

	// Component manipulation
	switch( event->key() )
	{
		case Qt::Key_Underscore:
		case Qt::Key_Minus:		CompShrink();	break;
		case Qt::Key_Plus:
		case Qt::Key_Equal:		CompGrow();		break;
		case Qt::Key_Left:		if ( !m_board.GetDisableMove() ) { m_board.MoveUserComps(0,-1);	UpdateHistory("Move part(s) left");  RepaintWithRouting(); } break;
		case Qt::Key_Right:		if ( !m_board.GetDisableMove() ) { m_board.MoveUserComps(0, 1);	UpdateHistory("Move part(s) right"); RepaintWithRouting(); } break;
		case Qt::Key_Up:		if ( !m_board.GetDisableMove() ) { m_board.MoveUserComps(-1,0);	UpdateHistory("Move part(s) up");    RepaintWithRouting(); } break;
		case Qt::Key_Down:		if ( !m_board.GetDisableMove() ) { m_board.MoveUserComps( 1,0);	UpdateHistory("Move part(s) down");  RepaintWithRouting(); } break;
	}
	if ( !bIsAutoRepeat )
	{
		switch( event->key() )
		{
			case Qt::Key_Z:			CompRotateCCW();	break;
			case Qt::Key_X:			CompRotateCW();		break;
			case Qt::Key_Delete:	if ( GetCurrentTextId() != BAD_TEXTID || ( nComps && compMode != COMPSMODE::OFF ) )
										Delete();	//	So delete works like the backspace keyboard shortcut
									break;
		}
	}
	// Painting/Unpainting
	if ( !bIsAutoRepeat )
	{
		// Only one flag for paint-board/paint-pins/paint-flood must be true
		switch( event->key() )
		{
			case Qt::Key_P:		if ( trackMode == TRACKMODE::OFF || compMode == COMPSMODE::OFF || GetPaintBoard() || GetPaintFlood() ) return;
								SetPaintPins(true);		centralWidget()->setCursor(Qt::CrossCursor); break;
			case Qt::Key_Space:	if ( trackMode == TRACKMODE::OFF || GetPaintPins() || GetPaintFlood() ) return;
								SetPaintBoard(true);	centralWidget()->setCursor(Qt::CrossCursor); break;
			case Qt::Key_F:		if ( trackMode == TRACKMODE::OFF || compMode == COMPSMODE::OFF || GetPaintBoard() || GetPaintPins() ) return;
								if ( m_board.GetRoutingEnabled() ) return;
								SetPaintFlood(true);	centralWidget()->setCursor(Qt::CrossCursor); break;
			case Qt::Key_W:		WipeTracks();	break;
		}
	}

	if ( !GetDefiningRect() )
		m_board.GetRectMgr().Clear();

	if ( bUpdateControls )
		UpdateControls();

	event->accept();	// If we don't do this, we can get the same event passed multiple times if we're on MS Windows.
}

void MainWindow::keyReleaseEvent(QKeyEvent* event)
{
	commonKeyReleaseEvent(event);

	if ( m_board.GetMirrored() ) return;
	if ( event->isAutoRepeat() ) return;

	if ( m_board.GetCompEdit() )
		UpdateCompDialog();
	else
	{
		bool bUpdateControls(false);
		switch( event->key() )
		{
			case Qt::Key_R:		SetDefiningRect(false);	bUpdateControls = true;	break;
			case Qt::Key_P:		SetPaintPins(false);	break;
			case Qt::Key_F:		SetPaintFlood(false);	break;
			case Qt::Key_Space:	SetPaintBoard(false);	break;
		}
		if ( GetCurrentTextId() != BAD_TEXTID && m_bMouseClick )
			centralWidget()->setCursor(Qt::ClosedHandCursor);
		else if ( GetCurrentCompId() != BAD_COMPID && m_bMouseClick )
			centralWidget()->setCursor(Qt::ClosedHandCursor);
		else if ( GetDefiningRect() )
			centralWidget()->setCursor(Qt::SizeFDiagCursor);
		else
			centralWidget()->setCursor(Qt::OpenHandCursor);

		if ( bUpdateControls )
			UpdateControls();

		ListNodes();	// Slow due lots of MH calcs
	}
	event->accept();	// If we don't do this, we can get the same event passed multiple times if we're on MS Windows.
}

void MainWindow::commonKeyPressEvent(QKeyEvent* event)		// So child dialogs can relay Ctrl and Shift to the main window
{
	switch( event->key() )
	{
		case Qt::Key_Shift:		return SetShiftKeyDown(true);
		case Qt::Key_Control:	if ( m_bMouseClick ) centralWidget()->setCursor(Qt::ClosedHandCursor);
								return SetCtrlKeyDown(true);
	}
}

void MainWindow::commonKeyReleaseEvent(QKeyEvent* event)	// So child dialogs can relay Ctrl and Shift to the main window
{
	switch( event->key() )
	{
		case Qt::Key_Shift:		return SetShiftKeyDown(false);
		case Qt::Key_Control:	return SetCtrlKeyDown(false);
	}
}

void MainWindow::specialKeyPressEvent(QKeyEvent* event)		// So child dialogs can do Ctrl+Q etc
{
	commonKeyPressEvent(event);
	switch( event->key() )
	{
		case Qt::Key_N:	if ( GetCtrlKeyDown() ) return New();
		case Qt::Key_O:	if ( GetCtrlKeyDown() ) return Open();
		case Qt::Key_M:	if ( GetCtrlKeyDown() ) return Merge();
		case Qt::Key_S:	if ( GetCtrlKeyDown() ) return GetShiftKeyDown() ? SaveAs() : Save();
		case Qt::Key_Q:	if ( GetCtrlKeyDown() ) return Quit();
	}
}

void MainWindow::dragEnterEvent(QDragEnterEvent *e)
{
	if ( e->mimeData()->hasUrls() )
		e->acceptProposedAction();
}

void MainWindow::dropEvent(QDropEvent *e)
{
	if ( e->mimeData()->hasUrls() )
	{
		QString fileName = e->mimeData()->urls().first().toLocalFile();
		if ( fileName.isEmpty() ) return;

		SetCtrlKeyDown(false);	// Clear flag since key release can get missed.
		if ( GetIsModified() )
			if ( QMessageBox::question(this, tr("Confirm Open"),
											 tr("Your circuit is not saved. You will lose it if you open a new one.  Continue?"),
											 QMessageBox::Yes | QMessageBox::No, QMessageBox::No) == QMessageBox::No ) return;
		OpenVrt(fileName);
	}
}
