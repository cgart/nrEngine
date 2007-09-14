#include <nrEngine/nrEngine.h>

#if NR_PLATFORM == NR_PLATFORM_WIN32
	#define _PluginExport __declspec( dllexport )
#else
	#define _PluginExport
#endif


#include "ScriptLoader.h"
#include "Script.h"
#include "ScriptConnector.h"

using namespace nrEngine;

//---------------------------------------------------------
// Some globals
//---------------------------------------------------------
Engine* 				mRoot = NULL;
ResourceLoader			mLoader;
ScriptConnector*		mConnector = NULL;
bool					mLuaStateCreated = false;

//---------------------------------------------------------
// Get full string according to the plugin version
//---------------------------------------------------------
extern "C" _PluginExport char* plgVersionString( void )
{
	return "Lua embedded scripting v0.1 for nrEngine";
}

//---------------------------------------------------------
// Initialize the plugin
//---------------------------------------------------------
extern "C" _PluginExport int plgInitialize(Engine* root, PropertyList* args)
{
	// initialize lua subsystem
	NR_Log(Log::LOG_PLUGIN, "luaScript: %s", plgVersionString());

	// so now check the parameters, if we have a parameter luastate
	if (args){
		if (args->exists("luastate")){
			Property& prop = (*args)["luastate"];
			NR_Log(Log::LOG_PLUGIN, "luaScript: Parameter 'luastate' is given, use it!");
			try{
				LuaScript::State = prop.get<lua_State*>();
			}catch(boost::bad_any_cast& err){
				// ok we have got a bad cast, so check for user data in the property
				if (!prop.hasUserData()){
					NR_Log(Log::LOG_PLUGIN, "luaScript: Parameter 'luastate' does not contain valid value nor user data!");
					LuaScript::State = NULL;
				}else
					LuaScript::State = static_cast<lua_State*>(prop.getUserData());
			}
		}
	}
	
	// setup lua by myself
	if (LuaScript::State == NULL)
	{
		LuaScript::State = luaL_newstate();
		if (LuaScript::State == NULL){
			NR_Log(Log::LOG_PLUGIN, Log::LL_ERROR, "luaScript: Initialization of lua state failed!");
			return -1;
		}
		mLuaStateCreated = true;
		luaL_openlibs(LuaScript::State);
	}

	// initialize the new kind of loader
	mRoot = root;
	mLoader.reset(new LuaScriptLoader());
	mRoot->sResourceManager()->registerLoader("LuaScriptLoader", mLoader);

	// initialize the script connector
	mConnector = new LuaScriptConnector();
	
	return 0;	
}
 

//---------------------------------------------------------
// Get the engines version number for which this plugin was wrote
//---------------------------------------------------------
extern "C" _PluginExport  unsigned int plgEngineVersion( void )
{
	return createVersionInteger(0,0,7);
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
	// remove the loader from the engine
	Engine::sResourceManager()->removeLoader("LuaScriptLoader");
	
	if (mLuaStateCreated && LuaScript::State != NULL){
		lua_close(LuaScript::State);
	}
	
	mLoader.reset();

	if (mConnector) delete mConnector;
		
	NR_Log(Log::LOG_PLUGIN, "luaScript: Plugin closed and removed");

}
