cmake_minimum_required(VERSION 3.18)
project(BattleshipGame)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_OSX_DEPLOYMENT_TARGET "11.0")

find_package(Qt6 REQUIRED COMPONENTS Core Widgets)

# Включение автоматической обработки MOC
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

add_executable(BattleshipGame
    main.cpp
    battleshipgame.cpp
    battleshipgame.h
)

set_target_properties(BattleshipGame PROPERTIES
    MACOSX_BUNDLE TRUE
    MACOSX_BUNDLE_BUNDLE_NAME "Морской Бой"
    MACOSX_BUNDLE_GUI_IDENTIFIER "com.example.battleshipgame"
    MACOSX_BUNDLE_SHORT_VERSION_STRING "1.0"
    MACOSX_BUNDLE_LONG_VERSION_STRING "1.0.0"
)

target_link_libraries(BattleshipGame 
    Qt6::Core 
    Qt6::Widgets
)

