cmake_minimum_required(VERSION 3.5)

project(10_TCPclient VERSION 0.1 LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)


find_package(Qt6 COMPONENTS Widgets Network REQUIRED)


set(PROJECT_SOURCES
	    WIN32
        main.cpp
        mainwindow.cpp
        mainwindow.h
        mainwindow.ui
        tcpclient.h
        tcpclient.cpp
        structs.h
)


qt_add_executable(10_TCPclient
    MANUAL_FINALIZATION
    ${PROJECT_SOURCES}
)


target_link_libraries(10_TCPclient PRIVATE Qt6::Widgets
                                           Qt6::Network)

qt_finalize_executable(10_TCPclient)

