cmake_minimum_required(VERSION 3.0.0)
project(Tp3_Sebastien_Pierre LANGUAGES C CXX VERSION 0.1.0)

include(CTest)
enable_testing()
# Verbose compiling output
set(CMAKE_VERBOSE_MAKEFILE TRUE)

set(CMAKE_MODULE_PATH
    ${CMAKE_CURRENT_LIST_DIR}
    ${CMAKE_MODULE_PATH}
)



find_package(MPI REQUIRED)
include_directories(SYSTEM ${MPI_INCLUDE_PATH})
# Change path of executables
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin)
# set(CMAKE_CXX_COMPILE_FLAGS ${CMAKE_CXX_COMPILE_FLAGS} ${MPI_COMPILE_FLAGS})
# Add our own library
# add_library(Compute
#             src/Compute.cpp
#             src/Compute.hpp
#             )
# add_library(Types
#             src/Types.cpp
#             src/Types.hpp
#             )
 add_library(Matrix
            src/Matrix.cpp
            src/Matrix.hpp)
# ###################################
# SET(CMAKE_C_COMPILER mpicc)
# SET(CMAKE_CXX_COMPILER mpicxx)
# target_link_libraries(test mpi)
# ####################################
# Main programs to be compiled
add_executable(Tp3_Sebastien_Pierre_main src/main.cpp)


# Libraries to link for the main program
#target_link_libraries (Tp2_Sebastien_Pierre_main_for_maison gmpxx gmp Types Compute)
target_link_libraries(Tp3_Sebastien_Pierre_main ${MPI_CXX_LIBRARIES} ${MPI_CXX_LINK_FLAGS} Matrix)
target_compile_options(Tp3_Sebastien_Pierre_main PRIVATE ${MPI_CXX_COMPILE_FLAGS})
target_compile_options(Tp3_Sebastien_Pierre_main PRIVATE -O3)

add_custom_command(TARGET Tp3_Sebastien_Pierre_main PRE_BUILD COMMAND ${CMAKE_COMMAND} -E make_directory ${PROJECT_SOURCE_DIR}/bin)
#add_custom_command(TARGET Tp2_Sebastien_Pierre_main PRE_BUILD COMMAND ${CMAKE_COMMAND} -E copy_if_different ${PROJECT_SOURCE_DIR}/src/nombres.txt ${PROJECT_SOURCE_DIR}/bin/)
#add_custom_command(TARGET Tp1_Sebastien_Pierre_par POST_BUILD COMMAND ${CMAKE_COMMAND} -E remove_directory ${PROJECT_SOURCE_DIR}/build)



# Cmake done by vscode...
set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
