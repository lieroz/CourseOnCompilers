cmake_minimum_required(VERSION 3.13)
project(CourseWork)

set(TARGET ${PROJECT_NAME}-bin)
add_definitions("-std=c++17")

include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup()

find_package(Boost 1.71.0 REQUIRED)
find_package(Gtest REQUIRED)

add_subdirectory(src)

add_executable(${TARGET} main.cc)
target_link_libraries(${TARGET}
    CourseWork
)
target_include_directories(${TARGET} PRIVATE src)
set_target_properties(${TARGET} PROPERTIES OUTPUT_NAME ${PROJECT_NAME})

enable_testing()
add_subdirectory(tst)
