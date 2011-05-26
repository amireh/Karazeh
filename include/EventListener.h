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

#ifndef H_EventListener_H
#define H_EventListener_H

#include <iostream>
#include <utility>
#include <map>
#include <vector>
#include <list>
#include <queue>
#include "Utility.h"
#include "Event.h"
//#include "Message.h"
//#include "UIEvt.h"
#include "EventManager.h"
#include "EventHandler.h"


using std::map;
using std::vector;
using std::pair;
using std::list;
using std::queue;
namespace Pixy
{
    /*! \class EventListener "EventListener.h" "../include/EventListener.h"
     *
     *  \brief
     *  EventListener interfaces with EventManager for attaching events
	 *	to handlers
	 *
	 *	Components interested in Events need to derive from this class.
	 *	Using bind() children are able to register as many handlers
	 *	as needed to a certain event.
     *
	 *	\warning
	 *	Modules subclassing EventListener MUST implement a VIRTUAL destructor!!!
	 *
     *  \note
     *  Usage:
	 *	You should never need to create or call an EventListener
     *  directly. Instead, derive from it and use bind()
	 *
	 *	Example:
	 *
	 *	\code
	 *	class Foo : public EventListener {
	 *		Foo();
	 *		virtual ~Foo();
	 *		bool bar(Event* inEvt);
	 *	};
	 *
	 *	Foo::Foo() {
	 *		bind<Foo>(EVT_SOMETHING, this, &Foo::bar);
	 *	}
	 *	\endcode
	 *
     */
    class EventListener
		{
        public:
			
			EventListener();
            //! dtor
            virtual ~EventListener();
			
			/*!
			 *	\brief
			 *	Processes events in queue.
			 */
			void processEvents();
			
			/*!
             *  \brief
             *  Pushes inEvt into the processing queue, and calls every 
			 *	handler bound to it
             *
			 *	Events are removed from the queue as soon as ALL of their
			 *	handlers are done processing (return true). Should a handler
			 *	raise an exception, it will simply be discarded.
			 *
             *  \note
             *  This method is called internally by EventHandler
             *  upon dispatching and should not be called directly.
			 *
             */
			void enqueue(Event* inEvt);
			
			/* \brief
			 *	Binds an event handling method to an event
			 *
			 *	An event can have multiple handlers bound to it,
			 *	the order of calling will, naturally, be the order
			 *	of binding.
			 *
			 *	@param inUID
			 *		identifier for the event we want to register our handler to
			 *	@param inInstance
			 *		a child instance of EventListener (the caller)
			 *	@param inMethod
			 *		point to member method (the handler)
			 *
			 *	\note
			 *	Event handlers must match the following signature:
			 *	\code
			 *		bool method(Event* inEvt);
			 *	\endcode
			 *	Static member methods or simply static are not supported.
			 */
			/*
			template < class T, class EventType >
			inline void 
			bind(EVENT_UID inUID, 
				 EventListener* inInstance, 
				 bool (T::*inMethod)(EventType*), 
				 bool fSubscribe = true) 
			{	
				_bindMap::iterator _binder = mBindings.find(inUID);
				
				// register Event if it isn't already
				if (_binder == mBindings.end())
				{
					vector<Handler*> _handlers;
					_binder = mBindings.insert(make_pair(inUID, _handlers)).first;
				}
				
				// create handler
				vector<Handler*>* _handlers = &_binder->second;
				_handlers->push_back(new EventHandler<T, EventType>(static_cast<T*>(inInstance), inMethod));
				
				if (fSubscribe)
					EventManager::getSingletonPtr()->subscribe(inUID, inInstance);
			}
			*/
			
			
			template < class T, class EventType >
			inline void 
			bindToCat(std::string inCategory, 
				 EventListener* inInstance, 
				 bool (T::*inMethod)(EventType*), 
				 bool fSubscribe = true) 
			{	
				cat_bind_t::iterator _binder = mCatBindings.find(inCategory);
				
				// register Event if it isn't already
				if (_binder == mCatBindings.end())
				{
					vector<Handler*> _handlers;
					_binder = mCatBindings.insert(make_pair(inCategory, _handlers)).first;
				}
				
				// create handler
				vector<Handler*>* _handlers = &_binder->second;
				try {
					_handlers->push_back(new EventHandler<T, EventType>(static_cast<T*>(inInstance), inMethod));
				} catch (std::exception& e) {
					//Utility::getLogger().errorStream() << "** EvtListener: couldn't create handler! " << e.what() << "\n";
					std::cerr << "** EvtListener: couldn't create handler! " << e.what() << "\n";
				}
				
				if (fSubscribe)
					EventManager::getSingletonPtr()->subscribeToCat(inCategory, inInstance);
			}
			
			template < class T, class EventType >
			inline void 
			bindToName(std::string inName, 
				 EventListener* inInstance, 
				 bool (T::*inMethod)(EventType*), 
				 bool fSubscribe = true) 
			{	
				name_bind_t::iterator _binder = mNameBindings.find(inName);
				
				// register Event if it isn't already
				if (_binder == mNameBindings.end())
				{
					std::vector<Handler*> _handlers;
					_binder = mNameBindings.insert(make_pair(inName, _handlers)).first;
				}
				
				// create handler
				std::vector<Handler*>* _handlers = &_binder->second;
				try {
					_handlers->push_back(new EventHandler<T, EventType>(static_cast<T*>(inInstance), inMethod));
				} catch (std::exception& e) {
					//Utility::getLogger().errorStream() << "** EvtListener: couldn't create handler! " << e.what() << "\n";
					std::cerr << "** EvtListener: couldn't create handler! " << e.what() << "\n";
				}
				
				if (fSubscribe)
					EventManager::getSingletonPtr()->subscribeToName(inName, inInstance);
			}
			
			inline void 
			unbind(std::string inName, EventListener* inInstance) 
			{	
			  name_bind_t::iterator _binder = mNameBindings.find(inName);
				
				// register Event if it isn't already
				if (_binder != mNameBindings.end())
				{
					_binder->second.clear();
					mNameBindings.erase(_binder);
				}
				
				EventManager::getSingletonPtr()->unsubscribeFromName(inName, inInstance);
			};
			
			template < class T >
			inline void 
			bindToAll( EventListener* inInstance, 
					   bool (T::*inMethod)(Event*), 
					   bool fSubscribe = true) 
			{	
				// create handler
				try {
					mFullBindings.push_back(new EventHandler<T, Event>(static_cast<T*>(inInstance), inMethod));
				} catch (std::exception& e) {
					//Utility::getLogger().errorStream() << "** EvtListener: couldn't create handler! " << e.what() << "\n";
					std::cerr << "** EvtListener: couldn't create handler! " << e.what() << "\n";
				}					
				
				if (fSubscribe)
					EventManager::getSingletonPtr()->subscribeToAll(inInstance);
			}
			
			/* \brief
			 *	Binds a handler that is called only when there are no
			 *	specialized handlers are bound.
			 */
			template < class T, class EventType >
			inline void 
			bindFallback(EventListener* inInstance, 
						 bool (T::*inMethod)(EventType*)) 
			{
				if (mFBHandler)
					delete mFBHandler;
				
				mFBHandler = new EventHandler<T, EventType>(static_cast<T*>(inInstance), inMethod);
			}
			/*
			virtual void sendMessage(int inMsg) {	
				Message* _msg = EventManager::getSingletonPtr()->createEvt<Message>();
				_msg->setType(EVT_REQ);
				_msg->setBody(inMsg);
				EventManager::getSingletonPtr()->hook(_msg);
			}
			*/
			

    protected:
			void track(Handler*);
			void stopTracking(Handler*);
			bool beingTracked(Handler*);
			
			bool listeningTo(Event* inEvt);
			
			//typedef map< EVENT_UID, vector<Handler*> > _bindMap;
			typedef std::vector<Handler*>				handler_list_t;
			typedef handler_list_t					full_bind_t;
			typedef map< string, handler_list_t >	cat_bind_t;
			typedef map< string, handler_list_t >	name_bind_t;
			
			//_bindMap mBindings;
			full_bind_t mFullBindings;
			cat_bind_t	mCatBindings;
			name_bind_t mNameBindings;
			
			
			list<Handler*> mTracker;
			Handler* mFBHandler;
			queue<Event*> mEvents; // processing queue
		};
}
#endif // H_EventListener_H
