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


#ifndef _NR_EVENT_CHANNEL_H_
#define _NR_EVENT_CHANNEL_H_

//----------------------------------------------------------------------------------
// Includes
//----------------------------------------------------------------------------------
#include "Prerequisities.h"
#include "EventActor.h"
#include <queue>

namespace nrEngine{

	//! Event channel used for communication between application/engine's components
	/**
	 * \par
	 * EventChannel represents a message bus for the communication between
	 * system components. Events/messages could be send through this channel
	 * to all listeners connected to this channel.
	 *
	 * \par
	 * In our engine we mix the concept of event messaging and state machines.
	 * So we do not have only events but also states. The states are stored in
	 * communication channels, so each listener of a channel could also be noticed
	 * if any state changes. This is also how we handle it in the nrEngine.
	 *
	 * \par
	 * The communication actors are connected to the channel by its name. Also
	 * the lifetime of each actor is tracked. So if an actor is removed from the
	 * memory, so it will be automaticaly disconnected from the database.
	 * We do not use smart pointers for this purpose, because it will cause in
	 * undefined working flow. i.e. if you think the actor is removed from the memory
	 * because you have deleted it, it still get noticed about the things going on
	 * the channel. So this will ends up in undefined state.In our implementation
	 * the destructor of EventActor does simply say all channels it connected to,
	 * that the object does not exists anymore, so it can be disconnected.
	 * 
	 * \ingroup event
	**/
	class _NRExport EventChannel {
		public:

			//! Create a new instance of the communication channel 
			EventChannel(EventManager* manager, const std::string& name);

			//! Releas eused memory and destroy hte instance.
			virtual ~EventChannel();

			/**
			 * Connect a new actor to the channel. The actor will be connected
			 * so it get noticed about any communication on the channel.
			 *
			 * The actor has to implement default event reactions needed for
			 * our channel communication system. Compilation error will occur
			 * if the reactions to the default events are not implemented.
			 * 
			 * @param actor An actor to connect to the channel
			 * @param notice Should channel let notice the actor that he is connected now (default YES)
			 **/
			Result add(EventActor* actor, bool notice = true);

			/**
			 * Disconnect an actor from the channel. It is a good coding style
			 * if you call this function, when you do not need the connection anymore.
			 * However our engine does provide you the possibility to
			 * forget about disconnecting the actors from the channel, because the lifetime
			 * of an actor will be tracked. If the object does not exists anymore, so
			 * it will be automaticaly disconnected.
			 *
			 * If you want to be performant, so remove actors from the channels, if they do not
			 * need the connection anymore.
			 *
			 * @param actor An actor already connected to the channel
			 * @param notice Should channel notice the actor, about disconnection (default YES)
			 **/
			Result del(EventActor* actor, bool notice = true);

			/**
			 * Get the name of the channel
			 **/
			NR_FORCEINLINE const std::string& getName () const { return mName; }

			/**
			 * Emit a certain event to a channel. This will send this event
			 * to all connected actors, so they get noticed about new event.
			 *
			 * @param event Smart pointer to an event object
			 **/
			void emit (SharedPtr<Event> event);

			/**
			 * Instead of emit() this will not send the message directly to
			 * the channel listeners, but it store the message first in
			 * a queue based on priority numbers of the events. When
			 * you call deliver(), then all messages are getting delivered
			 * to the channel listeners
			 *
			 * @param event Smart pointer to the event message
			 *
			 * NOTE: If event priority is immediately so the message will
			 * 		be emitted immediately without be stored in the queue
			 **/
			void push(SharedPtr<Event> event);

			/**
			 * Deliver all stored event messages from the queue
			 * to the actors connected to the channel.
			 **/
			void deliver();
			
		protected:
			//! The event manager system is a friend to this class
			friend class EventManager;

			//! Store here the mapping between actor names and their connections
			typedef std::map<std::string, EventActor*> ActorDatabase;

			//! Unique name of the communication channel
			std::string mName;
			
			//! We always does store a pointer to the event manager where this channel belongs to
			EventManager* mParentManager;

			//! Connected actor database
			ActorDatabase mActorDb;
			 
			/**
			 * This structure is used as a wrapper to define a way
			 * how to distinguish between priority numbers of events.
			 **/
			template <class ClassT>
			struct GreatEvent : public std::binary_function<ClassT, ClassT, bool>
			{
				bool operator()(ClassT x, ClassT y) const
				{
					return x->getPriority() > y->getPriority();
				}
			};
			
			//! We store our event messages in a priority queue
			typedef std::priority_queue<SharedPtr<Event>, std::vector<SharedPtr<Event> >, GreatEvent<SharedPtr<Event> > > EventQueue;

			//! Store the event messages in this variable
			EventQueue mEventQueue;
			
			//! Check whenever a given actor is already connected
			bool isConnected(const std::string& name);
			
			//! Disconnect all actors from the channel
			void _disconnectAll();

	};
	
}; // end namespace

#endif
