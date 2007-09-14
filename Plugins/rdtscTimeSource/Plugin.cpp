

#include <nrEngine.h>
#include <boost/shared_ptr.hpp>

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


//---------------------------------------------------------
// Initialize the plugin
//---------------------------------------------------------
extern "C" int plgInitialize(Engine* root)
{
	
	mRoot = root;
	
	// create cpu instance
	mCpu.reset(new Cpu());
	NR_Log(Log::LOG_PLUGIN, "rdtscTimeSource: Retrieve CPU Information");
	mCpu->detect();
	NR_Log(Log::LOG_PLUGIN, "rdtscTimeSource: %s", mCpu->getCPUDescription().c_str());
	
	// create Time Source
	mTimeSource.reset( new rdtscTimeSource(mCpu) );

	// get the current time source
	mOldTimeSource = mRoot->sClock()->getTimeSource();
		
	// now set new time source to the clock
	NR_Log(Log::LOG_PLUGIN, "rdtscTimeSource: Set the high performance time source for the clock");
	mRoot->sClock()->setTimeSource(mTimeSource);

	// declare new function to the scripting engine
	//std::vector<ScriptParam> param;
	//param.push_back(mCpu.get());
	mRoot->sScriptEngine()->add("getCpuString", Cpu::getCpuString, VarArg(mCpu.get()));
		
	return 0;	
} 
  

//---------------------------------------------------------
// Get the engines version number for which this plugin was wrote
//---------------------------------------------------------
extern "C" unsigned int plgEngineVersion( void )
{
	return createVersionInteger(0,0,5);
}


//---------------------------------------------------------
// Get full string according to the plugin version
//---------------------------------------------------------
extern "C" char* plgVersionString( void )
{
	return "High Performance Time Source v0.1 (rdtsc & cpuid instruction using)";
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
		
	// some log
	NR_Log(Log::LOG_PLUGIN, "rdtscTimeSource: Set old time source back and good bye!");
}

//---------------------------------------------------------
// Get information about provided subroutines
//---------------------------------------------------------
/*extern "C" void plgGetMethods( std::vector<Plugin::PlgMethod>& methods)
{
	methods = mMethods;
}


//---------------------------------------------------------
// Call certain subroutine 
//---------------------------------------------------------
extern "C" void plgCall(const std::string& name, std::vector<Plugin::PlgParam>& params)
{

	if (name == "getCpuInfo"){
		getCpuInfoString();
	}
	
}*/

