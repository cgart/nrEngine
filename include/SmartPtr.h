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

#ifndef __NR_SMART_PTR_H_
#define __NR_SMART_PTR_H_


//----------------------------------------------------------------------------------
// Includes
//----------------------------------------------------------------------------------
#include "Prerequisities.h"

namespace nrEngine{

	//! All smart pointers created whithin this deleter could not be deleted outside
	/**
	* This is an null_deleter - deleter. We need this to create smart pointer
	* (SharedPtr<T>) from our singleton objects. Each pointer created with null
	* deleter can not be released otherwhere else here. So we can give smart pointer
	* to our singleton back and we can be sure, that our singleton object will not
	* be deleted before Release-Function will be executed
	* @see ::boost.org for smart pointers
	* \ingroup gp
	**/
	struct _NRExport null_deleter{
		void operator()(void const *) const
		{
		}
	};
	
#define SharedPtr boost::shared_ptr

#if 0
	/**
	 * In our engine we are use always smart pointers, so that you do not have 
	 * to care about the lifetime of them. We provide our own pointer definition
	 * rather than using boost's for example. Boosts pointers are fine, but they
	 * depends on a big library and could not be changed in a way, which are
	 * usefull for our puprose.
	 * 
	 * The pointer does provide a reference counting mechanism. But rather then 
	 * deriving the classes which could be referenced by this pointer, we store
	 * the counter
	 * \ingroup gp
	 **/
	template <typename T> 
	class RefPtr {
		private:
		
			//! Store here the reference counter
			int32 mCounter;
			
			//! here we store the pointer itself
			T* mPointer;
		
			//! Reference the object, so it will increment the counter 
			void ref() 
			{
				mCount ++;
			}
		
			//! Decrement the counter and release object if counter is equal 0
			void unref()
			{ 
				mCounter --;
				if (mCounter == 0){
					delete mPointer;
				}
			}

		public:
		
			//! Define the pointer as a friend of it's own
			template <class Y> friend class RefPtr; 
			
			//! Create a pointer and initialze it with a NULL pointer
			RefPtr() : mPointer(0), mCounter(0)
			{
			}
			
			//! Create a smart pointer from a given raw pointer 
			RefPtr(T* raw) : mPointer(0), mCounter(0)
			{
				// if the given object is valid, so reference it
				if (raw){
					mPointer = raw;
					ref();
				}
			}
		
			//! Copyconstructor, copy the pointer and reference it again
			RefPtr(const RefPtr<T>& ptr) : mPointer(ptr.mPointer)
			{
				if (mPointer) ref();
			}
			
			//! Release the pointer, delete the holding object only if it reference counter goes to null
			~RefPtr()
			{
				if (mPointer) unref();
				mPointer = 0;
			}


			//! Assigment of a raw pointer. Use it with care, because now the RefPtr gets ownership	
			NR_FORCEINLINE RefPtr& operator=(T* ptr)
			{
			 if (_ptr==ptr) return *this;
            T* tmp_ptr = _ptr;
            _ptr = ptr;
            if (_ptr) _ptr->ref();
            // unref second to prevent any deletion of any object which might
            // be referenced by the other object. i.e rp is child of the
            // original _ptr.
            if (tmp_ptr) tmp_ptr->unref();
            return *this;
	    			if (mPointer == ptr) return *this;
				if ()
				{
					release(); 
					counter = new Counter; 
					rawPtr = raw; 
				}
				return *this;
			}
	
	template <typename Y>
	ptr& operator=(Y* raw)
	{
		if (raw)
		{
			release();
			counter = new Counter; 
			rawPtr = static_cast<X*>(raw);
		}
		return *this;
	}
	
	/* 
		assignment to long to allow ptr< X > = NULL, 
		also allows raw pointer assignment by conversion. 
		Raw pointer assignment is really dangerous!
		If the raw pointer is being used elsewhere, 
		it will get deleted prematurely. 
	*/ 
	ptr& operator=(long num)
	{
		if (num == 0)  //pointer set to null
		{
			release(); 
		}
	
		else //assign raw pointer by conversion
		{
			release();
			counter = new Counter; 
			rawPtr = reinterpret_cast<X*>(num);
		}	
	
		return *this; 
	} 
	
	/*
		Member Access
	*/
		X* operator->() const 
		{
			return GetRawPointer(); 
		}
	
	
	/*
		Dereference the pointer
	*/
		X& operator* () const 
		{
			return *GetRawPointer(); 
		}
	
	
	/*
		Conversion/casting operators
	*/
		operator bool() const
		{
			return IsValid();
		}
	
		template <typename Y>
		operator Y*() const
		{
			return static_cast<Y*>(rawPtr);  
		}
	
		template <typename Y>
		operator const Y*() const
		{
			return static_cast<const Y*>(rawPtr);
		}
	
	
	/*
		Provide access to the raw pointer 
	*/
	
		X* GetRawPointer() const         
		{
			if (rawPtr == 0) throw new NullPointerException;
			return rawPtr;
		}
	
		
	/* 
		Is there only one reference on the counter?
	*/
		bool IsUnique() const
		{
			if (counter && counter->count == 1) return true; 
			else return false; 
		}
		
		bool IsValid() const
		{
			if (counter && rawPtr) return true;
			else return false; 
		}
	
		unsigned GetCount() const
		{
			if (counter) return counter->count;
			else return 0;
		}
	
	};
	
	
	template <typename X, typename Y>
	bool operator==(const ptr< X >& lptr, const ptr< Y >& rptr) 
	{
		return lptr.GetRawPointer() == rptr.GetRawPointer(); 
	}
	
	template <typename X, typename Y>
	bool operator==(const ptr< X >& lptr, Y* raw) 
	{
		return lptr.GetRawPointer() == raw ; 
	}
	
	template <typename X>
	bool operator==(const ptr< X >& lptr, long num)
	{
		if (num == 0 && !lptr.IsValid())  //both pointer and address are null
		{
			return true; 
		}
	
		else //convert num to a pointer, compare addresses
		{
			return lptr == reinterpret_cast<X*>(num);
		}
		
	} 
	
	template <typename X, typename Y>
	bool operator!=(const ptr< X >& lptr, const ptr< Y >& rptr) 
	{
		return ( !operator==(lptr, rptr) );
	}
	
	template <typename X, typename Y>
	bool operator!=(const ptr< X >& lptr, Y* raw) 
	{
			return ( !operator==(lptr, raw) );
	}
	
	template <typename X>
	bool operator!=(const ptr< X >& lptr, long num)
	{
		return (!operator==(lptr, num) ); 
	}
	
	template <typename X, typename Y>
	bool operator&&(const ptr< X >& lptr, const ptr< Y >& rptr)
	{
		return lptr.IsValid() &&  rptr.IsValid();
	}
	
	template <typename X>
	bool operator&&(const ptr< X >& lptr, bool rval)
	{
		return lptr.IsValid() && rval;
	}
	
	template <typename X>
	bool operator&&(bool lval, const ptr< X >& rptr)
	{
		return lval &&  rptr.IsValid();
	}
	
	template <typename X, typename Y>
	bool operator||(const ptr< X >& lptr, const ptr< Y >& rptr)
	{
		return lptr.IsValid() || rptr.IsValid();
	}
	
	template <typename X>
	bool operator||(const ptr< X >& lptr, bool rval)
	{
		return lptr.IsValid() || rval;
	}
	
	template <typename X>
	bool operator||(bool lval, const ptr< X >& rptr)
	{
		return lval || rptr.IsValid(); 
	}
	
	template <typename X>
	bool operator!(const ptr< X >& p)
	{
		return (!p.IsValid());
	}
	
	
	/* less than comparisons for storage in containers */
	template <typename X, typename Y>
	bool operator< (const ptr< X >& lptr, const ptr < Y >& rptr)
	{
		return lptr.GetRawPointer() < rptr.GetRawPointer();
	}
	
	template <typename X, typename Y>
	bool operator< (const ptr< X >& lptr, Y* raw)
	{
		return lptr.GetRawPointer() < raw;
	}
	
	template <typename X, typename Y>
	bool operator< (X* raw, const ptr< Y >& rptr)
	{
		raw < rptr.GetRawPointer();
	}
#endif
}; // end namespace

#endif
