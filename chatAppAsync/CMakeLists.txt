cmake_minimum_required(VERSION 3.10)

project(ServerClientApp)

find_package(Boost REQUIRED system thread serialization)

add_executable(serverAsync serverAsync.cpp)
target_link_libraries(serverAsync ${Boost_LIBRARIES})

add_executable(clientAsync clientAsync.cpp)
target_link_libraries(clientAsync ${Boost_LIBRARIES})

