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
#include <nrEngine/PluginLoader.h>
#include <nrEngine/Log.h>

namespace nrEngine{
	
	//----------------------------------------------------------------------------------
	ScriptFunctionDec(loadPlugin, PluginLoader)
	{
		// check if the parameter count is valid
		if (args.size() <= 2){
			NR_Log(Log::LOG_ENGINE, Log::LL_ERROR, "PluginLoader: loadPlugin(name, file) : wrong parameter count");
			return ScriptResult(std::string("More parameters required! loadPlugin(name, file)"));
		}
		
		// try to load all the plugins given
		Engine::instance()->loadPlugin("", args[2], args[1]);

		// ok
		return ScriptResult();
	}
	
	//----------------------------------------------------------------------------------
	PluginLoader::PluginLoader() : IResourceLoader("PluginLoader")
	{
		initializeResourceLoader();
		
		// register some new functions by the scripting engine
		Engine::sScriptEngine()->add("loadPlugin", loadPlugin);
	}

	
	//----------------------------------------------------------------------------------
	PluginLoader::~PluginLoader()
	{
		// remove the functions provided to the scripting engine
		Engine::sScriptEngine()->del("loadPlugin");
	}
	
	//----------------------------------------------------------------------------------
	Result PluginLoader::initializeResourceLoader()
	{

		// fill out supported resource types;
		declareSupportedResourceType("Plugin");
		declareSupportedResourceType("nrPlugin");
		
		// we do only support dll files in windows version of our engine
#if NR_PLATFORM == NR_PLATFORM_WIN32
		declareSupportedFileType("dll");
		declareTypeMap("dll", "Plugin");
#elif NR_PLATFORM == NR_PLATFORM_LINUX
		declareSupportedFileType("so");
		declareTypeMap("so", "Plugin");
#endif
		return OK;
	}

	//----------------------------------------------------------------------------------
	std::string PluginLoader::getSuffix(const std::string& resType)
	{
#if NR_PLATFORM == NR_PLATFORM_WIN32
		return std::string("dll");
#elif NR_PLATFORM == NR_PLATFORM_LINUX
		return std::string("so");
#endif
	}

	//----------------------------------------------------------------------------------
	Result PluginLoader::loadResource(IResource* res, const std::string& fileName, PropertyList* param)
	{

		// check whenever we have a valid file name of the plugin
		if (fileName.length() <= 3){
			NR_Log(Log::LOG_ENGINE, Log::LL_ERROR, "File name of the plugin is not valid %s. It must contain at least 3 characters.", fileName.c_str());
			return RES_BAD_FILETYPE;
		}

		std::string name = fileName;
		
#if NR_PLATFORM == NR_PLATFORM_LINUX
		if (name.substr(name.length() - 3, 3) != ".so")
		{
			// dlopen() does not add .so to the filename, like windows does for .dll
			NR_Log(Log::LOG_ENGINE, Log::LL_WARNING, "\".so\" added to the plugin file name %s", name.c_str());
			name += ".so";
		}
#elif NR_PLATFORM == NR_PLATFORM_WIN32
		if (name.substr(name.length() - 4, 4) == ".dll")
		{
			// windows does automaticly add .dll to the file name, so we have to cut the name
			name = name.substr(0, name.length() - 4);
		}		
#endif

		// set filename to the extended one
		std::list<std::string> flist;
		flist.push_back(name);
		res->setResourceFilename(flist);

		// call reloading method in resource, so it get loaded
		return res->reload(param);		
	}
	
	//----------------------------------------------------------------------------------
	IResource* PluginLoader::createResource(const std::string& resourceType, PropertyList* params)
	{	
		// create an plugin instance
		return new Plugin();
	}
	
	
	//----------------------------------------------------------------------------------
	IResource* PluginLoader::createEmptyResource(const std::string& resourceType)
	{
		// create an instance of empty plugin
		return new EmptyPlugin();	
	}
	
	
};

