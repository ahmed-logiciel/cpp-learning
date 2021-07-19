cmake_minimum_required(VERSION 3.10)

project(ServerClientApp)

find_package(Boost REQUIRED system thread serialization)
find_package(GTest)

add_executable(serverParallel serverParallel.cpp)
target_link_libraries(serverParallel ${Boost_LIBRARIES} gtest gtest_main)

add_executable(clientSync clientSync.cpp)
target_link_libraries(clientSync ${Boost_LIBRARIES})

add_executable(test gtestMain.cpp)
target_link_libraries(test ${Boost_LIBRARIES} gtest gtest_main)
