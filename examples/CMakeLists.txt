INCLUDE(cmake/macros/ConfigureRSync)

INCLUDE_DIRECTORIES(.) # for test.hpp
INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR}/include)
INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR}/deps)
INCLUDE_DIRECTORIES(${CMAKE_BINARY_DIR}/exports)

ADD_EXECUTABLE(example__basic basic/main.cpp)
TARGET_LINK_LIBRARIES(example__basic kzh)

IF(APPLE)
  SET(CMAKE_CXX_FLAGS "-std=c++11 -Wc++11-extensions")
ENDIF()