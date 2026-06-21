cmake_minimum_required(VERSION 3.22)
project(CubicBattle)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Для Android - используем SFML через NDK
find_package(SFML CONFIG REQUIRED)

add_library(cubicbattle SHARED
    main.cpp
    lobby.cpp
    game.cpp
    enemy.cpp
)

target_include_directories(cubicbattle PRIVATE .)
target_link_libraries(cubicbattle
    sfml-graphics
    sfml-window
    sfml-system
    log
    android
)
