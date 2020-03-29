cmake_minimum_required(VERSION 3.13)
project(lab_01)

set(TARGET ${PROJECT_NAME}-bin)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17")

add_subdirectory(src)

add_executable(${TARGET} main.cc)
target_link_libraries(${TARGET} lab_01)
target_include_directories(${TARGET} PRIVATE src)
set_target_properties(${TARGET} PROPERTIES OUTPUT_NAME ${PROJECT_NAME})
