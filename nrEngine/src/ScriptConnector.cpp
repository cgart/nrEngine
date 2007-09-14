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

#include "ScriptConnector.h"
#include "events/EngineEvent.h"
#include "ScriptEngine.h"
#include "Engine.h"

namespace nrEngine{

	//----------------------------------------------------------------------
	ScriptConnector::ScriptConnector(const std::string& name) : EventActor(name + "_EventListener")
	{
		connect(NR_DEFAULT_EVENT_CHANNEL);
	}

	//----------------------------------------------------------------------
	void ScriptConnector::initialize()
	{
		uint32 count = Engine::sScriptEngine()->getFunctionCount();
		for (uint32 i = 0; i < count; i++)
		{
			ScriptFunctor functor;
			const std::string& name = Engine::sScriptEngine()->getFunction(i, functor);
			OnRegisterFunction(name, functor);
		}
	}

	//----------------------------------------------------------------------
	ScriptConnector::~ScriptConnector()
	{
		disconnect(NR_DEFAULT_EVENT_CHANNEL);
	}
	
	//----------------------------------------------------------------------
	void ScriptConnector::OnEvent(const EventChannel& channel, SharedPtr<Event> event)
	{
		// check if we got a new function event
		if (event->same_as<ScriptRegisterFunctionEvent>())
		{
			SharedPtr<ScriptRegisterFunctionEvent> ev = event_shared_cast<ScriptRegisterFunctionEvent>(event);
			OnRegisterFunction(ev->getName(), ev->getFunctor());
		}

		// we got a remove function event
		if (event->same_as<ScriptRemoveFunctionEvent>())
		{
			SharedPtr<ScriptRemoveFunctionEvent> ev = event_shared_cast<ScriptRemoveFunctionEvent>(event);
			OnRemoveFunction(ev->getName());
		}		
	}

};
