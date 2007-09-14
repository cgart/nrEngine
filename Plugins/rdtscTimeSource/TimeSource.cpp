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
#include "TimeSource.h"
#include "Cpu.h"
#include <time.h>


#if NR_PLATFORM != NR_PLATFORM_WIN32
#    include <sys/time.h>
#endif

		
using namespace nrEngine;

//------------------------------------------------------------------------
rdtscTimeSource::rdtscTimeSource(boost::shared_ptr<Cpu> cpu) : TimeSource()
{
	mCpu = cpu;
	
	// check whenever we have got the high performance counter support
	if (mCpu->isRDTSC())
	{
		NR_Log(Log::LOG_PLUGIN, "rdtscTimeSource: It seems your CPU supports the \"rdtsc\" instruction");
		NR_Log(Log::LOG_PLUGIN, "rdtscTimeSource: High perfromance time counter will be used!");
	}
}

//------------------------------------------------------------------------
rdtscTimeSource::~rdtscTimeSource()
{

}
	

//------------------------------------------------------------------------
float64 rdtscTimeSource::getTime()
{
	// get current time
	if (mCpu->isRDTSC()){
		
		// retrive current ticks number
		uint64 now = _startTimeRT;
		mCpu->rdtsc(now);

		// some precalculation
		//float64 a =  1.0 / static_cast<float64>(mCpu->getSpeed());
		//float64 b = static_cast<float64>(now - _startTimeRT);
		
		uint64 b = now - _startTimeRT;
				
		// calculate current time in seconds
		_currentTime = (0.001 * float64(b)) / float64(mCpu->getSpeed()) + _syncTime;
	}
	
	// return it back
	return _currentTime;
}

//------------------------------------------------------------------------
void rdtscTimeSource::reset()
{
	// check whenever we have got the RDTSC support
	if (mCpu->isRDTSC()){
		mCpu->rdtsc(_startTimeRT);
	}
	
}

//------------------------------------------------------------------------
void rdtscTimeSource::sync()
{
	_syncTime = _currentTime;
	reset();
	mCpu->calculateSpeed(10);
}

