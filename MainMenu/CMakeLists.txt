cmake_minimum_required(VERSION 3.5)
project(MainMenu)

set(CMAKE_AUTOMOC ON)
find_package(Qt6 COMPONENTS Widgets REQUIRED)

add_executable(mainmenu main.cpp mainmenu.cpp)
target_link_libraries(mainmenu Qt6::Widgets)