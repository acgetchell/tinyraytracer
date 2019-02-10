cmake_minimum_required(VERSION 3.11)

project(tinyraytracer VERSION 0.1.0 LANGUAGES CXX)

if(${CMAKE_SOURCE_DIR} STREQUAL ${CMAKE_BINARY_DIR})
  message(
      FATAL_ERROR
      "Do not build in-source.
                       Please remove CMakeCache.txt and the CMakeFiles/ directory. Then build out-of-source."
  )
endif()

add_executable(${PROJECT_NAME} ${PROJECT_SOURCE_DIR}/src/tinyraytracer.cpp)