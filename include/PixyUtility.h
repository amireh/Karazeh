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

#ifndef H_Utility_H
#define H_Utility_H

#include "PixyPlatform.h"
#include "PixyExceptions.h"
#include <typeinfo>
#include <vector>
#include <sstream>
#include <string>
#if PIXY_PLATFORM == PIXY_PLATFORM_APPLE
#include <CoreFoundation/CoreFoundation.h>
#endif

namespace Pixy {

class Utility {

  public:

#if PIXY_PLATFORM == PIXY_PLATFORM_APPLE

	// This function will locate the path to our application on OS X,
	// unlike windows you cannot rely on the current working directory
	// for locating your configuration files and resources.
	inline static
	std::string macBundlePath()
	{
		char path[1024];
		CFBundleRef mainBundle = CFBundleGetMainBundle();
		assert(mainBundle);

		CFURLRef mainBundleURL = CFBundleCopyBundleURL(mainBundle);
		assert(mainBundleURL);

		CFStringRef cfStringRef = CFURLCopyFileSystemPath( mainBundleURL, kCFURLPOSIXPathStyle);
		assert(cfStringRef);

		CFStringGetCString(cfStringRef, path, 1024, kCFStringEncodingASCII);

		CFRelease(mainBundleURL);
		CFRelease(cfStringRef);

		return std::string(path);
	}
#endif

  /* splits a string s using the delimiter delim */
  inline static
  std::vector<std::string> split(const std::string &s, char delim) {
      std::vector<std::string> elems;
      std::stringstream ss(s);
      std::string item;
      while(std::getline(ss, item, delim)) {
          elems.push_back(item);
      }
      return elems;
  }

	template<typename T>
	inline static std::string stringify(const T& x)
	{
		std::ostringstream o;
		if (!(o << x))
			throw BadConversion(std::string("stringify(")
								+ typeid(x).name() + ")");
		return o.str();
	}

	// helper; converts an integer-based type to a string
	template<typename T>
	inline static void convert(const std::string& inString, T& inValue,
						bool failIfLeftoverChars = true)
	{
		std::istringstream _buffer(inString);
		char c;
		if (!(_buffer >> inValue) || (failIfLeftoverChars && _buffer.get(c)))
			throw BadConversion(inString);
	}

	template<typename T>
	inline static T convertTo(const std::string& inString,
					   bool failIfLeftoverChars = true)
	{
		T _value;
		convert(inString, _value, failIfLeftoverChars);
		return _value;
	}

};

};

#endif
