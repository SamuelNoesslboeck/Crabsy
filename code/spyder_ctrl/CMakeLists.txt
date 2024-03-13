cmake_minimum_required(VERSION 3.1)
project(spyder)

set(CMAKE_CXX_STANDARD 23)

find_package(PiPCA9685 REQUIRED)
add_executable(spyder-ctrl spyder.cpp)
target_link_libraries(spyder-ctrl ${PiPCA9685_LIBRARY})