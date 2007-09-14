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
#ifndef _NR__RESOURCES_SYSTEM_H
#define _NR__RESOURCES_SYSTEM_H

/*!
 * \defgroup resource Resource managment
 *
 * <b>nrEngine</b> has support for resource managment. Each resource can be
 * loaded or unloaded at the runtime. There is a class for generic support
 * of resource managment system ResourceManager. This class provides generic
 * functions used for resource reservation/allocation/release. We also provides
 * a possibility to unload the resource at runtime but not to delete it from
 * the memory completly. So the application can still access to it and will
 * get only empty resource back.
 * 
 * Each resource class has to be derived from IResource and provide
 * their own functions for returning an empty resource. ResourcePtr is a
 * class representing smart pointer to any resource. This pointers can be
 * used as normal pointers and will give you transparent access to empty
 * item if this resource is not present in the memory.
 *
 * So with the help of
 * such subsystem you do not have to check each time you want to use the
 * data if it still exists in the memory. This happens transparent to you and
 * will stay efficient.
 *
 * <img src="../dias/Resources.png">
 **/

 

//----------------------------------------------------------------------------------
// Includes
//----------------------------------------------------------------------------------
#include "Prerequisities.h"
#include "Priority.h"
#include "Property.h"

//----------------------------------------------------------------------------------
// Types
//----------------------------------------------------------------------------------
namespace nrEngine{
		
	//! Smart pointer pointing to a loader
	typedef SharedPtr<IResourceLoader> ResourceLoader;
	
	//! This handle will store something like resource id.
	typedef uint32					ResourceHandle;

	//! Cast a resource loader of basis type to any derived type
	template<class T>
	SharedPtr<T> ResourceLoaderCast (ResourceLoader loader)
	{
		return boost::dynamic_pointer_cast<T, IResourceLoader>(loader);
	}
};


//----------------------------------------------------------------------------------
// Includes for teh system
//----------------------------------------------------------------------------------
#include "ResourceManager.h"
#include "Resource.h"
#include "ResourceHolder.h"
#include "ResourceLoader.h" 
#include "ResourcePtr.h"


#endif
