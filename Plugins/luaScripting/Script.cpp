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

#include "Script.h"
#include <boost/lexical_cast.hpp>

lua_State*	LuaScript::State = NULL;

#if 1
// Dump all the contents of your lua_state (Lua Stack)
 // Find out the location of ur table and try to print out the details by
 // modifying the below code

 static void stackDump (lua_State *L) {
	 int i;
	 int top = lua_gettop(L);
	 printf("Stack has %d elements\n", top);
	 for (i = 1; i <= top; i++) { /* repeat for each level */
		 int t = lua_type(L, i);
		 switch (t) {

			 case LUA_TSTRING: /* strings */
				 printf("string: `%s'", lua_tostring(L, i));
				 break;

			case LUA_TBOOLEAN: /* booleans */
				 printf(lua_toboolean(L, i) ? "true" : "false");
				 break;

			case LUA_TNUMBER: /* numbers */
				printf("number: %g", lua_tonumber(L, i));
				 break;

			default: /* other values */
				 printf("type: %s", lua_typename(L, t));
				 break;
		 }
		 printf("\n"); /* put a separator */
	 }
	 printf("\n"); /* end the listing */
 }
#endif

//----------------------------------------------------------------------------------
LuaScript::LuaScript() : IScript("LuaScript")
{
	mFuncRef = LUA_NOREF;
}

//----------------------------------------------------------------------------------
LuaScript::~LuaScript()
{

}


//----------------------------------------------------------------------------------
Result LuaScript::loadFromString(const std::string& str)
{
	bool loaded = false;

	// load lua from string
	mContent = str;

	// first check if there is a lua function calling loadScript
	// which is able to load scripts and setup environments
	lua_getglobal(State, "loadstringscript");
	if (lua_isfunction(State, -1))
	{
		// ok the function is defined, so use it to load scripts
		lua_pushstring(State, str.c_str());
		if (lua_pcall(State, 1, 1, 0))
		{
			const char* error = lua_tostring(State, -1);
			lua_pop(State, lua_gettop(State));
			pushErrorMessage(std::string("luaScript: Error in execution in loadstringscript - ") + std::string(error));
			return SCRIPT_ERROR;
		}
		loaded = true;

	// the function is not defined, so load the stuff in the default way
	}else{
		// use the default function instead
		int32 result = luaL_dostring(State, mContent.c_str());
		if (result == LUA_ERRSYNTAX){
			NR_Log(Log::LOG_PLUGIN, Log::LL_ERROR, "luaScript: Error in script %s", getResourceName().c_str());
			return SCRIPT_PARSE_ERROR;
		}else if (result == LUA_ERRMEM){
			NR_Log(Log::LOG_PLUGIN, Log::LL_ERROR, "luaScript: Not enough memory to load script %s", getResourceName().c_str());
			return OUT_OF_MEMORY;
		}
	}

	// now let keep the script in memory
	mFuncRef = luaL_ref(State, LUA_REGISTRYINDEX);
	if (mFuncRef == LUA_REFNIL && loaded){
		NR_Log(Log::LOG_PLUGIN, Log::LL_ERROR, "luaScript: Script could not be compiled %s", getResourceName().c_str());
		return SCRIPT_ERROR;
	}

	notifyLoaded();

	return OK;
}

//----------------------------------------------------------------------------------
Result LuaScript::unloadResource()
{
	/*if (isResLoaded())
	{
		luaL_unref(State, LUA_REGISTRYINDEX, mFuncRef);
		mFuncRef = LUA_NOREF;
	}*/

	return OK;
}

//----------------------------------------------------------------------------------
Result LuaScript::reloadResource()
{
	/*if (!isResLoaded())
	{
		return loadFromString(mContent);
	}*/
	return OK;
}


//----------------------------------------------------------------------------------
Result LuaScript::run()
{
	// check consistency
	if (lua_gettop(State))
	{
		pushErrorMessage(std::string("luaScript: The stack must be empty when calling a function!"));
		stackDump(State);
		return SCRIPT_ERROR;
	}

	// load the script on the top
	lua_rawgeti(State, LUA_REGISTRYINDEX, mFuncRef);
	lua_getfield(State, -1, "main");

	if (lua_isfunction(State, -1))
	{
		// let lua run the script and check for return codes
		int32 res = lua_pcall(State, 0, LUA_MULTRET, 0);

		if (res != 0){
			// get error from the lua stack and give it in the log file
			const char* error = lua_tostring(State, -1);
			lua_pop(State, lua_gettop(State));
			pushErrorMessage(std::string("luaScript: Error in execution - ") + std::string(error));
			return SCRIPT_ERROR;
		}
	}

	// we just run the script, we are not interested in results, so clear the stack
	lua_pop(State, lua_gettop(State));

	return OK;
}


//----------------------------------------------------------------------------------
Result LuaScript::callfunc(const std::string& funcName, ScriptResult& result)
{
	// check consistency
	if (lua_gettop(State))
	{
		pushErrorMessage(std::string("luaScript: The stack must be empty when calling a function!"));
		stackDump(State);
		return SCRIPT_ERROR;
	}

	// load the script on the top
	lua_rawgeti(State, LUA_REGISTRYINDEX, mFuncRef);

	// set the function to be called
	lua_getfield(State, -1, funcName.c_str());

	// check if is a valid function
	if (lua_gettop(State) == 0 || lua_isfunction(State, -1) == 0){
		lua_pop(State, lua_gettop(State));
		pushErrorMessage(std::string("luaScript: Can not find function ") + funcName);
		return SCRIPT_FUNCTION_NOT_FOUND;
	}

	// parameter count
	int32 argn = 0;

	// ok retrieve arguments from the argument list and push them on lua stack
	while (true)
	{
		// get the arguemnt
		std::string type, value;
		int32 count = popArgument(funcName, type, value);
		if (count == -1) break;

		// now check for the type and do conversion
		try{
			if (type == "string")
				lua_pushstring(State, value.c_str());
			else if (type == "number")
				lua_pushnumber(State, boost::lexical_cast<lua_Number>(value));
			else if (type == "bool"){
				if (value == "yes" || value == "true" || value == "on" || value == "1")
					lua_pushboolean(State, 1);
				else
					lua_pushboolean(State, 0);
			}
		} catch (boost::bad_lexical_cast s) {
			pushErrorMessage(std::string("luaScript: Wrong parameter value ") + value + std::string(" of type ") + type);
			return SCRIPT_WRONG_PARAMETER_VALUE;
		}

		// increase argument number
		argn ++;
	}

	// call the function and check for return code
	int32 res = lua_pcall(State, argn, LUA_MULTRET, 0);

	if (res != 0){
		// get error from the lua stack and give it in the log file
		const char* error = lua_tostring(State, -1);
		lua_pop(State, lua_gettop(State));
		pushErrorMessage(std::string("luaScript: Error in execution - ") + std::string(error));
		return SCRIPT_ERROR;
	}

	// now retrieve all results from the stack and return
	// do only get results until the last element on the stack
	// because this is our environment tabkle
	result.empty();
	while (lua_gettop(State) > 1){
		const char* str = lua_popstring(State);
		result = result , std::string(str);
	}

	// remove the last element from the stack
	lua_pop(State, 1);

	return OK;
}
