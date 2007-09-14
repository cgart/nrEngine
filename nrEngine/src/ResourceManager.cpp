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
#include "ResourceManager.h"
#include "Log.h"
#include "Exception.h"
#include "Engine.h"

namespace nrEngine{

	//----------------------------------------------------------------------------------
	ScriptFunctionDec(scriptLoadResource, ResourceManager)
	{
		// check parameter count
		if (args.size() < 3){
			return ScriptResult(std::string("Not valid parameter count! Parameters (name,group,filename [,type])"));
		}

		std::string type;
		if (args.size() == 5 ) type = args[4];

		// load a resource with the given name in a group from a file
		if (Engine::sResourceManager()->loadResource(args[1], args[2], args[3], type).isNull())
		{
			char msg[128];
			sprintf(msg, "There was an error by loading a resource %s from %s", args[1].c_str(), args[3].c_str());
			return ScriptResult(std::string(msg));
		}

		return ScriptResult();
	}

	//----------------------------------------------------------------------------------
	ScriptFunctionDec(scriptUnloadResource, ResourceManager)
	{
		// check parameter count
		if (args.size() < 1){
			return ScriptResult(std::string("Not valid parameter count! You should specify (name)"));
		}

		// unload resource and check for return code
		if (Engine::sResourceManager()->unload(args[1]) != OK)
		{
			char msg[128];
			sprintf(msg, "Could not unload %s", args[1].c_str());
			return ScriptResult(std::string(msg));
		}

		return ScriptResult();
	}

	//----------------------------------------------------------------------------------
	ResourceManager::ResourceManager(){
		mLastHandle = 1;

		// register functions by scripting engine
		Engine::sScriptEngine()->add("loadResource", scriptLoadResource);
		Engine::sScriptEngine()->add("unloadResource", scriptUnloadResource);

	}


	//----------------------------------------------------------------------------------
	ResourceManager::~ResourceManager(){

		// remove registered functions
		Engine::sScriptEngine()->del("loadResource");
		Engine::sScriptEngine()->del("unloadResource");

		// unload all resources
		removeAllRes();

		// remove all loaders
		removeAllLoaders();

	}

	//----------------------------------------------------------------------------------
	void ResourceManager::removeAllRes()
	{

		// iterate through each resource and remove it
		/*res_hdl_map::const_iterator it;

		for (it = mResource.begin(); it != mResource.end(); it++){
			ResourceHandle hdl = it->first;
			remove(hdl);
		}
		mResource.clear();
		*/

		// iterate through each empty resource and remove it
		res_empty_map::iterator jt;

		for (jt = mEmptyResource.begin(); jt != mEmptyResource.end(); jt++){
			SharedPtr<IResource>& res = jt->second;

			NR_Log(Log::LOG_ENGINE, Log::LL_DEBUG, "ResourceManager: Remove empty resource of type %s", res->getResourceType().c_str());
			res.reset();
			mEmptyResource.erase(jt);
		}
		mEmptyResource.clear();

	}

	//----------------------------------------------------------------------------------
	void ResourceManager::removeAllLoaders(){

		// iterate through all loaders
		loader_map::const_iterator it;

		for (it = mLoader.begin(); it != mLoader.end(); it++){
			removeLoader(it->first);
		}

	}

	//----------------------------------------------------------------------------------
	/*Result ResourceManager::setMemoryBudget(size_t bytes){
		mMemBudget = bytes;
		return checkMemoryUsage();
	}


	//----------------------------------------------------------------------------------
	size_t ResourceManager::getMemoryBudget() const{
		return mMemBudget;
	}

	//----------------------------------------------------------------------------------
	size_t ResourceManager::getMemoryUsage() const{
		return mMemUsage;
	}*/

	//----------------------------------------------------------------------------------
	Result ResourceManager::registerLoader(const std::string& name, ResourceLoader loader){

		// check whenver such a loader already exists
		if (mLoader.find(name) != mLoader.end()){
			NR_Log(Log::LOG_ENGINE, "ResourceManager: %s loader already registered", name.c_str());
			return RES_LOADER_ALREADY_EXISTS;
		}

		// check for bad parameters
		if (loader == NULL || name.length() == 0){
			return BAD_PARAMETERS;
		}

		NR_Log(Log::LOG_ENGINE, "ResourceManager: Register new resource loader %s", name.c_str());

		// register the loader
		mLoader[name] = loader;

		// Give some log information about supported filetypes and resource types
		try{
			std::vector< std::string>::const_iterator it;
			std::string strTemp;

			const std::vector< std::string>&	fileTypes = loader->getSupportedFileTypes();
			for (it = fileTypes.begin(); it != fileTypes.end(); it++){
				strTemp += (*it) + " ";
			}
			NR_Log(Log::LOG_ENGINE, "ResourceManager: Found loader \"%s\" for file types %s", name.c_str(), strTemp.c_str());
			strTemp = "";

			const std::vector< std::string>&	resTypes = loader->getSupportedResourceTypes();
			for (it = resTypes.begin(); it != resTypes.end(); it++){
				strTemp += (*it) + " ";
			}
			NR_Log(Log::LOG_ENGINE, "ResourceManager: Found loader \"%s\" for resource types %s", name.c_str(), strTemp.c_str());
		}catch(...){
			return BAD_PARAMETERS;
		}

		return OK;
	}

	//----------------------------------------------------------------------------------
	Result ResourceManager::removeLoader(const std::string& name){

		// get id of the loader
		loader_map::iterator jt = mLoader.find(name);
		if (jt == mLoader.end()){
			return RES_LOADER_NOT_REGISTERED;
		}

		NR_Log(Log::LOG_ENGINE, "ResourceManager: Remove loader %s", name.c_str());

		mLoader.erase(name);

		return OK;
	}

	//----------------------------------------------------------------------------------
	ResourceLoader ResourceManager::getLoaderByFile(const std::string& fileType){

		if (fileType.length() == 0) return ResourceLoader();

		// scan through all loaders and ask them if they do support this kind of file type
		loader_map::const_iterator it;

		for (it = mLoader.begin(); it != mLoader.end(); it++){
			if (it->second->supportFileType(fileType)){
				return it->second;
			}
		}

		// Give some log information
		NR_Log(Log::LOG_ENGINE, Log::LL_WARNING, "ResourceManager: There is no loader for *.%s files", fileType.c_str());

		return ResourceLoader();
	}

	//----------------------------------------------------------------------------------
	ResourceLoader ResourceManager::getLoader(const std::string& name){

		loader_map::iterator it = mLoader.find(name);

		if (it == mLoader.end()){
			return ResourceLoader();
		}

		return mLoader[name];
	}

	//----------------------------------------------------------------------------------
	ResourceLoader ResourceManager::getLoaderByResource(const std::string& resType){

		// scan through all loaders and ask them if they do support this kind of file type
		loader_map::const_iterator it;

		for (it = mLoader.begin(); it != mLoader.end(); it++){
			if (it->second->supportResourceType(resType)){
				return it->second;
			}
		}

		return ResourceLoader();

	}

	//----------------------------------------------------------------------------------
	IResourcePtr ResourceManager::createResource (const std::string& name, const std::string& group, const std::string& resourceType, PropertyList* params)
	{
		// check if such a resource already exists
		IResourcePtr pRes = getByName(name);
		if (!pRes.isNull()){
			NR_Log(Log::LOG_ENGINE, Log::LL_WARNING, "ResourceManager: Can not create new resource %s of type %s because such already exists", name.c_str(), resourceType.c_str());
			return pRes;
		}

		// Get appropriate loader/creator
		ResourceLoader creator = getLoaderByResource(resourceType);
		if (creator == NULL){
			NR_Log(Log::LOG_ENGINE, "ResourceManager: Can not create a resource %s because there is no loader for %s resource type", name.c_str(), resourceType.c_str());
			return IResourcePtr();
		}

		// now call the loader to create a resource
		SharedPtr<IResource> res (creator->create(name, group, resourceType, params));
		if (res.get() == NULL)
		{
			NR_Log(Log::LOG_ENGINE, "ResourceManager: Can not create a resource %s of %s type", name.c_str(), resourceType.c_str());
			return IResourcePtr();
		}

		NR_Log(Log::LOG_ENGINE, Log::LL_DEBUG, "ResourceManager: Create resource %s of type %s in group %s", name.c_str(), resourceType.c_str(), group.c_str());

		// get the holder for this resource, it must be there
		SharedPtr<ResourceHolder>& holder = *getHolderByName(name);
		NR_ASSERT(holder != NULL && "Holder must be valid here!");

		return IResourcePtr(holder);
	}


	//----------------------------------------------------------------------------------
	IResourcePtr ResourceManager::loadResource(
			const std::string& name,const std::string& group,const std::string& fileName,
			const std::string& resourceType,PropertyList* params,ResourceLoader loader)
	{

		NR_Log(Log::LOG_ENGINE, Log::LL_DEBUG, "ResourceManager: Load resource %s of type %s from file %s", name.c_str(), resourceType.c_str(), fileName.c_str());

		// check for right parameters
		if (name.length() == 0 || fileName.length() == 0)
			return IResourcePtr();

		// check whenever such a resource already exists
		IResourcePtr pRes = getByName(name);
		if (!pRes.isNull()){
			NR_Log(Log::LOG_ENGINE, Log::LL_WARNING, "ResourceManager: Resource %s already loaded. Do nothing.", name.c_str());
			return pRes;
		}

		// detect the file type by reading out it's last characters
		std::string type;
		for (int32 i = fileName.length()-1; i >= 0; i--){
			if (fileName[i] == '.'){
				break;
			}
			type = fileName[i] + type;
		}
		bool isManualSpecified = (loader != NULL);

		// if the loader is manually specified, so do nothing
		if (!isManualSpecified)
		{
			loader = getLoaderByResource(resourceType);
			if (loader == NULL) loader = getLoaderByFile(type);

			if (loader == NULL){
				NR_Log(Log::LOG_ENGINE, Log::LL_ERROR, "ResourceManager: valid loader for resource %s was not found or no manual loader was specified, give up!", name.c_str());
				return IResourcePtr();
			}
		}

		// now call the loader to create a resource
		// loader will notify the manager, manager will add it into database
		SharedPtr<IResource> res = loader->load(name, group, fileName, resourceType, params);
		if (res == NULL) return IResourcePtr();

		// get the holder for this resource, it must be there
		SharedPtr<ResourceHolder>& holder = *getHolderByName(name);
		NR_ASSERT(holder.get() != NULL && "Holder must be valid here!");

		return IResourcePtr(holder);
	}

	//----------------------------------------------------------------------------------
	Result ResourceManager::unload(const std::string& name){

		ResourcePtr<IResource> res = getByName(name);

		if (res.isNull()){
			return RES_NOT_FOUND;
		}

		lockResource(res);
			NR_Log(Log::LOG_ENGINE, "ResourceManager: Unload resource %s", res->getResourceName().c_str(), res->getResourceHandle());
			Result ret = res->unload();
		unlockResource(res);

		return ret;
	}

	//----------------------------------------------------------------------------------
	Result ResourceManager::unload(IResourcePtr& res){

		if (res.isNull()){
			return RES_NOT_FOUND;
		}

		lockResource(res);
			NR_Log(Log::LOG_ENGINE, "ResourceManager: Unload resource %s",
				res.getBase()->getResourceName().c_str(), res.getBase()->getResourceHandle());
			Result ret = res.getBase()->unload();
		unlockResource(res);

		return ret;
	}

	//----------------------------------------------------------------------------------
	Result ResourceManager::unload(ResourceHandle& handle){

		ResourcePtr<IResource> res = getByHandle(handle);

		if (res.isNull()){
			return RES_NOT_FOUND;
		}

		lockResource(res);
			NR_Log(Log::LOG_ENGINE, "ResourceManager: Unload resource %s",
				res->getResourceName().c_str(), res->getResourceHandle());
			Result ret = res->unload();
		unlockResource(res);

		return ret;
	}

	//----------------------------------------------------------------------------------
	Result ResourceManager::reload(const std::string& name){

		ResourcePtr<IResource> res = getByName(name);

		if (res.isNull()){
			return RES_NOT_FOUND;
		}

		lockResource(res);
			NR_Log(Log::LOG_ENGINE, Log::LL_DEBUG, "ResourceManager: Reload resource %s",
				res->getResourceName().c_str(), res->getResourceHandle());
			Result ret = res->reload();
		unlockResource(res);

		return ret;
	}

	//----------------------------------------------------------------------------------
	Result ResourceManager::reload(ResourceHandle& handle){

		ResourcePtr<IResource> res = getByHandle(handle);

		if (res.isNull()){
			return RES_NOT_FOUND;
		}

		lockResource(res);
			NR_Log(Log::LOG_ENGINE, Log::LL_DEBUG, "ResourceManager: Reload resource %s",
				res->getResourceName().c_str(), res->getResourceHandle());
			Result ret = res->reload();
		unlockResource(res);

		return ret;
	}

	//----------------------------------------------------------------------------------
	Result ResourceManager::reload(IResourcePtr& res){

		if (res.isNull()){
			return RES_NOT_FOUND;
		}

		lockResource(res);
			NR_Log(Log::LOG_ENGINE, Log::LL_DEBUG, "ResourceManager: Reload resource %s",
				res.getBase()->getResourceName().c_str(), res.getBase()->getResourceHandle());
			Result ret = res.getBase()->reload();
		unlockResource(res);

		return ret;
	}

	//----------------------------------------------------------------------------------
/*	Result ResourceManager::removeImpl(IResourcePtr& resPtr){

		NR_ASSERT(!resPtr.isNull() && "Given pointer shouldn't be zero");

		// we need to lock the resource, because we need the real resource object not empty
		lockResource(resPtr);

			ResourcePtr<IResource> ptr = resPtr;
			ResourceHandle hdl = ptr->getResourceHandle();
			std::string name = ptr->getResourceName();
			std::string grp = ptr->getResourceGroup();

			if (hdl == 0){
				NR_Log(Log::LOG_ENGINE, "ResourceManager: Can not remove resource %s because it is not in the database", name.c_str());
				return RES_NOT_FOUND;
			}

			// first unload it
			unload(resPtr);

			NR_Log(Log::LOG_ENGINE, "ResourceManager: Remove resource %s (%s)", name.c_str(), grp.c_str());

			// remove the handle from the group list
			if (grp.length() > 0){
				res_grp_map::iterator jt = mResourceGroup.find(grp);
				if (jt != mResourceGroup.end()){
					jt->second.remove(hdl);

					// check whenever group contains nomore elements, and delete it
					if (jt->second.size() == 0){
						mResourceGroup.erase(grp);
						NR_Log(Log::LOG_ENGINE, "ResourceManager: %s's group \"%s\" does not contain elements, so remove it", name.c_str(), grp.c_str());
					}
				}else{
					NR_Log(Log::LOG_ENGINE, Log::LL_WARNING, "ResourceManager: There is no group in the database according to resource's group name \"%s\"", grp.c_str());
				}
			}

		// unlock it back
		unlockResource(resPtr);

		// clear the database
		mResourceName.erase(name);
		mResource.erase(hdl);

		return OK;
	}
*/
	//----------------------------------------------------------------------------------
	Result ResourceManager::remove(const std::string& name){

		// check whenever such a resource exists
		IResourcePtr ptr = getByName(name);

		if (ptr.isNull()){
			NR_Log(Log::LOG_ENGINE, "ResourceManager: Remove resource %s not found", name.c_str());
			return RES_NOT_FOUND;
		}

		lockResource(ptr);
			Result ret = ptr.getBase()->remove();
		unlockResource(ptr);

		return ret;
	}

	//----------------------------------------------------------------------------------
	Result ResourceManager::remove(ResourceHandle& handle){

		// check whenever such a resource exists
		IResourcePtr ptr = getByHandle(handle);

		if (ptr.isNull()){
			NR_Log(Log::LOG_ENGINE, "ResourceManager: Remove resource %d not found", handle);
			return RES_NOT_FOUND;
		}

		lockResource(ptr);
			Result ret = ptr.getBase()->remove();
		unlockResource(ptr);

		return ret;
	}

	//----------------------------------------------------------------------------------
	Result ResourceManager::remove(IResourcePtr& ptr){

		// check whenever such a resource exists
		if (!ptr.isNull()){
			lockResource(ptr);
				Result ret = ptr.getBase()->remove();
			unlockResource(ptr);

			return ret;
		}
		return OK;
	}

	//----------------------------------------------------------------------------------
	SharedPtr<ResourceHolder>* ResourceManager::getHolderByName(const std::string& name)
	{
		// find the handle
		res_str_map::iterator it = mResourceName.find(name);
		if (it == mResourceName.end()){
			return NULL;
		}

		// get through the handle the holder
		res_hdl_map::iterator jt = mResource.find(it->second);
		NR_ASSERT(jt != mResource.end() && "Fatal Error in the Database !!!");
		return &(jt->second);
	}

	//----------------------------------------------------------------------------------
	SharedPtr<ResourceHolder>* ResourceManager::getHolderByHandle(const ResourceHandle& handle)
	{
		// find through the handle
		res_hdl_map::iterator it = mResource.find(handle);
		if (it == mResource.end())
		{
			return NULL;
		}
		return &(it->second);
	}

	//----------------------------------------------------------------------------------
	IResourcePtr ResourceManager::getByName(const std::string& name){
		SharedPtr<ResourceHolder>* holder = getHolderByName(name);
		if (holder == NULL){
			return IResourcePtr();
		}

		return IResourcePtr(*holder);
	}

	//----------------------------------------------------------------------------------
	IResourcePtr ResourceManager::getByHandle(const ResourceHandle& handle){
		SharedPtr<ResourceHolder>* holder = getHolderByHandle(handle);
		if (holder == NULL){
			return IResourcePtr();
		}
		return IResourcePtr(*holder);
	}

	//----------------------------------------------------------------------------------
/*	Result ResourceManager::add(IResource* res, const std::string& name, const std::string& group)
	{

		// check for bad parameters
		if (res == NULL) return OK;
		if (name.length() == 0) return BAD_PARAMETERS;
		if (res->mResLoader == NULL) return RES_LOADER_NOT_EXISTS;

		// check whenever such resource already exists
		if (!getByName(name).isNull()){
			NR_Log(Log::LOG_ENGINE, "ResourceManager: WARNING: Add resource %s but it already exists", name.c_str());
			return OK;
		}

		ResourceHandle handle = mLastHandle++;
		NR_Log(Log::LOG_ENGINE, "ResourceManager: Add resource %s id=%d to the database from outside", name.c_str(), handle);

		// check now if a empty resource of that type already exists and load it if not
		SharedPtr<IResource> empty;
		checkEmptyResource(res->getResourceType(), empty, res->mResLoader);

		// create a holder for that resource
		SharedPtr<ResourceHolder> holder(new ResourceHolder());
		holder->resetRes(res);
		holder->setEmptyResource(empty);

		// store the resource in database
		mResourceGroup[group].push_back(handle);
		mResource[handle] = holder;
		mResourceName[name] = handle;

		res->mResGroup = group;
		res->mResName = name;
		res->mResHandle = handle;
		res->mParentManager = this;

		// check for memory usage
		mMemUsage += holder->getResource()->getResDataSize();
		Result ret = checkMemoryUsage();

		if (ret != OK){
			NR_Log(Log::LOG_ENGINE, "ResourceManager: Could not rearrange memory");
			return ret;
		}

		// return a pointer to that resource
		return OK;
	}
*/

	//----------------------------------------------------------------------------------
	Result ResourceManager::lockResource(const std::string& name){

		// get appropriate pointer
		IResourcePtr ptr = getByName(name);

		// if it is null, so we do not have such a resource
		if (ptr.isNull()) return RES_NOT_FOUND;

		return ptr.lockResource();
	}

	//----------------------------------------------------------------------------------
	Result ResourceManager::lockResource(ResourceHandle& handle){

		// get appropriate pointer
		IResourcePtr ptr = getByHandle(handle);

		// if it is null, so we do not have such a resource
		if (ptr.isNull()) return RES_NOT_FOUND;

		return ptr.lockResource();

	}

	//----------------------------------------------------------------------------------
	Result ResourceManager::lockResource(IResourcePtr& res){

		// lock through the pointer
		return res.lockResource();

	}

	//----------------------------------------------------------------------------------
	Result ResourceManager::unlockResource(const std::string& name){

		// get appropriate holder
		SharedPtr<ResourceHolder>* holder = getHolderByName(name);

		if (holder != NULL){
			// lock the resource through the holder
			(*holder)->unlockResource();
		}else{
			return RES_NOT_FOUND;
		}

		return OK;

	}

	//----------------------------------------------------------------------------------
	Result ResourceManager::unlockResource(ResourceHandle& handle){

		// get appropriate holder
		SharedPtr<ResourceHolder>* holder = getHolderByHandle(handle);

		if (holder != NULL){
			// lock the resource through the holder
			(*holder)->unlockResource();
		}else{
			return RES_NOT_FOUND;
		}

		return OK;

	}

	//----------------------------------------------------------------------------------
	Result ResourceManager::unlockResource(IResourcePtr& res){

		// if pointer does not pointing anywhere
		if (res.isNull()){
			return RES_ERROR;
		}

		// lock through the holder
		res.getResourceHolder()->unlockResource();

		return OK;
	}

	//----------------------------------------------------------------------------------
	Result ResourceManager::unloadGroup(const std::string& group){

		// check whenever such a group exists
		ResourceGroupMap::const_iterator it = mResourceGroup.find(group);
		if (it == mResourceGroup.end()){
			NR_Log(Log::LOG_ENGINE, "ResourceManager: Can not unload group \"%s\" because not found in database", group.c_str());
			return RES_GROUP_NOT_FOUND;
		}

		NR_Log(Log::LOG_ENGINE, "ResourceManager: Unload the group \"%s\"", group.c_str());

		// scan through all elements
		std::list<ResourceHandle>::iterator jt = mResourceGroup[group].begin();
		for (; jt != mResourceGroup[group].end(); jt++){
			Result ret = unload(*jt);
			if (ret != OK) return ret;
		}

		// OK
		return OK;
	}

	//----------------------------------------------------------------------------------
	Result ResourceManager::reloadGroup(const std::string& group){

		// check whenever such a group exists
		ResourceGroupMap::const_iterator it = mResourceGroup.find(group);
		if (it == mResourceGroup.end()){
			NR_Log(Log::LOG_ENGINE, "ResourceManager: Can not reload group \"%s\" because not found in database", group.c_str());
			return RES_GROUP_NOT_FOUND;
		}

		NR_Log(Log::LOG_ENGINE, "ResourceManager: Reload the group \"%s\"", group.c_str());

		// scan through all elements
		std::list<ResourceHandle>::iterator jt = mResourceGroup[group].begin();
		for (; jt != mResourceGroup[group].end(); jt++){
			Result ret = reload(*jt);
			if (ret != OK) return ret;
		}

		// OK
		return OK;

	}

	//----------------------------------------------------------------------------------
	Result ResourceManager::removeGroup(const std::string& group){

		// check whenever such a group exists
		ResourceGroupMap::const_iterator it = mResourceGroup.find(group);
		if (it == mResourceGroup.end()){
			NR_Log(Log::LOG_ENGINE, "ResourceManager: Can not remove group \"%s\" because not found in database", group.c_str());
			return RES_GROUP_NOT_FOUND;
		}

		NR_Log(Log::LOG_ENGINE, "ResourceManager: Remove all elements from the group \"%s\"", group.c_str());

		// scan through all elements
		std::list<ResourceHandle>::iterator jt = mResourceGroup[group].begin();
		for (; jt != mResourceGroup[group].end(); jt++){
			Result ret = remove(*jt);
			if (ret != OK) return ret;
		}

		// remove the group
		mResourceGroup.erase(group);

		// OK
		return OK;

	}

	//----------------------------------------------------------------------------------
	const std::list<ResourceHandle>& ResourceManager::getGroupHandles(const std::string& name)
	{
		// check if such a group exists
		ResourceGroupMap::const_iterator it = mResourceGroup.find(name);

		// return the empty list
		if (it == mResourceGroup.end())
			return mEmptyResourceGroupHandleList;

		// return the group to this name
		return it->second;
	}

	//----------------------------------------------------------------------------------
	SharedPtr<IResource> ResourceManager::getEmpty(const std::string& type)
	{
		res_empty_map::iterator it = mEmptyResource.find(type);
		if (it == mEmptyResource.end())
		{
			return SharedPtr<IResource>();
		}

		return it->second;
	}

	//----------------------------------------------------------------------------------
	void ResourceManager::setEmpty(const std::string& type, SharedPtr<IResource> empty)
	{
		// check if empty is valid
		if (empty == NULL) return;

		// setup some default data on it
		mEmptyResource[type] = empty;
	}

	//----------------------------------------------------------------------------------
	bool ResourceManager::isResourceRegistered(const std::string& name)
	{
		IResourcePtr res = getByName(name);
		return res.isNull() == false;
	}

	//----------------------------------------------------------------------------------
	void ResourceManager::notifyLoaded(IResource* res)
	{
		if (res == NULL) return;

		// check if such a resource is already in the database
		if (isResourceRegistered(res->getResourceName())) return;

		// get some data from the resource
		const std::string& group = res->getResourceGroup();
		const std::string& name = res->getResourceName();
		const ResourceHandle& handle = res->getResourceHandle();
		const std::string& type = res->getResourceType();

		// get according empty resource object and give exception if no such exists
		IResource* empty = getEmpty(type).get();
		if (empty == NULL)
		{
			char msg[128];
			sprintf(msg, "There was no empty resource created for the type %s", type.c_str());
			NR_EXCEPT(RES_NO_EMPTY_RES_FOUND, std::string(msg), "ResourceManager::notifyLoaded()");
		}

		// get name of the resource and create a holder for this
		SharedPtr<ResourceHolder> holder(new ResourceHolder(res, empty));

		// store the resource in database
		mResourceGroup[group].push_back(handle);
		mResource[handle] = holder;
		mResourceName[name] = handle;

		//printf("LOADED: %s\n", holder->getResource()->getResourceName().c_str());
	}

	//----------------------------------------------------------------------------------
	void ResourceManager::notifyCreated(IResource* res)
	{
		notifyLoaded(res);
	}

	//----------------------------------------------------------------------------------
	void ResourceManager::notifyUnloaded(IResource*)
	{

	}

	//----------------------------------------------------------------------------------
	void ResourceManager::notifyRemove(IResource* res)
	{
		if (res == NULL) return;

		// check if such a resource is already in the database
		if (!isResourceRegistered(res->getResourceName())) return;

		// get some data from the resource
		const std::string& group = res->getResourceGroup();
		const std::string& name = res->getResourceName();
		const ResourceHandle& handle = res->getResourceHandle();

		NR_Log(Log::LOG_ENGINE, "ResourceManager: Remove resource %s (%s)", name.c_str(), group.c_str());

		if (handle == 0){
			NR_Log(Log::LOG_ENGINE, "ResourceManager: Can not remove resource %s because it is not in the database", name.c_str());
			return;
		}

		// remove the handle from the group list
		if (group.length() > 0){
			ResourceGroupMap::iterator jt = mResourceGroup.find(group);
			if (jt != mResourceGroup.end()){
				jt->second.remove(handle);

				// check whenever group contains no more elements, and delete it
				if (jt->second.size() == 0){
					mResourceGroup.erase(group);
					NR_Log(Log::LOG_ENGINE, "ResourceManager: %s's group \"%s\" does not contain elements, so remove it", name.c_str(), group.c_str());
				}
			}else{
				NR_Log(Log::LOG_ENGINE, Log::LL_WARNING, "ResourceManager: There is no group in the database with the name \"%s\" for resource %s", group.c_str(), name.c_str());
			}
		}
		// get the according holder and remove the resource there
		SharedPtr<ResourceHolder>& holder = *getHolderByName(name);
		if (holder == NULL){
			NR_Log(Log::LOG_ENGINE, Log::LL_WARNING, "ResourceManager: Could not find resource holder for %s", name.c_str());
			return;
		}
		holder->resetResource(NULL);

		// clear the database
		mResourceName.erase(name);
		mResource.erase(handle);
	}

};

