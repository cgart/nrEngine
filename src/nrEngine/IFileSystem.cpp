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
#include <nrEngine/IFileSystem.h>

namespace nrEngine {

	//----------------------------------------------------------------------------------
	IFileSystem::IFileSystem()
	{

	}

	//----------------------------------------------------------------------------------
	IFileSystem::~IFileSystem()
	{

	}
		
	//----------------------------------------------------------------------------------
	Result IFileSystem::set(uint32 param, const std::string& value)
	{
		if (param >= mParameter.size()){
			return VFS_NO_PARAMETER;
		}

		//mParameter.set(param, value);
		
		return OK;
	}
	
	//----------------------------------------------------------------------------------
	Result IFileSystem::set(const std::string& name, const std::string& value)
	{
		//if (!mParameter.exists(name))
		//	return VFS_NO_PARAMETER;

		//mParameter[name] = value;
		return OK;
	}
		
};

