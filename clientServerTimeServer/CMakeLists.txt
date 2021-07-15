cmake_minimum_required(VERSION 3.10)

project(ServerClientApp)

find_package(Boost REQUIRED system thread)
find_package(g3log)

add_executable(server server.cpp)
target_link_libraries(server ${Boost_LIBRARIES} g3log)

add_executable(client client.cpp)
target_link_libraries(client ${Boost_LIBRARIES} ${g3log_LIBRARIES} g3log)