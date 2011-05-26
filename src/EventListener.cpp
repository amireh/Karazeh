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

#include "EventListener.h"

namespace Pixy {
	
	EventListener::EventListener() {
		mFBHandler = 0;
		mFullBindings.clear();
		mCatBindings.clear();
		mNameBindings.clear();
		mTracker.clear();
		while (!mEvents.empty())
			mEvents.pop();
	}
	
	EventListener::~EventListener() {

		// unbind ourself from the EventManager
		// full bindings
		EventManager::getSingletonPtr()->unsubscribe(this);
		// category bindings
		cat_bind_t::iterator lCategory = mCatBindings.begin();
		for (lCategory;
			 lCategory != mCatBindings.end();
			 ++lCategory) {
			EventManager::getSingletonPtr()->unsubscribeFromCat(lCategory->first, this);
		}
		// finally, name bindings
		name_bind_t::iterator lEvent = mNameBindings.begin();
		for (lEvent;
			 lEvent != mNameBindings.end();
			 ++lEvent) {
			EventManager::getSingletonPtr()->unsubscribeFromName(lEvent->first, this);
		}
		
		// now destroy our handler objects
		// destroy general bindings
		if (!mFullBindings.empty()) {
			handler_list_t::iterator _handler = mFullBindings.begin();
			for (_handler; _handler != mFullBindings.end(); ++_handler)
				delete *_handler;
			
			mFullBindings.clear();
		}
		
		// destroy name specific bindings		
		if (!mNameBindings.empty()) {
			name_bind_t::iterator _itr = mNameBindings.begin();
			for (_itr; _itr != mNameBindings.end(); ++_itr) {
				handler_list_t::iterator _handler = _itr->second.begin();
				for (_handler; _handler != _itr->second.end(); ++_handler)
					delete *_handler;
			}
			mNameBindings.clear();
		}
		
		// destroy category bindings
		if (!mCatBindings.empty()) {
			cat_bind_t::iterator _itr = mCatBindings.begin();
			for (_itr; _itr != mCatBindings.end(); ++_itr) {
				handler_list_t::iterator _handler = _itr->second.begin();
				for (_handler; _handler != _itr->second.end(); ++_handler)
					delete *_handler;
			}
			mCatBindings.clear();
		}
		
		
		// clean up any events
		while (!mEvents.empty())
			mEvents.pop();
		
		if (!mTracker.empty())
			mTracker.clear();
		
		if (mFBHandler)
			delete mFBHandler;
		
		mFBHandler = 0;

	}
	
	void EventListener::enqueue(Event* inEvt) {

		
		// do we have any handlers?
		if ( listeningTo(inEvt) ) {
			
			// inform the event that there's a listener using it
			// so it can destruct properly when *all* listeners
			// are done processing it
			inEvt->addHandler();
			
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
		/*
		else
			if (mFBHandler)
				mFBHandler->call(inEvt); // call fallback handler, if any
		*/
	}
	
	void EventListener::processEvents() {
		// do we have any events to process?
		if (mEvents.empty())
			return;
		
		//std::cout << "handling events\n";
		// we do, grab the first
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
			full_bind_t::iterator	lFullTracker;
			cat_bind_t::iterator	lCatTracker;
			name_bind_t::iterator	lNameTracker;
			
			/*
			_bindTracker = mBindings.find(mEvt->getUID());
			if (_bindTracker != mBindings.end())
				mHandlers.push_back(&(_bindTracker->second));
			*/
			
			// enqueue full bindings
			mHandlers.push_back(&(mFullBindings));
			
			// enqueue category bindings
			lCatTracker = mCatBindings.find(mEvt->getCategory());
			if (lCatTracker != mCatBindings.end())
				mHandlers.push_back(&(lCatTracker->second));
			
			// enqueue name bindings
			lNameTracker = mNameBindings.find(mEvt->getName());
			if (lNameTracker != mNameBindings.end()) {
				mHandlers.push_back(&(lNameTracker->second));
			}
			
			
			/*std::cout << "tracking ["
			<< mHandlers[0]->size() << "] specific handlers, ["
			<< mHandlers[1]->size() << "] category handlers, [\n";
			//<< mHandlers[2]->size() << "] all event handlers\n";*/
			
			
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
			  
			
		} catch (std::exception& e) { // discard
			//Utility::getLogger().errorStream() << "** EvtListener: handler error! " << e.what() << "\n";
			std::cerr << "** EvtListener: handler error! " << e.what() << "\n";
		}
		
		if (mTracker.empty()) {
			// we're done with this event,
			// remove it and reset our tracker
			//std::cout << "no more handlers to call, detaching from event\n";
			
			mEvents.front()->removeHandler(); // inform the event that a handler is done
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
			  //std::cout << "erasing handler\n";
			  //inHandler->call(mEvents.front());
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
		
		return (!mFullBindings.empty() ||
				mCatBindings.find(inEvt->getCategory()) != mCatBindings.end() ||
				mNameBindings.find(inEvt->getName()) != mNameBindings.end()
		);
		
	}
	
}
