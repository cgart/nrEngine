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


#ifndef _NR_RESOURCE_FACTORY_H_
#define _NR_RESOURCE_FACTORY_H_


//----------------------------------------------------------------------------------
// Includes
//----------------------------------------------------------------------------------
#include "Prerequisities.h"
#include "ResourceSystem.h"
#include <boost/enable_shared_from_this.hpp>

namespace nrEngine{

	
	//! Factory class to built instances of resources
	/**
	* Resource factory is used to create instances of resource objects.
	* Each resource factory has a list of supported resources types. The resource
	* manager can access the previously registered factories to create instances
	* of resources.
	*
	* Loading of resource itself is done in the resource class. Factories are
	* only used to create instances. Factories should also be able to create instances
	* of empty resources.
	*
	* Resource factory should be registered by the resource manager to give him a
	* possibility to create or to delete resource instances.
	* system on compiling layer/level.
	*
	* Factories have got a method which says to the manager what kind of resource types
	* it supports. By creating of a resource, you should specify the type of to be 
	* created resource. Manager will look in the database, wich factory can create an instance
	* of such a type and will force it to create an instance.
	*
	* Factories are strongly associated within created resources. This means that if factory
	* is removed from the memory, then also all created resource will be removed. This behaviour
	* is needed, because otherwise we could get segmentation faults if you use resources from
	* plugins ala dynamic libraries.
	*
	* Resource factory can also let you know which file types are supported by the supported
	* resource types. So you can find out if resource factory can handle resources
	* @note File types and resource types are case sensitive. So *.png and *.PNG are different file types.
	*
	* \ingroup resource
	**/
	class _NRExport ResourceFactory : public boost::enable_shared_from_this<ResourceFactory>{
		public:
			
			/**
			* Remove factory instance. Removing will cause created resources also to be removed
			**/
			virtual ~ResourceFactory();
									
			/**
			 * Create an instance of appropriate resource object. Like <code>new ResourceType()</code>
			 *
			 * @param resourceType Unique name of the resource type to be created
			 * @param params Default is NULL, so no parameters. Specify here pairs of string that
			 *				represents the parameters for the creating functions. The derived
			 *				resource loader should know how to handle with them.
			 * @return Instance of such a resource
			 *
			 * @note When creating there also will be a check if an empty resource of such a
			 * type already exists. If no, then empty resource will be created and registered in the
			 * database.
			 **/
			SharedPtr<IResource> createResource(const std::string& resourceType, PropertyList* params = NULL);

			/**
			* @copydoc ResourceFactory::createResource()
			* @param name Unique name of the resource
			* @param group Unique group name of the resource
			**/
			SharedPtr<IResource> createResource(const std::string& name, const std::string& group, const std::string& resourceType, PropertyList* params = NULL);

			/**
			* @return A vector of strings containing all supported resource types
			**/
			NR_FORCEINLINE const std::vector<std::string>& getSupportedResourceTypes(){return mSupportedResourceTypes;}
			
			/**
			* @return A vector of string cotntainign all supported file types
			 **/
			NR_FORCEINLINE const std::vector<std::string>& getSupportedFileTypes(){return mSupportedFileTypes;}
			
			/**
			* Check if the factory does support creating of resource of the given
			* resource type.
			* 
			* @param resourceType Unique name of the resource type
			* @return <b>true</b> if resource type is supported, otherwise false
			**/
			bool supportResourceType(const std::string& resourceType) const;

			/**
			* Check whenever the factory can handle given file types.
			*
			* @param fileType File type name
			* @return <b>true</b> if resource instances of this file type are supported
			**/
			bool supportFileType(const std::string& fileType) const ;

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
			 * Remove the resource from the memory. This method will be called
			 * by the resource manager, so the loader can do the stuff needed
			 * to remove the resource.
			 **/
			Result remove(SharedPtr<IResource> resource);

			/**
			 * Reload a certain resource again. A resource object has to be created before.
			 * If the resource object is loaded, so it will be unloaded and loaded again. Also
			 * the resource object will be notified about this event, so it can react on this.
			 * No new instancies will be created.
			 **/
			Result reload(SharedPtr<IResource> resource);


		protected:

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
			
			/**
			 * Derived classes must overload this function. This method
			 * should load a resource for a given file name
			 *
			 * @param res Resource instance created before with create()
			 * @param fileName Name of the file containing the resource
			 * @param param Specific parameters specified by the user
			 **/
			virtual Result loadResourceImpl(IResource* res, const std::string& fileName, PropertyList* param = NULL) = 0;

			/**
			 * Implement this function to provide functionaliy
			 * of creating resources of certain types. In this function you do not
			 * have to check whenever the given resourceType string is valid. This
			 * method can only be called from the base class, which does already
			 * have checked this.
			 * 
			 **/
			virtual IResource* createResourceImpl(const std::string& resourceType, PropertyList* params = NULL) = 0;
			
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
			 * 
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
			
			/**
			 * This method will return back a proper suffix according to the resource type.
			 * Derived classes should overwrite this method, because they know which suffix 
			 * are proper to use. The suffix does not contain dot. Empty suffixes are possible.
			 * @param resType Resource type for which one suffix is generated
			 **/
			virtual std::string getSuffix(const std::string& resType){return std::string();}
			

		private:
			
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
			 * Get shared pointer from this class
			 **/	
			SharedPtr<IResourceLoader> getSharedPtrFromThis()
			{
				return shared_from_this();
			}

			/**
			 * Notify the resource loader that a certain resource object
			 * will be removed from the memory now.
			 **/
			void notifyRemoveResource(SharedPtr<IResource>);

			/**
			 * Notify unload resource. The method can be called either
			 * from resource manager or by the resource itself
			 **/
			void notifyUnloadResource(SharedPtr<IResource>);
			
	};

};

#endif
