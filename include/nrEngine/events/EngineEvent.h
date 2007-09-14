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


#ifndef _NR_ENGINE_EVENT__H_
#define _NR_ENGINE_EVENT__H_

//----------------------------------------------------------------------------------
// Includes
//----------------------------------------------------------------------------------
#include "../Prerequisities.h"
#include "../Event.h"
#include "ScriptEngine.h"

/*!
 * \defgroup sysevent System events
 *
 * This group represents all system events used in the engine.
 * The events are send trough the default engine's communication
 * channel. Very important events will be send on every channel, so
 * user application get noticed about them.
 *
 * The event interface is usefull for appropriate work of the engine.
 * All events are handled in the background fully transparent to the user,
 * so you do not have to care about them.
 *
 * Some of the events could only be created by the system, so normal
 * application is not allowed to send messages of this type. This is done
 * by declaring the event constructor as protected/private, so only friend
 * classes can create such events. You as normal user either don't need them or
 * uncontrolled sending of them could cause system damage. However you are able
 * to recieve the messages sended by the engine, if you need them this is another
 * question ;-)
 * 
 **/

namespace nrEngine{

	//! Base event class for scripting engine events
	/**
	 * This is a base class for all type of events sended by the script
	 * engine. 
	 * \ingroup sysevent
	**/
	class _NRExport ScriptEvent : public Event {
		
		META_Event(ScriptEvent)
		
		protected:
			
			/**
			 * Protect the constructor, so only friend can emit this events.
			 **/
			ScriptEvent(Priority prior = Priority::IMMEDIATE) : Event(prior) {}
			
		private:
			friend class ScriptEngine;

	};

	
	//! Send this event by registering of a new function in the script engine
	/**
	 * Listen on this event if you want to be informed when some engine's component
	 * or your application does register a new function in the ScriptEngine. This is
	 * important if you implement your own glue code for any scripting language.
	 * If you get this event you know that you know have to provide the function
	 * with the given name in your scripting language. This can be done by
	 * calling ScriptEngine::call() method with the given function name and function
	 * arguments.
	 * 
	 * \ingroup sysevent
	 **/
	class _NRExport ScriptRegisterFunctionEvent : public ScriptEvent {
		
		META_Event(ScriptRegisterFunctionEvent)
		
		public:
			//! Get the function name which has been registered
			const std::string& getName() const { return mName; }

			//! Get the functor for this event
			ScriptFunctor getFunctor() { return mFunctor; }
			
		private:
			friend class ScriptEngine;
			
			ScriptRegisterFunctionEvent(const std::string& name, const  ScriptFunctor& func) : ScriptEvent()
			{
				this->mName = name;
				this->mFunctor = func;
			}

			//! Functor on the function
			ScriptFunctor mFunctor;
			
			//! Function name
			std::string mName;
	};

	
	//! Send this event by removing of a function from the script engine
	/**
	 * Listen on this event if you want to know when a certain function is
	 * removed from the ScriptEngie subsystem.
	 * 
	 * \ingroup sysevent
	 **/
	class _NRExport ScriptRemoveFunctionEvent : public ScriptEvent {
		
		META_Event (ScriptRemoveFunctionEvent)
		
		public:
			//! Get the function name which has been registered
			const std::string& getName() const { return mName; }
			
		private:
			friend class ScriptEngine;

			ScriptRemoveFunctionEvent(const std::string& name) : ScriptEvent()
			{
				this->mName = name;
			}

			//! Function name
			std::string mName;
	};

};

#endif
