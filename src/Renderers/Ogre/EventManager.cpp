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

#include "Renderers/Ogre/EventManager.h"
#include "Renderers/Ogre/EventListener.h"

using std::cout;
namespace Pixy {

	EventManager* EventManager::mInstance = NULL;

	EventManager::EventManager() {
		mLog = new log4cpp::FixedContextCategory(PIXY_LOG_CATEGORY, "EvtMgr");
		mLog->infoStream() << "up and running";
	}

	EventManager::~EventManager()
	{
		mLog->infoStream() << "shutting down";
		// clean up events
		while (!mEventPool.empty()) {
			//cout << "removing event from pool\n";
			mEventPool.pop_back();
			//releaseEvt(mEventPool.back());
		}
		while (!mEvents.empty()) {
			//cout << "removing event from queue\n";
			mEvents.pop_back();
		}

		// clean up Listeners

		mInstance = 0;

		mSubscribers.clear();

		if (mLog)
			delete mLog;

		mLog = 0;
	};

	//! Retrieve the singleton instance of this class
	EventManager* EventManager::getSingletonPtr() {
		return mInstance = (!mInstance) ? new EventManager() : mInstance;
	}

	EventManager& EventManager::getSingleton() {
		return *getSingletonPtr();
	}

	void EventManager::shutdown() {
		delete mInstance;
	}

	void EventManager::subscribe(std::string inName, EventListener* inListener)
	{
		//std::cout << "subscribing a listener to events named `" << inName << "'";

		// is the event registered yet?
		name_subscription_t::iterator _itr = mSubscribers.find(inName);

		if (_itr == mSubscribers.end()) {
			// event doesn't exist in our map yet, register it
			vector<EventListener*> _listeners;
			_itr = mSubscribers.insert(make_pair(inName, _listeners)).first;
		}

		// is the listener already registered?
		if (alreadySubscribed(inListener, &_itr->second))
			return;


		//std::cout << "registering listener to event named " << inName;
		// it's safe to subscribe this Listener
		_itr->second.push_back(inListener);
	};


	void EventManager::hook(Event* inEvt)
	{
		mEvents.push_back(inEvt);
	}; // hook()

	void EventManager::update()
	{
		if (!mEvents.empty())
		{
			dispatch(mEvents.front());
			dequeue();
		}
	}; // update()


	void EventManager::dispatch(Event* inEvt)
	{
		/*
		 *	a) notify all listeners with full subscription
		 *	b) notify all listeners subscribed to the event's category
		 *	c) notify all listeners subscribed to the event's name
		 */

		/* \------------------------------------------------------------ /*
		 *
		 *
		 *  @ mSubscribers : ptr to the container of the Event's
		 *      subscribers; this list will be transferred to our
		 *      mSubMap map.
		 *
		 *  @ _vecItr : iterates through the original subscribers
		 *      list during the transfer
		 *
		 *  @ mSubMap : maps each subscribed registered to the Event along
		 *      with its status; the flag indicates whether this helper method
		 *      is done processing or not. The Event will be flagged as EVT_DONE
		 *      only when ALL subscribers in the map are flagged as true.
		 *
		 *  @ _mapItr : used for iterating through the map and calling
		 *      the subscribers; and finally for determining the count
		 *      of subscribers handled
		 *
		 *  @ fDone : tracks the status of the subscribers;
		 *      when fDone is true, the Event will be dispatched.
		 *
		 *
		 * \--------------------------------------------------------------- */

		vector<EventListener*>* evtListeners = 0;
		vector<EventListener*>::const_iterator _listener;
		name_subscription_t::iterator _nameFinder;

		// are there any listeners for this named event?
		_nameFinder = mSubscribers.find(inEvt->getName());
		if (_nameFinder != mSubscribers.end()) {
			// there are listeners interested in this named event
			// notify them
			evtListeners = &_nameFinder->second;
			_listener = evtListeners->begin();
			for (_listener; _listener != evtListeners->end(); ++_listener)
				(*_listener)->enqueue(inEvt);

		}

	} // dispatch()

	void EventManager::dequeue() {
		// remove from queue
		//mLog->debugStream() << "dequeuing event";
		Event* evt = mEvents.front();
		if (evt->_getNrHandlers() == 0) {
			mLog->warnStream() << "WARN - " << evt << " has no handlers! Releasing...";
			this->_releaseEvt(evt);
			return;
		}
		mEvents.pop_front();

	}

	void EventManager::_releaseEvt(Event* inEvt) {
		//mLog->debugStream() << "EventPool size: " << mEventPool.size() << " Events queue size: " << mEvents.size();

		 // remove from pool
		 list<Event*>::iterator _itr;
		 for (_itr=mEventPool.begin(); _itr != mEventPool.end(); ++_itr)
			 if ((*_itr) == inEvt) {
				 mEventPool.erase(_itr);
				 break;
			 }

		deque<Event*>::iterator _q_itr;
		for (_q_itr=mEvents.begin(); _q_itr != mEvents.end(); ++_q_itr)
			if ((*_q_itr) == inEvt) {
				mEvents.erase(_q_itr);
				break;
			}

		// destroy!!
		//mLog->debugStream() << "destroying " << inEvt;
		delete inEvt;

    inEvt = 0;
	}

	bool EventManager::alreadySubscribed(EventListener* inListener, vector<EventListener*>* inList) {
		vector<EventListener*>::const_iterator _finder = inList->begin();
		for (_finder; _finder != inList->end(); ++_finder)
			if (*_finder == inListener)
				return true; // oh here it is

		// not found
		return false;
	}


	void EventManager::unsubscribe(std::string inName, EventListener* inListener) {
		detachListener(inListener, &(mSubscribers.find(inName)->second));

	}

	void EventManager::detachListener(EventListener* inListener, subscription_t* inContainer) {
		subscription_t::iterator lSubscriber = inContainer->begin();
		for (lSubscriber;
			 lSubscriber != inContainer->end();
			 ++lSubscriber)
			if (*lSubscriber == inListener) {
				inContainer->erase(lSubscriber);
				break;
			}
	}

}
