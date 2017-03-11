cmake_minimum_required(VERSION 2.8.3)

set (CMAKE_CXX_STANDARD 11)
set (CMAKE_CXX_FLAGS "--std=gnu++11 ${CMAKE_CXX_FLAGS}")

file(GLOB GEN_SOURCES
  gen-cpp/Example.h
  gen-cpp/Example.cpp
  gen-cpp/EventHandler.h
  gen-cpp/EventHandler.cpp
  gen-cpp/example_constants.h
  gen-cpp/example_constants.cpp
  gen-cpp/example_types.h
  gen-cpp/example_types.cpp
)

add_executable(server
  server.cpp
  ${GEN_SOURCES})

add_executable(client
  client.cpp
  ${GEN_SOURCES})

target_link_libraries(server thrift thriftnb)
target_link_libraries(client thrift)
