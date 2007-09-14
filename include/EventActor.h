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


#ifndef _NR_EVENT_ACTOR_H_
#define _NR_EVENT_ACTOR_H_

//----------------------------------------------------------------------------------
// Includes
//----------------------------------------------------------------------------------
#include "Prerequisities.h"
#include "Event.h"

namespace nrEngine{


	//! Event actors could acts as a server and client on event communication channels
	/**
	 * \par
	 * EventActor is a class representing an actor working on the event messaging
	 * communication channel. An actor could recieve and send new events. It will
	 * be informed by the channel if there is new events available, so it can react
	 * on them. EventActor could also send new events to a connected channel, so
	 * it acts as a server on the communication.
	 *
	 * \par
	 * We do not want to separate event servers and clients like it does in a lot
	 * of event based messaging systems.
	 *
	 * \ingroup event
	**/
	class _NRExport EventActor{
		public:

			//! Create new actor without any name (@see setName() ) 
			EventActor();
			
			//! Create a new actor within unique name
			EventActor(const std::string& name);

			//! Release used memory and disconnect the actor from the channel
			virtual ~EventActor();

			/**
			 * Get the name of an actor. The name should be unique
			 * to a certain channel. The communication channel will use the
			 * names to access its database for actors.
			 **/
			const std::string& getName() const;

			/**
			 * Set name for this actor. You have to setup a name before you can
			 * connect the actor to any communication channel.
			 **/
			void setName(const std::string& name);
			
			/**
			 * This is a function which will be called from the channel
			 * if any new event arise. You have to check for the event types
			 * derived by this function to do the job for the certain
			 * type of events.
			 *
			 * @param channel Channel from where does this event occurs
			 * @param event Smart pointer to an event object representing the message
			 *
			 **/
			virtual void OnEvent(const EventChannel& channel, SharedPtr<Event> event) = 0;

			/**
			 * Send a message through all connected channels.
			 * 
			 * @param event Message to be send
			 **/
			Result emit(SharedPtr<Event> event);
			
			/**
			 * Connect an actor to a certain channel.
			 *
			 * @param name Unique name of the channel to connect to.
			 * @return either OK or an error from EVENT_ERROR-group
			 **/
			Result connect (const std::string& name);

			/**
			 * Disconnect this actor from a certain channel
			 * @param name Unique name of the channel to disconnect
			 **/
			Result disconnect(const std::string& name);

		protected:

			//! Unique name of an actor in the channel
			std::string mName;

			//! The EventChannel is a friend so he is able to change default values
			friend class EventChannel;

			//! Here we store the all the channels we are connected to
			std::list<std::string> mChannel;

			//! EventManager which this actor belongs to
			//EventManager* mParentManager;

			//! Check whenever we are already connected to a channel
			bool isConnected (const std::string& name);

			/**
			 * This function will be called from the channel, to notice the actor,
			 * that he is got now a new connection.
			 **/
			void _noticeConnected(EventChannel* channel);

			/**
			 * Notice an actor that he is disconnected now.
			 **/
			void _noticeDisconnected(EventChannel* channel);

	};

}; // end namespace

#endif
