cmake_minimum_required(VERSION 2.8)
project(cpp_http_server)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(SOURCE_FILES main.cpp http_server.cpp http_server.h connection.cpp connection.h thread_pool.h header.h response.h response.h response.cpp)
add_executable(final ${SOURCE_FILES})

link_directories(/usr/lib)
link_directories(/usr/local/lib)
target_link_libraries(final http_parser event)