cmake_minimum_required(VERSION 2.8.3)

set (CMAKE_CXX_STANDARD 11)
set (CMAKE_CXX_FLAGS "--std=gnu++11 ${CMAKE_CXX_FLAGS}")

file(GLOB GEN_SOURCES
  ${CMAKE_BINARY_DIR}/gen-cpp/Example.h
  ${CMAKE_BINARY_DIR}/gen-cpp/Example.cpp
  ${CMAKE_BINARY_DIR}/gen-cpp/EventHandler.h
  ${CMAKE_BINARY_DIR}/gen-cpp/EventHandler.cpp
  ${CMAKE_BINARY_DIR}/gen-cpp/example_constants.h
  ${CMAKE_BINARY_DIR}/gen-cpp/example_constants.cpp
  ${CMAKE_BINARY_DIR}/gen-cpp/example_types.h
  ${CMAKE_BINARY_DIR}/gen-cpp/example_types.cpp
)

execute_process(COMMAND thrift --gen cpp ${CMAKE_SOURCE_DIR}/example.thrift)
execute_process(COMMAND thrift --gen py ${CMAKE_SOURCE_DIR}/example.thrift)
link_directories("/usr/local/lib/")
include_directories("/usr/local/include/")
include_directories(${CMAKE_BINARY_DIR})

add_executable(server
  server.cpp
  ${GEN_SOURCES})

add_executable(client
  client.cpp
  ${GEN_SOURCES})

target_link_libraries(server thrift thriftnb event)
target_link_libraries(client thrift)
