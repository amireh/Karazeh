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

#include "Renderers/GTK3/GTK3Renderer.h"
#include "Launcher.h"
#include "Patcher.h"
#include <stdio.h>
#include <string>
#include <glib.h>

using std::cout;
using std::cin;
using std::string;
namespace Pixy {

	GTK3Renderer::GTK3Renderer() {
	  mLog = new log4cpp::FixedContextCategory(PIXY_LOG_CATEGORY, "GTK3Renderer");
		mLog->infoStream() << "firing up";

		mName = "GTK3Renderer";
  }

	GTK3Renderer::~GTK3Renderer() {

		mLog->infoStream() << "shutting down";

		if (fSetup)
		  cleanup();

		if (mLog)
		  delete mLog;

    txtLatestChanges = 0;
    txtStatus = 0;
    btnLaunch = 0;
    progressBar = 0;
    window = 0;

	}

	bool GTK3Renderer::setup(int argc, char** argv) {
    if (fSetup)
      return true;

    mLog->infoStream() << "set up";

    fShuttingDown = false;

	  return true;
	};

	bool GTK3Renderer::cleanup() {
    if (fShuttingDown)
      return true;

    fShuttingDown = true;

    return true;
	};

	void GTK3Renderer::go(int argc, char** argv) {
    std::string lPath = Launcher::getSingleton().getRootPath();
    lPath += "/resources/gtk3/karazeh.glade";

    g_thread_init( NULL );
    gdk_threads_init();
    Glib::thread_init();

    gdk_threads_enter();
    Gtk::Main kit(argc, argv);

    //main_loop_ = Glib::MainLoop::create();
    //main_loop_->run();

    builder = Gtk::Builder::create_from_file(lPath.c_str());

    // bind references to widgets
    builder->get_widget("Karazeh", window);
    builder->get_widget("btnLaunch", btnLaunch);
    builder->get_widget("progressBar", progressBar);
    builder->get_widget("txtStatus", txtStatus);
    builder->get_widget("txtLatestChanges", txtLatestChanges);

    // bind Glib::Dispatchers
    d_guiStarted.connect(sigc::mem_fun(*this, &GTK3Renderer::onGuiStart));
    d_unableToConnect.connect(sigc::mem_fun(*this, &GTK3Renderer::handleUnableToConnect));
    d_validateStarted.connect(sigc::mem_fun(*this, &GTK3Renderer::handleValidateStarted));
    d_validateComplete.connect(sigc::mem_fun(*this, &GTK3Renderer::handleValidateComplete));
    d_patchStarted.connect(sigc::mem_fun(*this, &GTK3Renderer::handlePatchStarted));
    d_patchSize.connect(sigc::mem_fun(*this, &GTK3Renderer::handlePatchSize));
    d_patchProgress.connect(sigc::mem_fun(*this, &GTK3Renderer::handlePatchProgress));
    d_patchFailed.connect(sigc::mem_fun(*this, &GTK3Renderer::handlePatchFailed));
    d_patchComplete.connect(sigc::mem_fun(*this, &GTK3Renderer::handlePatchComplete));
    d_applicationPatched.connect(sigc::mem_fun(*this, &GTK3Renderer::handleApplicationPatched));

    btnLaunch->signal_released().connect(sigc::mem_fun(*this, &GTK3Renderer::doLaunch));
    // start the validation once the gui is loaded
    d_guiStarted.emit();

    kit.run(*window);
    gdk_threads_leave();
	};

  void GTK3Renderer::infoDialog(std::string inCaption, std::string inMsg) {
    gdk_threads_enter();

    Gtk::MessageDialog dialog(*window, inCaption, false, Gtk::MESSAGE_INFO);
    dialog.set_secondary_text(inMsg);
    dialog.run();

    gdk_threads_leave();
  }

  void GTK3Renderer::errorDialog(std::string inCaption, std::string inMsg) {
    gdk_threads_enter();

    Gtk::MessageDialog dialog(*window, inCaption, false, Gtk::MESSAGE_ERROR, Gtk::BUTTONS_CLOSE);
    dialog.set_secondary_text(inMsg);
    dialog.run();

    gdk_threads_leave();
  }

  bool GTK3Renderer::promptDialog(std::string inCaption, std::string inMsg) {
    bool res;
    gdk_threads_enter();

    Gtk::MessageDialog
      dialog(*window, inCaption, false, Gtk::MESSAGE_QUESTION, Gtk::BUTTONS_OK_CANCEL);
    dialog.set_secondary_text(inMsg.c_str());

    res = dialog.run() == Gtk::RESPONSE_OK;

    gdk_threads_leave();
    return res;
  };


  void GTK3Renderer::onGuiStart() {
    Launcher::getSingleton().startValidation();

    // disable the launch button while we're patching
    btnLaunch->set_sensitive(false);
  };

  void GTK3Renderer::doReqPatch() {
    Launcher::getSingleton().startPatching();
  }

  void GTK3Renderer::doLaunch() {
    Launcher::getSingleton().launchExternalApp();
  }

  void GTK3Renderer::injectUnableToConnect( void ) {
    d_unableToConnect.emit();
  };
	void GTK3Renderer::injectValidateStarted( void ) {
    d_validateStarted.emit();
	}
	void GTK3Renderer::injectValidateComplete(bool inNeedUpdate, Version const& inTargetVersion) {
    fNeedUpdate = inNeedUpdate;
    mTargetVersion = inTargetVersion;
    if (!inNeedUpdate)
      mCurrentVersion = mTargetVersion;

    d_validateComplete.emit();
  }

	void GTK3Renderer::injectPatchStarted( Version const& inTargetVersion ) {
    mTargetVersion = inTargetVersion;

    d_patchStarted.emit();
	}

  void GTK3Renderer::injectPatchSize( pbigint_t inBytes ) {
    mPatchSize = inBytes;

    d_patchSize.emit();
	}
  void GTK3Renderer::injectPatchProgress(float inPercent) {
    if (inPercent == 0)
      return;

    mProgress = inPercent;
    d_patchProgress.emit();
  }
	void GTK3Renderer::injectPatchFailed(std::string inMsg, Version const& inTargetVersion) {
    mFailMsg = inMsg;
    mTargetVersion = inTargetVersion;

    d_patchFailed.emit();
	}

	void GTK3Renderer::injectPatchComplete(Version const& inCurrentVersion) {
    mCurrentVersion = inCurrentVersion;
    d_patchComplete.emit();
  }

  void GTK3Renderer::injectApplicationPatched( Version const& inCurrentVersion ) {
    mCurrentVersion = inCurrentVersion;
    d_applicationPatched.emit();
  }


  void GTK3Renderer::handleUnableToConnect( ) {
    txtStatus->set_label("Unable to connect, please verify your internet connectivity.");
  };

	void GTK3Renderer::handleValidateStarted( ) {
    txtStatus->set_label("Validating version, please wait...");
	}
	void GTK3Renderer::handleValidateComplete( ) {

    if (fNeedUpdate) {
      if (promptDialog("Updates are available.", "Would you like to install them now?")) {
        //Launcher::getSingleton().startPatching();
          sigc::signal<void> sig;
          sig.connect(sigc::mem_fun(*this, &GTK3Renderer::doReqPatch));
          sig.emit();
      } else {
        Gtk::Main::quit();
        return;
      }

    } else {
      handleApplicationPatched();
    }

	}
	void GTK3Renderer::handlePatchStarted( ) {
    std::string lMsg = "Updating to version " + mTargetVersion.toNumber();

    txtStatus->set_label(lMsg.c_str());
    progressBar->set_fraction(0);

	}
  void GTK3Renderer::handlePatchSize() {
  }

  void GTK3Renderer::handlePatchProgress() {
    progressBar->set_fraction(mProgress / 100.0f);
  }
	void GTK3Renderer::handlePatchFailed() {
    errorDialog("Update failed!", mFailMsg);
    Gtk::Main::quit();
	}

	void GTK3Renderer::handlePatchComplete() {
    std::string lMsg = "Successfully updated to version " + mCurrentVersion.toNumber();

    txtStatus->set_label(lMsg.c_str());
  }

  void GTK3Renderer::handleApplicationPatched() {
    std::string lMsg = "Application is up to date, version " + mCurrentVersion.toNumber();

    txtStatus->set_label(lMsg.c_str());
    btnLaunch->set_sensitive(true);
  }

};
