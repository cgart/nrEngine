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
#include "Property.h"
#include "Exception.h"
#include "Log.h"

namespace nrEngine {

	//----------------------------------------------------------------------------------
	Property::Property() :mUserData(NULL)
	{

	}

	//----------------------------------------------------------------------------------
	Property::Property(const std::string& name) : mName(name), mFullName(name), mUserData(NULL)
	{

	}

	//----------------------------------------------------------------------------------
	Property::Property (const std::string& name, const boost::any& value) : mName(name), mFullName(name), mValue(value), mUserData(NULL)
	{

	}

	//----------------------------------------------------------------------------------
	Property::Property(const Property& p):mName(p.getName()), mFullName(p.getFullName()), mValue(p.getValue()), mUserData(p.mUserData)
	{

	}

	//----------------------------------------------------------------------------------
	Property& Property::operator=(const boost::any& val)
	{
		mValue = val;
		return *this;
	}

	//----------------------------------------------------------------------------------
	bool Property::operator==(const Property& p)
	{
		return (p.getFullName() == mFullName && p.getValue().type() == mValue.type());
	}

	
	//----------------------------------------------------------------------------------
	PropertyList& PropertyList::operator, (const Property& p)
	{
		push_back(p);
		return *this;
	}

	//----------------------------------------------------------------------------------
	PropertyList& PropertyList::operator, ( Property& p)
	{
		p = front();
		pop_front();
		return *this;
	}

	//----------------------------------------------------------------------------------
	bool PropertyList::exists(const std::string& name) const
	{
		// search for the key
		const_iterator it = begin();
		for (; it != end(); it++)
			if (it->getName() == name) return true;
		return false;
	}

	//----------------------------------------------------------------------------------
	/*const Property& PropertyList::operator[](const std::string& name)
	{
		// search for the key
		for (const_iterator it = begin(); it != end(); it++) if (it->getName() == name) return *it;

		// we could not find it, so create it and return reference
		push_back(Property(name));
		for (const_iterator it = begin(); it != end(); it++) if (it->getName() == name) return *it;
		
		// ok it should be there, so exception
		NR_EXCEPT(UNKNOWN_ERROR, "This is a BUG! Contact Author!", "PropertyList::operator[]");
	}*/
	
	//----------------------------------------------------------------------------------
	Property& PropertyList::operator[](const std::string& name)
	{
		// search for the key
		for (iterator it = begin(); it != end(); it++) if (it->getName() == name) return *it;

		// we could not find it, so create it and return reference
		Property p(name);
		push_back(p);

		// some debug info
		NR_Log(Log::LOG_ENGINE, Log::LL_DEBUG, "Property: Initialize new property '%s'", name.c_str());

		return back();
	}

	//----------------------------------------------------------------------------------
	const Property& PropertyList::operator[](const std::string& name) const
	{
		// search for the key
		for (const_iterator it = begin(); it != end(); it++) if (it->getName() == name) return *it;

		// we could not find it, so error, because we can not add new elements to const
		NR_EXCEPT(UNKNOWN_ERROR, "const PropertyList can not add new elements", "PropertyList::operator[]");
	}

};

