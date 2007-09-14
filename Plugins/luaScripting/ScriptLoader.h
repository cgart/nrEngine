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


#ifndef _NR_LUA_LOADER_H_
#define _NR_LUA_LOADER_H_

//----------------------------------------------------------------------------------
// Includes
//----------------------------------------------------------------------------------
#include <nrEngine/nrEngine.h>

using namespace nrEngine;
	
/**
* Scriptloader is used to load standard scripts based on the engine's language.
* Standard scripts are very simple and at the time they can only be used
* to load other script languages having more functionality.
*
* @see IResourceLoader
* \ingroup script
**/
class LuaScriptLoader : public IResourceLoader{
public:

	/**
	* Declare supported resource types and supported files.
	**/
	LuaScriptLoader();

	/**
	* Destructor
	**/
	~LuaScriptLoader();

	private:
		
	/**
	* Initialize supported resource and file types for the resource script
	**/
	Result initializeResourceLoader();

	/**
	* Load the script resource.
	**/
	Result loadResource(IResource* res, const std::string& fileName, PropertyList* param = NULL);

	/**
	* Create an empty script resource. The resource represents a script
	* which does have no effect.
	**/
	IResource* createEmptyResource(const ::std::string& resourceType);


	/**
	* Create an instance of script object.
	**/
	IResource* createResource(const std::string& resourceType, PropertyList* params = NULL);

};

#endif
