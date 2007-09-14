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


#ifndef _PLUGIN_RDTSC_TIMESOURCE_H_
#define _PLUGIN_RDTSC_TIMESOURCE_H_

//----------------------------------------------------------------------------------
// Includes
//----------------------------------------------------------------------------------
#include <nrEngine.h>
#include "Cpu.h"
#include <boost/shared_ptr.hpp>


using namespace nrEngine;


/**
* This is a derived class of time source which provide the functionality
* of rdtsc and cpuid instructions to get the time.
* Use this as Plugin for the engine.
* \par
* Because we are using high definition timers in our engine, so each time
* step depends on the current cpu frequency. If you retrive a wrong speed
* by initialization of the clock, you will get wrong time calculations
* along whole time calculation process. So sometimes we need to recalculate
* the speed of used cpu (i.e. CPU/FSB speed stepping)<br>
* To do this our clock has a function named sync(). This will snyc the current
* clock with a system time and will force the CPU class to recalculate it's speed.
* Syncing is done automatically every user defined time step. Sou you can decide
* how important the high performance time for your application is.
**/
class rdtscTimeSource : public TimeSource {
public:
	
	/**
	* Create an instance of the time source object.
	**/
	rdtscTimeSource(boost::shared_ptr<Cpu> cpu);

	/**
	* Release used memory
	**/
	~rdtscTimeSource();
	
	/**
	* Get the current time in seconds. Returned time is not the same as system time.
	* @return current time in seconds
	**/
	float64 getTime();

	
	/**
	* Reset the time source, so it can start reading the time from the beginning.
	* You do not have to call this method, clock will do it automaticaly, if
	* it is needed.
	**/
	void reset();

	/**
	* Syncing the time source means we get the current time store it as sync time point
	* caluclate the new start point do all calculations again.
	* This syncing can help us to sync clock for example if the cpu speed changes.
	* If no new calculation about the cpu speed is done and we use rdtsc to compute
	* the current time, so we will get wrong time steps. Syncing should prevent this.<br>
	* Clock-Engine will decide by itself when to sync, so time source shouldn't do it.
	**/
	void sync();
	
private:

	//! Store the start time as 64Bit integer, for rtdsc counter
	uint64	_startTimeRT;

	//! Cpu to get info
	boost::shared_ptr<Cpu>	mCpu;
		
};

#endif
