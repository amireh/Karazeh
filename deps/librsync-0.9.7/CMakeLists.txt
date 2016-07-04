CMAKE_MINIMUM_REQUIRED(VERSION 2.8)

PROJECT(rsync)

SET(CMAKE_BUILD_TYPE Release)

INCLUDE_DIRECTORIES(.)

IF(WIN32)
  INCLUDE_DIRECTORIES(./windows)
ELSE(UNIX)
  INCLUDE_DIRECTORIES(./linux)
ENDIF()

# add sources
SET(LIBRSYNC_SRCS
  ./stream.c
  ./netint.c
  ./delta.c
  ./prototab.c
  ./readsums.c
  ./mksum.c
  ./tube.c
  ./version.c
  ./rollsum.c
  ./checksum.c
  ./whole.c
  ./fileutil.c
  ./trace.c
  ./patch.c
  ./sumset.c
  ./msg.c
  ./base64.c
  ./job.c
  ./stats.c
  ./emit.c
  ./scoop.c
  ./hex.c
  ./mdfour.c
  ./command.c
  ./search.c
  ./buf.c
  ./util.c
)

SET(LIBRARY_OUTPUT_PATH "${CMAKE_SOURCE_DIR}/lib")

ADD_LIBRARY(rsync SHARED ${LIBRSYNC_SRCS})
ADD_LIBRARY(rsync-static STATIC ${LIBRSYNC_SRCS})

ADD_EXECUTABLE(rdiff rdiff.c)
TARGET_LINK_LIBRARIES(rdiff rsync)

INSTALL(TARGETS rsync LIBRARY DESTINATION lib)
INSTALL(TARGETS rsync-static LIBRARY ARCHIVE DESTINATION lib)