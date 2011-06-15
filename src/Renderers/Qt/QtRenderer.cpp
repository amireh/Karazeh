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

    if (mApp)
      delete mApp;

		if (mLog)
		  delete mLog;

    mApp = 0;
    mLog = 0;
	}

	bool QtRenderer::setup(int argc, char** argv) {
    if (fSetup)
      return true;

    margc = argc;
    margv = argv;

    qRegisterMetaType<Version>("Version");
    qRegisterMetaType<pbigint_t>("pbigint_t");

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
                     SIGNAL(emitPatchProgress(float)),
                     this,
                     SLOT(handlePatchProgress(float)));
    QObject::connect(this,
                     SIGNAL(emitPatchSize(pbigint_t)),
                     this,
                     SLOT(handlePatchSize(pbigint_t)));
    QObject::connect(this,
                     SIGNAL(emitPatchFailed(QString, Version const&)),
                     this,
                     SLOT(handlePatchFailed(QString, Version const&)));
    QObject::connect(this,
                     SIGNAL(emitPatchComplete(Version const&)),
                     this,
                     SLOT(handlePatchComplete(Version const&)));
    QObject::connect(this,
                     SIGNAL(emitApplicationPatched(Version const&)),
                     this,
                     SLOT(handleApplicationPatched(Version const&)));

	  mLog->infoStream() << "QT GOING in thread: " << thread();
    mApp = new QApplication(margc, margv);
    mApp->setOrganizationName("Karazeh");
    mApp->setApplicationName("Karazeh");

    setupWidgets();

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
    return true;
	};

	void QtRenderer::go(int argc, char** argv) {
    // start validating when the app is loaded
    QObject::connect(this, SIGNAL(guiStarted()), this, SLOT(onGuiStart()));
    QTimer::singleShot(0, this, SIGNAL(guiStarted()));

    mApp->exec();
	};

  void QtRenderer::onGuiStart() {
    Launcher::getSingleton().startValidation();
  };

  void QtRenderer::injectUnableToConnect( void ) {
    emit emitUnableToConnect();
  };

  void QtRenderer::injectPatchProgress(float inPercent) {
    emit emitPatchProgress(inPercent);
  }

	void QtRenderer::injectValidateStarted( void ) {
    emit emitValidateStarted();
	}
	void QtRenderer::injectValidateComplete(bool inNeedUpdate, Version const& inTargetVersion) {
    emit emitValidateComplete(inNeedUpdate, inTargetVersion);
	}

	void QtRenderer::injectPatchStarted( Version const& inTargetVersion ) {
    emit emitPatchStarted(inTargetVersion);
	}

  void QtRenderer::injectPatchSize( pbigint_t inBytes ) {
    emit emitPatchSize(inBytes);
	}

	void QtRenderer::injectPatchFailed(std::string inMsg, Version const& inTargetVersion) {
    emit emitPatchFailed(QString::fromStdString(inMsg), inTargetVersion);
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
    mUI.labelStatus->setText("Status: Validating");
  }
  void QtRenderer::handleValidateComplete( bool inNeedUpdate, Version const& inTargetVersion ) {
    if (inNeedUpdate) {
      mUI.labelStatus->setText("Status: Out of date");
      std::string lMsg =
        "Application needs updating. Latest version is" + inTargetVersion.Value
        + " and current version is " + " .. would you like to update now?";
      mDlgUI.textBrowser->setText(QString(lMsg.c_str()));
      mYesNoDlg->exec();

    } else
      handleApplicationPatched(inTargetVersion);
  }

  void QtRenderer::handlePatchAccepted() {
    Launcher::getSingleton().startPatching();
  }
  void QtRenderer::handlePatchStarted( Version const& inTargetVersion ) {
    std::string lMsg = "Status: Updating to " + inTargetVersion.Value;
    mUI.labelStatus->setText(lMsg.c_str());
    mUI.progressBar->setValue(0);
  }
  void QtRenderer::handlePatchSize( pbigint_t inBytes ) {
    //lPatchSize = inSize;
  };

  void QtRenderer::handlePatchProgress(float inPercent) {
    mUI.progressBar->setValue((int)inPercent);
  }

  void QtRenderer::handlePatchFailed( QString inMsg, Version const& inTargetVersion ) {
    QString lMsg = tr("Update failed! ") + inMsg;
    mUI.textPatchStatus->setText(lMsg);
    mUI.labelStatus->setText("Status: " + lMsg);
  }
  void QtRenderer::handlePatchComplete( Version const& inCurrentVersion ) {
    mUI.labelStatus->setText("Status: Application updated");
  }
  void QtRenderer::handleApplicationPatched( Version const& inCurrentVersion ) {
    mUI.progressBar->setValue(100);
    mUI.labelStatus->setText("Status: Application is up to date, v" + QString(inCurrentVersion.toNumber().c_str()));
  }

  void QtRenderer::handleLaunchApplication() {
    Launcher::getSingleton().launchExternalApp();
  }
};
