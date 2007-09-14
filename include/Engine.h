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

#ifndef __NR_ENGINE_CORE_H_
#define __NR_ENGINE_CORE_H_


//----------------------------------------------------------------------------------
// Includes
//----------------------------------------------------------------------------------
#include "Prerequisities.h"
#include "Log.h"

namespace nrEngine{

	//! Core class of the engine. Used to create subsystems.
	/**
	* This is our main class representing the engine itself. This class is
	* able to create and initialize particular elements of the engine's core.
	* So before you can use the engine you have to create an instance
	* of this class.
	*
	* The engine's core class is a singleton. We also store all subsystems as
	* static, to be sure that they exists also only once in memory.
	*
	* \ingroup engine
	**/
	class _NRExport Engine{
		public:

			/**
			* Initialize the log component of the engine. You have to call
			* this function before you get using of engine's components. They all
			* will log their work information to appropriate log files, so the log
			* part of the engie has to be initialized before.<br>
			* If you do not initilaize the logging functionality, so no log
			* files will be written. No error will occurs.
			*
			* \param logPath Under this directory all log files will be created
			* \return false if error occurs or true otherwise
			**/
			bool initializeLog(const std::string& logPath);

			/**
			* Release the engine and all used memory. The call of this function
			* will immidiately stop all kernel tasks and will stop the engine.<br>
			* Call this function if you want to quit the engine.
			**/
			void stopEngine();

			/**
			* "Gentlemans, start your engines!" - This is the entry point for
			* your application to start the engine. Starting of the engine will
			* also start a kernel, which is running system + application tasks.
			* If no more tasks is active, the kernel will be shutted down and engine
			* will stop.
			**/
			void runEngine();

			/**
			* If you do not want to run the engine in their own loop (\a startEngine() )
			* you can update the engine through this function. This function should be
			* called each frame. This will update the kernel tasks and all engine's
			* subsystems
			**/
			void updateEngine();

			/**
			* This method will initialize all engine's subsystems, that are essential
			* for engine's work. Please call this function after you have intialized
			* the logging, because there will be a lot of log information.
			*
			* \return true if all intialization are succeed, or false otherwise
			**/
			bool initializeEngine();

			/**
			 * Load a certain plugin directly into the engine. The plugin will
			 * be loaded and the initialize subroutine will be called. If the plugin
			 * returns an invalid resulting code, so the plugin will be unloaded again.
			 *
			 * Use this function to load certain plugins needed for your application
			 * (i.e. ScriptLoader) and let them run with certain parameters, so you are
			 * able to use the engine as you need.
			 *
			 * @param path Path where the plugin can be founded
			 * @param file Filename relative to the path of the plugin
			 * @param name Name which will be then used to access the plugin later
			 *
			 * @return false if an error occurs
			 *
			 * NOTE: Check log files for more detailed error description
			 **/
			bool loadPlugin(const std::string& path, const std::string& file, const std::string& name);


			/**
			* Get a pointer to a log subsystem of the engine.
			* The pointer is always not equal to NULL. By initializing of
			* the log subsystem you just setup the log target.
			* So you can always log to the log-system without checking
			* whenever pointer is valid or not.
			**/
			static Log* sLog();

			/**
			* Return a pointer to the kernel subsystem of the engine. Our kernel
			* is normaly a singleton, so you can access to it, without this method.
			* 
			* Returned pointer is always valid.
			* @see Kernel
			**/
			static Kernel* sKernel();

			/**
			* Return a pointer to the underlying clock of the engine. The clock
			* is a singleton, so you can access it on another way.
			*
			* Returned pointer is always valid
			* @see Clock
			**/
			static Clock* sClock();

			/**
			* Returns a pointer to the profiler object. The profiler
			* is used to mess the time of execution
			**/
			static Profiler* sProfiler();
			
			/**
			* Returns a pointer to the ressource manager object.
			**/
			static ResourceManager* sResourceManager() ;
			
			/**
		 	* Return an instance of the event manager class
		 	**/
			static EventManager* sEventManager();
		 	
		 	/**
		 	* Get a pointer to the scripting engine interface
		 	**/
			static ScriptEngine* sScriptEngine();

			/**
			 * Get instance to the property manager of the engine.
			 **/
			static PropertyManager* sPropertyManager();
			
			/**
			 * Get the singleton instance. Passing the parameter specify
			 * if the instance should be released
			 **/
			static Engine* instance(bool release = false);

			/**
			 * Get the pointer to the instance of the engine
			 **/
			static Engine* sEngine() { return instance(); }
			
			 /**
			 * Check whenever the engine'S singleton was created before
			 **/
			static bool valid(void* p = sSingleton.get(), char* name = "Engine", bool showWarn = true);
			 
			/**
			 * Release the singleton object
			 **/
			static void release();

			/**
			* Delete the core object and try to deinitialize and to delete
			* all subcomponents of the engine
			**/
			~Engine();

		private:

			/**
			* Create an instance of the engine's core object. The constructor
			* is private, so you only can create instances through instance() method
			**/
			Engine();

			static Log*			_logger;
			static Kernel*		_kernel;
			static Clock*		_clock;
			static Profiler*	_profiler;
			static ResourceManager* _resmgr;
			static ScriptEngine* _script;
			static EventManager* _event;
			static PropertyManager* _propmgr;
			
			//! Store pointer to the engine's core singleton object
			static SharedPtr<Engine> sSingleton;
			
	};

}; // end namespace


#endif
