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


#ifndef _NR_RESOURCE_LOADER_H_
#define _NR_RESOURCE_LOADER_H_


//----------------------------------------------------------------------------------
// Includes
//----------------------------------------------------------------------------------
#include "Prerequisities.h"
#include "ResourceSystem.h"
#include <boost/enable_shared_from_this.hpp>

namespace nrEngine{

	
	//! Interface for loading/creating resources
	/**
	* This is an interface which has to be implemented by each resource loader.
	* A resource loader is an object which can load different kind of resources
	* from different kind of files. For example there can be a image loader
	* which can load PNG, DDS, TGA, .. images and this one can be used to load
	* texture resources. In this case you can derive resource loader classes for
	* different filetypes from your texture resource loader class or just use
	* image loading functions for different types.
	*
	* Resource loader should be registered by the resource manager. After it is
	* registered it will be automaticly used to load files of different types.
	* If there is no loader registered for that type, so file will not be loaded,
	* error will occurs and empty resource will be used.
	*
	* Each loader has also to support creating/loading of empty resources.
	* By creating of an empty resource it's holder will not hold any resource.
	* After you loading the resource it will be initialized with the resource data
	* and with the empty resource.
	*
	* Each loader has a method which says to the manager what kind of resource types
	* it supports. By creating of a resource, you should specify the type of to be 
	* created resource. Manager will look in the database, wich loader can create an instance
	* of such a type and will force such a loader to create an instance.
	*
	* Because only loader does know how to load/unload or create/delete resources
	* in/from the memory so it has the full access on the resources. If you remove
	* any loader, so all associated resources will be also removed from the memory.
	* We need this behaviour also to prevent seg faults by using loaders from plugins,
	* which does own own memory.
	* 
	* @note File types and resource types are case sensitive. So *.png and *.PNG are different file types.
	*
	* \ingroup resource
	**/
	class _NRExport IResourceLoader : public boost::enable_shared_from_this<IResourceLoader>{
		public:
			
			/**
			 * Removing resource loader from the memory, will also remove
			 * all loaded objects with this loader.
			**/
			virtual ~IResourceLoader();
				
			/**
			 * This method should create and load a resource from file. The resource does
			 * know its filename.
			 *
			 * @param name Unique name for the resource
			 * @param group Unique group name for this resource
			 * @param fileName Name of the file containing the resource
			 * @param resourceType Unique type name of the resource [optional]
			 * @param param Define load specific name value pairs. Derived classes
			 * 				should know how to handle them. [optional]
			 * 
			 * @return either OK or an error code
			**/
			SharedPtr<IResource> load(const std::string& name, const std::string& group, const std::string& fileName, const std::string& resourceType = std::string(), PropertyList* param = NULL);

			/**
			 * Create an instance of appropriate resource object. Like <code>new ResourceType()</code>
			 *
			 * @param name Unique name of the resource
			 * @param group Unique group name for the resource
			 * @param resourceType Unique name of the resource type to be created
			 * @param params Default is NULL, so no parameters. Specify here pairs of string that
			 *				represents the parameters for the creating functions. The derived
			 *				resource loader should know how to handle with them.
			 * @return Instance of such a resource
			 *
			 * NOTE: If a new type of resource will be created, so also a new empty resource
			 * object will be created of this type. This will be automaticaly registered by the manager.
			 **/
			SharedPtr<IResource> create(const std::string& name, const std::string& group, const std::string& resourceType, PropertyList* params = NULL);

			/**
			* This method should return a vector of strings containing information about
			* which kind of resource instances can this loader create. Each resource has it's
			* unique resource type name. So this vector contains such names.
			**/
			NR_FORCEINLINE const std::vector<std::string>& getSupportedResourceTypes(){return mSupportedResourceTypes;}
			
			/**
			 * This function will return a vector containing information about supported filetypes.
			 * It means that this loader can load each file of such a filetype.
			 **/
			NR_FORCEINLINE const std::vector<std::string>& getSupportedFileTypes(){return mSupportedFileTypes;}
			
			/**
			* This method will say if this loader does support creating of resource of the given
			* resource type.
			* 
			* @param resourceType Unique name of the resource type
			* @return <b>true</b> if this loader can create instances of such a type, false otherwise
			**/
			bool supportResourceType(const std::string& resourceType) const;

			/**
			* This method will say if this loader does support loading of resource of the given
			* file type.
			* 
			* @param fileType File type name 
			* @return <b>true</b> if this loader can load such files, false otherwise
			**/
			bool supportFileType(const std::string& fileType) const ;

#if 0
			/**
			 * Unload the given resource. Unloading of a resource does not mean that the
			 * resource is removed from the memory. It just release almost all used
			 * memory and an empty resource will be used instead. To remove it completly
			 * from the memory call remove() instead.
			 *
			 * This method will be called by the manager or the resource, as soon as a resource must
			 * be unloaded.
			 **/
			Result unload(SharedPtr<IResource> resource);

			/**
			 * Reload a certain resource again. A resource object has to be created before.
			 * If the resource object is loaded, so it will be unloaded and loaded again. Also
			 * the resource object will be notified about this event, so it can react on this.
			 * No new instancies will be created.
			 **/
			Result reload(SharedPtr<IResource> resource);
#endif
			/**
			 * Remove the resource from the memory. This method will be called
			 * by the resource manager, so the loader can do the stuff needed
			 * to remove the resource.
			 **/
			Result remove(SharedPtr<IResource> resource);

		protected:
#if 0
			/**
			 * Unload a certain resource from the memory.
			 * To unload a resource the IResource::unloadRes() method will be called. Herefor
			 * each resource should know how to release it's data. However you can overwrite
			 * this method in derived classes to change this default behaviour.
			 * 
			 * @param res Pointer to the resource which should be unloaded
			 *
			 **/
			virtual Result unloadResourceImpl(IResource* res);

			/**
			 * Each derived class has to implement a bahaviour of reloading
			 * of a resource. This method will be called to reload a given
			 * resource object from the file again. All according data is already
			 * stored in the resource only the data from the file must be reloaded.
			 *
			 * The default behaviour of this method is to call loadImpl() again
			 * with the given resource object and a file name (as user parameters the
			 * NULL will be passed).
			 * @param res Resource to be reloaded
			 **/
			virtual Result reloadResourceImpl(IResource* res);
#endif			
			/**
			 * Derived classes must overload this function. This method
			 * should load a resource for a given file name
			 *
			 * @param res Resource instance created before with create()
			 * @param fileName Name of the file containing the resource
			 * @param param Specific parameters specified by the user
			 **/
			virtual Result loadResource(IResource* res, const std::string& fileName, PropertyList* param = NULL) = 0;

			/**
			 * Implement this function to provide functionaliy
			 * of creating resources of certain types. In this function you do not
			 * have to check whenever the given resourceType string is valid. This
			 * method can only be called from the base class, which does already
			 * have checked this.
			 * @param resourceType Type of the resource
			 * @param params Parameter list which can be passed for creating
			 **/
			virtual IResource* createResource(const std::string& resourceType, PropertyList* params = NULL) = 0;
			
			/**
			 * Creating of an empty resource object. An empty resource object can be used in
			 * normal way but contains no data.
			 *
			 * @param resourceType Unique name of the resource type to be created
			 * @return Instance of empty resource
			 **/
			virtual IResource* createEmptyResource(const std::string& resourceType) = 0;
			
			/**
			 * Call this function to initilize the loader. Usually initialization of the
			 * loader does declare supported file and resource types. So because this function
			 * is pure virtual it must overloaded by derived classes.
			 *
			 * This method should be called from the constructor, to declare supported types.
			 **/
			virtual Result initializeResourceLoader() = 0;
			
			/**
			 * Create instance of the resource loader.
			 * @param name Unique name of the loader
			 **/
			IResourceLoader(const std::string& name);

			/**
			* Map a certain file type to the resource type. Call this method if
			* you want to find out resources of which types will be created
			* if you use this filetype.
			*
			* @param fileType Type of the file (ending *.so, *.bmp, ...)
			* @return resource type name which will be create for this file type. If
			* 			the given file type is not supported empty string will be given back
			**/
			NR_FORCEINLINE std::string mapFileTypeToResourceType(const std::string& fileType)
			{
				std::map<std::string, std::string>::const_iterator it = mTypeMap.find(fileType);
				if (it == mTypeMap.end()) return std::string();
				return it->second;
			}
						
			/**
			* Declare the mapping of file types to resource types.
			* NOTE: You have to specify supported types before
			**/
			NR_FORCEINLINE void declareTypeMap(const std::string& fileType, const std::string& resourceType)
			{
				if (supportResourceType(resourceType) && supportFileType(fileType))
					mTypeMap[fileType] = resourceType;
			}
			
			/**
			* Internal function which must be called by all derived classes to 
			* setup all supported resource types.
			* @param name Unique name of supported resource type
			*/
			NR_FORCEINLINE void declareSupportedResourceType(const std::string& name){
				mSupportedResourceTypes.push_back(name);
			}
	
			/**
			* Internal function which must be called by all derived classes to 
			* setup all supported file types.
			* @param name Unique name of file type which is supported by the derived loader
			*/
			NR_FORCEINLINE void declareSupportedFileType(const std::string& name)
			{
				mSupportedFileTypes.push_back(name);
			}
			
#if 0
			/**
			 * This method will return back a proper suffix according to the resource type.
			 * Derived classes should overwrite this method, because they know which suffix 
			 * are proper to use. The suffix does not contain dot. Empty suffixes are possible.
			 * @param resType Resource type for which one suffix is generated
			 **/
			virtual std::string getSuffix(const std::string& resType){return std::string();}
#endif
			/**
			 * Notify the resource loader that a certain resource object
			 * will be now removed from the memory. The method should be called,
			 * by the resource manager, to notify the loader.
			 **/
			void notifyRemoveResource(SharedPtr<IResource>);

#if 0
			/**
			* Resource will notify the loader that it has been reloaded through
			* this method. Resource loader should
			 **/
			void notifyUnloadResource(SharedPtr<IResource>);
#endif

		private:

			//! Resource manager can access the methods here
			friend class ResourceManager;
			
			//! List of supported resource types
			std::vector< std::string >		mSupportedResourceTypes;

			//! List of supported file types
			std::vector< std::string >		mSupportedFileTypes;

			//! Mapping from file type to resource type
			std::map<std::string, std::string> mTypeMap;

			//! Unuque name of the loader
			std::string mName;

			typedef std::list< SharedPtr<IResource> >  ResourceList;
			
			//! List of resources managed by this loader
			ResourceList mHandledResources;

					
			/**
			 * Create an instance of appropriate resource object. 
			 **/
			SharedPtr<IResource> create(const std::string& resourceType, PropertyList* params = NULL);

			/**
			 * Get shared pointer from this class
			 **/	
			SharedPtr<IResourceLoader> getSharedPtrFromThis()
			{
				return shared_from_this();
			}

#if 0
			/**
			 * Notify unload resource. The method can be called either
			 * from resource manager or by the resource itself
			 **/
			void notifyUnloadResource(SharedPtr<IResource>);
#endif
	};

};

#endif
