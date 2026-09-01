cmake_minimum_required(VERSION 3.19)

project(Qt6_IndustrialMonitorDemo LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)#强制使用C++17标准

find_package(Qt6 6.5 REQUIRED COMPONENTS Core Widgets SerialBus PrintSupport)#查找

qt_standard_project_setup()#Qt6 Cmake专用宏，自动开启MOC、UIC、RCC

add_subdirectory(lib/qcustomplot)
add_subdirectory(src)


