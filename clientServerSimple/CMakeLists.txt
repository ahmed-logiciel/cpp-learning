cmake_minimum_required(VERSION 3.10)

project(ServerClientApp)

find_package(Boost REQUIRED system thread)

add_executable(server server.cpp)
target_link_libraries(server ${Boost_LIBRARIES})

add_executable(client client.cpp)
target_link_libraries(client ${Boost_LIBRARIES})