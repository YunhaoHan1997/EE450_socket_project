cmake_minimum_required(VERSION 3.22)
project(project)

#set(CMAKE_CXX_STANDARD 14)


set(CMAKE_CXX_STANDARD 11)

#add_executable(client client.cpp test.cpp serverA.cpp)
add_executable(client client.cpp)
add_executable(serverM serverM.cpp)
add_executable(test test.cpp)
add_executable(serverA serverA.cpp)
add_executable(serverB serverB.cpp)
add_executable(serverC serverC.cpp)
add_executable(testClient testClient.cpp)
add_executable(monitor monitor.cpp)