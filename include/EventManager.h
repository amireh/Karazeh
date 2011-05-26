/*
 *  This file is part of Vertigo.
 *
 *  Vertigo - a cross-platform arcade game powered by Ogre3D.
 *  Copyright (C) 2011  Ahmad Amireh <ahmad@amireh.net>
 * 
 *  Vertigo is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  Vertigo is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with Vertigo.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#ifndef H_EventManager_H
#define H_EventManager_H

#include "Pixy.h"
#include "Event.h"

#include <string>
#include <map>
#include <deque>
#include <list>
#include <vector>
#include <utility>
#include <exception>

using std::make_pair;
using std::pair;
using std::deque;
using std::map;
using std::vector;
using std::string;
using std::list;
namespace Pixy
{
	class EventListener;
    /*! \class EventManager "EventManager.h" "../include/EventManager.h"
     *  \brief
     *  Creates, releases, registers, and fires Events, the EventManager makes
	 *	for the interface for subscribing EventListeners to be notified upon
	 *	dispatching of certain Events. Also acts as a Factory for the creation
	 *	of Events and releasing them.
     *
     *  \remarks
     *  All Event interaction should be done via this interface;
     *  when a component would like to trigger an event, it first has to
	 *	retrieve a newly created instance by calling createEvt(), then 
	 *	enqueue it by calling hook(). From there on, it's this module's job
	 *	to make sure all EventListeners interested in this Event are notified
	 *	when the Event's turn has come in the queue.
     */
    class EventManager
		{
			typedef vector<EventListener*> subscription_t;
			typedef subscription_t full_subscription_t; // alias
			typedef map< string, subscription_t > cat_subscription_t;
			typedef map< string, subscription_t > name_subscription_t;
			
        public:
            virtual ~EventManager();
			
            //! Retrieve the singleton instance of this class
            static EventManager* getSingletonPtr();
			static EventManager& getSingleton();
			static void shutdown();
			
			
            /*! \brief Subscribes an EventListener method to an Event.
             *
			 *	EventListeners subscribed to an Event evt will be notified
			 *	whenever evt is dispatched. From there on, the Listeners
			 *	will process the event by calling their registered handlers.
			 *
             *  @param inIdEvt 
			 *		Unique identifier for the Event
             *  @param inListener
			 *		Instance of the class that will handle this event
             *
             *  @return nothing
             *
             *  \note inListener MUST be a derivative of EventListener
             */			
			//void subscribe(EVENT_UID inIdEvt, EventListener* inListener);
			void subscribeToName(std::string inEvtName, EventListener* inListener);
			
            /*! \brief Subscribes an EventListener method to an Event's Category.
             *
			 *	EventListeners subscribed to a category will be notified
			 *	upon dispatching ANY of the Events belonging to that category.
			 *
             *  @param inCategory 
			 *		Category to which events belong
             *  @param inListener
			 *		Instance of the class that will handle this event
             *
             *  @return nothing
             *
             *  \note inListener MUST be a derivative of EventListener
             */
			void subscribeToCat(std::string inCategory, EventListener* inListener);
			
			void subscribeToAll(EventListener* inListener);
			
			void unsubscribe(EventListener* inListener);
			void unsubscribeFromName(std::string inName, EventListener* inListener);
			void unsubscribeFromCat(std::string inCategory, EventListener* inListener);
			
			/*! \brief
			 *	Informs the EventManager that this Listener is interested
			 *	in capturing all REQUEST events to send them to the game Server.
			 *	When registered, any Event flagged as REQ will bypass all
			 *	registered handlers and will be instead dispatched ONLY to this
			 *	Listener.
			 *
			 *	If the game instance is a Client, this must be called prior
			 *	to hooking any events, so that the NetworkManager(which is
			 *	the listener, naturally) passes on the request to the server.
			 *
			 *	Once a RESPONSE event is received from the server (again, 
			 *	passed by through NetMgr) it is then indeed dispatched to all
			 *	the subscribed Listeners.
			 */
			void subscribeNetworkDispatcher(EventListener* inInstance);
			
			
			/*! \brief
			 *	Factory-like method for retrieving a newly created Event.
			 *
			 *	This is the only way to create new Events; the creation
			 *	and destruction of *all* Events in game is centralized here
			 *	and done by the EventManager.
			 *
			 *	\note
			 *	Events will automatically be released, there is no need to worry
			 *	about destructing them. See release() below.
			 *
			 *	@return
			 *		EventType* \
			 *		Newly created Event of the parameterized type, with an assigned
			 *		global identifier.
			 */
			/*
			template <class EventType>
			inline EventType* createEvt(void) {
				
				EventType* _evt = new EventType(mIdCounter++);
				mEventPool.push_back(_evt);
				
				return _evt;
			}
			*/
			
			/*! \brief
			 *	Duplicated no-brainer method for creating an Event by the given UID.
			 *	This is done merely to get around the limitation of not being
			 *	able to expose templated member methods to Lua, which disables
			 *	us from using the main event factory method.
			 *
			 *	\remarks
			 *	This is a temporary solution until I get around to make it work
			 *	with Lua using the original method.
			 *
			 *	\warning
			 *	Do NOT use this, use the templated version instead!
			 *
			 *	@return 
			 *		Event* \
			 *		Newly created Event with the correct type identified
			 *		by its UID. This object has to be properly casted in Lua 
			 *		BEFORE USE.
			 */
			//Event* createEvt(EVENT_UID inUID);
			Event* createEvt(std::string inName);
			//EntityEvent* createEntityEvt(std::string inName);
			
            /*! \brief Hooks an event onto the queue for processing.
             *
             *  Called by Event Listeners when an event is
             *  required to be fired.
             *
             *  \remarks
             *  When an Event is requested to be fired, ie hooked,
             *  EventManager determines the position for this Event
             *  in the queue using the Rank of the event;
             *
             *  If the Event is ranked MINOR_EVT, it is queued right
             *  after the last minor event in the queue.
             *  If the Event is ranked MAJOR_EVT, it is queued in the
             *  end; will be processed once all preceding events
             *  are done.
             *
             *  \note
             *  For more info about the Ranks of events, see
             *  the class reference for Event.
             */	
			void hook(Event* inEvt);
			
            /*! \brief 
			 *	Processes Events in queue.
             *
             *  \note Should be updated in the game loop.
             */
            void update();
			
			/* called by the current GameState when resetting */
			void _clearQueue();
			
			/*! \brief
			 *	Removes inEvt from the master container and destructs it.
			 *
			 *	Events have an internal counter that tracks how many Listeners 
			 *  are processing them;
			 *	once a Listener is done, it flags the Event as so, which in turn
			 *	allows the Event to know when all Listeners are done processing it,
			 *	then it's time for destruction.
			 *
			 *	\note
			 *	This is managed internally by the Event, there is no need
			 *	to call this. Think of a shared/smart pointer.
			 */
			void releaseEvt(Event* inEvt);
					
      protected:
        EventManager();
        EventManager(const EventManager& src);
        EventManager& operator=(const EventManager& rhs);
        static EventManager* mInstance;

        /*! \brief 
        *	Calls all EventListeners subscribed to the Event being processed
        *
        *  Order of dispatching is controlled by the order of subscription.
        */
        void dispatch(Event* inEvt);

			/*! \brief
			 *	Removes the front Event from the processing queue after being
			 *	dispatched.
			 *
			 *	Note that the Event here is only dequeued, not destructed. See
			 *	release() for more info.
			 */
			void dequeue();
			
			bool alreadySubscribed(EventListener* inListener, vector<EventListener*>* inList);
			
			void detachListener(EventListener* inListener, subscription_t* inContainer);
    private:
			log4cpp::Category* mLog;
			
			//typedef map< EVENT_UID, vector<EventListener*> > uid_subscription_t;

			
			EventListener* mNetworkDispatcher;
			
			//! container for direct subscribers
			//uid_subscription_t mSubscribers;
			
			//! container for subscribers for Categories
			cat_subscription_t mCatSubscribers;
			full_subscription_t mFullSubscribers;
			name_subscription_t mNameSubscribers;
			
			//! processing queue
            deque<Event*> mEvents;
			
			//! container for all event instances requested
			list<Event*> mEventPool;
			
			//! global unique evt identifier generator
			//int mIdCounter;
		};
	
} // Pixy namespace
#endif // H_EventManager_H
