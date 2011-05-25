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
 
#include "Renderers/CEGUI/CEGUIRenderer.h"
#include "Launcher.h"

namespace Pixy {
	
	CEGUIRenderer::CEGUIRenderer() {
	  mLog = new log4cpp::FixedContextCategory(PIXY_LOG_CATEGORY, "CEGUIRenderer");
		mLog->infoStream() << "firing up";
		
		mName = "CEGUIRenderer";
  }
	
	CEGUIRenderer::~CEGUIRenderer() {
		
		mLog->infoStream() << "shutting down";
		
		if (fSetup)
		  cleanup();
		
		CEGUI::OgreRenderer::destroySystem();
		
		if (mLog)
		  delete mLog;
	}
	  
	void CEGUIRenderer::getWindowHandle(size_t *windowHnd) {
	  //mRenderWindow->getCustomAttribute( "WINDOW", windowHnd );
	};
	
	void CEGUIRenderer::getWindowExtents(int *width, int *height) {
	  *width = 640;
	  *height = 480;
	};
			  
	bool CEGUIRenderer::setup() {
    if (fSetup)
      return true;

    // fake args for glutInit
    int argc = 1;
    const char* argv = PIXY_APP_NAME;
    
	  CEGUI::OgreRenderer::bootstrapSystem();
	  mSystem = &CEGUI::System::getSingleton();
		//this->setupResources(lPathResources.str());
		
		fSetup = true;
		
	  return true;
	};
	
	bool CEGUIRenderer::deferredSetup() {

	};
	
	bool CEGUIRenderer::cleanup() {

	};
	
	void CEGUIRenderer::update(unsigned long lTimeElapsed) {
    //mSystem->renderGUI();
	}
	
	void CEGUIRenderer::injectError(PATCHERROR errorCode, std::string inMsg) {
	}
  void CEGUIRenderer::injectNotice(PATCHNOTICE noticeCode, std::string inMsg) {
  }
  
  bool CEGUIRenderer::injectPrompt(std::string inMsg) {
  }
  void CEGUIRenderer::injectStatus(std::string inMsg) {
  
  }
  void CEGUIRenderer::injectProgress(int progress) {
  
  }
	bool CEGUIRenderer::keyPressed( const OIS::KeyEvent &e ) {
		return true;
	}
	
	bool CEGUIRenderer::keyReleased( const OIS::KeyEvent &e ) {
		if (e.key == OIS::KC_ESCAPE)
		  Launcher::getSingleton().requestShutdown();
		  
		return true;
	}
	
	bool CEGUIRenderer::mouseMoved( const OIS::MouseEvent &e ) {

		
		return true;
	}
	
	bool CEGUIRenderer::mousePressed( const OIS::MouseEvent &e, OIS::MouseButtonID id ) {

		
		return true;
	}
	
	bool CEGUIRenderer::mouseReleased( const OIS::MouseEvent &e, OIS::MouseButtonID id ) {

		
		return true;
	}
};
