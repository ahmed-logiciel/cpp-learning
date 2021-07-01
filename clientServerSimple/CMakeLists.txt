cmake_minimum_required(VERSION 3.20)

project(ServerClientApp)

find_package(Boost REQUIRED system thread)

add_executable(ServerClientApp_Server server.cpp)
target_link_libraries(ServerClientApp_Server ${Boost_LIBRARIES})

add_executable(ServerClientApp_Client client.cpp)
target_link_libraries(ServerClientApp_Client ${Boost_LIBRARIES})