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


#ifndef _NR_EVENTMANAGER_H_
#define _NR_EVENTMANAGER_H_

/*!
 * \defgroup event Event-Management
 *
 * If you want to start to write a game you should be familar with event
 * systems and event based programming. Event system are used to pass events
 * between system modules without calling certain functions of that modules.
 * So using of events could reduce the dependability of the system components.
 *
 * In our engine we are trying to improve the normal behaviour of the event
 * management system. We do not only use simple events, instead of this we
 * mix several techniques like signals, triggers and state machines within the concept
 * of events to give the develeoper a possibility to increase the dependecy gap
 * between application modules.
 *
 * The engine does also use name driven event system instead of id driven design.
 * This is done for the reasons of readability and expandability. So if new
 * event should be added to the system, it can be added by using other name rather
 * then to recompile the application by adding new ids. Also using of string based
 * names for the vents allows better using of scripting.
 *
 * New type of events could be easely added through plugins, so the engine must not be
 * recompiled. For example: the engine could easely be extended to an input system.
 * The input system can fully be programmed as a plugin. To allow a communication
 * between the components of the engine and application, the new plugin could
 * generate a new type of events, namely "input". Each event listener which is
 * registered for hearing the communication channel for the input events will be
 * informed if there is a new event.
 *
 * Our events are priority based. This means that each event does get a priority number
 * defining how soon this event should be delivered. In each cycle the application
 * running all events/messages comming to a channel will be collected in a priority queue.
 * When the EventManager gets updated it tells the channels, that they now can deliver the
 * messages to all connected actors. So the events with the highest priority will be
 * delivered as first and the messages with the lowest priority will be delivered as last.
 * If there is two events of the same priority, so it is not defined which one will be
 * delivered as first.
 *
 * There is also special events having the priority number IMMEDIATE which will
 * cause the channel to deliver this message immediately without be stored in the queue.
 * This actors will be informed about this events as soon as the event is triggered.
 *
 * C++ does not provide any possibility to write the event management typesafe without
 * big circumastances. There are some techniques, but this will end up in a big
 * code plus a lot of function calls if to provide you the derived class instance instead
 * of the base Event class. However we will try to give you the possibility of using
 * your own event types derived from the base class in a controlled typesafe way.
 * This means that if you handle correctly, so you will never cast to any wrong
 * class by handling events.
 *
 * <hr>
 * Another problem which we have to solve is: How to share event messages between
 * plugins and application. Assume you compiled your application with the nrEngine
 * library linked in. Then if you start the app you use the same memory mapping like
 * the engine does. It means sending now events through the nrEngine's subsystem to
 * your application is not a big problem. However if you now use plugins which are
 * dynamic libraries, which are linked dynamicaly while your app is running, this
 * approach could not work so easy. it means you are still able to send events, but
 * you now can not convert an event to a type which is provided by the plugin.
 * (i.e. plugin defines new type of events (NewEvent). Plugin create an instance of
 * this class and send it to your application, you now not able to cast base class
 * Event to this NewEvent class). Problem of sharing type information between modules.
 *
 * One solution were to forbidd plugins to send any new type of events which was
 * not declared in the engine before. This solution might work, but then we loose
 * to generality of our engine, because then we have to implement all things which
 * are needed for properly use in the engine (graphics, inputs, ...)
 *
 * The other solution is to provide the engine TODO !!!!!!
 **/

//----------------------------------------------------------------------------------
// Includes
//----------------------------------------------------------------------------------
#include "Prerequisities.h"
#include "ITask.h"

namespace nrEngine{


	//! Main class providing the event messaging system
	/**
	 * EventManager is a class managing the whole event communication
	 * system in our engine. The system can be thought as a message bus
	 * where all senders and recievers are connected to. They send/recieve
	 * messages through this bus. In the message bus we define message channels
	 * which suppose to send/recieve only events/messages of a certain type.
	 *
	 * \ingroup event
	**/
	class _NRExport EventManager: public ITask {
		public:

			/**
			 * Create a new event messaging channel. Any listener
			 * connected to this channel will recieve only the messages
			 * coming from senders connected to this channel. Any state change
			 * of the channel will produce a new notice event to give the
			 * listeners a possibility to react to the new state.
			 *
			 * @param name Unique name for this channel
			 * @return either OK or an error code
			 **/
			Result createChannel(const std::string& name);

			/**
			 * Delete a certain communication channel.
			 * All actors connected to the channel are getting disconnected
			 * from the channel.
			 *
			 * @param name Unique name for the channel to remove
			 **/
			Result removeChannel(const std::string& name);

			/**
			 * Get a channel by its name
			 * @return smart pointer to the channel
			 **/
			SharedPtr<EventChannel> getChannel(const std::string& name);

			/**
			 * Send a new event message to the channel. The priority number
			 * of the event will be used to check if the message should be
			 * send immediately or if it should go to the channel queue as first.
			 *
			 * @param name Unique channel name, where to emit the event (empty for all channels)
			 * @param event SMart pointer on event to be emited
			 **/
			Result emit(const std::string& name, SharedPtr<Event> event);

			/**
			 * Same as emit() but this will emit messages to the system specific
			 * default channel. This channel is used by the engine to establish
			 * a communication between system components.
			 *
			 * NOTE: In next version this channel will be secured, so user are
			 * not allowed to send messages here anymore
			 **/
			Result emitSystem(SharedPtr<Event> event);

			/**
			 * Inherited method from the ITask interface. Our event manager
			 * is updated in each cycle to allow the channels to provide events
			 * to all connected parties.
			 **/
			Result updateTask();

			/**
			 * Call this function if you prefer to create a new event object
			 * from all registerd factories. The function will go through all
			 * registered factories and ask them which does support the given
			 * event type. If one could be found, so create it.
			 **/
			SharedPtr<Event> createEvent(const std::string& eventType);

			/**
			 * Register a new event factory. The given event factory will be
			 * stored in a list. The factory can later be used to create instancies
			 * of certain event types.
			 *
			 * @param name Unique name of a factory
			 * @param factory Smart pointer containing the factory object
			 **/
			Result registerFactory(const std::string& name, SharedPtr<EventFactory> factory);

			/**
			 * Delete a registered factory from the list.
			 **/
			Result removeFactory(const std::string& name);

			#if 0
			/**
			 * Create and emit a message through a channel.
			 * @param channel Channel name where to emit 
			 * @return Result of emit() method
			 **/
			template<class EventT>
			Result createEmit(const std::string& channel, void* param = NULL)
			{
				SharedPtr<Event> msg(new EventT);
				return emit(channel, msg);
			}
			#endif
			
		private:

			//! Only engine is allowed to create the instance
			friend class Engine;

			/**
			* Create default system specific communication channel.
			* This channel will be used to allow communication between
			* system components.
			*
			* In the next version of nrEngine we should secure this channel
			* by flags, so user applications are not allowed to hear or to emit
			* messages in this channel!!!
			**/
			EventManager();

			//! Close all event channels and release used memory
			~EventManager();

			//! Database representing the connection channels by their names
			typedef std::map<std::string, SharedPtr<EventChannel> > ChannelDatabase;

			//! Store the database in this variable
			ChannelDatabase mChannelDb;

			//! Here we do store event factories able to create new instancies
			typedef std::map<std::string, SharedPtr<EventFactory> > FactoryDatabase;

			//! Variable to hold the data
			FactoryDatabase mFactoryDb;

	};

}; // end namespace

#endif
