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

#ifndef H_EventHandler_H
#define H_EventHandler_H

#include <iostream>
#include <utility>

#include "Handler.h"

namespace Pixy
{
    class Event;
	
    /*! \class EventHandler
     *
     *  \brief
     *  Functionoid encapsulating event handling methods
     *
     *  \note
     *  You should never need to create, call, or register an EventHandler
     *  directly. Use EventListener for binding instead.
     */
    template < class T, class EventType >
    class EventHandler : public Handler	{
        public:
            typedef bool (T::*methodPtr)(EventType*);
			
            //! ctor
            inline EventHandler(T* inInstance, methodPtr inMethod) :
            mClassInstance(inInstance),
            mMethod(inMethod)
            {
            };
			
            //! dtor
            inline virtual ~EventHandler()
            {
                mClassInstance = NULL;
                mMethod = NULL;
            };
			
            /*!
             *  \brief
             *  Responsible of calling the appropriate handler method of the subscriber.
             *
             *  \remarks
             *  Depending on the EventType, call() will be able to determine on runtime
             *  the proper registered method to call.
             *
             *  \note
             *  This method is called internally by Pixy::EventHandler
             *  and should not be called directly.
             */
            inline virtual bool call(Event* inEvt)
            {
                return ((*mClassInstance).*mMethod)(static_cast<EventType*>(inEvt));
            };
			
        protected:
            T* mClassInstance;
            methodPtr mMethod;
		};
}
#endif // H_EventHandler_H
