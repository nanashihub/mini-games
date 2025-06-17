cmake_minimum_required(VERSION 3.16)
project(battleship LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_AUTOMOC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

find_package(Qt6 REQUIRED COMPONENTS Widgets)

add_executable(battleshipgame main.cpp battleshipgame.cpp battleshipgame.h)
target_link_libraries(battleshipgame PRIVATE Qt6::Widgets)