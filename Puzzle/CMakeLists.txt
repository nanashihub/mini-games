cmake_minimum_required(VERSION 3.14)
project(PuzzleGame LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)


set(CMAKE_PREFIX_PATH "/opt/homebrew/opt/qt/lib/cmake")

find_package(Qt6 COMPONENTS Widgets REQUIRED)

add_executable(PuzzleGame
    puzzle.cpp
    puzzlewindow.cpp
    puzzlewindow.h
)

target_link_libraries(PuzzleGame Qt6::Widgets)
