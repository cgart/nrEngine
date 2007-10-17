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


#ifndef _NR_SCRIPT_RESOURCE__H_
#define _NR_SCRIPT_RESOURCE__H_


//----------------------------------------------------------------------------------
// Includes
//----------------------------------------------------------------------------------
#include "Prerequisities.h"
#include "IScript.h"

namespace nrEngine{


	//! Simple script object based on engine's simple script language
	/**
	 * This is a simple script language object. We use this language as engine's
	 * default one to load other more powerfull script languages. This languages
	 * could be found in plugins.
	 *
	 * Also simple scripts can be used to setup some variables or to write config
	 * files that will be used to setup the engine's environment. This config files
	 * could also contains plugins loading and file system setup. So actually this
	 * simple language is enough to write simple applications.
	 *
	 * The scripts have very simple syntax:
	 *  - each line is a script command
	 *  - the line is bypassed to the script engine (so commands must be registered or it will cause no effect)
	 *  - one command per one frame (if not other specified)
	 *  - commands can be bound to a certain time (execute command at this time)
	 *  - time values are always relative to the start time of the script
	 *  - nontimed commands are executed sequentially
	 *  - timed commands are executed quasi parallel to the nontimed
	 *  - if one script calls another one, so the new one runs in parallel
	 *
	 *
	 * Example of such a script (I called this timeline scripts):
	 *
	 *  seq_command param1 // execute seq_command sequantially with param1 as first argument
	 *  1.452 | cmd1 par1	// execute the command cmd1 with parameter par1 at the time 1.452s (after starting of the script)
	 *  1.693 | run script/script2.tml // execute script script/script2.tml after the time reaches 1.693
 	 *
	 * Scripts could include subscripts. Subscripts are defined between
	 * {} - brackets. The string between this brackets is used as a scipts
	 * as if it was used in an explicit file. So the string is parsed
	 * as a script and runs also parallel to the parent script. For timed
	 * and non-timed commands the same restrictions are used as in non-subscripts.
	 * Subscripts could define new subscripts. Because scripts are running as task, 
	 * we add a task dependency between parent script and subscript. So parent script 
	 * depends on subscript. This means parent script can only step forward after
	 * subscripts has done their steps.
	 *
 	 * NOTE:
	 * 	- We represent each script as a task, so the scripts are running in parallel
	 *	- This type of script can run stepwise which is default behaviour. Specify it in
	 * 		another way if you want to run the script completely in one frame.
	 *
	 * @see IScript
	 * \ingroup script
	**/
	class _NRExport Script : public IScript{
		public:

			//! Allocate memory and initilize simple script
			Script();

			//! Deallocate memory and release used data
			~Script();


			/**
			* Load simple script language from a string.
			* @copydoc IScript::loadFromString()
			**/
			Result loadFromString(const std::string& str);

			/**
			 * Execute the script completely. So the script will be
			 * executed until it finishes. This function will lock the execution
			 * of the engine while the script is running.
			 *
			 * NOTE: Timed commands used in nrScript would not been executed
			 * 		here. This is because of the time which will not be updated
			 * 		while the script is running. Updating the time, means either to
			 * 		update the clock or the kernel. This is not allowed by tasks,
			 * 		it means tasks can not update them self only kernel can do this.
			 *
			 * 		So if you call execute() so only sequential commands will be
			 * 		executed!
			 *
			 * NOTE: Be carefull by using looped scripts. If there is a loop, so the
			 * 		script will also be executed in loop mode, so if you do not stop it
			 * 		somehow your application could not react anymore!!!
			 *		If the script is looped, so warnign will be printed in a log file!
			**/
			Result fullRun();

			/**
			 * Set if this script should run stepwise or completely in
			 * one cycle. If you run the script stepwise, so each line
			 * of the script is executed in one kernel cycle. Also time commands
			 * are getting executed as soon, as according time is passed.
			 *
			 * If you run the script fully in one cycle, so look to fullRun()
			 **/
			NR_FORCEINLINE void setRunStepwise(bool b) { bRunStepwise = b; }

			/**
			 * Check whenever the script is running stepwise
			 **/
			NR_FORCEINLINE bool isRunStepwise() const { return bRunStepwise; }

		private:

			//! Return true if pipeline is full
			bool hasCommands();

			//! Script was started
			void onStartScript();

			//! Derived from IScript
			Result run();

			//!  Parse the given string as nrScript language
			Result parse(const std::string&);

			//! Remove all comments and empty lines from the script
			std::string cleanScript(const std::string&);

			//! Parse subscripts and return non-sub-script back
			std::string parseSubscripts(const std::string&);

			//! Set certain parameter from the script
			bool setParameter(const std::string& param, const std::vector<std::string>& args);

			//! Reset the sequentiall command fifo
			void resetCommandFifo();

			//! Reset the timed command fifo
			void resetTimedCommandFifo(bool firstReset = false);

			//! Tokenize the given line into command and arguments
			void tokenize(const std::string& line, std::string& cmd, std::vector<std::string>& args);

			//! Reset the command lists (if return false, so the task is removed from kernel)
			bool reset();

			//! Script is valid
			bool mValid;

			//! Error in line
			int32 mLastLine;

			//! Subscripts which belongs to this one
			std::vector<ResourcePtr<IScript> > mSubscripts;

			//! Time script have to stop after this duration
			float32 mRunningTimeLength;

			//! Time when script started
			float32 mStopTime;

			//! Run teh script stepwise
			bool bRunStepwise;

			//! Each command has this type
			typedef struct _cmd{

				//! Timestamp ( = 0 -> command is not timed
				float32 timestamp;

				//! estimated starting time based on the global clock time
				float32 estimatedStart;

				//! current time of the command
				float32 time;

				//! function name
				std::string cmd;

				//! arguments of the command
				std::vector<std::string> args;
			} Command;

			//! Here we store our timed commands
			std::vector< Command > mTimedCommand;

			//! Here we store our sequentiall commands
			std::vector< Command > mCommand;

			//! Store command queue of waiting command ids
			std::list< int32 > mCommandFifo;

			//! Store timed commands sorted by their time in the queue
			std::list< int32 > mTimedCommandFifo;

			//! Script looping modes
			typedef enum _loopMode{
				NO_LOOP = 0,
				LOOP_COMMAND = 1 << 0,
				LOOP_TIMED_COMMAND = 1 << 1,
				LOOP = LOOP_COMMAND | LOOP_TIMED_COMMAND
			} LoopMode;

			//! Looping mode
			int32 mLoop;

			//! Current time of the script
			float32 mTime;
			
			//! Does script run to the first time
			bool mFirstRunTimed;
			
			//! Timer used by the script (usefull for timed commands)
			SharedPtr<Timer> mTimer;
			
			//! Specified script starting time (ala absolute time start)
			float32 mScriptStartTime;
			
			//! Run only one step from the script
			Result step();

	};


};

#endif
