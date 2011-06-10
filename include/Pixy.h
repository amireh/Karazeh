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

#ifndef H_Pixy_H
#define H_Pixy_H

#include "KarazehConfig.h"
#include <exception>
#include <stdint.h>
#include "log4cpp/Category.hh"
#include "log4cpp/FixedContextCategory.hh"
#include "log4cpp/FileAppender.hh"
#include "PixyLogLayout.h"
/* 
 * log4cpp defines int64_t as __int64_t but does not use it at all,
 * and this conflicts with boost on WIN32, so we undefine it here
 */
#ifdef int64_t
  #undef int64_t
#endif
#include "PixyExceptions.h"

/* Application specific definitions */
#define PIXY_APP_VERSION "VERSION 1.0.0" // used in log
#define PIXY_APP_NAME "Karazeh" // used in Renderer windows titlebar
#define PIXY_LOG_CATEGORY "Karazeh" // used in log4cpp
#define PIXY_RESOURCE "Karazeh.dat"
#define PIXY_MIRRORS_RESOURCE "patch_mirrors.txt"
//#define PIXY_PERSISTENT

/* Finds the current platform
 * Note: proudly stolen from Ogre3D code in OgrePlatform.h */
#define PIXY_PLATFORM_WIN32 1
#define PIXY_PLATFORM_LINUX 2
#define PIXY_PLATFORM_APPLE 3
#define PIXY_PLATFORM_SYMBIAN 4
#define PIXY_PLATFORM_IPHONE 5

#if defined( __SYMBIAN32__ )
#   define PIXY_PLATFORM PIXY_PLATFORM_SYMBIAN
#elif defined( __WIN32__ ) || defined( _WIN32 )
#   define PIXY_PLATFORM PIXY_PLATFORM_WIN32
#elif defined( __APPLE_CC__)
#   if __ENVIRONMENT_IPHONE_OS_VERSION_MIN_REQUIRED__ >= 30000 || __IPHONE_OS_VERSION_MIN_REQUIRED >= 30000
#       define PIXY_PLATFORM PIXY_PLATFORM_IPHONE
#   else
#       define PIXY_PLATFORM PIXY_PLATFORM_APPLE
#   endif
#else
#   define PIXY_PLATFORM PIXY_PLATFORM_LINUX
#endif

/* Application paths:
 *
 * 1) PROJECT_ROOT: relative path to the root where everything will be found
 * 2) PROJECT_LOG_DIR: path to the directory where the logs will be dumped
 * 3) PROJECT_RESOURCES: where you keep your application-specific data/assets
 * 4) PROJECT_TEMP_DIR: a temp directory used for staging patch files
 *
 */

#if PIXY_PLATFORM == PIXY_PLATFORM_WIN32
#define PROJECT_LOG_DIR "log"
#define PROJECT_RESOURCES "resources"
#define PROJECT_OGRE_RESOURCES "ogre"
#define PROJECT_TEMP_DIR "tmp"
#define PIXY_EXTERNAL_APP_PATH "Karazeh.exe" // path to the application launched relative to this application
#define PIXY_EXTERNAL_APP_NAME "Karazeh" // name of the application to be launched
#define PIXY_EXTERNAL_APP_ARG "" // if you need to pass more than 1 argument, see Launcher::launchExternalApplication()
const int PIXY_DISTANCE_FROM_ROOT = 2; // how many directories the Launcher is located from the app root
#elif PIXY_PLATFORM == PIXY_PLATFORM_APPLE
#define PROJECT_LOG_DIR "/Resources/log"
#define PROJECT_RESOURCES "/Resources"
#define PROJECT_OGRE_RESOURCES "ogre"
#define PROJECT_TEMP_DIR "/Resources/tmp"
#define PIXY_EXTERNAL_APP_PATH "Karazeh" // path to the application launched relative to this application
#define PIXY_EXTERNAL_APP_NAME "Karazeh" // name of the application to be launched
#define PIXY_EXTERNAL_APP_ARG "Qt" // if you need to pass more than 1 argument, see Launcher::launchExternalApplication()
const int PIXY_DISTANCE_FROM_ROOT = 1;
#else
#define PROJECT_LOG_DIR "log"
#define PROJECT_RESOURCES "resources"
#define PROJECT_OGRE_RESOURCES "ogre"
#define PROJECT_TEMP_DIR "tmp"
#define PIXY_EXTERNAL_APP_PATH "Karazeh" // path to the application launched relative to this application
#define PIXY_EXTERNAL_APP_NAME "Karazeh" // name of the application to be launched
#define PIXY_EXTERNAL_APP_ARG "Qt" // if you need to pass more than 1 argument, see Launcher::launchExternalApplication()
const int PIXY_DISTANCE_FROM_ROOT = 1;
#endif

#endif
