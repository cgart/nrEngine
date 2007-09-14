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


#ifndef _NR_LUA_CONNECTOR_H_
#define _NR_LUA_CONNECTOR_H_

//----------------------------------------------------------------------------------
// Includes
//----------------------------------------------------------------------------------
#include <nrEngine/nrEngine.h>

#include "Script.h"

using namespace nrEngine;


/**
 * Glue code to connect script functions within the lua environment
 * 
 * @see ScriptConnector
**/
class LuaScriptConnector : public ScriptConnector{

	public:

		/**
		 * Initialize the connector
		 **/
		LuaScriptConnector();

		/**
		 * If any new function is registered
		 **/
		void OnRegisterFunction(const std::string& name, const ScriptFunctor& func);

		/**
		 * If a function was removed
		 **/
		void OnRemoveFunction(const std::string& name);

	private:

		/**
		 * This is a wrapper function which will be called from lua.
		 * From here the appropriate real function will be called.
		 **/
		static int callFunc(lua_State* L);

		//! List containing already registered functions
		std::list<std::string>	mRegistered;
		
		//! Include lua file (this function will be registered in lua)
		static int includeFile(lua_State *L);
		
		//! Initialize default functions, which are registered in the lua environemnt
		void initializeLuaEnvironment();
		
};

#endif
