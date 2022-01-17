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

// Class to manage history files for undo/redo functionality
// An upper limit of 1000 files has been set, so the max undo level is 999.
// This is to prevent the files using too much disk space.

#include "Board.h"

const size_t MAX_HISTORY_FILES = 1000;

typedef std::tuple<size_t, int, std::string>	HistoryItem;	// <index, compId, description>
typedef std::list<HistoryItem>::const_iterator	HistoryItemIter;

class HistoryManager
{
public:
	HistoryManager() : m_ID(0), m_bLocked(false) {}
	~HistoryManager() { Clear(); }
	void SetPathStr(const std::string& str)	{ m_pathStr = str; }
	bool GetCanUndo() const					{ return !m_list.empty() && m_currentIter != m_list.begin(); }
	bool GetCanRedo() const					{ return !m_list.empty() && GetNextIter() != m_list.end();   }
	const std::string& GetUndoText() const	{ return std::get<2>(*m_currentIter); }
	const std::string& GetRedoText() const	{ return std::get<2>(*GetNextIter()); }
	bool Reset(const std::string& str, Board& board)
	{
		if ( m_bLocked ) return false;

		if ( m_ID != 0 ) Clear();	// If m_ID is valid then wipe all files that used it

		// Set a new (unique) m_ID
		for (m_ID = 1; m_ID < INT_MAX; m_ID++ )	// Keep increasing this until we get a unique ID
		{
			std::ifstream fTest( GetFilename(0) );	// See if (zeroth) filename with this ID exists
			if ( !fTest.good() ) break;				// ID is not already in use so break
		}

		AddEntry(0, BAD_COMPID, str);
		return Save(board);
	}
	bool Update(const std::string& str, const int compId, Board& board)
	{
		if ( m_bLocked ) return false;

		if ( compId != BAD_COMPID )	// compId is only used when updating the Name and Value fields for a part
		{							// or the 4 text fields in the component editor (which always uses compId == 0)
			assert(	str ==	"Part name change"	||
					str ==	"Part value change"	||
					str ==	"Edit Value string"	||
					str ==	"Edit Prefix string"||
					str ==	"Edit Type string"	||
					str ==	"Edit Import string" );
			if ( (std::get<1>(*m_currentIter) == compId ) &&	// If we're working on the same part ...
				 (std::get<2>(*m_currentIter) == str ) )		// ... and the same text field ...
				return Save(board);								// ... then overwrite the current entry instead of adding a new one
		}

		auto iterNext = GetNextIter();
		if ( iterNext != m_list.end() )	// We're modifying within the list ...
		{
			// Delete later history files
			for (auto iter = iterNext; iter != m_list.end(); ++iter) remove( GetFilename(std::get<0>(*iter)) );
			m_list.erase(iterNext, m_list.end());	// Erase later history list items
		}
		else if ( m_list.size() == MAX_HISTORY_FILES )	// We're at the end of the list and the list is full ...
		{
			// We'll overwrite the first history file, so no need to delete it
			m_list.erase(m_list.begin());	// Erase first history list item
		}
		AddEntry(std::get<0>(*m_currentIter) + 1, compId, str);

		return Save(board);
	}
	void Lock()		{ m_bLocked = true;  }
	void UnLock()	{ m_bLocked = false; }
	bool Undo(Board& board)
	{
		assert( m_bLocked );	// Should only Undo while the list is locked
		if ( !GetCanUndo() ) return false;
		--m_currentIter;
		return Load(board);
	}
	bool Redo(Board& board)
	{
		assert( m_bLocked );	// Should only Redo while the list is locked
		if ( !GetCanRedo() ) return false;
		++m_currentIter;
		return Load(board);
	}
	const char* GetCurrentFilename() const { return ( m_list.empty() ) ? "\0" : GetFilename(std::get<0>(*m_currentIter)); }
private:
	void AddEntry(const size_t& index, const int compId, const std::string& str)
	{
		m_list.push_back( HistoryItem(index % MAX_HISTORY_FILES, compId, str) );
		m_currentIter = m_list.end();
		m_currentIter--;
	}
	void Clear()
	{
		for (size_t i = 0; i < MAX_HISTORY_FILES; i++) remove( GetFilename(i) );// Delete all possible history files
		m_bLocked = false;
		m_list.clear();
	}
	bool Load(Board& board)
	{
		DataStream inStream(DataStream::READ);
		if ( !inStream.Open( GetCurrentFilename() ) ) return false;
		board.Load(inStream);
		inStream.Close();
		return inStream.GetOK();
	}
	bool Save(Board& board)
	{
		DataStream outStream(DataStream::WRITE);
		if ( !outStream.Open( GetCurrentFilename() ) ) return false;
		board.Save(outStream);
		outStream.Close();
		return true;
	}
	HistoryItemIter GetNextIter() const { auto iter = m_currentIter; ++iter; return iter; }
	const char* GetFilename(const size_t& index) const
	{
		assert( index < MAX_HISTORY_FILES );	// Sanity check
		memset(m_buffer, 0, 256 * sizeof(char));
		sprintf(m_buffer, "%s/history/history_%d_%d.vrt", m_pathStr.c_str(), m_ID, (int)index);
		return m_buffer;
	}
private:
	std::string				m_pathStr;		// Path to the "history" folder
	int						m_ID;			// Allows multiple VeroRoute instances to share the same "history" folder.
	bool					m_bLocked;		// Should lock the list while doing Undo() or Redo()
	std::list<HistoryItem>	m_list;			// Each history item is an <Index, Description> pair
	HistoryItemIter			m_currentIter;	// Points to the last written history item
	mutable char			m_buffer[256];	// For constructing history filenames
};
