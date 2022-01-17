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

#include "Board.h"


// Import Protel V1 / Tango netlist (exported from TinyCAD / gEDA)

bool Board::ImportTango(const TemplateManager& templateMgr, const std::string& filename, std::string& errorStr)
{
	Clear();

	std::string			nameStr;	// "R23","U1"			==> TinyCAD "Ref"     / gEDA "refdes"
	std::string			valueStr;	// "4k7", "TL072"		==> TinyCAD "Name"    / gEDA "device"
	std::string			typeStr;	// "RESISTOR","DIP8"	==> TinyCAD "Package" / gEDA "footprint"
	std::string			typeStrCut;	// Cut down version of typeStr. e.g.  DIP40 ==> DIP
	std::string			pinStr;		// Number of pins, or pin number
	std::string			netStr;		// Net name
	std::vector<int>	nodeList;

	// List of package identifiers for footprints with variable numbers of pins/lengths.
	// "PADS" ==> Create separate on-board PAD objects for an off-board part.
	// "SWITCH_ST_DIP" must be tested before "SWITCH_ST_DIP".
	const int NUM_VARIABLE_PIN_PARTS = 13;
	const std::string strVar[NUM_VARIABLE_PIN_PARTS] = {"SIP", "DIP", "PADS", "SWITCH_ST_DIP", "SWITCH_ST", "SWITCH_DT", "STRIP_100MIL", "BLOCK_100MIL", "BLOCK_200MIL", "RESISTOR", "DIODE", "CAP_CERAMIC", "CAP_FILM"};

	std::ifstream inStream;
	inStream.open(filename.c_str(), std::ios::in | std::ios::binary);
	bool bOK = inStream.is_open();
	bool bPart(false), bNet(false);	// Flags indicating "part" and "netlist" sections
	int iRow(0);					// Row counter within "part" and "netlist" sections
	int iNodeId(BAD_NODEID);		// Increase this with each imported node

	std::list<std::string> offBoard;	// List of off-board part names
	while( bOK )	// Loop through file
	{
		if ( inStream.eof() ) break;

		std::string str;							// For reading from file.  Ensure clear before reading
		StringHelper::getline_safe(inStream, str);	// Read the whole line and handle line-ending nicely

		if ( str == "[" ) { bOK = !bPart && !bNet;	bPart = true;	iRow = 0;	continue; }
		if ( str == "]" ) { bOK =  bPart && !bNet;	bPart = false;				continue; }
		if ( str == "(" ) { bOK = !bPart && !bNet;	bNet  = true;	iRow = 0; 	continue; }
		if ( str == ")" ) { bOK = !bPart &&  bNet;	bNet  = false;				continue; }

		if ( bPart )	// We're in a "Part" description section
		{
			if ( iRow == 0 )
			{
				nameStr = str;	// TinyCAD "Ref" / gEDA "refdes"		==> VeroRoute "Name"
				if ( bOK )
				{
					bOK  = ( str.find("-") == std::string::npos );	// Name should not have a "-"
					if ( !bOK ) errorStr = "Part section: " + nameStr + "\nPart names must not contain a minus sign";
				}
				if ( bOK )
				{
					bOK = ( m_compMgr.GetComponentIdFromName(nameStr) == BAD_COMPID );	// Name must be unique
					if ( !bOK ) errorStr = "Part section: " + nameStr + "\nPart names must be unique";
				}
			}
			if ( iRow == 1 )
			{
				typeStrCut = typeStr = str;	// TinyCAD "Package" / gEDA "footprint"	==> VeroRoute "Type"
			}
			if ( iRow == 2 )
			{
				valueStr = str;	// TinyCAD "Name" / gEDA "device"		==> VeroRoute "Value"

				// Build the part and place it ...

				// If footprint is variable length, then get the number of pins/length from typeStr
				int numPins(0), nLength(0);	// Invalid by default
				for (int i = 0; i < NUM_VARIABLE_PIN_PARTS; i++)
				{
					const std::string&	strTmp	= strVar[i];	// e.g. "SIP", "DIP, etc
					const auto			L		= strTmp.length();
					if ( typeStr.length() >= L && typeStr.substr(0, L) == strTmp )
					{
						pinStr		= typeStr.substr(L);	// e.g. "DIP40" ==> "40"
						typeStrCut	= typeStr.substr(0, L);	// e.g. "DIP40" ==> "DIP"
						if ( typeStrCut == "PADS" )				// If we have an off-board part ...
						{
							typeStrCut = "SIP";					// ... treat it as a SIP for the moment
							offBoard.push_back(nameStr);		// ... and add it to the list of off-board parts
						}
						if ( typeStrCut == "RESISTOR" || typeStrCut == "DIODE" || typeStrCut == "CAP_CERAMIC" || typeStrCut == "CAP_FILM" )
						{
							nLength = atoi( pinStr.c_str() );	// Missing or zero ==> Use default length
							if ( nLength > 0 )					// The length is in 100ths of a mil ...
								nLength += 1;					// ... so must add 1 to get part length in grid squares
						}
						else	// DIP/SIP/SWITCH/STRIP/BLOCK
						{
							numPins = atoi( pinStr.c_str() );
							if ( numPins == 0 )					// Missing or zero ...
								numPins = -1;					// ... use -1 instead.  Don't use 0 as that implies "use default".
						}
						break;
					}
				}

				bool bCustom(false);	// true ==> We've found a custom template with matching import string
				Component custom;		// The matching custom template

				const COMP eType = GetTypeFromImportStr(typeStrCut);
				if ( bOK )
				{
					bOK = ( eType != COMP::CUSTOM && eType != COMP::TRACKS && eType != COMP::INVALID );
					if ( !bOK )	// Search template manager
						bOK = bCustom = templateMgr.GetFromImportStr(typeStrCut, custom);
					if ( !bOK ) errorStr = "Part section: " + nameStr + "\nVeroRoute does not support the part type: " + typeStr;
				}

				// Check pins per component is within limits
				if ( numPins == 0 ) numPins = ( bCustom ) ? (int) custom.GetNumPins() : GetDefaultNumPins(eType);
				if ( bOK )
				{
					bOK = ( numPins > 0 );
					if ( !bOK ) errorStr = "Part section: " + nameStr + "\nInternal error: Part type has no pins " + typeStr;
				}
				// Check length is within limits for components with fixed numbers of pins
				if ( nLength > 0 )
				{
					if ( bOK )
					{
						bOK = bCustom || ( nLength >= GetMinLength(eType) );
						if ( !bOK ) errorStr = "Part section: " + nameStr + "\nInternal error: Part length is too small " + typeStr;
					}
					if ( bOK )
					{
						bOK = bCustom || ( nLength <= GetMaxLength(eType) );
						if ( !bOK ) errorStr = "Part section: " + nameStr + "\nInternal error: Part length is too large " + typeStr;
					}
				}
				if ( bOK )
				{
					bOK = bCustom || ( numPins >= GetMinNumPins(eType) );
					if ( !bOK ) errorStr = "Part section: " + nameStr + "\nPart type has fewer pins than VeroRoute supports: " + typeStr;
				}
				if ( bOK )
				{
					bOK = bCustom || ( numPins <= GetMaxNumPins(eType) );
					if ( !bOK ) errorStr = "Part section: " + nameStr + "\nPart type has more pins than VeroRoute supports: " + typeStr;
				}
				if ( bOK )
				{
					if ( bCustom )
					{
						assert( custom.GetType() == COMP::CUSTOM );
						custom.SetNameStr(nameStr);
						custom.SetValueStr(valueStr);
						bOK = ( AddComponent(-1, -1, custom) != BAD_COMPID );	// Create part and place it

					}
					else
					{
						nodeList.resize(numPins, BAD_NODEID);
						Component tmp(nameStr, valueStr, eType, nodeList);
						if ( nLength > 0 )
						{
							while ( tmp.GetCols() < nLength ) tmp.Stretch(true);	// grow
							while ( tmp.GetCols() > nLength ) tmp.Stretch(false);	// shrink
						}
						bOK = ( AddComponent(-1, -1, tmp) != BAD_COMPID );	// Create part and place it
					}
					if ( !bOK ) errorStr = "Part section: " + nameStr + "\nInternal error creating and placing the part";
				}
			}
		}
		if ( bNet )	// We're in a "Netlist" description section
		{
			if ( iRow == 0 )
			{
				netStr = str;
				iNodeId++;	// str has the net name, so make a new NodeID for it
			}
			else
			{
				// The line should have a part identifier and pin number separated by a '-'.
				const auto pos = str.find("-");
				if ( bOK )
				{
					bOK  = ( pos != std::string::npos );	// Should have a "-" on the line
					if ( !bOK ) errorStr = "Net section: " + netStr + "\nLine has no minus sign: " + str;
				}
				if ( bOK)
				{
					pinStr = str.substr(pos+1);
					bOK = ( pinStr.find("-") == std::string::npos );	// Should only have one "-" on the line
					if ( !bOK )	errorStr = "Net section: " + netStr + "\nLine has more than one minus sign: " + str;
				}
				const auto nameStr = ( bOK ) ? str.substr(0, pos) : std::string("");
				if ( bOK )
				{
					bOK = !StringHelper::IsEmptyStr(nameStr);	// Should have part name
					if ( !bOK ) errorStr = "Net section: " + netStr + "\nLine has no part identifier: " + str;
				}
				if ( bOK )
				{
					// Paint the component pin using SetNodeIdByUser
					const size_t	iPinIndex	= atoi(pinStr.c_str()) - 1;
					const int		compId		= m_compMgr.GetComponentIdFromName(nameStr);

					bOK = compId != BAD_COMPID;
					if ( !bOK ) errorStr = "Net section: " + netStr + "\nLine has unknown part identifier: " + str;

					if ( bOK )
					{
						bOK = iPinIndex < m_compMgr.GetComponentById(compId).GetNumPins();
						if ( !bOK ) errorStr = "Net section: " + netStr + "\nLine has invalid pin number: " + str;
					}
					if ( bOK )
					{
						int row, col;
						bOK = GetPinRowCol(compId, iPinIndex, row, col);
						if ( !bOK )	errorStr = "Net section: " + netStr + "\nLine: " + str + "\nInternal error mapping the pin to a board location";
						if ( bOK )
							SetNodeIdByUser(0, row, col, iNodeId, true);	// true ==> paint pins
					}
				}
			}
		}
		iRow++;
	}
	if ( inStream.is_open() ) inStream.close();

	// Break the SIPS representing off-board parts into PADs
	if ( bOK )
		for (const auto& nameStr : offBoard)
			BreakComponentIntoPads( m_compMgr.GetComponentById( m_compMgr.GetComponentIdFromName(nameStr) ) );

	return bOK;
}


// Import OrCAD2 netlist (exported from KiCAD)
bool Board::ImportOrcad(const TemplateManager& templateMgr, const std::string& filename, std::string& errorStr)
{
	Clear();

	std::string			nameStr;	// "R23","U1"			==> TinyCAD "Ref"     / gEDA "refdes"
	std::string			valueStr;	// "4k7", "TL072"		==> TinyCAD "Name"    / gEDA "device"
	std::string			typeStr;	// "RESISTOR","DIP8"	==> TinyCAD "Package" / gEDA "footprint"
	std::string			typeStrCut;	// Cut down version of typeStr. e.g.  DIP40 ==> DIP
	std::string			pinStr;		// Number of pins, or pin number
	std::string			netStr;		// Net name
	std::vector<int>	nodeList;

	// List of package identifiers for footprints with variable numbers of pins/lengths.
	// "PADS" ==> Create separate on-board PAD objects for an off-board part.
	// "SWITCH_ST_DIP" must be tested before "SWITCH_ST_DIP".
	const int NUM_VARIABLE_PIN_PARTS = 13;
	const std::string strVar[NUM_VARIABLE_PIN_PARTS] = {"SIP", "DIP", "PADS", "SWITCH_ST_DIP", "SWITCH_ST", "SWITCH_DT", "STRIP_100MIL", "BLOCK_100MIL", "BLOCK_200MIL", "RESISTOR", "DIODE", "CAP_CERAMIC", "CAP_FILM"};

	std::ifstream inStream;
	inStream.open(filename.c_str(), std::ios::in | std::ios::binary);
	bool bOK = inStream.is_open();
	int maxNodeId(BAD_NODEID);		// Increase this with each new node we encounter

	std::unordered_map<std::string, int> mapNetToNodeId;

	bool bPartStart(false);
	std::list<std::string> offBoard;	// List of off-board part names

	int compId(BAD_COMPID);

	while( bOK )	// Loop through file
	{
		if ( inStream.eof() ) break;

		std::string str;							// For reading from file.  Ensure clear before reading
		StringHelper::getline_safe(inStream, str);	// Read the whole line and handle line-ending nicely
		if ( str.empty() ) continue;	// Skip blank lines

		// First non-blank char on every line should be '(' or ')' or '*'
		const bool bCurvedOpen		= str.find("(")	!= std::string::npos;
		const bool bCurvedClose		= str.find(")")	!= std::string::npos;

		if ( bCurvedOpen && str.find("{") != std::string::npos && str.find("}") != std::string::npos )
			continue;	// Line is a comment so skip it

		if ( !bCurvedOpen && !bCurvedClose )
		{
			if ( str.find("*") != std::string::npos )
				break;	// reached the '*'
			else
			{
				bOK = false;
				errorStr = "Expecting all lines to start with '('' or ')'' or '*'";
				break;
			}
		}

		if ( bCurvedOpen && !bPartStart )
		{
			// We're expecting the line to say something like "( /5D5ADFE2 DIP16 IC1 SAD1024"

			std::vector<std::string> strList;

			StringHelper::GetSubStrings(str, strList);	// Break str into space separated list
			const size_t numSubStrings =  strList.size();

			if ( numSubStrings < 4 || numSubStrings > 5 )
			{
				bOK = false;
				errorStr = "Expecting format:  ( /5D5ADFE2 FOOTPRINT NAME VALUE  , but got:" + str;
				break;
			}

			typeStrCut	= typeStr	= strList[2];
			nameStr		= strList[3];
			valueStr	= ( numSubStrings == 5 ) ? strList[4] : "";

			bOK = ( m_compMgr.GetComponentIdFromName(nameStr) == BAD_COMPID );	// Name must be unique
			if ( !bOK )
			{
				errorStr = "\nPart name " + nameStr + " is not unique";
				break;
			}

			// Build the part and place it ...

			// If footprint is variable length, then get the number of pins/length from typeStr
			int numPins(0), nLength(0);	// Invalid by default
			for (int i = 0; i < NUM_VARIABLE_PIN_PARTS; i++)
			{
				const std::string&	strTmp	= strVar[i];	// e.g. "SIP", "DIP, etc
				const auto			L		= strTmp.length();
				if ( typeStr.length() >= L && typeStr.substr(0, L) == strTmp )
				{
					pinStr		= typeStr.substr(L);	// e.g. "DIP40" ==> "40"
					typeStrCut	= typeStr.substr(0, L);	// e.g. "DIP40" ==> "DIP"
					if ( typeStrCut == "PADS" )				// If we have an off-board part ...
					{
						typeStrCut = "SIP";					// ... treat it as a SIP for the moment
						offBoard.push_back(nameStr);		// ... and add it to the list of off-board parts
					}
					if ( typeStrCut == "RESISTOR" || typeStrCut == "DIODE" || typeStrCut == "CAP_CERAMIC" || typeStrCut == "CAP_FILM" )
					{
						nLength = atoi( pinStr.c_str() );	// Missing or zero ==> Use default length
						if ( nLength > 0 )					// The length is in 100ths of a mil ...
							nLength += 1;					// ... so must add 1 to get part length in grid squares
					}
					else	// DIP/SIP/SWITCH/STRIP/BLOCK
					{
						numPins = atoi( pinStr.c_str() );
						if ( numPins == 0 )					// Missing or zero ...
							numPins = -1;					// ... use -1 instead.  Don't use 0 as that implies "use default".
					}
					break;
				}
			}

			bool bCustom(false);	// true ==> We've found a custom template with matching import string
			Component custom;		// The matching custom template

			const COMP eType = GetTypeFromImportStr(typeStrCut);

			bOK = ( eType != COMP::CUSTOM && eType != COMP::TRACKS && eType != COMP::INVALID );
			if ( !bOK )	// Search template manager
				bOK = bCustom = templateMgr.GetFromImportStr(typeStrCut, custom);
			if ( !bOK )
			{
				errorStr = "Part: " + nameStr + "\nVeroRoute does not support the part type: " + typeStr;
				break;
			}

			// Check pins per component is within limits
			if ( numPins == 0 ) numPins = ( bCustom ) ? (int) custom.GetNumPins() : GetDefaultNumPins(eType);
			bOK = ( numPins > 0 );
			if ( !bOK )
			{
				errorStr = "Part: " + nameStr + "\nInternal error: Part type has no pins " + typeStr;
				break;
			}

			// Check length is within limits for components with fixed numbers of pins
			if ( nLength > 0 )
			{
				bOK = bCustom || ( nLength >= GetMinLength(eType) );
				if ( !bOK )
				{
					errorStr = "Part: " + nameStr + "\nInternal error: Part length is too small " + typeStr;
					break;
				}
				bOK = bCustom || ( nLength <= GetMaxLength(eType) );
				if ( !bOK )
				{
					errorStr = "Part: " + nameStr + "\nInternal error: Part length is too large " + typeStr;
					break;
				}
			}

			bOK = bCustom || ( numPins >= GetMinNumPins(eType) );
			if ( !bOK )
			{
				errorStr = "Part: " + nameStr + "\nPart type has fewer pins than VeroRoute supports: " + typeStr;
				break;
			}
			bOK = bCustom || ( numPins <= GetMaxNumPins(eType) );
			if ( !bOK )
			{
				errorStr = "Part: " + nameStr + "\nPart type has more pins than VeroRoute supports: " + typeStr;
				break;
			}
			if ( bCustom )
			{
				assert( custom.GetType() == COMP::CUSTOM );
				custom.SetNameStr(nameStr);
				custom.SetValueStr(valueStr);
				bOK = ( AddComponent(-1, -1, custom) != BAD_COMPID );	// Create part and place it
			}
			else
			{
				nodeList.resize(numPins, BAD_NODEID);
				Component tmp(nameStr, valueStr, eType, nodeList);
				if ( nLength > 0 )
				{
					while ( tmp.GetCols() < nLength ) tmp.Stretch(true);	// grow
					while ( tmp.GetCols() > nLength ) tmp.Stretch(false);	// shrink
				}
				compId = AddComponent(-1, -1, tmp);
				bOK = ( compId!= BAD_COMPID );	// Create part and place it
			}
			if ( !bOK )
			{
				errorStr = "Part: " + nameStr + "\nInternal error creating and placing the part";
				break;
			}

			bPartStart = true;
			continue;
		}
		else
		{
			// we're in the pin section...
			assert( bCurvedClose );	// Should have ')' on every line
			if ( !bCurvedOpen )
			{
				// If we have just a ')' on the line then we're done with the pins ...
				bPartStart = false;	// ... and we're done with the part, move onto the next one
				compId = BAD_COMPID;
				continue;
			}

			std::vector<std::string> strList;

			StringHelper::GetSubStrings(str, strList);	// Break str into space separated list
			if ( strList.size() != 4 )
			{
				bOK = false;
				errorStr = "Expecting format:  ( PINNUMBER NETNAME )  , but got:" + str;
				break;
			}

			pinStr = strList[1];
			netStr = strList[2];

			int nodeId(BAD_NODEID);
			auto iter = mapNetToNodeId.find(netStr);
			if ( iter != mapNetToNodeId.end() )
				nodeId = iter->second;
			else
			{
				maxNodeId++;
				mapNetToNodeId[netStr] = nodeId = maxNodeId;
			}
			bOK = ( nodeId != BAD_NODEID );
			if ( !bOK )
			{
				errorStr = "Internal error: VeroRoute has run out of node IDs";
				break;
			}

			// Paint the component pin using SetNodeIdByUser
			const size_t	iPinIndex	= atoi(pinStr.c_str()) - 1;
			const int		compId		= m_compMgr.GetComponentIdFromName(nameStr);

			bOK = compId != BAD_COMPID;
			if ( !bOK )
			{
				errorStr = "Internal error: VeroRoute lost the component ID";
				break;
			}
			bOK = iPinIndex < m_compMgr.GetComponentById(compId).GetNumPins();
			if ( !bOK )
			{
				errorStr = "Part: " + nameStr + "\nLine has invalid pin number: " + str;
				break;
			}
			int row, col;
			bOK = GetPinRowCol(compId, iPinIndex, row, col);
			if ( !bOK )
			{
				errorStr = "Part: " + nameStr + "\nLine: " + str + "\nInternal error mapping the pin to a board location";
				break;
			}

			SetNodeIdByUser(0, row, col, nodeId, true);	// true ==> paint pins
		}
	}
	if ( inStream.is_open() ) inStream.close();

	// Break the SIPS representing off-board parts into PADs
	if ( bOK )
		for (const auto& nameStr : offBoard)
			BreakComponentIntoPads( m_compMgr.GetComponentById( m_compMgr.GetComponentIdFromName(nameStr) ) );

	return bOK;
}


bool Board::BreakComponentIntoPads(Component& comp)
{
	const COMP& eType = comp.GetType();
	if ( eType == COMP::MARK || eType == COMP::PAD || eType == COMP::WIRE ) return false;	// Not real components
	if ( !comp.GetIsPlaced() ) return false;	// Can't break a floating component

	std::vector<int> nodeList;	// Re-used for each new pad
	nodeList.resize(1, BAD_NODEID);

	const size_t numPins = comp.GetNumPins();
	for (size_t iPinIndex = 0; iPinIndex < numPins; iPinIndex++)	// Loop component pins
	{
		// Create a new PAD component for the pin, with suitable name, value, nodeId
		static char buffer[32];
		sprintf(buffer, "_%d", (int)(iPinIndex+1));
		nodeList[0] = comp.GetNodeId(iPinIndex);
		Component tmp(comp.GetNameStr() + std::string(buffer), comp.GetValueStr(), COMP::PAD, nodeList);

		// Find board location of existing pin, and put the new PAD there
		int row, col;
		if ( GetPinRowCol(comp.GetId(), iPinIndex, row, col) )
		{
			tmp.SetRow(row);
			tmp.SetCol(col);
			AddComponent(-1, -1, tmp, false);	// Add PAD floating over the existing pin
		}
	}
	DestroyComponent(comp);	// All pins have been copied, so destroy the old component
	PlaceFloaters();		// Unfloat the new PADs
	return true;
}

bool Board::GetPinRowCol(const int& compId, const size_t& iPinIndex, int& row, int& col) const
{
	if ( compId == BAD_COMPID || iPinIndex == BAD_PININDEX ) return false;

	for (int i = 0, iSize = GetSize(); i < iSize; i++)
	{
		Element* p = GetAtConst(i);
		if ( !p->GetHasWire() && p->GetCompId() == compId && p->GetPinIndex() == iPinIndex )
		{
			GetRowCol(p, row, col);
			return true;
		}
	}
	return false;
}
