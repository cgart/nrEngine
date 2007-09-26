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

#ifndef __NR_LOG_H_
#define __NR_LOG_H_


//----------------------------------------------------------------------------------
// Includes
//----------------------------------------------------------------------------------
#include "Prerequisities.h"
#include "Engine.h"

//----------------------------------------------------------------------------------
// Some usefull macros which makes our life easear
//----------------------------------------------------------------------------------
#define NR_Log nrEngine::Engine::sLog()->log


namespace nrEngine{

	//! Simple class giving you logging functionality for your application.
	/**
	* Log Class is using to generate and print logging information on console,
	* MessageBox(WIN32) or in a log file on the disk. You can generate log messages
	* either by giving the message directly or by using message ID-Number of needed.
	* This guarantee to be log messages not hardcoded and they can also be localized (language). All used
	* log messages should be stored in an extra file to allow logging by Msg-Id-Number.
	*
	* \ingroup engine
	**/
	class _NRExport Log{
		public:
		
			//! Log target is an unsigned integer number
			typedef uint32 LogTarget;
			
			//! Enumeration containing all available log targets
			enum {
				
				//! Log the messages to the client log file. Here you should log all messages coming from client.
				LOG_CLIENT = 1 << 0,
				
				//! Log here messages coming from server if you using networking
				LOG_SERVER = 1 << 1,
			
				//! Use this target to log all application logs coming from your game
				LOG_APP = 1 << 2,
			
				//! This target will be used by kernel to log it's own messages
				LOG_KERNEL = 1 << 3,
			
				//! The engine will use this target to log it own messages. Note: Kernel has got it's own target
				LOG_ENGINE = 1 << 4,
			
				//! This will log to standard console output
				LOG_CONSOLE = 1 << 5,
			
				//! Messages coming from plugins should be logged here
				LOG_PLUGIN = 1 << 6,

				//! Log to all targets above
				LOG_ANYTHING = 0xFFFFFFFF
			};

			/**
			 * Each log message has a certain log level.
			 * Each log message that should be logged has such kind of information
			 * about the log level.<br>
			 * You can specify which kind of level information should be logged.
			 * All messsages with the type above the specified type will be logged.
			 * i.e. setting upo the log engine to log messages with LL_WARNING
			 * will log fatal errors, errors and warnings.
			 **/
			typedef enum _LogLevel{
			
				//! Messages of this level are produced by fatal errors
				LL_FATAL_ERROR = 0,

				//! This are normal error messages
				LL_ERROR,

				//! Only warning, without an error
				LL_WARNING,

				//! Normal logging, some useful information
				LL_NORMAL,

				//! Just a little bit more information than normal
				LL_DEBUG,

				//! Very verbose message types, say anything anywhere
				LL_CHATTY

			} LogLevel;

			
			/**
			* Initialize logging subsystem
			* @param logPath - Path to directory where log files will be created
			* @return either OK or:
			*			- LOG_ERROR if there was an error by creating the log files
			**/
			Result initialize(const std::string& logPath);
			
			/**
			* Log any message to the given log target.
			* You can bitwise combine log targets (i.e. LOG_CONSOLE | LOG_APP)
			* to log to one or more log targets. Each message logged
			* through this method will get a normal log level.
			* \param target Target where the log message should be written
			* \param msg Formatted message to log
			**/
			void log(LogTarget target, const char* msg, ...);

			/**
			* Same as the normal log function, but here you can specifiy also
			* the level of logged messages.
			**/
			void log(LogTarget target, LogLevel level, const char* msg, ...);
						
			/**
			* This function will setup logging echo. So you can for example specify
			* that all messages given to the engine can be echoing to the console.
			* @param from Echo messages from this target
			* @param to All messages will be echoed here
			**/
			void setEcho(LogTarget from, LogTarget to){
				_echoMap[(int32)from] = (int32)to;	
			}

			/**
			* Setup the default log level. All messages with the level lesser or
			* equal to the specified will be logged.
			* \param logLevel Level treshold of logged messages
			**/
			void setLevel(LogLevel logLevel){
				_logLevel = logLevel;
			}

						
		private:

			//! Only engine's core class is allowed to create the instance
			friend class Engine;

			/**
			* Create an instance of the log subsystem
			**/
			Log();

			/**
			* Remove the log subsystem from the memory
			**/
			~Log();

			// This are our logging targets
			std::ofstream _appLog;
			std::ofstream _engineLog;
			std::ofstream _clientLog;
			std::ofstream _serverLog;
			std::ofstream _kernelLog;
			std::ofstream _pluginLog;

			// Log path containing log files
			std::string logPath;
		
			// map which stores the logging echo targets
			std::map <int32, int32> _echoMap;

			// Log level information
			LogLevel	_logLevel;
			
			// logging function
			void logIt(int32 target, LogLevel level, const char *msg);

			// get log level string
			std::string getLevelStr(LogLevel level);

            // check whenever logging was initialized
            bool _bInitialized;
	};
	
}; // end namespace


#endif
