# add sources
SET(Karazeh_SRCS
  Launcher.cpp
  Patcher.cpp
  Downloader.cpp
  Repository.cpp
  PixyLogLayout.cpp

  Renderers/Vanilla/VanillaRenderer.cpp

  bsdiff.c
  bspatch.c
  binreloc.c

  main.cpp
)

# renderers
IF(KARAZEH_RENDERER_OGRE)
  FIND_PACKAGE(OGRE COMPONENTS RenderSystem_Direct3D9 RenderSystem_GL REQUIRED)
  FIND_PACKAGE(OIS REQUIRED)

  INCLUDE_DIRECTORIES(${OGRE_INCLUDE_DIRS} ${OIS_INCLUDE_DIRS})

  SET(Karazeh_SRCS
    ${Karazeh_SRCS}
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

  if (NOT QT4_FOUND)
    message(STATUS "Qt not found!")
  endif()

  INCLUDE(${QT_USE_FILE})
  INCLUDE_DIRECTORIES(${QT_INCLUDE_DIRS})

  SET(Karazeh_SRCS
    ${Karazeh_SRCS}
    Renderers/Qt/QtRenderer.cpp)

  QT4_WRAP_UI(Karazeh_SRCS
    "${CMAKE_SOURCE_DIR}/resources/qt/karazeh.ui"
    "${CMAKE_SOURCE_DIR}/resources/qt/dialog.ui")
  QT4_WRAP_CPP(Karazeh_SRCS
    "${CMAKE_SOURCE_DIR}/include/Renderers/Qt/QtRenderer.h")

  LINK_DIRECTORIES(${QT_LIBRARY_DIRS})
  LINK_LIBRARIES(${QT_LIBRARIES})
ENDIF()

IF(QT4_FOUND)
  OPTION(KARAZEH_THREADS_QT "Use Qt threads, must choose EXACTLY one" ON)
ENDIF()

ADD_EXECUTABLE(${PROJECT_NAME} ${Karazeh_SRCS})
TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${Karazeh_LIBRARIES})
