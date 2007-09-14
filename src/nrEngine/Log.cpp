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


#include <nrEngine/Log.h>
#include <iostream>
#include <time.h>

namespace nrEngine{

	//-------------------------------------------------------------------------
	Log::Log()
	{
		_logLevel = LL_NORMAL;
	}
	
	//-------------------------------------------------------------------------
	Log::~Log()
	{	
		log (LOG_ANYTHING, LL_DEBUG, "Logging stopped");
		
		// close streams
		_kernelLog.close();
		_engineLog.close();
		_serverLog.close();
		_clientLog.close();
		_pluginLog.close();
		_appLog.close();
	}
	
	//-------------------------------------------------------------------------
	Result Log::initialize(const std::string& logPath){
		
		this->logPath = logPath;
		
		// open log files
		_appLog.open((logPath + "/application.log").c_str(),std::ios::out);// | ios::app);
		_kernelLog.open((logPath + "/kernel.log").c_str(), std::ios::out);// | ios::app);
		//_clientLog.open((logPath + "/client.log").c_str(), std::ios::out);// | ios::app);
		//_serverLog.open((logPath + "/server.log").c_str(), std::ios::out);// | ios::app);
		_engineLog.open((logPath + "/engine.log").c_str(), std::ios::out);// | ios::app);
		_pluginLog.open((logPath + "/plugin.log").c_str(), std::ios::out);// | ios::app);
			
		log (LOG_ANYTHING, LL_DEBUG, "Logging activated");
				
		return OK;
	}
	
	//-------------------------------------------------------------------------
	void Log::log(LogTarget target, const char* msg, ...)
	{
		// check whenver we have already initialised the log engine
		/*if (logPath.length() == 0){
			static bool warned = false;
			if (warned == false){
				logIt(LOG_CONSOLE, LL_WARNING, "You can not log messages before you initialize the log engine");
				warned = true;
			}
			return;
		}*/
			
		// get messages 
		va_list args; 
		va_start(args,msg);
		char szBuf[2056];
		memset(szBuf, 0, sizeof(char)*2056);
		vsprintf(szBuf,msg,args);
		va_end(args);
		
		// log the message
		logIt(target, LL_NORMAL, szBuf);
		
		// echo logging
		if (_echoMap[target]){
			logIt(_echoMap[target], LL_NORMAL, szBuf);
		}

	}
	
	//-------------------------------------------------------------------------
	void Log::log(LogTarget target, LogLevel level, const char* msg, ...)
	{
	
		// check whenver we have already initialised the log engine
		/*if (logPath.length() == 0){
			static bool warned = false;
			if (warned == false){
				logIt(LOG_CONSOLE, LL_WARNING, "You can not log messages before you initialize the log engine");
				warned = true;
			}
			return;
		}*/
			
		// get messages 
		va_list args; 
		va_start(args,msg);
		char szBuf[2056];
		memset(szBuf, 0, sizeof(char)*2056);
		vsprintf(szBuf,msg,args);
		va_end(args);
		
		// log the message
		logIt(target, level, szBuf);
		
		// echo logging
		if (_echoMap[target]){
			logIt(_echoMap[target], level, szBuf);
		}
		
	}

	//-------------------------------------------------------------------------
	void Log::logIt(int32 target, LogLevel level, const char *msg){

		// check whenever the coming message is allowed to be logged
		if (level > _logLevel) return;
		
		// get messages 
		const char *szBuf = msg;
		
		// Get time & date string
		time_t _time = time(NULL);
		tm* time = localtime(&_time);
		
		// create the string before the message		
		char timeDate[255];
		std::string sLevel = getLevelStr(level);
		if (time != NULL)
			sprintf(timeDate, "%02d:%02d:%02d - %s", time->tm_hour, time->tm_min, time->tm_sec, sLevel.c_str());
		else
			sprintf(timeDate, "%s", sLevel.c_str());

						
		// Check whereever to log
		if((target & LOG_APP) && _appLog.is_open()){
			_appLog << timeDate << ": " << szBuf << "\n";
			_appLog.flush();
		}
		
		if((target & LOG_CLIENT) && _clientLog.is_open()){
			_clientLog << timeDate << ": " << szBuf << "\n";
			_clientLog.flush();
		}
		
		if((target & LOG_SERVER) && _serverLog.is_open()){
			_serverLog << timeDate << ": " << szBuf << "\n";
			_serverLog.flush();
		}
		
		if((target & LOG_KERNEL) && _kernelLog.is_open()){
			_kernelLog << timeDate << ": " << szBuf << "\n";
			_kernelLog.flush();
		}
		
		if((target & LOG_ENGINE) && _engineLog.is_open()){
			_engineLog << timeDate << ": " << szBuf << "\n";
			_engineLog.flush();
		}
		
		if((target & LOG_PLUGIN) && _pluginLog.is_open()){
			_pluginLog << timeDate << ": " << szBuf << "\n";
			_pluginLog.flush();
		}
	
		if (target & LOG_CONSOLE){
			std::cout << timeDate << ": " << szBuf << "\n";
			std::cout.flush();
		}
			
	}

	//-------------------------------------------------------------------------
	std::string Log::getLevelStr(LogLevel level)
	{
		switch(level){
			case LL_FATAL_ERROR: return "FatalError";
			case LL_ERROR: return "Error";
			case LL_WARNING: return "Warning";
			case LL_NORMAL: return "Log";
			case LL_DEBUG: return "Debug";
			case LL_CHATTY: return "Info";
			default: return "";
		}
		return "";
	}
	
};// end namespace

