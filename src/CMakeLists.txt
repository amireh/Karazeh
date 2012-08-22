# dependencies
SET(DEPS_PATH ${CMAKE_CURRENT_SOURCE_DIR}/../deps)
INCLUDE_DIRECTORIES(${DEPS_PATH})
INCLUDE_DIRECTORIES(${DEPS_PATH}/tinyxml2)
INCLUDE_DIRECTORIES(${DEPS_PATH}/md5)

SET(HEADERS_PATH ${CMAKE_CURRENT_SOURCE_DIR}/../include/karazeh)
INCLUDE_DIRECTORIES(${HEADERS_PATH} ${HEADERS_PATH}/../)

# add sources
SET(Karazeh_SRCS
  ${Karazeh_SRCS}

  ${HEADERS_PATH}/karazeh.hpp
  ${HEADERS_PATH}/logger.hpp
  
  ${DEPS_PATH}/tinyxml2/tinyxml2.cpp
  ${DEPS_PATH}/md5/md5.cpp  
  ${DEPS_PATH}/binreloc/binreloc.c  

  hashers/md5_hasher.cpp
  hasher.cpp

  logger.cpp
  resource_manager.cpp
  release_manifest.cpp
  patcher.cpp
  operations/create.cpp
  operations/delete.cpp
  settings.cpp

  delta_encoder.cpp
)

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
  ADD_EXECUTABLE(${PROJECT_NAME} main.cpp)
  ADD_LIBRARY(kzh SHARED ${Karazeh_SRCS})
ENDIF()
TARGET_LINK_LIBRARIES(kzh ${Karazeh_LIBRARIES} rsync)
TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${Karazeh_LIBRARIES} kzh)