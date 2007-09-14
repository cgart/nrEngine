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


#ifndef _NR_I_TIMESOURCE_H_
#define _NR_I_TIMESOURCE_H_

/*!
* \defgroup time Clock and Timers
*
* This groups contains all classes and interfaces you will need by working with the time.
* Time is relative like Einstein said. So this part of the engine allows you to use
* this aspect in your applications.
* To use this you have to derive a time source class and bind it to any clock. Then
* the clock should ticks and will give you always right time. Now you create any timer
* from the clock. The timer has its own speed relative to the clock. This allows you
* to use one timer for GUI and one for your game. If you pause the game timer gui
* is still working. Also some nice effects are possible by using more than one
* timer in you rapplication.
**/

//----------------------------------------------------------------------------------
// Includes
//----------------------------------------------------------------------------------
#include "Prerequisities.h"

namespace nrEngine{
		
	
	//! Interface for time source give clock a current time
	/**
	* \par
	* TimeSource is an interface for each time emitter. You have to bind any time source to
	* the clock to have possibility to know the current time. You can think on time source
	* as on tick mechanism in every clock.<br>
	* In our default imlementation of the time source, we use a high performance tick counter
	* provided by the cpu if such one exists. So you will be able to get more precise time
	* results as usual.
	*
	* \par
	* Each time source should be able to provide real system time (real does not mean absolute).
	* The provided time has to be computed through system clock or similar techniques
	* without using of high performance tick counters (e.g. rdtsc). We need this to be able
	* to sync nrEngine's Clock-System for the current CPU-Speed. Because today's CPU does
	* support frequency scaling, so we need somewhere to sync the time.
	*
	* \ingroup time
	**/
	class _NRExport TimeSource {
	public:
		
		/**
		* Create an instance of the time source object.
		**/
		TimeSource();

		/**
		* Release used memory
		**/
		virtual ~TimeSource();
		
		/**
		* Get the current time in seconds. Returned time is not the same as system time.
		* @return current time in seconds
		**/
		virtual float64 getTime();

		/**
		* Get current system time as it is provided through system functions without
		* using of high performance tick counter (like rdtsc).
		**/
		virtual float64 getSystemTime();
		
		/**
		* Reset the time source, so it can start reading the time from the beginning.
		* You do not have to call this method, clock will do it automaticaly, if
		* it is needed.
		* @param startValue This value will be added to the default start value, which is normally
		*  0. So specifying the startValue parameter modifies the time to which the source should reset.
		**/
		virtual void reset(float64 startValue = 0.0);

		/**
		* Syncing the time source means we get the current time store it as sync time point
		* caluclate the new start point do all calculations again.
		* This syncing can help us to sync clock for example if the cpu speed changes.
		* If no new calculation about the cpu speed is done and we use rdtsc to compute
		* the current time, so we will get wrong time steps. Syncing should prevent this.<br>
		* Clock-Engine will decide by itself when to sync, so time source shouldn't do it.
		**/
		virtual void sync();

		/**
		* Call this method to notify the time source, that new frame beginns.
		* This method is usefull if we are using non-time based time sources.
		* So the time source could read time from some file, which is based
		* on frames.
		**/
		virtual void notifyNextFrame();
		
	protected:

		//! Store here time value given by gettimeofday - function
		struct timeval _startTime;

		//! Store the current time in seconds, since the source was resetted
		float64 _currentTime;

		//! Here we store the sncying time point. This will help us to sync the source
		float64 _syncTime;
		
		//! This is the reset value. The value is used to store time of reset point 
		float64 _resetTime;
	};
	
}; // end namespace

#endif
