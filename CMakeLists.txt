cmake_minimum_required(VERSION 3.5.1)
project(StateMachine)

set(CMAKE_CXX_STANDARD 14)

find_package (Threads)


include_directories(include)
add_executable(StateMachine main.cpp include/StateMachineManager.h include/server/Server.h include/client/Client.h
        include/StateMachine.h include/shm.h include/StateMachineResource.h include/pthread_lock_guard.h src/Client.cpp src/Server.cpp
        include/SharedMemoryResource.h include/client/ClientSharedMemoryResource.h
        include/server/ServerSharedMemoryResource.h src/ClientSharedMemoryResource.cpp src/ServerSharedMemoryResource.cpp
        include/server/ServerManager.h include/client/ClientManager.h
        include/ifstream_guard.h include/ofstream_guard.h
)

target_link_libraries (StateMachine  ${CMAKE_THREAD_LIBS_INIT})

