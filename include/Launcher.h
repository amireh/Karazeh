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
#include "PixyThread.h"
#include "KarazehConfig.h"
#include "Renderer.h"
#include "Patcher.h"
#include "Downloader.h"
#include "binreloc.h"

// thread support
#ifdef KARAZEH_USE_QT
#include <QThread>
#else
#include <boost/thread.hpp>
#endif

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
		void go(int argc, char** argv);

		/*! \brief
		 *  Terminates the current process and launches the application found at
		 *  inPath identified by inAppName using execl();
		 *
		 *  \arg inPath: full path to the application with extension, ie C:\\Foo.exe
		 *  \arg inAppName: stripped name of the application, ie Foo
		 */
		void launchExternalApp();

		/*! \brief
		 *	Shuts down the system and all components.
		 */
		void requestShutdown();

    /*! \brief
     *  Fires up the processor thread which calls the Patcher.
     *
     * The first time Patcher() is called, it validates the application. The
     * second call makes it actually process the patch. The first call is always
     * triggered by the Launcher, while the second is triggered by the Renderer
     * based on user input (if they want to update or not).
     */
    void updateApplication();

		Renderer* getRenderer();

    std::string& getRootPath();
    std::string& getTempPath();
    std::string& getBinPath();

	private:
		Launcher();
		Launcher(const Launcher&) {}
		Launcher& operator=(const Launcher&);

    static Launcher *__instance;

    /*! \brief
     *  Determines the paths to the application.
     *
     * In Linux, this is done using BinReloc, on Mac it's done using NSBundlePath(),
     * and on Windows it's done using GetModuleFileName().
     */
    void resolvePaths();

		/*! \brief
		 *  Starts up the log4cpp logger.
		 */
		void initLogger();

		Renderer      *mRenderer;

		log4cpp::Category* mLog;

    std::string mBinPath;
    std::string mRootPath;
    std::string mTempPath;
    std::string mLogPath;
/*
#ifdef KARAZEH_USE_QT
    // processor using Qt threads
    class Processor: public QThread {
      public:
      void run() {
        Patcher::getSingleton()();
      }
    };
#else
    // processor using boost threads
    class Processor {
      public:
      Processor() { };
      ~Processor() { };
      bool operator()() {
        Patcher::getSingleton()();
      }
    };
#endif
    Processor mProc;*/
    Thread<Patcher> *mProc;
	};
} // end of namespace

#endif
