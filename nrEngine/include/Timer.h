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


#ifndef _NR_C_TIMER_H_
#define _NR_C_TIMER_H_


//----------------------------------------------------------------------------------
// Includes
//----------------------------------------------------------------------------------
#include "Prerequisities.h"
#include "Clock.h"
#include "ITimeObserver.h"
#include "Log.h"

namespace nrEngine{

	//! Timers are used to retrieve time. You can hav emore than one timer in your app
	/**
	* Timer is an standard timer shipped with the engine. You can
	* create your own timers because they are specialized for your application.
	* This timer can only ticks and provide user with information he/she need.
	* Also this timer supports scaling and pausing, so you are able to use
	* at lesser speed or hold on parts of your game using this timer.
	*
	* @note Because each timer is created from the clock, so the timer lifespan
	* is the same as from the clock. So if clock is destroyed, timer will also
	* be killed. You also can not destroy the timer object, only clock can do this.
	*
	* \ingroup time
	**/
	class _NRExport Timer: public ITimeObserver {
	public:
	
		/**
		* Define default values and set clock for using with this timer.
		* This timer will automaticly be added to the clock as observer.
		**/
		Timer(Clock& clock);	
		
		/**
		* Release used memory and remove this timer from observer list of
		* the clock.
		**/
		virtual ~Timer();
		
		/**
		* Returns current time since this timer is started
		**/
		float64 getTime() const { return _currentTime; }
		
		/**
		* Returns the time interval between two frames
		* Frame interval depends on the scale value you set for
		* this timer. So it can be different for different timers
		**/
		float32 getFrameInterval() const { return static_cast<float32>(_frameTime); }
	
		/**
		* Returns real frame interval based on Clock::getRealFrameInterval();
		**/
		float32 getRealFrameInterval() const;
		
		/**
		* Returns true if this timer is currently sleeping/paused
		**/
		bool isPaused () const{ return _bPaused; }
			
		/**
		* Returns the time scale value of this timer. Each timer can have each own
		* opinion how much time is passed. This is done by setting the scale value
		* of the timer. This scale shows how much faster/slower this timer runs
		**/
		float32 getScale () const{ return _fScale; }
	
		/**
		* This will hold on this timer so time stops
		* @param bOn if true timer stops, otherwise it runs
		**/
		void setPause (bool bOn){ _bPaused = bOn; }
		
		/**
		* Set time scale value for this timer. With the help of this value you have
		* the possibility to run your local time at different speed
		**/
		void	setScale (float32 fScale){ _fScale = fScale; }
		
		/**
		* This function will be always called by clock. Here the main update of the timer
		* is done. 
		**/
		virtual void notifyTimeObserver();

		/**
		* If you set this to true so fix frame rate will be used. This helps us
		* to run our application on console or do internal calculations based
		* on fix frame rate like Doom3 does.
		* @param setFixRate if true fix frame rate will be used.
		* @param fixFrameRate frame rate to be used
		**/
		void	setFixFrameRate(bool setFixRate, float32 fixFrameRate = 60.0f);

		/**
		* Reset the timer, so it start count the time from the value you define
		**/
		NR_FORCEINLINE void resetObserver(float64 resetToTime = 0.0f);
		
	private:
		
		// clock to be used with this timer	
		Clock& _clock;
		
		// current time and interval
		float64	_currentTime;
		float64	_frameTime;
		float64	_fixFrameTime;
		
		// pausing and scaling
		bool		_bPaused;
		float32		_fScale;
		bool		_bFixFrameRate;
		
	};

	

	//----------------------------------------------------------------------------------
	NR_FORCEINLINE void Timer::resetObserver(float64 resetToTime)
	{
		_currentTime = resetToTime;
		NR_Log(Log::LOG_ENGINE, "Timer: Reset timer %d to %f", resetToTime, getObserverID());
	}

	
}; // end namespace


#endif
