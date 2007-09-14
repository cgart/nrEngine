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

#ifndef __NR_EXCEPTION_C_H_
#define __NR_EXCEPTION_C_H_


//----------------------------------------------------------------------------------
// Includes
//----------------------------------------------------------------------------------
#include "Prerequisities.h"


/**
 * This is a macro which throw an exception object
 * \ingroup error
 **/
#define NR_EXCEPT( num, desc, src ) throw( ::nrEngine::Exception( num, desc, src, __FILE__, __LINE__ ) )

/**
 * Assertion definition used in the engine. This assertion will produce more readable text.
 * \ingroup error
 **/ 
#define NR_ASSERT( exp )	::nrEngine::Exception::Assert(exp, #exp, __FILE__, "" , __LINE__ )


namespace nrEngine{

	//! Exception thrown by the engine if any non returnable error occurs
	/**
	* \par
	* 	nrEngine does use return codes to indicate that an error occurs. This is done because
	* 	of performance and code styling reasons. So the user does not have to encapsulate
	* 	each function call of the engine into try{}catch(){} block. Instead of this just
	* 	only check the return value of the function.<br>
	* 	But sometimtes we need to throw an error instead of return back an error code.
	* 	This is done for example in constructors and destructors, where you are not able
	* 	to return any value.<br>
	* 	For such cases you have to catch up the error thrown by the engine. Each exception
	* 	object is of this class.
	*
	* \par
	*	Probably in next releases of the engine we will go away from the return codes to
	*	the exception system.
	*
	* \ingroup error
	**/
	class _NRExport Exception
	{
		public:
			/**
			* Default constructor.
			* \param number Error code. Can be combined of more than one error code
			* \param description Description of the esception
			* \param source Where was exception thrown (function name)
			*/
			Exception( Result number, const std::string& description, const std::string& source );
	
			/**
			* Advanced constructor.
			* \param number Error code. Can be combined of more than one error code
			* \param description Description of the esception
			* \param source Where was exception thrown (function name)
			* \param file In which file was the exception
			* \param line Where was the exception thrown
			*/
			Exception( Result number, const std::string& description, const std::string& source, char* file, long line );
	
			/**
			* Copy constructor.
			*/
			Exception(const Exception& rhs);

		
			/**
			 * Assert function declared as static to allow using without initialisation
			 * of the object itself. This function will generate a formatted
			 * error message and will log it.
			 * @param exp Boolean expression to be checked (false = fail)
			 * @param szExp Error message
			 * @param szFilename Filename of the file where error happens
			 * @param szFuncName Name of the function where assertion is checked
			 * @param iLineNum Line number where assert fails
			 **/
			static void Assert(bool exp, const char *szExp, const char *szFilename, const char* szFuncName, int iLineNum);

		private:
			long 			lineNumber;
			std::string	description;
			std::string	source;
			std::string	file;
			Result			number;

			
			/**
			* Generate the formatted assert error message.
			* We use two functions to allow compiler to optimize the assertion
			* calls.
			**/
			static void _assertMsg(const char *szExp, const char *szFilename, const char* szFuncName, int iLineNum);

			/**
			 * Give some message to log files
			 **/
			static void _log(const char *szExp, const char *szFilename, const char* szFuncName, int iLineNum, const char* msg);
	};
	
}; // end namespace

#endif
