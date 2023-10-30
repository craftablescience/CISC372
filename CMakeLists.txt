cmake_minimum_required(VERSION 3.16)
project(CISC372 C)
set(CMAKE_C_STANDARD 11)

list(APPEND ${PROJECT_NAME}_HW3_SOURCES
        "${CMAKE_CURRENT_SOURCE_DIR}/src/image.c"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/image.h"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/stb_image.h"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/stb_image_write.h")

add_executable(${PROJECT_NAME}_HW3_SERIAL   ${${PROJECT_NAME}_HW3_SOURCES})
add_executable(${PROJECT_NAME}_HW3_PTHREADS ${${PROJECT_NAME}_HW3_SOURCES})
add_executable(${PROJECT_NAME}_HW3_OPENMP   ${${PROJECT_NAME}_HW3_SOURCES})

target_link_libraries(${PROJECT_NAME}_HW3_SERIAL   PUBLIC m)
target_link_libraries(${PROJECT_NAME}_HW3_PTHREADS PUBLIC m)
target_link_libraries(${PROJECT_NAME}_HW3_OPENMP   PUBLIC m)

# Serial
target_compile_definitions(${PROJECT_NAME}_HW3_SERIAL PUBLIC CONVOLUTION_MODE_SERIAL)

# PThreads
set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)
target_link_libraries(${PROJECT_NAME}_HW3_PTHREADS PUBLIC Threads::Threads)
target_compile_definitions(${PROJECT_NAME}_HW3_PTHREADS PUBLIC CONVOLUTION_MODE_PTHREADS)

# OpenMP
find_package(OpenMP REQUIRED)
target_link_libraries(${PROJECT_NAME}_HW3_OPENMP PUBLIC OpenMP::OpenMP_C)
target_compile_definitions(${PROJECT_NAME}_HW3_OPENMP PUBLIC CONVOLUTION_MODE_OPENMP)
