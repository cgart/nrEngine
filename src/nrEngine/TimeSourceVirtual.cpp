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
#include <nrEngine/TimeSourceVirtual.h>
#include <nrEngine/TimeSource.h>
#include <nrEngine/Log.h>


namespace nrEngine{
		
	//------------------------------------------------------------------------
	TimeSourceVirtual::TimeSourceVirtual(float64 step) : TimeSource(), _timeStep(step)
	{
		
	}

	//------------------------------------------------------------------------
	TimeSourceVirtual::~TimeSourceVirtual()
	{
	}
		
	//------------------------------------------------------------------------
	float64 TimeSourceVirtual::getTime()
	{
		// return it back
		return _currentTime;
	}

	//------------------------------------------------------------------------
	void TimeSourceVirtual::notifyNextFrame()
	{
		// clculate the time in seconds
		_currentTime += _timeStep; 
	}

	//------------------------------------------------------------------------
	void TimeSourceVirtual::reset(float64 startValue)
	{
		_currentTime = startValue;
	}
	
}; // end namespace	

