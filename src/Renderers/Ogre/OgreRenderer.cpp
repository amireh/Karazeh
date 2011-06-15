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

#include "Renderers/Ogre/OgreRenderer.h"
#include "Launcher.h"
#include "Patcher.h"

using namespace Ogre;
using namespace OgreBites;
namespace Pixy {

	OgreRenderer::OgreRenderer() {
	  mLog = new log4cpp::FixedContextCategory(PIXY_LOG_CATEGORY, "OgreRenderer");
		mLog->infoStream() << "firing up";

		mName = "OgreRenderer";
    fShutdown = false;
  }

	OgreRenderer::~OgreRenderer() {

		mLog->infoStream() << "shutting down";

		if (fSetup)
		  cleanup();

		if( mInputMgr )
		    delete mInputMgr;

		if( mRoot )
		    delete mRoot;

		if (mLog)
		  delete mLog;

    mLog = 0;
    mRoot = 0;
    mInputMgr = 0;

	}

  void OgreRenderer::loadRenderSystems()
  {
    bool rendererInstalled = false;
    // try using Direct3D on Windows
#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
    /*try {
      mRoot->loadPlugin("RenderSystem_Direct3D9");
      mRoot->setRenderSystem(mRoot->getRenderSystemByName("Direct3D9 Rendering Subsystem"));
      rendererInstalled = true;
    } catch (Ogre::Exception& e) {
      mLog->errorStream() << "Unable to create D3D9 RenderSystem: " << e.getFullDescription();
    } catch(std::exception& e) {
      mLog->errorStream() << "Unable to create D3D9 RenderSystem: " << e.what();
    }*/
#endif

    std::string lPluginsPath;
#if OGRE_PLATFORM == OGRE_PLATFORM_APPLE
    lPluginsPath = macBundlePath() + "/Contents/Plugins/";
#elif OGRE_PLATFORM == OGRE_PLATFORM_WIN32
    lPluginsPath = ".\\";
#else
    lPluginsPath = boost::filesystem::path(Launcher::getSingleton().getBinPath() + "/").string();
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

	bool OgreRenderer::configureGame() {

		mLog->infoStream() << "configuring video settings";

    Ogre::RenderSystem* mRS = mRoot->getRenderSystem();
    mRS->setConfigOption("Full Screen", "No");
    mRS->setConfigOption("Video Mode", "640 x 480");

		// Initialise and create a default rendering window
		mRenderWindow = mRoot->initialise( true, PIXY_APP_NAME );

		// Create needed scenemanagers
		Ogre::SceneManager *mSceneMgr = mRoot->createSceneManager( "DefaultSceneManager", "KarazehScene" );
		Ogre::Camera *mCamera = mSceneMgr->createCamera("KarazehCamera");
		mViewport = mRenderWindow->addViewport(mCamera);
		mCamera->setAspectRatio((Ogre::Real)mViewport->getActualWidth() / (Ogre::Real)mViewport->getActualHeight());
		mCamera = 0; mSceneMgr = 0;

		return true;
	}

	void OgreRenderer::setupResources(std::string inPath) {

		// Load resource paths from config file
		ConfigFile cf;
		cf.load( inPath );

		// Go through all settings in the file
		ConfigFile::SectionIterator itSection = cf.getSectionIterator();

    String lRoot = Launcher::getSingleton().getRootPath();
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
			  ResourceGroupManager::getSingleton().addResourceLocation( String(lRoot + "/" + sArch), sType, sSection);
#endif

        ++itSetting;
	    }
		}
		ResourceGroupManager::getSingleton().initialiseAllResourceGroups();
	}

	void OgreRenderer::getWindowHandle(size_t *windowHnd) {
	  mRenderWindow->getCustomAttribute( "WINDOW", windowHnd );
	};

	void OgreRenderer::getWindowExtents(int *width, int *height) {
	  (*width) = 640;
	  (*height) = 480;
	};

	bool OgreRenderer::setup(int argc, char** argv) {
    if (fSetup)
      return true;

    mEvtMgr = EventManager::getSingletonPtr();

    using boost::filesystem::path;

    std::string lRoot = Launcher::getSingleton().getRootPath();
    std::string lPathResources;

		// setup paths
	  //using std::ostringstream;
	  //ostringstream lPathResources, lPathPlugins, lPathCfg, lPathOgreCfg, lPathLog;
#if OGRE_PLATFORM == OGRE_PLATFORM_WINDOWS
	  lPathResources << PROJECT_ROOT << PROJECT_RESOURCES << "\\config\\resources_win32.cfg";
    lPathPlugins << "..\\resources\\config\\plugins.cfg";
    lPathCfg << "..\\resources\\config\\";
	  lPathOgreCfg << lPathCfg.str() << "ogre.cfg";
#elif OGRE_PLATFORM == OGRE_PLATFORM_APPLE
	  lPathResources << macBundlePath() << "/Contents/Resources/config/resources_osx.cfg";
	  lPathPlugins << macBundlePath() << "/Contents/Resources/config/plugins.cfg";
	  lPathCfg << macBundlePath() << "/Contents/Resources/config/";
	  lPathOgreCfg << lPathCfg.str() << "ogre.cfg";
#else
    lPathResources = path(lRoot + "/" + PROJECT_RESOURCES + "/" + PROJECT_OGRE_RESOURCES + "/config/resources_linux.cfg").string();
#endif

		mRoot = OGRE_NEW Root();
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
		    return false;
		}

		//mRenderWindow = mRoot->getAutoCreatedWindow();
		WindowEventUtilities::addWindowEventListener( mRenderWindow, this );

		mOverlayMgr = Ogre::OverlayManager::getSingletonPtr();

		this->setupResources(lPathResources);

    int width = 0;
    int height = 0;
    size_t windowHnd = 0;
    getWindowHandle(&windowHnd);
    getWindowExtents(&width, &height);

    // Setup input & register as listener
    mInputMgr = InputManager::getSingletonPtr();
    mInputMgr->initialise( windowHnd, width, height );

    mInputMgr->addKeyListener( this, this->getName() );
    mInputMgr->addMouseListener( this, this->getName() );

		mTrayMgr =
		new SdkTrayManager(
		  "TrayManager",
		  mRenderWindow,
		  InputManager::getSingletonPtr()->getMouse(), this);

		//mTrayMgr->getTrayContainer(TL_NONE)->hide();
	  //mTrayMgr->setTrayPadding(10);

	  mOverlayMgr->getByName("Karazeh/UI")->show();
	  //mTrayMgr->createLabel(TL_CENTER, "StatusLabel", "Status");
	  mStatusBox = mTrayMgr->createTextBox(TL_CENTER, "Status", "Status", 320, 160);

	  mTrayMgr->createButton(TL_CENTER, "Launch", "Launch");
	  mTrayMgr->createButton(TL_CENTER, "Patch", "Patch");
	  mProgress = mTrayMgr->createProgressBar(TL_BOTTOM, "Progess", "Progress", 480, 20);
	  mTrayMgr->adjustTrays();

    bind("GuiLoaded", this, &OgreRenderer::evtGuiLoaded);
    bind("UnableToConnect", this, &OgreRenderer::evtUnableToConnect);
    bind("ValidateStarted", this, &OgreRenderer::evtValidateStarted);
    bind("ValidateComplete", this, &OgreRenderer::evtValidateComplete);
    bind("PatchStarted", this, &OgreRenderer::evtPatchStarted);
    bind("PatchProgress", this, &OgreRenderer::evtPatchProgress);
    bind("PatchFailed", this, &OgreRenderer::evtPatchFailed);
    bind("PatchComplete", this, &OgreRenderer::evtPatchComplete);
    bind("ApplicationPatched", this, &OgreRenderer::evtApplicationPatched);

    mLog->infoStream() << "setup";

	  return true;
	};


	bool OgreRenderer::cleanup() {
	  delete mTrayMgr;
    return true;
	};

	void OgreRenderer::go(int argc, char** argv) {

		lTimeLastFrame = 0;
		lTimeCurrentFrame = 0;
		lTimeSinceLastFrame = 0;

    mRoot->getTimer()->reset();

    //mEvtMgr->hook(mEvtMgr->createEvt("GuiLoaded"));
		while( !fShutdown ) {

	    // calculate time since last frame and remember current time for next frame
	    lTimeCurrentFrame = mRoot->getTimer()->getMilliseconds();
	    lTimeSinceLastFrame = lTimeCurrentFrame - lTimeLastFrame;
	    lTimeLastFrame = lTimeCurrentFrame;

	    // update input manager
	    mInputMgr->capture();

      // update the event manager and handle events
      mEvtMgr->update();
      processEvents();

      mFrameEvt.timeSinceLastFrame = mFrameEvt.timeSinceLastEvent = lTimeSinceLastFrame;
      mTrayMgr->frameRenderingQueued(mFrameEvt);

      WindowEventUtilities::messagePump();

			// render next frame
		  mRoot->renderOneFrame();
		}
	}

	bool OgreRenderer::keyPressed( const OIS::KeyEvent &e ) {
		return true;
	}

	bool OgreRenderer::keyReleased( const OIS::KeyEvent &e ) {
		if (e.key == OIS::KC_ESCAPE)
		  fShutdown = true;

		return true;
	}

	bool OgreRenderer::mouseMoved( const OIS::MouseEvent &e ) {
		OIS::MouseState state = e.state;
    OIS::MouseEvent orientedEvt((OIS::Object*)e.device, state);
		if (mTrayMgr->injectMouseMove(e)) return true;

		return true;
	}

	bool OgreRenderer::mousePressed( const OIS::MouseEvent &e, OIS::MouseButtonID id ) {
	  OIS::MouseState state = e.state;
	  OIS::MouseEvent orientedEvt((OIS::Object*)e.device, state);
	  if (mTrayMgr->injectMouseDown(orientedEvt, id)) return true;

		return true;
	}

	bool OgreRenderer::mouseReleased( const OIS::MouseEvent &e, OIS::MouseButtonID id ) {
	  OIS::MouseState state = e.state;
	  OIS::MouseEvent orientedEvt((OIS::Object*)e.device, state);

	  if (mTrayMgr->injectMouseUp(orientedEvt, id)) return true;

		return true;
	}

	void OgreRenderer::yesNoDialogClosed(const Ogre::DisplayString& question, bool yesHit) {
	  if (yesHit) {
      Launcher::getSingleton().startPatching();
	  }
	}
  void OgreRenderer::buttonHit(OgreBites::Button* b) {
    if (b->getName() == "Launch") {
      Launcher::getSingleton().launchExternalApp();
    }

  };
  bool OgreRenderer::evtGuiLoaded(Event* inEvt) {
    Launcher::getSingleton().startValidation();
    return true;
  };
  void OgreRenderer::injectUnableToConnect( void ) {
    mEvtMgr->hook(mEvtMgr->createEvt("UnableToConnect"));
  };

	void OgreRenderer::injectValidateStarted( void ) {
    mEvtMgr->hook(mEvtMgr->createEvt("ValidateStarted"));
	}
	void OgreRenderer::injectValidateComplete(bool inNeedUpdate, Version const& inTargetVersion) {
    Event* lEvt = mEvtMgr->createEvt("ValidateComplete");
    lEvt->setProperty("NeedUpdate", inNeedUpdate ? "Yes" : "No");
    lEvt->setAny((void*)&inTargetVersion);
    mEvtMgr->hook(lEvt);
	}

	void OgreRenderer::injectPatchStarted( Version const& inTargetVersion ) {
    Event* lEvt = mEvtMgr->createEvt("PatchStarted");
    lEvt->setAny((void*)&inTargetVersion);
    mEvtMgr->hook(lEvt);
	}

  void OgreRenderer::injectPatchSize( pbigint_t inSize ) {
    mPatchSize = inSize;
  };

  void OgreRenderer::injectPatchProgress(float inPercent) {
    /*Event* lEvt = mEvtMgr->createEvt("PatchProgress");
    lEvt->setProperty("Percent", Utility::stringify(inPercent));
    mEvtMgr->hook(lEvt);*/
  }


	void OgreRenderer::injectPatchFailed(std::string inMsg, Version const& inTargetVersion) {
    Event* lEvt = mEvtMgr->createEvt("PatchFailed");
    lEvt->setProperty("Msg", inMsg);
    lEvt->setAny((void*)&inTargetVersion);
    mEvtMgr->hook(lEvt);
	}

	void OgreRenderer::injectPatchComplete(Version const& inCurrentVersion) {
    Event* lEvt = mEvtMgr->createEvt("PatchComplete");
    lEvt->setAny((void*)&inCurrentVersion);
    mEvtMgr->hook(lEvt);
  }

  void OgreRenderer::injectApplicationPatched( Version const& inCurrentVersion ) {
    Event* lEvt = mEvtMgr->createEvt("ApplicationPatched");
    lEvt->setAny((void*)&inCurrentVersion);
    mEvtMgr->hook(lEvt);
  }



  bool OgreRenderer::evtUnableToConnect( Event* inEvt ) {
    mStatusBox->setCaption("Error");
    mStatusBox->setText("Unable to connect to patch server, please verify your internet connectivity.");

    return true;
  }
  bool OgreRenderer::evtValidateStarted( Event* inEvt ) {
	  //mLog->infoStream() << "Handling evt: " << inEvt->getName();

	  mStatusBox->setCaption("Validating...");
    mStatusBox->setText("Downloading latest patch information");
    return true;
  }
  bool OgreRenderer::evtValidateComplete( Event* inEvt ) {
    bool inNeedUpdate = inEvt->getProperty("NeedUpdate") == "Yes";
    Version inTargetVersion(*(static_cast<Version*>(inEvt->getAny())));

	  if (inNeedUpdate) {
	    mTrayMgr->showYesNoDialog("Notice", "Updates are available. Would you like to update now?");
	    mStatusBox->setText("Application needs updating. Latest version is: " + inTargetVersion.Value);
	  } else {
	    mStatusBox->setText("Application is up to date " + inTargetVersion.Value);
	  }
    return true;
  }
  bool OgreRenderer::evtPatchStarted( Event* inEvt ) {
    Version inTargetVersion = *(static_cast<Version*>(inEvt->getAny()));

	  mStatusBox->setCaption("Updating");
	  mStatusBox->setText("Updating to version " + inTargetVersion.Value);
    return true;
  }
  bool OgreRenderer::evtPatchProgress( Event* inEvt ) {
    int inPercent = Utility::convertTo<float>(inEvt->getProperty("Percent"));

    mProgress->setProgress((int)inPercent / 100.0f);
    return true;
  }
  bool OgreRenderer::evtPatchFailed( Event* inEvt ) {
    Version inTargetVersion = *(static_cast<Version*>(inEvt->getAny()));
    std::string inMsg = inEvt->getProperty("Msg");

    std::string lMsg = "There was a problem patching to version "
      + inTargetVersion.Value
      + ". Reinstalling the application is required as the current version "
      + "seems corrupt."
      + "\nCause: " + inMsg;

    //mTrayMgr->showOkDialog("Patch Error", lMsg);

    mStatusBox->setCaption("Update failed.");
    //mStatusBox->setText("Please verify your installation or re-install if this problem persists.");
    mStatusBox->setText(lMsg);
    return true;
  }
  bool OgreRenderer::evtPatchComplete( Event* inEvt ) {
    Version inCurrentVersion = *(static_cast<Version*>(inEvt->getAny()));

    std::string lMsg = "Application was successfully updated to "
      + inCurrentVersion.Value;
	  mTrayMgr->showOkDialog("Patch Successful", lMsg);

	  mStatusBox->setCaption("Updated");
	  mStatusBox->setText(lMsg);

    return true;
  }
  bool OgreRenderer::evtApplicationPatched( Event* inEvt ) {
    Version inCurrentVersion = *(static_cast<Version*>(inEvt->getAny()));

    std::string lMsg = "All updates were successful. Application is now " + inCurrentVersion.Value;
	  mTrayMgr->showOkDialog("Application up to date", lMsg);

	  mStatusBox->setCaption("Application is up to date.");
	  mStatusBox->setText(lMsg);

    return true;
  }
};
