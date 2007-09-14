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
#include <nrEngine/Resource.h>
#include <nrEngine/Exception.h>
#include <nrEngine/Log.h>

namespace nrEngine {

	//----------------------------------------------------------------------------------
	IResource::IResource(const std::string& resType)
	{
		// setup default data
		mResIsLoaded = false;
		mResHandle = 0;
		mResIsEmpty = false;
		mResDataSize = sizeof(*this);
		mResIsDirty = false;
		setResourceType(resType);
	}

	//----------------------------------------------------------------------------------
	IResource::~IResource()
	{
	
	}

	//----------------------------------------------------------------------------------
	Result IResource::unload()
	{
		// check if resource is loaded
		if (isResourceLoaded())
		{
			// unload resource
			Result ret = unloadResource();

			// if ok, then inform resource manager, that we are unloaded now
			if (ret == OK)
				Engine::sResourceManager()->notifyUnloaded(this);
			else
				return ret;
		}
		return OK;
	}
	
	//----------------------------------------------------------------------------------
	Result IResource::reload(PropertyList* params)
	{
		// if resource is loaded, then unload it first
		if (isResourceLoaded())
		{
			Result ret = unload();
			if (ret != OK) return ret;
		}
			
		// check if resource is loaded
		if (!isResourceLoaded())
		{
			// unload resource
			Result ret = reloadResource(params);

			// if ok, then inform resource manager, that we are reloaded now
			if (ret == OK){
				Engine::sResourceManager()->notifyLoaded(this);
				mResIsDirty = false;
			}else
				return ret;
		}
		return OK;
	}
	
	//----------------------------------------------------------------------------------
	Result IResource::remove()
	{
		// unload resource first
		unload();

		// remove resource in the loader
		return mResLoader->remove(getSharedPtrFromThis());
	}

	//----------------------------------------------------------------------------------
	void IResource::addResourceFilename(const std::string& filename)
	{
		// first check if such a filename already exists
		std::list<std::string>::iterator it = std::find(mResFileNames.begin(), mResFileNames.end(), filename);
		if (it == mResFileNames.end())
			mResFileNames.push_back(filename);
	}
	
	//----------------------------------------------------------------------------------
	void IResource::addResourceFilename(const std::list<std::string>& flist)
	{
		std::list<std::string>::const_iterator it = flist.begin();
		for (; it != flist.end(); it++)
			addResourceFilename(*it);
	}

	//----------------------------------------------------------------------------------
	void IResource::setResourceFilename(const std::list<std::string>& flist)
	{
		mResFileNames = flist;
	}

};

