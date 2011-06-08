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

#include "Renderers/Ogre/EventListener.h"

namespace Pixy {

	EventListener::EventListener() {
		mBindings.clear();
		mTracker.clear();
		//while (!mEvents.empty())
		//	mEvents.pop();
	}

	EventListener::~EventListener() {

		// unbind ourself from the EventManager
		name_bind_t::iterator lEvent = mBindings.begin();
		for (lEvent;
			 lEvent != mBindings.end();
			 ++lEvent) {
			EventManager::getSingletonPtr()->unsubscribe(lEvent->first, this);
		}

		// now destroy our handler objects
		if (!mBindings.empty()) {
			name_bind_t::iterator _itr = mBindings.begin();
			for (_itr; _itr != mBindings.end(); ++_itr) {
				handler_list_t::iterator _handler = _itr->second.begin();
				for (_handler; _handler != _itr->second.end(); ++_handler)
					delete *_handler;
			}
			mBindings.clear();
		}

		// clean up any events
		while (!mEvents.empty()) {
		  mEvents.front()->_removeHandler();
			mEvents.pop();
	  }

		if (!mTracker.empty())
			mTracker.clear();
	}

	void EventListener::enqueue(Event* inEvt) {


		// do we have any handlers?
		if ( listeningTo(inEvt) ) {

			// inform the event that there's a listener using it
			// so it can destruct properly when *all* listeners
			// are done processing it
			inEvt->_addHandler();

			// enqueue it for processing
			/*
			//std::cout
				<< "EvtListener: enqueuing event "
				<< inEvt
				<< ", and my queue has " << mEvents.size()
				<< " events\n";
			*/
			mEvents.push(inEvt);
		}
	}

	void EventListener::processEvents() {
		// do we have any events to process?
		if (mEvents.empty())
			return;

		Event* mEvt = mEvents.front();

		// let's track our Event handlers for calling
		// we will be using mTracker to call all handlers
		// bound to this Event every update loop until they're done.
		// Once the tracker is empty, we know that all handlers are
		// done processing
		if (mTracker.empty()) {


			// retrieve this Event's specific handlers
			// then retrieve handlers bound to this event's Category
			// then finally retrieve handlers bound to receive notifications
			// on ALL events (mCatBindings["Event"])

			vector< handler_list_t* > mHandlers;
			name_bind_t::iterator	lNameTracker;

			lNameTracker = mBindings.find(mEvt->getName());
			if (lNameTracker != mBindings.end()) {
				mHandlers.push_back(&(lNameTracker->second));
			}

			// now do the actual tracking...
			handler_list_t::iterator _handler;
			for (int i=0; i<mHandlers.size(); ++i) {

				_handler = mHandlers[i]->begin();
				for (_handler; _handler != mHandlers[i]->end(); ++_handler)
					// we don't need to check if handler is already tracked
					// on the first loop.. otherwise, we need to make sure
					// no handler is tracked twice ( via beingTracked() )
					if ( (i == 0) || !beingTracked((*_handler)) )
						track((*_handler));
			}

			mHandlers.clear();
		}

		// call the handlers we're tracking
		try	{
			list<Handler*>::iterator _handler = mTracker.begin();

			for (_handler; _handler != mTracker.end(); _handler)
				if ( (*_handler)->call(mEvt) )
					// handler is done, stop tracking it
				  _handler = mTracker.erase(_handler);
			  else
			    ++_handler;


		} catch (std::exception& e) { // abort
			//Utility::getLogger().errorStream() << "** EvtListener: handler error! " << e.what() << "\n";
			std::cerr << "** EvtListener: handler error! " << e.what() << "\n";
			mTracker.clear();
		}

		if (mTracker.empty()) {
			// we're done with this event,
			// remove it and reset our tracker
			//std::cout << "no more handlers to call, detaching from event\n";

			mEvents.front()->_removeHandler(); // inform the event that a handler is done
			/*
			//std::cout
			<< "EvtListener: removing event "
			<< mEvents.front()
			<< ", and my queue has " << mEvents.size()
			<< " events\n";
			 */
			mEvents.pop();
			mTracker.clear();
		}
	}

	void EventListener::track(Handler* inHandler) {
		mTracker.push_back(inHandler);
	}

	void EventListener::stopTracking(Handler* inHandler) {
	  if (!inHandler) {
	    //std::cout << "WARNING: INVALID HANDLER\n";
	    return;
	  }

	  //std::cout << mTracker.size() << "\n";

		list<Handler*>::iterator _itr = mTracker.begin();
		for (_itr; _itr != mTracker.end(); ++_itr)
			if ((*_itr) == inHandler) {
				mTracker.erase(_itr);
				break;
		  }

		//std::cout << "handlers are freed: " << mTracker.size() << "\n";
	}

	bool EventListener::beingTracked(Handler* inHandler) {
		list<Handler*>::iterator _itr = mTracker.begin();
		for (_itr; _itr != mTracker.end(); ++_itr)
			if ((*_itr) == inHandler)
				return true;

		return false;
	}

	bool EventListener::listeningTo(Event* inEvt) {
		return (mBindings.find(inEvt->getName()) != mBindings.end()	);
	}

}
