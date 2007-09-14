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
#include <nrEngine/ScriptLoader.h>
#include <nrEngine/Log.h>
#include <nrEngine/FileStream.h>
#include <nrEngine/Kernel.h>

namespace nrEngine{

	//----------------------------------------------------------------------------------
	ScriptLoader::ScriptLoader() : IResourceLoader("ScriptLoader")
	{
		initializeResourceLoader();
	}


	//----------------------------------------------------------------------------------
	ScriptLoader::~ScriptLoader()
	{

	}

	//----------------------------------------------------------------------------------
	Result ScriptLoader::initializeResourceLoader()
	{
		// fill out supported resource types;
		declareSupportedResourceType("Script");
		declareSupportedResourceType("Config");
		declareSupportedResourceType("nrScript");

		// we do only support dll files in windows version of our engine
		declareSupportedFileType("script");
		declareSupportedFileType("nrscript");
		declareSupportedFileType("cfg");

		// now delcare mapping of the types
		declareTypeMap("nrscript", "nrScript");
		declareTypeMap("script", "nrScript");
		declareTypeMap("cfg", "Config");

		return OK;
	}
	
	//----------------------------------------------------------------------------------
	Result ScriptLoader::loadResource(IResource* res, const std::string& fileName, PropertyList* param)
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
	IResource* ScriptLoader::createResource(const std::string& resourceType, PropertyList* params)
	{
		// check if we want to create just a script
		if (resourceType == "Script" || resourceType == "nrScript")
			return new Script();
		else if (resourceType == "Config"){
			// config scripts does run completely not stepwise
			Script* scr = new Script();
			scr->setRunStepwise(false);
			return scr;
		}
		return NULL;
	}


	//----------------------------------------------------------------------------------
	IResource* ScriptLoader::createEmptyResource(const std::string& resourceType)
	{
		// create an instance of empty plugin
		return new EmptyScript(resourceType);
	}


};

