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

#include "Dispatcher.h"
namespace Pixy {
  Dispatcher* Dispatcher::__instance = NULL;

  Dispatcher::Dispatcher() {
    mLog = new log4cpp::FixedContextCategory(PIXY_LOG_CATEGORY, "Dispatcher");
		mLog->infoStream() << "firing up";
  }

  Dispatcher::~Dispatcher() {
    mLog->infoStream() << "shutting down";

		if (mLog)
		  delete mLog;
    mLog = 0;
  }

	Dispatcher* Dispatcher::getSingletonPtr() {
		if( !__instance ) {
		    __instance = new Dispatcher();
		}

		return __instance;
	}

	Dispatcher& Dispatcher::getSingleton() {
		return *getSingletonPtr();
	}

  bool Dispatcher::setup() { };

  bool Dispatcher::deferredSetup() { };

  bool Dispatcher::cleanup() {

  }


  void injectUnableToConnect( void ) {

  }

  void injectValidateStarted( void ) {

  }

  void injectValidateComplete( bool inNeedUpdate ) {

  }

  void injectPatchStarted( Version& inTargetVersion ) {

  }

  void injectPatchProgress( int inPercent ) {

  }

  void injectPatchFailed( std::string inMsg ) {

  }

  void injectPatchComplete( Version& inCurrentVersion ) {

  }

  void injectApplicationPatched( void ) {

  }
}
