INCLUDE(cmake/macros/ConfigureRSync)
INCLUDE(GenerateExportHeader)

INCLUDE_DIRECTORIES(../include)
INCLUDE_DIRECTORIES(../deps)
INCLUDE_DIRECTORIES(../deps/tinyxml2)
INCLUDE_DIRECTORIES(../deps/md5)
INCLUDE_DIRECTORIES(${CMAKE_BINARY_DIR}/exports)

IF(WIN32)
  ADD_DEFINITIONS("/wd4251")
ENDIF()

SET(Karazeh_SRCS
  ../include/karazeh/hashers/md5_hasher.hpp
  ../include/karazeh/operations/create.hpp
  ../include/karazeh/operations/update.hpp
  ../include/karazeh/operations/delete.hpp
  ../include/karazeh/config.hpp
  ../include/karazeh/delta_encoder.hpp
  ../include/karazeh/downloader.hpp
  ../include/karazeh/exception.hpp
  ../include/karazeh/file_manager.hpp
  ../include/karazeh/hasher.hpp
  ../include/karazeh/karazeh.hpp
  ../include/karazeh/logger.hpp
  ../include/karazeh/operation.hpp
  ../include/karazeh/patcher.hpp
  ../include/karazeh/path_resolver.hpp
  ../include/karazeh/release_manifest.hpp
  ../include/karazeh/utility.hpp

  ../deps/tinyxml2/tinyxml2.h
  ../deps/tinyxml2/tinyxml2.cpp
  ../deps/md5/md5.hpp
  ../deps/md5/md5.cpp
  ../deps/binreloc/binreloc.h
  ../deps/binreloc/binreloc.c

  hashers/md5_hasher.cpp
  operations/create.cpp
  operations/delete.cpp
  operations/update.cpp

  delta_encoder.cpp
  downloader.cpp
  file_manager.cpp
  logger.cpp
  patcher.cpp
  path_resolver.cpp
  release_manifest.cpp
)

# generate library
ADD_LIBRARY(kzh SHARED ${Karazeh_SRCS})

IF(WIN32)
  TARGET_COMPILE_DEFINITIONS(kzh PRIVATE TINYXML2_EXPORT=1)
ENDIF()

GENERATE_EXPORT_HEADER(kzh
	BASE_NAME kzh
	EXPORT_MACRO_NAME KARAZEH_EXPORT
	EXPORT_FILE_NAME ${CMAKE_BINARY_DIR}/exports/karazeh_export.h
	STATIC_DEFINE KARAZEH_BUILT_AS_STATIC
)

TARGET_LINK_LIBRARIES(kzh ${Karazeh_LIBRARIES} rsync)

