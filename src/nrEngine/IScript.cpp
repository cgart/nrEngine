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
#include <nrEngine/IScript.h>
#include <nrEngine/Log.h>
#include <nrEngine/Kernel.h>

namespace nrEngine{


	//--------------------------------------------------------------------
	void IScript::notifyLoaded()
	{
		markResourceLoaded();
	}

	//--------------------------------------------------------------------
	Result IScript::onStartTask()
	{
		// notice derived classes about starting
		onStartScript();

		return OK;
	}

	//--------------------------------------------------------------------
	Result IScript::execute(bool runOnce)
	{
		// set running counter parameter
		mbRunOnce = runOnce;

		// if task is already running, so do not do anything
		if (getTaskState() == TASK_RUNNING) return SCRIPT_ALREADY_RUNNING;

		// add the task to the kernel
		// also kernel is not allowed to remove this object, because
		// we created it from this, so no reference counting is used
		SharedPtr<ITask> task (this, null_deleter());
		Engine::sKernel()->AddTask(task, ORDER_NORMAL);

		return OK;
	}

	//--------------------------------------------------------------------
	Result IScript::forceExecute(bool runOnce)
	{
		mbRunOnce = runOnce;

		// we only run the script nothing more
		if (runOnce) return run();

		// we add it to the kernel and run it first
		return execute(runOnce);
	}

	//--------------------------------------------------------------------
	Result IScript::updateTask()
	{
		// run the script
		Result ret = run();

		// if script should run once, so check if it has still commands
		if (shouldRunOnce())
		{
			// check if script still have some commands
			if (!hasCommands())
			{
				Engine::sKernel()->RemoveTask(this->getTaskID());
			}
		}

		// return result
		return ret;
	}

	//--------------------------------------------------------------------
	IScript::~IScript()
	{

	}

	//--------------------------------------------------------------------
	IScript::IScript(const std::string& name) : IResource(name)
	{
		mbRunOnce = true;
		setTaskName(std::string("Script_") + name);
	}

	//--------------------------------------------------------------------
	uint32 IScript::pushArgument(const std::string& funcName, const std::string& type, const std::string& value)
	{
		// store the new argument in the database
		mArguments[funcName].push_back(std::pair<std::string,std::string>(type, value));

		// get the number of arguments for the function
		return mArguments[funcName].size();
	}

	//--------------------------------------------------------------------
	uint32 IScript::setArguments(const std::string& funcName, const ArgumentList& args)
	{
		mArguments[funcName] = args;
		return mArguments[funcName].size();
	}

	//--------------------------------------------------------------------
	int32 IScript::popArgument(const std::string& funcName, std::string& type, std::string& value)
	{
		// first check if such a function does exists
		ArgumentDatabase::iterator it = mArguments.find(funcName);
		if (it == mArguments.end()) return -1;

		// check the number of elements
		if (it->second.size() == 0) return -1;

		// pop the element and return the number of remaining elements
		const std::pair<std::string, std::string>& first = it->second.front();
		type = first.first;
		value = first.second;

		it->second.pop_front();

		return it->second.size();
	}

	//----------------------------------------------------------------------------------
	ScriptResult IScript::call(const std::string& funcName)
	{
		// call a certain function
		ScriptResult res;
		Result ret = callfunc(funcName, res);
		if (ret == OK) return res;

		// we got an error, so throw an error
		NR_EXCEPT(ret, mErrorMsg.front(), "IScript::call()");
	}

	//----------------------------------------------------------------------------------
	Result IScript::callfunc(const std::string& funcName, ScriptResult& result)
	{
		return OK;
	}

	//----------------------------------------------------------------------------------
	std::string IScript::popLastError()
	{
		if (mErrorMsg.size() == 0) return std::string();
		std::string msg = mErrorMsg.front();
		mErrorMsg.pop_front();
		return msg;
	}

	//----------------------------------------------------------------------------------
	void IScript::pushErrorMessage(const std::string& sh)
	{
		mErrorMsg.push_front(sh);
		NR_Log(Log::LOG_CONSOLE, Log::LL_ERROR, "%s", sh.c_str());
	}

	//----------------------------------------------------------------------------------
	// ----------------- Implementation of the empty script object ---------------------
	//----------------------------------------------------------------------------------
	//----------------------------------------------------------------------------------
	EmptyScript::EmptyScript(const std::string& typeName) : IScript(typeName + "Empty")
	{

	}

	//----------------------------------------------------------------------------------
	EmptyScript::~EmptyScript()
	{

	}

	//----------------------------------------------------------------------------------
	Result EmptyScript::loadFromString(const std::string& str)
	{
		return OK;
	}

	//----------------------------------------------------------------------------------
	Result EmptyScript::run()
	{
		return OK;
	}


}; // end namespace

