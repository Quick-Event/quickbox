cmake_minimum_required(VERSION 3.18.4)
project(quickbox LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
include(CTest)
set(CMAKE_SHARED_LIBRARY_PREFIX "") # we don't want CMake to prepend "lib" to our libraries, we prefer adding that ourselves

if (NOT TARGET libnecrolog)
    add_subdirectory(3rdparty/necrolog)
endif()

find_package(Qt5 REQUIRED COMPONENTS Core)

if(Qt5_FOUND)
    set(CMAKE_AUTOMOC ON)
else()
    message(STATUS "Qt5 not found")
endif()
