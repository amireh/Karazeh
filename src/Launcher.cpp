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

using namespace Ogre;
namespace Pixy
{
	Launcher* Launcher::mLauncher;
	
	Launcher::Launcher() :
	mRoot(NULL),
	mInputMgr(NULL),
	fShutdown(false) {
	}
	
	Launcher::~Launcher() {

    delete mDownloader;
    
		if( mInputMgr )
		    delete mInputMgr;
		
		if( mRoot )
		    delete mRoot;
		
		mLog->infoStream() << "++++++ Elementum cleaned up successfully ++++++";
		if (mLog)
		  delete mLog;
		  
		log4cpp::Category::shutdown();
		mRoot = NULL; mInputMgr = NULL;
	}

  void Launcher::loadRenderSystems() 
  {
    bool rendererInstalled = false;
    // try using Direct3D on Windows
#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32 
    try { 
      mRoot->loadPlugin("RenderSystem_Direct3D9"); 
      mRoot->setRenderSystem(mRoot->getRenderSystemByName("Direct3D9 Rendering Subsystem"));
      rendererInstalled = true;
    } catch (Ogre::Exception& e) {
      mLog->errorStream() << "Unable to create D3D9 RenderSystem: " << e.getFullDescription();
    } catch(std::exception& e) { 
      mLog->errorStream() << "Unable to create D3D9 RenderSystem: " << e.what(); 
    }
#endif

    std::string lPluginsPath;
#if OGRE_PLATFORM == OGRE_PLATFORM_APPLE
    lPluginsPath = macBundlePath() + "/Contents/Plugins/";
#elif OGRE_PLATFORM == OGRE_PLATFORM_WIN32
    lPluginsPath = ".\\";
#else
    lPluginsPath = "./";
#endif
    // try using OpenGL
    try {
      mRoot->loadPlugin(lPluginsPath + "RenderSystem_GL");
      if (!rendererInstalled) {
        mRoot->setRenderSystem(mRoot->getRenderSystemByName("OpenGL Rendering Subsystem"));
        rendererInstalled = true;
      }
    } 
    catch(Ogre::Exception& e) { 
      mLog->errorStream() << "Unable to create OpenGL RenderSystem: " << e.getFullDescription(); 
    } 

  }
  
	void Launcher::go() {
	
		// init logger
		initLogger();
		
		// setup paths
	  using std::ostringstream;
	  ostringstream lPathResources, lPathPlugins, lPathCfg, lPathOgreCfg, lPathLog;
#if OGRE_PLATFORM == OGRE_PLATFORM_WINDOWS
	  lPathResources << PROJECT_ROOT << PROJECT_RESOURCES << "\\config\\resources_win32.cfg";
    lPathPlugins << "..\\resources\\config\\plugins.cfg";
    lPathCfg << "..\\resources\\config\\";
	  lPathOgreCfg << lPathCfg.str() << "ogre.cfg";	
	  lPathLog << PROJECT_LOG_DIR << "\\Ogre.log";	
#elif OGRE_PLATFORM == OGRE_PLATFORM_APPLE
	  lPathResources << macBundlePath() << "/Contents/Resources/config/resources_osx.cfg";
	  lPathPlugins << macBundlePath() << "/Contents/Resources/config/plugins.cfg";
	  lPathCfg << macBundlePath() << "/Contents/Resources/config/";
	  lPathOgreCfg << lPathCfg.str() << "ogre.cfg";	
	  lPathLog << macBundlePath() << "/Contents/Logs/Ogre.log";    
#else
	  lPathResources << PROJECT_ROOT << PROJECT_RESOURCES << "/config/resources_linux.cfg";
    lPathPlugins << PROJECT_ROOT << PROJECT_RESOURCES << "/config/plugins.cfg";
    lPathCfg << PROJECT_ROOT << PROJECT_RESOURCES << "/config/";
	  lPathOgreCfg << lPathCfg.str() << "ogre.cfg";	
	  lPathLog << PROJECT_LOG_DIR << "/Ogre.log";
#endif
    
    mConfigPath = lPathCfg.str();
    
		mRoot = OGRE_NEW Root(lPathPlugins.str(), lPathOgreCfg.str(), lPathLog.str());
		if (!mRoot) {
			throw Ogre::Exception( Ogre::Exception::ERR_INTERNAL_ERROR, 
								  "Error - Couldn't initalize OGRE!", 
								  std::string(PIXY_APP_NAME) + " - Error");
		}
		loadRenderSystems();
	  
		// Setup and configure game
		if( !this->configureGame() ) {
		    // If game can't be configured, throw exception and quit application
		    throw Ogre::Exception( Ogre::Exception::ERR_INTERNAL_ERROR,
								  "Error - Couldn't Configure Renderwindow",
								  std::string(PIXY_APP_NAME) + " - Error" );
		    return;
		}

	  // Setup input & register as listener
		mInputMgr = InputManager::getSingletonPtr();
		mRenderWindow = mRoot->getAutoCreatedWindow();
		mInputMgr->initialise( mRenderWindow );
		WindowEventUtilities::addWindowEventListener( mRenderWindow, this );
				
		this->setupResources(lPathResources.str());
		
		mInputMgr->addKeyListener( this, "Launcher" );
		mInputMgr->addMouseListener( this, "Launcher" );
		
		// Change to first state
		//this->changeState( Intro::getSingletonPtr() );
		
		mDownloader = Downloader::getSingletonPtr();
		boost::thread mThread(launchDownloader);
		
		// lTimeLastFrame remembers the last time that it was checked
		// we use it to calculate the time since last frame
		
		lTimeLastFrame = 0;
		lTimeCurrentFrame = 0;
		lTimeSinceLastFrame = 0;

    mRoot->getTimer()->reset();
    
		// main game loop
		while( !fShutdown ) {
			
	    // calculate time since last frame and remember current time for next frame
	    lTimeCurrentFrame = mRoot->getTimer()->getMilliseconds();
	    lTimeSinceLastFrame = lTimeCurrentFrame - lTimeLastFrame;
	    lTimeLastFrame = lTimeCurrentFrame;
		
	    // update input manager
	    mInputMgr->capture();
		
	    // cpdate current state
	    //mStates.back()->update( lTimeSinceLastFrame );

			WindowEventUtilities::messagePump();

			// render next frame
		  mRoot->renderOneFrame();
		}
		
	}
	
	void Launcher::launchDownloader() {
	  try { 
	    Patcher::getSingleton().validateVersion();
	  } catch (BadVersion* e) {
	    //mLog->errorStream() << "version mismatch!";
	    std::cout << "version mismatch!";
	    delete e;
	  }
	}
	
	bool Launcher::configureGame() {
		
		mLog->infoStream() << "configuring video settings";
		// Load config settings from ogre.cfg
		//if( !mRoot->restoreConfig() ) {
		  Ogre::RenderSystem* mRS = mRoot->getRenderSystem();
		  mRS->setConfigOption("Full Screen", "No");
		  mRS->setConfigOption("Video Mode", "640 x 480");
		    // If there is no config file, show the configuration dialog
		    /*if( !mRoot->showConfigDialog() ) {
		        return false;
		    }*/
		//}
		
		// Initialise and create a default rendering window
		mRenderWindow = mRoot->initialise( true, PIXY_APP_NAME );
		
		// Initialise resources
		//ResourceGroupManager::getSingleton().initialiseAllResourceGroups();
		
		// Create needed scenemanagers
		Ogre::SceneManager *mSceneMgr = mRoot->createSceneManager( "DefaultSceneManager", "LauncherScene" );
		Ogre::Camera *mCamera = mSceneMgr->createCamera("LauncherCamera");
		mRenderWindow->addViewport(mCamera, -1);
		
		return true;
	}
	
	void Launcher::setupResources(std::string inPath) {

		// Load resource paths from config file
		ConfigFile cf;
		cf.load( inPath );
		
		// Go through all settings in the file
		ConfigFile::SectionIterator itSection = cf.getSectionIterator();
		
		String sSection, sType, sArch;
		while( itSection.hasMoreElements() ) {
		    sSection = itSection.peekNextKey();
			
		    ConfigFile::SettingsMultiMap *mapSettings = itSection.getNext();
		    ConfigFile::SettingsMultiMap::iterator itSetting = mapSettings->begin();
		    while( itSetting != mapSettings->end() ) {
		        sType = itSetting->first;
		        sArch = itSetting->second;
#if OGRE_PLATFORM == OGRE_PLATFORM_APPLE
				ResourceGroupManager::getSingleton().addResourceLocation( String(macBundlePath() + "/" + sArch), sType, sSection);
#else
				ResourceGroupManager::getSingleton().addResourceLocation( sArch, sType, sSection);
#endif
				
		        ++itSetting;
		    }
		}
		ResourceGroupManager::getSingleton().initialiseAllResourceGroups();
	}
	
	void Launcher::requestShutdown() {
		fShutdown = true;
	}
	
	bool Launcher::keyPressed( const OIS::KeyEvent &e ) {
		// Call keyPressed of current state
		//mStates.back()->keyPressed( e );
		
		return true;
	}
	
	bool Launcher::keyReleased( const OIS::KeyEvent &e ) {
		// Call keyReleased of current state
		//mStates.back()->keyReleased( e );
		if (e.key == OIS::KC_ESCAPE)
		  this->requestShutdown();
		  
		return true;
	}
	
	bool Launcher::mouseMoved( const OIS::MouseEvent &e ) {
		// Call mouseMoved of current state
		//mStates.back()->mouseMoved( e );
		
		return true;
	}
	
	bool Launcher::mousePressed( const OIS::MouseEvent &e, OIS::MouseButtonID id ) {
		// Call mousePressed of current state
		//mStates.back()->mousePressed( e, id );
		
		return true;
	}
	
	bool Launcher::mouseReleased( const OIS::MouseEvent &e, OIS::MouseButtonID id ) {
		// Call mouseReleased of current state
		//mStates.back()->mouseReleased( e, id );
		
		return true;
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
#if OGRE_PLATFORM == OGRE_PLATFORM_WINDOWS
		lLogPath += "\\Launcher.log";
#elif OGRE_PLATFORM == OGRE_PLATFORM_APPLE
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
		<< "\n+                           Launcher v" << PIXY_APP_VERSION << "                                    +";
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
