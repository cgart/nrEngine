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
#include "events/KernelEvent.h"

namespace nrEngine{

	//----------------------------------------------------------------------------------
	KernelEvent::KernelEvent(const std::string& taskName, TaskId id, Priority prior) : Event(prior)
	{
		mTaskId = id;
		mTaskName = taskName;
	}

}; // end namespace

