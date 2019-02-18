cmake_minimum_required(VERSION 3.0.0)
project(Tp2_Sebastien_Pierre LANGUAGES C CXX VERSION 0.1.0)

include(CTest)
enable_testing()
# Verbose compiling output
set(CMAKE_VERBOSE_MAKEFILE TRUE)

set(CMAKE_MODULE_PATH
    ${CMAKE_CURRENT_LIST_DIR}
    ${CMAKE_MODULE_PATH}
)
#find_package(GMP REQUIRED)
find_package(GMPXX REQUIRED)
# OpenMp
find_package(OpenMP REQUIRED)
if (OPENMP_FOUND)
    set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
    set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
    set (CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${OpenMP_EXE_LINKER_FLAGS}")
endif()

# Change path of executables
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin)
# Add our own library
add_library(Compute
            src/Compute.cpp
            src/Compute.hpp
            )
add_library(Types
            src/Types.cpp
            src/Types.hpp
            )

# Main programs to be compiled
add_executable(Tp2_Sebastien_Pierre_par src/main.cpp)
add_executable(Tp2_Sebastien_Pierre_par src/main_simple.cpp)

# Libraries to link for the main program
target_link_libraries (Tp2_Sebastien_Pierre_par gmpxx gmp Types Compute)
add_custom_command(TARGET Tp2_Sebastien_Pierre_par PRE_BUILD COMMAND ${CMAKE_COMMAND} -E make_directory ${PROJECT_SOURCE_DIR}/bin)
add_custom_command(TARGET Tp2_Sebastien_Pierre_par PRE_BUILD COMMAND ${CMAKE_COMMAND} -E copy_if_different ${PROJECT_SOURCE_DIR}/src/nombres.txt ${PROJECT_SOURCE_DIR}/bin/)
#add_custom_command(TARGET Tp1_Sebastien_Pierre_par POST_BUILD COMMAND ${CMAKE_COMMAND} -E remove_directory ${PROJECT_SOURCE_DIR}/build)

target_compile_options(Tp2_Sebastien_Pierre_par PRIVATE -O3)


# Cmake done by vscode...
set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)


            #file(GLOB_RECURSE SOURCES src/*.c)
            #add_library(get_prime STATIC ${SOURCES})
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
