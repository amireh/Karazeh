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
	mInputMgr(0),
	fShutdown(false) {
	  signal(SIGINT, handle_interrupt);
	  signal(SIGTERM, handle_interrupt);
	  signal(SIGKILL, handle_interrupt);
	  
	  goFunc = &Launcher::goVanilla;
	}
	
	Launcher::~Launcher() {

		if( mInputMgr )
		    delete mInputMgr;

    if (mRenderer)
      delete mRenderer;

    delete Downloader::getSingletonPtr();
    delete Patcher::getSingletonPtr();
        
    EventManager::shutdown();
    		
		mLog->infoStream() << "++++++ " << PIXY_APP_NAME << " cleaned up successfully ++++++";
		if (mLog)
		  delete mLog;
		  
		log4cpp::Category::shutdown();
		
		mRenderer = NULL; mInputMgr = NULL;
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
  
	void Launcher::go(const char* inRendererName) {
	
		// init logger
		initLogger();
		
		EventManager::getSingletonPtr();
		Patcher::getSingletonPtr();
		Downloader::getSingletonPtr();		
		
		goFunc = &Launcher::goVanilla;
		bool validRenderer = false;
		if (inRendererName != 0) {
		  validRenderer = true;
#ifdef KARAZEH_RENDERER_OGRE		  
		  if (strcmp(inRendererName, "Ogre") == 0)
		    mRenderer = new OgreRenderer();
#elif defined(KARAZEH_RENDERER_Qt)		    
		  if (strcmp(inRendererName, "Qt") == 0)
		    mRenderer = new QtRenderer();
#endif

		  if (!mRenderer) {
		    mLog->errorStream() << "Invalid renderer! " << inRendererName << ", going vanilla";
		    validRenderer = false;
		  }
		}
		
		if (validRenderer) {
		  goFunc = &Launcher::goWithRenderer;
		  
	    bool res = mRenderer->setup();
	    
	    if (!res) {
	      mLog->errorStream() << "could not initialise renderer!";
	      return;
	    }
		
		  int width = 0;
		  int height = 0;
		  size_t windowHnd = 0;
		  mRenderer->getWindowHandle(&windowHnd);
		  mRenderer->getWindowExtents(&width, &height);
		
		  // Setup input & register as listener
		  mInputMgr = InputManager::getSingletonPtr();
		  mInputMgr->initialise( windowHnd, width, height );

		  mInputMgr->addKeyListener( mRenderer, mRenderer->getName() );
		  mInputMgr->addMouseListener( mRenderer, mRenderer->getName() );
		  
		  mRenderer->deferredSetup();
		}
		

		
		
		// lTimeLastFrame remembers the last time that it was checked
		// we use it to calculate the time since last frame
		lTimeLastFrame = boost::posix_time::microsec_clock::universal_time();
		lTimeCurrentFrame = boost::posix_time::microsec_clock::universal_time();

    mLog->infoStream() << "my current thread id: " << boost::this_thread::get_id();
    
    boost::thread mThread(boost::ref(Patcher::getSingleton()));
    
		// main game loop
		while( !fShutdown )
			(this->*goFunc)();
		
	}
	
	void Launcher::goWithRenderer() {
    EventManager::getSingleton().update();
    
    lTimeCurrentFrame = boost::posix_time::microsec_clock::universal_time();
    lTimeSinceLastFrame = lTimeCurrentFrame - lTimeLastFrame;
    lTimeLastFrame = lTimeCurrentFrame;
	
    // update input manager
    mInputMgr->capture();
    
    Patcher::getSingleton().update();
    mRenderer->update(lTimeSinceLastFrame.total_milliseconds());	
    
	};
	
	void Launcher::goVanilla() {
	  // nothing to do here really
	  EventManager::getSingleton().update();
	  Patcher::getSingleton().update();
	};

	void Launcher::requestShutdown() {
		fShutdown = true;
	}
	

	
	
	void Launcher::initLogger() {

		std::string lLogPath = PROJECT_LOG_DIR;
#if PIXY_PLATFORM == PIXY_PLATFORM_WINDOWS
		lLogPath += "\\Launcher.log";
#elif PIXY_PLATFORM == PIXY_PLATFORM_APPLE
		lLogPath = macBundlePath() + "/Contents/Logs/Launcher.log";
#else
		lLogPath += "/Launcher.log";		
#endif	
		std::cout << "| Initting log4cpp logger @ " << lLogPath << "!\n";
		
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
    execl(inPath.c_str(), inAppName.c_str(), "Ogre", NULL);
    
    //system(inPath.c_str());
    //exit(0);
#endif
	}
} // end of namespace Pixy
