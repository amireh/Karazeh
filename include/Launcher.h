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

#ifndef H_Launcher_H
#define H_Launcher_H

#include <exception>
#include <stdint.h>
#include <map>
#include <Ogre.h>
#include <OgrePlatform.h>
#include <OgreConfigFile.h>
#include <OgreWindowEventUtilities.h>
#include <OgreException.h>
#if OGRE_PLATFORM == OGRE_PLATFORM_APPLE
#include "OSX/macUtils.h"
#endif
#include "Pixy.h"
#include "InputManager.h"
#include "Downloader.h"
#include <boost/thread.hpp>

namespace Pixy
{

	class Launcher : public OIS::KeyListener, OIS::MouseListener, Ogre::WindowEventListener {
	public:
		~Launcher( void );
		
		//! Loads StateGame game state to start the game.
		/*! 
		 *	
		 */
		void go();
				
		//!	Shuts down the system, consequently shutting down all running game states.
		/*! 
		 *	\note
		 *	
		 */
		void requestShutdown();
		
		//! Retrieves a pointer to the Singleton instance of this class.
		/*!
		 *	@return
		 *	Pointer to the instance of this Singleton class.
		 */
		static Launcher* getSingletonPtr();
		static Launcher& getSingleton();
		
		static void launchDownloader();
		
		const std::string getVersion();
		
		void evtValidateStarted();
		void evtValidateComplete(bool needsUpdate);
		void evtFetchStarted();
		void evtFetchComplete(bool success);
		void evtPatchStarted();
		void evtPatchComplete(bool success);
		
	private:
		Launcher();
		Launcher(const Launcher&) {}
		Launcher& operator=(const Launcher&);
		
		//! Prepares Ogre for use by the game components
		/*! 
		 *	\note
		 *	This method is called internally within startGame().
		 *
		 *	Ogre Root, RenderWindow, Resource Groups, and SceneManager
		 *	are set up here.
		 */
		bool configureGame();
		
		void loadRenderSystems();
		
		//! Loads resources for use by the Ogre engine
		/*! 
		 *	\note
		 *	This method is called internally within startGame().
		 *
		 *	Parses configuration scripts and re/stores settings.
		 */
		void setupResources(std::string inResourcesPath);
		
		//! OIS key input event handler/dispatcher method
		/*! 
		 *	\note
		 *	Events received here are dispatched to the
		 *	current running GameState for processing.
		 */
		bool keyPressed( const OIS::KeyEvent &e );

		//! OIS key input event handler/dispatcher method
		/*! 
		 *	\note
		 *	Events received here are dispatched to the
		 *	current running GameState for processing.
		 */
		bool keyReleased( const OIS::KeyEvent &e );
		
		//! OIS mouse input event handler/dispatcher method
		/*! 
		 *	\note
		 *	Events received here are dispatched to the
		 *	current running GameState for processing.
		 */	
		bool mouseMoved( const OIS::MouseEvent &e );

		//! OIS mouse input event handler/dispatcher method
		/*! 
		 *	\note
		 *	Events received here are dispatched to the
		 *	current running GameState for processing.
		 */	
		bool mousePressed( const OIS::MouseEvent &e, OIS::MouseButtonID id );

		//! OIS mouse input event handler/dispatcher method
		/*! 
		 *	\note
		 *	Events received here are dispatched to the
		 *	current running GameState for processing.
		 */		
		bool mouseReleased( const OIS::MouseEvent &e, OIS::MouseButtonID id );
		
		/*! Starts off the logger
		 *
		 */
		void initLogger();
		
		Ogre::Root			    *mRoot;
		Ogre::RenderWindow	*mRenderWindow;
		InputManager		    *mInputMgr;
		
		unsigned long lTimeLastFrame, lTimeCurrentFrame, lTimeSinceLastFrame;
		
		//! do we want to shutdown?
		bool fShutdown;
		static Launcher *mLauncher;
		log4cpp::Category* mLog;
				
		std::string mConfigPath;
		
		Downloader *mDownloader;
	};
} // end of namespace

#endif
