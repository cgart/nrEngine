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

#ifndef __NR_ENGINE_STD_HELPERS_H_
#define __NR_ENGINE_STD_HELPERS_H_

//----------------------------------------------------------------------------------
// Includes
//----------------------------------------------------------------------------------
#include "Prerequisities.h"

/*!
 * \defgroup helpers Global helper functions
 *
 * This group defines some usefull functions, macros and classes
 * which are helpfull by the work with the engine.
 *
 **/

namespace nrEngine{

	/**
	* NR_sleep do a sleep for a given count of milliseconds.
	* The sleeping function is defined to be platform independent, so
	* you are able to use it on every platform.
	* \param milliseconds Time in milliseconds, how long to sleep
	* \ingroup helpers
	**/
	void _NRExport sleep( uint32 milliseconds );


	/**
	* Convert a given version integer into a understandable string
	* which have a form like x.x.x
	* @param version Version integer build by NR_createVersionInteger
	* \ingroup helpers
	**/
	std::string _NRExport convertVersionToString(uint32 version);


	/**
	* Create an version integer from major, minor, patch numbers.
	* @param major,minor,patch 8 Bit long version number parts
	* \ingroup helpers
	**/
	uint32 _NRExport createVersionInteger(uint8 major, uint8 minor, uint8 patch);

	/**
	 * Delete spaces in the given string.
	 * @param str String to trim
	 * \ingroup helpers
	 **/
	std::string _NRExport trim(const std::string& str);

	/**
	 * Convert a given order name into a readable string
	 * \ingroup helpers
	 **/
	std::string _NRExport orderToString(int32 order);
	
}; // end namespace

#endif

