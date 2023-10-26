cmake_minimum_required(VERSION 3.16)
project(CISC372 C)
set(CMAKE_C_STANDARD 11)

add_executable(${PROJECT_NAME}
        "${CMAKE_CURRENT_SOURCE_DIR}/src/image.c"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/image.h"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/stb_image.h"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/stb_image_write.h")
target_link_libraries(${PROJECT_NAME} m)
