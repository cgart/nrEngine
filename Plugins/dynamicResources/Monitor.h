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


#ifndef _NR_MONITOR_TASK_H_
#define _NR_MONITOR_TASK_H_


//----------------------------------------------------------------------------------
// Includes
//----------------------------------------------------------------------------------
#include <nrEngine/nrEngine.h>
using namespace nrEngine;


#if NR_PLATFORM == NR_PLATFORM_LINUX
	#include "inotify-cxx.h"
	#define USE_INOTIFY
#endif


/**
 * This class does monitor all loaded resources and will notify the 
 * resource loader if file has changed, so that the file could be reloaded.
 **/
class Monitor : public ITask
{
	public:

		//! Create monitoring instance
		Monitor(Engine*);
		
		//! Release the instance 
		~Monitor();
		
		//! Calls when the task is finished
		Result stopTask();	
		
		//! Initialize the task, cause ti was added to the kernel
		Result onStartTask();
		
		//! Update the task
		Result updateTask() { return update();}


	private:
	
		//! Engine pointer 
		Engine* mRoot;
		
		//! Initialize the monitor 
		void init();
		
		//! Update the monitoring interface
		Result update();
		
		//! Add new file and associated resource to watch on
		void addMonitor (IResourcePtr res, const std::string& file);

		// We are using inotify interface
		#ifdef USE_INOTIFY
		
			//! file descriptor
			SharedPtr<Inotify> mInotify;
			
			//! To watch we use this structures
			typedef struct _WatchData
			{
				IResourcePtr resource;
				InotifyWatch* watcher;
				std::string resourceName;
			} WatchData;
			
			//! Define map type for appropriate storing of watch descriptors
			typedef std::map<int, std::list<WatchData> > WatcherMap;
			
			//! Here we store the watching descriptors for the files
			WatcherMap mWatchMap;
			
			//! Initialize inotify interface
			Result initInotify();
		#endif
		
};


#endif
