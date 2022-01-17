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

#include "Template.h"

// Manager class to handle component templates

class TemplateManager : public Persist
{
public:
	TemplateManager()	{}
	~TemplateManager()	{ m_listGeneric.clear(); m_listUser.clear(); }
	TemplateManager(const TemplateManager& o) { *this = o; }
	TemplateManager& operator=(const TemplateManager& o)
	{
		for (int i = 0; i < 2; i++ )
		{
			const bool bGeneric = (i == 0);
			auto& listThis	= bGeneric ? m_listGeneric : m_listUser;
			auto& listOther	= bGeneric ? o.m_listGeneric : o.m_listUser;

			listThis.clear();
			for (auto& r : listOther)
				listThis.push_back( Template(r) );
		}
		return *this;
	}
	const std::string& GetPathStr() const { return m_pathStr; }
	void SetPathStr(const std::string& str)	{ m_pathStr = str; }
	size_t GetSize(bool bGeneric) const
	{
		return bGeneric ? m_listGeneric.size() : m_listUser.size();
	}
	const Component& GetNth(bool bGeneric, size_t N) const
	{
		assert( N < GetSize(bGeneric) );
		auto iter = bGeneric ? m_listGeneric.begin() : m_listUser.begin();
		for (size_t i = 0; i < N; i++) iter++;
		return *iter;
	}
	bool GetFromImportStr(const std::string& importStr, Component& out) const
	{
		for (auto& o : m_listUser)
			if ( o.GetType() == COMP::CUSTOM && o.GetImportStr() == importStr ) { out = o; return true; }
		return false;
	}
	void AddDefaults()
	{
		std::string nameStr(""), valueStr("");
		for (auto& eType : GetListCompTypes())
		{
			if ( eType == COMP::TRACKS ) continue;	// Not a real component
			if ( eType == COMP::CUSTOM ) continue;	// Don't show custom components in the left pane
			const size_t numPins = GetDefaultNumPins(eType);
			std::vector<int> nodeList;
			nodeList.resize(numPins, BAD_NODEID);
			Component comp(nameStr, valueStr, eType, nodeList);

			comp.SetId(BAD_COMPID);	// This indicates a component template
			Add(true, comp);	// true ==> generic
		}
	}
	bool Add(bool bGeneric, const Component& comp, std::string* pErrorStr = nullptr)
	{
		Template entry;
		if ( !entry.MakeTemplate(comp) )
			return false;

		auto& lst = bGeneric ? m_listGeneric : m_listUser;

		// Custom components need unique strings
		if ( comp.GetType() == COMP::CUSTOM )
		{
			assert( !bGeneric );
			for (auto& o : lst)
			{
				if ( comp.GetTypeStr() == o.GetTypeStr() && comp.GetValueStr() == o.GetValueStr() )
				{
					if ( pErrorStr ) *pErrorStr = "The template (Type = " + o.GetTypeStr() + ") "
												+ "(Value = " + o.GetValueStr() + ") already exists";
					return false;
				}
				else if ( !comp.GetImportStr().empty() && comp.GetImportStr() == o.GetImportStr() )
				{
					if ( pErrorStr ) *pErrorStr = "The template (Type = " + o.GetTypeStr() + ") "
												+ "(Value = " + o.GetValueStr() + ") "
												+ "already has the Import string " + o.GetImportStr();
					return false;
				}
			}
		}

		auto iter = lst.begin();
		bool bOK = iter == lst.end() || entry.IsLessThan(*iter, bGeneric);
		if ( bOK ) lst.insert(iter, entry);

		while( iter != lst.end() && !bOK )
		{
			Template& prev = *iter;
			++iter;
			bOK = prev.IsLessThan(entry, bGeneric)
			   && ( iter == lst.end() || entry.IsLessThan(*iter, bGeneric) );
			if ( bOK ) lst.insert(iter, entry);
		}
		return bOK;
	}
	bool Remove(const Component& comp)
	{
		Template entry;
		if ( !entry.MakeTemplate(comp) ) return false;

		const auto iter = std::find(m_listUser.begin(), m_listUser.end(), entry);
		const bool bOK = ( iter != m_listUser.end() );
		if ( bOK ) m_listUser.erase(iter);	// Erase entry if it exists
		return bOK;
	}
	// Persist interface functions
	virtual void Load(DataStream& inStream) override
	{
		m_listUser.clear();
		unsigned int iSize(0);
		inStream.Load(iSize);
		for (unsigned int i = 0; i < iSize; i++)
		{
			Template entry;
			entry.Load(inStream);
			m_listUser.push_back(entry);
		}
	}
	virtual void Save(DataStream& outStream) override
	{
		const unsigned int iSize = static_cast<unsigned int>( m_listUser.size() );
		outStream.Save(iSize);
		for (auto& o : m_listUser) o.Save(outStream);
	}
private:
	std::string			m_pathStr;		// Path to the "templates" folder
	std::list<Template>	m_listGeneric;	// List of generic components
	std::list<Template>	m_listUser;		// List of template components
};
