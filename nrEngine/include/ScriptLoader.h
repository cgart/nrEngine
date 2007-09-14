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


#ifndef _NR_SCRIPT_RESOURCE_LOADER_H_
#define _NR_SCRIPT_RESOURCE_LOADER_H_

//----------------------------------------------------------------------------------
// Includes
//----------------------------------------------------------------------------------
#include "Prerequisities.h"
#include "ResourceLoader.h"
#include "IScript.h"
#include "Script.h"

namespace nrEngine{
	
	//! Script loader is used to create/load engine's script objects
	/**
	* Scriptloader is used to load standard scripts based on the engine's language.
	* Standard scripts are very simple and at the time they can only be used
	* to load other script languages having more functionality.
	* 
	* @see IResourceLoader
	* \ingroup script
	**/
	class _NRExport ScriptLoader : public IResourceLoader{
		public:
				
			/**
			* Declare supported resource types and supported files.
			**/
			ScriptLoader();
			
			/**
			* Destructor 
			**/
			~ScriptLoader();
		
		private:
			/**
			 * Initialize supported resource and file types for the resource script
			 **/
			Result initializeResourceLoader();
			
			/**
			* Load the script resource.
			*
			* If a script is loaded then it will be added to the kernel as a task,
			* because scripts are task executed in the kernel.
			* If you write your own script-loader (i.e. different script language),
			* so you have to worry about the adding the script into the kernel .
			**/
			Result loadResource(IResource* res, const std::string& fileName, PropertyList* param = NULL);
	
			/**
			* Create an empty script resource. The resource represents a script
			* which does have no effect.
			*
			* @param resourceType Unique name of the resource type to be created
			* @return Instance of empty resource
			**/
			IResource* createEmptyResource(const std::string& resourceType);
			
			/**
			* @see IResourceLoader::createResourceImpl()
			**/
			IResource* createResource(const std::string& resourceType, PropertyList* params = NULL);
			
	};

};

#endif
