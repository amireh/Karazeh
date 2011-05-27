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
#include "Patcher.h"
#include "Downloader.h"
#include <boost/thread.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

namespace Pixy
{
  /*! \class Launcher
   *  \brief
   *  The Launcher is the application's "root", initialises the components,
   *  the Renderer, maintains the main loop, and launches the target application.
   *
   *  Renderers can be attached to the Launcher which will handle all the UI
   *  events and interact with the Patcher/Downloader. See OgreRenderer for
   *  an example Renderer.
   */
	class Launcher {
	public:
		~Launcher( void );

		static Launcher* getSingletonPtr();
		static Launcher& getSingleton();
				
		/*! \brief 
		 *	Starts up the components: InputManager, Renderer, Patcher and Downloader,
		 *  fires Patcher::validate() in a thread, and begins the application loop.
		 *
		 *  \arg inRendererName specify which Renderer you'd like to use, if empty
		 *  the "vanilla" renderer is used: stdout
		 */
		void go(const char* inRendererName = 0);
		
		/*! \brief 
		 *  Terminates the current process and launches the application found at
		 *  inPath identified by inAppName using execl();
		 *
		 *  \arg inPath: full path to the application with extension, ie C:\\Foo.exe
		 *  \arg inAppName: stripped name of the application, ie Foo
		 */		
		void launchExternalApp(std::string inPath, std::string inAppName);
		
		/*! \brief 
		 *	Shuts down the system and all components.
		 */
		void requestShutdown();
		
	private:
		Launcher();
		Launcher(const Launcher&) {}
		Launcher& operator=(const Launcher&);
		
		void (Launcher::*goFunc)();
		
		void goWithRenderer();
		void goVanilla();
		
		/*! \brief 
		 *  Starts up the log4cpp logger.
		 */
		void initLogger();
		
		Renderer      *mRenderer;
		InputManager	*mInputMgr;
		
		//unsigned long lTimeLastFrame, lTimeCurrentFrame, lTimeSinceLastFrame;
		boost::posix_time::ptime  lTimeLastFrame, lTimeCurrentFrame;
		boost::posix_time::time_duration lTimeSinceLastFrame;
		
		bool fShutdown;
		static Launcher *mLauncher;
		log4cpp::Category* mLog;
				
		std::string mConfigPath;
		
	};
} // end of namespace

#endif
