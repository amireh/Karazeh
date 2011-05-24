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
#include "Renderers/Ogre/OgreRenderer.h"

namespace Pixy
{
	Launcher* Launcher::mLauncher;

  void handle_interrupt(int param)
  {
    printf("Something wrong happened, attempting to cleanup: %d\n", param);
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

    delete Downloader::getSingletonPtr();
    delete Patcher::getSingletonPtr();
        
		if( mInputMgr )
		    delete mInputMgr;

    if (mRenderer)
      delete mRenderer;
    		
		mLog->infoStream() << "++++++ " << PIXY_APP_NAME << " cleaned up successfully ++++++";
		if (mLog)
		  delete mLog;
		  
		log4cpp::Category::shutdown();
		mRenderer = NULL; mInputMgr = NULL;
	}



  
	void Launcher::go(bool withRenderer) {
	
		// init logger
		initLogger();
		
		goFunc = &Launcher::goVanilla;
		if (withRenderer) {
		  goFunc = &Launcher::goWithRenderer;
		  
	    mRenderer = new OgreRenderer();
	    bool res = mRenderer->setup();
	    
	    if (!res) {
	      mLog->errorStream() << "could not initialise renderer!";
	      return;
	    }
		
		  int width, height = 0;
		  size_t windowHnd = 0;
		  mRenderer->getWindowHandle(&windowHnd);
		  mRenderer->getWindowExtents(&width, &height);
		
		  // Setup input & register as listener
		  mInputMgr = InputManager::getSingletonPtr();
		  mInputMgr->initialise( windowHnd, width, height );

		  mInputMgr->addKeyListener( mRenderer, mRenderer->getName() );
		  mInputMgr->addMouseListener( mRenderer, mRenderer->getName() );
		}
		
		mDownloader = Downloader::getSingletonPtr();
		boost::thread mThread(launchDownloader);
		
		// lTimeLastFrame remembers the last time that it was checked
		// we use it to calculate the time since last frame
		
		lTimeLastFrame = 0;
		lTimeCurrentFrame = 0;
		lTimeSinceLastFrame = 0;

    //mRoot->getTimer()->reset();
    
		// main game loop
		while( !fShutdown )
			(this->*goFunc)();
		
	}
	
	void Launcher::goWithRenderer() {
    // calculate time since last frame and remember current time for next frame
    /*lTimeCurrentFrame = mRoot->getTimer()->getMilliseconds();
    lTimeSinceLastFrame = lTimeCurrentFrame - lTimeLastFrame;
    lTimeLastFrame = lTimeCurrentFrame;*/
	
    // update input manager
    mInputMgr->capture();
    
    mRenderer->update(0);	
	};
	
	void Launcher::goVanilla() {
	  // nothing to do here really
	};
	
	void Launcher::launchDownloader() {
	  try { 
	    Patcher::getSingleton().validateVersion();
	  } catch (BadVersion* e) {
	    //mLog->errorStream() << "version mismatch!";
	    std::cout << "version mismatch!";
	    delete e;
	  }
	}
	
	void Launcher::requestShutdown() {
		fShutdown = true;
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
		<< "\n+                           " << PIXY_APP_NAME << " " << PIXY_APP_VERSION << "                                    +";
		lCat->infoStream() 
		<< "\n+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+\n";
		
		lApp->setLayout(lLayout);
		
		lApp = 0;
		lCat = 0;
		lLayout = 0;
		lHeaderLayout = 0;
		
		mLog = new log4cpp::FixedContextCategory(PIXY_LOG_CATEGORY, "Launcher");
	}
	
	const std::string Launcher::getVersion() {
	  return std::string(PIXY_APP_VERSION);
	};
	
	
	void Launcher::evtValidateStarted() {
	  std::cout << "Validating version...\n";
	}
	void Launcher::evtValidateComplete(bool needsUpdate) {
	  if (needsUpdate) {
	    std::cout << "Application needs updating\n";
	    Patcher::getSingleton().doPatch(NULL);
	  }
	  else
	    std::cout << "Application is up to date\n";
	}
	void Launcher::evtFetchStarted() {
	  std::cout << "Downloading patch...\n";
	}
	void Launcher::evtFetchComplete(bool success) {
	  std::cout << "Patch is downloaded\n";
	}
	void Launcher::evtPatchStarted() {
	  std::cout << "Patching application...\n";
	}
	void Launcher::evtPatchComplete(bool success) {
	  std::cout << "Patching is complete\n";
	}
} // end of namespace Pixy
