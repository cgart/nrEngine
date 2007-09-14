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
#include <nrEngine/Plugin.h>
#include <nrEngine/Engine.h>

namespace nrEngine {

	//----------------------------------------------------------------------------------
	Plugin::Plugin() : IResource("Plugin"), mPluginHandle(0)
	{
		m_plgInitialize = NULL;
		m_plgError = NULL;
		m_plgRelease = NULL;
	}

	//----------------------------------------------------------------------------------
	Plugin::~Plugin()
	{
		unloadResource();
	}

	//----------------------------------------------------------------------------------
	void* Plugin::getSymbol(const std::string& name) const
	{
		PluginHandle hdl =  (PluginHandle)NR_PLUGIN_GETSYM(mPluginHandle, name.c_str());
		return hdl;
	}

	//----------------------------------------------------------------------------------
	Result Plugin::initialize(PropertyList* params)
	{
		NR_Log(Log::LOG_ENGINE, Log::LL_DEBUG, "Check if the loaded library is valid plugin");

		// get version information
		m_plgEngineVersion = (plgEngineVersion)getSymbol("plgEngineVersion");
		m_plgVersionString = (plgVersionString)getSymbol("plgVersionString");

		if (!m_plgEngineVersion || !m_plgVersionString)
		{
			NR_Log(Log::LOG_ENGINE, Log::LL_ERROR, "Plugin seems not to be written for the nrEngine");
			NR_Log(Log::LOG_ENGINE, Log::LL_ERROR, "plgVersionString/plgEngineVersion symbols were not found!!!");
			return PLG_SYMBOL_NOT_FOUND;
		}

		// Log this
		NR_Log(Log::LOG_ENGINE, "Plugin found: %s written for nrEngine v%s", m_plgVersionString(), convertVersionToString(m_plgEngineVersion()).c_str());

		// check if plugin is working with the current engine version
		if (m_plgEngineVersion() > nrEngineVersion){
			NR_Log(Log::LOG_ENGINE, Log::LL_ERROR, "Plugin has got greater version as the engine, so plugin not loaded");
			return PLG_WRONG_VERSION;
		}

		// log something
		NR_Log(Log::LOG_PLUGIN, "Initialize plugin %s", getResourceName().c_str());

#define GET_SYMBOL(var, type)\
		{\
			NR_Log(Log::LOG_ENGINE, Log::LL_DEBUG, "Get plugin symbol %s", #type);\
			var = (type)getSymbol(#type);\
			if (!var){\
				NR_Log(Log::LOG_ENGINE, Log::LL_ERROR, "Plugin symbol %s was not found", #type);\
				return PLG_SYMBOL_NOT_FOUND;\
			}\
		}

		// Get plugin symbols
		GET_SYMBOL(m_plgInitialize, plgInitialize);
		GET_SYMBOL(m_plgError, plgError);
		GET_SYMBOL(m_plgRelease, plgRelease);
#undef GET_SYMBOL

		// call the function and check for return code
		int result = m_plgInitialize(Engine::instance(), params);

		// check for error
		if (result != 0){
			NR_Log(Log::LOG_ENGINE, Log::LL_ERROR, "Plugin returns error %d (%s). See plugin log for more detail information", result, m_plgError(result));
			return PLG_EXTERNAL_ERROR;
		}

		// now get some extra symbols
		/*m_plgGetMethods = (plgGetMethods)getSymbol("plgGetMethods");
		m_plgCall = (plgCall)getSymbol("plgCall");

		if (m_plgGetMethods){

			// get the list of methods provided by this plugin
			m_plgGetMethods(mPlgMethods);

			NR_Log(Log::LOG_ENGINE, Log::LL_DEBUG, "Plugin provides following symbols: ");

			// now go through each of this method and print some log info about it
			for (uint32 i=0; i < mPlgMethods.size(); i++){
				std::string params;				
				for (uint32 j=0; j < mPlgMethods[i].param.size(); j++){
					params += mPlgMethods[i].param[j].name;
					if (j < mPlgMethods[i].param.size() - 1) params += ", ";
				}
				NR_Log(Log::LOG_ENGINE, Log::LL_DEBUG, "  found  -  %s (%s)", mPlgMethods[i].name.c_str(), params.c_str());
			}		
		}*/
		
		// all right!
		return OK;
	}

	//----------------------------------------------------------------------------------
	Result Plugin::unloadResource()
	{
		// only unload, if we are loaded
		if (isResourceLoaded())
		{
			// call the release function of the plugin
			m_plgRelease();

			// set all symbols to NULL
			m_plgInitialize = NULL;
			m_plgEngineVersion = NULL;
			m_plgVersionString = NULL;
			m_plgError = NULL;
			m_plgRelease = NULL;

			// now unload the plugin handle from memory
			if (NR_PLUGIN_UNLOAD(mPluginHandle))
			{
				NR_Log(Log::LOG_ENGINE, Log::LL_ERROR,
					"Could not unload plugin %s. System Msg: %s",
					getResourceName().c_str(), getLastPluginError().c_str());
				return PLG_UNLOAD_ERROR;
			}
	
			// set the handle to 0
			mPluginHandle = NULL;
			markResourceUnloaded();
		}

		// OK
		return OK;
	}

	//----------------------------------------------------------------------------------
	Result Plugin::reloadResource(PropertyList* params)
	{
		if (!isResourceLoaded())
		{
			// get filename
			const std::string& name = getResourceFilenameList().front();
			
			// now load the library
			mPluginHandle = (PluginHandle)NR_PLUGIN_LOAD(name.c_str());

			// check whenever the library could be loaded
			if (mPluginHandle == NULL)
			{
				NR_Log(Log::LOG_ENGINE, Log::LL_ERROR,
					"Plugin %s could not be loaded! System Msg: %s",
					getResourceName().c_str(), getLastPluginError().c_str());
				return PLG_COULD_NOT_LOAD;
			}

			// force the plugin resource object to initialize itself after loading
			return initialize(params);
		}
		return OK;
	}

	//-----------------------------------------------------------------------
	std::string Plugin::getLastPluginError()
	{
#if NR_PLATFORM == NR_PLATFORM_WIN32
		LPVOID lpMsgBuf; 
		FormatMessage( 
			FORMAT_MESSAGE_ALLOCATE_BUFFER | 
			FORMAT_MESSAGE_FROM_SYSTEM | 
			FORMAT_MESSAGE_IGNORE_INSERTS, 
			NULL, 
			GetLastError(), 
			MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), 
			(LPTSTR) &lpMsgBuf, 
			0, 
			NULL 
			); 
		std::string ret = (char*)lpMsgBuf;
		
		// Free the buffer.
		LocalFree( lpMsgBuf );
		return ret;
#elif NR_PLATFORM == NR_PLATFORM_LINUX
		return std::string(dlerror());
#elif NR_PLATFORM == NR_PLATFORM_APPLE
		return std::string(mac_errorBundle());
#else
		return std::string("");
#endif
	}

	//----------------------------------------------------------------------------------
	EmptyPlugin::EmptyPlugin() : Plugin()
	{

	}

	//----------------------------------------------------------------------------------
	EmptyPlugin::~EmptyPlugin()
	{

	}

	//----------------------------------------------------------------------------------
	Result EmptyPlugin::unloadResource()
	{
		return OK;
	}
	//----------------------------------------------------------------------------------
	Result EmptyPlugin::reloadResource()
	{
		return OK;
	}

	//----------------------------------------------------------------------------------
	void* EmptyPlugin::getSymbol(const std::string& name) const
	{
		return NULL;
	}

};

