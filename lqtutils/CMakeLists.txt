cmake_minimum_required(VERSION 3.5)

project(LQtUtilsTest LANGUAGES CXX)

find_package(Qt5Test REQUIRED)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
enable_testing()

include_directories(../)

add_executable(LQtUtilsTest
    tst_lqtutils.cpp
    )
add_test(NAME LQtUtilsTest COMMAND LQtUtilsTest)

target_link_libraries(LQtUtilsTest PRIVATE Qt5::Test)
