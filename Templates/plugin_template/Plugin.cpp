

#include <nrEngine.h>

using namespace nrEngine;

//---------------------------------------------------------
// Some globals
//---------------------------------------------------------
Engine* 							mRoot = NULL;

//---------------------------------------------------------
// Initialize the plugin
//---------------------------------------------------------
extern "C" int plgInitialize(Engine* root)
{
	
	mRoot = root;
	
	return 0;	
}
 

//---------------------------------------------------------
// Get the engines version number for which this plugin was wrote
//---------------------------------------------------------
extern "C" unsigned int plgEngineVersion( void )
{
	return NR_createVersionInteger(0,0,5);
}


//---------------------------------------------------------
// Get full string according to the plugin version
//---------------------------------------------------------
extern "C" char* plgVersionString( void )
{
	return "PLUGIN Version v0.1";
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

}
