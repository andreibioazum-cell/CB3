cmake_minimum_required(VERSION 3.22)
project(CubicBattle)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(cubicbattle SHARED
    main.cpp
    lobby.cpp
    game.cpp
    enemy.cpp
)

target_include_directories(cubicbattle PRIVATE .)
target_link_libraries(cubicbattle
    log
    android
    EGL
    GLESv2
)
