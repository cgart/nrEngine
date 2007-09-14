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
#include <nrEngine/EventChannel.h>
#include <nrEngine/Log.h>
#include <nrEngine/Profiler.h>

namespace nrEngine{

	//------------------------------------------------------------------------
	EventChannel::EventChannel(EventManager* manager, const std::string& name) : mName(name){
		mParentManager = manager;
	}

	//------------------------------------------------------------------------
	EventChannel::~EventChannel(){
		// disconnect all actors
		_disconnectAll();
	}

	//------------------------------------------------------------------------
	Result EventChannel::add(EventActor* actor, bool notice)
	{
		// check if actor has got a name 
		if (actor == NULL) return OK;
		if (actor->getName().length() == 0)
		{
			NR_Log(Log::LOG_ENGINE, Log::LL_ERROR, "EventChannel (%s): Want to connect nameless actor", getName().c_str());
			return EVENT_NO_VALID_ACTOR;
		}
		
		// we first check whenever the actor is already connected
		if (isConnected(actor->getName())) return EVENT_ALREADY_CONNECTED;

		// connect the actor to the OnEvent - Signal slot
		mActorDb[actor->getName()] = actor;

		// notice an actor that he is got a connection now
		if (notice) actor->_noticeConnected(this);

		// Log debug stuff
		NR_Log(Log::LOG_ENGINE, Log::LL_DEBUG, "EventChannel (%s): New actor connected \"%s\"", getName().c_str(), actor->getName().c_str());

		return OK;
	}

	//------------------------------------------------------------------------
	Result EventChannel::del(EventActor* actor, bool notice)
	{
		if (actor == NULL) return OK;
		
		// we first check whenever the actor is already connected
		if (!isConnected(actor->getName())) return EVENT_NOT_CONNECTED;

		// disconnect the actor to the OnEvent - Signal slot
		mActorDb.erase(mActorDb.find(actor->getName()));

		// notice an actor that it is disconnected now
		if (notice) actor->_noticeDisconnected(this);

		// Log debug stuff
		NR_Log(Log::LOG_ENGINE, Log::LL_DEBUG, "EventChannel (%s): Actor \"%s\" disconnected ", getName().c_str(), actor->getName().c_str());

		return OK;
	}

	//------------------------------------------------------------------------
	void EventChannel::_disconnectAll()
	{
		// Profiling of the engine
		_nrEngineProfile("EventChannel._disconnectAll");

		// some logging
		NR_Log(Log::LOG_ENGINE, Log::LL_DEBUG, "EventChannel (%s): Disconnect all actors", getName().c_str());

		// iterate through all connections and close them
		ActorDatabase::iterator it = mActorDb.begin();
		for (; it != mActorDb.end(); it++){
			mActorDb.erase(it);
			it->second->_noticeDisconnected(this);
		}

	}

	//------------------------------------------------------------------------
	bool EventChannel::isConnected(const std::string& name)
	{
		ActorDatabase::iterator it = mActorDb.find(name);
		return it != mActorDb.end();
	}

	//------------------------------------------------------------------------
	void EventChannel::emit (SharedPtr<Event> event)
	{
		// iterate through all connected actors and emit the signal
		ActorDatabase::iterator it = mActorDb.begin();
		for (; it != mActorDb.end(); it++){
			it->second->OnEvent(*this, event);
		}
	}

	//------------------------------------------------------------------------
	void EventChannel::push (SharedPtr<Event> event)
	{
		// check if the event priority is immediat
		if (event->getPriority() == Priority::IMMEDIATE){
			emit(event);
		}else{
			mEventQueue.push(event);
		}
	}


	//------------------------------------------------------------------------
	void EventChannel::deliver()
	{
		// Profiling of the engine
		_nrEngineProfile("EventChannel.deliver");

		// we go through all elements in our event queue and deliver
		// the messages from the queue to connected actors
		while (!mEventQueue.empty()){
			emit(mEventQueue.top());
			mEventQueue.pop();
		}

	}

}; // end namespace

