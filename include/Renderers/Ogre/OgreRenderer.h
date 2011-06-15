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

#ifndef H_OgreRenderer_H
#define H_OgreRenderer_H

#include "Renderer.h"
#include <Ogre.h>
#include <OgreConfigFile.h>
#include <OgreWindowEventUtilities.h>
#include <OgreException.h>
#if OGRE_PLATFORM == OGRE_PLATFORM_APPLE
#include "OSX/macUtils.h"
#endif
#include "Renderers/Ogre/InputManager.h"
#include "Renderers/Ogre/OgreSdkTrays.h"
#include "Renderers/Ogre/EventManager.h"
#include "Renderers/Ogre/EventListener.h"

namespace Pixy {

	/*	\class OgreRenderer
	 *	\brief
	 *
	 */
	class OgreRenderer :
	  public Renderer,
	  public Ogre::WindowEventListener,
	  public OgreBites::SdkTrayListener,
    public OIS::KeyListener,
    public OIS::MouseListener,
    public Pixy::EventListener
	{

	public:
	  OgreRenderer();
		virtual ~OgreRenderer();

		/*! \brief
		 *  Sets up Ogre, EventManager, OIS, and OgreSdkTrays.
		 */
		virtual bool setup(int argc, char** argv);

    /*! \brief
     *  Contains the main loop in which all the components are updated and
     *  the event loop.
     */
		virtual void go(int argc, char** argv);

		/*! \brief
		 *  Cleans up the SdkTray manager.
		 */
 		virtual bool cleanup();

		/*! \brief
     *  Assign the handle to the window the Renderer has created. This is used
     *  by OIS to capture window input.
     *
     *  \warn
     *  This MUST return a valid window handle otherwise the InputManager will crash.
     */
		virtual void getWindowHandle(size_t *windowHnd);

		/*! \brief
		 *  Assign the dimensions of the window you're creating. Used by the
		 *  InputManager.
		 */
		virtual void getWindowExtents(int *width, int *height);

		//! OIS key input event handler/dispatcher method
		/*!
		 *	\note
		 *	Events received here are dispatched to the
		 *	current running GameState for processing.
		 */
		virtual bool keyPressed( const OIS::KeyEvent &e );

		//! OIS key input event handler/dispatcher method
		/*!
		 *	\note
		 *	Events received here are dispatched to the
		 *	current running GameState for processing.
		 */
		virtual bool keyReleased( const OIS::KeyEvent &e );

		//! OIS mouse input event handler/dispatcher method
		/*!
		 *	\note
		 *	Events received here are dispatched to the
		 *	current running GameState for processing.
		 */
		virtual bool mouseMoved( const OIS::MouseEvent &e );

		//! OIS mouse input event handler/dispatcher method
		/*!
		 *	\note
		 *	Events received here are dispatched to the
		 *	current running GameState for processing.
		 */
		virtual bool mousePressed( const OIS::MouseEvent &e, OIS::MouseButtonID id );

		//! OIS mouse input event handler/dispatcher method
		/*!
		 *	\note
		 *	Events received here are dispatched to the
		 *	current running GameState for processing.
		 */
		virtual bool mouseReleased( const OIS::MouseEvent &e, OIS::MouseButtonID id );

		virtual void injectUnableToConnect( void );
		virtual void injectValidateStarted( void );
		virtual void injectValidateComplete( bool inNeedUpdate, Version const& inTargetVersion );
		virtual void injectPatchStarted( Version const& inTargetVersion );
    virtual void injectPatchSize( pbigint_t inBytes );
		virtual void injectPatchProgress( float inPercent );
		virtual void injectPatchFailed( std::string inMsg, Version const& inTargetVersion );
		virtual void injectPatchComplete( Version const& inCurrentVersion );
		virtual void injectApplicationPatched( Version const& inCurrentVersion );

	protected:
    bool evtGuiLoaded(Event* inEvt);

		bool evtUnableToConnect( Event* inEvt );
		bool evtValidateStarted( Event* inEvt );
		bool evtValidateComplete( Event* inEvt );
		bool evtPatchStarted( Event* inEvt );
		bool evtPatchProgress( Event* inEvt );
		bool evtPatchFailed( Event* inEvt );
		bool evtPatchComplete( Event* inEvt );
		bool evtApplicationPatched( Event* inEvt );

    InputManager *mInputMgr;
    EventManager *mEvtMgr;

    Ogre::Root *mRoot;
    Ogre::RenderWindow *mRenderWindow;
    Ogre::Viewport* mViewport;
		Ogre::OverlayManager *mOverlayMgr;
		OgreBites::SdkTrayManager *mTrayMgr;

		Ogre::FrameEvent mFrameEvt;

		OgreBites::TextBox* mStatusBox;
		OgreBites::ProgressBar* mProgress;

		virtual void buttonHit(OgreBites::Button* b);
		virtual void yesNoDialogClosed(const Ogre::DisplayString& question, bool yesHit);

		void setupResources(std::string inPath);
		bool configureGame();
		void loadRenderSystems();

		bool fShowingOkDialog;
    bool fShutdown;

    pbigint_t mPatchSize;

    unsigned long lTimeLastFrame, lTimeCurrentFrame, lTimeSinceLastFrame;

	private:
		OgreRenderer(const OgreRenderer& src);
		OgreRenderer& operator=(const OgreRenderer& rhs);
	};
}
#endif
