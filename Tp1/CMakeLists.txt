cmake_minimum_required(VERSION 3.0.0)
project(Tp1_Sebastien_Pierre LANGUAGES C CXX VERSION 0.1.0)

include(CTest)
enable_testing()
# Verbose compiling output
set(CMAKE_VERBOSE_MAKEFILE TRUE)

set(CMAKE_MODULE_PATH
    ${CMAKE_CURRENT_LIST_DIR}
    ${CMAKE_MODULE_PATH}
)
# Modules required
find_package(Threads REQUIRED)
#find_package(GMP REQUIRED)
find_package(GMPXX REQUIRED)

# Change path of executables
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin)
# Add our own library
add_library(get_prime
            src/get_prime.c
            src/get_prime.h
            )
#file(GLOB_RECURSE SOURCES src/*.c)
#add_library(get_prime STATIC ${SOURCES})

# Main program to be compiled
add_executable(Tp1_Sebastien_Pierre src/main.cpp)
# Libraries to link for the main program
target_link_libraries (Tp1_Sebastien_Pierre ${CMAKE_THREAD_LIBS_INIT} gmpxx gmp get_prime)
#target_compile_options(Tp1_Sebastien_Pierre PRIVATE -lgmpxx)
#target_compile_options(Tp1_Sebastien_Pierre PRIVATE -lgmp)
#target_compile_options(Tp1_Sebastien_Pierre PRIVATE -pthread )

# Ajoute la dépendance
# add_dependencies(Tp1_Sebastien_Pierre get_prime)
# Post and pre build intructions
# Create the dir bin if it doesn't exists before compiling and copy nombres.txt in it after compiling
### AT BUILD SYSTEM GENERATION ###
# file(MAKE_DIRECTORY ${PROJECT_SOURCE_DIR}/bin)
# file(COPY ${PROJECT_SOURCE_DIR}/src/nombres.txt DESTINATION ${PROJECT_SOURCE_DIR}/bin/)
### AT EACH BUILD ###
add_custom_command(TARGET Tp1_Sebastien_Pierre PRE_BUILD COMMAND ${CMAKE_COMMAND} -E make_directory ${PROJECT_SOURCE_DIR}/bin)
add_custom_command(TARGET Tp1_Sebastien_Pierre PRE_BUILD COMMAND ${CMAKE_COMMAND} -E copy ${PROJECT_SOURCE_DIR}/src/nombres.txt ${PROJECT_SOURCE_DIR}/bin/nombres.txt)

# Cmake done by vscode...
set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
