cmake_minimum_required(VERSION 3.16)
project(BattleshipGame)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 REQUIRED COMPONENTS Core Widgets)

qt6_standard_project_setup()

qt6_add_executable(BattleshipGame
    main.cpp
    battleshipgame.cpp
    battleshipgame.h
)

qt6_add_resources(BattleshipGame "resources"
    PREFIX "/"
    FILES
)

target_link_libraries(BattleshipGame Qt6::Core Qt6::Widgets)
`
