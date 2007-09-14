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


#ifndef _NR_I_TIMEOBSERVER_H_
#define _NR_I_TIMEOBSERVER_H_


//----------------------------------------------------------------------------------
// Includes
//----------------------------------------------------------------------------------
#include "Prerequisities.h"

namespace nrEngine{

	//! Interface for every kind of objects used to observe the ticking of the clock	
	/**
	* ITimeObserver is an object which observers our global clock. Each time clock ticks
	* (this is not every second, but every frame) observer will be notified about
	* this and can do his work. Good example for such an observer is a timer, which
	* is updated every frame and provide time for parts of your application.
	* This also can be an alarm-clock which wait until certain time point and then fires.
	*
	* \ingroup time
	**/
	class _NRExport ITimeObserver {
	public:
		
		// Clock class is a friend, so he has full access
		friend class Clock;
		
		// constructor & destructor
		ITimeObserver() : _observerID(0) {}
		virtual ~ITimeObserver(){}
		
		/**
		* Clock should notify me every frame.
		**/
		virtual void notifyTimeObserver() = 0;
		
		/**
		* Returns the observer id given us by the clock.
		* You will need this id if you want to delete observer
		* from the clock's list of observers
		**/
		int32 getObserverID(){return _observerID; }
		
		/**
		 * Call this method to reset an observer.
		 * Each observer should know how to reset himself
		 * @param resetToTime reset the observer to this starting time
		 **/
		virtual void resetObserver(float64 resetToTime = 0.0f) = 0;
		
	private:
		int32 _observerID;
		
	};
	
}; // end namespace

#endif
