cmake_minimum_required(VERSION 3.16)
project(CISC372 C)
set(CMAKE_C_STANDARD 11)

# Option to select between strings
macro(option_enum)
    cmake_parse_arguments(IN "" "NAME;DEFAULT;DESCRIPTION" "OPTIONS" ${ARGN})
    set(${IN_NAME}_OPTIONS ${IN_OPTIONS})
    set(${IN_NAME} ${IN_DEFAULT} CACHE STRING ${IN_DESCRIPTION})
    set_property(CACHE ${IN_NAME} PROPERTY STRINGS ${IN_NAME}_OPTIONS)
    if(NOT ${IN_NAME} IN_LIST ${IN_NAME}_OPTIONS)
        message(FATAL_ERROR "${IN_NAME} (currently ${${IN_NAME}}) must be one of the following values: ${${IN_NAME}_OPTIONS}")
    endif()
endmacro()

option_enum(
        NAME "${PROJECT_NAME}_CONVOLUTION_MODE"
        DESCRIPTION "The mode for the convolution algorithm."
        DEFAULT "SERIAL"
        OPTIONS "SERIAL" "PTHREADS" "OPENMP")

add_executable(${PROJECT_NAME}
        "${CMAKE_CURRENT_SOURCE_DIR}/src/image.c"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/image.h"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/stb_image.h"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/stb_image_write.h")
target_link_libraries(${PROJECT_NAME} PUBLIC m)

if(${PROJECT_NAME}_CONVOLUTION_MODE STREQUAL "SERIAL")
    target_compile_definitions(${PROJECT_NAME} PUBLIC CONVOLUTION_MODE_SERIAL)
elseif(${PROJECT_NAME}_CONVOLUTION_MODE STREQUAL "PTHREADS")
    set(THREADS_PREFER_PTHREAD_FLAG ON)
    find_package(Threads REQUIRED)
    target_link_libraries(${PROJECT_NAME} PUBLIC Threads::Threads)
    target_compile_definitions(${PROJECT_NAME} PUBLIC CONVOLUTION_MODE_PTHREADS)
elseif(${PROJECT_NAME}_CONVOLUTION_MODE STREQUAL "OPENMP")
    find_package(OpenMP REQUIRED)
    target_link_libraries(${PROJECT_NAME} PUBLIC OpenMP::OpenMP_C)
    target_compile_definitions(${PROJECT_NAME} PUBLIC CONVOLUTION_MODE_OPENMP)
endif()
