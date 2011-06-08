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

		if (fSetup)
		  cleanup();

		if (mLog)
		  delete mLog;
	}

	bool QtRenderer::setup(int argc, char** argv) {
    if (fSetup)
      return true;

    margc = argc;
    margv = argv;
    //boost::thread mQt(startQt, argc, argv);

    qRegisterMetaType<Version>("Version");

    QObject::connect(this,
                     SIGNAL(emitUnableToConnect(void)),
                     this,
                     SLOT(handleUnableToConnect(void)));
    QObject::connect(this,
                     SIGNAL(emitValidateStarted(void)),
                     this,
                     SLOT(handleValidateStarted(void)));
    QObject::connect(this,
                     SIGNAL(emitValidateComplete(bool, Version const&)),
                     this,
                     SLOT(handleValidateComplete(bool, Version const&)));
    QObject::connect(this,
                     SIGNAL(emitPatchStarted(Version const&)),
                     this,
                     SLOT(handlePatchStarted(Version const&)));
    QObject::connect(this,
                     SIGNAL(emitPatchProgress(int)),
                     this,
                     SLOT(handlePatchProgress(int)));
    QObject::connect(this,
                     SIGNAL(emitPatchFailed(std::string, Version const&)),
                     this,
                     SLOT(handlePatchFailed(std::string, Version const&)));
    QObject::connect(this,
                     SIGNAL(emitPatchComplete(Version const&)),
                     this,
                     SLOT(handlePatchComplete(Version const&)));
    QObject::connect(this,
                     SIGNAL(emitApplicationPatched(Version const&)),
                     this,
                     SLOT(handleApplicationPatched(Version const&)));
    mLog->infoStream() << "set up";
	  return true;
	};

	void QtRenderer::setupWidgets() {

    mWindow = new QMainWindow();
    //mWindow->setLayout(mLayout);
    mUI.setupUi(mWindow);

    //Ui::Dialog dlg;
    mYesNoDlg = new QDialog(mWindow);
    //mYesNoDlg->setModal(true);
    mDlgUI.setupUi(mYesNoDlg);

    QObject::connect(mDlgUI.buttonBox,
                     SIGNAL(accepted()),
                     this,
                     SLOT(handlePatchAccepted()));
    QObject::connect(mUI.btnLaunch, SIGNAL(clicked()), this, SLOT(handleLaunchApplication()));

    mWindow->show();
	};


	QWidget *QtRenderer::getWindow() { return mWindow; };

	bool QtRenderer::cleanup() {

	};

	void QtRenderer::go() {
	  mLog->infoStream() << "QT GOING in thread: " << thread();
    mApp = new QApplication(margc, margv);
    mApp->setOrganizationName("Karazeh");
    mApp->setApplicationName("Karazeh");

    setupWidgets();

    mApp->exec();

	  //QApplication::instance()->exec();
	};


  void QtRenderer::injectUnableToConnect( void ) {
    emit emitUnableToConnect();
  };

  void QtRenderer::injectPatchProgress(int inPercent) {
    emit emitPatchProgress(inPercent);
  }

	void QtRenderer::injectValidateStarted( void ) {
    mLog->infoStream() << "injecting validate started";
    emit emitValidateStarted();
	}
	void QtRenderer::injectValidateComplete(bool inNeedUpdate, Version const& inTargetVersion) {

    mLog->infoStream() << "injecting validate complete";

    emit emitValidateComplete(inNeedUpdate, inTargetVersion);
	}

	void QtRenderer::injectPatchStarted( Version const& inTargetVersion ) {
    emit emitPatchStarted(inTargetVersion);
	}

	void QtRenderer::injectPatchFailed(std::string inMsg, Version const& inTargetVersion) {
    emit emitPatchFailed(inMsg, inTargetVersion);
	}

	void QtRenderer::injectPatchComplete(Version const& inCurrentVersion) {
    emit emitPatchComplete(inCurrentVersion);
  }

  void QtRenderer::injectApplicationPatched( Version const& inCurrentVersion ) {
    emit emitApplicationPatched(inCurrentVersion);
  }

  void QtRenderer::handleUnableToConnect( void ) {
    mUI.labelStatus->setText("Status: Unable to connect to patch server");
  }
  void QtRenderer::handleValidateStarted( void ) {
    mLog->infoStream() << "validate started";
    mUI.labelStatus->setText("Status: Validating");
  }
  void QtRenderer::handleValidateComplete( bool inNeedUpdate, Version const& inTargetVersion ) {
    mLog->infoStream() << "validate complete";

    if (inNeedUpdate) {
      std::string lMsg =
        "Application needs updating. Latest version is" + inTargetVersion.Value
        + " and current version is " + " .. would you like to update now?";
      mDlgUI.textBrowser->setText(QString(lMsg.c_str()));
      mYesNoDlg->exec();
      mUI.labelStatus->setText("Status: Out of date");
    } else {
      mUI.labelStatus->setText("Status: Application is up to date");
    }
  }

  void QtRenderer::handlePatchAccepted() {
    Launcher::getSingleton().updateApplication();
  }
  void QtRenderer::handlePatchStarted( Version const& inTargetVersion ) {
    mLog->infoStream() << "started patching";

    std::string lMsg = "Status: Updating to " + inTargetVersion.Value;
    mUI.labelStatus->setText(lMsg.c_str());
  }
  void QtRenderer::handlePatchProgress( int inPercent ) {
    mLog->infoStream() << "updating patch progress";

    mUI.progressBar->setValue(inPercent);
  }
  void QtRenderer::handlePatchFailed( std::string inMsg, Version const& inTargetVersion ) {
    mLog->infoStream() << "patching failed";

    std::string lMsg = "Update failed! " + inMsg;
    mUI.textPatchStatus->setText(lMsg.c_str());
    mUI.labelStatus->setText("Status: Update failed");
  }
  void QtRenderer::handlePatchComplete( Version const& inCurrentVersion ) {
    mLog->infoStream() << "patching cmoplete";
    mUI.labelStatus->setText("Status: Application updated");
  }
  void QtRenderer::handleApplicationPatched( Version const& inCurrentVersion ) {
    mLog->infoStream() << "all patches complete";
    mUI.labelStatus->setText("Status: Application is up to date");
  }

  void QtRenderer::handleLaunchApplication() {
    Launcher::getSingleton().launchExternalApp("./Karazeh", "Karazeh");
  }
};
