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
#include <nrEngine/Script.h>
#include <nrEngine/ScriptEngine.h>
#include <nrEngine/Clock.h>
#include <nrEngine/Log.h>
#include <nrEngine/Kernel.h>
#include <boost/algorithm/string/trim.hpp>

namespace nrEngine {

	//----------------------------------------------------------------------------------
	Script::Script() : IScript("Script")
	{
		mLastLine = 0;
		mLoop = NO_LOOP;
		mTime = 0;
		bRunStepwise = true;
		mStopTime = 0;
		mRunningTimeLength = 0;
		mFirstRunTimed = true;
		mScriptStartTime = 0;
	}

	//----------------------------------------------------------------------------------
	Script::~Script()
	{
		unloadResource();
	}


	//----------------------------------------------------------------------------------
	Result Script::loadFromString(const std::string& str)
	{
		// set task name as resource name
		setTaskName(getResourceName() + "_ScriptTask");

		// setup content
		mContent = str;

		// parse it
		Result ret = parse(mContent);
		if (ret == OK) notifyLoaded();

		return ret;
	}

	//------------------------------------------------------------------------------
	std::string Script::cleanScript(const std::string& script)
	{
		// check the lenght of the script
		std::stringstream str(script);
		if (script.length() == 0 || !str.good()) return script;
		std::string resultScript;

		// get linewise
		char buf[2048];
		std::string buffer;
		std::string::size_type pos;
		std::string line;

		// for the whole stream do following
		while (!str.eof())
		{
			// get line
			str.getline(buf, 2048);
			buffer = std::string(buf);

			// remove comments
			pos = buffer.find("//");
			if (pos == std::string::npos)
				line = buffer;
			else
				line = buffer.substr(0, pos);

			// trim the line
			boost::algorithm::trim(line);

			// if the line is empty, so get the next one
			if (line.length() == 0) continue;

			// if we are here, so we do not have any comments or empty lines
			resultScript += line;
			if (!str.eof()) resultScript += std::string("\n");
		}

		return resultScript;
	}

	//------------------------------------------------------------------------------
	std::string Script::parseSubscripts(const std::string& script)
	{
		// vars
		int depth = 0, count = 0, curdepth = 0;

		// for parsing we are using stack-algorithm
		//std::vector<std::string> stack;
		//stack.push_back(std::string(""));
		//stack.push_back(std::string(""));

		std::string main;// = &stack[0];
		std::string sub;//  = &stack[1];
		std::string* cur  = &main;
		
		for (unsigned int i=0; i < script.length(); i++)
		{
			if (script.at(i) == '{') curdepth ++;
			
			if (script.at(i) == '{' && cur == &main && curdepth == 1)
			{
				cur = &sub;
				depth ++;
			}
			else if (script.at(i) == '}' && cur == &sub && curdepth == 1)
			{
				count ++;
				depth --;
				
				// create name for the new subscript
				std::stringstream subscriptName;
				subscriptName << getResourceName() << std::string("_sub_") << count ;

				// current top element is a new script, so create it
				ResourcePtr<IScript> scr = Engine::sResourceManager()->createResource(subscriptName.str(), getResourceName(), "Script");

				// if script could be created, so fill it and store, to execute later
				if (scr.valid())
				{
					scr.lockResource();
						scr->loadFromString(sub);
					scr.unlockResource();
					mSubscripts.push_back(scr);
				}else{
					NR_Log(Log::LOG_ENGINE, Log::LL_ERROR, "Script (%s): Subscript could not be created. Maybe no proper loader exists. Script will not run.\n", getResourceName().c_str());
					return std::string("");
				}
				
				// reset pointer back to main script and empty the subscript string
				cur = &main;
				sub = std::string("");
			}
			else
			{
				// write character into valid script
				*cur += script.at(i);
			}
			
			if (script.at(i) == '}') curdepth --;
		}
		
		// check if closing brackets were complete
		if (depth != 0 || curdepth != 0)
		{
			NR_Log(Log::LOG_ENGINE, Log::LL_ERROR, "Script (%s): Subscript-brackets were not used properly. Depth is %d/%d, must be 0/0. Script disabled!", getResourceName().c_str(), depth, curdepth);
			return std::string("");
		}

		//printf("\n%s\n%s\n\n", getResourceName().c_str(), main.c_str());
		
		return main;
		
		/*std::string* top = &stack[stack.size()-1];

		// go through the given script
		for (unsigned int i=0; i < script.length(); i++)
		{
			top = &(stack[stack.size()-1]);
			if (script.at(i) == '{')
			{
				depth ++;
				stack.push_back(std::string(""));
			}
			else if (script.at(i) == '}')
			{
				depth --;
				count ++;

				if (depth >= 0)
				{
					// create name for the new subscript
					std::stringstream subscriptName;
					subscriptName << getResourceName() << std::string("_sub_") << count << std::string("_") << depth;

					// current top element is a new script, so create it
					ResourcePtr<IScript> scr = Engine::sResourceManager()->createResource(subscriptName.str(), getResourceName(), "Script");

					// if script could be created, so fill it and store, to execute later
					if (scr.isNull())
					{
						NR_Log(Log::LOG_ENGINE, Log::LL_ERROR, "Script (%s): Subscript could not be created. Maybe no proper loader exists. Script will not run.\n", getResourceName().c_str());
						return std::string("");
					}else{
						scr.lockResource();
							scr->loadFromString(*top);
						scr.unlockResource();
						mSubscripts.push_back(scr);
					}

					// remove top element
					stack.pop_back();
				}
			}else{
				*top += script.at(i);
			}
		}


		// check if closing brackets were complete
		if (depth != 0)
		{
			NR_Log(Log::LOG_ENGINE, Log::LL_WARNING, "Script (%s): Subscript-brackets were not used properly. Depth is %d, must be 0. Script could not run as expected!", getResourceName().c_str(), depth);
		}

		// return mainscript back
		return cleanScript(stack[0]);*/
	}

	//------------------------------------------------------------------------------
	Result Script::parse(const std::string& script)
	{
		// remove all non-commands from the script
		// check for subscripts
		std::stringstream str(parseSubscripts(cleanScript(script)));
		if (script.length() == 0 || !str.good()) return OK;

		//printf("%s: \n%s\n---\n\n", getResourceName().c_str(),str.str().c_str());
		// get linewise
		char buf[1024];
		std::string::size_type pos;
		std::string line;

		while (!str.eof())
		{
			// get line
			str.getline(buf, 1024);
			line = std::string(buf);

			// check for script parameters
			pos = line.find('^');
			if (pos != std::string::npos)
			{
				// get parameter
				std::string sparam = line.substr(pos+1);
				boost::algorithm::trim(sparam);
				
				// now tokenize parameter, to get special argument values for the parameters
				std::vector<std::string> args;
				std::string param;
				tokenize(sparam, param, args); 
				
				// set parameters
				if (!setParameter(param, args))
				{
					NR_Log(Log::LOG_ENGINE, Log::LL_ERROR, "Script: Unknown local parameter %s in line \"%s\"", param.c_str(), line.c_str());
					return SCRIPT_PARSE_ERROR;
				}
				continue;
			}

			// check for the timestamp
			pos = line.find('|');
			Command cmd;
			cmd.timestamp = 0;
			cmd.estimatedStart = 0;

			// timestamp found, so do extra stuff with this kind of commands
			if(pos != std::string::npos )
			{
				// retrieve the timestamp and the command and store it into the command buffer
				std::string time = line.substr(0,pos);
				boost::algorithm::trim(time);
				std::string command = line.substr(pos+1);

				// try to convert the time and add the command to the list
				try{
					cmd.timestamp = boost::lexical_cast<float32>(time);
					tokenize(command, cmd.cmd, cmd.args);
					if (cmd.args.size() > 0 && cmd.args[0].length() > 0)
					{
						if (cmd.cmd == std::string("_stop_"))
						{
							mRunningTimeLength = cmd.timestamp;
						}else
							mTimedCommand.push_back(cmd);
					}
				}catch(...){
					NR_Log(Log::LOG_ENGINE, Log::LL_ERROR, "Script: Unknown syntax in \"%s\"\n", line.c_str());
					return SCRIPT_PARSE_ERROR;
				}

			// timestamp not found, so it is a simple command
			}else{
				tokenize(line, cmd.cmd, cmd.args);
				if (cmd.args.size() > 0 && cmd.args[0].length() > 0)
				{
					mCommand.push_back(cmd);
				}
			}
		}

		resetCommandFifo();

		// if we got any command so we are valid
		return OK;
	}

	//------------------------------------------------------------------------------
	void Script::tokenize(const std::string& str, std::string& cmd, std::vector<std::string>& tokens)
	{
		// do not tokenize if we do not get any token
		if (str.length() == 0) return;

		using namespace std;

		// tokenize on space characters
		const string delimiters = " ";

		// Skip delimiters at beginning.
		string::size_type lastPos = str.find_first_not_of(delimiters, 0);

		// Find first "non-delimiter".
		string::size_type pos     = str.find_first_of(delimiters, lastPos);

		while (pos != string::npos || lastPos != string::npos)
		{
			// Found a token, add it to the vector.
			if (pos  - lastPos > 0)
				tokens.push_back(str.substr(lastPos, pos - lastPos));

			// Skip delimiters.  Note the "not_of"
			lastPos = str.find_first_not_of(delimiters, pos);

			// Find next "non-delimiter"
			pos = str.find_first_of(delimiters, lastPos);
		}

		// now the first element in the token list is the command name
		cmd = tokens[0];
	}

	//------------------------------------------------------------------------------
	bool Script::setParameter(const std::string& param, const std::vector<std::string>& args){

		if (param == "loop")
			mLoop |= LOOP;
		else if (param == "loop_seq")
			mLoop |= LOOP_COMMAND;
		else if (param == "loop_timed")
			mLoop |= LOOP_TIMED_COMMAND;
		else if (param == "runcomplete")
			bRunStepwise = false;
		//else if (param == "runonce")
		//	setRunOnce(true);
		else if (param == "start_at")
			try{
				if (args.size() > 1) mScriptStartTime = boost::lexical_cast<float32>(args[1]);
				else throw boost::bad_lexical_cast();
			}catch(boost::bad_lexical_cast &){
				NR_Log(Log::LOG_ENGINE, Log::LL_WARNING, "Script: %s - not a valid value for \"start_at\" parameter specified", getResourceName().c_str());
			}
		else
			return false;

		return true;
	}

	//------------------------------------------------------------------------------
	void Script::resetCommandFifo()
	{
		// the untimed command fifo is very simple
		// the commands are executed in the way they were found in the file
		mCommandFifo.clear();
		for (uint32 i=0; i < mCommand.size(); i++){
			mCommandFifo.push_front(i);
		}
	}

	//------------------------------------------------------------------------------
	void Script::resetTimedCommandFifo(bool firstReset)
	{
		if (firstReset) mTimedCommandFifo.clear();

		// timed commands could only be resetted when they
		// are already executed
		
		// the timed commands are also setted in the fifo
		// the estimated end time is also computed
		for (int32 i=0; i < (int32)mTimedCommand.size(); i++)
		{
			// check whenever fifo does not contain this command 
			std::list< int32 >::iterator it = std::find(mTimedCommandFifo.begin(), mTimedCommandFifo.end(), i);
			
			// if such command not exists, then reset it
			if (it == mTimedCommandFifo.end() || firstReset)
			{
				mTimedCommandFifo.push_front(i);
				mTimedCommand[i].estimatedStart = mTimer->getTime() + mTimedCommand[i].timestamp;
			}
		}
	}

	//----------------------------------------------------------------------------------
	Result Script::step()
	{
		// update time
		mTimer->setPause(false);
		mTime = mTimer->getTime();
		
		//printf("%s (%f - %f)\n", getResourceName().c_str(), mTime, mScriptStartTime);

		// do execute the script only if start time was passed
		if (mScriptStartTime > mTime)
		{
			// if the script is waiting for the start, then it runs in a loop mode 
			setRunOnce(false);
			
			// do nothing at now
			return OK;
		}
		
		// if the script running first time, then do reset on timed commands
		if (mFirstRunTimed)
		{
			// initialize timed commands
			mFirstRunTimed = false;
			resetTimedCommandFifo(true);
			setRunOnce(true);
		}
		
		// compute stop time, when to stop the script if such defined
		if (mStopTime == 0 && mRunningTimeLength > 0) mStopTime = mTime + mRunningTimeLength;
		
		// ----- timed commands (quasi parallel execution) -----
		// scan through the command fifo
		std::list<int32>::iterator it;
		for (it = mTimedCommandFifo.begin(); it != mTimedCommandFifo.end(); )
		{
			int32 id = *it;
			mTimedCommand[id].time = mTime;
			
			//std::string msg;
			//for (unsigned int i=1; i < mTimedCommand[id].args.size(); i++) msg += std::string(" ") + mTimedCommand[id].args[i];
			//printf("%s (%s): %f %f\n", mTimedCommand[id].cmd.c_str(), msg.c_str(), mTimedCommand[id].estimatedStart, mTimedCommand[id].time);

			// if the estimated start time is reeached, so start the command and remove it from the queue
			if (mTimedCommand[id].estimatedStart < mTimedCommand[id].time)
			{
				// remove the command from the fifo
				it = mTimedCommandFifo.erase(it);

				// call the commando
				Engine::sScriptEngine()->call(mTimedCommand[id].cmd, mTimedCommand[id].args);
			}else
				it ++;
		}

		// ----- sequentiall commands -----
		if (mCommandFifo.size())
		{
			// get the id of the command
			int32 id = mCommandFifo.back();
			mCommandFifo.pop_back();

			// check if we have a stop commando
			if (mCommand[id].cmd != std::string("_stop_"))
			{
				Engine::sScriptEngine()->call(mCommand[id].cmd, mCommand[id].args);
			}else{
				Engine::sKernel()->RemoveTask(this->getTaskID());
			}
		}

		// reset the command list
		reset();

		// check whenever execution time of the script exceeds
		if (mTime > mStopTime && mStopTime > 0)
		{
			Engine::sKernel()->RemoveTask(this->getTaskID());
		}

		//printf("\n\n");
		
		return OK;
	}

	//----------------------------------------------------------------------------------
	bool Script::hasCommands()
	{
		if (mCommandFifo.size() == 0 && mTimedCommandFifo.size() == 0 && !(mLoop & LOOP_COMMAND) && !(mLoop & LOOP_TIMED_COMMAND))
			return false;

		return true;
	}

	//----------------------------------------------------------------------------------
	Result Script::run()
	{
		// check whenver this script is running stepwise
		if (bRunStepwise) return step();

		// make full run of the script
		return fullRun();
	}


	//----------------------------------------------------------------------------------
	Result Script::fullRun()
	{

		// check for looped script
		static bool warned = false;
		if (!warned && mLoop & LOOP_COMMAND){
			warned = true;
			NR_Log(Log::LOG_ENGINE, Log::LL_WARNING, "Script: %s - you are executing a looped script!!! Check this!!!", getResourceName().c_str());
		}

		// loop the content of the script if script should be looped
		bool bLoop = false;
		do {
			// we go through the whole command list and execute it.
			while (mCommandFifo.size() > 0)
			{
				// get the id of the command
				int32 id = mCommandFifo.back();
				mCommandFifo.pop_back();

				// execute it
				Engine::sScriptEngine()->call(mCommand[id].cmd, mCommand[id].args);
			}

			// reset the list and check if the task is removed from the kernel
			reset();
			bLoop = hasCommands();

		} while(bLoop && (mLoop & LOOP_COMMAND));

		return OK;
	}

	//--------------------------------------------------------------------
	void Script::onStartScript()
	{
		// do run all subscripts specified in this script
		for (unsigned int i = 0; i < mSubscripts.size(); i++)
		{
			// cast subscripts to script objects
			Script* subscript = dynamic_cast<Script*>(mSubscripts[i].get());
			if (subscript)
			{
				// first set starting time of subscripts relative to the main script 
				subscript->mScriptStartTime += mScriptStartTime;
				
				// now execute the script 
				subscript->execute();
				
				// setup dependencies
				addTaskDependency(subscript->getTaskID());
			}
			
		}
		
		// intialize the timer and pause it
		mTimer = Engine::sClock()->createTimer();
		mTimer->setPause(true);
		
		// reset the script, so it can run
		resetCommandFifo();
		resetTimedCommandFifo(true);
		mFirstRunTimed = true;
	}

	//----------------------------------------------------------------------------------
	bool Script::reset()
	{
		if (mCommandFifo.size() == 0 && (mLoop & LOOP_COMMAND)){
			resetCommandFifo();
		}else if (mTimedCommandFifo.size() == 0 && (mLoop & LOOP_TIMED_COMMAND)){
			resetTimedCommandFifo(false);
		}
		return true;
	}

};

