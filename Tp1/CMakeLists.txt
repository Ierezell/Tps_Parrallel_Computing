cmake_minimum_required(VERSION 3.0.0)
project(Tp1_Sebastien_Pierre VERSION 0.1.0)

include(CTest)
enable_testing()
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin)

add_library(get_prime
            src/get_prime.c
            src/get_prime.h)

add_executable(Tp1_Sebastien_Pierre src/main.cpp)
target_link_libraries (Tp1_Sebastien_Pierre ${CMAKE_THREAD_LIBS_INIT} get_prime)

##############################
# AT BUILD SYSTEM GENERATION #
##############################
# Create the dir bin if it doesn't exists before compiling and copy nombres.txt in it after compiling
# file(MAKE_DIRECTORY ${PROJECT_SOURCE_DIR}/bin)
# file(COPY ${PROJECT_SOURCE_DIR}/src/nombres.txt DESTINATION ${PROJECT_SOURCE_DIR}/bin/)
#################
# AT EACH BUILD #
#################
# Create the dir bin if it doesn't exists before compiling and copy nombres.txt in it after compiling
add_custom_command(TARGET Tp1_Sebastien_Pierre PRE_BUILD COMMAND ${CMAKE_COMMAND} -E make_directory ${PROJECT_SOURCE_DIR}/bin)
add_custom_command(TARGET Tp1_Sebastien_Pierre POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy ${PROJECT_SOURCE_DIR}/src/nombres.txt ${PROJECT_SOURCE_DIR}/bin/nombres.txt)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
