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


#ifndef _NR_PLUGIN_RESOURCE_LOADER_H_
#define _NR_PLUGIN_RESOURCE_LOADER_H_

//----------------------------------------------------------------------------------
// Includes
//----------------------------------------------------------------------------------
#include "Prerequisities.h"
#include "ResourceLoader.h"
#include "Plugin.h"
#include "ScriptEngine.h"

//----------------------------------------------------------------------------------
// Define the library loading functions on the appropriate OS API
//----------------------------------------------------------------------------------
#if NR_PLATFORM == NR_PLATFORM_WIN32
#    define NR_PLUGIN_LOAD( a ) LoadLibrary( a )
#    define NR_PLUGIN_GETSYM( a, b ) GetProcAddress( a, b )
#    define NR_PLUGIN_UNLOAD( a ) FreeLibrary( a )

#elif NR_PLATFORM == NR_PLATFORM_LINUX
#    define NR_PLUGIN_LOAD( a ) dlopen( a, RTLD_LAZY )
#    define NR_PLUGIN_GETSYM( a, b ) dlsym( a, b )
#    define NR_PLUGIN_UNLOAD( a ) dlclose( a )

#elif NR_PLATFORM == NR_PLATFORM_APPLE
#    define NR_PLUGIN_LOAD( a ) mac_loadExeBundle( a )
#    define NR_PLUGIN_GETSYM( a, b ) mac_getBundleSym( a, b )
#    define NR_PLUGIN_UNLOAD( a ) mac_unloadExeBundle( a )
#endif

namespace nrEngine{
	
	//! Plugin loader does handle loading/creating of plugin libraries for the engine
	/**
	* This is a derived class from ResourceLoader. This class does support loading
	* of plugins (*.so, *.dll).
	* 
	* @see IResourceLoader
	* \ingroup plugin
	**/
	class _NRExport PluginLoader : public IResourceLoader{
	public:
			
		/**
		* Declare supported resource types and supported files.
		**/
		PluginLoader();
		
		/**
		* Destructor 
		**/
		~PluginLoader();

	private:
	
		/**
	 	* Initialize supported resource and file types
	 	* @see IResourceLoader::initialize()
		**/
		Result initializeResourceLoader();

		/**
		 * @see IResourceLoader::loadResource()
		 **/
		Result loadResource(IResource* res, const std::string& fileName, PropertyList* param = NULL);

		/**
		 * @see IResourceLoader::createResource()
		 **/
		IResource* createResource(const std::string& resourceType, PropertyList* params = NULL);
			
		/**
		 * @see IResourceLoader::createEmptyResource()
		 **/
		IResource* createEmptyResource(const std::string& resourceType);

		/**
		 * @see IResourceLoader::getSuffix()
		 **/
		std::string getSuffix(const std::string& resType);


		//! New scripting function to allow loading of plugins
		ScriptFunctionDef(loadPlugin);
		
	};

};

#endif
