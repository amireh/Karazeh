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
#include "Renderers/Ogre/InputManager.h"
#include <Ogre.h>
#include <OgreConfigFile.h>
#include <OgreWindowEventUtilities.h>
#include <OgreException.h>
#if OGRE_PLATFORM == OGRE_PLATFORM_APPLE
#include "OSX/macUtils.h"
#endif
#include "Renderers/Ogre/OgreSdkTrays.h"

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
    public OIS::MouseListener
	{

	public:
	  OgreRenderer();
		virtual ~OgreRenderer();

		/* \brief
		 *
		 */
		virtual bool setup(int argc, char** argv);

		/* \brief
		 *
		 */
		virtual void update(unsigned long lTimeElapsed);

		/* \brief
		 *
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
		virtual void injectValidateComplete( bool inNeedUpdate, const Version& inTargetVersion );
		virtual void injectPatchStarted( const Version& inTargetVersion );
		virtual void injectPatchProgress( int inPercent );
		virtual void injectPatchFailed( std::string inMsg, const Version& inTargetVersion );
		virtual void injectPatchComplete( const Version& inCurrentVersion );
		virtual void injectApplicationPatched( const Version& inCurrentVersion );

	protected:
    InputManager *mInputMgr;

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

	private:
		OgreRenderer(const OgreRenderer& src);
		OgreRenderer& operator=(const OgreRenderer& rhs);
	};
}
#endif
