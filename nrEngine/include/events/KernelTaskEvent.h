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


#ifndef _NR_ENGINE_KERNEL_TASK_EVENT__H_
#define _NR_ENGINE_KERNEL_TASK_EVENT__H_

//----------------------------------------------------------------------------------
// Includes
//----------------------------------------------------------------------------------
#include "KernelEvent.h"

namespace nrEngine{

	//! Send this event if kernel start execution of a task
	/**
	 * KernelStartTaskEvent will be send by the kernel if it start
	 * a task. The event will only be sent when the kernel start ticking
	 * so the tasks are executed first. SO before they executed this event
	 * will be sent.
	 * 
	 * \ingroup sysevent
	 **/
	class _NRExport KernelStartTaskEvent : public KernelEvent {
	
		META_Event(KernelStartTaskEvent)
		
		private:
			KernelStartTaskEvent(const std::string& taskName, TaskId id, Priority prior = Priority::IMMEDIATE)
			: KernelEvent(taskName, id, prior){}
					friend class Kernel;
	};

	//! This event is sent if a task stopped/removed from pipeline
	/**
	 * KernelStopTaskEvent will be send if a task is stops his execution.
	 * Stopped tasks are automaticaly removed from the kernel's execution tree.
	 * The event is sent after the task method taskStop() was called.
	 *
	 * \ingroup sysevent
	 **/
	class _NRExport KernelStopTaskEvent : public KernelEvent {
		
		META_Event(KernelStopTaskEvent)
		
		private:
			KernelStopTaskEvent(const std::string& taskName, TaskId id, Priority prior = Priority::IMMEDIATE)
			: KernelEvent(taskName, id, prior){}
			friend class Kernel;
	};

	//! Task is get into sleep state now
	/**
	 * KernelSuspendTaskEvent is sent if a task is get into sleep/suspend
	 * mode. Kernel calls taskOnSupsend() and if the function returns OK
	 * so this event will be triggered
	 *
	 * \ingroup sysevent
	 **/
	class _NRExport KernelSuspendTaskEvent : public KernelEvent {
		
		META_Event(KernelSuspendTaskEvent)
		
		private:
			KernelSuspendTaskEvent(const std::string& taskName, TaskId id, Priority prior = Priority::IMMEDIATE)
			: KernelEvent(taskName, id, prior){}
			friend class Kernel;
	};

	//! Event was waked up and is runnign now
	/**
	 * KernelResumeTaskEvent will be send if a task is waked up
	 * and will resume his execution. The event is sent after a successfull
	 * call to taskOnResume() method.
	 *
	 * \ingroup sysevent
	 **/
	class _NRExport KernelResumeTaskEvent : public KernelEvent {
	
		META_Event(KernelResumeTaskEvent)
		
		private:
			KernelResumeTaskEvent(const std::string& taskName, TaskId id, Priority prior = Priority::IMMEDIATE)
			: KernelEvent(taskName, id, prior){}
			friend class Kernel;
	};

}; // end namespace

#endif
