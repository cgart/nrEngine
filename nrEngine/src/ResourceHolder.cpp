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
#include "ResourceHolder.h"
#include "Exception.h"
#include "ResourceSystem.h"
#include "Log.h"

namespace nrEngine{
	
	//----------------------------------------------------------------------------------
	ResourceHolder::~ResourceHolder()
	{
		mResource = NULL;
	}
		
	//----------------------------------------------------------------------------------
	ResourceHolder::ResourceHolder(IResource* res, IResource* empty):
			mResource(res), mEmptyResource(empty), countAccess(0)
	{
		NR_ASSERT(res != NULL && empty != NULL);
			
		// empty the lock stack
		for (int32 i=0; i < NR_RESOURCE_LOCK_STACK; i++) mLockStack[i] = false;
		mLockStackTop = 0;
	
		// empty the lock stack
		for (int32 i=0; i < NR_RESOURCE_LOCK_STACK; i++) mEmptyLockStack[i] = false;
		mEmptyLockStackTop = 0;
	}
		
	//----------------------------------------------------------------------------------
	bool ResourceHolder::lockResource()
	{
		// check if resource is already locked
		if (isEmptyLocked() && mResource)
			NR_Log(Log::LOG_ENGINE, Log::LL_WARNING, "You are trying to lock real resource while empty resource is locked. Empty stay locked for %s", mResource->getResourceName().c_str());
		else if (isEmptyLocked() && !mResource)
			NR_Log(Log::LOG_ENGINE, Log::LL_WARNING, "You are trying to lock real resource while empty resource is locked. Empty stay locked");
	
		// check whenever we've got the maximum, so do not lock
		if (mLockStackTop >= NR_RESOURCE_LOCK_STACK){
			if (mResource){
				NR_Log(Log::LOG_ENGINE, Log::LL_DEBUG, 
					"Can not lock %s anymore. Lock state stack is full!", mResource->getResourceName().c_str());
			}else{
				NR_Log(Log::LOG_ENGINE, Log::LL_DEBUG, 
					"Can not lock anymore. Lock state stack is full!");
			}
			
			return false;
		}else{		
			// lock it
			mLockStack[mLockStackTop++] = true;
		}
		
		return true;
	}	
		
	//----------------------------------------------------------------------------------
	void ResourceHolder::unlockResource()
	{
		// check whenever we've got right top position
		if (mLockStackTop > 0){
			// unlock it
			mLockStack[--mLockStackTop] = false;
		}
	}

	//----------------------------------------------------------------------------------
	bool ResourceHolder::lockEmpty()
	{
		// check if resource is already locked
		if (isLocked() && mResource)
			NR_Log(Log::LOG_ENGINE, Log::LL_WARNING, "You are trying to lock empty resource while real resource is locked. Empty will be used for %s", mResource->getResourceName().c_str());
		else if (isLocked() && !mResource)
			NR_Log(Log::LOG_ENGINE, Log::LL_WARNING, "You are trying to lock empty resource while real resource is locked. Empty will be used");
		 
		// check whenever we've got the maximum, so do not lock
		if (mEmptyLockStackTop >= NR_RESOURCE_LOCK_STACK)
		{
			if (mResource)
			{
				NR_Log(Log::LOG_ENGINE, Log::LL_DEBUG, 
					"Can not lock %s anymore. Lock state stack is full!", mResource->getResourceName().c_str());
			}else{
				NR_Log(Log::LOG_ENGINE, Log::LL_DEBUG, 
					"Can not lock anymore. Lock state stack is full!");
			}
			return false;
		}else{
			// lock it
			mEmptyLockStack[mEmptyLockStackTop++] = true;
		}
		
		return true;
	}	
		
	//----------------------------------------------------------------------------------
	void ResourceHolder::unlockEmpty()
	{
		// check whenever we've got right top position
		if (mEmptyLockStackTop > 0){
			// unlock it
			mEmptyLockStack[--mEmptyLockStackTop] = false;
		}
	}

	//----------------------------------------------------------------------------------
	void ResourceHolder::resetResource(IResource* res)
	{
		mResource = (res);
	}
	
	
	//----------------------------------------------------------------------------------
	void ResourceHolder::setEmptyResource(IResource* res)
	{
		mEmptyResource = (res);
	}
		
	//----------------------------------------------------------------------------------
	IResource* ResourceHolder::getEmpty()
	{
		return mEmptyResource;
	}
	
		
	//----------------------------------------------------------------------------------
	IResource* ResourceHolder::getResource()
	{
		NR_ASSERT(getEmpty() != NULL && "Empty resource must be defined");
		
		// check if empty is locked, then return empty resource
		if (isEmptyLocked()) return getEmpty();
		
		// get resource only if it is exists and loaded or if it exists and locked
		if (mResource!=NULL)
		{
			// check if we have locked to an empty resource
			if (mResource->isResourceLoaded() || isLocked())
			{
				touchResource();
				return mResource;
			}
		}

		return getEmpty();
	}

	//----------------------------------------------------------------------------------
	void ResourceHolder::touchResource()
	{
		// check if resource need to be reloaded
		if (mResource->isResourceDirty()) mResource->reload();
		
		// count up the access count variable
		countAccess ++;
	}

};

