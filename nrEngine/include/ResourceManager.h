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


#ifndef _NR_RESOURCE_MANAGER_H_
#define _NR_RESOURCE_MANAGER_H_


//----------------------------------------------------------------------------------
// Includes
//----------------------------------------------------------------------------------
#include "Prerequisities.h"
#include "ResourceSystem.h"
#include "ScriptEngine.h"

namespace nrEngine {

	//! General pointer/handle based resource management system
	/**
	* This class is a manger for every kind of resource used in our games.
	* Textures, sounds, meshes, ... - all things that can be loaded from disk
	* are resources managed by this module. You can derive your own classes
	* to write own storing algorithms for different kind of resources. This class
	* will use standard implementation and will try to be as efficient as possible.
	* This manager can index resources, look them up, load and destroy them, and
	* temporaly unload resources from memory to use only allowd size of memory.
	*
	* We will use a priority system to determine which resources can be unloaded
	* and for two resources of the same priority the most used will stay in memory.
	*
	* This code and idea was get from Game Programming Gems 4. <a href="http://www.ogre3d.org">
	* Ogre3D-Library</a> was also used to get an idea how to get the resource manager
	* working without using a whole library behind.
	*
	*
	* <b>-</b> Meaning of resource groups:
	*		@par - Each resource can exists in a group.
	*			 - Each group has got it's unique names.
	*			 - You can access to whole groups either to single resources.
	*			 - E.g: you can split your level in patches and each resource in such a patch is
	*			   in patch's resource group. So if the player go to far away from such a patch,
	*			   it can be disabled or unloaded from the memory. You have now just to call one
	*			   unload function for the whole group.
	*			 - Groups must not be disjoint, so you can have same resource in different groups.
	*
	* \ingroup resource
	**/
	class _NRExport ResourceManager{
		public:
	
			/**
			 * Release all used memory and also mark all loaded resources to unload. After
			 * you calling the destructor of ResourceManager class all created resources
			 * and loaders will be removed from the memory.
			**/
			virtual ~ResourceManager();

#if 0
			/**
			* Set amount of memory that can be used by the manager. Set this value to 80-90%
			* of your physical memory if you do not have any virtual memory in your system
			* available. The manager will try to stay in the budget you given for him.
			* Resources that are least used and has got lowest priority will be unloaded
			* to free the memory to stay in the given budget of memory.
			*
			* @param bytes Count of bytes which can be used for resource storing
			* @return either OK or error code.
			*
			* @note You can set this value to ca 80-90% of your memory available.
			*		 Before resource manager unload least used resources it will allocate
			*		 memory for the new resource to check how much it needs. So we still need
			*		 some free memory for this temporary allocation. So 80-90% are good
			*		 values for memory budget.
			*		 If you want to be very precisious you can set this value to memory
			*		 size minus biggest resource size. Which will use maximal available
			*		 memory space.
			**/
			Result setMemoryBudget(size_t bytes);
	
	
			/**
			* Get the memory budget resource manager have got.
			**/
			size_t	 getMemoryBudget() const;
	
	
			/**
			* Returns the usage of memory in bytes.
			**/
			size_t	 getMemoryUsage() const;
#endif
	
			/**
			* Here you can register any loader by the manager. Loader are used to load resources
			* from files/memory and to store resources in the memory. By registration of
			* a loader you have to specify his unique name. This name will be used to access
			* to stored loader (for example to remove it).
			*
			* @param name Unique loader name
			* @param loader Smart pointer containing the loader
			*
			* @return either OK or error code:
			*			- RES_LOADER_ALREADY_EXISTS
			*			- BAD_PARAMETERS
			*
			* @note If there is two loaders which do support loading of file of the same filetype,
			*		 so the behavior of getting appropriate loader is undefined.
			**/
			Result	registerLoader	(const std::string& name, ResourceLoader loader);
	
	
			/**
			* Removes the loader from the loader list. All bounded filetypes
			* registrations will also be removed. So after you calling this you have
			* to check whenever you still able to load filetypes you want to use.
			*
			* @param name Unique name of the loader
			* @return either OK or error code:
			*		- RES_LOADER_NOT_REGISTERED
			*		- RES_ERROR
			**/
			Result	removeLoader	(const std::string& name);
	
	
			/**
			* Return the loader by given filetype. If there is no loader which can load
			* such a filetype NULL will be returned
			* @param fileType File type for which one the loader should be found
			**/
			ResourceLoader getLoaderByFile	(const std::string& fileType);
	
	
			/**
			* Get a loader that support creating of resource instances of the given resource type
			* @param resType Unique name of the resource type
			* @return either NULL or resource loader
			**/
			ResourceLoader getLoaderByResource(const std::string& resType);
	
	
			/**
			* Return the loader by given name. If the loader with this name does not
			* exists, so NULL will be given back
			**/
			ResourceLoader getLoader			(const std::string& name);
	
	
			/**
			* Create a resource object of a certain type. An according resource loader has
			* to be registered before to provide manager with creating function.
			* If no loader was registered NULL will be returned.
			* 
			* After you created a resource you will still not be able to use it. You have
			* to load a resource from a file. You have also to load it, because we are using
			* concept of empty resources. So loader has to get access on the resource to
			* initialize it with empty data.
			*
			* @param name Unique name of that resource. The name must be specified !!!
			* @param group Name of the group to which this resource belongs.
			* @param resourceType Unique type name of the resource type
			* @param params Here you can specify parameter which will be send to the loader/creator
			*				so that he can setup resource parameters. Default is NULL, so no parameters.
			**/
			IResourcePtr	createResource	(const std::string& name,
											const std::string& group,
											const std::string& resourceType,
											PropertyList* params = NULL);
	
			/**
			* Load a resource from a file. For loading of a resource the registered
			* loader will be used. The filetype is defined by the last
			* characters of the file name. If we can not find this characters
			* so the manual loader will be used if such one is specified. Manual loader
			* will also be used if it is specified and filetype is detected.
			* You can assign your resource to special group. The group names
			* has to be unique.
			*
			* If such a resource could not load, NULL will be returned.
			*
			* If you do not specify any resource type, so the loader which has to be used
			* will be detected by the file name. If no such found error will be  given back.
			* 
			* @param name Unique name fo the resource
			* @param group Group name to which this resource should be assigned
			* @param resourceType Unique type name of the resource
			* @param fileName File name of the file to be loaded
			* @param params Here you can specify parameter which will be send to the loader/creator
			*				so that he can setup resource parameters. Default is NULL, so no parameters.
			* @param manualLoader Loader which should be used if you want to overload
			*				all registered loader.
			*
			**/
			IResourcePtr	loadResource	(const std::string& name,
											const std::string& group,
											const std::string& fileName,
											const std::string& resourceType = std::string(),
											PropertyList* params = NULL,
											ResourceLoader manualLoader = ResourceLoader());

#if 0
			/**
			* This function will add a given resource to the resource management system.
			* From now one resource management get the rights for unloading/reloading and
			* removing resources from the memory.
			*
			* @param res  Resource pointer to the resource. (Be careful this is not a smart pointer)
			* @param name Unique name of the resource that should be used in the database
			* @param group Name of the group to which the resource belongs. If no name specified,
			* 				then use name stored in the resource.
			* @return either OK or error code
			*
			* @note The resource should already know which loader does loading
			*		 of it from files. So database will not assign any loader to
			*		 that resource. You should do it before calling this method.
			**/
			Result		add(IResource* res, const std::string& name, const std::string& group = "");
#endif

			/**
			* This method is used to lock the resource with the given name for using.
			* Locking of pure resource means here, that we want now to access to real resource
			* either to empty resource. Because of our transparent functionality of changing
			* between real and empty resource (if real resource is unloaded), we must have a possibility
			* to access to real resources bypassing this changing mechanism. You will need this for
			* example if you want to change some properties of a resource (for example texture flags).
			* If your resource is currently unloaded from the memory and you will try to change properties,
			* so there would be now effect because settings get changed in empty resource which would
			* not change anything (Assumption: resources handles correctly).
			*
			* Assume the resource is not loaded and you want to access it.
			* <code>
			* 	resource->setPropertyOne(...);
			* </code>
			* Affect to empty resource, because the resource is unloaded.<br>
			* <code>
			* 	ResourceMgr.lockResource("resource");<br>
			* 	resource->setPropertyOne(...);<br>
			* 	ResourceMgr.unlockResource("resource");<br>
			* </code>
			* Affect to the resource you probably means. You will set the property of the real
			* resource and not the empty one.
			*
			* @param name Unique name of the resource
			* @return either OK or error code
			*
			* @note You have to decide whenever you want to lock your resource or just work as it is. It is safer
			* to do locking if you have something like write function and do not lock anything for read only functions.
			* e.g:
			* 	- ReadOnly by textures : getting of a ID, drawing, ...
			* 	- Write by texture: set new size, get name (it has also to be locked, because empty texture will return
			* 		a name according to empty texture), load, ...
			**/
			virtual Result		lockResource(const std::string& name);
	
			/**
			* Overloaded function to allow locking through pointer directly
			* @param res Pointer to the resource
			**/
			virtual Result		lockResource(IResourcePtr& res);
	
			/**
			* Overloaded function to allow locking through handle.
			* @param handle Unique handle of the resource
			**/
			virtual Result		lockResource(ResourceHandle& handle);
	
			/**
			* This function is complement to the lockResource(...) methods. This will unlock the
			* real resource from using
			* @param name Unique name of the resource to be unlocked
			**/
			virtual Result		unlockResource(const std::string& name);
	
			/**
			* Overloaded function to allow unlocking through pointer directly
			* @param res Pointer to the resource
			**/
			virtual Result		unlockResource(IResourcePtr& res);
	
			/**
			* Overloaded function to allow unlocking through handle.
			* @param handle Unique handle of the resource
			**/
			virtual Result		unlockResource(ResourceHandle& handle);

			/**
			* Unload the resource from the memory.
			* After this call all the resource pointers assigned within the resource
			* will point to an empty resource.
			*
			* @param name Unique name of the resource
			* @return either OK or error code:
			*		- RES_NOT_FOUND
			* @see IResourceLoader
			**/
			Result		unload(const std::string& name);
	
			//! @see unload(name)
			Result		unload(IResourcePtr& res);
	
			//! @see unload(name)
			Result		unload(ResourceHandle& handle);
	
	
			/**
			* Reload the resource, so it will now contain it's real data.
			* Call this function if want to get your resource back after
			* it was unloaded.
			*
			* @param name Unique name of the resource
			* @return either OK or error code:
			*		- RES_NOT_FOUND
			**/
			Result		reload(const std::string& name);
	
			//! @see reload(name)
			Result		reload(IResourcePtr& res);
	
			//! @see reload(name)
			Result		reload(ResourceHandle& handle);
	
	
			/**
			* This will remove the resource from the database.
			* After you removed the resource and would try to access to it through the
			* manager NULL will be returned.
			*
			* If you remove the resource from the database all resource pointers
			* for this resource will now point to the empty resource object.
			* @param name Unique name of the resource
			* @return either OK or error code:
			*		- RES_NOT_FOUND
			* @see IResourcePtr
			**/
			Result		remove(const std::string& name);
	
			//! @see remove(name)
			Result		remove(IResourcePtr& res);
	
			//! @see remove(name)
			Result		remove(ResourceHandle& handle);
		
			/**
			* Get the pointer to a resource by it's name.
			* If there is no resource with this name, so NULL pointer will be returned
			*
			* @param name Unique name of the resource
			*
			**/
			IResourcePtr	getByName(const std::string& name);
	
			/**
			* Same as getByName(), but here you get the resource by handle.
			**/
			IResourcePtr	getByHandle(const ResourceHandle& handle);
	
			/**
			* Unload all elements from the group.
			* @param group Unique name of the group
			* @return either OK or RES_GROUP_NOT_FOUND or an error code from unload(...) - method
			**/
			Result		unloadGroup(const std::string& group);
	
			/**
			* Reload all elements in the group
			* @param group Unique name of the group
			* @return either OK or RES_GROUP_NOT_FOUND or an error code from reload(...) - method
			**/
			Result		reloadGroup(const std::string& group);
	
			/**
			* Remove all elements in the group
			* @param group Unique name of the group
			* @return either OK or RES_GROUP_NOT_FOUND or an error code from remove(...) - method
			**/
			Result		removeGroup(const std::string& group);
	
			/**
			 * Get a list of all resource handles for a specified group. The list can be
			 * iterated to get the resource handles which are in this group. Use the handles to 
			 * get a resource according to it.
			 *
			 * @param name Unique name of the group 
			 * @return List of resource handles containing in this group 
			 **/
			const std::list<ResourceHandle>& getGroupHandles(const std::string& name);
			
			
			//! Typedef for the resource map returned by the getResourceMap() method
			typedef std::map< std::string, std::list<ResourceHandle> > ResourceGroupMap;
			
			/**
			 * Return the map containing group names and according resource list.
			 **/
			const ResourceGroupMap& getResourceMap() { return mResourceGroup; }
			
		private:
		
			/**
			* Release all resources. This will unload and then remove all 
			* resources from the manager. Use this to clean used memory.
			**/
			void removeAllRes();
	
			/**
			* Remove all loaders from the manager
			**/
			void removeAllLoaders();
	
	
			//! Only engine can create the instance
			friend class Engine;
	
			/**
			* The constructor is protected, so only friends (engine's core)
			* are able to create the instance
			**/
			ResourceManager();
	
	
			//------------------------------------------
			// Variables
			//------------------------------------------
			//size_t		mMemBudget;
			//size_t		mMemUsage;
	
			ResourceHandle	mLastHandle;
	
			typedef std::map< std::string, ResourceLoader> loader_map;
			typedef std::map<ResourceHandle, SharedPtr<ResourceHolder> >   res_hdl_map;
			typedef std::map< std::string, ResourceHandle>               res_str_map;
			typedef std::map< std::string, SharedPtr<IResource> >        res_empty_map;
	
			loader_map	mLoader;
	
		
			res_hdl_map        mResource;
			res_str_map        mResourceName;
			ResourceGroupMap   mResourceGroup;
			res_empty_map      mEmptyResource;
	
			// this list shouldn't be filled. it will be returned if no group were found in getGroupHandles() method 
			std::list<ResourceHandle>	mEmptyResourceGroupHandleList;
			
			//------------------------------------------
			// Methods
			//------------------------------------------
	
#if 0
			/**
			* Check if we have now memory available.
			* If we need to remove some resources from the memory to get free place
			* so do it.
			**/
			virtual Result checkMemoryUsage();
			/**
			* This will remove the resource from the database.
			* After you removed the resource and would try to access to it through the
			* manager NULL will be returned.
			*
			* However if you remove the resource it is not just deleted from the memory
			* because of shared pointer system. So the parts of application, wich gets
			* pointers to it, are still able to access to it.
			*
			* @param resPtr Remove the resource whithin this pointer
			* @return either OK or error code:
			*		- RES_NOT_FOUND
			**/
			Result removeImpl(IResourcePtr& resPtr);
#endif
	
	
			/**
			* Get the appropriate holder for the given resource name.
			* This is a internal function, so it will return a pointer to a smart pointer
			* pointing to the holder. This is done because of performance reasons.
			* If nothing found NULL will be returned.
			**/
			SharedPtr<ResourceHolder>*	getHolderByName(const std::string& name);
	
			/**
			* Same as getHolderByName(...) but now get the holder through a handle
			**/
			SharedPtr<ResourceHolder>*	getHolderByHandle(const ResourceHandle& handle);

#if 0
			/**
			* This function will check if there is already an empty resource for the given resource
			* type and will create such one if it is not exists
			* @param resourceType Unique name of a resource type
			* @param empty Reference to smart pointer containing an empty resource after
			* @param loader Appropritate loader for that resource type
			**/
			virtual Result checkEmptyResource(const std::string resourceType,
												SharedPtr<IResource>& empty,
												ResourceLoader loader);
#endif

		private:
			
			//! Resource loader have access to certain functions
			friend class IResourceLoader;

			//! Resource object hast got access to certain objects
			friend class IResource;
			
			/**
			* Get an empty resource of given type.
			*
			* @param type Type of the resource for which one the empty object should be returned
			**/
			SharedPtr<IResource> getEmpty(const std::string& type);
		
			/**
			 * Setup a new empty resource for the given resource type.
			 **/
			void setEmpty(const std::string& type, SharedPtr<IResource> empty);
			
			/**
			 * Check if a resource with the given name already registered in
			 * the database.
			 **/
			bool isResourceRegistered(const std::string& name);

			/**
			 * Notify the manager about newly created resource.
			 * Manager will create a holder for this
			 * and store the things in the database
			 **/
			void notifyCreated(IResource*);
			
			/**
			 * This method is called by the loaders to notify the database,
			 * that the according resource was loaded
			 **/
			void notifyLoaded(IResource*);

			/**
			 * Notify the database, that a certain resource was unloaded.
			 **/
			void notifyUnloaded(IResource*);

			/**
			 * Notify the database that a certain resource was removed from the
			 * memory. After this method returns the resource will be deleted.
			 **/
			void notifyRemove(IResource*);

			/**
			 * Get a new handle for the resource object. Handles are unique.
			 **/
			NR_FORCEINLINE ResourceHandle getNewHandle() { return ++mLastHandle; }

			//! Load any resource from the script
			ScriptFunctionDef(scriptLoadResource);
			
			//! Unload any resource from scripts
			ScriptFunctionDef(scriptUnloadResource);
			
	};

};

#endif
