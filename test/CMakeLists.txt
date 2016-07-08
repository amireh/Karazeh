FIND_PACKAGE(GTest REQUIRED)

INCLUDE_DIRECTORIES(.) # for test.hpp
INCLUDE_DIRECTORIES(${GTEST_INCLUDE_DIRS})
INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR}/include)
INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR}/deps)
INCLUDE_DIRECTORIES(${CMAKE_BINARY_DIR}/exports)

ADD_EXECUTABLE(${PROJECT_NAME}_unit_tests
  test_utils.cpp
  hashers/md5_hasher.test.cpp
  file_manager.test.cpp
  downloader.test.cpp
  delta_encoder.test.cpp
  hasher.test.cpp
  patcher.test.cpp
  path_resolver.test.cpp
  main.cpp
)

IF(WIN32)
  TARGET_COMPILE_DEFINITIONS(${PROJECT_NAME}_unit_tests PRIVATE TINYXML2_IMPORT=1)
ENDIF()


TARGET_LINK_LIBRARIES(${PROJECT_NAME}_unit_tests ${GTEST_LIBRARIES} kzh)