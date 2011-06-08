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

namespace Pixy {

	VanillaRenderer::VanillaRenderer() {
	  mLog = new log4cpp::FixedContextCategory(PIXY_LOG_CATEGORY, "VanillaRenderer");
		mLog->infoStream() << "firing up";

		mName = "VanillaRenderer";
  }

	VanillaRenderer::~VanillaRenderer() {

		mLog->infoStream() << "shutting down";

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


	bool VanillaRenderer::deferredSetup() {
	};

	bool VanillaRenderer::cleanup() {
    fShuttingDown = true;
	};

	void VanillaRenderer::go() {
    while (!fShuttingDown) {
    };
	};

  void VanillaRenderer::injectUnableToConnect( void ) {
  };
  void VanillaRenderer::injectPatchProgress(int inPercent) {
  }
	void VanillaRenderer::injectValidateStarted( void ) {
	}
	void VanillaRenderer::injectValidateComplete(bool inNeedUpdate, const Version& inTargetVersion) {
	}
	void VanillaRenderer::injectPatchStarted( const Version& inTargetVersion ) {
	}
	void VanillaRenderer::injectPatchFailed(std::string inMsg, const Version& inTargetVersion) {
	}
	void VanillaRenderer::injectPatchComplete(const Version& inCurrentVersion) {
  }
  void VanillaRenderer::injectApplicationPatched( const Version& inCurrentVersion ) {
  }

};
