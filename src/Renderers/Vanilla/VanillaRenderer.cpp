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

#include "Renderers/Vanilla/VanillaRenderer.h"
#include "Launcher.h"
#include "Patcher.h"
#include <stdio.h>
#include <string>

using std::cout;
using std::cin;
using std::string;
namespace Pixy {

	VanillaRenderer::VanillaRenderer() {
	  mLog = new log4cpp::FixedContextCategory(PIXY_LOG_CATEGORY, "VanillaRenderer");
		mLog->infoStream() << "firing up";

		mName = "VanillaRenderer";

    fShuttingDown = false;
    fWantsToLaunch = false;
  }

	VanillaRenderer::~VanillaRenderer() {

		mLog->infoStream() << "shutting down";

    fShuttingDown = true;

		if (fSetup)
		  cleanup();

		if (mLog)
		  delete mLog;

	}

	bool VanillaRenderer::setup(int argc, char** argv) {
    if (fSetup)
      return true;

    mLog->infoStream() << "set up";

    fShuttingDown = false;

	  return true;
	};

	bool VanillaRenderer::cleanup() {
    fShuttingDown = true;

    return true;
	};

	void VanillaRenderer::go(int argc, char** argv) {

    cout << "+-+-" << PIXY_APP_NAME << "-+-+\n";
    Launcher::getSingleton().startValidation();

    while (!fShuttingDown) {
#if PIXY_PLATFORM == PIXY_PLATFORM_WIN32
      Sleep(1/60);
#else
      sleep(1/60);
#endif
    };

    cout << "Shutting down. Bye!\n";
    cout << "+-+-" << PIXY_APP_NAME << "-+-+\n";

    if (fWantsToLaunch)
      return (void)Launcher::getSingleton().launchExternalApp();
	};

  void VanillaRenderer::injectUnableToConnect( void ) {
    cout << "Unable to connect to patch server, please check your internet connectivity.\n";
    fShuttingDown = true;
  };

	void VanillaRenderer::injectValidateStarted( void ) {
    cout << "Validating application version...\n";
	}

  void VanillaRenderer::injectShowPatchLog(std::string inLogPath) {
    cout << "";
  }
  	
	void VanillaRenderer::injectValidateComplete(bool inNeedUpdate, Version const& inTargetVersion) {
    string answer;
    if (inNeedUpdate) {
      cout << "Application is out of date. Would you like to update now? [Y/n] ";
      cin >> answer;
      if (answer == "n" || answer == "N") {
        fShuttingDown = true;
      } else {
        Launcher::getSingleton().startPatching();
      }
    } else {
      cout << "Application is up to date. Would you like to launch it now? [Y/n] ";
      cin >> answer;
      if (answer == "n" || answer == "N") {
        fShuttingDown = true;
      } else {
        Launcher::getSingleton().launchExternalApp();
      }
    }
	}
	void VanillaRenderer::injectPatchStarted( Version const& inTargetVersion ) {
    cout << "Patch started: upgrading to version " << inTargetVersion.toNumber() << "\n";
	}

  void VanillaRenderer::injectPatchSize( long long inSize ) {
    cout << "Patch size: " << inSize << " bytes\n";
    lPatchSize = inSize;
  };

  void VanillaRenderer::injectPatchProgress(float inPercent) {
#ifdef KARAZEH_REAL_PROGRESS
    cout << "Patch " << (int)inPercent << "% complete (";
    cout << lPatchSize * (inPercent / 100.0f) << "b of " << lPatchSize << " bytes) \n";
#else
    cout << "Patch " << inPercent << "% complete \n";
#endif
  }

	void VanillaRenderer::injectPatchFailed(std::string inMsg, Version const& inTargetVersion) {
    cout << "Patch failed! Error: " << inMsg << "\n";
    fShuttingDown = true;
	}
	void VanillaRenderer::injectPatchComplete(Version const& inCurrentVersion) {
    cout << "Upgraded to version " << inCurrentVersion.toNumber() << " successfully.\n";
  }
  void VanillaRenderer::injectApplicationPatched( Version const& inCurrentVersion ) {
    string answer;

    cout << "All patches are complete, application is now up to date with version " << inCurrentVersion.toNumber() << "\n";
    cout << "Would you like to launch the application? [Y/n] ";
    cin >> answer;
    if (answer == "Y" || answer == "y") {
      fWantsToLaunch = true;
      fShuttingDown = true;
    } else {
      fShuttingDown = true;
    }
  }

};
