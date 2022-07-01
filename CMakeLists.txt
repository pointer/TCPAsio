cmake_minimum_required(VERSION 3.0.0)
SET (VERSION_MAJOR 1)
SET (VERSION_MINOR 0)
SET (VERSION_FEATURE 0)
SET (VERSION_PATCH 0)
SET (VERSION "${VERSION_MAJOR}.${VERSION_MINOR}")
SET (VERSION "${VERSION}.${VERSION_FEATURE}.${VERSION_PATCH}")
MESSAGE ("Version: ${VERSION}")

set_property(GLOBAL PROPERTY USE_FOLDERS TRUE)
# the name of the target operating system
SET(CMAKE_SYSTEM_NAME Windows)
project(TCPgRPC VERSION ${VERSION} LANGUAGES C CXX)
set(CMAKE_GENERATOR_PLATFORM x64)
set(CMAKE_BUILD_TYPE Debug)
# Organize targets into folders.
# set(CMAKE_VERBOSE_MAKEFILE FALSE)
# adjust the default behaviour of the FIND_XXX() commands:
# search headers and libraries in the target environment, search
# programs in the host environment
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Typically, you'll also want to turn off compiler-specific extensions:
set( CMAKE_CXX_STANDARD_REQUIRED ON )
set( CMAKE_CXX_EXTENSIONS OFF)
set(VALID_BUILD_TYPES "Release" "Debug" "MinSizeRel" "RelWithDebInfo")

if( CMAKE_COMPILER_IS_GNUCC AND CMAKE_CXX_COMPILER_VERSION VERSION_GREATER 10.0.0)
     set( CMAKE_CXX_STANDARD 20 )
     MESSAGE( STATUS "Setting C++ version to C++20" )
 else()
    set( CMAKE_CXX_STANDARD 14 )
    MESSAGE( STATUS "Setting C++ version to C++14" )
endif()
#set CMAKE_BUILD_TYPE default value
if(NOT CMAKE_CONFIGURATION_TYPES)
    if("${CMAKE_BUILD_TYPE}" STREQUAL "")
        set(CMAKE_BUILD_TYPE "Debug" CACHE STRING "Build configuration" FORCE)
    endif()
endif()

#validate CMAKE_BUILD_TYPE
if(NOT CMAKE_CONFIGURATION_TYPES)
    list(FIND VALID_BUILD_TYPES "${CMAKE_BUILD_TYPE}" INDEX)
    if(${INDEX} MATCHES -1)
        message(FATAL_ERROR "Invalid build type. Valid types are [${VALID_BUILD_TYPES}]")
    endif()
endif()

#Create dropdown list for CMake gui
if(NOT CMAKE_CONFIGURATION_TYPES)
    if(DEFINED CMAKE_BUILD_TYPE)
        set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS ${VALID_BUILD_TYPES})
    endif()
endif() 
include(CTest)
enable_testing()

# Organize targets into folders.
set(CMAKE_VERBOSE_MAKEFILE TRUE)

if(NOT DEFINED CMAKE_SUPPRESS_DEVELOPER_WARNINGS)
     set(CMAKE_SUPPRESS_DEVELOPER_WARNINGS 1 CACHE INTERNAL "No dev warnings")
endif()

ADD_SUBDIRECTORY(TCPAsioNetworking)

ADD_SUBDIRECTORY(TCPAsioClient)

ADD_SUBDIRECTORY(TCPAsioServer)

if (BUILD_TESTS)
    add_subdirectory(test)
endif()

# add_executable(TCPgRPC main.cpp)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
