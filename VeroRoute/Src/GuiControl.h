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

#include "CompElement.h"	// For BAD_COMPID, BAD_NODEID, TRAX_COMPID

enum class DIAGSMODE { OFF = 0, MIN, MAX };
enum class TRACKMODE { OFF = 0, MONO, COLOR, PCB };
enum class COMPSMODE { OFF = 0, OUTLINE, NAME, VALUE };
enum class HOLETYPE  { NPTH = 0, PTH };

// A class to hold the variables set via the GUI

const int BAD_TEXTID = -1;

class GuiControl : public Persist, public Merge
{
public:
	GuiControl() {}
	~GuiControl() {}
	GuiControl(const GuiControl& o) { *this = o; }
	GuiControl& operator=(const GuiControl& o)
	{
		m_currentLayer		= o.m_currentLayer;
		m_currentCompId		= o.m_currentCompId;
		m_currentNodeId		= o.m_currentNodeId;
		m_groundNodeId0		= o.m_groundNodeId0;
		m_groundNodeId1		= o.m_groundNodeId1;
		m_currentTextId		= o.m_currentTextId;
		m_diagsMode			= o.m_diagsMode;
		m_iTrackMode		= o.m_iTrackMode;
		m_iCompMode			= o.m_iCompMode;
		m_iHoleType			= o.m_iHoleType;
		m_GRIDPIXELS		= o.m_GRIDPIXELS;
		m_PAD_PERCENT		= o.m_PAD_PERCENT;
		m_TRACK_PERCENT		= o.m_TRACK_PERCENT;
		m_HOLE_PERCENT		= o.m_HOLE_PERCENT;
		m_GAP_PERCENT		= o.m_GAP_PERCENT;
		m_MASK_PERCENT		= o.m_MASK_PERCENT;
		m_SILK_PERCENT		= o.m_SILK_PERCENT;
		m_EDGE_PERCENT		= o.m_EDGE_PERCENT;
		m_VIAPAD_PERCENT	= o.m_VIAPAD_PERCENT;
		m_VIAHOLE_PERCENT	= o.m_VIAHOLE_PERCENT;
		m_iRenderQuality	= o.m_iRenderQuality;
		m_iSaturation		= o.m_iSaturation;
		m_iFillSaturation	= o.m_iFillSaturation;
		m_iCropMargin		= o.m_iCropMargin;
		m_iTargetRows		= o.m_iTargetRows;
		m_iTargetCols		= o.m_iTargetCols;
		m_iTextSizeComp		= o.m_iTextSizeComp;
		m_iTextSizePins		= o.m_iTextSizePins;
		m_iRoutingMethod	= o.m_iRoutingMethod;
		m_bShowTarget		= o.m_bShowTarget;
		m_bWireShare		= o.m_bWireShare;
		m_bWireCross		= o.m_bWireCross;
		m_bVeroTracks		= o.m_bVeroTracks;
		m_bCurvedTracks		= o.m_bCurvedTracks;
		m_bFatTracks		= o.m_bFatTracks;
		m_bRoutingEnabled	= o.m_bRoutingEnabled;
		m_bViasEnabled		= o.m_bViasEnabled;
		m_bShowGrid			= o.m_bShowGrid;
		m_bShowText			= o.m_bShowText;
		m_bFlipH			= o.m_bFlipH;
		m_bFlipV			= o.m_bFlipV;
		m_bPinLabels		= o.m_bPinLabels;
		m_bGroundFill		= o.m_bGroundFill;
		m_bVerticalStrips	= o.m_bVerticalStrips;
		m_bCompEdit			= o.m_bCompEdit;
		return *this;
	}
	bool operator==(const GuiControl& o) const	// Compare persisted info
	{
		return	m_currentLayer		== o.m_currentLayer
			&&	m_currentCompId		== o.m_currentCompId
			&&	m_currentNodeId		== o.m_currentNodeId
			&&	m_groundNodeId0		== o.m_groundNodeId0
			&&	m_groundNodeId1		== o.m_groundNodeId1
			&&	m_currentTextId		== o.m_currentTextId
			&&	m_diagsMode			== o.m_diagsMode
			&&	m_iTrackMode		== o.m_iTrackMode
			&&	m_iCompMode			== o.m_iCompMode
			&&	m_iHoleType			== o.m_iHoleType
			&&	m_GRIDPIXELS		== o.m_GRIDPIXELS
			&&	m_PAD_PERCENT		== o.m_PAD_PERCENT
			&&	m_TRACK_PERCENT		== o.m_TRACK_PERCENT
			&&	m_HOLE_PERCENT		== o.m_HOLE_PERCENT
			&&	m_GAP_PERCENT		== o.m_GAP_PERCENT
			&&	m_MASK_PERCENT		== o.m_MASK_PERCENT
			&&	m_SILK_PERCENT		== o.m_SILK_PERCENT
			&&	m_EDGE_PERCENT		== o.m_EDGE_PERCENT
			&&	m_VIAPAD_PERCENT	== o.m_VIAPAD_PERCENT
			&&	m_VIAHOLE_PERCENT	== o.m_VIAHOLE_PERCENT
			&&	m_iRenderQuality	== o.m_iRenderQuality
			&&	m_iSaturation		== o.m_iSaturation
			&&	m_iFillSaturation	== o.m_iFillSaturation
			&&	m_iCropMargin		== o.m_iCropMargin
			&&	m_iTargetRows		== o.m_iTargetRows
			&&	m_iTargetCols		== o.m_iTargetCols
			&&	m_iTextSizeComp		== o.m_iTextSizeComp
			&&	m_iTextSizePins		== o.m_iTextSizePins
			&&	m_iRoutingMethod	== o.m_iRoutingMethod
			&&	m_bShowTarget		== o.m_bShowTarget
			&&	m_bWireShare		== o.m_bWireShare
			&&	m_bWireCross		== o.m_bWireCross
			&&	m_bVeroTracks		== o.m_bVeroTracks
			&&	m_bCurvedTracks		== o.m_bCurvedTracks
			&&	m_bFatTracks		== o.m_bFatTracks
			&&	m_bRoutingEnabled	== o.m_bRoutingEnabled
			&&	m_bViasEnabled		== o.m_bViasEnabled
			&&	m_bShowGrid			== o.m_bShowGrid
			&&	m_bShowText			== o.m_bShowText
			&&	m_bFlipH			== o.m_bFlipH
			&&	m_bFlipV			== o.m_bFlipV
			&&	m_bPinLabels		== o.m_bPinLabels
			&&	m_bGroundFill		== o.m_bGroundFill
			&&	m_bVerticalStrips	== o.m_bVerticalStrips
			&&	m_bCompEdit			== o.m_bCompEdit;
	}
	bool operator!=(const GuiControl& o) const
	{
		return !(*this == o);
	}
	void Clear()
	{
		SetCurrentCompId(BAD_COMPID);
		SetCurrentNodeId(BAD_NODEID);
		SetGroundNodeId0(BAD_NODEID);
		SetGroundNodeId1(BAD_NODEID);
		SetCurrentTextId(BAD_TEXTID);
		SetTrackMode(TRACKMODE::COLOR);
		SetCompMode(COMPSMODE::NAME);
		SetRoutingEnabled(false);
		SetFlipH(false);
		SetFlipV(false);
		SetGroundFill(false);
		SetCompEdit(false);
	}
	// Merge interface functions
	virtual void UpdateMergeOffsets(MergeOffsets& o) override
	{
		if ( m_currentCompId != BAD_COMPID &&
			 m_currentCompId != TRAX_COMPID )	o.deltaCompId = std::max(o.deltaCompId, m_currentCompId + 1);
		if ( m_currentNodeId != BAD_NODEID )	o.deltaNodeId = std::max(o.deltaNodeId, m_currentNodeId + 1);
		if ( m_groundNodeId0 != BAD_NODEID )	o.deltaNodeId = std::max(o.deltaNodeId, m_groundNodeId0 + 1);
		if ( m_groundNodeId1 != BAD_NODEID )	o.deltaNodeId = std::max(o.deltaNodeId, m_groundNodeId1 + 1);
	}
	virtual void ApplyMergeOffsets(const MergeOffsets& o) override
	{
		if ( m_currentCompId != BAD_COMPID &&
			 m_currentCompId != TRAX_COMPID)	m_currentCompId += o.deltaCompId;
		if ( m_currentNodeId != BAD_NODEID )	m_currentNodeId += o.deltaNodeId;
		if ( m_groundNodeId0 != BAD_NODEID )	m_groundNodeId0 += o.deltaNodeId;
		if ( m_groundNodeId1 != BAD_NODEID )	m_groundNodeId1 += o.deltaNodeId;
	}
	void Merge(const GuiControl& o)
	{
		m_currentCompId	= o.m_currentCompId;
		m_currentNodeId	= o.m_currentNodeId;
		m_groundNodeId0	= o.m_groundNodeId0;
		m_groundNodeId1	= o.m_groundNodeId1;
	}
	// Persist functions
	virtual void Load(DataStream& inStream) override
	{
		m_currentLayer = 0;
		if ( inStream.GetVersion() >= VRT_VERSION_34 )
			inStream.Load(m_currentLayer);		// Added in VRT_VERSION_34
		inStream.Load(m_currentCompId);
		inStream.Load(m_currentNodeId);
		m_groundNodeId0 = BAD_NODEID;
		if ( inStream.GetVersion() >= VRT_VERSION_3 )
			inStream.Load(m_groundNodeId0);		// Added in VRT_VERSION_3
		m_groundNodeId1 = BAD_NODEID;
		if ( inStream.GetVersion() >= VRT_VERSION_34 )
			inStream.Load(m_groundNodeId1);		// Added in VRT_VERSION_34
		m_currentTextId = BAD_TEXTID;
		if ( inStream.GetVersion() >= VRT_VERSION_14 )
			inStream.Load(m_currentTextId);		// Added in VRT_VERSION_14
		int diagMode(0), trackMode(0), compMode(0), holeType(0);
		inStream.Load(diagMode);
		inStream.Load(trackMode);
		inStream.Load(compMode);
		if ( inStream.GetVersion() >= VRT_VERSION_33 )
			inStream.Load(holeType);			// Added in VRT_VERSION_33
		m_diagsMode		= static_cast<DIAGSMODE>	(diagMode);
		m_iTrackMode	= static_cast<TRACKMODE>	(trackMode);
		m_iCompMode		= static_cast<COMPSMODE>	(compMode);
		m_iHoleType		= static_cast<HOLETYPE>		(holeType);
		inStream.Load(m_GRIDPIXELS);
		inStream.Load(m_PAD_PERCENT);
		inStream.Load(m_TRACK_PERCENT);
		inStream.Load(m_HOLE_PERCENT);
		m_GAP_PERCENT = 10;
		if ( inStream.GetVersion() >= VRT_VERSION_3 )
			inStream.Load(m_GAP_PERCENT);		// Added in VRT_VERSION_3
		m_MASK_PERCENT = 4;
		m_SILK_PERCENT = 7;
		if ( inStream.GetVersion() >= VRT_VERSION_32 )
		{
			inStream.Load(m_MASK_PERCENT);		// Added in VRT_VERSION_32
			inStream.Load(m_SILK_PERCENT);		// Added in VRT_VERSION_32
		}
		m_EDGE_PERCENT = 20;
		if ( inStream.GetVersion() >= VRT_VERSION_33 )
			inStream.Load(m_EDGE_PERCENT);		// Added in VRT_VERSION_33
		m_VIAPAD_PERCENT	= 50;
		m_VIAHOLE_PERCENT	= 25;
		if ( inStream.GetVersion() >= VRT_VERSION_35 )
		{
			inStream.Load(m_VIAPAD_PERCENT);	// Added in VRT_VERSION_35
			inStream.Load(m_VIAHOLE_PERCENT);	// Added in VRT_VERSION_35
		}
		inStream.Load(m_iRenderQuality);
		m_iSaturation = 100;
		if ( inStream.GetVersion() >= VRT_VERSION_6 )
			inStream.Load(m_iSaturation);		// Added in VRT_VERSION_6
		m_iFillSaturation = 0;
		if ( inStream.GetVersion() >= VRT_VERSION_29 )
			inStream.Load(m_iFillSaturation);	// Added in VRT_VERSION_29
		m_iCropMargin = 0;
		if ( inStream.GetVersion() >= VRT_VERSION_13 )
			inStream.Load(m_iCropMargin);		// Added in VRT_VERSION_13
		m_iTargetRows = 10;
		if ( inStream.GetVersion() >= VRT_VERSION_21 )
			inStream.Load(m_iTargetRows);		// Added in VRT_VERSION_21
		m_iTargetCols = 10;
		if ( inStream.GetVersion() >= VRT_VERSION_21 )
			inStream.Load(m_iTargetCols);		// Added in VRT_VERSION_21
		m_iTextSizeComp = 9;
		if ( inStream.GetVersion() >= VRT_VERSION_17 )
			inStream.Load(m_iTextSizeComp);		// Added in VRT_VERSION_17
		m_iTextSizePins = 9;
		if ( inStream.GetVersion() >= VRT_VERSION_17 )
			inStream.Load(m_iTextSizePins);		// Added in VRT_VERSION_17
		m_iRoutingMethod = 0;
		if ( inStream.GetVersion() >= VRT_VERSION_24 )
			inStream.Load(m_iRoutingMethod);	// Added in VRT_VERSION_24
		m_bShowTarget = false;
		if ( inStream.GetVersion() >= VRT_VERSION_21 )
			inStream.Load(m_bShowTarget);		// Added in VRT_VERSION_21
		m_bWireShare = true;
		if ( inStream.GetVersion() >= VRT_VERSION_28 )
			inStream.Load(m_bWireShare);		// Added in VRT_VERSION_28
		m_bWireCross = false;
		if ( inStream.GetVersion() >= VRT_VERSION_28 )
			inStream.Load(m_bWireCross);		// Added in VRT_VERSION_28
		inStream.Load(m_bVeroTracks);
		inStream.Load(m_bCurvedTracks);
		m_bFatTracks = true;
		if ( inStream.GetVersion() >= VRT_VERSION_36 )
			inStream.Load(m_bFatTracks);		// Added in VRT_VERSION_36
		inStream.Load(m_bRoutingEnabled);
		m_bViasEnabled = true;
		if ( inStream.GetVersion() >= VRT_VERSION_37 )
			inStream.Load(m_bViasEnabled);		// Added in VRT_VERSION_37
		m_bShowGrid = m_iTrackMode != TRACKMODE::OFF;
		if ( inStream.GetVersion() >= VRT_VERSION_5 )
			inStream.Load(m_bShowGrid);			// Added in VRT_VERSION_5
		m_bShowText = true;
		if ( inStream.GetVersion() >= VRT_VERSION_14 )
			inStream.Load(m_bShowText);			// Added in VRT_VERSION_14
		inStream.Load(m_bFlipH);
		m_bFlipV = false;
		if ( inStream.GetVersion() >= VRT_VERSION_14 )
			inStream.Load(m_bFlipV);			// Added in VRT_VERSION_14
		m_bPinLabels = false;
		if ( inStream.GetVersion() >= VRT_VERSION_2 )
			inStream.Load(m_bPinLabels);		// Added in VRT_VERSION_2
		m_bGroundFill = false;
		if ( inStream.GetVersion() >= VRT_VERSION_3 )
			inStream.Load(m_bGroundFill);		// Added in VRT_VERSION_3
		m_bVerticalStrips = true;
		if ( inStream.GetVersion() >= VRT_VERSION_12 )
			inStream.Load(m_bVerticalStrips);	// Added in VRT_VERSION_12
		m_bCompEdit = false;
		if ( inStream.GetVersion() >= VRT_VERSION_19 )
			inStream.Load(m_bCompEdit);			// Added in VRT_VERSION_19
	}
	virtual void Save(DataStream& outStream) override
	{
		outStream.Save(m_currentLayer);		// Added in VRT_VERSION_34
		outStream.Save(m_currentCompId);
		outStream.Save(m_currentNodeId);
		outStream.Save(m_groundNodeId0);	// Added in VRT_VERSION_3
		outStream.Save(m_groundNodeId1);	// Added in VRT_VERSION_34
		outStream.Save(m_currentTextId);	// Added in VRT_VERSION_14
		outStream.Save((int) m_diagsMode);
		outStream.Save((int) m_iTrackMode);
		outStream.Save((int) m_iCompMode);
		outStream.Save((int) m_iHoleType);	// Added in VRT_VERSION_33
		outStream.Save(m_GRIDPIXELS);
		outStream.Save(m_PAD_PERCENT);
		outStream.Save(m_TRACK_PERCENT);
		outStream.Save(m_HOLE_PERCENT);
		outStream.Save(m_GAP_PERCENT);		// Added in VRT_VERSION_3
		outStream.Save(m_MASK_PERCENT);		// Added in VRT_VERSION_32
		outStream.Save(m_SILK_PERCENT);		// Added in VRT_VERSION_32
		outStream.Save(m_EDGE_PERCENT);		// Added in VRT_VERSION_33
		outStream.Save(m_VIAPAD_PERCENT);	// Added in VRT_VERSION_35
		outStream.Save(m_VIAHOLE_PERCENT);	// Added in VRT_VERSION_35
		outStream.Save(m_iRenderQuality);
		outStream.Save(m_iSaturation);		// Added in VRT_VERSION_6
		outStream.Save(m_iFillSaturation);	// Added in VRT_VERSION_29
		outStream.Save(m_iCropMargin);		// Added in VRT_VERSION_13
		outStream.Save(m_iTargetRows);		// Added in VRT_VERSION_21
		outStream.Save(m_iTargetCols);		// Added in VRT_VERSION_21
		outStream.Save(m_iTextSizeComp);	// Added in VRT_VERSION_17
		outStream.Save(m_iTextSizePins);	// Added in VRT_VERSION_17
		outStream.Save(m_iRoutingMethod);	// Added in VRT_VERSION_24
		outStream.Save(m_bShowTarget);		// Added in VRT_VERSION_21
		outStream.Save(m_bWireShare);		// Added in VRT_VERSION_28
		outStream.Save(m_bWireCross);		// Added in VRT_VERSION_28
		outStream.Save(m_bVeroTracks);
		outStream.Save(m_bCurvedTracks);
		outStream.Save(m_bFatTracks);		// Added in VRT_VERSION_36
		outStream.Save(m_bRoutingEnabled);
		outStream.Save(m_bViasEnabled);		// Added in VRT_VERSION_37
		outStream.Save(m_bShowGrid);		// Added in VRT_VERSION_5
		outStream.Save(m_bShowText);		// Added in VRT_VERSION_14
		outStream.Save(m_bFlipH);
		outStream.Save(m_bFlipV);			// Added in VRT_VERSION_14
		outStream.Save(m_bPinLabels);		// Added in VRT_VERSION_2
		outStream.Save(m_bGroundFill);		// Added in VRT_VERSION_3
		outStream.Save(m_bVerticalStrips);	// Added in VRT_VERSION_12
		outStream.Save(m_bCompEdit);		// Added in VRT_VERSION_19
	}
	bool SetCurrentLayer(const int& i)		{ const bool bChanged = m_currentLayer		!= i; m_currentLayer	= i; return bChanged; }
	bool SetCurrentNodeId(const int& i)		{ const bool bChanged = m_currentNodeId		!= i; m_currentNodeId	= i; return bChanged; }
	bool SetCurrentCompId(const int& i)		{ const bool bChanged = m_currentCompId		!= i; m_currentCompId	= i; return bChanged; }
	bool SetGroundNodeId0(const int& i)		{ const bool bChanged = m_groundNodeId0		!= i; m_groundNodeId0	= i; return bChanged; }
	bool SetGroundNodeId1(const int& i)		{ const bool bChanged = m_groundNodeId1		!= i; m_groundNodeId1	= i; return bChanged; }
	bool SetCurrentTextId(const int& i)		{ const bool bChanged = m_currentTextId		!= i; m_currentTextId	= i; return bChanged; }
	bool SetDiagsMode(const DIAGSMODE& e)	{ const bool bChanged = m_diagsMode			!= e; m_diagsMode		= e; return bChanged; }
	bool SetTrackMode(const TRACKMODE& e)	{ const bool bChanged = m_iTrackMode		!= e; m_iTrackMode		= e; return bChanged; }
	bool SetCompMode(const COMPSMODE& e)	{ const bool bChanged = m_iCompMode			!= e; m_iCompMode		= e; return bChanged; }
	bool SetHoleType(const HOLETYPE& e)		{ const bool bChanged = m_iHoleType			!= e; m_iHoleType		= e; return bChanged; }
	bool SetGRIDPIXELS(const int& i)		{ const bool bChanged = m_GRIDPIXELS		!= i; m_GRIDPIXELS		= i; return bChanged; }
	bool SetPAD_PERCENT(const int& i)		{ const bool bChanged = m_PAD_PERCENT		!= i; m_PAD_PERCENT		= i; return bChanged; }
	bool SetTRACK_PERCENT(const int& i)		{ const bool bChanged = m_TRACK_PERCENT		!= i; m_TRACK_PERCENT	= i; return bChanged; }
	bool SetHOLE_PERCENT(const int& i)		{ const bool bChanged = m_HOLE_PERCENT		!= i; m_HOLE_PERCENT	= i; return bChanged; }
	bool SetGAP_PERCENT(const int& i)		{ const bool bChanged = m_GAP_PERCENT		!= i; m_GAP_PERCENT		= i; return bChanged; }
	bool SetMASK_PERCENT(const int& i)		{ const bool bChanged = m_MASK_PERCENT		!= i; m_MASK_PERCENT	= i; return bChanged; }
	bool SetSILK_PERCENT(const int& i)		{ const bool bChanged = m_SILK_PERCENT		!= i; m_SILK_PERCENT	= i; return bChanged; }
	bool SetEDGE_PERCENT(const int& i)		{ const bool bChanged = m_EDGE_PERCENT		!= i; m_EDGE_PERCENT	= i; return bChanged; }
	bool SetVIAPAD_PERCENT(const int& i)	{ const bool bChanged = m_VIAPAD_PERCENT	!= i; m_VIAPAD_PERCENT	= i; return bChanged; }
	bool SetVIAHOLE_PERCENT(const int& i)	{ const bool bChanged = m_VIAHOLE_PERCENT	!= i; m_VIAHOLE_PERCENT	= i; return bChanged; }
	bool SetRenderQuality(const int& i)		{ const bool bChanged = m_iRenderQuality	!= i; m_iRenderQuality	= i; return bChanged; }
	bool SetSaturation(const int& i)		{ const bool bChanged = m_iSaturation		!= i; m_iSaturation		= i; return bChanged; }
	bool SetFillSaturation(const int& i)	{ const bool bChanged = m_iFillSaturation	!= i; m_iFillSaturation	= i; return bChanged; }
	bool SetCropMargin(const int& i)		{ const bool bChanged = m_iCropMargin		!= i; m_iCropMargin		= i; return bChanged; }
	bool SetTargetRows(const int& i)		{ const bool bChanged = m_iTargetRows		!= i; m_iTargetRows		= i; return bChanged; }
	bool SetTargetCols(const int& i)		{ const bool bChanged = m_iTargetCols		!= i; m_iTargetCols		= i; return bChanged; }
	bool SetTextSizeComp(const int& i)		{ const bool bChanged = m_iTextSizeComp		!= i; m_iTextSizeComp	= i; return bChanged; }
	bool SetTextSizePins(const int& i)		{ const bool bChanged = m_iTextSizePins		!= i; m_iTextSizePins	= i; return bChanged; }
	bool SetRoutingMethod(const int& i)		{ const bool bChanged = m_iRoutingMethod	!= i; m_iRoutingMethod	= i; return bChanged; }
	bool SetShowTarget(const bool& b)		{ const bool bChanged = m_bShowTarget		!= b; m_bShowTarget		= b; return bChanged; }
	bool SetWireShare(const bool& b)		{ const bool bChanged = m_bWireShare		!= b; m_bWireShare		= b; return bChanged; }
	bool SetWireCross(const bool& b)		{ const bool bChanged = m_bWireCross		!= b; m_bWireCross		= b; return bChanged; }
	bool SetVeroTracks(const bool& b)		{ const bool bChanged = m_bVeroTracks		!= b; m_bVeroTracks		= b; return bChanged; }
	bool SetCurvedTracks(const bool& b)		{ const bool bChanged = m_bCurvedTracks		!= b; m_bCurvedTracks	= b; return bChanged; }
	bool SetFatTracks(const bool& b)		{ const bool bChanged = m_bFatTracks		!= b; m_bFatTracks		= b; return bChanged; }
	bool SetRoutingEnabled(const bool& b)	{ const bool bChanged = m_bRoutingEnabled	!= b; m_bRoutingEnabled	= b; return bChanged; }
	bool SetViasEnabled(const bool& b)		{ const bool bChanged = m_bViasEnabled		!= b; m_bViasEnabled	= b; return bChanged; }
	bool SetShowGrid(const bool& b)			{ const bool bChanged = m_bShowGrid			!= b; m_bShowGrid		= b; return bChanged; }
	bool SetShowText(const bool& b)			{ const bool bChanged = m_bShowText			!= b; m_bShowText		= b; return bChanged; }
	bool SetFlipH(const bool& b)			{ const bool bChanged = m_bFlipH			!= b; m_bFlipH			= b; return bChanged; }
	bool SetFlipV(const bool& b)			{ const bool bChanged = m_bFlipV			!= b; m_bFlipV			= b; return bChanged; }
	bool SetShowPinLabels(const bool& b)	{ const bool bChanged = m_bPinLabels		!= b; m_bPinLabels		= b; return bChanged; }
	bool SetGroundFill(const bool& b)		{ const bool bChanged = m_bGroundFill		!= b; m_bGroundFill		= b; return bChanged; }
	bool SetVerticalStrips(const bool& b)	{ const bool bChanged = m_bVerticalStrips	!= b; m_bVerticalStrips	= b; return bChanged; }
	bool SetCompEdit(const bool& b)			{ const bool bChanged = m_bCompEdit			!= b; m_bCompEdit		= b; return bChanged; }
	const int&			GetCurrentLayer() const		{ return m_currentLayer; }
	const int&			GetCurrentNodeId() const	{ return m_currentNodeId; }
	const int&			GetCurrentCompId() const	{ return m_currentCompId; }
	const int&			GetGroundNodeId0() const	{ return m_groundNodeId0; }
	const int&			GetGroundNodeId1() const	{ return m_groundNodeId1; }
	const int&			GetCurrentTextId() const	{ return m_currentTextId; }
	const DIAGSMODE&	GetDiagsMode() const		{ return m_diagsMode; }
	const TRACKMODE&	GetTrackMode() const		{ return m_iTrackMode; }
	const COMPSMODE&	GetCompMode() const			{ return m_iCompMode; }
	const HOLETYPE&		GetHoleType() const			{ return m_iHoleType; }
	const int&			GetGRIDPIXELS() const		{ return m_GRIDPIXELS; }
	const int&			GetPAD_PERCENT() const		{ return m_PAD_PERCENT; }
	const int&			GetTRACK_PERCENT() const	{ return m_TRACK_PERCENT; }
	const int&			GetHOLE_PERCENT() const		{ return m_HOLE_PERCENT; }
	const int&			GetGAP_PERCENT() const		{ return m_GAP_PERCENT; }
	const int&			GetMASK_PERCENT() const		{ return m_MASK_PERCENT; }
	const int&			GetSILK_PERCENT() const		{ return m_SILK_PERCENT; }
	const int&			GetEDGE_PERCENT() const		{ return m_EDGE_PERCENT; }
	const int&			GetVIAPAD_PERCENT() const	{ return m_VIAPAD_PERCENT; }
	const int&			GetVIAHOLE_PERCENT() const	{ return m_VIAHOLE_PERCENT; }
	const int&			GetRenderQuality() const	{ return m_iRenderQuality; }
	const int&			GetSaturation() const		{ return m_iSaturation; }
	const int&			GetFillSaturation() const	{ return m_iFillSaturation; }
	const int&			GetCropMargin() const		{ return m_iCropMargin; }
	const int&			GetTargetRows() const		{ return m_iTargetRows; }
	const int&			GetTargetCols() const		{ return m_iTargetCols; }
	const int&			GetTextSizeComp() const		{ return m_iTextSizeComp; }
	const int&			GetTextSizePins() const		{ return m_iTextSizePins; }
	const int&			GetRoutingMethod() const	{ return m_iRoutingMethod; }
	const bool&			GetShowTarget() const		{ return m_bShowTarget; }
	const bool&			GetWireShare() const		{ return m_bWireShare; }
	const bool&			GetWireCross() const		{ return m_bWireCross; }
	const bool&			GetVeroTracks() const		{ return m_bVeroTracks; }
	const bool&			GetCurvedTracks() const		{ return m_bCurvedTracks; }
	const bool&			GetFatTracks() const		{ return m_bFatTracks; }
	const bool&			GetRoutingEnabled() const	{ return m_bRoutingEnabled; }
	const bool&			GetViasEnabled() const		{ return m_bViasEnabled; }
	const bool&			GetShowGrid() const			{ return m_bShowGrid; }
	const bool&			GetShowText() const			{ return m_bShowText; }
	const bool&			GetFlipH() const			{ return m_bFlipH; }
	const bool&			GetFlipV() const			{ return m_bFlipV; }
	const bool&			GetShowPinLabels() const	{ return m_bPinLabels; }
	const bool&			GetGroundFill() const		{ return m_bGroundFill; }
	const bool&			GetVerticalStrips() const	{ return m_bVerticalStrips; }
	const bool&			GetCompEdit() const			{ return m_bCompEdit; }
	// Helpers
	bool	GetMirrored() const			{ return GetFlipH() || GetFlipV(); }
	bool	SetTrackSliderValue(int i)	{ const bool bChanged = ( GetTrackSliderValue() != i ); SetTrackMode( static_cast<TRACKMODE>(i) ); return bChanged; }
	bool	SetCompSliderValue(int i)	{ const bool bChanged = ( GetCompSliderValue()  != i ); SetCompMode(  static_cast<COMPSMODE>(i) ); return bChanged; }
	int		GetTrackSliderValue() const	{ return static_cast<int>(GetTrackMode()); }
	int		GetCompSliderValue() const	{ return static_cast<int>(GetCompMode());  }
	int		GetHalfPadWidth() const		{ return std::max(1, static_cast<int> (GetGRIDPIXELS() * GetPAD_PERCENT()		* 0.005 )); }	// Half pad width in pixels
	int		GetHalfTrackWidth() const	{ return std::max(1, static_cast<int> (GetGRIDPIXELS() * GetTRACK_PERCENT()		* 0.005 )); }	// Half track width in pixels
	int		GetHalfViaPadWidth() const	{ return std::max(1, static_cast<int> (GetGRIDPIXELS() * GetVIAPAD_PERCENT()	* 0.005 )); }	// Half via-pad width in pixels
	int		GetGapWidth() const			{ return std::max(1, static_cast<int> (GetGRIDPIXELS() * GetGAP_PERCENT()		* 0.010 )); }	// Gap width in pixels
	double	GetSilkWidth() const		{ return std::max(1.0,				   GetGRIDPIXELS() * GetSILK_PERCENT()		* 0.010 );  }	// Silk-screen pen width in pixels
	double	GetEdgeWidth() const		{ return std::max(1.0,				   GetGRIDPIXELS() * GetEDGE_PERCENT()		* 0.010 );  }	// Board edge margin in pixels
	int		GetMIN_TRACK_SEPARATION_PERCENT() const	// Minimum guaranteed track separation in mil
	{
		// To keep track/pads at least N mil apart:
		// In diags mode keep     (Pad + Track) / 2 <= ( 70.71 - N).  Keep Gap >= N if used.
		// In non-diags mode keep (Pad + Pad  ) / 2 <= ( 100.0 - N).  Keep Gap >= N if used.
		const double dGap		= ( GetGroundFill() ) ? GetGAP_PERCENT() : 100;
		const double dPad		= std::max(GetPAD_PERCENT(), GetVIAPAD_PERCENT());
		const double dPadPad	= 100 - dPad;
		const double dPadTrack	= ( GetDiagsMode() == DIAGSMODE::OFF ) ? 100 : ( 50 * sqrt(2.0) - 0.5 * ( dPad + GetTRACK_PERCENT() ) );
		return (int) floor(std::max(0.0, std::min(dGap, std::min(dPadPad, dPadTrack))));
	}
	//const int H = std::min(GetHalfPadWidth(), (int) ( GetGRIDPIXELS() * (sqrt(2.0)-1) * 0.5));	// Biggest OK half track width in pixels
private:
	int			m_currentLayer		= 0;				// Currently selected layer for display
	int			m_currentCompId		= BAD_COMPID;		// Currently selected component ID
	int			m_currentNodeId		= BAD_NODEID;		// Currently selected node ID
	int			m_groundNodeId0		= BAD_NODEID;		// The node ID used for ground-fill on layer 0
	int			m_groundNodeId1		= BAD_NODEID;		// The node ID used for ground-fill on layer 1
	int			m_currentTextId		= BAD_TEXTID;		// Currently selected text box
	DIAGSMODE	m_diagsMode			= DIAGSMODE::MIN;	// OFF, MIN, MAX
	TRACKMODE	m_iTrackMode		= TRACKMODE::COLOR;	// OFF, MONO, COLOR, PCB
	COMPSMODE	m_iCompMode			= COMPSMODE::NAME;	// OFF, OUTLINE, NAME, VALUE
	HOLETYPE	m_iHoleType			= HOLETYPE::NPTH;	// NPTH, PTH (Non-Plated Through Hole, Plated Through Hole)
	int			m_GRIDPIXELS		= 24;				// Default 24 pixels per grid square (i.e. per 100 mil)
	int			m_PAD_PERCENT		= 90;				// Range 50 to 98 of a grid square   (i.e. 1 PERCENT = 1 mil)
	int			m_TRACK_PERCENT		= 46;				// Range 12 to 50 of a grid square   (i.e. 1 PERCENT = 1 mil)
	int			m_HOLE_PERCENT		= 26;				// Range 20 to 40 of a grid square   (i.e. 1 PERCENT = 1 mil)
	int			m_GAP_PERCENT		= 10;				// Range  5 to 30 of a grid square   (i.e. 1 PERCENT = 1 mil)
	int			m_MASK_PERCENT		= 4;				// Range  0 to 10 of a grid square   (i.e. 1 PERCENT = 1 mil)
	int			m_SILK_PERCENT		= 7;				// Range  1 to 10 of a grid square   (i.e. 1 PERCENT = 1 mil)
	int			m_EDGE_PERCENT		= 20;				// Range  0 to 50 of a grid square   (i.e. 1 PERCENT = 1 mil)
	int			m_VIAPAD_PERCENT	= 50;				// Range 50 to 80 of a grid square   (i.e. 1 PERCENT = 1 mil)
	int			m_VIAHOLE_PERCENT	= 25;				// Range 20 to 40 of a grid square   (i.e. 1 PERCENT = 1 mil)
	int			m_iRenderQuality	= 1;				// 0 (Low) to 2 (High)
	int			m_iSaturation		= 60;				// Track color saturation (20 to 100 percent)
	int			m_iFillSaturation	= 0;				// Component fill saturation (0 to 100 percent)
	int			m_iCropMargin		= 0;				// Number of border squares after auto-crop
	int			m_iTargetRows		= 10;				// Desired board size
	int			m_iTargetCols		= 10;				// Desired board size
	int			m_iTextSizeComp		= 9;				// Point size for component text
	int			m_iTextSizePins		= 9;				// Point size for component pins
	int			m_iRoutingMethod	= 0;				// Routing method. 0 ==> fast, 1 ==> allow rip-up
	bool		m_bShowTarget		= false;			// true ==> show target board area
	bool		m_bWireShare		= true;				// true ==> allow 2 wires per hole
	bool		m_bWireCross		= false;			// true ==> allow wires to cross/overlay
	bool		m_bVeroTracks		= false;
	bool		m_bCurvedTracks		= false;
	bool		m_bFatTracks		= true;
	bool		m_bRoutingEnabled	= false;
	bool		m_bViasEnabled		= true;				// true ==> allow vias for 2-layer boards
	bool		m_bShowGrid			= true;				// true ==> show grid dots
	bool		m_bShowText			= true;				// true ==> show text boxes
	bool		m_bFlipH			= false;			// true ==> flip L and R (with no manual manipulation)
	bool		m_bFlipV			= false;			// true ==> flip T and B (with no manual manipulation)
	bool		m_bPinLabels		= true;				// Show SIP/DIP pins as labels in non-mono mode
	bool		m_bGroundFill		= false;
	bool		m_bVerticalStrips	= true;
	bool		m_bCompEdit			= false;			// true ==> component editor mode
};
