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

#ifndef H_Renderer_H
#define H_Renderer_H

#include "Pixy.h"
#include "InputManager.h"
#include "Patcher.h"
#include "EventManager.h"
#include "EventListener.h"

namespace Pixy {
	
	/*	\class Renderer
	 *	\brief
	 *	
	 */
	class Renderer : public OIS::KeyListener, public OIS::MouseListener, public Pixy::EventListener {
		
	public:
	  inline Renderer() { mLog = 0; fSetup = false; mName = "Renderer"; };
		inline virtual ~Renderer() { mLog = 0; fSetup = false; mName = ""; };
		
		inline virtual std::string& getName() { return mName; };
		/* \brief
		 *
		 */
		virtual bool setup()=0;
		
		/* \brief
		 *
		 */
		virtual bool deferredSetup()=0;

		/* \brief
		 *
		 */		
		virtual void update(unsigned long lTimeElapsed)=0;
		
		/* \brief
		 *
		 */
 		virtual bool cleanup()=0;
		
		virtual void getWindowHandle(size_t *windowHnd)=0;
		virtual void getWindowExtents(int *width, int *height)=0;
		
		virtual bool injectError(Event* inEvt)=0;
    virtual bool injectNotice(Event* inEvt)=0;
    virtual bool injectPrompt(Event* inEvt)=0;
    virtual bool injectStatus(Event* inEvt)=0;
    virtual bool injectProgress(Event* inEvt)=0;
    
		//! OIS key input event handler/dispatcher method
		/*! 
		 *	\note
		 *	Events received here are dispatched to the
		 *	current running GameState for processing.
		 */
		virtual bool keyPressed( const OIS::KeyEvent &e )=0;

		//! OIS key input event handler/dispatcher method
		/*! 
		 *	\note
		 *	Events received here are dispatched to the
		 *	current running GameState for processing.
		 */
		virtual bool keyReleased( const OIS::KeyEvent &e )=0;
		
		//! OIS mouse input event handler/dispatcher method
		/*! 
		 *	\note
		 *	Events received here are dispatched to the
		 *	current running GameState for processing.
		 */	
		virtual bool mouseMoved( const OIS::MouseEvent &e )=0;

		//! OIS mouse input event handler/dispatcher method
		/*! 
		 *	\note
		 *	Events received here are dispatched to the
		 *	current running GameState for processing.
		 */	
		virtual bool mousePressed( const OIS::MouseEvent &e, OIS::MouseButtonID id )=0;

		//! OIS mouse input event handler/dispatcher method
		/*! 
		 *	\note
		 *	Events received here are dispatched to the
		 *	current running GameState for processing.
		 */		
		virtual bool mouseReleased( const OIS::MouseEvent &e, OIS::MouseButtonID id )=0;

	protected:
    log4cpp::Category* mLog;
    bool fSetup;
    std::string mName;
    
	private:
		Renderer(const Renderer& src);
		Renderer& operator=(const Renderer& rhs);
	};
}
#endif
