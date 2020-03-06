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

#include "AdjInfoManager.h"
#include "MyRGB.h"

#define MYNUMCOLORS 		12
#define MY_GREY				(MYNUMCOLORS)
#define MY_BLACK			(MYNUMCOLORS+1)
#define NUM_PIXMAP_COLORS	(MYNUMCOLORS+2)
#define MY_LYR_BOT			(MYNUMCOLORS+3)
#define MY_LYR_TOP			(MYNUMCOLORS+4)

static MyRGB g_color[MYNUMCOLORS] = { MyRGB(0x3C18C8), MyRGB(0xC024F8), MyRGB(0xD26060), MyRGB(0xDCDC58)
									, MyRGB(0x60C858), MyRGB(0x5896C8), MyRGB(0x6010FF), MyRGB(0xE142D2)
									, MyRGB(0xE18C30), MyRGB(0xA0C828), MyRGB(0x48C896), MyRGB(0x5080FF) };

// Manager class to handle assignment of colors to nodeIds

const int BAD_COLORID = -1;

class ColorManager
{
public:
	ColorManager() : m_iSaturation(100), m_iFillSaturation(0), m_bReAssign(true) {}
	ColorManager(const ColorManager& o) { assert(0); *this = o; }	// Never called
	ColorManager& operator=(const ColorManager& o)
	{
		m_mapNodeIdToColorId.clear();
		m_mapNodeIdToColorId.insert(o.m_mapNodeIdToColorId.begin(), o.m_mapNodeIdToColorId.end());
		m_iSaturation		= o.m_iSaturation;
		m_iFillSaturation	= o.m_iFillSaturation;
		m_bReAssign			= o.m_bReAssign;
		return *this;
	}
	~ColorManager()					{ m_mapNodeIdToColorId.clear(); }
	void ReAssignColors()			{ m_bReAssign = true; }
	void SetSaturation(int i)		{ m_iSaturation = i; }
	void SetFillSaturation(int i)	{ m_iFillSaturation = i; }
	void CalculateColors(AdjInfoManager& adjManager, ElementGrid* pBoard)	// The coloring algorithm
	{
		adjManager.SortByLowestNodeId();
		if ( m_bReAssign )
		{
			if ( pBoard->GetNumNodeIds() > MYNUMCOLORS )
				m_mapNodeIdToColorId.clear();	// Only wipe if we require more colors
			m_bReAssign = false;
		}
		std::list<int> cnList[MYNUMCOLORS];	// Lists of nodeIds used by colours
		int iStartColorId(0);
		for (size_t i = 0, iSize = adjManager.GetSize(); i < iSize; i++)	// Loop nodeIds in adjManager
		{
			AdjInfo*	pI		= adjManager.GetAt(i);
			const int&	nodeIdI	= pI->GetNodeId();
			if ( nodeIdI == BAD_NODEID ) continue;
			const int colorIdI = GetColorId(nodeIdI);
			if ( colorIdI != BAD_COLORID )
			{
				cnList[colorIdI].push_back(nodeIdI);	// Update list
				continue;	// Don't recolor
			}
			int bestColorId(0), minCost(INT_MAX);
			for (int iLoopColor = 0; iLoopColor < MYNUMCOLORS; iLoopColor++)
			{
				const int iColorId = ( iStartColorId + iLoopColor ) % MYNUMCOLORS;
				m_mapNodeIdToColorId[nodeIdI] = iColorId;
				int cost(0);
				for (size_t j = 0; j < iSize; j++)	// Loop nodeIds in adjManager
				{
					if ( j == i ) continue;	// Skip self
					AdjInfo*	pJ		= adjManager.GetAt(j);
					const int&	nodeIdJ	= pJ->GetNodeId();
					if ( nodeIdJ == BAD_NODEID || !pI->GetHasAdj(nodeIdJ) ) continue;	// Skip non-adjacent nodes
					assert( pJ->GetHasAdj(nodeIdI) );	// Sanity check.  Adjacencies should be symmetric
					if ( iColorId == GetColorId(nodeIdJ) ) cost++;
				}
				if ( cost < minCost ) { minCost = cost;	bestColorId = iColorId; }	// Update bestColorId
			}
			m_mapNodeIdToColorId[nodeIdI] = bestColorId;
			cnList[bestColorId].push_back(nodeIdI);	// Update list
			iStartColorId = bestColorId;
		}
		// Handle case when we have unused colors
		while ( true )
		{
			// Find first unused colorId, and the most used colorId (used more than once)
			int iMostUsedColor(BAD_COLORID), iUnusedColor(BAD_COLORID);
			size_t nMaxCount(1);
			for (int i = 0; i < MYNUMCOLORS; i++)
			{
				if ( cnList[i].size() > nMaxCount ) { iMostUsedColor = i; nMaxCount = cnList[i].size(); }
				if ( cnList[i].size() == 0 && iUnusedColor == BAD_COLORID) iUnusedColor = i;
			}
			if ( iUnusedColor == BAD_COLORID ) break;		// All colors used
			if ( iMostUsedColor == BAD_COLORID ) break;		// No color used more than once
			// Do the color swap
			auto iter = cnList[iMostUsedColor].begin(); ++iter;
			const int iNodeId = *iter;						// Pick second lowest nodeId
			m_mapNodeIdToColorId[iNodeId] = iUnusedColor;	// Give it the unused colorId
			cnList[iMostUsedColor].erase(iter);				// Remove nodeId from list of iMostUsedColor
			cnList[iUnusedColor].push_back(iNodeId);		// Add nodeId to list of iUnusedColor
		}
	}
	int GetColorId(const int& nodeId) const
	{
		if ( nodeId == BAD_NODEID ) return BAD_COLORID;
		const auto iter = m_mapNodeIdToColorId.find(nodeId);
		return ( iter != m_mapNodeIdToColorId.end() ) ? iter->second : BAD_COLORID;
	}
	void GetRGB(const int& colorId, int& R, int& G, int& B) const
	{
		if ( colorId == BAD_COLORID ) { R = G = B = 255; return; }
		const int iA = (100 - m_iSaturation) * 255;
		if ( m_iFillSaturation == 0 )
		{
			MyRGB& rgb = g_color[colorId % MYNUMCOLORS];
			R = ( iA + m_iSaturation * rgb.GetR() ) / 100;
			G = ( iA + m_iSaturation * rgb.GetG() ) / 100;
			B = ( iA + m_iSaturation * rgb.GetB() ) / 100;
		}
		else
		{
			R = G = B = iA / 100;
		}
	}
	void GetPixmapRGB(const int& iEffColorId, int& R, int& G, int& B) const
	{
		if ( iEffColorId < MYNUMCOLORS ) return GetRGB(iEffColorId, R, G, B);
		if ( iEffColorId == MY_GREY )		{ R = G = B = 96;			return; }
		if ( iEffColorId == MY_LYR_BOT )	{ R = B = 0; G = 128;		return; }
		if ( iEffColorId == MY_LYR_TOP )	{ R = 0; G = 96; B = 192; 	return; }
		R = G = B = 0;	assert( iEffColorId == MY_BLACK );
	}
private:
	std::unordered_map<int,int>	m_mapNodeIdToColorId;
	int							m_iSaturation;			// 0 to 100. At 0 the colors would all fade to white.
	int							m_iFillSaturation;		// 0 to 100. If non-zero then turn colors grey.
	bool						m_bReAssign;
};
