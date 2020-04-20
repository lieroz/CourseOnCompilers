cmake_minimum_required(VERSION 3.13)
project(lab_02)

set(TARGET ${PROJECT_NAME}-bin)
add_definitions("-std=c++17")

add_executable(${TARGET} main.cc)
