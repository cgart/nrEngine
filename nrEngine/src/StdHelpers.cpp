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
#include "StdHelpers.h"
#include "ITask.h"

#include <time.h>
#if NR_PLATFORM != NR_PLATFORM_WIN32
#    include <sys/time.h>
#endif

namespace nrEngine{

	//-------------------------------------------------------------------------
	void sleep( uint32 milliseconds )
	{
		#if NR_PLATFORM == NR_PLATFORM_WIN32
			Sleep( (DWORD)milliseconds );
		#else
			#if NR_PLATFORM == NR_PLATFORM_NETWARE
				delay(milliseconds/1000+1);
			#elif NR_PLATFORM == NR_PLATFORM_OS2
				DosSleep(milliseconds/1000+1);
			#else
				// retrive current time
				timeval start;
				int64 a,b;
				gettimeofday(&start, NULL);

				// calculate start and end time point
				a = start.tv_sec * 1000000L + start.tv_usec;
				b = a + milliseconds * 1000L;

				// do loop until we are over the end point
				while (a < b){
					gettimeofday(&start, NULL);
					a = start.tv_sec * 1000000L + start.tv_usec;
				}
			#endif
		#endif
	}


	//-------------------------------------------------------------------------
	std::string convertVersionToString(uint32 version)
	{
		// extract patch number
		uint32 patch = version % 256;

		// extract minor number
		uint32 minor = ((version) / 256) % 256;

		// extract major number
		uint32 major = ((version) / (256 * 256)) % 256;

		// create a string
		char str[32];
		sprintf(str, "%d.%d.%d", major, minor, patch);

		// return a string
		return std::string(str);
	}


	//-------------------------------------------------------------------------
	uint32 createVersionInteger(uint8 major, uint8 minor, uint8 patch)
	{
		return uint32(major) * 256 * 256 + uint32(minor) * 256 + uint32(patch);
	}


	//-------------------------------------------------------------------------
	std::string trim(const std::string& str)
	{
		std::string res;

		for (uint32 i=0; i < str.length();i++){
			if(str[i] != ' ' && str[i] != '\n'){
				res += str[i];
			}
		}

		return res;
	}

	//-------------------------------------------------------------------------
	std::string _NRExport orderToString(int32 order)
	{
#define CHECK(a) case a: result=#a; break;
		char ord[256];
		sprintf(ord, "%d", order);
		std::string result = ord;
		
		switch (order){
			CHECK(ORDER_SYS_ROOT)
			CHECK(ORDER_SYS_FIRST)
			CHECK(ORDER_SYS_SECOND)
			CHECK(ORDER_SYS_THIRD)
			CHECK(ORDER_SYS_FOURTH)
			CHECK(ORDER_SYS_FIVETH)
			CHECK(ORDER_SYS_LAST)
			CHECK(ORDER_FIRST)
			CHECK(ORDER_ULTRA_HIGH)
			CHECK(ORDER_VERY_HIGH)
			CHECK(ORDER_HIGH)
			CHECK(ORDER_NORMAL)
			CHECK(ORDER_LOW)
			CHECK(ORDER_VERY_LOW)
			CHECK(ORDER_ULTRA_LOW)
			CHECK(ORDER_LAST)
		};

#undef CHECK
		return result;
	}

}; // end namespace

