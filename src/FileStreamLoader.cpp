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
#include "FileStreamLoader.h"
#include "Log.h"

namespace nrEngine{

	//----------------------------------------------------------------------------------
	FileStreamLoader::FileStreamLoader() : IResourceLoader("FileStreamLoader")
	{
		initializeResourceLoader();
	}


	//----------------------------------------------------------------------------------
	FileStreamLoader::~FileStreamLoader()
	{

	}

	//----------------------------------------------------------------------------------
	Result FileStreamLoader::initializeResourceLoader()
	{
		
		// fill out supported resource types;
		declareSupportedResourceType("File");

		return OK;
	}
	
	//----------------------------------------------------------------------------------
	Result FileStreamLoader::loadResource(IResource* res, const std::string& fileName, PropertyList* param)
	{
		// create a pointer to the stream object and open the file
		FileStream* fileStream = dynamic_cast<FileStream*>(res);
		return fileStream->open(fileName);
	}


	//----------------------------------------------------------------------------------
	IResource* FileStreamLoader::createResource(const std::string& resourceType, PropertyList* params)
	{
		// create an plugin instance
		return new FileStream();
	}


	//----------------------------------------------------------------------------------
	IResource* FileStreamLoader::createEmptyResource(const std::string& resourceType)
	{
		// create an instance of empty plugin
		return new EmptyFileStream();
	}

};

