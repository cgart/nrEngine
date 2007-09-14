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


#ifndef _NR_C_CLOCK_H_
#define _NR_C_CLOCK_H_


//----------------------------------------------------------------------------------
// Includes
//----------------------------------------------------------------------------------
#include "Prerequisities.h"
#include "TimeSource.h"
#include "ITimeObserver.h"
#include "Timer.h"
#include "ITask.h"
#include "TimeSourceVirtual.h"
#include "ScriptEngine.h"

namespace nrEngine{

	//! Global clock of the engine used to retrieve time or create timers
	/**
	* \par
	* Clock allows you to create timers from it. Clock is always ticks if any
	* time source is bounded. In contrast to Einstein's relativity theory we have
	* only one global clock. So your complete application has one time which runs
	* with the same speed. But to be able to use Einstein's theory you can create
	* timers from each clock, which allows you to use different time speeds.
	* Clock can also derive/create timers. But if you want to create the timer by
	* yourself you can do this.
	*
	* @note
	*		- Our clock works internal with 64Bit float64 values to stay precise.
	* 			Because using of float32 values is enough for every simulation you will get
	* 			32Bit float32 values as frame interval time.
	* 		- If you compile the engine on 64Bit systems, so you can change this and use
	*			64Bit doubles instead.
	*		- Each created timer dies when clock dies.
	*		- You can only destroy timers through the clock interface or it will be
	*			deleted automaticly from the memory when clock is deleted
	*		- Deleting the timer outside from the clock has no effect
	*
	* \ingroup time
	**/
	class _NRExport Clock : public ITask {
	public:
	
		/**
		* Set the time source to be used to get current time. Our clock class
		* is platform independent but the time get code depends mostly on the
		* platform you using. So just derive your time source from ITimeSource
		* interface and add this source into our clock.
		* @param timeSource smart pointer containing to the time source
		**/
		void setTimeSource(SharedPtr<TimeSource> timeSource);
		
		/**
		* Returns true if any time source is bounded to our clock.
		**/
		bool isAnySourceBounded();
		
		/**
		* Return a smart pointer to the current time source
		* @return a smart pointer pointing to the current time source or NULL if
		* no time source is bound at now.
		**/
		SharedPtr<TimeSource> getTimeSource() { return timeSource; }
		
		/**
		 * This method will be called by the kernel to start the clock. It is called
		 * when clock is added to the kernel pipeline.
		 **/
		Result onStartTask();

		/**
		* Updates the clock task by getting time values and calculating of
		* frame rate and frame interval.
		* Also each connected time observer will be notified so it can do
		* own updates. This allows you just to create any observer (like timer)
		* bind them to the clock and forgett it.
		**/
		Result updateTask();

		/**
		* Stop the clock task and release all used objects.
		* So all observers will be deleted from the observer list and also
		* the time source is removed. After this function you still able to access
		* to the clock and timers (because of smart pointers), but the time will
		* hold on and no updates of the time will be made.
		**/
		Result stopTask();

		/**
		* Same as taskUpdate(). This name is choosen to separate calls
		* to the clock by not knowing, that it is a task.
		* Use this function from somewhere else than a kernel.
		**/
		void		Ticks(){ updateTask();}
		
		/**
		* Returns current time since clock start ticking.
		**/
		float64 	getTime() const;
		
	
		/**
		* Returns filtered interval time between two or more frames. nrEngine's clock
		* does not only calculate the interval between two following frames, but it uses
		* a arithmetic average to compute the frame interval between a lot of frames.
		* This helps you to ignore some frames if their update time was to big or to less.
		*/
		float32 	getFrameInterval() const;
	
		/**
		* Returns number of frames drawn since clock start ticking
		**/		
		int32 		getFrameNumber() const;
			
		/**
		* Returns FPS. Like the frameinterval this value is also computed as average
		* between a lot of frames. So this value is more smooth.
		**/
		float32		getFrameRate() const;
	
		/**
		 * Get the frame interval time of a real time. If you use fix frame rate, so you
		 * get as an interval a 1.0 / fix frame rate. This method allows you to get
		 * the time which your frame really need to render.
		 **/
		float32 getRealFrameInterval() const;

		/**
		 * Get real frame rate based on getRealFrameInterval() method
		 **/
		float32 getRealFrameRate() const;

		/**
		 * This method allows you to set the maximal frame rate at which the clock
		 * shoudl run. It will use this frame rate as an upper bound. If applications
		 * runs faster than this time, so it will retard to this speed. If app runs
		 * much slower so nothing happens. At speed above the given boundaries
		 * the clock runs as for fixed frame rate. So you would retrieve fixed
		 * frame interval values. Function which retrieves real values are not
		 * affected by them, so you are still able to get information about the
		 * real time, which you frame need
		 **/
		void setMaxFrameRate(bool set, float32 maxFPS = 60.0);

		/**
		* Add new time observer to our list of observers. Each observer will
		* be notified on each frame after clock calculations are done. With this
		* system we can separate our timers from game loop, but they still be
		* updated on each frame.
		* @param timeObserver smart pointers to the observer
		* @return internal ID of observer to allow of removing them from the list
		* @note Each observer has to implement the ITimeObserver interface
		**/
		int32 		addObserver(SharedPtr<ITimeObserver> timeObserver);
		
		/**
		* Removes an observer from the list by given ID
		* @param observerID id of the observer returned by addObserver()
		* @return either OK or:
		*		- CLOCK_OBSERVER_NOT_FOUND if such id is not valid
		**/
		Result	removeObserver(int32 observerID);
		
		/**
		* Add an time observer by a name. So you can delete them also by using
		* this name.
		* @param obsName name of the observer
		* @param timeObserver smart pointer pointing to the observer
		* @return either OK or:
		*			- CLOCK_OBSERVER_ALREADY_ADDED if such is already added
		**/
		Result	addObserver(const std::string& obsName, SharedPtr<ITimeObserver> timeObserver);
	
		/**
		* Removes an observer by using it's name.
		* @param obsName name of the observer
		* @return either OK or:
		*			- CLOCK_OBSERVER_NOT_FOUND if such an observer not exists
		**/
		Result	removeObserver(const std::string& obsName);
		
		/**
		* Set the count of frames to be computed to calculate frame rate/interval.
		* We need this to smooth big peaks on frame rates to allow application
		* run smoother than without this window. Here we just using averaging
		* of the frame intervals during certain number of frames.
		* @param frameCount size of such frame calculation window
		* @param defaultFrameTime is a frame interval added by default as if there
		*		was a frame rendered with this speed. This allows us to start smoothly
		**/
		void	setFrameWindow(int32 frameCount = 10, float32 defaultFrameTime = 0.03333);
	
		/**
		* If you set this to true so fix frame rate will be used. This helps us
		* to run our application on console or do internal calculations based
		* on fix frame rate like Doom3 does.
		* @param setFixRate if true fix frame rate will be used.
		* @param fixFrameRate frame rate to be used
		**/
		void	setFixFrameRate(bool setFixRate, float32 fixFrameRate = 60.0f);

		
		/**
		* Create a new timer which does use this clock as reference.
		**/
		SharedPtr<Timer>	createTimer();

		/**
		* Setup the syncing interval. The clock will be synced with system time
		* so it helps to prevent timing errors produced by high performance time
		* counters in addidition to CPU speed stepping functionality of the processor unit.
		**/
		void setSyncInterval(uint32 milliseconds);
		
		/**
		 * Reset the clock. The clock time will be resetted to the given time.
		 * @param resetToTime Reset the clock to the given time value
		 * @param resetAllObservers if true all registered obseervers would be also resetted
		 **/
		void reset(float64 resetToTime = 0.0f, bool resetAllObservers = false);
		
		/**
		 * Script-Function to reset the clock. You can specify in parameters if all timers, which were
		 * created before should be also reseted.
		 **/
		ScriptFunctionDef(scriptResetClock);


	private:

		//! Only engine's core object can create instance
		friend class Engine;

		/**
		* After the object is created and before it runs the returned value
		* for the frame rate will be 0.
		* The value for frame window is set to 7 per default.
		**/
		Clock();

		/**
		* Release all used memory.
		**/
		~Clock();

		// observers
		typedef std::vector< SharedPtr<ITimeObserver> > ObserverList;
		std::map < std::string, int32>	observerIDList;
		ObserverList				observers;
		
		// frame filtering
		std::deque<float64>	frameDurationHistory;
		float64			frameDefaultTime;
		int32			frameFilteringWindow;
		
		// time source
		SharedPtr<TimeSource>	timeSource;
		SharedPtr<TimeSource>	realTimeSource;
		float64		currentTime;
		float64		frameTime;
		int32		frameNumber;
		bool		useFixRate;
		float32		fixFrameRate;

		float32     maxFrameRate;
		float64     lastCurrentTime;
		float64     invMaxFrameRate;
		bool        useMaxRate;

		// values to hold real frame information if we use fix frame rate
		float64     realFrameTime;

		// methods
		void	_addToFrameHistory(float64 duration);
		float64	_getFilteredFrameDuration() const;
		float64	_getExactFrameDuration();
		
		// helping variables to find out the frame rate
		float64 sourceStartValue;
		float64 sourceLastValue;
		float64 sourceLastRealValue;
		
		// Next time when the clock has to be synced with system time
		float64 nextSyncTime;
		uint32  syncInterval;
		
	};
	
}; // end namespace
#endif
