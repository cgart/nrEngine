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


#ifndef _NR_GLFW_EVENTFACTORY_H_
#define _NR_GLFW_EVENTFACTORY_H_

//----------------------------------------------------------------------------------
// Includes
//----------------------------------------------------------------------------------
#include <nrEngine.h>
#include "glfwBindings.h"

/**
 * Create events of supported types
 **/
class InputEventFactory : public nrEngine::EventFactory {
	public:

		//! default constructor
		InputEventFactory() : nrEngine::EventFactory("InputEventFactory")
		{
			fillSupported();
		}

		/**
		 * Create a instance of supported types
		 **/
		SharedPtr<Event> create(const std::string& eventType)
		{
			// only create if we realy support this
			if (!isSupported(eventType)) return SharedPtr<Event>();

			if (eventType == std::string("KeyboardPressEvent")){
				return SharedPtr<Event>(new KeyboardPressEvent());
			}else if (eventType == std::string("KeyboardReleaseEvent")){
				return SharedPtr<Event>(new KeyboardReleaseEvent());
			}
			return SharedPtr<Event>();
		}

		/**
		 * Fill the list of supported event types
		 **/
		void fillSupported()
		{
			mSupportedTypes.push_back("KeyboardPressEvent");
			mSupportedTypes.push_back("KeyboardReleaseEvent");
		}


};

#endif	//_NR...
