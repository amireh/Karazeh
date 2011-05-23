#ifndef H_Pixy_H
#define H_Pixy_H

#include <exception>
#include <stdint.h>
#include <OgrePlatform.h>
#include "log4cpp/Category.hh"
#include "log4cpp/FixedContextCategory.hh"
#include "log4cpp/FileAppender.hh"
#include "PixyLogLayout.h"

// some definitions
#define PIXY_APP_VERSION "0.1" // used in log
#define PIXY_APP_NAME "Elementum" // used in RenderWindow title
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
