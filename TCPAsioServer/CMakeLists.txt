cmake_minimum_required(VERSION 3.19)
project(TCPAsioServer)

set(CMAKE_CXX_STANDARD 20)
find_program(CMAKE_CXX_COMPILER NAMES $ENV{CXX} g++ PATHS ENV PATH NO_DEFAULT_PATH)
add_executable(${PROJECT_NAME} main.cpp)

target_include_directories(${PROJECT_NAME}
        PUBLIC
            TCPAsioNetworking/include
)

target_link_libraries(${PROJECT_NAME}
        PUBLIC
            TCPAsioNetworking
)