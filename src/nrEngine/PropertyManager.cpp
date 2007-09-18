/***************************************************************************
 *                                                                         *
 *   (c) Art Tevs, MPI Informatik Saarbruecken                             *
 *       mailto: <tevs@mpi-sb.mpg.de>                                      *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

//----------------------------------------------------------------------------------
// Includes
//----------------------------------------------------------------------------------
#include <nrEngine/PropertyManager.h>
#include <nrEngine/Exception.h>

namespace nrEngine {

	//----------------------------------------------------------------------------------
	PropertyManager::PropertyManager()
	{

	}

	//----------------------------------------------------------------------------------
	PropertyManager::~PropertyManager()
	{
		// clean the property map
		mPropertyMap.clear();
	}
		
	//----------------------------------------------------------------------------------
	void PropertyManager::createGroup(const std::string& name)
	{
		mPropertyMap.insert(std::pair<std::string, PropertyList>(name, PropertyList()));
	}

	//----------------------------------------------------------------------------------
	const PropertyList& PropertyManager::getPropertyList(const std::string& group)
	{
		return mPropertyMap[group];
	}

	//----------------------------------------------------------------------------------
	Property& PropertyManager::getProperty(const std::string& name, const std::string& group)
	{
		return mPropertyMap[group][name];
	}

	//----------------------------------------------------------------------------------
	Property& PropertyManager::getPropertyByFullname(const std::string& fullname)
	{
		// iterate through all groups
		PropertyMap::iterator it = mPropertyMap.begin();
		for (; it != mPropertyMap.end(); it++)
		{
			// search for such an element
			PropertyList::iterator jt = it->second.begin();
			for (; jt != it->second.end(); jt++)
				if (jt->getFullname() == fullname)
				{
					return *jt;
				}
		}

		// we have not found any such element, so create one
		NR_Log(Log::LOG_ENGINE, Log::LL_WARNING, "PropertyManager: Property with fullname '%s' is not registered, so create it in default group", fullname.c_str());
		return getProperty(fullname, "");
	}

	//----------------------------------------------------------------------------------
	bool PropertyManager::isPropertyRegistered(const std::string& name, const std::string& group) const
	{
		// iterate through all groups
		PropertyMap::const_iterator it = mPropertyMap.begin();
		for (; it != mPropertyMap.end(); it++)
		{
			// if we are checking in a certain group
			if (it->first == group)
			{
				return it->second.exists(name);
			}
		}
		return false;
	}

	//----------------------------------------------------------------------------------
	bool PropertyManager::isPropertyRegisteredByFullname(const std::string& fullname) const
	{
		// extract the group name
		return isPropertyRegistered(getPropertyName(fullname), getGroupName(fullname));
	}

	//----------------------------------------------------------------------------------
	std::string PropertyManager::getGroupName(const std::string& fullname) const
	{
		// find the first occurence of . character
		uint32 pos = fullname.find('.');

		if (pos == std::string::npos || pos == 0) return std::string();

		return fullname.substr(0, pos - 1);
	}

	//----------------------------------------------------------------------------------
	std::string PropertyManager::getPropertyName(const std::string& fullname) const
	{
		// find the first occurence of . character
		uint32 pos = fullname.find('.');

		if (pos == std::string::npos) return fullname;
		if (pos == 0) pos = -1;

		return fullname.substr(pos + 1);
	}
	
	//----------------------------------------------------------------------------------
	void PropertyManager::set(const Property& property, const std::string& name, const std::string& group)
	{
		Property& p = getProperty(name, group);
		p.copyDataOnly(property);
		p.mName = name;
		p.mFullname = group + std::string(".") + name;		
	}
	
	//----------------------------------------------------------------------------------
	void PropertyManager::setByFullname(const Property& property, const std::string& fullname)
	{
		// get property by fullname
		Property& p = getPropertyByFullname(fullname);
		p.copyDataOnly(property);
		p.mFullname = fullname;
		if (p.mName.length() == 0) p.mName = fullname;
	}


	//----------------------------------------------------------------------------------
	void PropertyManager::set(const boost::any& value, const std::string& name, const std::string& group)
	{
		// get property
		Property& p = getProperty(name, group);

		// set new value
		p.mValue = value;
	}

	//----------------------------------------------------------------------------------
	void PropertyManager::setByFullname(const boost::any& value, const std::string& fullname)
	{
		// get property
		Property& p = getPropertyByFullname(fullname);

		// set new value
		p.mValue = value;
	}

};



