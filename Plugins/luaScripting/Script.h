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


#ifndef _NR_LUA_SCRIPT_H_
#define _NR_LUA_SCRIPT_H_


//----------------------------------------------------------------------------------
// Includes
//----------------------------------------------------------------------------------
#include <nrEngine/nrEngine.h>

extern "C" {
#include "lua/lua.h"
#include "lua/lualib.h"
#include "lua/lauxlib.h"
}

using namespace nrEngine;


/**
 * This class does support handling of scripts written in lua language
 **/
class LuaScript : public nrEngine::IScript{
	public:

		/**
		 * Constructor
		 **/
		LuaScript();

		/**
		 * Destructor
		 **/
		~LuaScript();
		
		/**
		* Load a script from string. The given string should contain
		* the script in the appropriate language, so it can be executed.
		*
		* @param str String containing the script
		* @return either OK or:
		* 		- SCRIPT_PARSE_ERROR
		**/
		Result loadFromString(const std::string& str) ;

		//! Lua state for all the scripts
		static lua_State*	State;

	private:

		/**
		 * Run one step from the script. This method will be called from taskUpdate()
		 * so the script is get updated stepwise.
		 **/
		Result run();

		/**
		 * Call a certain function from the script
		 **/
		Result callfunc(const std::string& funcName, ScriptResult& result);

		//! keep track of loaded script
		int32 mFuncRef;

		/**
		 * Unload the Script by removing the lua script from the memory
		 **/
		Result unloadResource();
		
		/**
		 * Reload the Script
		 **/
		Result reloadResource();
		
		//! Does always return false, because lua scripts are running completely once 
		bool hasCommands() { return false; }
};

/**
 * New function allowing pop a number from a lua stack
 **/
inline LUA_API lua_Number lua_popnumber(lua_State *L)
{
	register lua_Number tmp = lua_tonumber(L, lua_gettop(L));
	lua_pop(L, 1);
	return tmp;
}

/**
 * Function wrapper to pop a string from a stack
 **/
inline LUA_API const char *lua_popstring(lua_State *L)
{
	register const char *tmp = lua_tostring(L, lua_gettop(L));
	lua_pop(L, 1);
	return tmp;
}

#endif
