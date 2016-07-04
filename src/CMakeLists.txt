INCLUDE(cmake/macros/ConfigureRSync)

INCLUDE_DIRECTORIES(../include)
INCLUDE_DIRECTORIES(../deps)
INCLUDE_DIRECTORIES(../deps/tinyxml2)
INCLUDE_DIRECTORIES(../deps/md5)

SET(Karazeh_SRCS
  ../deps/tinyxml2/tinyxml2.cpp
  ../deps/md5/md5.cpp
  ../deps/binreloc/binreloc.c

  karazeh.cpp

  hashers/md5_hasher.cpp
  hasher.cpp

  logger.cpp
  resource_manager.cpp
  release_manifest.cpp
  patcher.cpp
  operations/create.cpp
  operations/delete.cpp
  operations/update.cpp
  settings.cpp

  delta_encoder.cpp
)

# generate library
ADD_LIBRARY(kzh SHARED ${Karazeh_SRCS})
TARGET_LINK_LIBRARIES(kzh ${Karazeh_LIBRARIES} rsync)

# generate CLI front-end:
ADD_EXECUTABLE(${PROJECT_NAME} main.cpp)
TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${Karazeh_LIBRARIES} kzh)
