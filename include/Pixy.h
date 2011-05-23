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

#include <exception>
#include <stdint.h>
#include <OgrePlatform.h>
#include "log4cpp/Category.hh"
#include "log4cpp/FixedContextCategory.hh"
#include "log4cpp/FileAppender.hh"
#include "PixyLogLayout.h"
#include "PixyExceptions.h"

// some definitions
#define PIXY_APP_VERSION "VERSION 1.0.0" // used in log
#define PIXY_APP_NAME "Karazeh" // used in RenderWindow title
#define PIXY_LOG_CATEGORY "Launcher" // used in log

/* -------------
 * PATHS
 * ------------- */
#define PROJECT_ROOT ".."

#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
#define PROJECT_LOG_DIR "..\\log"
#define PROJECT_RESOURCES "\\resources"
#define PROJECT_SCRIPTS "\\resources\\scripts"
#define PROJECT_TEMP_DIR "..\\tmp\\"
#elif OGRE_PLATFORM == OGRE_PLATFORM_APPLE
#define PROJECT_RESOURCES "/Resources"
#define PROJECT_LOG_DIR "/Resources/log"
#define PROJECT_SCRIPTS "/Resources/scripts"
#define PROJECT_TEMP_DIR "/Resources/tmp/"
#else
#define PROJECT_LOG_DIR "../log"
#define PROJECT_RESOURCES "/resources"
#define PROJECT_SCRIPTS "/resources/scripts"
#define PROJECT_TEMP_DIR "../tmp/"
#endif

#endif
