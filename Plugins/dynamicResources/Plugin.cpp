#include <nrEngine/nrEngine.h>

#if NR_PLATFORM == NR_PLATFORM_WIN32
	#define _PluginExport __declspec( dllexport )
#else
	#define _PluginExport
#endif


using namespace nrEngine;

#include "Monitor.h"

//---------------------------------------------------------
// Some globals
//---------------------------------------------------------
Engine*  mRoot = NULL;
bool     mThreaded = true;
SharedPtr<Monitor>  mTask;
TaskId   mTaskId = 0;

//---------------------------------------------------------
// Get full string according to the plugin version
//---------------------------------------------------------
extern "C" _PluginExport char* plgVersionString( void )
{
	return "Dynamic Resources v0.1 for nrEngine";
}

//---------------------------------------------------------
// Initialize the plugin
//---------------------------------------------------------
extern "C" _PluginExport int plgInitialize(Engine* root, PropertyList* args)
{
	NR_Log(Log::LOG_PLUGIN, "dynamicResources: %s", plgVersionString());

	// check if we user watn to start the monitor in a thread
	if (args)
	{
		if (args->exists("isThreade"))
		{
			Property& prop = (*args)["isThreaded"];
			NR_Log(Log::LOG_PLUGIN, "dynamicResources: Parameter 'isThreaded' is given, use it!");
			try{
				mThreaded = prop.get<bool>();
			}catch(boost::bad_any_cast& err)
			{
				if (!prop.hasUserData())
				{
					NR_Log(Log::LOG_PLUGIN, "dynamicResources: Parameter 'isThreaded' does not contain valid value nor user data!");
				}else
					mThreaded = *(static_cast<bool*>(prop.getUserData()));
			}
		}
	
	}
		
	// initilize monitoring task and add it to the kernel
	mTask.reset(new Monitor(root));
	if (mThreaded)
		mTaskId = mRoot->sKernel()->AddTask(mTask, ORDER_LOW, TASK_IS_THREAD);
	else
		mTaskId = mRoot->sKernel()->AddTask(mTask, ORDER_LOW);
	
	// initialize the new kind of loader
	mRoot = root;
	
	return 0;	
}
 

//---------------------------------------------------------
// Get the engines version number for which this plugin was wrote
//---------------------------------------------------------
extern "C" _PluginExport  unsigned int plgEngineVersion( void )
{
	return createVersionInteger(0,0,8);
}


//---------------------------------------------------------
// Return a string according to the given error code
//---------------------------------------------------------
extern "C" _PluginExport  char* plgError(int err)
{
	return "Unknown";
}


//---------------------------------------------------------
// Release the plugin
//---------------------------------------------------------
extern "C" _PluginExport  void plgRelease( void )
{
	// remove monitor task 
	mRoot->sKernel()->RemoveTask(mTaskId);
	mTask.reset();
	
	// log
	NR_Log(Log::LOG_PLUGIN, "dynamicResources: Plugin closed and removed");
}
