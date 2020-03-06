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

// A helper for reading/writing data to a file in binary form

#include <QFile>
#include <QDataStream>
#include "VrtVersion.h"
#include "StringHelper.h"

struct DataStream
{
	enum TYPE { READ = 0, WRITE = 1 };
	DataStream(TYPE type) : m_type(type), m_version(0), m_bOK(true) {}
	~DataStream() {}
	bool Open(const char* fileName)
	{
		m_file.setFileName( QString(fileName) );
		m_bOK = m_file.open(m_type == READ ? QIODevice::ReadOnly : QIODevice::WriteOnly);
		if ( m_bOK )
		{
			m_ios.setDevice(&m_file);
			m_ios.setVersion(QDataStream::Qt_4_5);
		}
		return m_bOK;
	}
	void Close() { m_file.close(); }

	void Load(bool& o)					{				m_ios >> o; }
	void Load(char& o)					{ qint8		t;	m_ios >> t; o = (char)			t; }
	void Load(unsigned char& o)			{ quint8	t;	m_ios >> t; o = (unsigned char)	t; }
	void Load(short& o)					{ qint16	t;	m_ios >> t; o = (short)			t; }
	void Load(unsigned short& o)		{ quint16	t;	m_ios >> t; o = (unsigned short)t; }
	void Load(int& o)					{ qint32	t;	m_ios >> t; o = (int)			t; }
	void Load(unsigned int& o)			{ quint32	t;	m_ios >> t; o = (unsigned int)	t; }
	void Load(float& o)					{				m_ios >> o; }
	void Load(double& o)				{				m_ios >> o; }
	void Load(std::string& o)			{ QString	t;	m_ios >> t; o = t.toStdString();}

	void Save(const bool& o)			{ m_ios << o; }
	void Save(const char& o)			{ m_ios << (qint8)	o; }
	void Save(const unsigned char& o)	{ m_ios << (quint8)	o; }
	void Save(const short& o)			{ m_ios << (qint16)	o; }
	void Save(const unsigned short& o)	{ m_ios << (quint16)o; }
	void Save(const int& o)				{ m_ios << (qint32)	o; }
	void Save(const unsigned int& o)	{ m_ios << (quint32)o; }
	void Save(const float& o)			{ m_ios << o; }
	void Save(const double& o)			{ m_ios << o; }
	void Save(const std::string& o)		{ m_ios << QString::fromStdString(o); }

	const int&	GetVersion() const		{ return m_version; }
	void		SetVersion(int i)		{ m_version = i; }
	const bool& GetOK() const			{ return m_bOK; }
	void		SetOK(bool b)			{ m_bOK = b; }
private:
	TYPE			m_type;
	QFile			m_file;
	QDataStream		m_ios;
	int				m_version;	// Used to represent VRT version being loaded so the format can evolve
	bool			m_bOK;		// To flag problems
};

// The interface definition for load and saving data to file
struct Persist
{
	virtual void Load(DataStream&) = 0;
	virtual void Save(DataStream&) = 0;
};

