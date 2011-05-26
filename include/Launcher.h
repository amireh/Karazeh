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
#include <stdlib.h>
#include <signal.h>
#include <map>
#include "Pixy.h"
#include "Renderer.h"
#include "EventManager.h"
#include "InputManager.h"
#include "Downloader.h"
#include <boost/thread.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

namespace Pixy
{

	class Launcher {
	public:
		~Launcher( void );

		//! Retrieves a pointer to the Singleton instance of this class.
		/*!
		 *	@return
		 *	Pointer to the instance of this Singleton class.
		 */
		static Launcher* getSingletonPtr();
		static Launcher& getSingleton();
				
		//! Loads StateGame game state to start the game.
		/*! 
		 *	
		 */
		void go(const char* inRendererName = 0);
				
		//!	Shuts down the system, consequently shutting down all running game states.
		/*! 
		 *	\note
		 *	
		 */
		void requestShutdown();
		
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
		
		void (Launcher::*goFunc)();
		
		void goWithRenderer();
		void goVanilla();
		
		void loadRenderSystems();
	
		/*! Starts off the logger
		 *
		 */
		void initLogger();
		
		Renderer *mRenderer;
		InputManager		    *mInputMgr;
		
		//unsigned long lTimeLastFrame, lTimeCurrentFrame, lTimeSinceLastFrame;
		boost::posix_time::ptime  lTimeLastFrame, lTimeCurrentFrame;
		boost::posix_time::time_duration lTimeSinceLastFrame;
		
		//! do we want to shutdown?
		bool fShutdown;
		static Launcher *mLauncher;
		log4cpp::Category* mLog;
				
		std::string mConfigPath;
		
		Downloader *mDownloader;
	};
} // end of namespace

#endif
