cmake_minimum_required(VERSION 3.10)
project(battleship)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_BUILD_TYPE Release)

# Попытка найти ZeroMQ несколькими способами
find_package(ZeroMQ QUIET)
if(NOT ZeroMQ_FOUND)
    find_package(PkgConfig REQUIRED)
    pkg_check_modules(ZEROMQ REQUIRED IMPORTED_TARGET libzmq)
endif()

# Цели
add_executable(server server.cpp)
add_executable(client client.cpp)

# Линковка разными способами для надежности
if(ZeroMQ_FOUND)
    target_link_libraries(server PRIVATE ZeroMQ::ZeroMQ)
    target_link_libraries(client PRIVATE ZeroMQ::ZeroMQ)
else()
    target_link_libraries(server PRIVATE zmq)
    target_link_libraries(client PRIVATE zmq)
endif()

# Дополнительные флаги для отладки
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_compile_options(server PRIVATE -g -O0)
    target_compile_options(client PRIVATE -g -O0)
endif()