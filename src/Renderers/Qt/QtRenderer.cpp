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
 
#include "Renderers/Qt/QtRenderer.h"
#include "Launcher.h"
#include "Patcher.h"

namespace Pixy {
	
	QtRenderer::QtRenderer() {
	  mLog = new log4cpp::FixedContextCategory(PIXY_LOG_CATEGORY, "QtRenderer");
		mLog->infoStream() << "firing up";
		
		mName = "QtRenderer";
  }
	
	QtRenderer::~QtRenderer() {
		
		mLog->infoStream() << "shutting down";
		
		/*delete mQuitButton;
		delete mYesNoDlg;
		delete mLayout;
		delete mWindow;*/
		
		if (fSetup)
		  cleanup();
		    
		if (mLog)
		  delete mLog;
	}
 
	void QtRenderer::getWindowHandle(size_t *windowHnd) {
	  //mRenderWindow->getCustomAttribute( "WINDOW", windowHnd );
	};
	
	void QtRenderer::getWindowExtents(int *width, int *height) {
	  (*width) = 640;
	  (*height) = 480;
	};
			  
	bool QtRenderer::setup(int argc, char** argv) {
    if (fSetup)
      return true;
		
		Launcher::getSingleton().disableInputManager();
		
		bind("UnableToConnect", this, &QtRenderer::evtUnableToConnect);
		bind("ValidateStarted", this, &QtRenderer::evtValidateStarted);
		bind("ValidateComplete", this, &QtRenderer::evtValidateComplete);
		bind("PatchStarted", this, &QtRenderer::evtPatchStarted);
		bind("PatchProgress", this, &QtRenderer::evtPatchProgress);
		bind("PatchFailed", this, &QtRenderer::evtPatchFailed);
		bind("PatchComplete", this, &QtRenderer::evtPatchComplete);
		bind("ApplicationPatched", this, &QtRenderer::evtApplicationPatched);
		
    margc = argc;
    margv = argv;
    //boost::thread mQt(startQt, argc, argv);
    
    mLog->infoStream() << "set up";
	  return true;
	};
	
	void QtRenderer::setupWidgets() {
    mQuitButton = new QPushButton("Quit");
    QObject::connect(mQuitButton, SIGNAL(clicked()), qApp, SLOT(quit()));
    
    mLayout = new QVBoxLayout();
    mLayout->addWidget(mQuitButton);

    mWindow = new QWidget();
    mWindow->setLayout(mLayout);

    mYesNoDlg = new QDialog(mWindow);
    mYesNoDlg->setModal(true);
    
    mWindow->show();	
	};
	
	/*void QtRenderer::startQt(int argc, char** argv) {
	  std::cout << "starting QT\n";

    QApplication app(argc, argv);
    app.setOrganizationName("Karazeh");
    app.setApplicationName("Karazeh");

    static_cast<QtRenderer*>(Launcher::getSingleton().getRenderer())->setupWidgets();
    //static_cast<QtRenderer*>(Launcher::getSingleton().getRenderer())->getWindow()->show();
    //mWindow->show();
    
	  qApp->exec();
	}*/
	
	QWidget *QtRenderer::getWindow() { return mWindow; };
	bool QtRenderer::deferredSetup() {
    //QApplication* qApp = QApplication::instance();
	};
	
	bool QtRenderer::cleanup() {

	};
	
	void QtRenderer::go() {
	  mLog->infoStream() << "QT GOING";
    mApp = new QApplication(margc, margv);
    mApp->setOrganizationName("Karazeh");
    mApp->setApplicationName("Karazeh");
    
    setupWidgets();
    
    mApp->exec();
    	  
	  //QApplication::instance()->exec();
	};
	
	void QtRenderer::update(unsigned long lTimeElapsed) {
    processEvents();
    
    //qApp->processEvents();
	}

	bool QtRenderer::evtValidateStarted(Event* inEvt) {

	  return true;
	}
	bool QtRenderer::evtValidateComplete(Event* inEvt) {
    mYesNoDlg->exec();
    
	  return true;
	}
	
	bool QtRenderer::evtPatchStarted(Event* inEvt) {


	  return true;
	}
	
	bool QtRenderer::evtPatchFailed(Event* inEvt) {
      
    return true;
	}
	
	bool QtRenderer::evtPatchComplete(Event* inEvt) {
	  
		return true;
  }
  
  bool QtRenderer::evtApplicationPatched(Event* inEvt) {
	  
    return true;
  }
  
  
	bool QtRenderer::keyPressed( const OIS::KeyEvent &e ) {
		return true;
	}
	
	bool QtRenderer::keyReleased( const OIS::KeyEvent &e ) {
		if (e.key == OIS::KC_ESCAPE)
		  Launcher::getSingleton().requestShutdown();
		  
		return true;
	}
	
	bool QtRenderer::mouseMoved( const OIS::MouseEvent &e ) {

		return true;
	}
	
	bool QtRenderer::mousePressed( const OIS::MouseEvent &e, OIS::MouseButtonID id ) {

		return true;
	}
	
	bool QtRenderer::mouseReleased( const OIS::MouseEvent &e, OIS::MouseButtonID id ) {
		
		return true;
	}
  
  bool QtRenderer::evtUnableToConnect(Event* inEvt) {
    
    return true;
  };
  
  bool QtRenderer::evtPatchProgress(Event* inEvt) {

    return true;
  }
};
