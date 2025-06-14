cmake_minimum_required(VERSION 3.14)
project(MyQtApp LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)


set(CMAKE_PREFIX_PATH "/opt/homebrew/opt/qt/lib/cmake")
# set(CMAKE_PREFIX_PATH "/usr/local/opt/qt/lib/cmake")

find_package(Qt6 COMPONENTS Widgets REQUIRED)

add_executable(MyQtApp
    tic-tac-toe.cpp
    mainwindow.cpp
    mainwindow.h
)

target_link_libraries(MyQtApp Qt6::Widgets)
