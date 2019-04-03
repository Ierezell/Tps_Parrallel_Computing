cmake_minimum_required(VERSION 3.0.0)
project(Tp4_Sebastien_Pierre LANGUAGES C CXX VERSION 0.1.0)

include(CTest)
enable_testing()
# Verbose compiling output
set(CMAKE_VERBOSE_MAKEFILE TRUE)

set(CMAKE_MODULE_PATH
    ${CMAKE_CURRENT_LIST_DIR}
    ${CMAKE_MODULE_PATH}
)

find_package(OpenACC REQUIRED)
include_directories(${OpenACC_INCLUDE_DIRS})
link_directories(${OpenACC_LIBRARY})

find_package(OpenCL REQUIRED)
include_directories(${OpenCL_INCLUDE_DIRS})
link_directories(${OpenCL_LIBRARY})
# Change path of executables
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin)
# Add our own library
add_library(Matrix
            src/Matrix.cpp
            src/Matrix.hpp
            )

# Main programs to be compiled
add_executable(Tp4_Sebastien_Pierre_main_opencl src/main_opencl.cpp)
add_executable(Tp4_Sebastien_Pierre_main_acc src/main_acc.cpp)


target_include_directories (Tp4_Sebastien_Pierre_main_opencl PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_include_directories (Tp4_Sebastien_Pierre_main_acc PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
# Libraries to link for the main program
# target_link_libraries(test_tgt OpenCL::OpenCL)
target_link_libraries(Tp4_Sebastien_Pierre_main_opencl ${OpenCL_LIBRARY} Matrix)
target_compile_options(Tp4_Sebastien_Pierre_main_opencl PRIVATE -O3)

target_link_libraries(Tp4_Sebastien_Pierre_main_acc ${OpenACC_LIBRARY} Matrix)
target_compile_options(Tp4_Sebastien_Pierre_main_acc PRIVATE -O3)

# target_compile_options(Tp4_Sebastien_Pierre_main_acc PRIVATE ${MPI_CXX_COMPILE_FLAGS})
# target_compile_options(Tp4_Sebastien_Pierre_main_opencl PRIVATE ${MPI_CXX_COMPILE_FLAGS})

add_custom_command(TARGET Tp4_Sebastien_Pierre_main_opencl PRE_BUILD COMMAND ${CMAKE_COMMAND} -E make_directory ${PROJECT_SOURCE_DIR}/bin)
#add_custom_command(TARGET Tp1_Sebastien_Pierre_par POST_BUILD COMMAND ${CMAKE_COMMAND} -E remove_directory ${PROJECT_SOURCE_DIR}/build)

# Cmake done by vscode...
set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
