cmake_minimum_required(VERSION 3.0.0)
project(TP1_Sebastien_Pierre VERSION 0.1.0)

include(CTest)
enable_testing()

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

add_executable(Tp1_Sebastien_Pierre main.cpp)
add_executable(1-count-race 1-count-race.c)
add_executable(2-count-mutex 2-count-mutex.c)

target_link_libraries (1-count-race ${CMAKE_THREAD_LIBS_INIT})
target_link_libraries (2-count-mutex ${CMAKE_THREAD_LIBS_INIT})

set(CMAKE_VERBOSE_MAKEFILE ON)
set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
