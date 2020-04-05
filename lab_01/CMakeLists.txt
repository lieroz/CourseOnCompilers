cmake_minimum_required(VERSION 3.13)
project(lab_01)

set(TARGET ${PROJECT_NAME}-bin)
add_definitions("-std=c++17")

include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup()

find_package(Gtest REQUIRED)
find_package(GMock REQUIRED)

add_subdirectory(src)

add_executable(${TARGET} main.cc)
target_link_libraries(${TARGET} lab_01)
target_include_directories(${TARGET} PRIVATE src)
set_target_properties(${TARGET} PROPERTIES OUTPUT_NAME ${PROJECT_NAME})

enable_testing()
add_subdirectory(tst)
