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


#ifndef _NR_PLUGIN_RESOURCE__H_
#define _NR_PLUGIN_RESOURCE__H_


//----------------------------------------------------------------------------------
// Includes
//----------------------------------------------------------------------------------
#include "Prerequisities.h"
#include "Resource.h"
#include "PluginLoader.h"

/*!
 * \defgroup plugin Plugin managment
 *
 * Plugins are used to extend the functionality of the underlying system
 * without recompiling it. So our engine does support plugins, which allows
 * programm new functionality to the engine without recompiling it.
 * 
 * One of the best examples of such a plugin is a possibility to extend the resource
 * managment system to new resources. You can program addiditional texture loader
 * for example and "plug in" this into the engine. The new loader could load 
 * textures and prepare them as resources, so your program can get use of them.
 *
 * Another example could be a plugin that runs as task in the engine's kernel
 * and do something. For example an ingame console could be such a task, which can
 * be fully programmed as external plugin.
 *
 * Each plugin is a simple dynamic library (*.dll, *.so) file, which contains the plugin.
 * Each plugin will be loaded if it is added to the plugin manager. The plgLoad() function
 * of the plugin will be called. If the plugin is unloaded, so call plgUnload().
 *
 * Each plugin should export some default functions to be able to be loaded by the manager.
 * Plugins could be simple functionality plugins (like any loader) or a new task
 * in the engine's kernel (like a console). This capability of each plugin is readed 
 * through special plugin functions.
 *
 * Also each plugin should provide a function which can be used to access internal
 * functionality of that plugin by using of strings. In our case this subroutine
 * should be called plgCall and accept as first the name of the function and then a
 * parameter list of parameters and their values. We use this to provide the user
 * applications a possibility to call certain functions from plugins which are not
 * replace a default behaviour of the engine but add some new functionality. The good
 * example where it can be used is a scripting. You load a plugins from the script
 * and then call some functions from the plugin whithin scripting. 
 * 
 **/
 
namespace nrEngine{
	
		
	/**
	* Each plugin is an dynamic library. So we have to use some handles
	* given by the os api to handle with these libraries. On each os
	* this handle is of different type. So we have to find out
	* which os do we using now and define the right type
	*
	* \ingroup plugin
	**/
	#if NR_PLATFORM == NR_PLATFORM_WIN32
		typedef HINSTANCE PluginHandle;
	#elif NR_PLATFORM == NR_PLATFORM_LINUX
		typedef void* PluginHandle;
	#elif NR_PLATFORM == NR_PLATFORM_APPLE
		typedef CFBundleRef PluginHandle;
	#endif

	//! General interface for any plugin in the engine
	/**
	* Plugins are used to extend the functionality of the engine without rewriting it.
	* You can just write new plugin and add this to the engine, so the engine could get
	* it loaded. The user will then get the new functionality automaticaly or by using
	* the concept of application - plugin - communication.
	*
	* Plugins are resources that used in the same way like normal resources. 
	*
	* Each plugin library has to export following symbols:
	*	- int plgInitialize(Engine*)
	*		- will be called after library is loaded
	*		- plugin will get the pointer to the root object of the engine
	*		- return 0 for success otherwise error code
	*
	*	- unsigned int plgEngineVersion()
	*		- plugin should return a version number according to the engine
	*		- return the value also if not initialized before
	*
	*	- char* plgVersionString()
	*		- return a full string containing information about the plugin version/name
	*		- return the strign also if not initialized before
	*
	*	- char* plgError (int)
	*		- return a short error string for the given code
	*
	*	- void plgRelease()
	*		- will be called before plugin is getting unloaded
	*
	*
	* Following symbols are optional:
	*
	*	- void plgGetMethods (std::vector<PlgMethod> &)
	*		- should fill the given vector whithin informations of plugin methods
	*		  which could be used as extra calls from the user point of view (e.g. scripts)
	*
	*	- void plgCall (const std::string&, std::vector<PlgParam>& )
	* 		- call a subroutine from the plugin, also send parameters
	*
	*
	* @see IResource
	* \ingroup plugin
	**/
	class _NRExport Plugin : public IResource{
	public:
			
		Plugin();
		virtual ~Plugin();

		/**
		 * Retuns the addres of a symbol (function,variable) from the plugin.
		 * You have to cast this addres for using.
		 *
		 * @param name Unique name of the symbol
		 **/
		virtual void* getSymbol(const std::string& name) const;

		//! Each method in the plugin accept parameters of this type
		/*typedef struct _plgParam {

			//! Parameter name
			std::string name;

			//! Parameter value as string
			std::string value;

		} PlgParam;
		
		//! This structure describes a subroutine in a plugin
		typedef struct _plgMethod {

			//! Name of the subroutine
			std::string name;

			//! parameter names which can be passed to this method
			std::vector<PlgParam> param;

			//! Pointer pointing to the provided method
			void* symbol_ptr;
			
		} PlgMethod;*/
		 
	protected:
	
		//! Plugin loader is a friend, so it get full accecc to our functions
		friend class PluginLoader;

		//! Handle of the used dynamic library
		PluginHandle	mPluginHandle;

		//! this is the list of methods from the plugin
		//std::vector<PlgMethod> mPlgMethods;

		//! This defines the interface of the initialization function of the plugin
		typedef int (*plgInitialize) ( Engine*, PropertyList* );
		
		//! Define the type of the error get function of th eplugin
		typedef char* (*plgError) ( int );
				
		//! This is the interface of the release function of the plugin
		typedef void (*plgRelease) ( void );
		
		//! Interface for the version information
		typedef unsigned int (*plgEngineVersion) ( void );
		
		//! Get full version string of the plugin
		typedef char* (*plgVersionString) ( void );
		
		/*//! Get a list of exported functions
		typedef void (*plgGetMethods) ( std::vector<PlgMethod>& );

		//! Call a method whithin given parameters
		typedef void (*plgCall) (const std::string&, const std::vector<PlgParam>& );
		*/
		
		plgInitialize		m_plgInitialize;
		plgError			m_plgError;
		plgRelease			m_plgRelease;
		plgEngineVersion	m_plgEngineVersion;
		plgVersionString	m_plgVersionString;
			
		/*plgGetMethods		m_plgGetMethods;
		plgCall				m_plgCall;
		*/
		/**
		 * This method is called by the plugin loader after it has loaed the library.
		 * The plugin object has now to get all symbols and initialize the library.
		 * @return error code
		 **/
		Result initialize(PropertyList* params = NULL);
		
		/**
		 * Derived function from IResource
		 **/
		virtual Result unloadResource();
		virtual Result reloadResource(PropertyList* params);					

		//! Get last plugin error message from the system
		std::string getLastPluginError();

	};

	
	//! Simple class for empty plugin. This plugin doesn't have any effect
	/**
	 * This class represents an empty plugin object. This corresponds
	 * to our empty resource interface, needed to work for resource managment.
	 * Empty plugin does not do anything.
	 *
	 * \ingroup plugin
	 **/
	class _NRExport EmptyPlugin : public Plugin{
	public:
	
		//! Constructor does not call anything
		EmptyPlugin();
		
		//! Release used memory
		~EmptyPlugin();
		
		//! Unload the empty resource
		Result unloadResource();
		
		//! Reload the empty resource
		Result reloadResource();
		
		//! Return always NULL
		void* getSymbol(const std::string& name) const;
		
	};
	
};

#endif
