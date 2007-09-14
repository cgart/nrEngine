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
#include <nrEngine/ScriptEngine.h>
#include <nrEngine/Log.h>
#include <nrEngine/events/EngineEvent.h>
#include <nrEngine/EventManager.h>
#include <nrEngine/VariadicArgument.h>
#include <nrEngine/IScript.h>

namespace nrEngine{

	//----------------------------------------------------------------------------------
	ScriptFunctionDec(scriptLoad, ScriptEngine)
	{
		// check parameter count
		if (args.size() < 2){
			return ScriptResult(std::string("Not valid parameter count! Parameters (name, filename)"));
		}

		// get parameters
		IResourcePtr ptr = Engine::sScriptEngine()->load(args[1], args[2]);

		if (ptr.isNull())
		{
			return ScriptResult(std::string("Can not load script ") + args[1] + " from file " + args[2]);
		}

		return ScriptResult();
	}

	//----------------------------------------------------------------------------------
	ScriptFunctionDec(scriptRun, ScriptEngine)
	{
		// check parameter count
		if (args.size() < 1){
			return ScriptResult(std::string("Not valid parameter count! Parameters (name [, runonce = true [, immediate = false]])"));
		}

		// get parameters
		try{
			const std::string& name = args[1];
			bool once = true;
			if (args.size() > 2) once = boost::lexical_cast<bool>(args[2]);
			bool force = false;
			if (args.size() > 3) force = boost::lexical_cast<bool>(args[3]);

			// run
			if (!Engine::sScriptEngine()->execute(name, once, force))
				return ScriptResult(std::string("Can not execute script ") + name);
		}catch(boost::bad_lexical_cast s){
			return ScriptResult(std::string("Wrong parameter value! "));
		}
		
		return ScriptResult();
	}

	//----------------------------------------------------------------------------------
	ScriptFunctionDec(scriptLoadAndRun, ScriptEngine)
	{
		// check parameter count
		if (args.size() < 2){
			return ScriptResult(std::string("Not valid parameter count! Parameters (name, filename)"));
		}

		// get name and filename
		const std::string& name = args[1];
		const std::string& fileName = args[2];
		
		// create argument for calling
		std::vector<std::string> arg;
		arg.push_back(args[0]);
		arg.push_back(name);
		arg.push_back(fileName);
		
		// call load method
		ScriptResult res = scriptLoad(arg, param);
		
		// create arguments for run method 
		arg.pop_back();
		
		// call run if load was successfull
		if (res.size() == 0) res = scriptRun(arg, param);
		
		return res;
	}

	//----------------------------------------------------------------------------------
	ScriptFunctionDec(scriptCall, ScriptEngine)
	{
		// check parameter count
		if (args.size() < 2){
			return ScriptResult(std::string("Not valid parameter count! Parameters (name, funcname, [, param1, ..., paramn])"));
		}
		
		IScript::ArgumentList ar;
			
		// we get parameters, so extract them
		if (args.size() >= 3 )
		{
			for (uint32 i=3; i < args.size(); i++)
			{
				// get value 
				std::string param = args[i];
				
				// check if it contains ':' character
				std::string::size_type pos = param.find(':');
				if (pos == std::string::npos)
				{
					char msg[4096];
					sprintf(msg, "ScriptEngine: Parameter %s does not contain type information", param.c_str());
					return ScriptResult(std::string(msg));
				}
				
				// split on that character 
				std::string type = param.substr(pos + 1);
				std::string value = param.substr(0, pos);
	
				// store them
				ar.push_back(std::pair<std::string, std::string>(type, value));
			} 
		}

		// get resource
		ResourcePtr<IScript> ptr = Engine::sResourceManager()->getByName(args[1]);
		if (ptr.isNull())
			return ScriptResult(std::string("Can not find script ") + args[1]);

		// now prepare arguments
		ptr->setArguments(args[2], ar);
		ScriptResult res;
		ptr->call(args[2]);
		
		// call a specific function
		return res;
	}
	
	//----------------------------------------------------------------------------------
	ScriptEngine::ScriptEngine(){
		mDatabase["scriptLoad"].first = scriptLoad;
		mDatabase["scriptRun"].first = scriptRun;
		mDatabase["scriptExecute"].first = scriptRun;
		mDatabase["scriptCall"].first = scriptCall;
		mDatabase["scriptLoadAndRun"].first = scriptLoadAndRun;
	}

	//----------------------------------------------------------------------------------
	ScriptEngine::~ScriptEngine(){

		// empty the database
		mDatabase.clear();

	}

	//----------------------------------------------------------------------------------
	Result ScriptEngine::add(const std::string& name, ScriptFunctor func, const std::vector<ScriptParam>& param)
	{
		// first check whenever such function already registered
		if (isRegistered(name))
			return SCRIPT_FUNCTION_REGISTERED;

		// now add the function to the database
		mDatabase[name].first = func;
		mDatabase[name].second = param;

		// some statistical information
		NR_Log(Log::LOG_ENGINE, Log::LL_DEBUG, "ScriptEngine: New function \"%s\" registered", name.c_str());

		// emit a new event
		SharedPtr<Event> msg(new ScriptRegisterFunctionEvent(name, func));
		Engine::sEventManager()->emitSystem(msg);
		
		return OK;
	}

	//----------------------------------------------------------------------------------
	Result ScriptEngine::add(const std::string& name, ScriptFunctor func, const VarArg& v){

		// get the elements and store them in vector
		std::vector<ScriptParam> p;
		v.convert<ScriptParam>(p);

		// OK
		return add(name, func, p);
	}

	//----------------------------------------------------------------------------------
	Result ScriptEngine::del(const std::string& name)
	{
		// get the function
		FunctionDatabase::iterator it = mDatabase.find(name);
		if (it == mDatabase.end()) return SCRIPT_FUNCTION_NOT_REGISTERED;

		mDatabase.erase(it);

		// some statistical information
		NR_Log(Log::LOG_ENGINE, Log::LL_DEBUG, "ScriptEngine: Function \"%s\" was removed", name.c_str());

		// emit a new event
		SharedPtr<Event> msg(new ScriptRemoveFunctionEvent(name));
		Engine::sEventManager()->emitSystem(msg);

		return OK;
	}

	//----------------------------------------------------------------------------------
	ScriptResult ScriptEngine::call(const std::string& name, const std::vector<std::string>& args)
	{
		// if no such function was found
		FunctionDatabase::iterator f = get(name);
		if (f == mDatabase.end()){
			NR_Log(Log::LOG_ENGINE, Log::LL_WARNING, "ScriptEngine: Function \"%s\" was not found!", name.c_str());
			return ScriptResult();
		}
		
		// debug information 
		std::string msg;
		for (unsigned int i=1; i < args.size(); i++) msg += std::string(" ") + args[i];
		NR_Log(Log::LOG_ENGINE, Log::LL_DEBUG, "ScriptEngine: Call \"%s (%s)\" function!", name.c_str(), msg.c_str());

		// call the function
		return ((*f).second).first(args, (*f).second.second);
	}

	//----------------------------------------------------------------------------------
	bool ScriptEngine::isRegistered(const std::string& name)
	{
		// search for that name in the database
		FunctionDatabase::const_iterator it = mDatabase.find(name);
		return it != mDatabase.end();
	}

	//----------------------------------------------------------------------------------
	ScriptEngine::FunctionDatabase::iterator ScriptEngine::get(const std::string& name)
	{
		// search for that name in the database
		FunctionDatabase::iterator it = mDatabase.find(name);

		return it;
	}

	//------------------------------------------------------------------------
	ResourcePtr<IScript> ScriptEngine::load(const std::string& name, const std::string& file)
	{
		// try to load the resource and check for the error code
		ResourcePtr<IScript> plg = Engine::sResourceManager()->loadResource(name, "Scripts", file);

		if (plg.isNull()){
			NR_Log(Log::LOG_ENGINE, Log::LL_ERROR, "Can not load the script %s from %s", name.c_str(), file.c_str());
		}
		return plg;
	}


	//------------------------------------------------------------------------
	Result ScriptEngine::execute(const std::string& name, bool runOnce, bool immediate)
	{
		// get the pointer to the script resource
		ResourcePtr<IScript> scr = Engine::sResourceManager()->getByName(name);
		return _execute(scr, runOnce, immediate);		
	}

	//------------------------------------------------------------------------
	Result ScriptEngine::execute(const std::string& name, const std::string& fileName, bool runOnce, bool immediate)
	{
		ResourcePtr<IScript> scr = load(name, fileName);
		return _execute(scr, runOnce, immediate);
	}

	//------------------------------------------------------------------------
	Result ScriptEngine::_execute(const ResourcePtr<IScript>& ptr, bool runOnce, bool immediate)
	{
		if (ptr.isNull()) return RES_NOT_FOUND;
		if (immediate) return ptr->forceExecute(runOnce);
		return ptr->execute(runOnce);
	}

	//------------------------------------------------------------------------
	const std::string& ScriptEngine::getFunction(uint32 index, ScriptFunctor& functor)
	{
		NR_ASSERT(index < getFunctionCount());
		FunctionDatabase::const_iterator it = mDatabase.begin();
		for (uint32 i = 0; i < index; i++, it++){}

		functor = it->second.first;
		return it->first;
	}

};

