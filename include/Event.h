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

#ifndef H_Event_H
#define H_Event_H

#include <vector>
#include <exception>
#include <map>
#include <string>
#include <iostream>
#include <stdint.h>

using std::ostream;
using std::map;
namespace Pixy
{
	
  // obsolete, is only needed for a networked environment
	typedef enum {
		EVT_REQ,
		EVT_RESP
	} EVENT_TYPE;
	
	// obsolete, is only needed for a networked environment
	typedef enum {
		EVT_OK,
		EVT_ERROR,
		EVT_INVALID_REQ
	} EVENT_FEEDBACK;
	
	/*! \enum Pixy::EVENT_STATUS
	 * Describes the state of Event objects.
	 *
	 * \remarks
     *  These states are used by the EventHandler to process
     *  the Events. Events start in IDLE status, and eventually
     *  return to that state; once an Event is hooked to the
     *  handler, it waits until it can be processed by the queue.
     *  Once eligible, it will be flagged as PROCESSING, during
     *  this state, all subscribers to Event are called to handle.
     *  Until all subscribers register as being done handling the Event,
     *  it is flagged as DONE and unhooked.
     *
	 */
    typedef enum
		{
			EVT_IDLE,       //! The Event is not fired yet
			EVT_PROCESSING, //! The Event is fired and being handled by its subscribers
			EVT_DONE        //! The Event was fired and all subscribers are done handling
		} EVENT_STATUS;
	
	
    /*! \enum Pixy::EVENT_WEIGHT
     *  Describes the rank of the Event, by which the processing queue
     *  handles Events.
     *
     *  \remarks
     *  When a MAJOR Event is being processed, and a MINOR Event
     *  has been hooked, priority is given to the minor Event.
     *  Once the minor Event is done, the priority is given back to
     *  the last major event.
     *
     *  This mechanism is useful against certain Events that require
     *  the player's interaction; such as a response to an interface
     *  menu could be considered a Major event, because the Event
     *  will not be processed until input is made by the User. However,
     *  Events such as moving units or casting a spell are considered
     *  to be minor Events, since the processing of these Events is not
     *  dependant on user input.
	 *
	 *	\warning
	 *	Subclasses of Event MUST implement a copy constructor, see Event::clone()
     */
    typedef enum
		{
			MINOR_EVT,
			MAJOR_EVT
		} EVENT_WEIGHT;
	
	class Player;
    /*! \class Event "Event.h" "include/Event.h"
     *  \brief
     *  Base Event object that is used and handled to represent game events.
     */
    class Event {
	public:
		
		//! default ctor
		//Event(const int inId, const std::string inName);
		Event(const std::string inName);
		
		//! destructor; resets evt state
		virtual ~Event();
		
		//virtual /**/int getId() const;

		
		/*! \brief
		 *  Sets the name of this Event, used as a convenience for debugging
		 */
		virtual void setCategory(const std::string inCategory);
		
		/*! \brief
		 *  Retrieves the name of this Event.
		 */
		virtual std::string getCategory() const;
		
		/*! \brief
		 *  Sets the name of this Event, used as a convenience for debugging
		 */
		virtual void setName(const std::string inName);
		
		/*! \brief
		 *  Retrieves the name of this Event.
		 */
		virtual std::string getName() const;
		
		/*! \brief
		 *  Sets the Rank of this Event.
		 */
		virtual void setWeight(const EVENT_WEIGHT inRank);
		
		/*! \brief
		 *  Retrieves the Rank of this Event.
		 */
		virtual EVENT_WEIGHT getWeight() const;
		
		/*! \brief
		 *  Equality comparison operator, works using the
		 *  Event name which is the unique identifier.
		 */
		//virtual bool operator==(const Event& rhs);
		
		virtual void setType(EVENT_TYPE inType);
		virtual EVENT_TYPE getType() const;
		
		virtual void setFeedback(EVENT_FEEDBACK inMsg);
		virtual EVENT_FEEDBACK getFeedback() const;
		
		virtual bool isConfidential() const;
		
		virtual void addHandler();
		virtual void removeHandler();
		
		std::string getProperty(std::string inName) const;
		virtual void setProperty(const std::string inName, const std::string inValue);
		virtual bool propertyExists(const std::string& inName);
		
		friend ostream& operator<<(ostream& outStream, Event* inEvt);
        
		virtual void dump(std::ostream& inStream = std::cout);

    virtual void setAny(void* any);
    virtual void* getAny();
    			
		uint8_t getNrHandlers();
	protected:
		typedef	map< std::string, std::string > property_t;
		
		int				mId;
		EVENT_WEIGHT	mWeight;
		EVENT_TYPE		mType;
		EVENT_FEEDBACK	mFeedback;
		std::string		mName;
		std::string		mCategory;
		bool			fConfidential;
		uint8_t			nrHandlers;
		int				nrProperties;
		property_t		mProperties;
		void * mUserData;
		
		virtual void reset();
		
	private:
		Event(const Event& src);
		Event& operator=(const Event& rhs);
		//virtual void clone(const Event& src);
		
	};
} // end of namespace
#endif // H_Event_H
