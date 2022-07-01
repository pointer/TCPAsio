cmake_minimum_required(VERSION 3.19)

project(TCPAsioNetworking)
include_directories (include)
FIND_PACKAGE(Boost 1.79.0 REQUIRED)
IF (Boost_FOUND)
    MESSAGE("Boost information:")
    set(BOOST_ROOT ${Boost_INCLUDE_DIR})    
    set(Boost_INCLUDE_DIRS ${BOOST_ROOT})
    # set(Boost_LIBRARY_DIRS ${BOOST_ROOT}/libs)
    # find_package(Boost COMPONENTS coroutine filesystem system thread REQUIRED)
    # link_libraries(Boost::coroutine Boost::filesystem Boost::system Boost::thread)    
    MESSAGE("  BOOST_ROOT: ${BOOST_ROOT}")    
    MESSAGE("  Boost_INCLUDE_DIRS: ${Boost_INCLUDE_DIRS}") 
    MESSAGE("  Boost_LIBRARY_DIRS: ${Boost_LIBRARY_DIRS}") 
    INCLUDE_DIRECTORIES(${Boost_INCLUDE_DIRS})
    link_directories(${Boost_LIBRARY_DIRS})
    add_definitions(${Boost_DEFINITIONS})    
    ADD_DEFINITIONS( "-DHAS_BOOST" )
ENDIF()

set(Boost_USE_STATIC_LIBS        ON)  # only find static libs
set(Boost_USE_DEBUG_LIBS        OFF)  # ignore debug libs and
set(Boost_USE_RELEASE_LIBS       ON)  # only find release libs
set(Boost_USE_MULTITHREADED      ON)
set(Boost_USE_STATIC_RUNTIME    OFF)
option (Boost_USE_STATIC_LIBS "Use Static Boost libraries" ON)

# Must come before Boost includes, otherwise the
# IDE sees the wrong file due to boost/ symlinks.


find_package(OpenSSL)
if (NOT MSVC)
    link_libraries(${OPENSSL_LIBRARIES})
endif()

file (GLOB_RECURSE HEADERS
    ./include/client/*.h
    ./include/server/*.h
)
# file(GLOB_RECURSE SOURCES src/*.cpp )
file (GLOB_RECURSE SOURCES
    ./src/client/*.cpp
    ./src/server/*.cpp
)
add_library(${PROJECT_NAME} ${SOURCES})

target_include_directories(${PROJECT_NAME}
        PUBLIC
            $<INSTALL_INTERFACE:include>
            $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
            ${Boost_INCLUDE_DIRS}
        PRIVATE
)

target_link_libraries(${PROJECT_NAME}
        PRIVATE
            ${Boost_LIBRARIES}
            ws2_32 wsock32
)