/**
 * karazeh -- the library for patching software
 *
 * Copyright (C) 2011-2016 by Ahmad Amireh <ahmad@amireh.net>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public License
 * as published by the Free Software Foundation; either version 3 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
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