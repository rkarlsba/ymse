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

#include "Common.h"

struct StringHelper
{
	static bool IsEmptyStr(const std::string& in)
	{
		std::string s = in;
		s.erase(std::remove_if(s.begin(), s.end(), ::isspace), s.end());
		return s.empty();
	}
	static bool HasSpaces(const std::string& in)
	{
		std::string s = in;
		s.erase(std::remove_if(s.begin(), s.end(), ::isspace), s.end());
		return s.size() < in.size();
	}
	// Helper function for reading a line from a file during the netlist import.
	// Handles differing line-ending conventions between the file and the operating system.
	static std::istream& getline_safe(std::istream& inStream, std::string& str)
	{
		std::istream::sentry	se(inStream, true);	// Sentry object to protect and manage the stream buffer
		std::streambuf*			sb = inStream.rdbuf();
		str.clear();
		while(true)
		{
			const int c = sb->sbumpc();
			switch( c )
			{
				case '\n':
					return inStream;
				case '\r':
					if ( sb->sgetc() == '\n' )
						sb->sbumpc();
					return inStream;
				case EOF:
					if ( str.empty() )	// In case last line has no ending
						inStream.setstate(std::ios::eofbit);
					return inStream;
				default:
					str += (char)c;
			}
		}
	}
	std::vector<std::string> strList;

	static void GetSubStrings(const std::string& in, std::vector<std::string>& strList)
	{
		strList.clear();
		std::istringstream iss(in);
		for (std::string s; iss >> s; )
			strList.push_back(s);
	}

	StringHelper() { assert( true || PreventBuildWarnings() ); }
private:
	bool PreventBuildWarnings() const
	{
		std::ifstream	inStream;
		std::string		str;
		StringHelper::IsEmptyStr("");
		StringHelper::HasSpaces("");
		StringHelper::getline_safe(inStream, str);
		return true;
	}
};

