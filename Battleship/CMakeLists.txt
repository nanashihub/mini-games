cmake_minimum_required(VERSION 3.18)
project(BattleshipGame)


set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)


set(CMAKE_OSX_DEPLOYMENT_TARGET "10.15")


find_package(Qt6 REQUIRED COMPONENTS Core Widgets)


set(CMAKE_AUTOMOC ON)


add_executable(BattleshipGame
    main.cpp
    battleshipgame.cpp
    battleshipgame.h
)


set_target_properties(BattleshipGame PROPERTIES
    MACOSX_BUNDLE TRUE
    MACOSX_BUNDLE_INFO_PLIST ${CMAKE_CURRENT_SOURCE_DIR}/Info.plist
)


target_link_libraries(BattleshipGame 
    Qt6::Core 
    Qt6::Widgets
)
