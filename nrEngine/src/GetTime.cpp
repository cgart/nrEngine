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
#include "GetTime.h"

//namespace nrEngine{

	//------------------------------------------------------------------------
#if NR_PLATFORM == NR_PLATFORM_WIN32
#if 0
   int gettimeofday(struct timeval* tv, void* placeholder)
   {
		union {
			long long ns100;
			FILETIME ft;
		} now;

		GetSystemTimeAsFileTime (&now.ft);
		tv->tv_usec = (long) ((now.ns100 / 10LL) % 1000000LL);
		tv->tv_sec = (long) ((now.ns100 - 116444736000000000LL) / 10000000LL);
printf("%d\n", tv->tv_sec);

		return 0;
   }
#endif
#if 1
/*
 * Number of micro-seconds between the beginning of the Windows epoch
 * (Jan. 1, 1601) and the Unix epoch (Jan. 1, 1970).
 *
 * This assumes all Win32 compilers have 64-bit support.
 */
#if defined(_MSC_VER) || defined(_MSC_EXTENSIONS) || defined(__WATCOMC__)
  #define DELTA_EPOCH_IN_USEC  11644473600000000Ui64
#else
  #define DELTA_EPOCH_IN_USEC  11644473600000000ULL
#endif

static nrEngine::uint64 filetime_to_unix_epoch (const FILETIME *ft)
{
    nrEngine::uint64 res = (nrEngine::uint64) ft->dwHighDateTime << 32;

    res |= ft->dwLowDateTime;
    res /= 10;                   /* from 100 nano-sec periods to usec */
    res -= DELTA_EPOCH_IN_USEC;  /* from Win epoch to Unix epoch */
    return (res);
};

int gettimeofday (struct timeval *tv, void *tz)
{
    FILETIME  ft;
    nrEngine::uint64 tim;

    if (!tv) {
        errno = EINVAL;
        return (-1);
    }
    GetSystemTimeAsFileTime (&ft);
    tim = filetime_to_unix_epoch (&ft);
    tv->tv_sec  = (long) (tim / 1000000L);
    tv->tv_usec = (long) (tim % 1000000L);

    return (0);
}

#endif

#endif
		
//}; // end namespace	

