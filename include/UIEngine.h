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

#ifndef H_UIEngine_H
#define H_UIEngine_H

#include "InputManager.h"
#include <Ogre.h>
#include "ogre/OgreSdkTrays.h"


using namespace OgreBites;
namespace Pixy {
	
	
	/*	\class UIEngine
	 *	\brief
	 *	Manages all GUI and HUD elements of the game, primarily the Menu screen
	 *  and the game interface.
	 */
	class UIEngine : public SdkTrayListener {
		
	public:
		virtual ~UIEngine();
		static UIEngine* getSingletonPtr();
		static UIEngine& getSingleton();
		
		/* \brief
		 *  prepares shared resources across all gamestates; namely, the SdkTrayManager,
		 *  the Menu widgets and event bindings 
		 */
		virtual bool setup();
		
		/* \brief
		 *  prepares GameState-exclusive resources, in the case of Level state,
		 *  the overlays are refitted, see refitOverlays();
		 */
		virtual bool deferredSetup();
		virtual void update(unsigned long lTimeElapsed);
		
		/* \brief
		 *  Not much happens here since the SdkTrayMgr has to live through the whole
		 *  engine life, otherwise it goes bananas and raises segfaults.
		 */
		virtual bool cleanup();
		
		void keyPressed( const OIS::KeyEvent &e );
		void keyReleased( const OIS::KeyEvent &e );		
		void mouseMoved( const OIS::MouseEvent &e );
		void mousePressed( const OIS::MouseEvent &e, OIS::MouseButtonID id );
		void mouseReleased( const OIS::MouseEvent &e, OIS::MouseButtonID id );

		
	protected:

	  // prompts Ogre::OverlayManager for the overlays we're gonna use
	  // and assigns pointers to them
	  void assignHandles();
	  
	  // resizes shield bars to span 1/3 of the viewport each, and scales
	  // font size depending on the screen resolution 
	  void refitOverlays();
	  
	  // creates widgets for our menu
	  virtual void setupWidgets();

		EventManager *mEvtMgr;

    Ogre::Root *mRoot;
    Ogre::RenderWindow *mWindow;
    Ogre::Viewport* mViewport;
    SdkTrayManager *mTrayMgr;
		Ogre::OverlayManager *mOverlayMgr;

		Ogre::FrameEvent evt;
		
    // menu event handlers
    virtual void itemSelected(SelectMenu* menu);
	  virtual void buttonHit(OgreBites::Button* b);
    virtual void buttonOver(OgreBites::Button* b);
    virtual void okDialogClosed(const Ogre::DisplayString& message);
        	
	private:
		static UIEngine* __instance;
		UIEngine();
		UIEngine(const UIEngine& src);
		UIEngine& operator=(const UIEngine& rhs);
	};
}
#endif
