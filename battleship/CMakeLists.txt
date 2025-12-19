cmake_minimum_required(VERSION 3.10)
project(battleship)

set(CMAKE_CXX_STANDARD 17)

find_package(Threads REQUIRED)
find_package(PkgConfig REQUIRED)
pkg_check_modules(ZMQ REQUIRED libzmq)

add_executable(server server.cpp)
add_executable(client client.cpp)

target_include_directories(server PRIVATE ${ZMQ_INCLUDE_DIRS})
target_link_libraries(server PRIVATE ${ZMQ_LIBRARIES} Threads::Threads)

target_include_directories(client PRIVATE ${ZMQ_INCLUDE_DIRS})
target_link_libraries(client PRIVATE ${ZMQ_LIBRARIES} Threads::Threads)
