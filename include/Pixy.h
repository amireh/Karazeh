#ifndef H_Pixy_H
#define H_Pixy_H

#include <exception>
#include <stdint.h>
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

#if OGRE_PLATFORM == OGRE_PLATFORM_WINDOWS
#define PROJECT_LOG_DIR "..\\log"
#define PROJECT_RESOURCES "\\resources"
#define PROJECT_SCRIPTS "\\resources\\scripts"
#elif OGRE_PLATFORM == OGRE_PLATFORM_APPLE
#define PROJECT_RESOURCES "/Resources"
#define PROJECT_LOG_DIR "/Resources/log"
#define PROJECT_SCRIPTS "/Resources/scripts"
#else
#define PROJECT_LOG_DIR "../log"
#define PROJECT_RESOURCES "/resources"
#define PROJECT_SCRIPTS "/resources/scripts"
#endif

#endif
