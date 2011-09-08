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

#import "Renderers/Cocoa/CocoaRenderer.h"
#import "Renderers/Cocoa/KAppDelegate.h"

namespace Pixy {

  KAppDelegate *mCocoaApp = 0;
	CocoaRenderer::CocoaRenderer() {
		mName = "CocoaRenderer";
  }

	CocoaRenderer::~CocoaRenderer() {

		if (fSetup)
		  cleanup();

    mCocoaApp = 0;
	}

	bool CocoaRenderer::setup(int argc, char** argv) {
    if (fSetup)
      return true;

	  return true;
	};

	bool CocoaRenderer::cleanup() {
    return true;
	};

	void CocoaRenderer::go(int argc, char** argv) {
    NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];
    NSApplicationMain(argc,  (const char **) argv);
    [pool release];
	};

  void CocoaRenderer::injectUnableToConnect( void ) {
    [mCocoaApp unableToConnect];
  };
  
	void CocoaRenderer::injectValidateStarted( void ) {
    [mCocoaApp validateStarted];
	}

  void CocoaRenderer::injectShowPatchLog(std::string inLogPath) {
    NSString *lPath = [[NSString alloc] initWithCString:inLogPath.c_str() encoding:[NSString defaultCStringEncoding]];
    [mCocoaApp showPatchLog: lPath];
    [lPath release];
  }
  
	void CocoaRenderer::injectValidateComplete(bool inNeedUpdate, Version const& inTargetVersion) {
    [mCocoaApp validateComplete:inNeedUpdate : inTargetVersion];
	}
	void CocoaRenderer::injectPatchStarted( Version const& inTargetVersion ) {
    [mCocoaApp patchStarted:inTargetVersion];
	}

  void CocoaRenderer::injectPatchSize( pbigint_t inBytes ) {

  };

  void CocoaRenderer::injectPatchProgress(float inPercent) {
    [mCocoaApp patchProgress:inPercent];
  }

	void CocoaRenderer::injectPatchFailed(std::string inMsg, Version const& inTargetVersion) {
    NSString *lMsg = [[NSString alloc] initWithCString:inMsg.c_str() encoding:[NSString defaultCStringEncoding]];
    [mCocoaApp patchFailed: lMsg : inTargetVersion];
    [lMsg release];
	}
	void CocoaRenderer::injectPatchComplete(Version const& inCurrentVersion) {
    [mCocoaApp patchComplete: inCurrentVersion];
  }
  void CocoaRenderer::injectApplicationPatched( Version const& inCurrentVersion ) {
    [mCocoaApp applicationPatched: inCurrentVersion];
  }

  void CocoaRenderer::assignRenderer() {
    mCocoaApp = static_cast<KAppDelegate*>([[NSApplication sharedApplication] delegate]);
  }

};
