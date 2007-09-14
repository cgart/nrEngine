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
#include "EventActor.h"
#include "EventManager.h"
#include "EventChannel.h"
#include "Engine.h"

namespace nrEngine{

	//------------------------------------------------------------------------
	EventActor::EventActor()
	{
	
	}

	//------------------------------------------------------------------------
	EventActor::EventActor(const std::string& name) : mName(name){
	}
			
	//------------------------------------------------------------------------
	EventActor::~EventActor()
	{
		// first let each channel know, that we want to disconnect now
		std::list<std::string>::iterator it = mChannel.begin();
		for (; it != mChannel.end(); it++){
			Engine::sEventManager()->getChannel(*it)->del(this, false);
		}

	}

	//------------------------------------------------------------------------
	void EventActor::setName(const std::string& name)
	{
		mName = name;
	}
	
	
	//------------------------------------------------------------------------
	const std::string& EventActor::getName() const
	{
		return mName;
	}

	//------------------------------------------------------------------------
	/*void EventActor::OnEvent(const EventChannel& channel, SharedPtr<Event> event)
	{
		// we call the pure virtual derived function which has to be
		// overloaded for different event types. 
		_OnEvent(channel, event);
	}*/
	
	//------------------------------------------------------------------------
	Result EventActor::emit(SharedPtr<Event> event)
	{
		// iterate through channels and emit messages
		std::list<std::string>::iterator it = mChannel.begin();
		for (; it != mChannel.end(); it++)
		{
			Result res = Engine::sEventManager()->emit(*it, event);
			if (res != OK) 
			{
				NR_Log(Log::LOG_ENGINE, Log::LL_ERROR, "EventActor: Cannot emit message to '%s'", it->c_str());
				return res;
			}
		}
		return OK;
	}

	//------------------------------------------------------------------------
	Result EventActor::connect (const std::string& name)
	{
		// get a channel
		SharedPtr<EventChannel> channel = Engine::sEventManager()->getChannel(name);
		if (!channel) return EVENT_NO_CHANNEL_FOUND;

		// check if we are already connected
		if (isConnected(name)) return EVENT_ALREADY_CONNECTED;
		
		// ask the channel to connect me to it
		channel->add(this);
				
		// ok
		return OK;
	}
	
	//------------------------------------------------------------------------
	Result EventActor::disconnect (const std::string& name)
	{
		// get a channel
		SharedPtr<EventChannel> channel = Engine::sEventManager()->getChannel(name);
		if (!channel) return EVENT_NO_CHANNEL_FOUND;
		
		// check if we are already connected
		if (!isConnected(name)) return EVENT_NOT_CONNECTED;
		
		// ask the channel to connect me to it
		channel->del(this);

		// ok
		return OK;
	}

	//------------------------------------------------------------------------
	bool EventActor::isConnected(const std::string& name)
	{
		// search for the name in the list
		return std::find(mChannel.begin(), mChannel.end(), name) != mChannel.end();
	}

	//------------------------------------------------------------------------
	void EventActor::_noticeConnected(EventChannel* channel)
	{
		// add the name of this channel to the list
		if (!isConnected(channel->getName()))
			mChannel.push_back(channel->getName());
	}

	
	//------------------------------------------------------------------------
	void EventActor::_noticeDisconnected(EventChannel* channel)
	{
		// delete the name of this channel in the connection list
		if (isConnected(channel->getName()))
			mChannel.erase(std::find(mChannel.begin(), mChannel.end(), channel->getName()));
	}
	
}; // end namespace	

