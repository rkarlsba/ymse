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

#include "Pin.h"
#include "TrackElement.h"

class CompElement : public Pin, public TrackElement
{
	friend class Element;
public:
	CompElement() : Pin(), TrackElement() {}
	CompElement(const CompElement& o) : Pin(o), TrackElement(o) { *this = o; }
	~CompElement() {}
	CompElement& operator=(const CompElement& o)
	{
		Pin::operator=(o);			// Call operator= in base class
		TrackElement::operator=(o);	// Call operator= in base class
		return *this;
	}
	bool operator==(const CompElement& o) const	// Compare persisted info
	{
		return	Pin::operator==(o)
			&&	TrackElement::operator==(o);
	}
	bool operator!=(const CompElement& o) const
	{
		return !(*this == o);
	}

	// Merge interface functions
	virtual void UpdateMergeOffsets(MergeOffsets& o) override
	{
		Pin::UpdateMergeOffsets(o);	// Does nothing
		TrackElement::UpdateMergeOffsets(o);
	}
	virtual void ApplyMergeOffsets(const MergeOffsets& o) override
	{
		Pin::ApplyMergeOffsets(o);	// Does nothing
		TrackElement::ApplyMergeOffsets(o);
	}
	void Merge(const CompElement& o)
	{
		Pin::Merge(o);
		TrackElement::Merge(o);
	}
	// Persist interface functions
	virtual void Load(DataStream& inStream) override
	{
		if ( inStream.GetVersion() < VRT_VERSION_25 )
		{
			int  compId;	// Dummy
			bool bIsMark;	// Dummy
			Pin::Load(inStream);			// Load() base class
			inStream.Load(compId);			assert( compId == -1 );
			TrackElement::Load(inStream);	// Load() base class
			inStream.Load(bIsMark);			assert( bIsMark == false );
		}
		else
		{
			Pin::Load(inStream);			// Load() base class
			TrackElement::Load(inStream);	// Load() base class
		}
	}
	virtual void Save(DataStream& outStream) override
	{
		Pin::Save(outStream);				// Save() base class
		TrackElement::Save(outStream);		// Save() base class
	}
};
