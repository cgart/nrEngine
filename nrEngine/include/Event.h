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


#ifndef _NR_EVENT__H_
#define _NR_EVENT__H_

//----------------------------------------------------------------------------------
// Includes
//----------------------------------------------------------------------------------
#include "Prerequisities.h"
#include "Priority.h"
#include "Exception.h"

namespace nrEngine{

	/**
	* Macro for definining event type information.
	* Use this macro in all event definitions of classes
	* derived from Event.
	*
	* \ingroup event 
	**/
	#define META_Event(type) \
		public:\
			virtual const char* getEventType() const { return #type; }
		
	
	//! Base untemplated class used for the event instancies
	/**
	 * Event is a base untemplated class.
	 * Each new type of event should be derived from the templated event class
	 * EventT, which does store some runtime type information about the event.
	 *
	 * \ingroup event
	 **/
	class _NRExport Event {
		
		META_Event(Event)
		
		public:

			/**
			 * Release used memory
			 **/
			virtual ~Event();

			/**
			 * Get the priority of this event message.
			 **/
			const Priority& getPriority();
			
			/**
			 * Check whenever this class is of the same type
			 * as the given one. The function is templated,
			 * so you can check for every kind of classes
			 * if the type of an object the same on as given one.
			 **/
			template<class U> bool same_as()
			{
				// we try a dynamic cast if it fails, so types are different
				U* ptr = dynamic_cast<U*>(this);
				if (ptr == NULL) return false;

				return true;
			}

		protected:
			/**
			* Create new instance of a base class Event.
			*
			* @param prior Set priority for this event
			*
			* NOTE: You can not directly derive new classes from
			* this one, because the constructor is private and can
			* only be accessed from friend class EventT<>.
			* We implemented it in this way to hide the internal
			* runtime typechecking from the user.
			**/
			Event(Priority prior);

		private:

			//! Priority of the message
			Priority mPriority;
	};

	/**
	* Cast a certain event to another one. As casting we use
	* dynamic_cast<T>() which will return 0 if casting
	* can not be done of this kind of objects.
	*
	* If casting could not be done, so an exception will be thrown, which
	* you have to catch.
	 *
	 * \ingroup event
	**/
	template<class T>
	static T* event_cast(Event* base)
	{
		T* ptr = NULL;
		ptr = dynamic_cast<T*>(base);
		return ptr;
	}

	/**
	* Same as event_cast() but cast now between
	* smart pointers.
	 *
	 * \ingroup event
	**/
	template<class T>
	static SharedPtr<T> event_shared_cast(SharedPtr<Event> base)  throw (Exception)
	{
		SharedPtr<T> ptr;
		ptr = boost::dynamic_pointer_cast<T, Event>(base);
		return ptr;
	}


}; // end namespace

#endif
