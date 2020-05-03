cmake_minimum_required(VERSION 3.13)
project(lab_03)

set(TARGET ${PROJECT_NAME}-bin)
add_definitions("-Wall -Werror -Wextra -std=c++17 -ggdb")

include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup()

find_package(Boost REQUIRED)
find_package(Gtest REQUIRED)
find_package(GMock REQUIRED)

add_subdirectory(src)

add_executable(${TARGET} main.cc)
target_link_libraries(${TARGET} Boost::boost lab_03)
target_include_directories(${TARGET} PRIVATE ${Boost_INCLUDE_DIRS} src)
set_target_properties(${TARGET} PROPERTIES OUTPUT_NAME ${PROJECT_NAME})

# enable_testing()
# add_subdirectory(tst)
