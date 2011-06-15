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

/* registered renderers */
#include "Renderers/Vanilla/VanillaRenderer.h"
#ifdef KARAZEH_RENDERER_OGRE
#include "Renderers/Ogre/OgreRenderer.h"
#endif
#ifdef KARAZEH_RENDERER_QT
#include "Renderers/Qt/QtRenderer.h"
#endif
#ifdef KARAZEH_RENDERER_COCOA
#include "Renderers/Cocoa/CocoaRenderer.h"
#endif
#ifdef KARAZEH_RENDERER_GTK3
#include "Renderers/GTK3/GTK3Renderer.h"
#endif

#if PIXY_PLATFORM == PIXY_PLATFORM_WIN32
#include <windows.h>
#include <tchar.h>
#include <process.h>
#endif

namespace Pixy
{
	Launcher* Launcher::__instance = 0;

  void handle_interrupt(int param)
  {
    printf("Signal %d received, shutdown is forced; attempting to cleanup. Please see the log.\n", param);
    Launcher::getSingleton().requestShutdown();
  }

	Launcher::Launcher() :
	mRenderer(0),
  mVWorker(0),
  mPWorker(0) {
	  //signal(SIGINT, handle_interrupt);
	  //signal(SIGTERM, handle_interrupt);

    fShutdown = false;
    fLaunching = false;
	}

	Launcher::~Launcher() {




	}
  void Launcher::shutdown() {
    if (fShutdown)
      return;

    //delete __instance;
    if (mRenderer)
      delete mRenderer;

    delete Downloader::getSingletonPtr();
    delete Patcher::getSingletonPtr();

    if (mVWorker)
      delete mVWorker;
    if (mPWorker)
      delete mPWorker;
    mVWorker = mPWorker = 0;

		mLog->infoStream() << "++++++ " << PIXY_APP_NAME << " cleaned up successfully ++++++";
		if (mLog)
		  delete mLog;

		log4cpp::Category::shutdown();
    mRenderer = NULL;

    fShutdown = true;

    if (fLaunching)
      launchExternalApp();
  }
	Launcher* Launcher::getSingletonPtr() {
		if( !__instance ) {
		    __instance = new Launcher();
		}

		return __instance;
	}

	Launcher& Launcher::getSingleton() {
		return *getSingletonPtr();
	}

	void Launcher::go(int argc, char** argv) {

    this->resolvePaths();

		this->initLogger();

    mLog->debugStream() << "Binary path: " <<  mBinPath;
    mLog->debugStream() << "Root path: " <<  mRootPath;
    mLog->debugStream() << "Temp path: " <<  mTempPath;
    mLog->debugStream() << "Log path: " <<  mLogPath;

		Patcher::getSingletonPtr();
		Downloader::getSingletonPtr();

    this->initRenderer(argc, argv);

    // V0.2: Launcher is no longer responsible for the Patcher, Renderers
    // have to explicitly start both the validation and the patching processes
    //this->updateApplication();

		// main loop
    return mRenderer->go(argc,argv);
	}

  void Launcher::resolvePaths() {
    using boost::filesystem::path;
    bool dontOverride = false;

    // locate the binary and build its path
#if PIXY_PLATFORM == PIXY_PLATFORM_LINUX
    std::cout << "Platform: Linux\n";
    // use binreloc and boost::filesystem to build up our paths
    int brres = br_init(0);
    if (brres == 0) {
      std::cerr << "binreloc could not be initialised\n";
    }
    char *p = br_find_exe_dir(".");
    mBinPath = std::string(p);
    free(p);
    mBinPath = path(mBinPath).make_preferred().string();
#elif PIXY_PLATFORM == PIXY_PLATFORM_APPLE
    // use NSBundlePath() to build up our paths
    mBinPath = path(Utility::macBundlePath() + "/Contents/MacOS").make_preferred().string();
    // create the folders if they doesn't exist
    boost::filesystem::create_directory(path(mBinPath + "/../Resources").make_preferred());
    boost::filesystem::create_directory(path(mBinPath + "/../Resources/Log").make_preferred());
    boost::filesystem::create_directory(path(mBinPath + "/../Resources/Temp").make_preferred());

    mLogPath = (path(mBinPath).remove_leaf() / path("/Resources/Log").make_preferred()).string();
    mTempPath = (path(mBinPath).remove_leaf() / path("/Resources/Temp").make_preferred()).string();
    dontOverride = true;
#else
    // use GetModuleFileName() and boost::filesystem to build up our paths on Windows
    TCHAR szPath[MAX_PATH];

    if( !GetModuleFileName( NULL, szPath, MAX_PATH ) )
    {
        printf("Cannot install service (%d)\n", GetLastError());
        return;
    }

    mBinPath = std::string(szPath);
    mBinPath = path(mBinPath).remove_filename().make_preferred().string();
#endif

    // root is PIXY_DISTANCE_FROM_ROOT directories up from the binary's
    path lRoot = path(mBinPath);
    for (int i=0; i < PIXY_DISTANCE_FROM_ROOT; ++i) {
      lRoot = lRoot.remove_leaf();
    }

    mRootPath = lRoot.make_preferred().string();
    if (!dontOverride) {
      mTempPath = (path(mRootPath) / path(PROJECT_TEMP_DIR)).make_preferred().string();
      mLogPath = (path(mRootPath) / path(PROJECT_LOG_DIR)).make_preferred().string();
    }

#ifdef DEBUG
    std::cout << "Binary path: " <<  mBinPath << "\n";
    std::cout << "Root path: " <<  mRootPath << "\n";
    std::cout << "Temp path: " <<  mTempPath << "\n";
    std::cout << "Log path: " <<  mLogPath << "\n";
#endif

  };

  void Launcher::initRenderer(int argc, char** argv) {

#ifdef KARAZEH_RENDERER_COCOA
    if ("Cocoa" == KARAZEH_DEFAULT_RENDERER || (argc > 1 && strcmp(argv[1], "Cocoa") == 0))
      if (!mRenderer)
        mRenderer = new CocoaRenderer();
#endif
#ifdef KARAZEH_RENDERER_OGRE
	  if ("Ogre" == KARAZEH_DEFAULT_RENDERER || (argc > 1 && strcmp(argv[1], "Ogre") == 0))
      if (!mRenderer)
        mRenderer = new OgreRenderer();
#endif
#ifdef KARAZEH_RENDERER_QT
    if ("Qt" == KARAZEH_DEFAULT_RENDERER || (argc > 1 && strcmp(argv[1], "Qt") == 0))
      if (!mRenderer)
        mRenderer = new QtRenderer();
#endif
#ifdef KARAZEH_RENDERER_GTK3
    if ("GTK3" == KARAZEH_DEFAULT_RENDERER || (argc > 1 && strcmp(argv[1], "GTK3") == 0))
      if (!mRenderer)
        mRenderer = new GTK3Renderer();
#endif
    if (!mRenderer) {
      if (argc > 1)
        mLog->errorStream() << "unknown renderer specified! going vanilla";
      mRenderer = new VanillaRenderer();
    }

    bool res = mRenderer->setup(argc, argv);

    if (!res) {
      mLog->errorStream() << "could not initialise renderer!";
      return;
    }

  }
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
    if (mRenderer)
      mRenderer->cleanup();

	}

	void Launcher::initLogger() {
    using boost::filesystem::path;
    using boost::filesystem::exists;
    using boost::filesystem::create_directory;
    using boost::filesystem::is_directory;

    // TODO: fix other OSes paths
		std::string lLogPath = mLogPath;
#if PIXY_PLATFORM == PIXY_PLATFORM_WINDOWS
		lLogPath = path(mLogPath + "/Karazeh.log").string();
#elif PIXY_PLATFORM == PIXY_PLATFORM_APPLE
		lLogPath = path(mLogPath + "/Karazeh.log").make_preferred().string();
#else
		lLogPath = path(mLogPath + "/Karazeh.log").make_preferred().string();
#endif

    if (!is_directory(path(mLogPath).root_directory())) {
      create_directory(path(mLogPath).root_directory());
      std::cout << "creating log dir @ " << path(mLogPath).root_directory() << "\n";
    }
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

	void Launcher::launchExternalApp() {
    if (!fLaunching) {
      fLaunching = true;
      mLog->infoStream() << "preparing to launch";
      shutdown();
    }

    using boost::filesystem::path;
    std::string lPath = path(mBinPath + "/" + std::string(PIXY_EXTERNAL_APP_PATH)).make_preferred().string();

    std::cout << "launching external app @ " << lPath;

#if PIXY_PLATFORM == PIXY_PLATFORM_WIN32
    _execl(lPath.c_str(), PIXY_EXTERNAL_APP_NAME, PIXY_EXTERNAL_APP_ARG, NULL);
#else
    // to pass more arguments to the app, you need to change this line to reflect it
    execl(lPath.c_str(), PIXY_EXTERNAL_APP_NAME, PIXY_EXTERNAL_APP_ARG, NULL);
#endif
	}

  Renderer* Launcher::getRenderer() {
    return mRenderer;
  }

  void Launcher::startValidation() {
    if (mVWorker) {
      std::cerr << "Launcher: already validated, duplicate request, discarding...\n";
      return;
    } else if (mPWorker) {
      std::cerr << "Launcher: Patcher is updating or has completed updating, don't call validate. Aborting\n";
      return;
    }

    mVWorker = new Thread<Patcher>(Patcher::getSingleton());
  }

  void Launcher::startPatching() {
    if (mPWorker) {
      std::cerr << "Launcher: looks like Patcher is already updating, aborting request for patching\n";
      return;
    }

    mPWorker = new Thread<Patcher>(Patcher::getSingleton());
  }
} // end of namespace Pixy
