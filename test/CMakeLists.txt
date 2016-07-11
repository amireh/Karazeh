INCLUDE(cmake/macros/ConfigureRSync)

INCLUDE_DIRECTORIES(.) # for test_utils.hpp
INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR}/deps/catch-1.5.6) # for catch.hpp
INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR}/deps/FakeIt-2.0.2/config/standalone)
INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR}/deps/FakeIt-2.0.2/include)
INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR}/include)
INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR}/deps)
INCLUDE_DIRECTORIES(${CMAKE_BINARY_DIR}/exports)

ADD_EXECUTABLE(${PROJECT_NAME}_unit_tests
  ../src/hashers/__tests__/md5_hasher.test.cpp
  ../src/operations/__tests__/create.test.cpp
  ../src/__tests__/delta_encoder.test.cpp
  ../src/__tests__/downloader.test.cpp
  ../src/__tests__/file_manager.test.cpp
  ../src/__tests__/patcher.test.cpp
  ../src/__tests__/path_resolver.test.cpp
  main.cpp
)

IF(WIN32)
  TARGET_COMPILE_DEFINITIONS(${PROJECT_NAME}_unit_tests PRIVATE TINYXML2_IMPORT=1)
ENDIF()

TARGET_LINK_LIBRARIES(${PROJECT_NAME}_unit_tests kzh)