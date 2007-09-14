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
#include <nrEngine/Timer.h>


namespace nrEngine{
	
	//------------------------------------------------------------------------
	Timer::Timer(Clock& clock): _clock(clock){
		
		// unpause the timer and set default scaling
		setPause(false);
		setScale (1.0f);
		setFixFrameRate(false);
		
		// set default values
		_currentTime = 0;
		_frameTime = 0;
		
	}
		
	//------------------------------------------------------------------------
	Timer::~Timer(){
	
	}
		
	//------------------------------------------------------------------------
	void Timer::notifyTimeObserver(){
		// if there is no pause, so update times
		if (!_bPaused){
			float32 frameInterval = _bFixFrameRate ? _fixFrameTime : _clock.getFrameInterval();
			_frameTime = frameInterval * _fScale;
		}else{
			_frameTime = 0;
		}

		// calculate current time
		_currentTime += _frameTime;
	}
	
	//------------------------------------------------------------------------
	void Timer::setFixFrameRate(bool setFixRate, float32 fixFrameRate){
		_bFixFrameRate = setFixRate;
		_fixFrameTime = 1.0f / fixFrameRate;
	}

	
	//------------------------------------------------------------------------
	float32 Timer::getRealFrameInterval() const
	{
		return _clock.getRealFrameInterval();
	}

}; // end namespace	

