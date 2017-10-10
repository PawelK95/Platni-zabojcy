cmake_minimum_required(VERSION 3.5.1)
project(Platni_zabojcy)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)


list(APPEND CMAKE_PREFIX_PATH "/users/pawelkedzia/.openmpi")

find_package(MPI REQUIRED)
find_package(Threads)
include_directories(SYSTEM ${MPI_CXX_INCLUDE_PATH})

set(SOURCE_FILES main.cpp Lamport.cpp Lamport.h Client.cpp Client.h Company.h Company.cpp Parameters.h)

add_executable(Platni_zabojcy ${SOURCE_FILES})

target_link_libraries(Platni_zabojcy ${MPI_CXX_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT})