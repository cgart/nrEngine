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


#ifndef _NR_FILESTREAM_RESOURCE_LOADER_H_
#define _NR_FILESTREAM_RESOURCE_LOADER_H_

//----------------------------------------------------------------------------------
// Includes
//----------------------------------------------------------------------------------
#include "Prerequisities.h"
#include "ResourceLoader.h"
#include "FileStream.h"


namespace nrEngine{

	//! File stream loader is able to instantiate file stream objects
	/**
	* @see IResourceLoader
	* \ingroup vfs
	**/
	class _NRExport FileStreamLoader : public IResourceLoader{
	public:

		/**
		* Declare supported resource types and supported files.
		**/
		FileStreamLoader();

		/**
		* Destructor
		**/
		~FileStreamLoader();

		private:
			
			/**
			* Initialize supported resource and file types
			* @see IResourceLoader::initializeResourceLoader()
			**/
			Result initializeResourceLoader();
	
			/**
			* @see IResourceLoader::loadResource()
			**/
			Result loadResource(IResource* res, const std::string& fileName, PropertyList* param = NULL);
	
			/**
			* @see IResourceLoader::createResource()
			**/
			IResource* createResource(const std::string& resourceType, PropertyList* params = NULL);
				
			/**
			* @see IResourceLoader::createResourceEmpty()
			**/
			IResource* createEmptyResource(const std::string& resourceType);

	};

};

#endif
