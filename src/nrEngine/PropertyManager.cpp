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
	Property& PropertyManager::getPropertyByFullName(const std::string& fullname)
	{
		// iterate through all groups
		PropertyMap::iterator it = mPropertyMap.begin();
		for (; it != mPropertyMap.end(); it++)
		{
			// search for such an element
			PropertyList::iterator jt = it->second.begin();
			for (; jt != it->second.end(); jt++)
				if (jt->getFullName() == fullname)
				{
					return *jt;
				}
		}

		// we have not found any such element, so create one
		NR_Log(Log::LOG_ENGINE, Log::LL_WARNING, "PropertyManager: Property with fullname '%s' is not registered, so create it in default group", fullname.c_str());
		return getProperty(fullname, "");		
	}
	
	//----------------------------------------------------------------------------------
	void PropertyManager::set(const Property& property, const std::string& name, const std::string& group)
	{
		Property& p = getProperty(name, group);
		p.copyDataOnly(property);
		p.mName = name;
		p.mFullName = group + std::string(".") + name;		
	}
	
	//----------------------------------------------------------------------------------
	void PropertyManager::setByFullName(const Property& property, const std::string& fullname)
	{
		// get property by fullname
		Property& p = getPropertyByFullName(fullname);
		p.copyDataOnly(property);
		p.mFullName = fullname;
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
	void PropertyManager::setByFullName(const boost::any& value, const std::string& fullname)
	{
		// get property
		Property& p = getPropertyByFullName(fullname);

		// set new value
		p.mValue = value;
	}

};



