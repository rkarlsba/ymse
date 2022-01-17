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

// Enumeration and static methods for the component types

//*** Don't delete or modify existing values in COMP.  Just add new ones and update all methods below. ***
enum class COMP {	INVALID					=   -1,
					MARK					=    0,
					PAD						=  100,
					WIRE					=  200,
					RESISTOR				=  300,
					INDUCTOR				=  350,
					DIODE					=  400,
					LED						=  450,
					CAP_CERAMIC				=  500,
					CAP_FILM				=  600,
					CAP_FILM_WIDE			=  620,
					CAP_ELECTRO_200_NP		=  700,
					CAP_ELECTRO_250_NP		=  710,
					CAP_ELECTRO_300_NP		=  720,
					CAP_ELECTRO_400_NP		=  730,
					CAP_ELECTRO_500_NP		=  740,
					CAP_ELECTRO_600_NP		=  750,
					CAP_ELECTRO_200			=  800,
					CAP_ELECTRO_250			=  900,
					CAP_ELECTRO_300			= 1000,
					CAP_ELECTRO_400			= 1010,
					CAP_ELECTRO_500			= 1020,
					CAP_ELECTRO_600			= 1030,
					TO92					= 1100,
					TO18					= 1120,
					TO39					= 1130,
					TO220					= 1140,
					TRIM_VERT				= 1200,
					TRIM_VERT_OFFSET		= 1300,
					TRIM_VERT_OFFSET_WIDE	= 1350,
					TRIM_FLAT				= 1400,
					TRIM_FLAT_WIDE			= 1450,
					TRIM_3006P				= 1451,
					TRIM_3006W				= 1452,
					TRIM_3006Y				= 1453,
					TRIM_3329H				= 1460,
					TRIM_3329P_DK9_RC		= 1465,
					SIP						= 1500,
					DIP						= 1600,
					DIP_RECTIFIER			= 1620,
					STRIP_100				= 1900,
					BLOCK_100				= 1910,
					BLOCK_200				= 1920,
					SWITCH_ST				= 2000,
					SWITCH_DT				= 2100,
					SWITCH_ST_DIP			= 2200,
					SWITCH_BUTTON_4PIN		= 2250,
					CRYSTAL					= 3000,
					RELAY_HK19F				= 4000,
					RELAY_HJR_4102			= 4010,
					RELAY_FTR_B3C			= 4020,
					RELAY_G2R_2				= 4030,
					RELAY_G2R_2A			= 4032,
					RELAY_G2RK_2			= 4034,
					RELAY_G2RK_2A			= 4036,
					RELAY_G3MB_202P			= 4037,
					RELAY_JQC_3F_APPROX		= 4038,
					RELAY_S1A050000			= 4040,
					RELAY_TRCD				= 4042,
					RELAY_DIP_4PIN			= 4050,
					RELAY_DIP_8PIN			= 4052,
					FUSE_HOLDER				= 5000,

					CUSTOM					= 100000,	// A user-defined component
					TRACKS					= 1000000	// A track pattern
				};
//*** Don't delete or modify existing values in COMP.  Just add new ones and update all methods below. ***

static int GetListOrder(const COMP& eType)	// For dialogs/menus.  Lower number ==> higher up list
{
	switch( eType )
	{
		case COMP::PAD					: return 1;
		case COMP::WIRE					: return 2;
		case COMP::RESISTOR				: return 3;
		case COMP::INDUCTOR				: return 4;
		case COMP::CAP_CERAMIC			:
		case COMP::CAP_FILM				:
		case COMP::CAP_FILM_WIDE		: return 5;
		case COMP::CAP_ELECTRO_200		:
		case COMP::CAP_ELECTRO_250		:
		case COMP::CAP_ELECTRO_300		:
		case COMP::CAP_ELECTRO_400		:
		case COMP::CAP_ELECTRO_500		:
		case COMP::CAP_ELECTRO_600		: return 6;
		case COMP::CAP_ELECTRO_200_NP	:
		case COMP::CAP_ELECTRO_250_NP	:
		case COMP::CAP_ELECTRO_300_NP	:
		case COMP::CAP_ELECTRO_400_NP	:
		case COMP::CAP_ELECTRO_500_NP	:
		case COMP::CAP_ELECTRO_600_NP	: return 7;
		case COMP::TRIM_VERT			:
		case COMP::TRIM_VERT_OFFSET		:
		case COMP::TRIM_VERT_OFFSET_WIDE:
		case COMP::TRIM_FLAT			:
		case COMP::TRIM_FLAT_WIDE		:
		case COMP::TRIM_3006P			:
		case COMP::TRIM_3006W			:
		case COMP::TRIM_3006Y			:
		case COMP::TRIM_3329H			:
		case COMP::TRIM_3329P_DK9_RC	: return 8;
		case COMP::DIODE				:
		case COMP::LED					: return 10;
		case COMP::TO92					:
		case COMP::TO18					:
		case COMP::TO39					:
		case COMP::TO220				: return 11;
		case COMP::DIP					: return 12;
		case COMP::DIP_RECTIFIER		: return 13;
		case COMP::SIP					: return 14;
		case COMP::STRIP_100			:
		case COMP::BLOCK_100			:
		case COMP::BLOCK_200			: return 15;
		case COMP::SWITCH_ST			:
		case COMP::SWITCH_DT			:
		case COMP::SWITCH_ST_DIP		:
		case COMP::SWITCH_BUTTON_4PIN	: return 16;
		case COMP::CRYSTAL				: return 9;
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
		case COMP::RELAY_DIP_4PIN		:
		case COMP::RELAY_DIP_8PIN		: return 17;
		case COMP::FUSE_HOLDER			: return 18;
		case COMP::MARK					: return 100;
		default							: return 1000;	// Unhandled eType
	}
}

static bool IsPlug(const COMP& type)	// true ==> Can plug gap between rows of IC pins
{
	switch( type )
	{
		case COMP::MARK:
		case COMP::PAD:
		case COMP::WIRE:
		case COMP::RESISTOR:
		case COMP::DIODE:
		case COMP::CAP_CERAMIC:	return true;
		default:				return false;
	}
}

static std::map<COMP, std::string>	mapCompTypeToTypeStr;	// Mapping of COMP enum to Type strings
static std::map<COMP, std::string>	mapCompTypeToImportStr;	// Import mapping of COMP enum to strings
static std::list<COMP>				listCompTypes;			// Ordered by appearance in GUI

static void UpdateMaps(const COMP& eType, const std::string& typeStr, const std::string& importStr)
{
	mapCompTypeToTypeStr[eType]		= typeStr;
	mapCompTypeToImportStr[eType]	= importStr;
	listCompTypes.push_back(eType);
}

static void InitMapsCompTypeToStr()
{
	if ( !mapCompTypeToTypeStr.empty() ) return;
	UpdateMaps(COMP::MARK,					"Marker",						"");	// No import string
	UpdateMaps(COMP::PAD,					"Pad",							"PAD");
	UpdateMaps(COMP::WIRE,					"Wire",							"");	// No import string
	UpdateMaps(COMP::RESISTOR,				"Resistor",						"RESISTOR");
	UpdateMaps(COMP::INDUCTOR,				"Inductor",						"INDUCTOR");
	UpdateMaps(COMP::DIODE,					"Diode",						"DIODE");
	UpdateMaps(COMP::LED,					"LED",							"LED");
	UpdateMaps(COMP::CAP_CERAMIC,			"Ceramic",						"CAP_CERAMIC");
	UpdateMaps(COMP::CAP_FILM,				"Film",							"CAP_FILM");
	UpdateMaps(COMP::CAP_FILM_WIDE,			"Film (Wide)",					"CAP_FILM_WIDE");
	UpdateMaps(COMP::CAP_ELECTRO_200,		"Electrolytic 200 mil",			"CAP_ELECTRO_200");
	UpdateMaps(COMP::CAP_ELECTRO_250,		"Electrolytic 250 mil",			"CAP_ELECTRO_250");
	UpdateMaps(COMP::CAP_ELECTRO_300,		"Electrolytic 300 mil",			"CAP_ELECTRO_300");
	UpdateMaps(COMP::CAP_ELECTRO_400,		"Electrolytic 400 mil",			"CAP_ELECTRO_400");
	UpdateMaps(COMP::CAP_ELECTRO_500,		"Electrolytic 500 mil",			"CAP_ELECTRO_500");
	UpdateMaps(COMP::CAP_ELECTRO_600,		"Electrolytic 600 mil",			"CAP_ELECTRO_600");
	UpdateMaps(COMP::CAP_ELECTRO_200_NP,	"Electrolytic NP 200 mil",		"CAP_ELECTRO_200_NP");
	UpdateMaps(COMP::CAP_ELECTRO_250_NP,	"Electrolytic NP 250 mil",		"CAP_ELECTRO_250_NP");
	UpdateMaps(COMP::CAP_ELECTRO_300_NP,	"Electrolytic NP 300 mil",		"CAP_ELECTRO_300_NP");
	UpdateMaps(COMP::CAP_ELECTRO_400_NP,	"Electrolytic NP 400 mil",		"CAP_ELECTRO_400_NP");
	UpdateMaps(COMP::CAP_ELECTRO_500_NP,	"Electrolytic NP 500 mil",		"CAP_ELECTRO_500_NP");
	UpdateMaps(COMP::CAP_ELECTRO_600_NP,	"Electrolytic NP 600 mil",		"CAP_ELECTRO_600_NP");
	UpdateMaps(COMP::TO92,					"TO92",							"TO92");
	UpdateMaps(COMP::TO18,					"TO18",							"TO18");
	UpdateMaps(COMP::TO39,					"TO39",							"TO39");
	UpdateMaps(COMP::TO220,					"TO220",						"TO220");
	UpdateMaps(COMP::TRIM_VERT,				"Vertical",						"TRIM_VERT");
	UpdateMaps(COMP::TRIM_VERT_OFFSET,		"Vertical Offset",				"TRIM_VERT_OFFSET");
	UpdateMaps(COMP::TRIM_VERT_OFFSET_WIDE,	"Vertical Offset (Wide Gap)",	"TRIM_VERT_OFFSET_WIDE");
	UpdateMaps(COMP::TRIM_FLAT,				"Flat",							"TRIM_FLAT");
	UpdateMaps(COMP::TRIM_FLAT_WIDE,		"Flat (Wide Gap)",				"TRIM_FLAT_WIDE");
	UpdateMaps(COMP::TRIM_3006P,			"Bourns 3006P",					"TRIM_3006P");
	UpdateMaps(COMP::TRIM_3006W,			"Bourns 3006W",					"TRIM_3006W");
	UpdateMaps(COMP::TRIM_3006Y,			"Bourns 3006Y",					"TRIM_3006Y");
	UpdateMaps(COMP::TRIM_3329H,			"Bourns 3329H",					"TRIM_3329H");
	UpdateMaps(COMP::TRIM_3329P_DK9_RC,		"Bourns 3329P-DK9-RC",			"TRIM_3329P_DK9_RC");
	UpdateMaps(COMP::SIP,					"SIP",							"SIP");
	UpdateMaps(COMP::DIP,					"DIP",							"DIP");
	UpdateMaps(COMP::DIP_RECTIFIER,			"DIP (Rectifier)",				"DIP_RECTIFIER");
	UpdateMaps(COMP::STRIP_100,				"Pin Strip (100 mil)",			"STRIP_100MIL");
	UpdateMaps(COMP::BLOCK_100,				"Terminal Block (100 mil)",		"BLOCK_100MIL");
	UpdateMaps(COMP::BLOCK_200,				"Terminal Block (200 mil)",		"BLOCK_200MIL");
	UpdateMaps(COMP::SWITCH_ST,				"Single Throw",					"SWITCH_ST");
	UpdateMaps(COMP::SWITCH_DT,				"Double Throw",					"SWITCH_DT");
	UpdateMaps(COMP::SWITCH_ST_DIP,			"Single Throw (DIP)",			"SWITCH_ST_DIP");
	UpdateMaps(COMP::SWITCH_BUTTON_4PIN,	"Button (4 pin)",				"SWITCH_BUTTON_4PIN");
	UpdateMaps(COMP::CRYSTAL,				"Crystal",						"CRYSTAL");
	UpdateMaps(COMP::RELAY_HK19F,			"HK19F",						"RELAY_HK19F");
	UpdateMaps(COMP::RELAY_HJR_4102,		"HJR-4102",						"RELAY_HJR_4102");
	UpdateMaps(COMP::RELAY_FTR_B3C,			"FTR-B3C",						"RELAY_FTR_B3C");
	UpdateMaps(COMP::RELAY_G2R_2,			"G2R-2",						"RELAY_G2R_2");
	UpdateMaps(COMP::RELAY_G2R_2A,			"G2R-2A",						"RELAY_G2R_2A");
	UpdateMaps(COMP::RELAY_G2RK_2,			"G2RK-2",						"RELAY_G2RK_2");
	UpdateMaps(COMP::RELAY_G2RK_2A,			"G2RK-2A",						"RELAY_G2RK_2A");
	UpdateMaps(COMP::RELAY_G3MB_202P,		"G3MB_202P",					"RELAY_G3MB_202P");
	UpdateMaps(COMP::RELAY_JQC_3F_APPROX,	"JQC-3F (approx)",				"RELAY_JQC_3F_APPROX");
	UpdateMaps(COMP::RELAY_S1A050000,		"S1A050000",					"RELAY_S1A050000");
	UpdateMaps(COMP::RELAY_TRCD,			"TRCD",							"RELAY_TRCD");
	UpdateMaps(COMP::RELAY_DIP_4PIN,		"DIP 4-pin",					"RELAY_DIP_4PIN");
	UpdateMaps(COMP::RELAY_DIP_8PIN,		"DIP 8-pin",					"RELAY_DIP_8PIN");
	UpdateMaps(COMP::FUSE_HOLDER,			"Fuse Holder",					"FUSE_HOLDER");
	UpdateMaps(COMP::CUSTOM,				"Custom",						"");	// No import string (user-defined shapes have their own strings)
	UpdateMaps(COMP::TRACKS,				"Tracks",						"");	// No import string
}

static const std::list<COMP>& GetListCompTypes()
{
	InitMapsCompTypeToStr();
	return listCompTypes;
}

static COMP GetTypeFromTypeStr(const std::string& str)
{
	InitMapsCompTypeToStr();
	for (const auto& mapObj : mapCompTypeToTypeStr)
		if ( mapObj.second == str ) return mapObj.first;
	assert(0);	// Either InitMapsCompTypeToStr() needs updating, or str is bad
	return COMP::INVALID;
}

static std::string GetDefaultImportStr(const COMP& eType)
{
	InitMapsCompTypeToStr();
	const auto iter = mapCompTypeToImportStr.find(eType);
	if ( iter != mapCompTypeToImportStr.end() ) return iter->second;
	assert(0);	// Either InitMapsCompTypeToStr() needs updating, or eType is bad
	return "INVALID";
}

static std::string GetDefaultTypeStr(const COMP& eType)
{
	InitMapsCompTypeToStr();
	const auto iter = mapCompTypeToTypeStr.find(eType);
	if ( iter != mapCompTypeToTypeStr.end() ) return iter->second;
	assert(0);	// Either InitMapsCompTypeToStr() needs updating, or eType is bad
	return "INVALID";
}

static std::string GetFamilyStr(const COMP& eType)	// For grouping in the templates
{
	switch( eType )
	{
		case COMP::CAP_CERAMIC			:
		case COMP::CAP_FILM				:
		case COMP::CAP_FILM_WIDE		:
		case COMP::CAP_ELECTRO_200_NP	:
		case COMP::CAP_ELECTRO_250_NP	:
		case COMP::CAP_ELECTRO_300_NP	:
		case COMP::CAP_ELECTRO_400_NP	:
		case COMP::CAP_ELECTRO_500_NP	:
		case COMP::CAP_ELECTRO_600_NP	:
		case COMP::CAP_ELECTRO_200		:
		case COMP::CAP_ELECTRO_250		:
		case COMP::CAP_ELECTRO_300		:
		case COMP::CAP_ELECTRO_400		:
		case COMP::CAP_ELECTRO_500		:
		case COMP::CAP_ELECTRO_600		: return "Capacitor";
		case COMP::TRIM_VERT			:
		case COMP::TRIM_VERT_OFFSET		:
		case COMP::TRIM_VERT_OFFSET_WIDE:
		case COMP::TRIM_FLAT			:
		case COMP::TRIM_FLAT_WIDE		:
		case COMP::TRIM_3006P			:
		case COMP::TRIM_3006W			:
		case COMP::TRIM_3006Y			:
		case COMP::TRIM_3329H			:
		case COMP::TRIM_3329P_DK9_RC	: return "Trim-pot";
		case COMP::STRIP_100			:
		case COMP::BLOCK_100			:
		case COMP::BLOCK_200			: return "Connector";
		case COMP::SWITCH_ST			:
		case COMP::SWITCH_DT			:
		case COMP::SWITCH_ST_DIP		:
		case COMP::SWITCH_BUTTON_4PIN	: return "Switch";
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
		case COMP::RELAY_DIP_4PIN		:
		case COMP::RELAY_DIP_8PIN		: return "Relay";
		default							: return "";
	}
}

static std::string GetDefaultPrefixStr(const COMP& eType)	// Prefix for name on creation
{
	switch( eType )
	{
		case COMP::MARK					: return "Marker";
		case COMP::PAD					: return "Pad";
		case COMP::WIRE					: return "Wire";
		case COMP::RESISTOR				: return "R";
		case COMP::INDUCTOR				: return "L";
		case COMP::DIODE				:
		case COMP::LED					: return "D";
		case COMP::CAP_CERAMIC			:
		case COMP::CAP_FILM				:
		case COMP::CAP_FILM_WIDE		:
		case COMP::CAP_ELECTRO_200_NP	:
		case COMP::CAP_ELECTRO_250_NP	:
		case COMP::CAP_ELECTRO_300_NP	:
		case COMP::CAP_ELECTRO_400_NP	:
		case COMP::CAP_ELECTRO_500_NP	:
		case COMP::CAP_ELECTRO_600_NP	:
		case COMP::CAP_ELECTRO_200		:
		case COMP::CAP_ELECTRO_250		:
		case COMP::CAP_ELECTRO_300		:
		case COMP::CAP_ELECTRO_400		:
		case COMP::CAP_ELECTRO_500		:
		case COMP::CAP_ELECTRO_600		: return "C";
		case COMP::TO92					:
		case COMP::TO18					:
		case COMP::TO39					:
		case COMP::TO220				: return "Q";
		case COMP::TRIM_VERT			:
		case COMP::TRIM_VERT_OFFSET		:
		case COMP::TRIM_VERT_OFFSET_WIDE:
		case COMP::TRIM_FLAT			:
		case COMP::TRIM_FLAT_WIDE		:
		case COMP::TRIM_3006P			:
		case COMP::TRIM_3006W			:
		case COMP::TRIM_3006Y			:
		case COMP::TRIM_3329H			:
		case COMP::TRIM_3329P_DK9_RC	: return "RT";
		case COMP::SIP					:
		case COMP::DIP					:
		case COMP::DIP_RECTIFIER		: return "IC";
		case COMP::STRIP_100			:
		case COMP::BLOCK_100			:
		case COMP::BLOCK_200			: return "Connector";
		case COMP::SWITCH_ST			:
		case COMP::SWITCH_DT			:
		case COMP::SWITCH_ST_DIP		:
		case COMP::SWITCH_BUTTON_4PIN	: return "SW";
		case COMP::CRYSTAL				: return "XT";
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
		case COMP::RELAY_DIP_4PIN		:
		case COMP::RELAY_DIP_8PIN		: return "SW";
		case COMP::FUSE_HOLDER			: return "F";
		case COMP::CUSTOM				: return "";
		default							: return "INVALID";	// Unhandled eType
	}
}

static bool AllowTypeChange(const COMP& eTypeA, const COMP& eTypeB)
{
	const std::string prefixA = GetDefaultPrefixStr(eTypeA);
	const std::string prefixB = GetDefaultPrefixStr(eTypeB);
	if ( prefixA != prefixB ) return false;
	if ( prefixA == std::string("D") )		return true;
	if ( prefixA == std::string("C") )		return true;
	if ( prefixA == std::string("RT") )		return true;
	if ( prefixA == std::string("Strip") )	return true;
	return false;
}

static int GetPinSeparation(const COMP& eType)	// To handle change of component type for LEDs and electro-caps.
{
	switch( eType )
	{
		case COMP::LED					:
		case COMP::CAP_ELECTRO_200_NP	:
		case COMP::CAP_ELECTRO_200		:
		case COMP::CAP_ELECTRO_250_NP	:
		case COMP::CAP_ELECTRO_250		: return 2;
		case COMP::CAP_ELECTRO_300_NP	:
		case COMP::CAP_ELECTRO_300		:
		case COMP::CAP_ELECTRO_400_NP	:
		case COMP::CAP_ELECTRO_400		: return 3;
		case COMP::CAP_ELECTRO_500_NP	:
		case COMP::CAP_ELECTRO_500		:
		case COMP::CAP_ELECTRO_600_NP	:
		case COMP::CAP_ELECTRO_600		: return 4;
		default: return 0;
	}
}

static COMP GetTypeFromImportStr(const std::string& str)	// Just for Import()
{
	InitMapsCompTypeToStr();
	if ( str.empty() ) return COMP::INVALID;
	for (const auto& mapObj : mapCompTypeToImportStr)
		if ( mapObj.second == str ) return mapObj.first;
	return COMP::INVALID;
}

static std::string GetMakeInstructions(const COMP& eType, int& rows, int& cols)
{
	switch( eType )
	{
		case COMP::MARK					: rows = 1; cols = 1;  return ".";
		case COMP::PAD					: rows = 1; cols = 1;  return "1";
		case COMP::WIRE					: rows = 1; cols = 3;  return "1+2";
		case COMP::RESISTOR				: rows = 1; cols = 5;  return "1+++2";
		case COMP::INDUCTOR				: rows = 1; cols = 5;  return "1+++2";
		case COMP::DIODE				: rows = 1; cols = 5;  return "1+++2";
		case COMP::LED					: rows = 1; cols = 2;  return "12";	
		case COMP::CAP_CERAMIC			:
		case COMP::CAP_FILM				: rows = 1; cols = 3;  return "1+2";
		case COMP::CAP_FILM_WIDE		: rows = 3; cols = 3;  return "+++1+2+++";
		case COMP::CAP_ELECTRO_200_NP	:
		case COMP::CAP_ELECTRO_200		: rows = 1; cols = 2;  return "12";
		case COMP::CAP_ELECTRO_250_NP	:
		case COMP::CAP_ELECTRO_250		: rows = 3; cols = 2;  return "++12++";
		case COMP::CAP_ELECTRO_300_NP	:
		case COMP::CAP_ELECTRO_300		: rows = 3; cols = 3;  return "+++1+2+++";
		case COMP::CAP_ELECTRO_400_NP	:
		case COMP::CAP_ELECTRO_400		: rows = 5; cols = 5;  return "..+...+++.+1+2+.+++...+..";
		case COMP::CAP_ELECTRO_500_NP	:
		case COMP::CAP_ELECTRO_500		: rows = 5; cols = 6;  return ".++++..++++.+1++2+.++++..++++.";
		case COMP::CAP_ELECTRO_600_NP	:
		case COMP::CAP_ELECTRO_600		: rows = 7; cols = 6;  return "..++...++++.+++++++1++2+++++++.++++...++..";
		case COMP::TO92					: rows = 1; cols = 3;  return "123";
		case COMP::TO18					: rows = 2; cols = 2;  return "1+23";
		case COMP::TO39					: rows = 3; cols = 3;  return "+++1+3+2+";
		case COMP::TO220				: rows = 1; cols = 3;  return "123";
		case COMP::TRIM_VERT			: rows = 1; cols = 3;  return "123";
		case COMP::TRIM_VERT_OFFSET		: rows = 2; cols = 3;  return "+2+1+3";
		case COMP::TRIM_VERT_OFFSET_WIDE: rows = 3; cols = 3;  return "+2++++1+3";
		case COMP::TRIM_FLAT			: rows = 3; cols = 3;  return "+2++++1+3";
		case COMP::TRIM_FLAT_WIDE		: rows = 4; cols = 3;  return "+2+++++++1+3";
		case COMP::TRIM_3006P			: rows = 2; cols = 8;  return "+1++++3+++++2+++";
		case COMP::TRIM_3006W			: rows = 3; cols = 8;  return "+1++++3+++++++++++++2+++";
		case COMP::TRIM_3006Y			: rows = 2; cols = 8;  return "1++++++3++++2+++";
		case COMP::TRIM_3329H			: rows = 3; cols = 3;  return ".+.12+.3.";
		case COMP::TRIM_3329P_DK9_RC	: rows = 3; cols = 3;  return ".2.1+3.+.";
		case COMP::SIP					: rows = 1; cols = 8;  return "12345678";
		case COMP::DIP					: rows = 4; cols = 4;  return "8765--------1234";
		case COMP::DIP_RECTIFIER		: rows = 4; cols = 4;  return "4++3--------1++2";
		case COMP::STRIP_100			: rows = 1; cols = 2;  return "12";
		case COMP::BLOCK_100			: rows = 3; cols = 2;  return "++12++";
		case COMP::BLOCK_200			: rows = 3; cols = 5;  return ".+++..1+2..+++.";
		case COMP::SWITCH_ST			: rows = 3; cols = 1;  return "1+2";
		case COMP::SWITCH_DT			: rows = 5; cols = 1;  return "1+2+3";
		case COMP::SWITCH_ST_DIP		: rows = 4; cols = 2;  return "12++++34";
		case COMP::SWITCH_BUTTON_4PIN	: rows = 3; cols = 3;  return "1+2+++3+4";
		case COMP::CRYSTAL				: rows = 1; cols = 2;  return "12";
		case COMP::RELAY_HK19F			: rows = 4; cols = 8;  return "8++7+6+5++++++++++++++++1++2+3+4";
		case COMP::RELAY_HJR_4102		: rows = 4; cols = 6;  return "6+++54++++++++++++1+++23";
		case COMP::RELAY_FTR_B3C		: rows = 3; cols = 5;  return "8+765+++++1+234";
		case COMP::RELAY_G2R_2			: rows = 6; cols = 11; return "+++++++++++8+++++7+6+5++++++++++++++++++++++1+++++2+3+4+++++++++++";
		case COMP::RELAY_G2R_2A			: rows = 6; cols = 11; return "+++++++++++6+++++++5+4++++++++++++++++++++++1+++++++2+3+++++++++++";
		case COMP::RELAY_G2RK_2			: rows = 6; cols = 11; return "+++++++++++A+9+++8+7+6++++++++++++++++++++++1+2+++3+4+5+++++++++++";
		case COMP::RELAY_G2RK_2A		: rows = 6; cols = 11; return "+++++++++++8+7+++++6+5++++++++++++++++++++++1+2+++++3+4+++++++++++";
		case COMP::RELAY_G3MB_202P		: rows = 2; cols = 9;  return "+++++++++12+++3++4";
		case COMP::RELAY_JQC_3F_APPROX	: rows = 6; cols = 7;  return "1++++2+++++++++++++34+++++56+++++++7++++8+";
		case COMP::RELAY_S1A050000		: rows = 1; cols = 7;  return "1+2+3+4";
		case COMP::RELAY_TRCD			: rows = 6; cols = 8;  return "1+++2+++++++++++++++++++++++++++++++++++3++++++4";
		case COMP::RELAY_DIP_4PIN		: rows = 4; cols = 7;  return "4+++++3+++++++++++++++1+++2+";
		case COMP::RELAY_DIP_8PIN		: rows = 4; cols = 7;  return "87+++65++++++++++++++12+++34";
		case COMP::FUSE_HOLDER			: rows = 3; cols = 10; return "++++++++++1++++++++2++++++++++";
		case COMP::CUSTOM				: rows = 1; cols = 1;  return ".";
		default:	assert(0);			  rows = 0; cols = 0;  return "";	// Unhandled eType
	}
}

static std::string GetDefaultPinLabel(size_t iPinIndex)
{
	static char buffer[32];
	sprintf(buffer, "%d", (int)(iPinIndex+1));	// Pin numbers on screen start at 1
	return buffer;
}


static int GetDefaultPinAlign(size_t iPinIndex, size_t iNumPins, const COMP& eType)
{
	switch( eType )
	{
		case COMP::DIP:
		case COMP::DIP_RECTIFIER:	return ( 2 * iPinIndex < iNumPins ) ? Qt::AlignLeft : Qt::AlignRight;
		default:					return Qt::AlignHCenter;
	}
}

static bool StopBuildWarnings()
{
	// Following lines are just to avoid pointless build warnings about the previous functions being unused
	static int DUMMY(0);
	AllowTypeChange(COMP::INVALID,COMP::INVALID);
	GetPinSeparation(COMP::INVALID);
	GetTypeFromImportStr("");
	IsPlug(COMP::INVALID);
	GetListCompTypes();
	GetDefaultImportStr(COMP::INVALID);
	GetDefaultTypeStr(COMP::INVALID);
	GetFamilyStr(COMP::INVALID);
	GetTypeFromTypeStr("");
	GetListOrder(COMP::INVALID);
	GetMakeInstructions(COMP::INVALID, DUMMY, DUMMY);
	return true;
}

static int GetDefaultNumPins(const COMP& eType)
{
	assert(true || StopBuildWarnings());
	switch( eType )
	{
		case COMP::MARK					: return 0;
		case COMP::PAD					: return 1;
		case COMP::WIRE					:
		case COMP::RESISTOR				:
		case COMP::INDUCTOR				:
		case COMP::DIODE				:
		case COMP::LED					:
		case COMP::CAP_CERAMIC			:
		case COMP::CAP_FILM				:
		case COMP::CAP_FILM_WIDE		:
		case COMP::CAP_ELECTRO_200_NP	:
		case COMP::CAP_ELECTRO_250_NP	:
		case COMP::CAP_ELECTRO_300_NP	:
		case COMP::CAP_ELECTRO_400_NP	:
		case COMP::CAP_ELECTRO_500_NP	:
		case COMP::CAP_ELECTRO_600_NP	:
		case COMP::CAP_ELECTRO_200		:
		case COMP::CAP_ELECTRO_250		:
		case COMP::CAP_ELECTRO_300		:
		case COMP::CAP_ELECTRO_400		:
		case COMP::CAP_ELECTRO_500		:
		case COMP::CAP_ELECTRO_600		: return 2;
		case COMP::TO92					:
		case COMP::TO18					:
		case COMP::TO39					:
		case COMP::TO220				:
		case COMP::TRIM_VERT			:
		case COMP::TRIM_VERT_OFFSET		:
		case COMP::TRIM_VERT_OFFSET_WIDE:
		case COMP::TRIM_FLAT			:
		case COMP::TRIM_FLAT_WIDE		:
		case COMP::TRIM_3006P			:
		case COMP::TRIM_3006W			:
		case COMP::TRIM_3006Y			:
		case COMP::TRIM_3329H			:
		case COMP::TRIM_3329P_DK9_RC	: return 3;
		case COMP::SIP					:
		case COMP::DIP					: return 8;
		case COMP::DIP_RECTIFIER		: return 4;
		case COMP::STRIP_100			:
		case COMP::BLOCK_100			:
		case COMP::BLOCK_200			: return 2;
		case COMP::SWITCH_ST			: return 2;
		case COMP::SWITCH_DT			: return 3;
		case COMP::SWITCH_ST_DIP		: return 4;
		case COMP::SWITCH_BUTTON_4PIN	: return 4;
		case COMP::CRYSTAL				: return 2;
		case COMP::RELAY_HK19F			: return 8;
		case COMP::RELAY_HJR_4102		: return 6;
		case COMP::RELAY_FTR_B3C		: return 8;
		case COMP::RELAY_G2R_2			: return 8;
		case COMP::RELAY_G2R_2A			: return 6;
		case COMP::RELAY_G2RK_2			: return 10;
		case COMP::RELAY_G2RK_2A		: return 8;
		case COMP::RELAY_G3MB_202P		: return 4;
		case COMP::RELAY_JQC_3F_APPROX	: return 8;
		case COMP::RELAY_S1A050000		: return 4;
		case COMP::RELAY_TRCD			: return 4;
		case COMP::RELAY_DIP_4PIN		: return 4;
		case COMP::RELAY_DIP_8PIN		: return 8;
		case COMP::FUSE_HOLDER			: return 2;
		case COMP::CUSTOM				: return 0;
		default							: return 0;	// Unhandled eType
	}
}
static int GetMinNumPins(const COMP& eType)
{
	switch( eType )
	{
		case COMP::STRIP_100		:
		case COMP::BLOCK_100		:
		case COMP::BLOCK_200		:
		case COMP::SIP				: return 1;
		case COMP::DIP				:
		case COMP::SWITCH_ST		: return 2;
		case COMP::SWITCH_DT		: return 3;
		case COMP::SWITCH_ST_DIP	: return 2;
		default						: return GetDefaultNumPins(eType);
	}
}
static int GetMaxNumPins(const COMP& eType)
{
	switch( eType )
	{
		case COMP::STRIP_100		:
		case COMP::BLOCK_100		:
		case COMP::BLOCK_200		:
		case COMP::SIP				:
		case COMP::DIP				:
		case COMP::SWITCH_ST		:
		case COMP::SWITCH_DT		:
		case COMP::SWITCH_ST_DIP	: return 255;	// We're limited to (0 <= pinIndex <= 254)
		default						: return GetDefaultNumPins(eType);
	}
}
static int GetStretchIncrement(const COMP& eType)	// For stretchable components
{
	switch( eType )
	{
		case COMP::SWITCH_ST		:
		case COMP::SWITCH_DT		:			// Growing a bulk switch increases its footprint by 2 columns instead of 1
		case COMP::BLOCK_200		: return 2;	// As does a growing a strip with pitch of 2 squares
		default						: return 1;
	}
}
static int GetMinLength(const COMP& eType)	// For stretchable components
{
	switch( eType )
	{
		case COMP::WIRE				:
		case COMP::DIODE			:
		case COMP::RESISTOR			:
		case COMP::INDUCTOR			:
		case COMP::CAP_CERAMIC		:
		case COMP::CAP_FILM			:
		case COMP::CAP_FILM_WIDE	: return 2;
		case COMP::SIP				: return GetMinNumPins(eType);
		case COMP::DIP				:
		case COMP::STRIP_100		: return GetMinNumPins(eType);
		case COMP::BLOCK_100		: return GetMinNumPins(eType);
		case COMP::BLOCK_200		: return GetMinNumPins(eType) * 2 + 1;
		case COMP::SWITCH_ST		: return GetMinNumPins(eType) / 2;
		case COMP::SWITCH_DT		: return GetMinNumPins(eType) / 3;
		case COMP::SWITCH_ST_DIP	: return GetMinNumPins(eType) / 2;
		default:	assert(0);		  return 1;	// Non-stretchable component
	}
}
static int GetMaxLength(const COMP& eType)	// For stretchable components
{
	switch( eType )
	{
		case COMP::WIRE				: return INT_MAX;
		case COMP::DIODE			:
		case COMP::RESISTOR			:
		case COMP::INDUCTOR			:
		case COMP::CAP_CERAMIC		:
		case COMP::CAP_FILM			:
		case COMP::CAP_FILM_WIDE	: return 16;
		case COMP::SIP				: return GetMaxNumPins(eType);
		case COMP::DIP				: return GetMaxNumPins(eType) / 2;
		case COMP::STRIP_100		: return GetMaxNumPins(eType);
		case COMP::BLOCK_100		: return GetMaxNumPins(eType);
		case COMP::BLOCK_200		: return GetMaxNumPins(eType) * 2 + 1;
		case COMP::SWITCH_ST		: return GetMaxNumPins(eType) - 1;
		case COMP::SWITCH_DT		: return 2 * GetMaxNumPins(eType) / 3 - 1;
		case COMP::SWITCH_ST_DIP	: return GetMaxNumPins(eType) / 2;
		default:	assert(0);		  return 1;	// Non-stretchable component
	}
}
