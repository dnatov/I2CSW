cmake_minimum_required(VERSION 3.19)
project(I2CSW C)

set(CMAKE_C_STANDARD 99)

add_library(${PROJECT_NAME} i2csw.c i2csw.h)

target_link_libraries(${PROJECT_NAME})

target_include_directories (${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})