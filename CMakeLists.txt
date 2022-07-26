cmake_minimum_required(VERSION 3.18.4)
project(quickbox LANGUAGES C CXX)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
include(CTest)
set(CMAKE_SHARED_LIBRARY_PREFIX "") # we don't want CMake to prepend "lib" to our libraries, we prefer adding that ourselves

if (NOT TARGET libnecrolog)
    add_subdirectory(3rdparty/necrolog)
endif()

find_package(Qt5 5.11 REQUIRED COMPONENTS Core Widgets Gui Sql Qml Xml LinguistTools PrintSupport Svg SerialPort Multimedia Network)

if(Qt5_FOUND)
    set(CMAKE_AUTOMOC ON)
    set(CMAKE_AUTOUIC ON)
    set(CMAKE_AUTORCC ON)

    add_subdirectory(libqf)
    add_subdirectory(libsiut)
    add_subdirectory(libquickevent)
    add_subdirectory(quickhttpd)
    add_subdirectory(quickshow)
else()
    message(FATAL_ERROR "Qt5 not found")
endif()
