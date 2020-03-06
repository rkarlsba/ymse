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

#include "TextRect.h"
#include "GuiControl.h"	// For BAD_TEXTID

class TextManager : public Persist, public Merge
{
	friend class Board;
public:
	TextManager()	{ Clear(); }
	~TextManager()	{ Clear(); }
	void Clear()	{ m_mapIdtoText.clear(); }
	TextManager(const TextManager& o)	{ *this = o; }
	TextManager& operator=(const TextManager& o)
	{
		m_mapIdtoText.clear();
		for (const auto& mapObj : o.m_mapIdtoText)
		{
			const int&		iTextId		= mapObj.first;
			const TextRect& rect		= mapObj.second;
			m_mapIdtoText[ iTextId ]	= rect;
		}
		return *this;
	}
	bool operator==(const TextManager& o) const	// Compare persisted info
	{
		bool bOK = ( m_mapIdtoText.size() == o.m_mapIdtoText.size() );
		if ( bOK )
		{
			for (const auto& mapObj : m_mapIdtoText)
			{
				const int&		iTextId		= mapObj.first;
				const TextRect&	rectA		= mapObj.second;
				const auto		iterOther	= o.m_mapIdtoText.find( iTextId );
				if ( iterOther == o.m_mapIdtoText.end() ) return false;
				const TextRect& rectB		= iterOther->second;
				bOK = ( rectA == rectB );
				if ( !bOK ) return false;
			}
		}
		return bOK;
	}
	bool operator!=(const TextManager& o) const
	{
		return !(*this == o);
	}
	int AddNewRect(const int& row, const int& col)
	{
		TextRect tmp(row, row+2, col, col+8);

		// Find the first unused textId.
		int textId(0);
		while ( GetTextRectExists(textId) && textId != INT_MAX ) textId++;
		if ( textId == INT_MAX ) return BAD_TEXTID;	// Reached text box limit !!!

		m_mapIdtoText[textId] = tmp;	// tmp is ***copied*** into the map
		return textId;
	}
	void MoveRect(const int& iTextId, const int& deltaRow, const int& deltaCol)
	{
		GetTextRectById(iTextId).Move(deltaRow, deltaCol);
	}
	void UpdateRect(const int& iTextId, const int& row, const int& col)
	{
		auto& rect = GetTextRectById(iTextId);
		rect.m_rowMax = std::max(rect.m_rowMin, row);
		rect.m_colMax = std::max(rect.m_colMin, col);
	}
	void MoveAll(int iDown, int iRight)
	{
		for (auto& mapObj : m_mapIdtoText) mapObj.second.Move(iDown, iRight);
	}
	TextRect& GetTextRectById(const int& textId)
	{
		return m_mapIdtoText[textId];
	}
	const std::unordered_map<int, TextRect>& GetMapIdToText() const
	{
		return m_mapIdtoText;
	}
	void DestroyRect(const int& iTextId)
	{
		const auto iter = m_mapIdtoText.find(iTextId);
		if ( iter != m_mapIdtoText.end() )
			m_mapIdtoText.erase(iter);
	}
	Rect GetBounding() const
	{
		Rect bounding;
		for (const auto& mapObj : m_mapIdtoText) bounding |= mapObj.second;
		return bounding;
	}
	// Merge interface functions
	virtual void UpdateMergeOffsets(MergeOffsets& o) override
	{
		for (auto& mapObj : m_mapIdtoText) mapObj.second.UpdateMergeOffsets(o);
	}
	virtual void ApplyMergeOffsets(const MergeOffsets& o) override
	{
		for (auto& mapObj : m_mapIdtoText) mapObj.second.ApplyMergeOffsets(o);
	}
	void Merge(const TextManager& o)
	{
		int iMaxId(BAD_TEXTID);
		for (auto& mapObj : m_mapIdtoText)
			iMaxId = std::max(iMaxId, (int) mapObj.first);
		for (const auto& mapObj : o.m_mapIdtoText)
		{
			iMaxId++;
			m_mapIdtoText[ iMaxId ] = mapObj.second;
		}
	}
	// Persist interface functions
	virtual void Load(DataStream& inStream) override
	{
		Clear();
		unsigned int iSize(0);
		inStream.Load(iSize);
		m_mapIdtoText.clear();
		for (unsigned int i = 0; i < iSize; i++)
		{
			TextRect tmp;
			tmp.Load(inStream);
			int iTextId = i;
			if ( inStream.GetVersion() >= VRT_VERSION_15 )
				inStream.Load(iTextId);		// Added in VRT_VERSION_15
			m_mapIdtoText[ iTextId ] =  tmp;
		}
	}
	virtual void Save(DataStream& outStream) override
	{
		const unsigned int iSize = static_cast<unsigned int>( m_mapIdtoText.size() );
		outStream.Save(iSize);
		for (auto& mapObj : m_mapIdtoText)
		{
			TextRect& rect = mapObj.second;
			rect.Save(outStream);
			outStream.Save(mapObj.first);	// Added in VRT_VERSION_15
		}
	}
private:
	bool GetTextRectExists(const int& textId) const
	{
		return m_mapIdtoText.find(textId) != m_mapIdtoText.end();
	}
private:
	std::unordered_map<int, TextRect>	m_mapIdtoText;	// The text boxes (indexed by textId)
};
