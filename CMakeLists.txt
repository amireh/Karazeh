CMAKE_MINIMUM_REQUIRED(VERSION 2.8)

IF(DEFINED CMAKE_BUILD_TYPE)
  SET(CMAKE_BUILD_TYPE ${CMAKE_BUILD_TYPE} CACHE STRING "Choose the type of build, options are: None(CMAKE_CXX_FLAGS or CMAKE_C_FLAGS used) Debug Release RelWithDebInfo MinSizeRel.")
ELSE()
  SET(CMAKE_BUILD_TYPE RelWithDebInfo CACHE STRING "Choose the type of build, options are: None(CMAKE_CXX_FLAGS or CMAKE_C_FLAGS used) Debug Release RelWithDebInfo MinSizeRel.")
ENDIF()

INCLUDE (CheckIncludeFileCXX)
INCLUDE (CMakeDependentOption)

PROJECT(Karazeh)

SET( CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_SOURCE_DIR}/CMake ${CMAKE_SOURCE_DIR}/CMake/Utils ${CMAKE_SOURCE_DIR}/CMake/Packages )

# if CMake can't find some of the libraries on windows, adjust these paths
IF(WIN32)
  SET( CMAKE_PREFIX_PATH ${CMAKE_PREFIX_PATH} C:/Workspace/Build/Qt4.7.3 )
	SET( CMAKE_LIBRARY_PATH
    ${CMAKE_LIBRARY_PATH}
    C:/Workspace/Build/libcurl/lib
    C:/Workspace/Build/bzip2/Release
    C:/Workspace/Build/log4cpp/Release)
	SET( CMAKE_INCLUDE_PATH
    ${CMAKE_INCLUDE_PATH}
    C:/Workspace/Build/libcurl/include
    C:/Workspace/Build/bzip2/include
    C:/Workspace/Build/log4cpp/include)

  ADD_DEFINITIONS("-D_CRT_SECURE_NO_WARNINGS")
  ADD_DEFINITIONS("-DCURL_STATICLIB")
ELSE()
  ADD_DEFINITIONS("-DENABLE_BINRELOC")
ENDIF()

set(Boost_USE_STATIC_LIBS ON)
set(Boost_USE_MULTITHREAD ON)

FIND_PACKAGE(PkgConfig)
FIND_PACKAGE(Boost 1.46 COMPONENTS system filesystem REQUIRED)
FIND_PACKAGE(Boost 1.46 COMPONENTS thread date_time)
FIND_PACKAGE(CURL REQUIRED)
FIND_PACKAGE(BZip2 REQUIRED)
# optional dependencies
FIND_PACKAGE(log4cpp)
FIND_PACKAGE(TBB)
FIND_PACKAGE(POCO)

# project version
SET( ${PROJECT_NAME}_MAJOR_VERSION 0 )
SET( ${PROJECT_NAME}_MINOR_VERSION 1 )
SET( ${PROJECT_NAME}_BUILD_LEVEL 0 )

# options
OPTION(KARAZEH_PERSISTENT "Don't clean up patch data to enable resume support (experimental)" OFF)
OPTION(KARAZEH_REAL_PROGRESS "Injects the Renderer with precise (bytes) downloading progress (recommended)" ON)
OPTION(KARAZEH_RENDERER_OGRE "Build Ogre renderer (requires version 1.7.2+)" OFF)
OPTION(KARAZEH_RENDERER_QT "Build Qt renderer  (requires version 4.7+)" OFF)
OPTION(KARAZEH_RENDERER_GTK3 "Build the GTK3 renderer" OFF)

IF(DEFINED KARAZEH_DEFAULT_RENDERER)
  SET(KARAZEH_DEFAULT_RENDERER
    ${KARAZEH_DEFAULT_RENDERER}
    CACHE STRING "Choose a renderer to be chosen by default when Karazeh is run, options are: Vanilla (CLI), Ogre, Qt, Cocoa (Mac only), GTK3")
ELSE()
  SET(KARAZEH_DEFAULT_RENDERER
    Vanilla
    CACHE STRING "Choose a renderer to be chosen by default when Karazeh is run, options are: Vanilla (CLI), Ogre, Qt, Cocoa (Mac only), GTK3")
ENDIF()

IF(Boost_THREAD_FOUND AND Boost_DATE_TIME_FOUND)
  OPTION(KARAZEH_THREADS_BOOST "Use boost threads (you can choose AT MOST one thread providor)" OFF)
ENDIF()
IF(TBB_FOUND)
  OPTION(KARAZEH_THREADS_TBB "Use Intel TBB threads (you can choose AT MOST one thread providor)" OFF)
ENDIF()
IF(POCO_FOUND)
  OPTION(KARAZEH_THREADS_POCO "Use POCO for threading (you can choose AT MOST one thread providor)" OFF)
ENDIF()

SET(EXECUTABLE_OUTPUT_PATH "${CMAKE_SOURCE_DIR}/bin")
IF(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
  SET(CMAKE_INSTALL_PREFIX ${EXECUTABLE_OUTPUT_PATH} CACHE PATH "Karazeh install prefix" FORCE)
ENDIF()

# common includes and libraries
INCLUDE_DIRECTORIES(
  include
  ${CMAKE_CURRENT_BINARY_DIR}
  ${CURL_INCLUDE_DIRS}
  ${Boost_INCLUDE_DIRS}
  ${BZIP2_INCLUDE_DIR}
  ${LOG4CPP_INCLUDE_DIR}
)
SET(KARAZEH_USE_LOG4CPP ON)
IF(DEFINED KARAZEH_USE_LOG4CPP)
  INCLUDE_DIRECTORIES(${LOG4CPP_INCLUDE_DIR})
  LINK_LIBRARIES( ${LOG4CPP_LIBRARIES} )
ENDIF()

IF (KARAZEH_THREADS_BOOST)
  LINK_LIBRARIES( ${Boost_THREAD_LIBRARY} ${Boost_DATE_TIME_LIBRARY} )
ENDIF()

IF (KARAZEH_THREADS_TBB)
  LINK_LIBRARIES( ${TBB_LIBRARIES} )
ENDIF()

IF (KARAZEH_THREADS_POCO)
  LINK_LIBRARIES( ${POCO_LIBRARIES} )
ENDIF()

CONFIGURE_FILE( ${CMAKE_SOURCE_DIR}/KarazehConfig.h.cmake ${CMAKE_SOURCE_DIR}/include/KarazehConfig.h )

IF(APPLE)
  LINK_DIRECTORIES("/opt/local/lib")
  LINK_LIBRARIES("/opt/local/lib/libtar.a") # use static version of libtar
ELSE()
  LINK_LIBRARIES(tar)
ENDIF()

LINK_DIRECTORIES(${BOOST_LIBRARY_DIRS} ${CURL_LIBRARY_DIRS} ${BZip2_LIBRARY_DIRS})
LINK_LIBRARIES( ${Boost_LIBRARIES} ${CURL_LIBRARIES} ${BZIP2_LIBRARIES})

ADD_SUBDIRECTORY(src)

# just hide this
MARK_AS_ADVANCED(CMAKE_INSTALL_PREFIX)
