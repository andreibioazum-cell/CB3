cmake_minimum_required(VERSION 3.10)
project(CubicBattle3)

set(CMAKE_CXX_STANDARD 17)

find_package(SFML 2.6 REQUIRED COMPONENTS graphics window system)

add_executable(CubicBattle3
    main.cpp
    Lobby.cpp
    Game.cpp
    Enemy.cpp
    controls.cpp
)

target_link_libraries(CubicBattle3
    sfml-graphics
    sfml-window
    sfml-system
)
