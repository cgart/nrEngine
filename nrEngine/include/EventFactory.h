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


#ifndef _NR_EVENT_FACTORY_H_
#define _NR_EVENT_FACTORY_H_

//----------------------------------------------------------------------------------
// Includes
//----------------------------------------------------------------------------------
#include "Prerequisities.h"
#include "Event.h"

namespace nrEngine{

	//! Event factories are used to create events of certain types
	/**
	 * EventFatory is an object which is able to create events of certain
	 * type. You can either create events by yourself or through such
	 * a factory.
	 *
	 * We introduce the concept of a factory to enable sharing of new event
	 * types through plugins. That means, that plugins provide the engine
	 * such a factory to create an event object within the engine memory either
	 * in the plugin (dynamic library) memory.
	 *
	 * \ingroup event
	**/
	class _NRExport EventFactory{
		public:

			/**
			 * Create a instance of a new factory.
			 **/
			EventFactory(const std::string& name);

			/**
			 * Release used memory and remove the list of supported event types
			 * from the event manager. So no new events of the supported types
			 * could be created throguh this factory
			 **/
			virtual ~EventFactory();


			/**
			 * Return true if the given event type is supported
			 **/
			bool isSupported(const std::string& eventType) const;

			/**
			 * Create a new instance of an event of the given type
			 *
			 * @param eventType Type name of the event to create
			 * @return NULL if the event could not been created or
			 * 				a valid smart pointer otherwise
			 **/
			virtual SharedPtr<Event> create(const std::string& eventType) = 0;

			/**
			 * Get the name
			 **/
			const std::string& getName () const { return mName; }
		protected:

			//! List containing all supported event types by their names
			typedef std::list<std::string> NameList;

			//! Variabl eto hold all supported types
			NameList mSupportedTypes;

			//! Fill the list of supported event types
			virtual void fillSupported() = 0;

			//! Name of the factory
			std::string mName;

	};

}; // end namespace

#endif
