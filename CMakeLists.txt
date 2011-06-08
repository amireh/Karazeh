PROJECT(Karazeh)

CMAKE_MINIMUM_REQUIRED(VERSION 2.8)

INCLUDE (CheckIncludeFileCXX)

SET( CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_SOURCE_DIR}/CMake ${CMAKE_SOURCE_DIR}/CMake/Packages )

FIND_PACKAGE(PkgConfig)

FIND_PACKAGE(log4cpp REQUIRED)
FIND_PACKAGE(CURL REQUIRED)
FIND_PACKAGE(BZip2 REQUIRED)
FIND_PACKAGE(Boost COMPONENTS filesystem REQUIRED)
# optional dependencies
FIND_PACKAGE(OGRE COMPONENTS RenderSystem_Direct3D9 RenderSystem_GL)
FIND_PACKAGE(OIS)
FIND_PACKAGE(Qt4)

# project version
SET( ${PROJECT_NAME}_MAJOR_VERSION 0 )
SET( ${PROJECT_NAME}_MINOR_VERSION 1 )
SET( ${PROJECT_NAME}_BUILD_LEVEL 0 )

IF( NOT CMAKE_BUILD_TYPE )
  SET( CMAKE_BUILD_TYPE "RelWithDebInfo" )
ENDIF()

# options
OPTION(KARAZEH_RENDERER_OGRE "Build Ogre renderer using SdkTrays" OFF)
OPTION(KARAZEH_RENDERER_Qt "Build Qt renderer" OFF)
#OPTION(KARAZEH_USE_BOOST "Use boost libraries instead of POCO" ON)
#OPTION(KARAZEH_USE_POCO "Use POCO libraries instead of boost" OFF)

SET(CMAKE_C_FLAGS "-DENABLE_BINRELOC")

# add sources
SET(Karazeh_SRCS
  src/Launcher.cpp
  src/Patcher.cpp
  src/Downloader.cpp
  src/Repository.cpp
  src/PixyLogLayout.cpp

  src/Renderers/Vanilla/VanillaRenderer.cpp

  src/bsdiff.c
  src/bspatch.c
  src/binreloc.c

  src/main.cpp
)

# common includes and libraries
INCLUDE_DIRECTORIES(
  include
  ${CMAKE_CURRENT_BINARY_DIR}
  ${LOG4CPP_INCLUDE_DIRS}
  ${CURL_INCLUDE_DIRS}
  ${BOOST_INCLUDE_DIRS}
)

# renderers
IF(KARAZEH_RENDERER_OGRE)
  FIND_PACKAGE(OIS REQUIRED)
  AUX_SOURCE_DIRECTORY(src/Renderers/Ogre Karazeh_SRCS)
  INCLUDE_DIRECTORIES(${OGRE_INCLUDE_DIRS} ${OIS_INCLUDE_DIRS})
  LINK_DIRECTORIES(${OGRE_LIBRARY_DIRS} ${OIS_LIBRARY_DIRS})
  LINK_LIBRARIES(${OGRE_LIBRARIES} ${OIS_LIBRARIES})
ENDIF()
IF(KARAZEH_RENDERER_Qt)
  #AUX_SOURCE_DIRECTORY(src/Renderers/Qt Karazeh_SRCS)
  INCLUDE(${QT_USE_FILE})

  SET(Karazeh_SRCS
    ${Karazeh_SRCS}
    src/Renderers/Qt/QtRenderer.cpp
  )

  ADD_DEFINITIONS(${QT_DEFINITIONS})
  INCLUDE_DIRECTORIES(${QT_INCLUDE_DIRS})
  LINK_DIRECTORIES(${QT_LIBRARY_DIRS})
  LINK_LIBRARIES(${QT_LIBRARIES})

  QT4_WRAP_UI(Karazeh_SRCS src/Renderers/Qt/karazeh.ui src/Renderers/Qt/dialog.ui)
  QT4_WRAP_CPP(Karazeh_SRCS include/Renderers/Qt/QtRenderer.h)
ENDIF()

CONFIGURE_FILE( ${CMAKE_SOURCE_DIR}/KarazehConfig.h.cmake ${CMAKE_SOURCE_DIR}/include/KarazehConfig.h )

LINK_DIRECTORIES(${BOOST_LIBRARY_DIRS} ${CURL_LIBRARY_DIRS} ${BZip2_LIBRARY_DIRS})
LINK_LIBRARIES( boost_filesystem boost_thread ${CURL_LIBRARIES} log4cpp bz2)

ADD_EXECUTABLE(bin/${PROJECT_NAME} ${Karazeh_SRCS} )
TARGET_LINK_LIBRARIES(bin/${PROJECT_NAME} ${Karazeh_LIBRARIES})
