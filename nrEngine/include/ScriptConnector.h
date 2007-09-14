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


#ifndef _NR_SCRIPT_CONNECTOR_H_
#define _NR_SCRIPT_CONNECTOR_H_

//----------------------------------------------------------------------------------
// Includes
//----------------------------------------------------------------------------------
#include "Prerequisities.h"
#include "EventActor.h"
#include "ScriptEngine.h"

namespace nrEngine{


	//! Script connectors are listening on script engine events and react on them
	/**
	 * In our engine we do provide some default behaviour of how to access
	 * registered engine's functions in your scripts. However if you implement
	 * a new embedded scripting language you have somehow to bind the calling
	 * of engine's provided function within your script. Only the functions which
	 * are registered in ScriptEngine are for the public use within the engine.
	 *
	 * To connect your scripts with the functions you can ask scripting engine
	 * for registered functions and somehow connect them. The other more elegant
	 * way is to derive a class from this one. If any instance of this class
	 * is created (done automaticaly by the constructors), so the class will act
	 * as a event listener and wait for certain scripting engine events. If they
	 * occurs it will call virtual functions overwriten by your class which handle
	 * this events. In this way you just derive any new class from this one and
	 * write a glue code to connect new functions (coming by events) within your
	 * scripting language.
	 * 
	 * \ingroup script
	**/
	class _NRExport ScriptConnector : public EventActor{
		public:

			/**
			 * Remove the listener and release used memory
			 **/
			virtual ~ScriptConnector();

		protected:

			/**
			 * Call this function to get noticed about all currently
			 * registered functions
			 * This function should be called from the constructor of derived class.
			 **/
			void initialize();

			/**
			 * Protected constructor, so you can only create instancies
			 * of this class in derived classes.
			 * The constructor will add itself as a listener on scripting evnets.
			 **/
			ScriptConnector(const std::string& name);

			/**
			 * React on events coming from the scripting engine
			 **/
			void OnEvent(const EventChannel& channel, SharedPtr<Event> event);

			/**
			 * New function was registered, so handle it
			 **/
			virtual void OnRegisterFunction(const std::string& name, const ScriptFunctor& func) = 0;
			
			/**
			 * A function was removed from the database
			 **/
			virtual void OnRemoveFunction(const std::string& name) = 0;

			
	};


};

#endif
