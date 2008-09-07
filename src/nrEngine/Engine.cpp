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


#include <nrEngine/Engine.h>
#include <nrEngine/Kernel.h>
#include <nrEngine/Clock.h>
#include <nrEngine/Profiler.h>
#include <nrEngine/ResourceManager.h>
#include <nrEngine/PluginLoader.h>
#include <nrEngine/FileStreamLoader.h>
#include <nrEngine/EventManager.h>
#include <nrEngine/ScriptEngine.h>
#include <nrEngine/ScriptLoader.h>
#include <nrEngine/PropertyManager.h>
#include "DefaultScriptingFunctions.cpp"

namespace nrEngine{

	// Here we store the instances of singletons
	SharedPtr<Engine> 	Engine::sSingleton;
	Log*			Engine::_logger = NULL;
	Kernel*			Engine::_kernel = NULL;
	Clock*			Engine::_clock = NULL;
	Profiler*		Engine::_profiler = NULL;
	ResourceManager* 	Engine::_resmgr = NULL;
	ScriptEngine* 		Engine::_script = NULL;
	EventManager* 		Engine::_event = NULL;
	PropertyManager* 	Engine::_propmgr = NULL;

	//--------------------------------------------------------------------------
	bool Engine::valid(void* p, char* name, bool showWarn)
	{
		if (p == NULL && showWarn)
		{
			printf("Warning: Given pointer of type %s is not valid!\n", name);
			return false;
		}

		return p != NULL;
	}

	//------------------------------------------------------------------------
	Engine* Engine::instance(bool brelease)
	{
		if (brelease && valid()){
			sSingleton.reset();
			return NULL;
		}else if (!brelease && !valid()){
			sSingleton.reset(new Engine());
		}
		return sSingleton.get();
	}

	//------------------------------------------------------------------------
	void Engine::release()
	{
		if (valid())
		{
			sSingleton.reset();
		}
	}

	//--------------------------------------------------------------------------
	 Log* Engine::sLog()
	{
		valid(_logger, (char*)"Log");
		return _logger;
	}

	//--------------------------------------------------------------------------
	 Kernel* Engine::sKernel()
	{
		valid(_kernel, (char*)"Kernel");
		return _kernel;
	}

	//--------------------------------------------------------------------------
	 Clock* Engine::sClock()
	{
		valid(_clock, (char*)"Clock");
		return _clock;
	}

	//--------------------------------------------------------------------------
	 Profiler* Engine::sProfiler()
	{
		valid(_profiler, (char*)"Profiler");
		return _profiler;
	}

	//--------------------------------------------------------------------------
	 ResourceManager* Engine::sResourceManager()
	{
		valid(_resmgr, (char*)"ResourceManager");
		return _resmgr;
	}

	//--------------------------------------------------------------------------
	 EventManager* Engine::sEventManager()
	{
		valid(_event, (char*)"EventManager");
		return _event;
	}

	//--------------------------------------------------------------------------
	 ScriptEngine* Engine::sScriptEngine()
	{
		valid(_script, (char*)"ScriptEngine");
		return _script;
	}

	//--------------------------------------------------------------------------
	PropertyManager* Engine::sPropertyManager()
	{
		valid(_propmgr, (char*)"PropertyManager");
		return _propmgr;
	}

	//------------------------------------------------------------------------
	Engine::Engine()
	{
		// first of all create the log system, so we get valid pointer
		_logger = (new Log());
		if (_logger == NULL)
		{
			NR_EXCEPT(OUT_OF_MEMORY, "Log system could not be created. Probably memory is full", "Engine::Engine()");
		}

		// create property manager
		_propmgr = (new PropertyManager());
		if (_propmgr == NULL)
		{
			NR_EXCEPT(OUT_OF_MEMORY, "PropertyManager could not been created. Check if the memory is not full", "Engine::Engine()");
		}

		// initialize the scripting engine
		_script = (new ScriptEngine());
		if (_script == NULL)
		{
			NR_EXCEPT(OUT_OF_MEMORY, "Scripting Engine could not been created. Check if the memory is not full", "Engine::Engine()");
		}

		// now create a kernel subsystem
		_kernel = (new Kernel());
		if (_kernel == NULL)
		{
			NR_EXCEPT(OUT_OF_MEMORY, "Kernel system could not be created. Probably memory is full", "Engine::Engine()");
		}

		// initialize the clock
		_clock = (new Clock());
		if (_clock == NULL)
		{
			NR_EXCEPT(OUT_OF_MEMORY, "Clock could not be created. Probably memory is full", "Engine::Engine()");
		}
	}

	//------------------------------------------------------------------------
	Engine::~Engine()
	{
        stopEngine();

		// remove default scripting methods
		DefaultScriptingFunctions::delMethods();

		// remove the manager
		delete _resmgr;

		// delete the clock
		delete _clock;

		// delete the kernel
		delete _kernel;

		// delete the scripting engine
		delete _script;

		// delete the event system
		delete _event;

		// remove property manager
		delete _propmgr;

		// remove profiler
		delete _profiler;

		// delete the log system
		delete _logger;

	}

	//------------------------------------------------------------------------
	bool Engine::initializeLog(const std::string& logPath)
	{
		return _logger->initialize(logPath) == OK;
	}

	//------------------------------------------------------------------------
	void Engine::stopEngine()
	{
		// give log information
		_logger->log(Log::LOG_ENGINE, (char*)"nrEngine stopped");

		// stop kernel tasks
		_kernel->StopExecution();
        _kernel->OneTick();
	}
	//------------------------------------------------------------------------
	bool Engine::initializeEngine()
	{
		// give some info about the underlying engine
		NR_Log(Log::LOG_ENGINE | Log::LOG_CONSOLE | Log::LOG_KERNEL, (char*)"nrEngine v%s - %s", convertVersionToString(nrEngineVersion).c_str(), NR_VERSION_NAME);

		// initialize the clock
		SharedPtr<TimeSource> timeSource(new TimeSource());

		// initialize profiler singleton
		_profiler = (new Profiler(timeSource));
		if (_profiler == NULL)
			NR_Log(Log::LOG_ENGINE, Log::LL_ERROR, (char*)"Profiler singleton could not be created. Probably memory is full");

		// now add the clock into kernel
		_clock->setTimeSource(timeSource);
		_clock->setTaskType(TASK_SYSTEM);
		_kernel->AddTask(SharedPtr<ITask>(_clock, null_deleter()), ORDER_SYS_FIRST);

		// initialize resource manager singleton
		_event = (new EventManager());
		if (_event == NULL)
			NR_Log(Log::LOG_ENGINE, Log::LL_ERROR, (char*)"Event manager singleton could not be created. Probably memory is full");

		_event->createChannel(NR_DEFAULT_EVENT_CHANNEL);
		_event->setTaskType(TASK_SYSTEM);
		_kernel->AddTask(SharedPtr<ITask>(_event, null_deleter()), ORDER_SYS_SECOND);

		// initialise default scripting methods
		DefaultScriptingFunctions::addMethods();

		// initialize resource manager singleton
		_resmgr = (new ResourceManager());
		if (_resmgr == NULL)
			NR_Log(Log::LOG_ENGINE, Log::LL_ERROR, (char*)"Resource manager singleton could not be created. Probably memory is full");

		// Add the file reading functionality
		ResourceLoader fileLoader (new FileStreamLoader());
		_resmgr->registerLoader((char*)"FileStreamLoader", fileLoader);

		// create an instance of plugin loader and add it to the resource manager
		ResourceLoader loader ( new PluginLoader() );
		_resmgr->registerLoader((char*)"PluginLoader", loader);

		// create simple scripting language instancies
		ResourceLoader scriptLoader( new ScriptLoader() );
		_resmgr->registerLoader((char*)"ScriptLoader", scriptLoader);

		return true;
	}

	//------------------------------------------------------------------------
	void Engine::runEngine()
	{

		// log info
		_logger->log(Log::LOG_ENGINE, (char*)"nrEngine started");

		// start the kernel
		_kernel->Execute();

	}

	//------------------------------------------------------------------------
	void Engine::updateEngine()
	{
		// update the kernel
		_kernel->OneTick();
	}


	//------------------------------------------------------------------------
	bool Engine::loadPlugin(const std::string& path, const std::string& file, const std::string& name)
	{
		// check if the engine is initialized
		if (_resmgr == NULL)
		{
			NR_Log(Log::LOG_ENGINE, Log::LL_ERROR, (char*)"Engine was not initialized properly");
			return false;
		}

		// try to load the resource and check for the error code
		IResourcePtr plg = _resmgr->loadResource(name, (char*)"Plugins", path + file, (char*)"Plugin");

		return true;
	}


	//------------------------------------------------------------------------
	//Plugin getPlugin(const std::string& name){

	//}

}; // end namespace

