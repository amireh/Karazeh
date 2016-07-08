/*
 *  Copyright (c) 2011-2012 Ahmad Amireh <kandie@mxvt.net>
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

#ifndef H_KARAZEH_H
#define H_KARAZEH_H

#include <string>
#include <cassert>
#include "karazeh/exception.hpp"
#include <boost/filesystem.hpp>

#define KZH_DISTANCE_FROM_ROOT 1

// Finds the current platform.
//
// Proudly stolen from Ogre3D code in OgrePlatform.h
#define KZH_PLATFORM_WIN32 1
#define KZH_PLATFORM_LINUX 2
#define KZH_PLATFORM_APPLE 3
#define KZH_PLATFORM_SYMBIAN 4
#define KZH_PLATFORM_IPHONE 5

#if defined( __SYMBIAN32__ )
#   define KZH_PLATFORM KZH_PLATFORM_SYMBIAN
#elif defined( __WIN32__ ) || defined( _WIN32 )
#   define KZH_PLATFORM KZH_PLATFORM_WIN32
#elif defined( __APPLE_CC__)
#   if __ENVIRONMENT_IPHONE_OS_VERSION_MIN_REQUIRED__ >= 30000 || __IPHONE_OS_VERSION_MIN_REQUIRED >= 30000
#       define KZH_PLATFORM KZH_PLATFORM_IPHONE
#   else
#       define KZH_PLATFORM KZH_PLATFORM_APPLE
#   endif
#else
#   define KZH_PLATFORM KZH_PLATFORM_LINUX
#endif

namespace kzh {
  typedef std::string string_t;
  typedef unsigned long uint64_t;
  typedef boost::filesystem::path path_t;
}

#endif