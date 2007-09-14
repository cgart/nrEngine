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
#include "EventFactory.h"
#include "EventManager.h"

namespace nrEngine{

	//------------------------------------------------------------------------
	EventFactory::EventFactory(const std::string& name): mName(name)
	{
	}

	//------------------------------------------------------------------------
	EventFactory::~EventFactory(){
		// say the event manager, that it must be removed
		//if (EventManager::isValid()){
		//	EventManager::GetSingleton().removeFactory(mName);
		//}
	}

	//------------------------------------------------------------------------
	bool EventFactory::isSupported(const std::string& eventType) const
	{
		// iterate through the list and check
		NameList::const_iterator it = mSupportedTypes.begin();
		for (; it != mSupportedTypes.end(); it++)
			if ((*it) == eventType) return true;

		return false;
	}

}; // end namespace

