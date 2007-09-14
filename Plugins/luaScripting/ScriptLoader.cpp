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
#include "ScriptLoader.h"
#include "Script.h"

//----------------------------------------------------------------------------------
LuaScriptLoader::LuaScriptLoader() : IResourceLoader("LuaScriptLoader")
{
	initializeResourceLoader();
}


//----------------------------------------------------------------------------------
LuaScriptLoader::~LuaScriptLoader()
{

}

//----------------------------------------------------------------------------------
Result LuaScriptLoader::initializeResourceLoader(){
	
	// fill out supported resource types;
	declareSupportedResourceType("LuaScript");
	declareSupportedResourceType("Lua");

	// we do only support dll files in windows version of our engine
	declareSupportedFileType("lua");
	declareSupportedFileType("luascript");
	declareSupportedFileType("lscript");

	declareTypeMap("lua", "LuaScript");
	declareTypeMap("luascript", "LuaScript");
	declareTypeMap("lscript", "LuaScript");
	
	return OK;
}


//----------------------------------------------------------------------------------
Result LuaScriptLoader::loadResource(IResource* res, const std::string& fileName, PropertyList* param)
{

	// load a file so we use its content as a script
	FileStream* fStream = new FileStream();
	Result ret = fStream->open(fileName);
	if (ret == OK){

		// get the whole file content
		std::string str = fStream->getAsString();

		// cast the resource to the iscript interface
		IScript* scr = dynamic_cast<IScript*>(res);

		// load the script from a string
		ret = scr->loadFromString(str);
	}
	delete fStream;

	// return the last error
	return ret;
}


//----------------------------------------------------------------------------------
IResource* LuaScriptLoader::createResource(const ::std::string& resourceType, PropertyList* params)
{
	// create an plugin instance
	return new LuaScript();
}


//----------------------------------------------------------------------------------
IResource* LuaScriptLoader::createEmptyResource(const ::std::string& resourceType)
{
	// create an instance of empty plugin
	return new EmptyScript(resourceType);
}


