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


#ifndef _NR_I_TIMESOURCE_VIRTUAL_H_
#define _NR_I_TIMESOURCE_VIRTUAL_H_

//----------------------------------------------------------------------------------
// Includes
//----------------------------------------------------------------------------------
#include "Prerequisities.h"
#include "TimeSource.h"

namespace nrEngine{
		
	
	//! Timesource ticking certain time steps
	/**
	* Virtual time source does tick in fix steps.
	* You can use it to force clock ticking on fix steps
	* and not based on system time.
	*
	* \ingroup time
	**/
	class _NRExport TimeSourceVirtual : public TimeSource {
	public:
		
		/**
		**/
		TimeSourceVirtual(float64 timeStep);

		/**
		**/
		~TimeSourceVirtual();
		
		/**
		 * Return the time based on fixed ticks. Each calling of this function
		 * will increase the internal time counter. So the clock does call this method only 
		 * once per frame.
		**/
		float64 getTime();

		/**
		 * Reset the time source. This will force the time source to coutn from 0.
		 * Specify the startValue parameter to set the starting value of the time counter
		 **/
		void reset(float64 startValue);
		
		
		void notifyNextFrame();
		
	protected:

		//! Fix time step
		float64 _timeStep;
		
	};
	
}; // end namespace

#endif
