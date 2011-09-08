# add sources
SET(Karazeh_SRCS
  ${Karazeh_SRCS}

  ../include/Downloader.h
  ../include/Launcher.h
  ../include/PatchEntry.h
  ../include/Patcher.h
  ../include/Repository.h
  ../include/Pixy.h
  ../include/PixyExceptions.h
  ../include/PixyLog.h
  ../include/PixyLogLayout.h
  ../include/PixyUtility.h

  ../include/Renderer.h
  ../include/Renderers/Vanilla/VanillaRenderer.h

  ../include/PixyThread.h
  ../include/Threads/PixyThreadless.h
  ../include/Threads/PixyThreadQt.h
  ../include/Threads/PixyThreadBoost.h

  ../include/binreloc.h
  ../include/md5.hpp

  Launcher.cpp
  Patcher.cpp
  Downloader.cpp
  Repository.cpp
  PixyLogLayout.cpp

  Renderers/Vanilla/VanillaRenderer.cpp

  bsdiff.cpp
  bspatch.cpp
  binreloc.c

  main.cpp
)

# ---
# Ogre3D Renderer
# ---
IF(KARAZEH_RENDERER_OGRE)
  # help find Ogre and its dependencies on Windows
  IF(WIN32)
    SET( CMAKE_LIBRARY_PATH
      ${CMAKE_LIBRARY_PATH}
      C:/Workspace/Build/Ogre1.7.2/lib/Release
      C:/Workspace/Build/Ogre1.7.2/Dependencies/lib)
    SET( CMAKE_INCLUDE_PATH
      ${CMAKE_INCLUDE_PATH}
      C:/Workspace/Build/Ogre1.7.2/include
      C:/Workspace/Build/Ogre1.7.2/Dependencies/include)
  ENDIF()

  FIND_PACKAGE(OGRE COMPONENTS RenderSystem_GL REQUIRED)
  FIND_PACKAGE(OIS REQUIRED)

  INCLUDE_DIRECTORIES(${OGRE_INCLUDE_DIRS} ${OIS_INCLUDE_DIRS})

  LIST(APPEND Karazeh_SRCS
    ../include/Renderers/Ogre/Event.h
    ../include/Renderers/Ogre/EventHandler.h
    ../include/Renderers/Ogre/EventListener.h
    ../include/Renderers/Ogre/EventManager.h
    ../include/Renderers/Ogre/Handler.h
    ../include/Renderers/Ogre/InputManager.h
    ../include/Renderers/Ogre/Listener.h
    ../include/Renderers/Ogre/OgreRenderer.h
    ../include/Renderers/Ogre/OgreSdkTrays.h

    Renderers/Ogre/Event.cpp
    Renderers/Ogre/EventListener.cpp
    Renderers/Ogre/EventManager.cpp
    Renderers/Ogre/InputManager.cpp
    Renderers/Ogre/OgreRenderer.cpp)

  LINK_DIRECTORIES(${OGRE_LIBRARY_DIRS} ${OIS_LIBRARY_DIRS})
  LINK_LIBRARIES(${OGRE_LIBRARIES} ${OIS_LIBRARIES})
ENDIF()

# ---
# Qt4 Renderer
# ---
IF(KARAZEH_RENDERER_QT)
  FIND_PACKAGE(Qt4 REQUIRED)

  OPTION(KARAZEH_THREADS_QT "Use Qt threads (you can choose AT MOST one thread providor)" ON)

  INCLUDE(${QT_USE_FILE})
  INCLUDE_DIRECTORIES(${QT_INCLUDE_DIRS})

  LIST(APPEND Karazeh_SRCS
    ../include/Renderers/Qt/QtRenderer.h
    Renderers/Qt/QtRenderer.cpp)

  QT4_WRAP_UI(Karazeh_SRCS
    "${CMAKE_SOURCE_DIR}/resources/qt/karazeh.ui"
    "${CMAKE_SOURCE_DIR}/resources/qt/dialog.ui")
  QT4_WRAP_CPP(Karazeh_SRCS
    "${CMAKE_SOURCE_DIR}/include/Renderers/Qt/QtRenderer.h")

  INCLUDE_DIRECTORIES( ${CMAKE_CURRENT_BINARY_DIR} )
  LINK_DIRECTORIES(${QT_LIBRARY_DIRS})
  LINK_LIBRARIES(${QT_LIBRARIES})
ENDIF()

# ---
# GTK3 Renderer
# ---
IF(KARAZEH_RENDERER_GTK3)
  FIND_PACKAGE(GTK3 3.0 REQUIRED gtk gtkmm glade glademm)
  pkg_check_modules(GTKMM gtkmm-3.0)
  pkg_check_modules(GLIBMM glibmm-2.4)

  OPTION(KARAZEH_THREADS_GLIB "Use Glib threads (you can choose AT MOST one thread providor)" ON)

  LIST(APPEND Karazeh_SRCS
    ../include/Renderers/GTK3/GTK3Renderer.h
    Renderers/GTK3/GTK3Renderer.cpp)

  INCLUDE_DIRECTORIES(
    ${GTK3_GTK_INCLUDE_DIRS}
    ${GTKMM_INCLUDE_DIRS}
    ${GLIBMM_INCLUDE_DIRS})
  LINK_LIBRARIES(${GTK3_GTK_LIBRARY} ${GTKMM_LIBRARIES} ${GLIBMM_LIBRARIES})
ENDIF()

# ---
# Cocoa Renderer
# ---
IF(APPLE)

  SET(XIB_FILE ${CMAKE_SOURCE_DIR}/resources/cocoa/Karazeh.xib)
  SET(NIB_FILE ${CMAKE_BINARY_DIR}/Karazeh.nib)
  SET(ICNS_FILE ${CMAKE_SOURCE_DIR}/resources/cocoa/karazeh.icns)
  SET(PLIST_FILE ${CMAKE_SOURCE_DIR}/resources/cocoa/KarazehInfo.plist.in)

  # Patcher needs to be compiled as an Objective-C++ file on Mac
  LIST(REMOVE_ITEM Karazeh_SRCS Patcher.cpp)
  CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/Patcher.cpp ${CMAKE_CURRENT_SOURCE_DIR}/Patcher.mm)

  LIST(APPEND Karazeh_SRCS
    Patcher.mm

    ../include/Renderers/Cocoa/CocoaRenderer.h
    ../include/Renderers/Cocoa/KAppDelegate.h
    Renderers/Cocoa/CocoaRenderer.mm
    Renderers/Cocoa/KAppDelegate.mm

    ${XIB_FILE}
    ${NIB_FILE}
    ${ICNS_FILE}
  )

  # tell CMake that the NIB file will be generated so it won't error out now
  SET_SOURCE_FILES_PROPERTIES(${NIB_FILE} PROPERTIES GENERATED 1)

  FIND_LIBRARY(COREFOUNDATION_LIBRARY NAMES CoreFoundation)
  FIND_LIBRARY(COCOA_LIBRARY Cocoa )
  MARK_AS_ADVANCED (COREFOUNDATION_LIBRARY COCOA_LIBRARY)
  SET(MAC_LIBS ${COREFOUNDATION_LIBRARY} ${COCOA_LIBRARY})

  OPTION(KARAZEH_RENDERER_COCOA "Use the native Cocoa renderer (HIGHLY RECOMMENDED)" ON)

ENDIF(APPLE)

# ---
# generate target
# ---
IF(WIN32 AND (KARAZEH_RENDERER_QT OR KARAZEH_RENDERER_OGRE))
  ADD_EXECUTABLE(${PROJECT_NAME} WIN32 ${Karazeh_SRCS})
ELSEIF(APPLE)
  ADD_EXECUTABLE(${PROJECT_NAME} MACOSX_BUNDLE ${Karazeh_SRCS})
  IF(KARAZEH_RENDERER_COCOA)
    TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${MAC_LIBS})
  ENDIF()
ELSE()
  ADD_EXECUTABLE(${PROJECT_NAME} ${Karazeh_SRCS})
ENDIF()
TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${Karazeh_LIBRARIES})

# handle resources for Mac bundle
IF(APPLE)
  GET_TARGET_PROPERTY(Karazeh_PATH ${PROJECT_NAME} LOCATION)

  # ---
  # Generate Info.plist (see resources/cocoa/KarazehInfo.plist.in)
  # ---
  SET_TARGET_PROPERTIES(${PROJECT_NAME} PROPERTIES MACOSX_BUNDLE_INFO_PLIST ${PLIST_FILE})

  # ---
  # generate the NIB file and move it to the Bundle
  # ---
  FIND_PROGRAM(NIB_GENERATOR ibtool /usr/bin /usr/local/bin /opt/local/bin)
  IF(NIB_GENERATOR)
    SET(NIB_GENERATOR_OPTS --errors --warnings --notices --output-format human-readable-text)
    # generate the NIB file
    ADD_CUSTOM_COMMAND(
      TARGET ${PROJECT_NAME} POST_BUILD
      COMMAND ${NIB_GENERATOR} ${NIB_GENERATOR_OPTS} --compile
      ${NIB_FILE}
      ${XIB_FILE})

    # and copy it to the bundle's resources
    SET_SOURCE_FILES_PROPERTIES(
      ${NIB_FILE}
  		PROPERTIES
     	MACOSX_PACKAGE_LOCATION Resources/English.lproj)

  ENDIF(NIB_GENERATOR)

  # ---
  # copy the icon
  # ---
  SET_SOURCE_FILES_PROPERTIES(
    ${ICNS_FILE}
		PROPERTIES
   	MACOSX_PACKAGE_LOCATION Resources/English.lproj)

  SET(OSX_FWK_DEST Karazeh.app/Contents)
  SET(OSX_RES_DEST Karazeh.app/Contents/Resources)
     
  # Copy the dependency frameworks (log4cpp)
  SET(OSX_FWK_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../dependencies/osx/Frameworks")
  INSTALL(DIRECTORY "${OSX_FWK_PATH}" DESTINATION ${OSX_FWK_DEST} COMPONENT Runtime)
  
  # Copy resources
  INSTALL(FILES "${CMAKE_CURRENT_SOURCE_DIR}/../resources/cocoa/Shroom.png" DESTINATION ${OSX_RES_DEST} COMPONENT Runtime)
  
ENDIF(APPLE)
