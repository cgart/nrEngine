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
#include "ResourceLoader.h"
#include "Log.h"
#include "Exception.h"
#include "Engine.h"
#include "ResourceManager.h"

namespace nrEngine{

	//----------------------------------------------------------------------------------
	IResourceLoader::IResourceLoader(const std::string& name) : mName(name)
	{

	}

	//----------------------------------------------------------------------------------
	IResourceLoader::~IResourceLoader()
	{
		// copy the list in another list, so we can iterate and remove them
		std::vector<SharedPtr<IResource> > lst;
		for (ResourceList::iterator it = mHandledResources.begin(); it != mHandledResources.end(); it ++)
			lst.push_back(*it);

		// now go through the vector and remove elements
		for (uint32 i=0; i < lst.size(); i++)
			remove(lst[i]);
	}

	//----------------------------------------------------------------------------------
	SharedPtr<IResource> IResourceLoader::load(const std::string& name, const std::string& group, const std::string& fileName, const std::string& resourceType, PropertyList* param)
	{
		// check if a such resource already registered by the manager
		if (Engine::sResourceManager()->isResourceRegistered(name))
		{
			NR_Log(Log::LOG_ENGINE, Log::LL_ERROR, "ResourceLoader: You are trying to load resource %s which is already loaded.", name.c_str());
			return SharedPtr<IResource>();
		}

		bool typeFound = false;
		std::string type;
		std::string newFileName = fileName;
		
		// we search for the type if no type is specified
		if (resourceType.length() == 0)
		{
			NR_Log(Log::LOG_ENGINE, Log::LL_DEBUG, "ResourceLoader \"%s\" try to find according resource type by filename \"%s\"", mName.c_str(), fileName.c_str(), fileName.c_str());

			// detect the file type by reading out it's last characters
			for (int32 i = fileName.length()-1; i >= 0 && !typeFound; i--){
				if ((char)fileName[i] == '.'){
					typeFound = true;
				}
				if (!typeFound) type = (char)fileName[i] + type;
			}
			if (!typeFound) type = "";

			// check whenever the given file name is supported by the resource
			if (typeFound && !supportFileType(type))
			{
				NR_Log(Log::LOG_ENGINE, Log::LL_ERROR, "ResourceLoader \"%s\" can not load file of type \".%s\", because the type is not supported", mName.c_str(), type.c_str());
				return SharedPtr<IResource>();
			}

			#if 0
			// if no type foud, then check if suffix is defined
			if (!typeFound && resourceType.length() > 0)
			{
				type = getSuffix(resourceType);
				if (type.length() > 0)
				{	
					typeFound = true;
					newFileName = fileName + std::string(".") + type;
				}
			}
			#endif
						
			// if no type was specified so give up
			if (!typeFound)
			{
				NR_Log(Log::LOG_ENGINE, Log::LL_ERROR, "ResourceLoader %s: neither resource type nor valid file ending was found, give up!", mName.c_str());
				return SharedPtr<IResource>();
			}
		}

		// ok the name is supported, so now create a instance
		SharedPtr<IResource> res;
		if (resourceType.length() == 0){
			res = create(mapFileTypeToResourceType(type), param);
		}else{
			res = create(resourceType, param);
		}
		if (res.get() == NULL) return res;

		// setup some data on resource
		res->addResourceFilename(newFileName);
		res->mResName = name;
		res->mResGroup = group;

		// now call the implemented loading function
		if (loadResource(res.get(), newFileName, param) != OK)
		{
			NR_Log(Log::LOG_ENGINE, Log::LL_ERROR, "ResourceLoader %s can not load resource from file %s", mName.c_str(), newFileName.c_str());
			remove(res);
			return SharedPtr<IResource>();
		}
		res->mResIsLoaded = true;

		// now notify the resource manager, that a new resource was loaded
		Engine::sResourceManager()->notifyLoaded(res.get());

		return res;
	}

	//----------------------------------------------------------------------------------
	SharedPtr<IResource> IResourceLoader::create(const std::string& resourceType, PropertyList* params)
	{
		NR_Log(Log::LOG_ENGINE, Log::LL_DEBUG, "ResourceLoader: Create resource of type %s", resourceType.c_str());

		// first check if this type of resource is supported
		if (!supportResourceType(resourceType))
		{
			NR_Log(Log::LOG_ENGINE, Log::LL_ERROR, "ResourceLoader %s does not support resources of type %s", mName.c_str(), resourceType.c_str());
			return SharedPtr<IResource>();
		}

		// now call the implemented function to create the resource
		SharedPtr<IResource> res (createResource(resourceType, params), IResource::_deleter());

		// check if the result is valid, then add it into our database
		if (res.get() == NULL) return SharedPtr<IResource>();

		// now check if we have to create an empty resource for this type
		SharedPtr<IResource> empty = Engine::sResourceManager()->getEmpty(resourceType);

		// if not created before, so create it and register by the manager
		if (empty == NULL)
		{
			// create empty resource and fill default properties
			empty.reset( createEmptyResource(resourceType), IResource::_deleter() );
			empty->mResGroup = "_Empty_Resource_Group_";
			empty->mResName = "_Empty_" + resourceType;
			empty->mResHandle = Engine::sResourceManager()->getNewHandle();
			empty->mResIsEmpty = true;
			empty->mResLoader = getSharedPtrFromThis();
			empty->setResourceType(resourceType);
			
			// set the resource in the manager
			Engine::sResourceManager()->setEmpty(resourceType, empty);
		}

		// setup default resource data
		res->setResourceType(resourceType);
		res->mResHandle = Engine::sResourceManager()->getNewHandle();
		res->mResIsEmpty = false;
		res->mResLoader = getSharedPtrFromThis();
		
		// now set this resource in the list of handled resource objects
		mHandledResources.push_back(res);

		// ok now return the instance
		return res;
	}

	//----------------------------------------------------------------------------------
	SharedPtr<IResource> IResourceLoader::create(const std::string& name, const std::string& group, const std::string& resourceType, PropertyList* params)
	{
		// first check if this type of resource is supported
		if (!supportResourceType(resourceType))
		{
			NR_Log(Log::LOG_ENGINE, Log::LL_ERROR, "ResourceLoader %s does not support resources of type %s", mName.c_str(), resourceType.c_str());
			return SharedPtr<IResource>();
		}

		// now check if such a resource is already in the database
		IResourcePtr res = Engine::sResourceManager()->getByName(name);
		if (!res.isNull()){
			NR_Log(Log::LOG_ENGINE, Log::LL_WARNING, "You are trying to create a resource %s of type %s which is already in the database", name.c_str(), resourceType.c_str());
			return SharedPtr<IResource>();
		}

		// ok now create the resource
		SharedPtr<IResource> r = create(resourceType, params);
		if (r.get() == NULL) return SharedPtr<IResource>();

		// setup values
		r->mResName = name;
		r->mResGroup = group;

		// now let manager know about the new resource
		Engine::sResourceManager()->notifyCreated(r.get());

		return r;
	}
#if 0
	//----------------------------------------------------------------------------------
	Result IResourceLoader::unload(SharedPtr<IResource> resource)
	{
		// empty resource could not be unloaded
		if (resource.get() == NULL) return OK;

		// call the implementation of the unloader
		if (resource->mResIsLoaded){

			Result ret = unloadResourceImpl(resource.get());
			if (ret != OK) return ret;

			// now mark the resource that it has been unloaded
			resource->mResIsLoaded = false;

			// notify the resource manager about unloading the resource
			Engine::sResourceManager()->notifyUnloaded(resource.get());
		}

		return OK;
	}

	//----------------------------------------------------------------------------------
	Result IResourceLoader::reload(SharedPtr<IResource> resource)
	{
		// check if we are the handler for this resource
		if (std::find(mHandledResources.begin(), mHandledResources.end(), resource) == mHandledResources.end()){
			NR_Log(Log::LOG_ENGINE, Log::LL_WARNING, "ResourceLoader: You are trying to reload resource %s not handled by this loader %s", resource->getResName().c_str(), mName.c_str());
			return OK;
		}

		// unload the resource
		unload(resource);

		// call the implementation of the unloader
		if (resource->mResIsLoaded == false)
		{

			Result ret = reloadResourceImpl(resource.get());
			if (ret != OK) return ret;

			// now mark the resource that it has been unloaded
			resource->mResIsLoaded = true;

			// notify the resource manager about unloading the resource
			Engine::sResourceManager()->notifyLoaded(resource.get());
		}

		return OK;
	}
#endif
	//----------------------------------------------------------------------------------
	Result IResourceLoader::remove(SharedPtr<IResource> resource)
	{
		// remove only valid resources
		if (resource.get() == NULL) return OK;

		// emove only handled resources
		if (std::find(mHandledResources.begin(), mHandledResources.end(), resource) == mHandledResources.end())
		{
			NR_Log(Log::LOG_ENGINE, Log::LL_WARNING, "ResourceLoader: You are trying to remove resource %s not handled by this loader %s!", resource->getResourceName().c_str(), mName.c_str());
			return OK;
		}

		// unload resource
		resource->unload();

		// notify to remove the resource
		notifyRemoveResource(resource);

		// reset the resource, so it gets deleted, if it is not referenced elsewhere
		resource.reset();

		return OK;
	}

	//----------------------------------------------------------------------------------
	void IResourceLoader::notifyRemoveResource(SharedPtr<IResource> res)
	{
		ResourceList::iterator resit = std::find(mHandledResources.begin(), mHandledResources.end(), res);

		// check if such resource is loaded
		if (resit != mHandledResources.end() )
		{
			// ok remove the resource from the handled resources
			mHandledResources.erase(resit);

			// notify the manager about removing the resource
			Engine::sResourceManager()->notifyRemove(res.get());
		}
	}

#if 0

	//----------------------------------------------------------------------------------
	void IResourceLoader::notifyUnloadResource(SharedPtr<IResource> res)
	{
		if (res == NULL) return;
		if (std::find(mHandledResources.begin(), mHandledResources.end(), res) != mHandledResources.end() && res->mResIsLoaded){
			Engine::sResourceManager()->notifyUnloaded(res.get());
		}
	}

	//----------------------------------------------------------------------------------
	Result IResourceLoader::unloadResourceImpl(IResource* res)
	{
		if (res == NULL) return OK;
		if (res->mResIsLoaded) return res->unloadRes();
		return OK;
	}

	//----------------------------------------------------------------------------------
	Result IResourceLoader::reloadResourceImpl(IResource* res)
	{
		if (res == NULL) return OK;
		/*Result ret = loadResourceImpl(res, res->getResFileName(), NULL);
		if (ret != OK){
			NR_Log(Log::LOG_ENGINE, Log::LL_ERROR, "ResourceLoader %s can not load resource from file %s", mName.c_str(), res->getResFileName().c_str());
			return ret;
		}*/
		if (res->mResIsLoaded == false) res->reloadRes();
		//res->mResIsLoaded = true;

		// now notify the resource manager, that a new resource was loaded
		//Engine::sResourceManager()->notifyLoaded(res);

		return OK;
	}
#endif
	//----------------------------------------------------------------------------------
	bool IResourceLoader::supportResourceType(const std::string& resourceType) const {
		std::vector<std::string>::const_iterator it;
		for (it = mSupportedResourceTypes.begin(); it != mSupportedResourceTypes.end(); it++){
			if ((*it) == resourceType) return true;
		}
		return false;
	}

	//----------------------------------------------------------------------------------
	bool IResourceLoader::supportFileType(const std::string& fileType) const {
		std::vector<std::string>::const_iterator it;
		for (it = mSupportedFileTypes.begin(); it != mSupportedFileTypes.end(); it++){
			if ((*it) == fileType) {
				return true;
			}
		}
		return false;
	}

};

