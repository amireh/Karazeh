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
