/*
 *  Copyright (c) 2011 Ahmad Amireh <ahmad@amireh.net>
 *
 *  Permission is hereby granted, free of charge, to any person obtaining a
 *  copy of this software and associated documentation files (the "Software"),
 *  to deal in the Software without restriction, including without limitation
 *  the rights to use, copy, modify, merge, publish, distribute, sublicense,
 *  and/or sell copies of the Software, and to permit persons to whom the
 *  Software is furnished to do so, subject to the following conditions:
 *
 *  The above copyright notice and this permission notice shall be included in
 *  all copies or substantial portions of the Software.
 *
 *  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 *  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 *  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
 *  THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 *  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 *  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 *  SOFTWARE.
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
#include "Event.h"
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
		template < class T, class EventType >
		inline void
		bind(std::string inName,
			 EventListener* inInstance,
			 bool (T::*inMethod)(EventType*),
			 bool fSubscribe = true)
		{
			name_bind_t::iterator _binder = mBindings.find(inName);

			// register Event if it isn't already
			if (_binder == mBindings.end())
			{
				std::vector<Handler*> _handlers;
				_binder = mBindings.insert(make_pair(inName, _handlers)).first;
			}

			// create handler
			std::vector<Handler*>* _handlers = &_binder->second;
			try {
				_handlers->push_back(new EventHandler<T, EventType>(static_cast<T*>(inInstance), inMethod));
			} catch (std::exception& e) {
				std::cerr << "** EvtListener: couldn't create handler! " << e.what() << "\n";
			}

			if (fSubscribe)
				EventManager::getSingletonPtr()->subscribe(inName, inInstance);
		}

		inline void
		unbind(std::string inName, EventListener* inInstance)
		{
		  name_bind_t::iterator _binder = mBindings.find(inName);

			// register Event if it isn't already
			if (_binder != mBindings.end())
			{
			  handler_list_t::iterator _handler = _binder->second.begin();
			  while (!_binder->second.empty()) {
			    delete _binder->second.back();
			    _binder->second.pop_back();
			  }
				_binder->second.clear();
				mBindings.erase(_binder);
			}

			EventManager::getSingletonPtr()->unsubscribe(inName, inInstance);
		};

  protected:
		void track(Handler*);
		void stopTracking(Handler*);
		bool beingTracked(Handler*);

		bool listeningTo(Event* inEvt);

		typedef std::vector<Handler*>				handler_list_t;
		typedef map< string, handler_list_t >	name_bind_t;

		//_bindMap mBindings;
		name_bind_t mBindings;

		list<Handler*> mTracker;
		queue<Event*> mEvents; // processing queue
	};
}
#endif // H_EventListener_H
