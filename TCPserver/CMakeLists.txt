cmake_minimum_required(VERSION 3.14)

project(10_TCPserver LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 COMPONENTS Network Core REQUIRED)

add_executable(10_TCPserver
  main.cpp
  tcpserver.h
  tcpserver.cpp
  structs.h
)

target_link_libraries(10_TCPserver Qt6::Core
                                   Qt6::Network)
