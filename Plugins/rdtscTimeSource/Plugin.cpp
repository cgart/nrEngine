#include <nrEngine/nrEngine.h>

#if NR_PLATFORM == NR_PLATFORM_WIN32
    #define _PluginExport __declspec( dllexport )
#else
    #define _PluginExport
#endif

#include "Cpu.h"
#include "TimeSource.h"


using namespace nrEngine;

//---------------------------------------------------------
// Some globals
//---------------------------------------------------------
Engine* 							mRoot = NULL;

boost::shared_ptr<Cpu>				mCpu;
boost::shared_ptr<rdtscTimeSource>	mTimeSource;
boost::shared_ptr<TimeSource>		mOldTimeSource;

//std::vector<Plugin::PlgMethod>		mMethods;
uint32 syncInterval = 500;
uint32 syncTime = 10;


//---------------------------------------------------------
// Initialize the plugin
//---------------------------------------------------------
extern "C" int plgInitialize(Engine* root, PropertyList* args)
{	
	mRoot = root;
    mOldTimeSource = mRoot->sClock()->getTimeSource();

    if (args){
        if (args->exists("sync_interval"))
        {
            Property& prop = (*args)["luastate"];
            try{
                syncInterval = prop.get<uint32>();
                NR_Log(Log::LOG_PLUGIN, "rdtscTimeSource: sync_interval=%d", syncInterval);
            }catch(boost::bad_any_cast& err){
                NR_Log(Log::LOG_PLUGIN, "rdtscTimeSource: sync_interval is given, but has a bad type");
            }
        }
        if (args->exists("sync_duration"))
        {
            Property& prop = (*args)["luastate"];
            try{
                syncTime = prop.get<uint32>();
                NR_Log(Log::LOG_PLUGIN, "rdtscTimeSource: sync_duration=%d", syncTime);
            }catch(boost::bad_any_cast& err){
                NR_Log(Log::LOG_PLUGIN, "rdtscTimeSource: sync_duration is given, but has a bad type");
            }
        }
    }
    
	// create cpu instance
	mCpu.reset(new Cpu());
	NR_Log(Log::LOG_PLUGIN, "rdtscTimeSource: Retrieve CPU Information");
	mCpu->detect();
	NR_Log(Log::LOG_PLUGIN, "rdtscTimeSource: %s", mCpu->getCPUDescription().c_str());
	
	// create Time Source
	mTimeSource.reset( new rdtscTimeSource(mCpu) );
    mTimeSource->setSyncDuration(syncTime);
    
    // check whenever CPU does support the thing
    if (mTimeSource->isSupported())
    {		    
	    // now set new time source to the clock
	    NR_Log(Log::LOG_PLUGIN, "rdtscTimeSource: Set the high performance time source for the clock");
	    mRoot->sClock()->setTimeSource(mTimeSource);

        // let sync the timesource every 500 ms
        mRoot->sClock()->setSyncInterval(syncInterval);
        
	    // declare new function to the scripting engine
	    //std::vector<ScriptParam> param;
	    //param.push_back(mCpu.get());
	    mRoot->sScriptEngine()->add("rdtscPlugin_getCpuString", Cpu::getCpuString, VarArg(mCpu.get()));
	}else{
        NR_Log(Log::LOG_PLUGIN, "rdtscTimeSource: Your CPU seems not to support the rdtsc instruction");
    }	
	return 0;	
} 
  

//---------------------------------------------------------
// Get full string according to the plugin version
//---------------------------------------------------------
extern "C" _PluginExport char* plgVersionString( void )
{
    return "High Performance Time Source v0.1 (rdtsc & cpuid instruction using)";
}

//---------------------------------------------------------
// Get the engines version number for which this plugin was wrote
//---------------------------------------------------------
extern "C" unsigned int plgEngineVersion( void )
{
	return createVersionInteger(0,0,8);
}


//---------------------------------------------------------
// Return a string according to the given error code
//---------------------------------------------------------
extern "C" char* plgError(int err)
{
	return "Unknown";
}


//---------------------------------------------------------
// Release the plugin
//---------------------------------------------------------
extern "C" void plgRelease( void )
{
	// free pointers
	mCpu.reset();
	mTimeSource.reset();

	// set the old time source to the clock back
	mRoot->sClock()->setTimeSource(mOldTimeSource);
    mRoot->sClock()->setSyncInterval(0);
		
	// some log
	NR_Log(Log::LOG_PLUGIN, "rdtscTimeSource: Set old time source back and good bye!");
}

