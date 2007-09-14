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


#ifndef _NR_I_RESOURCE__H_
#define _NR_I_RESOURCE__H_


//----------------------------------------------------------------------------------
// Includes
//----------------------------------------------------------------------------------
#include "Prerequisities.h"
#include "ResourceSystem.h"
#include <boost/enable_shared_from_this.hpp>

namespace nrEngine{

	//! General interface to hold any kind of resources
	/**
	* This is an interface which is describing how resource classes should looks like.
	* Derive your own classes to get resource management system working. Each
	* resource containing only once in the memory if it has the same name. So all
	* names of resources should be unique. Resource is not directly stored by the
	* manager but by the ResourceHolder. This holder just stores the resource
	* and is unique for each resource. Manager just manages such holders and
	* gives the resource pointers access to it.
	*
	* All derived classes must implement the behavior of resource objects correctly.
	* Correct behavior of resource objects is done if resources do nothing what can
	* change the empty resource behavior if they are empty. For example if you use texture
	* and user is setting some values to it and it is at the moment unloaded, so the method
	* will be called by the empty texture object. Empty texture object should stay empty and
	* therefor there should not be any changes. So texture object has to check whenever it is
	* an empty resource and should not do anything.
	*
	* The empty flag should be set by the loader, because only the loader does know
	* if and when a resource can be defined as empty. For example you have to load the empty
	* resource texture before you declare this resource as empty.
	*
	* \ingroup resource
	**/
	class _NRExport IResource : public boost::enable_shared_from_this<IResource>{
	public:

		/**
		* Here the class should return count of bytes reserved
		* by the resource. The size must not contain the size of the class itself.
		* For example: 32Bit Texture of 16x16 Pixel has the size: 16*16*4 = 1024 Bytes +
		* some more bytes which does class need to store himself in the memory.
		**/
		NR_FORCEINLINE std::size_t getResourceDataSize(){return mResDataSize;}


		/**
		* Return here the name of the resource type. This name will be used
		* to assign right empty resources to the resource pointer/holder objects.
		* E.g: "Texture", "Sound",  ...
		**/
		NR_FORCEINLINE const std::string& getResourceType() const { return mResType; }


		/**
		* Return true if the resource is marked as loaded. If any resource is loaded
		* so it contains in the memory and has it's full data there.
		**/
		NR_FORCEINLINE bool 			isResourceLoaded() const {return mResIsLoaded;}

		/**
		* Return handle of this resource
		**/
		NR_FORCEINLINE ResourceHandle		getResourceHandle() const {return mResHandle;}

		/**
		* Return group name to which one this resource belongs
		**/
		NR_FORCEINLINE const std::string&	getResourceGroup() const {return mResGroup;}

		/**
		* Get the name of the resource
		**/
		NR_FORCEINLINE const std::string&	getResourceName() const {return mResName;}

		/**
		* Get the file name from which one this resource can be restored or loaded
		**/
		NR_FORCEINLINE const std::list<std::string>&	getResourceFilenameList() const {return mResFileNames;}

		/**
		* Returns true if this resource is a empty resource. Empty resources are used
		* to replace normal resources if they are unloaded.
		**/
		NR_FORCEINLINE bool isResourceEmpty()	{return mResIsEmpty;}

		/**
		* Unload resource. The resource should overwrite the IResource::unloadResource() method
		* which will be called. The resource should know how to unload itself.
		* The resource owner (in this case loader) will be notified about unloading on success.
		**/
		Result unload();

		/**
		* Reload resource if this was previously unloaded. Each resource should overwrite
		* IResource::reloadResource() method. Resource should know how to reload
		* itself if it was previously unloaded. Resource owner (in this case loader) will be notified
		* on success.
		* @note If resource was not unloaded before, so the IResource::unload() method
		* will be called before reloading.
		**/
		Result reload(PropertyList* params = NULL);

		/**
		* Remove resource. Removing resource through this method, meanse to
		* remove the resource from the loader and from the manager. After this method call
		* all resource pointers associated within this resource will point to the empty
		* resource. All shared instaces would also be removed.
		*
		* If you call this method through resource pointer, then nothing bad happens. If
		* you call it directly (i.e. YourResource* res; res->remove()), hten you are
		* responsible for deleting the pointer by yourself
		**/
		Result remove();

		/**
		 * Add resource file name. The filename is used to associated the resource with.
		 * If a resource is build upon of a multiple files, then all the files should
		 * be added through this method.
		 * @param filename Filename of a file associated with the resource
		 **/
		 void addResourceFilename(const std::string& filename);
		 
		 /**
		  * @see addResourceFilename(const std::string& filename)
		  * @param flist List of filenames to add 
		  **/
		 void addResourceFilename(const std::list<std::string>& flist);

		/**
		 * Set resource file name list. This method will replace the current
		 * filename list with the given one.
		 * @param flist List of filenames
		 **/
		void setResourceFilename(const std::list<std::string>& flist);

		/**
		 * Set the resource as dirty. If resource is marked as dirty, then it 
		 * will be reloaded on next access.
		 * @param dirty True to mark resource as dirty otherwise false.
		 * @note After the resource is reloaded it will be unmarked
		 **/
		NR_FORCEINLINE void setResourceDirty(bool dirty) { mResIsDirty = dirty; }
		
		/**
		* Check whenever a resource is dirty. Dirty resources require 
		* reloading as soon as possible (i.e. on next access)
		**/
		NR_FORCEINLINE bool isResourceDirty() { return mResIsDirty; }
		
	protected:
		
		/**
		 * Create resource instance.
		 **/
		IResource(const std::string& resourceType);

		/**
		 * As soon as this destructor is called, the resource manager will be notified.
		 *
		 * You have to call the unloadResource() method if you have some specific unloading
		 * routines for your resource object. We can not call this from here, because
		 * calling of virtual functions from constructor/destructor cause in
		 * undefined behaviour.
		 **/
		virtual ~IResource();

		/**
		 * Unload the resource. Each resource should know how to unload it from
		 * the memory. Unloading does not remove the resource it just unload used
		 * data from memory. To remove the resource from memory you have either
		 * to use ResourceManager or the ResourceLoader.
		 *
		 **/
		virtual Result unloadResource() = 0;

		/**
		 * Reload resource. Each resource object should be able to reload itself
		 * from the disk or other media. It can use the assigned loader to load files
		 * or to make it by itself. At the end the resource should be marked as loaded.
		 *
		 **/
		virtual Result reloadResource(PropertyList* params = NULL) = 0;
		
		
		/**
		 * Mark a resource as loaded. This method will not call the reloadResource()
		 * method. Instead it will just mark this resource as it were loaded,
		 * so you get access to real resource object through the pointers instead
		 * of emtpy resource. Call this method if you create a resource by your
		 * own without loading by loader.
		 **/
		NR_FORCEINLINE void markResourceLoaded() { mResIsLoaded = true; mResIsDirty = false; }
		
		/**
		 * Mark the resource as unloaded. When you mark it, then empty resource
		 * will be used instead.
		 **/
		NR_FORCEINLINE void markResourceUnloaded() { mResIsLoaded = false; }
		
		 //! Get resource loader assigned with the resource
		 NR_FORCEINLINE SharedPtr<IResourceLoader> getResourceLoader() { return mResLoader; }
		 
	private:
		
		//! Only loader can change resource data
		friend class IResourceLoader;

		//! Also resource manager is a friend
		friend class ResourceManager;

		//! Shows whenever resource is loaded (is in the memory)
		bool mResIsLoaded;

		//! If true so this is a empty resource
		bool mResIsEmpty;

		//! Resource's loader with which one it was created
		SharedPtr<IResourceLoader>	mResLoader;

		//! Handle of the resource given from manager
		ResourceHandle	mResHandle;

		//! File associated with the resource
		std::list<std::string>	mResFileNames;

		//! Count of bytes that this resource is occupies in the memory
		std::size_t	mResDataSize;

		//! Name of the resource type
		std::string	mResType;
		
		//! Group name to which one this resource own
		std::string mResGroup;

		//! Name of the resource
		std::string mResName;
		
		//! Set this variable to reload the resource on next access
		bool mResIsDirty;
		
		/**
		 * Set the resource type for this resource.
		 * Method is declared as protected, so only engine can do this.
		 *
		 * @param type Name of the resource type
		 **/
		NR_FORCEINLINE void	setResourceType(const std::string& type) { mResType = type; }

		/**
		* Get shared pointer from this class
		**/	
		SharedPtr<IResource> getSharedPtrFromThis()
		{
			return shared_from_this();
		}

		class _deleter{
		public:
			void operator()(IResource* p) { delete p; }
		};
	};

};

#endif
