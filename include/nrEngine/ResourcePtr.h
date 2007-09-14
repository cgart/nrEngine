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


#ifndef _NR_RESOURCE_POINTER_H_
#define _NR_RESOURCE_POINTER_H_



//----------------------------------------------------------------------------------
// Includes
//----------------------------------------------------------------------------------
#include "Prerequisities.h"
#include "Exception.h"
#include "Log.h"

namespace nrEngine{

	//! Base untemplated class for resource pointers
	/**
	* This is a base class for resource pointers. You can use this pointers as
	* normal pointers, except you can not use new,delete functions.
	* We need this base class which is not templated to implement converting
	* functions in derived classes
	* @note We do not have any public constructors or destructors. This will prevent
	*		 you by using the resource management system in the wrong way. You have
	*		 to use manager before you can use the pointers to resources. This will
	*		 give the nrEngine the safety of using its resource management system.
	* \ingroup resource
	**/
	class _NRExport IResourcePtr{
		public:

			/**
			* Copy constructor
			**/
			IResourcePtr(const IResourcePtr& resPtr);

			/**
			* Create an empty resource pointer, that do not point to anywhere
			**/
			IResourcePtr()
			{
				mHolder.reset();
			}

			/**
			* Virtual destructor
			**/
			virtual ~IResourcePtr()
			{
				mHolder.reset();
			};

			/**
			* Check if the given pointer is the same as this one.
			* @note Two pointers are the same, if they showing to the same resource, and
			* 		 if their holders are the same or if
			*		 both pointers points to NULL. If one of the pointers is an empty pointer,
			*		 so also false will be returned.
			**/
			virtual bool operator==(IResourcePtr& res) const;


			/**
			* Check equality to normal pointers.
			**/
			virtual bool operator==(const IResource* p) const;


			/**
			* Check whenver two pointers are not the same.
			* @see operator==
			**/
			virtual bool operator!=(IResourcePtr& res) const;


			/**
			* Check whenver two pointers are not the same.
			* @see operator==
			**/
			virtual bool operator!=(const IResource* res) const;


			/**
			* Check whenever this pointer is NULL - does not contain data
			**/
			NR_FORCEINLINE bool isNull()const {
				return (mHolder.get() == NULL);
			}

			/**
			 * Check whenever the pointer contains valid data. The method
			 * if opossite to isNull()
			 **/
			NR_FORCEINLINE bool valid()const {
				return (mHolder.get() != NULL);
			}

			/**
			* Lock the resource to which one this pointer points, to prevent using
			* of empty resource. See more information about locking of real resources
			* in the documentation about ResourceManager::lockPure() or
			* ResourceHolder::lockPure()
			* @return either OK or:
			*	- RES_PTR_IS_NULL if this pointer is null
			*	- RES_LOCK_STATE_STACK_IS_FULL if we are not able to lock anymore
			**/
			Result lockResource();


			/**
			* Unlock the resource to which one this pointer points, to prevent using
			* of empty resource. See more information about locking of real resources
			* in the documentation about ResourceManager::unlockpure()
			* @return either OK or RES_PTR_IS_NULL if this pointer is null
			**/
			Result unlockResource();


			/**
			* Lock an empty resource, so that even if resource is loaded, empty resource will
			* be used.
			* @see lockPure()
			**/
			Result lockEmpty();
			
			/**
			* Unlock empty resource
			**/
			Result unlockEmpty();
			
			/**
			* Access to the resource to which one this pointer points. This access need
			* 1 static_cast, 3 function calls until it returns the resource.
			* So this function is running in O(1) and is pretty efficient
			**/
			IResource* getBase() const;


			/**
			* Access to the resource to which one this pointer points. This access need
			* 1 static_cast, 3 function calls until it returns the resource.
			* So this function is running in O(1) and is pretty efficient
			**/
			//virtual IResource& operator*() const;

		protected:

			//! Resource Manager is a friend, so it can freely work with this pointers
			friend class IResourceLoader;

			//! Also resource manager is a friend
			friend class ResourceManager;

			//! Shared pointer holding the holder of the resource
			SharedPtr<ResourceHolder> mHolder;

			/**
			* Get the holder to which one this pointer shows
			**/
			NR_FORCEINLINE SharedPtr<ResourceHolder> getResourceHolder() const
			{
				return mHolder;
			}

			/**
			* Create an instance of the resource pointer. This pointer is pointing
			* to the resource stored by the holder. The constructor is private, so you
			* can not use the new function to create the pointer.
			**/
			IResourcePtr(SharedPtr<ResourceHolder> holder)
			{
				mHolder = holder;
			}


	};



	//! Resource pointer is a smart pointer pointing to the resource
	/**
	* This is a smart pointer that can be used to access resources manages by the
	* resource manager. This pointer will automaticly be pointing to empty resource
	* if the resource was unloaded by the manager.
	*
	* You can create more than one resource pointer to a resource. Each access to such
	* a resource through the manager will create a new one for you. However the pointers
	* are pointing to one resource holder. Each resource holder is controlled/managed
	* by the manager. So if you for example unload a resource to which one you has pointers,
	* the manager will replace the resource holding by the holder with empty one. So if
	* you try to access the resource you can still access them, but they are empty.
	* Our systems also allows you to delete the manager from the memory, but the pointers
	* will stay valid. So you can also remove the manager after you loaded all resources.
	* However you can then do not manage the resources, so this is a way you should <b>not</b>
	* do it !!!
	*
	* @note We do not have any public constructors or destructors. This will prevent
	*		 you by using the resource management system in the wrong way. You have
	*		 to use manager before you can use the pointers to resources. This will
	*		 give the nrEngine the safety of using its resource management system.
	*
	* \ingroup resource
	**/
	template<typename ResType>
	class _NRExport ResourcePtr: public IResourcePtr{
		public:

			/**
			 * Create an empty resource pointer. Such a pointer
			 * does not point to anything. So using of not initialized
			 * pointers will give you an exception
			 **/
			ResourcePtr() : IResourcePtr() {}

			/**
			* Copy constructor to allow copying from base class
			**/
			ResourcePtr(const IResourcePtr& res) : IResourcePtr(res){}
			
			/**
			* Access to the resource to which one this pointer points. This access need
			* 1 static_cast, 3 function calls until it returns the resource.
			* So this function is running in O(1) and is pretty efficient
			**/
			NR_FORCEINLINE ResType* operator->() const
			{
				ResType* ptr = dynamic_cast<ResType*>(IResourcePtr::getBase());
				if (ptr == NULL)
				{
					NR_Log(Log::LOG_ENGINE, Log::LL_ERROR, "ResourcePtr<%s> cannot cast from IResource", getBase()->getResourceName().c_str());
					NR_ASSERT(ptr != NULL && "The resource has wrong type");
				}
				return ptr;
			}


			/**
			* Get the object stored by this pointer.
			* NOTE: The instance is controlled by the pointer, so do not delete it
			**/
			NR_FORCEINLINE ResType* get()
			{
				return operator->();
			}

			/**
			* Access to the resource to which one this pointer points. This access need
			* 1 static_cast, 3 function calls until it returns the resource.
			* So this function is running in O(1) and is pretty efficient
			**/
			NR_FORCEINLINE ResType& operator*() const
			{
				return *(operator->());
			}
	};

};
#endif
