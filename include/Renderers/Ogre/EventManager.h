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
		  void subscribe(std::string inEvtName, EventListener* inListener);

		  void unsubscribe(std::string inEvtName, EventListener* inListener);

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

      template <class EventType>
      inline EventType* createEvt(std::string inName) {

        EventType* _evt = new EventType(inName);
        mEventPool.push_back(_evt);

        return _evt;
      }

      inline Event* createEvt(std::string inName) {
        return createEvt<Event>(inName);
      }

      /*! \brief Hooks an event onto the queue for processing.
       *
       *  Called by Event Listeners when an event is
       *  required to be fired.
       *
       *
       */
		  void hook(Event* inEvt);

      /*! \brief
      *	Processes Events in queue.
       *
       *  \note Should be updated in the game loop.
       */
      void update();

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
		  void _releaseEvt(Event* inEvt);

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

		  //! container for subscribers for Categories
		  name_subscription_t mSubscribers;

		  //! processing queue
      deque<Event*> mEvents;

		  //! container for all event instances requested
		  list<Event*> mEventPool;
	};

} // Pixy namespace
#endif // H_EventManager_H
