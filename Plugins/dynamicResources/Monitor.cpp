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

#include "Monitor.h"
using namespace nrEngine;

//if we are using linux system, then include inotify interface


//----------------------------------------------------------------------------------
Monitor::Monitor(Engine* root) : mRoot(root)
{
	setTaskName("DynamicResourceMonitor");
}


//----------------------------------------------------------------------------------
Monitor::~Monitor()
{

}

//----------------------------------------------------------------------------------
Result Monitor::onStartTask()
{
	Result res = UNKNOWN_ERROR;
	
	// use inotify if supported
	#ifdef USE_INOTIFY
		res = initInotify();
	#endif

	init();
	
	// nothing supported, so return an error
	return res;
}

//----------------------------------------------------------------------------------
Result Monitor::stopTask()
{
	// if we support inotify, then do
	#ifdef USE_INOTIFY
		mInotify->Close();
		NR_Log(Log::LOG_PLUGIN, "dynamicResources: Close inotify subsystem");
	#endif
	
	// ok
	return OK;
}


//----------------------------------------------------------------------------------
#ifdef USE_INOTIFY
Result Monitor::initInotify()
{
	// if we support inotify, then do
	NR_Log(Log::LOG_PLUGIN, "dynamicResources: Use inotify interface for file monitoring");
	
	try
	{
		mInotify.reset(new Inotify());
		mInotify->SetNonBlock(true);
	}catch(InotifyException& e)
	{
		NR_Log(Log::LOG_PLUGIN, Log::LL_ERROR, "dynamicResources: There was an error by initilizing the inotify interafce");
		NR_Log(Log::LOG_PLUGIN, Log::LL_ERROR, "dynamicResources: %s", e.GetMessage().c_str());
		return UNKNOWN_ERROR;
	}
	
	return OK;
}
#endif

//----------------------------------------------------------------------------------
void Monitor::init()
{
	// now scan through all resources, which are already loaded
	const ResourceManager::ResourceGroupMap& res = mRoot->sResourceManager()->getResourceMap();
	ResourceManager::ResourceGroupMap::const_iterator it = res.begin();
	for (; it != res.end(); it++)
	{
		// for each group do
		std::list<ResourceHandle>::const_iterator jt = it->second.begin();
		for (; jt != it->second.end(); jt++)
		{
			// get resource according to the handle
			IResourcePtr pr = mRoot->sResourceManager()->getByHandle(*jt);
			
			// resource is valid, so do
			if (pr.valid())
			{
				// now get the file name associated with the resource 
				pr.lockResource();
					const std::list<std::string>& files = pr.getBase()->getResourceFilenameList();
					std::list<std::string>::const_iterator kt = files.begin();
					
					// for each filename add a monitor 
					for (; kt != files.end(); kt ++)
					{
						addMonitor(pr, *kt);
					}
				pr.unlockResource();
			}else{
				NR_Log(Log::LOG_PLUGIN, Log::LL_WARNING, "dynamicResources: Resource %s seems to be NULL", pr.getBase()->getResourceName().c_str());
			}
			
		}
	}
}

#ifdef USE_INOTIFY
//----------------------------------------------------------------------------------
void Monitor::addMonitor(IResourcePtr res, const std::string& file)
{
	// do only add a watcher if inotify already initialized
	if (!mInotify || !res.valid() || file.length() < 1) return;
	
	// we monitor only non-empty resources
	res.lockResource();
	{
		// create a watch descriptor
		try
		{
			// first check if such a watcher already exists
			InotifyWatch* watch = mInotify->FindWatch(file);
			
			if (watch == NULL)
			{
				watch = new InotifyWatch(file, IN_MODIFY);
				mInotify->Add(watch);
			}
			
			// add new watcher
			NR_Log(Log::LOG_PLUGIN, Log::LL_DEBUG, "dynamicResources: Monitor %s --> %s", res.getBase()->getResourceName().c_str(), file.c_str());
			
			// add the watcher into the map
			WatchData data;
			data.resource = res;
			data.watcher = watch;
			data.resourceName = res.getBase()->getResourceName();
			mWatchMap[watch->GetDescriptor()].push_back(data);
			
		}catch(InotifyException& e)
		{
			NR_Log(Log::LOG_PLUGIN, Log::LL_ERROR, "dynamicResources: Cannot add a monitor %s --> %s", res.getBase()->getResourceName().c_str(), file.c_str());
			NR_Log(Log::LOG_PLUGIN, Log::LL_ERROR, "dynamicResources: %s", e.GetMessage().c_str());
			return;
		}
		
	}
	res.unlockResource();
}

#else

//----------------------------------------------------------------------------------
void Monitor::addMonitor(IResourcePtr res, const std::string& file)
{
	NR_Log(Log::LOG_PLUGIN, Log::LL_WARNING, "dynamicResources: Not valid monitor interface used to monitor %s --> %s", res.getBase()->getResourceName().c_str(), file.c_str());
}

#endif



#ifdef USE_INOTIFY
//----------------------------------------------------------------------------------
Result Monitor::update()
{
	if (!mInotify) return OK;
	
	//printf("update\n");
	// wait for inotify events
	try{
		mInotify->WaitForEvents(true);
	}catch(InotifyException& e)
	{
		NR_Log(Log::LOG_PLUGIN, Log::LL_ERROR, "dynamicResources: INotify Monitor cannot get events!");
		NR_Log(Log::LOG_PLUGIN, Log::LL_ERROR, "dynamicResources: %s", e.GetMessage().c_str());
		return OK;
	}
	//printf("ok\n");
	
	// now extract events until there are no more of them 
	InotifyEvent event;
	while(mInotify->GetEventCount() > 0)
	{
		// get event
		try
		{
			// get event
			mInotify->GetEvent(&event);
			std::list<WatchData>& data = mWatchMap[event.GetDescriptor()];
			
			// go through a list and update all resource combined with them
			std::list<WatchData>::iterator it = data.begin();
			for (; it != data.end(); it++)
			{
				if (it->resource.valid())
				{
					// do only reloading on non-empty resources
					it->resource.lockResource();
						NR_Log(Log::LOG_PLUGIN, Log::LL_DEBUG, "dynamicResources: Monitored %s resource was modified!", it->resourceName.c_str());
						//it->resource.getBase()->reload();
						it->resource.getBase()->setResourceDirty(true);
					it->resource.unlockResource();
				}else{
					NR_Log(Log::LOG_PLUGIN, Log::LL_WARNING, "dynamicResources: Monitored %s resource is not valid!", it->resourceName.c_str());
				}
			
			}
		}
		catch(InotifyException& e)
		{
			NR_Log(Log::LOG_PLUGIN, Log::LL_ERROR, "dynamicResources: Cannot retrieve INotify-Event !");
			NR_Log(Log::LOG_PLUGIN, Log::LL_ERROR, "dynamicResources: %s", e.GetMessage().c_str());
			return OK;
		}
		
	}

	
	return OK;
}


#else
//----------------------------------------------------------------------------------
Result Monitor::update()
{
	return OK;
}

#endif


