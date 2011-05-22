/*
 *  This file is part of Elementum.
 *
 *  Elementum - a cross-platform strategy game powered by Ogre3D.
 *  Copyright (C) 2011 
 *    Philip Allgaier <spacegaier@ogre3d.org>, 
 *    Ahmad Amireh <ahmad@amireh.net>
 * 
 *  Elementum is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  Elementum is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with Elementum.  If not, see <http://www.gnu.org/licenses/>.
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
