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
#include <nrEngine/TimeSource.h>
#include <nrEngine/Log.h>

#include <time.h>
#if NR_PLATFORM != NR_PLATFORM_WIN32
#    include <sys/time.h>
#endif

namespace nrEngine{
		
	//------------------------------------------------------------------------
	TimeSource::TimeSource() : _currentTime(0), _syncTime(0)
	{
		reset();
	}

	//------------------------------------------------------------------------
	TimeSource::~TimeSource()
	{
	}
		
	//------------------------------------------------------------------------
	float64 TimeSource::getSystemTime()
	{
		// do not use rdtsc for time calculation, just get time
		// by system calls
		timeval now;
		gettimeofday(&now, NULL);

		// some precalculation
		float64 a = static_cast<float64>(now.tv_sec) * static_cast<float64>(1000.0);
		float64 b = static_cast<float64>(now.tv_usec)/ static_cast<float64>(1000.0);

		// clculate the time in seconds
		return  static_cast<float64>(0.001) * (a+b);
		
	}

	//------------------------------------------------------------------------
	float64 TimeSource::getTime()
	{
		// retrive time information
		timeval now;
		gettimeofday(&now, NULL);

		// some precalculation
		float64 a =  static_cast<float64>(now.tv_sec - _startTime.tv_sec) *  static_cast<float64>(1000.0);
		float64 b =  static_cast<float64>(now.tv_usec- _startTime.tv_usec)/  static_cast<float64>(1000.0);
	
		// clculate the time in seconds
		_currentTime =  static_cast<float64>(0.001) * (a+b) + _resetTime + _syncTime;
		
		// return it back
		return _currentTime;
	}

	//------------------------------------------------------------------------
	void TimeSource::reset(float64 startValue)
	{
		gettimeofday(&_startTime, NULL);
		_resetTime = startValue;
	}

	//------------------------------------------------------------------------
	void TimeSource::sync()
	{
		_syncTime = _currentTime;
		reset();
	}

	//------------------------------------------------------------------------
	void TimeSource::notifyNextFrame()
	{
	}
		
}; // end namespace	

