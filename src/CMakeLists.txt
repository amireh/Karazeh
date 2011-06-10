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

# renderers
IF(KARAZEH_RENDERER_OGRE)
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

  OPTION(KARAZEH_DEFAULT_RENDERER_OGRE "Set Ogre as the default renderer instead of the CLI one" ON)
  
  INCLUDE_DIRECTORIES(${OGRE_INCLUDE_DIRS} ${OIS_INCLUDE_DIRS})

  SET(Karazeh_SRCS
    ${Karazeh_SRCS}
    
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

IF(KARAZEH_RENDERER_QT)
  FIND_PACKAGE(Qt4 REQUIRED)

  OPTION(KARAZEH_DEFAULT_RENDERER_QT "Set Qt as the default renderer instead of the CLI one" ON)
  OPTION(KARAZEH_THREADS_QT "Use Qt threads, must choose EXACTLY one" ON)
  
  INCLUDE(${QT_USE_FILE})
  INCLUDE_DIRECTORIES(${QT_INCLUDE_DIRS})

  SET(Karazeh_SRCS ${Karazeh_SRCS} ../include/Renderers/Qt/QtRenderer.h Renderers/Qt/QtRenderer.cpp)

  QT4_WRAP_UI(Karazeh_SRCS
    "${CMAKE_SOURCE_DIR}/resources/qt/karazeh.ui"
    "${CMAKE_SOURCE_DIR}/resources/qt/dialog.ui")
  QT4_WRAP_CPP(Karazeh_SRCS
    "${CMAKE_SOURCE_DIR}/include/Renderers/Qt/QtRenderer.h")

  INCLUDE_DIRECTORIES( ${CMAKE_CURRENT_BINARY_DIR} )
  LINK_DIRECTORIES(${QT_LIBRARY_DIRS})
  LINK_LIBRARIES(${QT_LIBRARIES})
ENDIF()

IF(WIN32 AND (KARAZEH_RENDERER_QT OR KARAZEH_RENDERER_OGRE))
  ADD_EXECUTABLE(${PROJECT_NAME} WIN32 ${Karazeh_SRCS})
ELSE()
  ADD_EXECUTABLE(${PROJECT_NAME} ${Karazeh_SRCS})
ENDIF()
TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${Karazeh_LIBRARIES})
