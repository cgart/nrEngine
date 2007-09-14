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

#ifndef __NR_GET_TIME_OF_DAY_H_
#define __NR_GET_TIME_OF_DAY_H


//----------------------------------------------------------------------------------
// Includes
//----------------------------------------------------------------------------------
#include "Prerequisities.h"

//namespace nrEngine{

#if NR_PLATFORM == NR_PLATFORM_WIN32
   #include <time.h>
   #if !defined(_WINSOCK2API_) && !defined(_WINSOCKAPI_)
       #ifndef NR_WIN32_TIMEVAL
       #define NR_WIN32_TIMEVAL
             /*struct timeval
             {
                long tv_sec;
                long tv_usec;
             };*/
       #endif
   #endif 
   _NRExport int gettimeofday(struct timeval* tv, void* placeholder);
#else
   #include <sys/time.h>
#endif 

//}; // end namespace


#endif
