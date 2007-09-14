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
#include "Exception.h"
#include "Engine.h"

namespace nrEngine{

	//-------------------------------------------------------------------------
	Exception::Exception( Result number, const std::string& description, const std::string& source )
	{
		this->number = number;
		this->description = description;
		this->source = source;
	}

	//-------------------------------------------------------------------------
	Exception::Exception( Result number, const std::string& description, const std::string& source, char* file, long line )
	{
		this->number = number;
		this->description = description;
		this->source = source;
		this->lineNumber = line;
		this->file = file;

		// log the exception
		_log(description.c_str(), file, source.c_str(), line, "Exception thrown!");
	}

	//-------------------------------------------------------------------------
	Exception::Exception(const Exception& rhs)
	{
		this->number = rhs.number;
		this->description = rhs.description;
		this->source = rhs.source;
		this->lineNumber = rhs.lineNumber;
		this->file = rhs.file;
	}

	//-------------------------------------------------------------------------
	void Exception::_log(const char *szExp, const char *szFilename, const char* szFuncName, int iLineNum, const char* msg)
	{
		#if NR_PLATFORM != NR_PLATFORM_WIN32

			// Give message on the console
			fprintf(stderr,"================================================\n");
			fprintf(stderr,"%s!\n", msg);
			if (strlen(szFuncName)) fprintf(stderr,"Func: %s\n)", szFuncName);
			if (strlen(szExp)) 		fprintf(stderr,"Expr: %s\n", szExp);
			if (strlen(szFilename)) fprintf(stderr,"File: %s\n", szFilename);
			if (iLineNum) 			fprintf(stderr,"Line: %d\n", iLineNum);
			fprintf(stderr,"================================================\n");

		#endif
		
		// give message to log engine
		if (Engine::valid())
		{
			NR_Log(Log::LOG_ENGINE, Log::LL_ERROR, "================================================");
			NR_Log(Log::LOG_ENGINE, Log::LL_ERROR, "%s", msg);
			if (strlen(szFuncName)) NR_Log(Log::LOG_ENGINE, Log::LL_ERROR, "Func: %s", szFuncName);
			if (strlen(szExp)) 		NR_Log(Log::LOG_ENGINE, Log::LL_ERROR, "Expr: %s", szExp);
			if (strlen(szFilename)) NR_Log(Log::LOG_ENGINE, Log::LL_ERROR, "File: %s", szFilename);
			if (iLineNum) 			NR_Log(Log::LOG_ENGINE, Log::LL_ERROR, "Line: %d", iLineNum);
			NR_Log(Log::LOG_ENGINE, Log::LL_ERROR, "================================================");
		}
	

	}

	//-------------------------------------------------------------------------
	void Exception::Assert(bool exp, const char *szExp, const char *szFilename, const char* szFuncName, int iLineNum)
	{
		if (!exp) _assertMsg(szExp, szFilename, szFuncName, iLineNum);
	}

	//-------------------------------------------------------------------------
	void Exception::_assertMsg(const char *szExp, const char *szFilename, const char* szFuncName, int iLineNum){

		// give some log information
		_log(szExp, szFilename, szFuncName, iLineNum, "Assertion failed!");

		// Try now to delete the engine
		if (Engine::valid())
		{
			Engine::instance()->stopEngine();
			Engine::release();
		}

		// if wondows, so show a message box
		#if NR_PLATFORM == NR_PLATFORM_WIN32
			std::string msg;
			msg += std::string("Assertion Failed!\n");
			if (strlen(szFuncName)) msg += std::string("Func: ") +  std::string(szFuncName) + "\n";
			if (strlen(szExp)) 		msg += "Expr: " + std::string(szExp) + "\n";
			if (strlen(szFilename)) msg += "File: " + std::string(szFilename) + "\n";
			if (iLineNum) 			msg += "Line: " + boost::lexical_cast<std::string>(iLineNum)+ "\n";
			MessageBox(NULL, msg.c_str(), (std::string("nrEngine v")  + std::string(" ") + NR_VERSION_NAME).c_str(), MB_OK | MB_ICONERROR);
		#endif

		// Now exit the application
		exit(1);
	}

}; // end namespace

