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

#include "Renderers/Ogre/Event.h"
#include "Renderers/Ogre/EventManager.h"

namespace Pixy
{

  Event::Event(const std::string inName)
  {
		reset();
		mName = inName;
	}

	Event::~Event()
  {
    mUserData = 0;
  }

	void Event::reset() {
		mName.clear();
		mName = "Dummy";
		nrHandlers = 0;
		nrProperties = 0;
		mUserData = 0;
	}

	ostream& operator<<(ostream& outStream, Event* inEvt) {
		outStream << "Event: " << inEvt->getName();
		return outStream;
	}

  std::string Event::getName() const {
      return mName;
  }

	void Event::_addHandler() {
		nrHandlers++;
	}

	void Event::_removeHandler() {
		nrHandlers--;
		if (nrHandlers == 0)
			EventManager::getSingletonPtr()->_releaseEvt(this);
	}

	void Event::setProperty(const string inName, const string inValue) {
		if ( hasProperty(inName) ) {
			mProperties.find(inName)->second = inValue;
			return;
		}

		mProperties.insert( make_pair<string, string>(inName, inValue) );
		nrProperties++;
	}

	string Event::getProperty(const string inName) const {
		return mProperties.find(inName)->second;
	}

	bool Event::hasProperty(const string& inName) {
		return (mProperties.find(inName) != mProperties.end());
	}

	uint8_t Event::_getNrHandlers() { return nrHandlers; };

	void Event::setAny(void* inAny) {
	  mUserData = inAny;
	}

	void* Event::getAny() { return mUserData; }
}
