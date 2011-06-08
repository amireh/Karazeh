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

#include "Launcher.h"
#include "PixyLogLayout.h"
#include "Renderers/Vanilla/VanillaRenderer.h"
#ifdef KARAZEH_RENDERER_OGRE
#include "Renderers/Ogre/OgreRenderer.h"
#endif
#ifdef KARAZEH_RENDERER_Qt
#include "Renderers/Qt/QtRenderer.h"
#endif

namespace Pixy
{
	Launcher* Launcher::mLauncher;

  void handle_interrupt(int param)
  {
    printf("Signal %d received, shutdown is forced; attempting to cleanup. Please see the log.\n", param);
    Launcher::getSingleton().requestShutdown();
  }

	Launcher::Launcher() :
	mRenderer(0),
	fShutdown(false) {
	  signal(SIGINT, handle_interrupt);
	  signal(SIGTERM, handle_interrupt);
	  signal(SIGKILL, handle_interrupt);

	}

	Launcher::~Launcher() {

    if (mRenderer)
      delete mRenderer;

    delete Downloader::getSingletonPtr();
    delete Patcher::getSingletonPtr();

		mLog->infoStream() << "++++++ " << PIXY_APP_NAME << " cleaned up successfully ++++++";
		if (mLog)
		  delete mLog;

		log4cpp::Category::shutdown();

		mRenderer = NULL;
	}

	Launcher* Launcher::getSingletonPtr() {
		if( !mLauncher ) {
		    mLauncher = new Launcher();
		}

		return mLauncher;
	}

	Launcher& Launcher::getSingleton() {
		return *getSingletonPtr();
	}

	void Launcher::go(int argc, char** argv) {

    findPaths();

		// init logger
		initLogger();



		Patcher::getSingletonPtr();
		Downloader::getSingletonPtr();

		if (argc > 1) {
#ifdef KARAZEH_RENDERER_OGRE
		  if (strcmp(argv[1], "Ogre") == 0)
		    mRenderer = new OgreRenderer();
#elif defined(KARAZEH_RENDERER_Qt)
		  if (strcmp(argv[1], "Qt") == 0)
		    mRenderer = new QtRenderer();
#endif
      if (!mRenderer) {
        mLog->errorStream() << "unknown renderer specified! going vanilla";
      }
		}

    if (!mRenderer) {
      mRenderer = new VanillaRenderer();
    }

    bool res = mRenderer->setup(argc, argv);

    if (!res) {
      mLog->errorStream() << "could not initialise renderer!";
      return;
    }

    mRenderer->deferredSetup();

		// lTimeLastFrame remembers the last time that it was checked
		// we use it to calculate the time since last frame
		//lTimeLastFrame = boost::posix_time::microsec_clock::universal_time();
		//lTimeCurrentFrame = boost::posix_time::microsec_clock::universal_time();

    //mLog->infoStream() << "my current thread id: " << boost::this_thread::get_id();

    //boost::thread mThread(boost::ref(Patcher::getSingleton()));
    updateApplication();

		// main loop
    mRenderer->go();
	}

  void Launcher::findPaths() {

    BrInitError* brerr;
    int brres = br_init(brerr);

    if (brres == 0) {
      std::cerr << "binreloc could not be initialised\n";
    }

    using boost::filesystem::path;

#if PIXY_PLATFORM == PIXY_PLATFORM_LINUX
    mBinPath = br_find_exe_dir(".");
    mBinPath = path(mBinPath).string();
    mRootPath = path(mBinPath).remove_leaf().string();
    mTempPath = path(mRootPath + "/" + std::string(PROJECT_TEMP_DIR)).string();
    mLogPath = path(mRootPath + "/" + std::string(PROJECT_LOG_DIR)).string();
#elif PIXY_PLATFORM == PIXY_PLATFORM_APPLY
#else
#endif
    std::cout << "Binary path: " <<  mBinPath << "\n";
    std::cout << "Root path: " <<  mRootPath << "\n";
    std::cout << "Temp path: " <<  mTempPath << "\n";
    std::cout << "Log path: " <<  mLogPath << "\n";
  };
  std::string& Launcher::getRootPath() {
    return mRootPath;
  };
  std::string& Launcher::getTempPath() {
    return mTempPath;
  };
  std::string& Launcher::getBinPath() {
    return mBinPath;
  };
	void Launcher::requestShutdown() {
		fShutdown = true;
    if (mRenderer)
      mRenderer->cleanup();

	}

	void Launcher::initLogger() {
    using boost::filesystem::path;
    // TODO: fix other OSes paths
		std::string lLogPath = mLogPath;
#if PIXY_PLATFORM == PIXY_PLATFORM_WINDOWS
		lLogPath = path(mLogPath + "/" + "Karazeh.log").string();
#elif PIXY_PLATFORM == PIXY_PLATFORM_APPLE
		lLogPath = macBundlePath() + "/Contents/Logs/Launcher.log";
#else
		lLogPath = path(mLogPath + "/" + "Karazeh.log").string();
#endif
		std::cout << "Karazeh: initting log4cpp logger @ " << lLogPath << "!\n";

		log4cpp::Appender* lApp = new
		log4cpp::FileAppender("FileAppender", lLogPath, false);

    log4cpp::Layout* lLayout = new PixyLogLayout();
		/* used for header logging */
		PixyLogLayout* lHeaderLayout = new PixyLogLayout();
		lHeaderLayout->setVanilla(true);

		lApp->setLayout(lHeaderLayout);

		std::string lCatName = PIXY_LOG_CATEGORY;
		log4cpp::Category* lCat = &log4cpp::Category::getInstance(lCatName);

    lCat->setAdditivity(false);
		lCat->setAppender(lApp);
		lCat->setPriority(log4cpp::Priority::DEBUG);

		lCat->infoStream()
		<< "\n+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+";
		lCat->infoStream()
		<< "\n+                                 " << PIXY_APP_NAME << "                                    +";
		lCat->infoStream()
		<< "\n+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+\n";

		lApp->setLayout(lLayout);

		lApp = 0;
		lCat = 0;
		lLayout = 0;
		lHeaderLayout = 0;

		mLog = new log4cpp::FixedContextCategory(PIXY_LOG_CATEGORY, "Launcher");
	}

	void Launcher::launchExternalApp(std::string inPath, std::string inAppName) {
#if PIXY_PLATFORM == PIXY_PLATFORM_WIN32
    ShellExecute(inPath);
#else
    execl(inPath.c_str(), inAppName.c_str(), "Qt", NULL);
#endif
	}

  Renderer* Launcher::getRenderer() {
    return mRenderer;
  }

  void Launcher::updateApplication() {
#ifdef KARAZEH_USE_QT
    mProc.start();
#else
    boost::thread mWorker(boost::ref(mProc));
#endif
  }
} // end of namespace Pixy
