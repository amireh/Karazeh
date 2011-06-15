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
    //gdk_threads_enter();
    //Gtk::Main::quit();
    //gdk_threads_leave();

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

    //gdk_threads_enter();
    //Gtk::Main::quit();
    //gdk_threads_leave();

    //Launcher::shutdown();

    return true;
	};

	void GTK3Renderer::go(int argc, char** argv) {
    std::string lPath = Launcher::getSingleton().getRootPath();
    lPath += "/resources/gtk3/karazeh.glade";

    g_thread_init(0);
    gdk_threads_init();

    gdk_threads_enter();
    Gtk::Main kit(argc, argv);
    gdk_threads_leave();

    builder = Gtk::Builder::create_from_file(lPath.c_str());

    builder->get_widget("Karazeh", window);
    builder->get_widget("btnLaunch", btnLaunch);
    builder->get_widget("progressBar", progressBar);
    builder->get_widget("txtStatus", txtStatus);
    builder->get_widget("txtLatestChanges", txtLatestChanges);



    gdk_threads_enter();
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

  void GTK3Renderer::injectUnableToConnect( void ) {
    gdk_threads_enter();
    txtStatus->set_label("Unable to connect, please verify your internet connectivity.");
    gdk_threads_leave();
  };
  void GTK3Renderer::injectPatchProgress(int inPercent) {
    gdk_threads_enter();
    progressBar->set_fraction(inPercent / 100.0f);
    gdk_threads_leave();
  }
	void GTK3Renderer::injectValidateStarted( void ) {
    /*gdk_threads_enter();
    txtStatus->set_label("Validating version, please wait...");
    gdk_threads_leave();*/
	}
	void GTK3Renderer::injectValidateComplete(bool inNeedUpdate, Version const& inTargetVersion) {
    string answer;
    if (inNeedUpdate) {
      if (promptDialog("Updates are available.", "Would you like to install them now?"))
        Launcher::getSingleton().updateApplication();
      else {
    gdk_threads_enter();
    Gtk::Main::quit();
    gdk_threads_leave();
    return;
      }
        //return Launcher::getSingleton().requestShutdown();

    } else {
      gdk_threads_enter();
      txtStatus->set_label("Application is up to date.");
      gdk_threads_leave();
    }

	}
	void GTK3Renderer::injectPatchStarted( Version const& inTargetVersion ) {
    std::string lMsg = "Updating to version " + inTargetVersion.toNumber();

    gdk_threads_enter();
    txtStatus->set_label(lMsg.c_str());
    gdk_threads_leave();

	}
	void GTK3Renderer::injectPatchFailed(std::string inMsg, Version const& inTargetVersion) {
    errorDialog("Update failed!", inMsg);
    Launcher::getSingleton().requestShutdown();
	}

	void GTK3Renderer::injectPatchComplete(Version const& inCurrentVersion) {
    std::string lMsg = "Successfully updated to version " + inCurrentVersion.toNumber();

    gdk_threads_enter();
    txtStatus->set_label(lMsg.c_str());
    gdk_threads_leave();
  }

  void GTK3Renderer::injectApplicationPatched( Version const& inCurrentVersion ) {
    std::string lMsg = "Application is up to date, version " + inCurrentVersion.toNumber();

    gdk_threads_enter();
    txtStatus->set_label(lMsg.c_str());
    gdk_threads_leave();
  }

};
