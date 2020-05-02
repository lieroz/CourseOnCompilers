cmake_minimum_required(VERSION 3.13)
project(lab_03)

set(TARGET ${PROJECT_NAME}-bin)
add_definitions("-Wall -Werror -Wextra -std=c++17 -ggdb")
# add_definitions("-std=c++17 -ggdb")

include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup()

find_package(Boost REQUIRED)

add_executable(${TARGET} main.cc)
target_link_libraries(${TARGET} Boost::boost)
target_include_directories(${TARGET} PRIVATE ${Boost_INCLUDE_DIRS})
set_target_properties(${TARGET} PROPERTIES OUTPUT_NAME ${PROJECT_NAME})
