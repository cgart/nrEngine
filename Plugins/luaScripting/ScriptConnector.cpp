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


//----------------------------------------------------------------------------------
// Includes
//----------------------------------------------------------------------------------
#include "ScriptConnector.h"
#include "Script.h"

//----------------------------------------------------------------------------------
LuaScriptConnector::LuaScriptConnector() : ScriptConnector("LuaScriptConnector")
{
	initialize();
	initializeLuaEnvironment();
}

//----------------------------------------------------------------------------------
void LuaScriptConnector::initializeLuaEnvironment()
{
	// add include function
	lua_pushcfunction(LuaScript::State, includeFile);
	lua_setglobal(LuaScript::State, "include");
}

	
//----------------------------------------------------------------------------------
void LuaScriptConnector::OnRegisterFunction(const std::string& name, const ScriptFunctor& func)
{
	// check if a function already registered
	if (std::find(mRegistered.begin(), mRegistered.end(), name) != mRegistered.end()) return;
	
	// register the function by lua
	lua_pushstring(LuaScript::State, name.c_str());
	lua_pushcclosure(LuaScript::State, callFunc, 1);
	lua_setglobal(LuaScript::State, name.c_str());

	// take this function into the list
	mRegistered.push_back(name);
}

//----------------------------------------------------------------------------------
void LuaScriptConnector::OnRemoveFunction(const std::string& name)
{
	// check if a function already registered
	if (std::find(mRegistered.begin(), mRegistered.end(), name) == mRegistered.end()) return;

	// remove the function from the list
	mRegistered.erase(std::find(mRegistered.begin(), mRegistered.end(), name));
}


//----------------------------------------------------------------------------------
int LuaScriptConnector::includeFile(lua_State *L)
{  
	int n=lua_gettop(L);
	while (n)
	{
		// check if do not have  anil value 
		if (!lua_isnil(L, -n))
		{
			// do we got a string
			if (lua_isstring(L, -n))
			{
				// read string parameter
				std::string filename = lua_tostring(L, -n);
				
				// open filestream and load it
				FileStream* fStream = new FileStream();
				Result ret = fStream->open(filename);
				if (ret == OK)
				{
					// get the whole file content
					std::string str = fStream->getAsString();
			
					// now let lua do the string
					luaL_dostring(L, str.c_str());
				}else{
					printf("ScriptConnector: Cannot load luafile %s, no file found\n", filename.c_str());
				}
				delete fStream;
			}else{
				printf("ScriptConnector: Cannot include luafile. Filename(string) must be given. You gave me %s:%p\n",luaL_typename(L,-n),lua_topointer(L,-n));
				return 0;
			}
		}
		--n;
	}
	
	// clear the stack
	lua_pop(L, lua_gettop(L));
	
	return 0;
}


//----------------------------------------------------------------------------------
int LuaScriptConnector::callFunc(lua_State* L)
{

	// THIS IS A HACK! I DO NOT KNOW HOW TO DO IT BETTER!
	lua_getupvalue(L, -(lua_gettop(L) + 1), 1);
	
	// prepare the vector with argument for the registered function
	std::vector<std::string> values;
	std::vector<std::string> args;

	// read out the whole stack
	while (lua_gettop(L) > 0)
	{
		const char* str = lua_popstring(L);
		values.push_back(std::string(str));
	}

	// check that the size is valid
	if (values.size() == 0) return 0;

	// get the function name and copy the arguments
	std::string funcName = values[0];
	args.push_back(funcName);	
	for (int32 i = (int32)values.size() - 1; i > 0; i--){
		args.push_back(values[i]);
	}
	
	// call the function from the script engine
	ScriptResult res = Engine::sScriptEngine()->call(funcName, args);

	// push the result on the stack
	if (res.size()){
		lua_pushstring(L, (res.get<std::string>(0)).c_str());
		lua_error(L);
		return 1;
	}

	// clear the stack
	lua_pop(L, lua_gettop(L));
	
	// return the number of results
	return 0;	
}

